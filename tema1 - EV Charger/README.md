Scopul proiectului: Stație de încărcare vehicule electrice (simulare LED). \
Comportament: LED-ul RGB trece în roșu când stația este ocupată, iar primul LED albastru va începe să clipească, de trei ori. \
După ce LED-ul albastru a clipit de 3 ori, rămâne aprins și clipește următorul LED alabastru ș.a.m.d până când clipește ultimul, adică al 4-lea. \
La final, vor clipi toate 3 împreună, iar după 3 clipiri se vor stinge și stația va reveni la starea de disponibilă (LED-ul RGB devine verde).

Butoanele de start și stop folosesc software debouncing, iar cel de stop funcționează prin longpress.

Componente: Arduino UNO, 4x LED albastru, 1x LED RGB catod comun, 

Rezistente 100 ohm: 5x rezistență 100Ohm pentru fiecare LED albastru și pentru componenta Blue a LED-ului RGB, încă 2x rezistențe înseriate 100Ohm pentru fiecare din componentele Red și Green ale LED-ului RGB. Total: 9 rezistențe 100Ohm. \
Rezistente 1K: 1x pentru fiecare buton (total 2).
2x Buton 
Breadboard
Linii de legătură (fire).

Poze setup hardware:
![WhatsApp Image 2024-10-21 at 08 09 58_383030d9](https://github.com/user-attachments/assets/2c299f16-8917-4836-9fb9-2667bde28831)
![WhatsApp Image 2024-10-21 at 08 09 58_eb69819b](https://github.com/user-attachments/assets/00a03d3f-e1f7-41cf-8b42-bf055082b50d)

Funcționalitate:
[![Watch the video](https://img.youtube.com/vi/mcoNxDeWV3o/maxresdefault.jpg)](https://youtu.be/mcoNxDeWV3o)

Schema electrica: (realizată în TinkerCAD):
[Link aici sau pe poza](https://youtu.be/mcoNxDeWV3o)
![image](https://github.com/user-attachments/assets/038f27eb-0a4b-44f7-8214-baf15c77566b)

Limitări: - overflow rapid din cauza functiei millis, care returneaza la precizie de miime de secunda. In 32.7 de secunde millis deja returneaza o valoare care provoaca overflow variabilelor long (fara unsigned) si la 65.5 isi da rollover.
