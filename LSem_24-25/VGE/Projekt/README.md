# Simplification Visualizer
Python program slúžiaci na vizualizáciu určitých algoritmov slúžiacich na simplifikáciu povrchov objektov. Knižnice použité v projekte sú Polyscope na vizualizáciu, Open3D na prácu s objektami a scipy a numpy na interné kalkulácie s vrcholmi objektov. 

Dokumentácie týchto knižníc: \
[Polyscope](https://polyscope.run/py/) \
[Open3D](https://www.open3d.org/docs/release/index.html) \
[Scipy a Numpy](https://docs.scipy.org/doc/)

## Inštalácia
Program funguje na Windowse a Linuxe a na inštaláciu je možné použiť skripty nachádzajúce sa v tomto priečinku. Plná inštalácia je možná však len na Windowse a to kvôli tomu že sa mi nepodarilo nainštalovať Spectral Mesh Simplification (SMS) na Linuxe. Ak sa to však niekomu podarí nainštalovať a chce to mať funkčné na Linuxe tak by aplikácia mala byť pripravená na túto možnosť. Avšak bude potrebné zmeniť cestu k súboru pri spustení subprocesu.

### Windows Inštalácia

Plná inštalácia:
1. Mať nainštalované Python, CMake a MSBuild (Napríklad z Visual Studia 17 a viac) a vložené do premennej PATH aby sa mohli spustiť z command line. ([Tutoriál](https://stackoverflow.com/questions/44272416/how-to-add-a-folder-to-path-environment-variable-in-windows-10-with-screensho) na vloženie aplikácie do PATH)
2. Povolené spúštanie skriptov alebo aspoň dočasne obídenú politiku spúšťania pomocou príkazu: `Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass`
3. Spustiť príkaz: `./setup.bat full` (Inštalácia knižníc do virtuálneho prostredia sa môže zdať chvílu zaseknutá ale ono sa to po čase rozbehne)
4. Ak sa počas inštalácie niečo pokazí odporúčam odstrániť priečinky či už ručne alebo pomocou `./setup.bat delete_venv` alebo `./setup.bat delete_spectral` a spustiť inštaláciu znovu.
5. Aktivácia a deaktivácia virtuálneho prostredia sa spúšta príkazmi: `./VisualizerEnv/Scripts/activate` a `deactivate`
6. Po inštalácii sa program spúšta: `python ./SimplificationVisualizer.py`

Inštalácia bez SMS:
1. Mať nainštalovaný Python a vložený do premennej PATH aby sa mohol spustiť z command line. ([Tutoriál](https://stackoverflow.com/questions/44272416/how-to-add-a-folder-to-path-environment-variable-in-windows-10-with-screensho) na vloženie aplikácie do PATH)
2. Povolené spúštanie skriptov alebo aspoň dočasne obídenú politiku spúšťania pomocou príkazu: `Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass`
3. Spustiť príkaz: `./setup.bat only_venv` (Inštalácia knižníc do virtuálneho prostredia sa môže zdať chvílu zaseknutá ale ono sa to po čase rozbehne)
4. Ak sa počas inštalácie niečo pokazí odporúčam odstrániť priečinky či už ručne alebo pomocou `./setup.bat delete_venv` a spustiť inštaláciu znovu.
5. Aktivácia a deaktivácia virtuálneho prostredia sa spúšta príkazmi: `./VisualizerEnv/Scripts/activate` a `deactivate`
6. Po inštalácii sa program spúšta: `python ./SimplificationVisualizer.py`

### Linux Inštalácia
Linux verzia zatiaľ nepodporuje SMS.
1. Mať nainštalovaný Python
2. Ak operačný systém odmietne spustiť instalačný skript tak povoľte spúšťanie pomocou príkazu: `chmod +x setup.sh`
3. Spustiť príkaz: `./setup.sh create` (Inštalácia knižníc do virtuálneho prostredia sa môže zdať chvílu zaseknutá ale ono sa to po čase rozbehne)
4. Ak sa počas inštalácie niečo pokazí odporúčam odstrániť priečinky či už ručne alebo pomocou `./setup.sh delete` a spustiť inštaláciu znovu.
5. Aktivácia a deaktivácia virtuálneho prostredia sa spúšta príkazmi: `source ./VisualizerEnv/bin/activate` a `deactivate`
6. Po inštalácii sa program spúšta: `python3 ./SimplificationVisualizer.py`


## Ovládanie

Ovládanie kamery, polohy a zafarbenia objektov ako aj ostatné nastavenia okna sa nastavujú pomocou Polyscope GUI v ľavej časti obrazovky. (Kamera je pôvodne nastavená na WASD + myš a ľavé tlačítko)

V pravej časti obrazovky je už GUI určené priamo pre vizualizér. Dá sa zvoliť jeden z 3 demo objektov a to skull, monkey alebo bunny. 

Následné 2 input boxy sú pre quadric decimation a teda sa dá nastaviť počet trojuholníkov a veľkosť skoku pri zmene trojuholníkov.

Potom sú 2 input boxy pre nastavenie veľkosti voxelov vo vertex clustering. Nastavenie maximálnej veľkosti voxelu je na to aby sa dalo jemnejšie nastaviť veľkosť voxelu.

Ak je SMS nainštalované a je nastavený objekt bunny (Jediný demo objekt na ktorom to funguje) tak sa dá zvoliť:

1. Počet vrcholov vo zjednodušenom objekte
2. Počet vlastných vektorov (eigenvectors), ktoré sa zachovajú
3. Ak pri decimácii bude možné flipovať hrany. Táto možnosť je náročnejšia na výpočet ale výsledný zjednodušený objekt je presnejší ku pôvodnému.