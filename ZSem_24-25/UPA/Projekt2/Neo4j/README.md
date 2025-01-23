# Neo4j

### Plný názov dátových sád:
Počet obyvatel dle věkové skupiny

https://data.gov.cz/datov%C3%A1-sada?iri=https%3A%2F%2Fdata.gov.cz%2Fzdroj%2Fdatov%C3%A9-sady%2F00845451%2F73fa9c81e137a13729981636c03f6a52

Počet úmrtí

https://data.gov.cz/datov%C3%A1-sada?iri=https%3A%2F%2Fdata.gov.cz%2Fzdroj%2Fdatov%C3%A9-sady%2F00845451%2Fef4229b643c95b91453c5f6a8cc4c98b

### Popis prvého datasetu:
Dátová sada je uložená v súbore CSV a obsahuje dáta o počte obyvateľov Ostravy podľa mestských častí a vekovej skupiny. Sada obsahuje ešte informácie o dátume merania a kódu mestskej časti.

### Popis druhého datasetu:
Dátová sada je znova uložená v súbore CSV. Obsahuje dáta o počte úmrtí v jednotlivých mestských častiach v určitých rokoch. Následne sada obsahuje ešte kód mestskej časti a priemerný vek zomretých obyvateľov.

### Popis riešenia

Pre dané datasety som zvolil grafovú databázu Neo4j 5.24.2. Túto databázu som si zvolil pretože viem jednotlivé mestské časti reprezentovať ako uzly grafu a následné informácie ako vzťahy. Grafová databáza Neo4j sa použiva hlavne pre prepojené dáta, kde sa využívajú vzťahy na efektívne dotazovanie zložitých štruktúr. Využitie si nájde hlavne pri sociálnych sieťach, kde je výhodný pri dotazoch na priateľov alebo pri interakciách s príspevkami.

### Postup riešenia

V rámci riešenia bol vytvorený skript v Pythone (download.py), ktorý načíta dáta zo stránky datasetov a následne ich uloží do CSV, ktoré sú pomenované "obyvatelstvo.csv" a "zemreli.csv". Prerekvizitou bola inštalácia Neo4j (Ako kontajner v docker) a Python knižníc pre prácu s Neo4j, pandas datasetmi a pre web scraping vo virtuálnom prostredí, ktoré sa vytvorí a knižnice sa stiahnu pomocou skriptu "create_venv.sh". V skripte neo4jbd.py sa najprv načítajú stiahnuté CSV súbory do pandas datasetov. Potom sa vytvorí "driver" na komunikáciu s inštanciou Neo4j pomocou uri adresy databázy a prístupovými údajmi na danú databázu. Po úspešnom vytvorení driveru môžme už priamo príkazmi v jazyku Cypher komunikovať s databázou. Následne si viem vytvoriť uzly pre každú mestskú časť. Zo súboru "zemreli.csv" si vytvorím uzly pre každý rok merania a zo súboru "obyvatelstvo.csv" si vytvorím uzly pre vekové skupiny. Potom spojím uzly s mestskou časťou a vekovou skupinou reláciou, ktorá obsahuje počet obyvateľov mestskej časti v danej vekovej skupine. Druhý typ relácie bude medzi mestskou časťou a rokom merania, kedy v nej bude uložený počet ľudí, ktorý v danej mestskej časti zomreli a ich priemerný vek.

Na konci skriptu je príklad dotazu nad dátami:

```
query_result = session.run(
    """
    MATCH (d:District)-[r:HAS_POPULATION]->(:AgeGroup)
    RETURN d.name AS district, SUM(r.population_count) AS total_population
    """
)

results = []
for record in query_result:
    results.append(dict(record))
    
for result in results:
    print(f"Mestska Cast: {result['district']}, Populacia: {result['total_population']}")
```

Tento dotaz spočíta celkovú populáciu pre každú mestskú časť tak že sčíta hodnoty pre každú vekovú skupinu. Neo4j najprv pomocou MATCH identifikuje všetky uzly distriktov, ktoré sú spojené s uzľami vekových skupín pomocou relácie HAS_POPULATION. Následne pomocou SUM sčíta všetky hodnoty populácie a potom pomocou RETURN vráti pre každú mestskú časť jej názov a sčítanú populáciu z relácií. Výsledné hodnoty sa vracajú ako pole štrukturovaných dát, ktoré sa v kóde upravia na slovníky a následne sa hodnoty v slovníku vytlačia na výstup programu.

Výstup:

```
Mestska Cast: Vítkovice, Populacia: 18533
Mestska Cast: Třebovice, Populacia: 3757
Mestska Cast: Svinov, Populacia: 8745
Mestska Cast: Stará Bělá, Populacia: 8492
Mestska Cast: Slezská Ostrava, Populacia: 44155
Mestska Cast: Radvanice a Bartovice, Populacia: 13291
Mestska Cast: Pustkovec, Populacia: 2700
Mestska Cast: Proskovice, Populacia: 2534
Mestska Cast: Poruba, Populacia: 122341
Mestska Cast: Polanka nad Odrou, Populacia: 10297
Mestska Cast: Plesná, Populacia: 3275
Mestska Cast: Petřkovice, Populacia: 6622
Mestska Cast: Ostrava-Jih, Populacia: 197120
Mestska Cast: Nová Ves, Populacia: 1445
Mestska Cast: Nová Bělá, Populacia: 4750
Mestska Cast: Moravská Ostrava a Přívoz, Populacia: 78085
Mestska Cast: Michálkovice, Populacia: 7017
Mestska Cast: Martinov, Populacia: 2496
Mestska Cast: Mariánské Hory a Hulváky, Populacia: 24021
Mestska Cast: Lhotka, Populacia: 2891
Mestska Cast: Krásné Pole, Populacia: 5607
Mestska Cast: Hrabová, Populacia: 8061
Mestska Cast: Hošťálkovice, Populacia: 3812
```

Pre zjednodušenie spustenia je pridaný aj bash skript "run.sh", ktorý by sa mal spúšťať po vytvorení virtuálneho python prostredia. Skript má za úlohu nastaviť virtuálne prostredie, následne spustí skripty na stiahnutie datasetov a nahranie datasetov do databázy aj s dodatočným dotazom. Po spustení skriptov deaktivuje virtuálne prostredie.

### Príklad dát zo súboru obyvatelstvo.csv

```
Datum,Obvod,Kód obvodu,Věková skupina,Počet
2024-08-31,Hošťálkovice,554332,10 až 20 let,210
2024-08-31,Hošťálkovice,554332,20 až 30 let,176
2024-08-31,Hošťálkovice,554332,30 až 40 let,266
2024-08-31,Hošťálkovice,554332,40 až 50 let,283
2024-08-31,Hošťálkovice,554332,50 až 60 let,271
2024-08-31,Hošťálkovice,554332,60 až 70 let,236
2024-08-31,Hošťálkovice,554332,70 a více let,240
2024-08-31,Hošťálkovice,554332,méně než 10 let,224
```

### Príklad dát zo súboru zemreli.csv

```
Rok,Obvod,Kód obvodu,Počet úmrtí,Průměrný věk
2023,Michálkovice,554430,32,72.9
2023,Poruba,546224,859,77.9
2023,Vítkovice,554227,105,73.6
2023,Hrabová,554669,36,76.4
2023,Nová Ves,554367,9,71.9
2023,Radvanice a Bartovice,554537,67,71.9
```