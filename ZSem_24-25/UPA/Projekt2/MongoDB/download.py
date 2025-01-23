import requests

url = "https://www.ostrava.cz/cs/urad/uredni-deska/uredni-deska.json"
response = requests.get(url)
with open('dataset.json', 'wb') as file:
    file.write(response.content)
