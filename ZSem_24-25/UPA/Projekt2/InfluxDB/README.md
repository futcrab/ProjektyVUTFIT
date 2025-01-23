# InfluxDB

### Plný názov datasetu:
Statistika průjezdu vozidel ze sledovaných křižovatek v roce 2023

https://data.gov.cz/datov%C3%A1-sada?iri=https%3A%2F%2Fdata.gov.cz%2Fzdroj%2Fdatov%C3%A9-sady%2F00845451%2Fc77f4bc5da52f2b6f02474f598e7c1cc


### Popis datasetu:
Dátová sada je uložená v súbore CSV, ktorý obsahuje informácie o počte vozidiel, ktoré prešli cez sledované križovatky v roku 2023. Dáta majú nasledujúce atribúty: dátum, skupina staníc, stanica, trieda objektu a počet vozidiel.

### Popis riešenia:

Pre daný dataset som zvolil databázu InfluxDB, ktorá je vhodná pre časové rady. Konkrétne sa jednalo o InfluxDB v2.7.10. Táto databáza bola zvolená kvôli tomu, že dáta obsahujú časové údaje a InfluxDB je vhodná pre časové rady. Dáta sú štruktúrované a nemajú žiadne chýbajúce hodnoty, čím sú vhodné pre uloženie v InfluxDB. Následne podporuje aj dotazy nad časovými radami, čo je vhodné pre analýzu dát, s možnosťou agregácie a filtrovania dát. (napr. zobrazenie počtu vozidiel pre daný dátum, skupinu staníc, stanicu, triedu objektu, okienkové funkcie atď.)

### Postup riešenia:

V rámci riešenia bol vytvorený skript v Pythone, ktorý načíta dáta zo stránky datasetu a následne ich uloží do CSV pomenovaného "dataset.csv". Prerekvizitou bola inštalácia InfluxDB a Python knižníc pre prácu s InfluxDB a pre web scraping. (použitý venv, ktorý je vytváraný pomocou scriptu "create_venv.sh") Následne bolo vytvorené pripojenie k InfluxDB a vytvorená databáza (bucket) "traffic_dataset". Dáta boli následne načítané z CSV a uložené do InfluxDB pomocou Python skriptu "influxdb.py". Na konci scriptu je príklad dotazu nad dátami:
    
```
query = f"""
    from(bucket: "{bucket}")
    |> range(start: 2023-01-01T00:00:00Z, stop: 2023-01-02T00:00:00Z)
    |> filter(fn: (r) => r._measurement == "stations")
    |> filter(fn: (r) => r._value > 10)
    |> group(columns: ["station"])

    """
```

dotaz sa spracováva následovne:

### Spracovanie dotazov:

InfluxDB je optimalizovaná pre časové rady a využíva tzv. "time series index" na rýchle vyhľadávanie dát. Pri spracovaní dotazu InfluxDB najprv analyzuje, že sa má pracovať s meraním "stations" v časovom rozsahu od 2023-01-01T00:00:00Z do 2023-01-02T00:00:00Z. Následne vyhľadáva relevantné dáta a filtruje záznamy, kde _value je väčší ako 10. Tieto dáta sú potom zoskupené podľa stĺpca "station". Po spracovaní sa výsledky formátujú do požadovanej podoby a odosielajú späť klientovi. Celý proces je optimalizovaný na rýchle vyhľadávanie cez časové rady.

### Príklad dát zo súboru dataset.csv:
```
Datum,Skupina stanic,Stanice,Třída objektu,Počet
01.01.2023 0:00,Novinářská x 28. října,Novinářská x 28. října - od Fifejd,Ostatní,27
01.01.2023 0:00,Novinářská x 28. října,Novinářská x 28. října - od Vítkovic,Ostatní,6
01.01.2023 0:00,Novinářská x 28. října,Novinářská x 28. října - od Vítkovic,Velké,1
01.01.2023 0:00,Novinářská x 28. října,Novinářská x 28. října - od centra,Ostatní,1
01.01.2023 0:00,Novinářská x 28. října,Novinářská x 28. října - od Poruby,Ostatní,11
01.01.2023 0:00,Bohumínská x 28. října,Bohumínská x 28. října - výjezd z mostu M. Sýkory
```