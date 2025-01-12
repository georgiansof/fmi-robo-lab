#include <WiFi.h>
#include <WebServer.h>
#include <TFT_eSPI.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include <Ds1302.h>
#include <ArduinoJson.h>
#include <Adafruit_BMP085.h>

//#define DEBUG 

// Define pins
#define TFT_BL    4   // Backlight Pin
#define SD_MISO  27   // GPIO27
#define SD_MOSI  26   // GPIO26
#define SD_SCK   25   // GPIO25
#define SD_CS    33   // GPIO33

TFT_eSPI tft = TFT_eSPI();
SPIClass sd_spi = SPIClass(HSPI);

#define RTC_CLK 12
#define RTC_DAT 13
#define RTC_RST 15

Ds1302 rtc(RTC_RST, RTC_CLK, RTC_DAT);

Adafruit_BMP085 bmp;

// Network credentials
const char* ssid = "ESP32-AP";
const char* password = "12345678";

// Web server
WebServer server(80);

// Image cycling variables
File root;                // Root directory
File currentFile;         // Current file being displayed
unsigned long lastImageChange = 0;
unsigned long lastInformationUpdate = 0;
const unsigned long imageInterval = 5000; // 5 seconds interval for image cycling
const unsigned long informationInterval = 1000;
bool atEndOfDirectory = false;
bool rewindDeletion = false;

