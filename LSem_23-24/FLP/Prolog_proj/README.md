# FLP Logický projekt
Peter Ďurica\
xduric05\
2023/24\
Kostra grafu
## Metóda riešenia
1. Program prečíta a rozdelí vstup do listu pomocou predikátov z input2.pl\
napr. [[[A], [B]], [[A], [C]] ...]
2. Podľa prečítaného vstupu vytvorí reprezentáciu ciest pomocou dynamického predikátu path(A, B) pre oba smery ciest
3. Podľa prečítaného vstupu vytvorí list vrcholov v grafe
4. Skontroluje či sú vrcholy medzi sebou prepojené. Ak nie ukončuje program.
5. Podľa predikátu path/2 vytvorí list všetkých ciest v grafe\
napr. [[A, B], [A, C] ...]
6. Kostry grafu majú vždy rovnaký počet hrán ako je počet vrcholov v grafe mínus jedna. Moja metóda tento fakt využíva tak, že nájde všetky permutácie ciest v grafe o danej dĺžke a následne odfiltruje tie, ktoré nespájajú všetky vrcholy.\
Reprezentácia kostry grafu v programe je napr. [[A, B], [A, C], [C, D]]
7. Výsledné kostry vloží na výstup v žiadanom formáte
## Návod na použitie
1. Použiť priložený makefile príkazom: make
2. Spustiť program pomocou ./flp23-log \< in.txt \> out.txt

V archíve prikladám 4 testovacie vstupy jednoduchých grafov:
1. in1.txt - Graf o troch vrcholoch: A-B, A-C, B-C
2. in2.txt - Graf o štyroch vrcholoch: A-B, A-C, A-D, B-C, C-D
3. in3.txt - Zložitejší kruhový graf s viacerými vrcholmi od A až po I
4. in4.txt - Nespojený graf pre demonštráciu nesprávneho vstupu

## Obmedzenia
Kedže metóda funguje na hladaní a kontrolovaní permutácií, pri grafe s väčším počtom vrcholov sa kostry grafu počítajú dlho.