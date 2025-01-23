import requests

url = "https://www.ndm.cz/cz/program/xml"
response = requests.get(url)
with open('dataset.xml', 'wb') as file:
    file.write(response.content)