// HTML page
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 Image Manager</title>
    <style>
        body {
            background-color: #f8f9fa;
            font-family: 'Arial', sans-serif;
            margin: 0;
            padding: 0;
        }

        .container {
            max-width: 800px;
            margin: 20px auto;
            padding: 20px;
            background-color: #fff;
            border-radius: 8px;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
        }

        h1, h3 {
            text-align: center;
            color: #343a40;
            padding-bottom: 100px;
        }

        .form-group {
            margin-bottom: 15px;
        }

        label {
            font-size: 1.1em;
            font-weight: bold;
        }

        .form-control {
            width: 100%;
            padding: 10px;
            margin-top: 5px;
            border: 1px solid #ccc;
            border-radius: 5px;
        }

        .btn {
            display: block;
            width: 100%;
            height: 50px;
            padding: 10px;
            margin-bottom: 10px;
            font-size: 1.1em;
            background-color: #007bff;
            color: white;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            transition: background-color 0.3s;
        }

        .btn:hover {
            background-color: #0056b3;
        }

        .btn-danger {
            background-color: #dc3545;
        }

        .btn-danger:hover {
            background-color: #c82333;
        }

        .btn-success {
            background-color: #28a745;
        }

        .btn-success:hover {
            background-color: #218838;
        }

        #imagePreview {
            display: block;
            margin: 20px auto;
            border-radius: 5px;
            max-width: 100%;
            max-height: 135px;
        }

        #downloadSection {
            text-align: center;
        }

        @media (max-width: 768px) {
            .container {
                padding: 10px;
            }

            .btn {
                font-size: 1em;
            }
        }

        @media (max-width: 480px) {
            .form-control, .btn {
                font-size: 0.9em;
                padding: 8px;
            }

            h1, h3 {
                font-size: 1.4em;
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>ESP32 Image Manager</h1>

        <form id="uploadForm" enctype="multipart/form-data" class="mb-4">
            <div class="form-group">
                <label for="fileInput">Select Image (PNG, JPG, JPEG)</label>
                <input type="file" class="form-control" id="fileInput" name="image" accept=".png,.jpg,.jpeg">
            </div>
            <button id="uploadButton" type="button" class="btn">Upload Image</button>
        </form>

        <div class="mt-4">
            <img id="imagePreview" alt="Resized preview will appear here">
        </div>

        <div id="downloadSection" style="display: none;" class="mt-4">
            <button id="downloadButton" class="btn" style="background-color: #FFDF00">Download 135x135 RGB565 binary photo</button>
        </div>

        <div>
            <button id="removeButton" type="button" class="btn btn-danger">New Slideshow</button>
        </div>

        <div class="form-group mt-4">
            <button type="button" class="btn btn-success" onclick="syncClock()">Sync Date and Time</button>
        </div>
    </div>

    <script>
        let rgb565Blob = null;
        let currentExtension = ".bin135";  // Default extension, will be updated from ESP32

        // Fetch the current extension from ESP32
        function fetchCurrentExtension() {
            fetch('/currentExtension')
                .then(response => response.text())
                .then(extension => {
                    currentExtension = extension;
                })
                .catch(error => alert("Error fetching extension."));
        }

        // Fetch the current extension when the page loads
        fetchCurrentExtension();

        document.getElementById('fileInput').addEventListener('change', function(event) {
            const file = event.target.files[0];
            if (!file) {
                return;
            }

            const reader = new FileReader();

            reader.onload = function(e) {
                const img = new Image();

                img.onload = function() {
                    // Resize the image using canvas
                    const canvas = document.createElement('canvas');
                    const ctx = canvas.getContext('2d');
                    const size = 135; // Target size
                    canvas.width = size;
                    canvas.height = size;
                    ctx.drawImage(img, 0, 0, size, size);

                    // Display the resized image in the preview
                    const preview = document.getElementById('imagePreview');
                    preview.src = canvas.toDataURL(); // Show the resized image

                    // Convert image data to RGB565
                    const imageData = ctx.getImageData(0, 0, size, size);
                    const pixels = imageData.data;
                    const rgb565Data = [];
                    for (let i = 0; i < pixels.length; i += 4) {
                        const r = pixels[i];
                        const g = pixels[i + 1];
                        const b = pixels[i + 2];
                        const rgb565 = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
                        rgb565Data.push((rgb565 >> 8) & 0xFF); // High byte
                        rgb565Data.push(rgb565 & 0xFF);       // Low byte
                    }

                    // Create a Blob for download
                    rgb565Blob = new Blob([new Uint8Array(rgb565Data)], { type: 'application/octet-stream' });

                    // Show the download button
                    const downloadSection = document.getElementById('downloadSection');
                    downloadSection.style.display = 'block';

                    const downloadButton = document.getElementById('downloadButton');
                    downloadButton.onclick = function() {
                        if (rgb565Blob) {
                            const url = URL.createObjectURL(rgb565Blob);
                            const a = document.createElement('a');
                            a.href = url;
                            a.download = file.name.replace(/\.[^/.]+$/, "") + '.bin135'; // Default name, will be overridden later
                            a.click();
                            URL.revokeObjectURL(url); // Clean up the URL object
                        } else {
                            alert("No image data to download.");
                        }
                    };
                };

                img.src = e.target.result; // Load the image as a data URL
            };

            reader.readAsDataURL(file); // Read the file as a data URL
        });

        document.getElementById('uploadButton').addEventListener('click', function() {
            if (!rgb565Blob) {
                alert("Please select and process an image first.");
                return;
            }

            const formData = new FormData();
            const originalFileName = document.getElementById('fileInput').files[0].name; // Get original file name
            const fileNameWithoutExtension = originalFileName.replace(/\.[^/.]+$/, ""); // Remove old extension
            const newFileName = `${fileNameWithoutExtension}${currentExtension}`; // Use the dynamic extension from ESP32

            formData.append('image', rgb565Blob, newFileName);

            fetch('/upload', {
                method: 'POST',
                body: formData
            })
            .then(response => response.text())
            .then(text => alert(text))
            .catch(error => alert("Error uploading the image."));
        });

        document.getElementById('removeButton').addEventListener('click', function() {
            fetch('/remove', {
                method: 'GET'
            })
            .then(response => response.text())
            .then(text => {
                alert(text);
                // Fetch the new extension after the "remove" action to keep it updated
                fetchCurrentExtension();
            })
            .catch(error => alert("Error hiding images."));
        });

        function syncClock() {
          // Get the current date and time from JavaScript's Date object
          const currentDate = new Date();

          // Create an object with the current date and time
          const clockData = {
              year: currentDate.getFullYear(),
              month: currentDate.getMonth() + 1,  // Month is 0-indexed in JavaScript
              day: currentDate.getDate(),
              hour: currentDate.getHours(),
              minute: currentDate.getMinutes(),
              second: currentDate.getSeconds(),
              dow: currentDate.getDay() + 1
          };

          // Send a POST request to the /setClock endpoint to sync the RTC
          fetch('/setClock', {
              method: 'POST',
              headers: {
                  'Content-Type': 'application/json'
              },
              body: JSON.stringify(clockData)
          })
          .then(response => response.text())
          .then(data => {
              alert(data);  // Show the response message
          })
          .catch(error => {
              console.error('Error:', error);
              alert('Failed to sync clock.');
          });
      }
    </script>
</body>
</html>

)rawliteral";


// File to store the current extension number
#define EXTENSION_FILE "/current_extension.txt"

int currentExtensionNumber = 135;  
String currentExtension = ".bin135";  


// Function to read the extension number from the file
int readCurrentExtension() {
  File file = SD.open(EXTENSION_FILE, FILE_READ);
  if (file) {
    int extensionNumber = file.parseInt();
    file.close();
    return extensionNumber;
  } else {
    // Default to .bin135 if the file doesn't exist
    return 135;
  }
}

// Function to write the extension number to the file
void writeCurrentExtension(int extensionNumber) {
  File file = SD.open(EXTENSION_FILE, FILE_WRITE);
  if (file) {
    file.print(extensionNumber);
    file.close();
  } else {
    #ifdef DEBUG
      Serial.println("Failed to write extension number to file");
    #endif
  }
}


// Handle the root page
void handleRoot() {
  server.send_P(200, "text/html", index_html);
}

// Handle file uploads
void handleFileUpload() {
  HTTPUpload& upload = server.upload();
  
  if (upload.status == UPLOAD_FILE_START) {
    String filename = "/" + upload.filename;
    
    // Check if the file is a binary file
    if (!filename.endsWith(currentExtension)) {
      server.send(400, "text/plain", "Internal extension error. Try reloading the page to get latest data.");
      return;
    }

    // Open file for writing
    File file = SD.open(filename, FILE_WRITE);
    if (!file) {
      server.send(500, "text/plain", "Failed to open file for writing.");
      return;
    }
    file.close();

  } else if (upload.status == UPLOAD_FILE_WRITE) {
    File file = SD.open("/" + upload.filename, FILE_APPEND);
    if (file) {
      file.write(upload.buf, upload.currentSize);
      file.close();
    } else {
      server.send(500, "text/plain", "Failed to open file for writing.");
      return;
    }

  } else if (upload.status == UPLOAD_FILE_END) {
    server.send(200, "text/plain", "File successfully uploaded!");
  }
}

void listFiles(File dir) {
  while (true) {
    File entry = dir.openNextFile();
    if (!entry) {
      break;
    }
    String filename = entry.name();
    #ifdef DEBUG
      Serial.print("File: ");
      Serial.print(entry.name());
      Serial.print("\tSize: ");
      Serial.println(entry.size());
    #endif
    entry.close();
  }
}

// Initialize SD card
void initializeSDCard() {
  sd_spi.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
  if (!SD.begin(SD_CS, sd_spi)) {
    #ifdef DEBUG
      Serial.println("Failed to initialize SD card.");
    #endif
    while (1);
  }
  root = SD.open("/");
  if (!root) {
    #ifdef DEBUG
      Serial.println("Failed to open root directory.");
    #endif
    return;
  }
  #ifdef DEBUG
    Serial.println("Files on SD card:");
  #endif
}

// Draw a raw RGB565 image
void drawRawImage(File file, int x, int y, int width, int height) {
  #ifdef DEBUG
    Serial.print("Drawing ");
    Serial.println(file.name());
  #endif
    uint16_t* imageBuffer = (uint16_t*)malloc(width * height * sizeof(uint16_t));
    if (!imageBuffer) {
      #ifdef DEBUG
        Serial.println("Failed to allocate memory for image buffer.");
      #endif
        return;
    }

    // Read RGB565 data into the buffer
    file.read((uint8_t*)imageBuffer, width * height * sizeof(uint16_t));
    
    // Draw the image to TFT screen
    tft.pushImage(x, y, width, height, imageBuffer);
    
    free(imageBuffer);
}

void handleRemoveImages() {
  // Increment extension number and update the extension string
  currentExtensionNumber++;
  currentExtension = ".bin" + String(currentExtensionNumber);

  // Write the new extension number to the SD card file
  writeCurrentExtension(currentExtensionNumber);

  tft.fillScreen(TFT_BLACK);

  server.send(200, "text/plain", "Photo gallery cleared !");
}

// Cycle through images in the SD card
void cycleImages() {
  if (millis() - lastImageChange < imageInterval) return;
  lastImageChange = millis();

  if (!currentFile || atEndOfDirectory) {
    root.rewindDirectory();
    currentFile = root.openNextFile();
    atEndOfDirectory = false;
  }

  while (currentFile) {
    String filename = currentFile.name();
    if (filename.endsWith(currentExtension)) { 
      #ifdef DEBUG
        Serial.print("Preparing to draw file ");
        Serial.println(filename);
      #endif
      drawRawImage(currentFile, 0, 0, 135, 135);  // Assuming image is 240x135
      currentFile.close();
      currentFile = root.openNextFile();
      return;
    } else {
      currentFile.close();
      currentFile = root.openNextFile();
    }
  }

  // If no more files are found, restart from the beginning
  atEndOfDirectory = true;
}

// Handle syncing the RTC
void handleSetClock() {
  if (server.method() == HTTP_POST) {
    // Parse the incoming JSON request body
    String json = server.arg("plain");
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, json);

    int year = doc["year"];
    year -= 1900;
    int month = doc["month"];
    int day = doc["day"];
    int hour = doc["hour"];
    int minute = doc["minute"];
    int second = doc["second"];
    int dow = doc["dow"];

    // Set the RTC with the received date and time
    Ds1302::DateTime newTime = {year, month, day, hour, minute, second, dow};
    rtc.setDateTime(&newTime);

    server.send(200, "text/plain", "RTC updated successfully.");
  } else {
    server.send(405, "text/plain", "Method Not Allowed");
  }
}

