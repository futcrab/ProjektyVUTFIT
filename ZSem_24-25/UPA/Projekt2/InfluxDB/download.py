import requests

url = "https://opendata.ostrava.cz/soubory/MMOIT/doprava/Statistika_poctu_prujezdu_krizovatkami_2023.csv"
response = requests.get(url)
with open('dataset.csv', 'wb') as file:
    file.write(response.content)
