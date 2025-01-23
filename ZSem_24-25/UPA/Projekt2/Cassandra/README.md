# Apache Cassandra
### Plný názov dátovej sady
Program Národního divadla moravskoslezského

https://opendata.ostrava.cz/program-narodniho-divadla-moravskoslezskeho/

### Popis datasetu

Dátová sada je uložená v súbore XML a obsahuje dáta o predstaveniach v Národnom divadle. Sada obsahuje informácie o názve predstavenia, dejisku, autorovi, typu hry, dátume a času hry ako aj krátky opis hry a webovú adresu predstavenia.

### Popis riešenia

Pre daný dataset som zvolil databázu Apache Cassandra 5.0.2. Zvolil som si ju preto, lebo umožnuje prácu s rozsiahlym množstvom dát, ku ktorým má rýchly prístup v určitých prípadoch, pretože používa LSM strom na ich ukladanie. Cassandra podporuje vďaka tomu len jednoduché typy dotazov a často je potrebné skladať tabuľky v databáze podľa konkrétneho dotazu, ktorý sa bude často nad databázou vykonávať.

### Postup riešenia

V rámci riešenia bol vytvorený skript v Pythone (download.py), ktorý načíta dáta zo stránky na ktorej je dataset uložený a následne ich uloží do XML súboru, ktorý sa nazýva "dataset.xml". Prerekvizitou bola inštalácia Cassandry (Ako kontajner v docker) a Python knižníc pre prácu s Cassandra databázou a web scraping vo virtuálnom prostredí, ktoré sa vytvorí a knižnice sa stiahnu pomocou skriptu "create_venv.sh". V skripte "cassandradb.py" sa najprv získa koreňový element z xml súboru. Potom sa pripojí na Cassandru pomocou Clusteru na localhostovi a vytvorí sa relácia na spravovanie databázy. Následne sa zresetuje keyspace a nastaví sa nové, čisté prostredie na tvorbu databázy. Vytvoria sa 4 tabuľky. Prvá je tabuľka "Plays", kde sa ukladá každá hodnota pre konkrétnu hru z xml súboru a ako primárny kľúč je použité id hry. Potom sú 3 tabuľky "Authors", "Stages" a "Divisions" ktoré každá obsahuje stĺpce s jedinečnými názvamy jednotlivých riadkov tabuľky ktoré sú použité ako primárny kľúč a zoznamom id hier, ktoré sa ich týkajú. Pri vkladaní údajov do tabuľky "Plays" sa pre každý prvok v xml súbore, ktorý označuje divadelnú hru, vytiahnu podúdaje a vygeneruje jedinečné id. Tieto údaje sú následne vložené do tabuľky ako nový riadok. Potom sa aktualizujú údaje v ostatných tabuľkách, kedy sa buď vytvorí nový riadok a uloží sa do neho id hry s názvom údaju v tabuľke alebo sa pridá id hry už do existujúceho riadku podľa mena autora/dejiska alebo typu hry.

Na konci skriptu je príklad dotazu nad dátami, v ktorom som sa snažil aby sa vyhladávali dáta len cez partíciové kľúče.

```
# Query to get all play_ids from author
author_name = "Giuseppe Verdi"
playsIds_by_author = session.execute("SELECT plays FROM Authors WHERE name = %s", (author_name,))
playsIds = playsIds_by_author.one().plays

if playsIds:
    PlayIds_string = ', '.join([str(play_id) for play_id in playsIds])

    # Query to get all plays by author from Plays table
    plays_by_author = session.execute(f"""
                    SELECT name, stage, division, date, start_time, end_time, url
                    FROM Plays
                    WHERE id IN ({PlayIds_string})
                """)

# Print the results
print(f"Plays by {author_name}:")
for play in plays_by_author:
    print(f"Name: {play.name}, Stage: {play.stage}, Division: {play.division}, Date: {play.date}, Start Time: {play.start_time}, End Time: {play.end_time}, url: {play.url}")
```

Tento dotaz sa skladá z dvoch SELECT príkazov, ktoré musia byť vykonané samostatne, kedže Cassandra nepodporuje vnorené dotazy. Prvý dotaz vytiahne zoznam id hier z tabuľky "Authors", podľa mena autora a druhý dotaz následne vytiahne informácie o hrách podľa id hier z prvého dotazu. Medzi dotazmi je potrebné upraviť dáta ktoré sme dostali do jedného stringu kde budú oddelené čiarkami pre zachovanie správnej syntaxe pre druhý dotaz. Na konci sa už len vytlačia dáta na výstup programu.

Pri prvom dotaze koordinačný uzoľ použije partíciový kľúč "name" na lokáciu relevantného uzľu, ktorý obsahuje id hier pre konkrétneho autora z tabuľky "Authors". Po získaní zoznamu id ho odošle koordinačný uzoľ nazad klientovi.