void setup() {
  #ifdef DEBUG
    Serial.begin(115200);
  #endif

  Wire.begin(); // SDA (GPIO21) and SCL (GPIO22)

  rtc.init();

  if(!bmp.begin()) {
    #ifdef DEBUG
      Serial.println("Error on initing temp sensor.");
    #endif
    while(1);
  }

  // Initialize SD card
  initializeSDCard();
  #ifdef DEBUG
    Serial.println("Passed SD card initialization !");
  #endif

  // Read the current extension number from the SD card file
  currentExtensionNumber = readCurrentExtension();
  currentExtension = ".bin" + String(currentExtensionNumber);

  // Initialize TFT display
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
  tft.init();
  tft.fillScreen(TFT_BLACK);

  // Start Access Point
  WiFi.softAP(ssid, password, 1, false, 2);

  // Define routes
  server.on("/", handleRoot);
  server.on("/upload", HTTP_POST, []() {}, handleFileUpload);
  server.on("/remove", HTTP_GET, handleRemoveImages);
  // Route to get current extension
  server.on("/currentExtension", HTTP_GET, []() {
    String extension = currentExtension; // Get the current extension from the global variable
    server.send(200, "text/plain", extension); // Send it as plain text
  });
  server.on("/setClock", HTTP_POST, handleSetClock);


  // Start server
  server.begin();
}

