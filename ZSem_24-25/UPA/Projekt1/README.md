# Tím xondru18

## Členovia tímu:
- **Tomáš Ondrušek** (xondru18)
- **Peter Ďurica** (xduric05)

## Názov e-shopu:
TOP ON ICE
https://eishockey-onlineshop.de/

## Význam stĺpcov v TSV súbore:
'URL', 'Name', 'Price', 'Manufacturer', 'Age Group', 'Hand', 'Flex', 'Stick Length', 'Blade Curve', 'Brand', 'Family', 'Composite Stick Brand'

**URL** - URL adresa produktu \
**Name** - Názov produktu \
**Price** - Cena produktu \
**Manufacturer** - Výrobca produktu \
**Age Group** - Veková skupina, pre ktorú je produkt určený (*kategorická premenná*) \
**Hand** - Ruka, pre ktorú je produkt určený (*kategorická premenná*) \
**Flex** - Tuhosť hokejky (*numerická premenná*) \
**Stick Length** - Dĺžka hokejky (*numerická premenná*) \
**Blade Curve** - Tvar čepele hokejky (*kategorická premenná*) \
**Brand** - Značka produktu \
**Family** - Rodina produktu \
**Composite Stick Brand** - Značka kompozitnej hokejky

## Testované v prostredí
**OS**: *Linux (Ubuntu)* \
**Prostredie**: *Python 3.10* a *Python 3.11*

**Eva**

**Merlin** (testované 15.10.2024)

## Poznámky

- V rámci projektu sme sa snažili o čo najjednoduchší a najprehľadnejší kód. Všetky položky, ktoré sa v nami vybranej stránke nachádzajú, sú nahradené kľúčovým slovom **Unknown**, ak sa v danom riadku nenachádzajú.

- Pri vývoji sme si všimli, že za obdobie vývoja nášho projektu sa zmenil počet produktov na stránke, ktorú sme skúmali. Je teda možné, že sa vám pri spustení nášho kódu zobrazí iný počet produktov, ako je uvedený v našom súbore *data.tsv*.

- V prípade, že by ste chceli získať všetky produkty, ktoré sa nachádzajú na stránke, ktorú sme skúmali, môžete zmeniť parameter volania skriptu product_scraper.py --n_of_products na hodnotu -1. Týmto spôsobom získate všetky produkty, ktoré sa nachádzajú na stránke. (Poznámka: defaultne je nastavená hodnota 10)

- Exekúcia scriptu **product_scraper.py** pri parametri *--n_of_products* 10 trvá približne 20 sekúnd.
Pri parametri *--n_of_products* -1 trvá približne 20 minút.

