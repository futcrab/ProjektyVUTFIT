import os
import pandas as pd
from influxdb_client import InfluxDBClient
from influxdb_client.client.write_api import SYNCHRONOUS

bucket = "traffic_dataset"
org = "admin"
token = os.getenv('INFLUXDB_TOKEN')
url = "http://localhost:8086"

# Connect to InfluxDB
client = InfluxDBClient(url=url, token=token, org=org)
write_api = client.write_api(write_options=SYNCHRONOUS)

# Load the CSV file into a DataFrame
df = pd.read_csv('dataset.csv')

lines = []
for index, row in df.iterrows():
    group = row['Skupina stanic'].replace(" ", "\\ ").replace(",", "\\,").replace("=", "\\=")  # Escape spaces, commas, equal signs
    station = row['Stanice'].replace(" ", "\\ ").replace(",", "\\,").replace("=", "\\=")
    object_class = row['Třída objektu'].replace(" ", "\\ ").replace(",", "\\,").replace("=", "\\=")
    count = row['Počet']
    time = int(pd.to_datetime(row['Datum']).timestamp() * 10**9)  # Convert to nanoseconds
    
    line = f'stations,group="{group}",station="{station}",class="{object_class}" count={count}i {time}'
    lines.append(line)

# Write the data to InfluxDB
write_api.write(bucket=bucket, org=org, record=lines)



query_api = client.query_api()
query = f"""
from(bucket: "{bucket}")
  |> range(start: 2023-01-01T00:00:00Z, stop: 2023-01-02T00:00:00Z)
  |> filter(fn: (r) => r._measurement == "stations")
  |> filter(fn: (r) => r._value > 10)
  |> group(columns: ["station"])

"""

tables = query_api.query(query, org=org)
for table in tables:
  for record in table.records:
    print(record)
