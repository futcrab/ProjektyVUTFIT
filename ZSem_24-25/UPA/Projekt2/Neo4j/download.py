import requests

url_1 = "https://opendata.ostrava.cz/soubory/DatovyPortal/20240731_20241001_obyvatelstvo.csv"
url_2 = "https://opendata.ostrava.cz/soubory/DatovyPortal/20000101_20240101_zemreli.csv"

response_1 = requests.get(url_1)
response_2 = requests.get(url_2)

with open('obyvatelstvo.csv', 'wb') as file:
    file.write(response_1.content)

with open('zemreli.csv', 'wb') as file:
    file.write(response_2.content)