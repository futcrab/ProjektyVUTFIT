Vizualizácia rozsiahlych grafických dát - README

Aplikacia bola testovana na Python 3.11.1. Je potrebne mat graficku kartu s podporou aspon OpenGL 4.3

Funkcnost:
Zobrazenie sceny zo vstupneho suboru pre Mitsuba renderer
Zobrazenie mracna bodov
Vykreslenie heatmapy na objektoch v scene podla point cloudu
Vizualizacia HSH funkcii

Install:
Predpoklada sa, ze Python3 a pip3 je nainstalovane
Prerekvizity sa instaluju pomocou pip prikazom: pip install -r requirements.txt

Linux: Ak budu problemy s tkinter tak este pouzit prikaz: sudo apt install python3-tk

Po tomto by mala byt aplikacia schopna fungovat na PC.

Vstupne subory pre aplikáciu:
Scena s objektami : XML súbor so scénou z Mitsuba renderer
Point Cloud : .dump špecifický súbor s mračnom bodov

Vstupne subory sa vyberaju po zapnuti aplikacie

Ovladanie:
Aplikacia sa ovlada pomocou GUI
Na ukoncenie aplikacie staci zavriet okno
V prípade scén ako cbox, ktora je v rozmeroch 0-300 je potrebne zmensit scale sceny, zvacsit velkost bodov a upravit padding pri nacitavani

Pohyb po scene:
WASD a myska
Pri pozerani je potrebna drzat prave tlacitko mysi

Usage:

python3 main.py [Nastavenie typu heatmapy]

heatmapa: 
-v alebo -vert alebo -vertex : Heatmapa sa pocita na vertexoch objektov (menej presne ale lepsi vykon)
-f alebo -frag alebo -fragment : Heatmapa sa pocita na fragmentoch objektov na obrazovke (viac presna ale slabsi vykon)
-h alebo -help : print usage
chyba v spustani programu : print usage
Parametre niesu case sensitive