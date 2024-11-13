Coechipier: Boț George

Descriere task:
  Implementarea unui joc de apasat cat mai rapid butonul corespunzator. 

  S-au folosit doua placi arduino care comunica prin protocolul SPI, una fiind master, iar una slave.
  Placa master este responsabila de controlarea LCD-ului si a game flow-ului in general, precum si a timer-ului reprezentat printr-un servomotor.
  Placa slave este responsabila de inputuri si de visual feedback (leduri).

  Jocul este turn-based, scopul fiind sa apesi cat mai repede butonul corespunzator culorii care apare pe led-ul rgb. Exista trei tipuri de punctaje: sub 0.3s, sub 0.6s si sub 1s. \
  Placa master controleaza inceperea / sfarsitul / game state-ul jocului, transmitand in fiecare runda ce culoare trebuie sa apese player-ul al carui este randul. \
  Placa slave primeste prin SPI culoarea si o reda led-ului rgb corespunzator player-ului curent si asteapta input. Daca greseste, acesta primeste 0p si primeste feedback auditiv prin buzzer. Daca expira timpul de 1s, buzzerul nu porneste, dar tot 0p se contorizeaza pentru runda respectiva.

  La final placa master afiseaza scorurile fiecarui player si cine a castigat, revenind la meniul principal de unde se poate incepe un nou joc.

  Componente: \
  2 placi arduino \
  2 player x 3 LED (r, g, b) = 6 LEDuri colorate. 
  2 player x 1 LEDRGB programabil = 2x RGBLED \
  2 player x 3 butoane corespunzatoare culorilor r,g,b \
  1 x buton de start \
  1 x servomotor \
  1 x LCD \
  1 x Buzzer \
  Rezistente de 220Ohm si 2K, pentru LED-uri, pullup si corectie pentru LED-urile care nu sunt albastre.   
  Fire de legatura 

![Poza hardware](https://github.com/user-attachments/assets/483ab4aa-71c9-4b11-91a0-05559e79d509) \

https://github.com/user-attachments/assets/b8df75df-c79c-4f9b-a5c2-3d52f7dbabb2