Pri druhom dotaze znova koordinačný uzoľ odošle žiadosti na uzle ktoré sú zodpovedné za jednotlivé partíciové kľúče z dotazu. Pri získaní dát ich koordinátor spojí a odošle nazad klientovi.

Výstup:
```
Plays by Giuseppe Verdi:
Name: NABUCCO, Stage: Divadlo Antonína Dvořáka, Division: Opera, Date: 2024-11-23, Start Time: 18:30:00, End Time: 21:10:00, url: www.ndm.cz/cz/opera/predstaveni/5802-nabucco/2024-11-23/76687/
Name: LA TRAVIATA, Stage: Divadlo Antonína Dvořáka, Division: Opera, Date: 2025-02-13, Start Time: 18:30:00, End Time: 21:30:00, url: www.ndm.cz/cz/opera/predstaveni/4659-la-traviata/2025-02-13/76871/
Name: LA TRAVIATA, Stage: Divadlo Antonína Dvořáka, Division: Opera, Date: 2025-02-27, Start Time: 18:30:00, End Time: 21:30:00, url: www.ndm.cz/cz/opera/predstaveni/4659-la-traviata/2025-02-27/76872/
Name: NABUCCO, Stage: Divadlo Antonína Dvořáka, Division: Opera, Date: 2025-02-15, Start Time: 18:30:00, End Time: 21:10:00, url: www.ndm.cz/cz/opera/predstaveni/5802-nabucco/2025-02-15/76873/
Name: LA TRAVIATA, Stage: Divadlo Antonína Dvořáka, Division: Opera, Date: 2025-03-19, Start Time: 18:30:00, End Time: 21:30:00, url: www.ndm.cz/cz/opera/predstaveni/4659-la-traviata/2025-03-19/76996/
Name: NABUCCO, Stage: Divadlo Antonína Dvořáka, Division: Opera, Date: 2024-11-05, Start Time: 18:30:00, End Time: 21:10:00, url: www.ndm.cz/cz/opera/predstaveni/5802-nabucco/2024-11-05/76686/
```

Pre zjednodušenie spustenia je pridaný aj bash skript "run.sh", ktorý by sa mal spúšťať po vytvorení virtuálneho python prostredia. Skript má za úlohu nastaviť virtuálne prostredie, následne spustí skripty na stiahnutie datasetu a nahranie datasetu do databázy aj s dodatočným dotazom. Po spustení skriptov deaktivuje virtuálne prostredie.

### Príklad dát z datasetu

```
<Item>
<id>77143</id>
<name>PRAVDA</name>
<author>Florian Zeller</author>
<stage>Divadlo „12“</stage>
<division>Činohra</division>
<date>2025-03-09</date>
<start>16:00:00</start>
<end>18:00:00</end>
<description>   Překlad Michal Zahálka Režie Vojtěch Štěpánek   Alice: Budu ti připadat naivní, ale někdy si říkám, jak by bylo všechno jednodušší, kdyby si všichni říkali pravdu... Michel: To by byla hrůza, Alice. Učiněná noční můra. Kdyby si lidi ze dne na den přestali lhát, nezbyl by na Zemi jedinej pár. Vlastně by to byl konec civilizace. Komedie Pravda vypráví zábavný příběh plný nečekaných zvratů o dvou manželských párech a jejich (ne)bezpečných vztazích. Autorem bryskní konverzační hry je jeden z nejoceňovanějších a nejhranějších současných francouzských dramatiků Florian Zeller. Komedie o dvou manželských párech a jejich nevěrách, ve které se lže jako o závod. Paul nedávno přišel o práci za velmi pohnutých okolností a zatím se mu nedaří najít novou. Při vzájemných tenisových duelech s nejlepším přítelem Michelem se mu už dlouho nedaří a stále prohrává. Michelovi leží smutný stav života jeho nejlepšího přítele na srdci. Tím spíš, že už půl roku tak trochu spí s jeho ženou. A tak na něj bere ohledy a milosrdně mu lže a nabádá k tomu i jeho ženu. Chce přece před zbytečnou bolestí ochránit své nejbližší, kamaráda i svou manželku. Proč ubližovat lidem, na kterých nám opravdu záleží? Michel zkrátka musí lhát, nic jiného mu v takové prekérní situaci nezbývá. Lež je v tomto ohledu projevem ohleduplnosti. Michel je přesvědčen, že je vždy o krok napřed a že ví víc než ostatní. A ani trochu není připravený na možnost, že by se karta mohla obrátit a události nabrat nečekaný spád. A tak se Michel, který je vyznavačem lhaní z ohleduplnosti, najednou musí vyrovnat s tím, že někdo třeba taky nemusel říct pravdu jemu samému... Premiéra 15. března 2024 v Divadle „12“ Délka představení: 1 hodina a 30 minut (představení se hraje bez přestávky) Mediální partneři:   Partneři:    </description>
<url>www.ndm.cz/cz/cinohra/predstaveni/6318-pravda/2025-03-09/77143/</url>
</Item>
```