String getDayOfWeek(int day) {
  const char dow[][4] = {
    "Sun",
    "Mon",
    "Tue",
    "Wed",
    "Thu",
    "Fri",
    "Sat"
  };

  return String(dow[day - 1]);
}

String leadingZero(int number) {
  if(number > 9)
    return String(number);
  
  return "0" + String(number);
}

void updateInformation() {
  if (millis() - lastInformationUpdate < informationInterval) 
      return;
  lastInformationUpdate = millis();

  Ds1302::DateTime dt;
  float newTemperature = bmp.readTemperature();
  rtc.getDateTime(&dt);

  String hourStr = leadingZero(dt.hour);
  String minuteStr = leadingZero(dt.minute);
  String secondStr = leadingZero(dt.second);

  String timeStr = hourStr + ":" + minuteStr + ":" + secondStr;

  String dayStr = leadingZero(dt.day);
  String monthStr = leadingZero(dt.month);
  
  String dateStr = dayStr + "/" + monthStr + "/" + String(dt.year + 2000);
  
  tft.drawRect(0, 135, 135, 105, 0);

  // Starting position at row 136
  int startY = 145;

  // Set text color and size
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);  // Adjust text size for readability

  // Update the time
  tft.setCursor(10, startY + 60);  // Position for time
  tft.println(timeStr);
  
  // Update the date
  tft.setCursor(10, startY + 30);  // Position for date
  tft.println(dateStr);
  
  // Update the temperature
  tft.setCursor(10, startY);  // Position for temperature
  String newTemperatureString = int(newTemperature) + String(".") + (int(newTemperature * 10) % 10);

  tft.println(getDayOfWeek(dt.dow) + ", " + newTemperatureString + "C");

  tft.setCursor(0, 0);
}

void loop() {
  server.handleClient();
  cycleImages();
  updateInformation();
}
