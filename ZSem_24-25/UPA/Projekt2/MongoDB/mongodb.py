import json
from pymongo import MongoClient

# Connect to MongoDB
client = MongoClient("mongodb://localhost:27017/")
db = client["UPA"]
collection = db["uredni_deska"]

# Clear existing documents
if collection.count_documents({}) > 0:
    collection.drop()

# Load and insert documents from JSON file
with open('dataset.json', 'r') as file:
    data = json.load(file)  # Load JSON content

def insert_flattened(doc):
    flat_doc = {}
    
    for key, value in doc.items():
        if isinstance(value, dict):
            for sub_key, sub_value in value.items():
                flat_doc[f"{key}_{sub_key}"] = sub_value
        else:
            flat_doc[key] = value
    collection.insert_one(flat_doc)

for item in data["informace"]:
    insert_flattened(item)

print("Documents inserted successfully.")

# Query to verify the insertion
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




client.close()
