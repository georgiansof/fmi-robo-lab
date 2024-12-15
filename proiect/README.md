    Introducere:
Ideea de proiect: o rama 'foto' digitala cu conexiune bluetooth la telefon, de la care preia un set de poze, vremea. Ceasul (RTC?) poate fi sincronizat prin internet (wi-fi).
Scopul: slideshow poze memorabile si afisare date utile.
Ideea: Am avut un ceas smart, mi s-a stricat si ii duc lipsa
Utilitate: ease of access la informatie cand se sta la birou.

    Descriere generala:
        Initializare: microcontroller-ul preia locatia prin bluetooth pentru a putea prelua automat timpul cu timezone inclus si vremea de afara. Va incarca prima poza din memorie si va incepe slideshow-ul.
        Butoane: 1x afisare date (vremea, ceasul complet, temperatura interioara, etc - pe un alt ecran pentru 5 secunde, dupa care revine la slideshow de unde a ramas)
                 1x next photo in slideshow
                 1x refresh (reia locatia pentru a actualiza datele - ajuta si la debug)
                 

    Schema bloc initiala:
![alt text](<schema bloc ddl1-1.png>)

    Hardware design:
        Schema electrică:
![schema electrica ddl2](https://github.com/user-attachments/assets/6f73d5ea-e7e8-4568-a52a-691a51dfbc05)

        Lista piese:
    - 1x Placuta ESP32 Wi-Fi, Bluetooth integrate
    - 1x LCD display HD (integrat, cu pini GPIO rezervati)
    - 3x Butoane
    - 1x senzor temperatura si presiune pentru interior
    - 1x RTC pentru a pastra timpul chiar daca placuta pierde alimentarea
    - rezistoare
    - fire de legătură

    Tabel GPIO:
    -------------------------------------------------------------------------------------------------------------
    | **Dispozitiv**            | **Pin**            | **GPIO ESP32** | **Explicații**                          |
    |---------------------------|--------------------|----------------|-----------------------------------------|
    |**Display (TFT, integrat)**| **MOSI (SDA)**     | **GPIO 19**    | SPI MOSI comun          |
    |                           | **SCLK (CLK)**     | **GPIO 18**    | SPI Clock comun          |
    |                           | **CS**             | **GPIO 5**     | Chip Select pentru display              |
    |                           | **DC**             | **GPIO 16**    | Pin Data/Command pentru display         |
    |                           | **RST**            | **GPIO 23**    | Reset pentru display                    |
    |                           | **BL**             | **GPIO 4**     | Control Backlight pentru display        |
    -------------------------------------------------------------------------------------------------------------
    | **Card SD**               | **MOSI**           | **GPIO 19**    | SPI MOSI comun                          |
    |                           | **MISO**           | **GPIO 12**    | MISO pentru cardul SD                   |
    |                           | **SCLK**           | **GPIO 18**    | SPI Clock comun                         |
    |                           | **CS**             | **GPIO 13**    | Chip Select pentru cardul SD            |
    -------------------------------------------------------------------------------------------------------------
    | **Senzor de temperatură** | **SDA**            | **GPIO 21**    | Linia de date I2C                       |
    |                           | **SCL**            | **GPIO 22**    | Linia de ceas I2C                       |
    -------------------------------------------------------------------------------------------------------------
    | **Modul RTC (3-wire)**    | **CLK**            | **GPIO 27**    | Pin ceas RTC                            |
    |                           | **DATA**           | **GPIO 26**    | Pin date RTC                            |
    |                           | **RST**            | **GPIO 25**    | Pin reset RTC                           |
    -------------------------------------------------------------------------------------------------------------

    Software design:

    Rezultate obtinute:
-

    Concluzii:
-
    Jurnal:
        - deadline 1 - ideea si o schema bloc preliminara \
        - deadline 2 - actualizare componente si descriere
-
    Bibliografie/Resurse:
    -   Wowki
