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


        Lista piese:
    - 1x Placuta ESP32 Wi-Fi, Bluetooth integrate
    - 1x LCD display HD
    - 3x Butoane
    - 1x senzor temperatura si presiune pentru interior
    - 1x RTC pentru a pastra timpul chiar daca placuta pierde alimentarea
    - rezistoare
    - fire de legătură

    Software design:

    Rezultate obtinute:
-

    Concluzii:
-
    Jurnal:
        - deadline 1 - ideea si o schema bloc preliminara
        - deadline 2 - actualizare componente si descriere
-
    Bibliografie/Resurse:
    -   Wowki
