import requests
from bs4 import BeautifulSoup
import argparse
import sys
import time


def process_product(url):
    """
    Extract the product information from the url.
    The product information is stored in a dictionary.

    Args:
    url (str): The url of the product

    Returns:
    dict: The product information stored in a dictionary
    """

    response = requests.get(url)
    soup = BeautifulSoup(response.content, 'html.parser')
    
    # Extract product name and price
    name = soup.find('h1', class_='product-title').text
    price = soup.find('div', class_='price_wrapper').find('strong', class_='price').find('span').text.strip()

    # Find all product attributes in the table
    product_attributes = soup.find('div', class_='product-attributes').find('table').find_all('tr')

    # List of interesting attributes
    interesting_attributes = ['Manufacturer', 'Age Group', 'Hand', 'Flex', 'Stick Length', 'Blade Curve', 'Brand', 'Family', 'Composite Stick Brand']
    
    # Dictionary to store the extracted attributes
    extracted_attributes = {'Name' : name, 'Price' : price, 'Manufacturer': 'Unknown', 'Age Group': 'Unknown', 'Hand': 'Unknown', 'Flex': 'Unknown', 'Stick Length': 'Unknown', 'Blade Curve': 'Unknown', 'Brand': 'Unknown', 'Family': 'Unknown', 'Composite Stick Brand': 'Unknown'}

    for row in product_attributes:
        label = row.find('td', class_='attr-label').text.strip().replace(":", "").replace("\u200d", "")
        if label in interesting_attributes:
            value_elements = row.find_all('span', class_='value')
            if not value_elements:
                value = row.find('td', class_='attr-value').text.strip()
            else:
                value = ', '.join([val.text.strip() for val in value_elements]) 
            extracted_attributes[label] = value

    return extracted_attributes


def scrape_eishockey_onlineshop_products(n_of_products, urls):
    """
    Scrape the product information from the urls.
    The product information is printed in the following format:
    URL \t Name \t Price \t Manufacturer \t Age Group \t Hand \t Flex \t Stick Length \t Blade Curve \t Brand \t Family \t Composite Stick Brand
    Use counter to sleep for 5 seconds after every 15 products to avoid website blocking.

    Args:
    n_of_products (int): Number of products to scrape
    urls (list): List of urls to scrape
    """

    counter = 0
    for url in urls[:n_of_products]:
        output = process_product(url.strip())
        output['URL'] = url.strip()
        print( '\t'.join([output['URL']] + [output[key] for key in output if key != 'URL']))
        
        # if counter is 15, sleep for 5 seconds
        counter += 1
        if counter == 15:
            time.sleep(5)
            counter = 0
    
if __name__ == "__main__":
    """
    The script is reading urls from stdin and scraping the product information from the urls.
    The script is printing the product information in the following format:
    URL \t Name \t Price \t Manufacturer \t Age Group \t Hand \t Flex \t Stick Length \t Blade Curve \t Brand \t Family \t Composite Stick Brand

    """
    parser = argparse.ArgumentParser()
    parser.add_argument('--n_of_products', type=int, default=10, help='Number of products to scrape')
    args = parser.parse_args()

    # urls are inputed from stdin
    urls = []
    for line in sys.stdin:
        urls.append(line.strip())


    scrape_eishockey_onlineshop_products(n_of_products=args.n_of_products, urls=urls)