# MongoDB

### Plný názov datasetu:
Úřední deska Ostrava

https://data.gov.cz/datov%C3%A1-sada?iri=https%3A%2F%2Fdata.gov.cz%2Fzdroj%2Fdatov%C3%A9-sady%2F00845451%2F1005745497


### Popis datasetu:
Dátová sada je uložená v súbore JSON, ktorý obsahuje informácie o úradnej deske mesta Ostrava. Dáta z datasetu majú zanorenú štruktúru, ktorá obsahuje informácie o jednotlivých záznamoch na úradnej deske. Dáta obsahujú taktiež rôzne štruktúry, ktoré sú závislé od typu záznamu, preto je takýto typ datasetu vhodný pre dokumentové databázy, ako je MongoDB.

### Popis riešenia:


Pre daný dataset som zvolil databázu MongoDB, ktorá je vhodná pre dokumentové dáta. Konkrétne sa jednalo o MongoDB 8.0.3. Táto databáza bola zvolená kvôli tomu, že dáta obsahujú zanorenú štruktúru a rôzne typy záznamov, ktoré sú závislé od typu záznamu. Či už sa jedná o polia, ktoré sú v datasete použité alebo slovníky, MongoDB umožňuje ukladanie takýchto dát bez nutnosti definovania schémy a následné dotazovanie sa s možnosťou filtrovaní nad týmito štruktúrami.

### Postup riešenia:

V rámci riešenia bol vytvorený skript v Pythone, ktorý načíta dáta zo stránky datasetu a následne ich uloží do JSON pomenovaného "dataset.json". Prerekvizitou bola inštalácia MongoDB (ako kontajner v Docker) a Python knižníc pre prácu s MongoDB a pre web scraping. (použitý venv, ktorý je vytváraný pomocou scriptu "create_venv.sh") Následne bolo vytvorené pripojenie k MongoDB a vytvorená databáza "UPA" a kolekcia "uredni_deska". Dáta boli následne načítané z JSON a uložené do MongoDB pomocou Python skriptu "mongodb.py". Na konci scriptu je príklad dotazu nad dátami:
    
```
interesting_documents = collection.find(
    {"kategorie": "Ochrana životního prostředí"},
    {"_id": 0, "typ": 1, "číslo_jednací": 1, "dokument.název.cs": 1, "dokument.url": 1}
)

for document in interesting_documents:
    types = ", ".join(document['typ'])
    string = f"Typ: {types}\n"
    string += f"Číslo jednací: {document['číslo_jednací']}\n"
    string += f"Název: {document['dokument'][0]['název']['cs']}\n"
    string += f"URL: {document['dokument'][0]['url']}\n"
    print(string)
```
V tomto príklade sa vyhľadajú dokumenty, ktoré patria do kategórie "Ochrana životního prostředí" a vypíšu sa ich typ, číslo jednací, názov a URL. (pridané formátovanie pre lepšiu čitateľnosť) a to následne:

### Spracovanie dotazov:
Databázový server vykoná plné skenovanie kolekcie (full collection scan), aby našiel všetky dokumenty s hodnotou kategorie: "Ochrana životního prostředí". Počas spracovania aplikuje projekciu, aby vrátil len požadované polia. Výsledky sú následne vrátené klientovi vo formáte JSON. Klient tieto výsledky následne môže iteratívne spracovať ďalej.

Výstup:
```
Typ: Digitální objekt, Informace na úřední desce
Číslo jednací: 1517/2024
Název: 1517.pdf
URL: https://www.ostrava.cz/cs/urad/uredni-deska/1517-2024/1517.pdf/download

Typ: Digitální objekt, Informace na úřední desce
Číslo jednací: 1516/2024
Název: 1516.pdf
URL: https://www.ostrava.cz/cs/urad/uredni-deska/1516-2024/1516.pdf/download
....
```
### Príklad dát zo súboru dataset.json:
```
"informace": [
        {
            "typ": [
                "Digitální objekt", 
                "Informace na úřední desce"
            ], 
            "ỉri": "https://www.ostrava.cz/cs/urad/uredni-deska/1525-2024/iri_data", 
            "url": "https://www.ostrava.cz/cs/urad/uredni-deska/1525-2024?json=1", 
            "název": {
                "cs": "Veřejná vyhláška - opatření obecné povahy - Příkaz o dopravním značení - \"Silnice II/478 - rekonstrukce mostu ev. č. 478-003 přes potok Polančice, Ostrava - Polanka - 2. prodloužení termínu.\""
            }, 
            "popis": {
                "cs": ""
            }, 
            "vyvěšení": {
                "typ": "Časový okamžik", 
                "datum": "2024-10-25T11:15:00+02:00"
            }, 
            "relevantní_do": {
                "typ": "Časový okamžik", 
                "datum": "2024-11-12T01:00:00+02:00"
            }, 
            "číslo_jednací": "1525/2024", 
            "dokument": [
                {
                    "typ": "Digitální objekt", 
                    "název": {
                        "cs": "1525.pdf"
                    }, 
                    "url": "https://www.ostrava.cz/cs/urad/uredni-deska/1525-2024/1525.pdf/download"
                }
            ], 
            "odkaz": [], 
            "kategorie": "Jiná oznámení"
        },
        ....
```