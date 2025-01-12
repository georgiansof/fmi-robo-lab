# Digital Photo Frame with Date and Indoor Temperature

## Introducere
- **Ideea de proiect**: O ramă 'foto' digitală, care afișează și data și temperatura interioară.
- **Scopul**: Slideshow cu poze memorabile și afișare date utile.
- **Ideea**: Am avut un ceas smart, mi s-a stricat și îi duc lipsa. Ideea inițială folosea Bluetooth și Wi-Fi, dar în timpul implementării au apărut probleme, așa că am reinventat puțin ideea.
- **Utilitate**: Ease of access, amintiri (poze încărcate).

---

## Descriere Generală
### Inițializare
Microcontroller-ul folosește:
- **Interfață serială** pentru mesaje de debug
- **I2C/TWI** pentru senzorul de temperatură
- **ThreeWI** pentru RTC
- **SPI** pentru cardul SD (intern, placa are un SPI separat pentru display)

Microcontroller-ul inițializează toate componentele și pornește o rețea Wi-Fi pentru a comunica cu un terminal (PC/telefon smart/orice alt dispozitiv cu capacități IP). Toată interfața cu MC-ul se face prin Access Point-ul deschis.

---

## Schema Bloc
![alt text](image.png)

---

## Hardware Design
### Schema Electrică
![alt text](image-1.png)

### Lista Piese
- 1x Plăcuță ESP32 Wi-Fi (Display integrat)
- 1x SD card module
- 1x Senzor temperatură
- 1x RTC (pentru a păstra timpul chiar dacă plăcuța pierde alimentarea)
- Fire de legătură
- 1x Card SD
- 1x Baterie CR2032

---

## Software Design
### Organizare Display
- **Partea de sus (135px x 135px)**: Afișează imaginile.
- **Partea de jos (105px x 135px)**: Afișează informațiile (data, ora, ziua săptămânii, temperatura interioară).

### Implementare
- Inițial, imaginile erau stocate pe placă și convertite în RGB565 direct la afișare, dar nici memoria, nici viteza microprocesorului nu fac față pentru o interacțiune consistentă.
- Microcontroller-ul funcționează ca un **server HTTP** și răspunde cererilor din rețeaua locală, pe IP-ul `192.168.4.1`, unde se află și interfața grafică.
- **Prelucrarea imaginilor** pentru RGB565 se face în front-end-ul serverului web. Placa primește direct ce trebuie să afișeze și stochează informațiile pe cardul SD pentru a le păstra după un reboot.
- **Sincronizarea timpului** se face prin HTTP și datele sunt stocate în modulul RTC, iar temperatura este măsurată periodic.

### Actualizare Ecran
- Cele două părți ale ecranului sunt independente:
  - Informațiile se actualizează la secundă.
  - Slideshow-ul avansează la fiecare 5 secunde.

---

## Rezultate Obținute
![alt text](image-2.png)
![alt text](<WhatsApp Image 2025-01-13 at 00.19.03_e5dcb533.jpg>)

---

## Probleme Întâmpinate
- Wi-Fi și Bluetooth folosesc prea multă memorie pentru a fi utilizate simultan.
- Display-urile cu interfață paralelă folosesc prea mulți pini.
- Incompatibilități între versiunea plăcii și librării prea noi.
- Stack overflow (cu și fără crash-uri, din cauza imaginilor prea mari).
- Contacte proaste.
- Probleme de lipire (soldering).

---

## Concluzii
- Un proiect mult mai greu decât m-am gândit inițial că va fi. La un moment dat, am fost tentat să renunț.
- Embedded consumă foarte mult timp și atenție.
- Dependențele librăriilor în embedded sunt mai rele decât în software development (nu credeam că se poate mai rău).

---

## Jurnal
- **Deadline 1**: Ideea și o schemă bloc preliminară.
- **Deadline 2**: Actualizare componente și descriere.
- **Deadline 3**: Memorie depășită. Ideea unui ecran mai mare, extern, a picat. Am renunțat la ideea de a conecta prin Bluetooth și am folosit Wi-Fi și HTTP (o schimbare foarte bună, privind retrospectiv).

---

## Bibliografie/Resurse
- [circuit-diagram.org](https://circuit-diagram.org)
- [docs.arduino.cc](https://docs.arduino.cc)
- [github.com/Xinyuan-LilyGO/TTGO-T-Display](https://github.com/Xinyuan-LilyGO/TTGO-T-Display) (GitHub-ul plăcii)
