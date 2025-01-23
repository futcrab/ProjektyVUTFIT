from cassandra.cluster import Cluster
import xml.etree.ElementTree as ET
from uuid import uuid4
    
# Load root element from XML file
with open('dataset.xml', 'r', encoding='utf-8') as file:
    xml_string = file.read()
root = ET.fromstring(xml_string)

# Connect to Cassandra
cluster = Cluster(['127.0.0.1'], port=9042)
session = cluster.connect()

# Drop old keyspace and create new one
session.execute("DROP KEYSPACE IF EXISTS theaterprogram")
session.execute("CREATE KEYSPACE theaterprogram WITH replication = {'class': 'SimpleStrategy', 'replication_factor': '1'}")
session.set_keyspace("theaterprogram")

# Create tables
session.execute("""
                CREATE TABLE IF NOT EXISTS Plays (
                    id UUID PRIMARY KEY,
                    name text,
                    author text,
                    stage text,
                    division text,
                    date text,
                    start_time text,
                    end_time text,
                    description text,
                    url text
                )
                """)
session.execute("""
                CREATE TABLE IF NOT EXISTS Authors (
                    name text PRIMARY KEY,
                    plays list<UUID>
                )
                """)
session.execute("""
                CREATE TABLE IF NOT EXISTS Stages (
                    name text PRIMARY KEY,
                    plays list<UUID>
                )
                """)
session.execute("""
                CREATE TABLE IF NOT EXISTS Divisions (
                    name text PRIMARY KEY,
                    plays list<UUID>
                )
                """)

# Insert data into Cassandra
for item in root.findall('Item'):
    play_id = uuid4()
    name = item.findtext('name', '')
    author = item.findtext('author', '')
    stage = item.findtext('stage', '')
    division = item.findtext('division', '')
    date = item.findtext('date', '')
    start = item.findtext('start', '')
    end = item.findtext('end', '')
    description = item.findtext('description', '')
    url = item.findtext('url', '')

    # Insert data into Plays table
    session.execute("""
                    INSERT INTO Plays (id, name, author, stage, division, date, start_time, end_time, description, url)
                    VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s, %s)
                    """, (play_id, name, author, stage, division, date, start, end, description, url)
                    )
    if author:
        session.execute("UPDATE Authors SET plays = plays + [%s] WHERE name = %s", (play_id, author))
    if stage:
        session.execute("UPDATE Stages SET plays = plays + [%s] WHERE name = %s", (play_id, stage))
    if division:
        session.execute("UPDATE Divisions SET plays = plays + [%s] WHERE name = %s", (play_id, division))
    
# Purposefully didnt just used the name of the author in plays table, because it's not partition key
# Query to get all play_ids from author
author_name = "Giuseppe Verdi"
playsIds_by_author = session.execute("SELECT plays FROM Authors WHERE name = %s", (author_name,))
playsIds = playsIds_by_author.one().plays

if playsIds:
    PlayIds_string = ', '.join([str(play_id) for play_id in playsIds])

    # Query to get all plays by author from Plays table
    plays_by_author = session.execute(f"""
                            SELECT name, stage, division, date, start_time, end_time, url
                            FROM Plays
                            WHERE id IN ({PlayIds_string})
                        """)

# Print the results
print(f"Plays by {author_name}:")
for play in plays_by_author:
    print(f"Name: {play.name}, Stage: {play.stage}, Division: {play.division}, Date: {play.date}, Start Time: {play.start_time}, End Time: {play.end_time}, url: {play.url}")

cluster.shutdown()