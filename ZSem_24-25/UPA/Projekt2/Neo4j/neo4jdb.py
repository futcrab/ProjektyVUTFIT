from neo4j import GraphDatabase
import pandas as pd

# Load data from CSV file to pandas DataFrame
obyvatelstvoData = pd.read_csv('obyvatelstvo.csv')
zemreliData = pd.read_csv('zemreli.csv')

# Neo4j connection parameters
uri = "bolt://localhost:7687"
username = "neo4j"
password = "upaPassword"

# Connect to Neo4j
driver = GraphDatabase.driver(uri, auth=(username, password))


with driver.session() as session:
    # Reset the database
    session.run("MATCH (n) DETACH DELETE n")

    # Create district nodes
    for _, row in obyvatelstvoData[['Obvod', 'Kód obvodu']].drop_duplicates().iterrows():
        session.run("MERGE (d:District {name: $name, code: $code})", name=row['Obvod'], code=row['Kód obvodu'])

    # Create age group nodes
    for _, row in obyvatelstvoData[['Věková skupina']].drop_duplicates().iterrows():
        session.run("MERGE (a:AgeGroup {name: $name})", name=row['Věková skupina'])

    # Create year nodes
    for _, row in zemreliData[['Rok']].drop_duplicates().iterrows():
        session.run("MERGE (y:Year {year: $year})", year=int(row['Rok']))
    
    # Create HAS_POPULATION relationship between district and age group
    for _, row in obyvatelstvoData.iterrows():
        session.run(
                """
                MATCH (d:District {name: $district, code: $code})
                MATCH (a:AgeGroup {name: $age_group})
                MERGE (d)-[:HAS_POPULATION {date: $date, population_count: $population_count}]->(a)
                """,
                district=row['Obvod'],
                code=row['Kód obvodu'],
                age_group=row['Věková skupina'],
                date=row['Datum'],
                population_count=int(row['Počet'])
            )
    # Create HAS_MORTALITY relationship
    for _, row in zemreliData.iterrows():
            session.run(
                """
                MATCH (d:District {name: $district, code: $code})
                MATCH (y:Year {year: $year})
                MERGE (d)-[:HAS_MORTALITY {deaths: $deaths, average_age: $average_age}]->(y)
                """,
                district=row['Obvod'],
                code=row['Kód obvodu'],
                year=int(row['Rok']),
                deaths=int(row['Počet úmrtí']),
                average_age=float(row['Průměrný věk'])
            )
    # Query to get total population for each district
    query_result = session.run(
        """
        MATCH (d:District)-[r:HAS_POPULATION]->(:AgeGroup)
        RETURN d.name AS district, SUM(r.population_count) AS total_population
        """
    )

    # Process the query results into a dictionaries
    results = []
    for record in query_result:
        results.append(dict(record))
    
    # Output the results
    for result in results:
        print(f"Mestska Cast: {result['district']}, Populacia: {result['total_population']}")


driver.close()