import requests
from bs4 import BeautifulSoup


def scrape_eishockey_onlineshop_urls():
    """
    Scrape the urls of the products from the eishockey-onlineshop.de/Composite-Sticks
    Uses the pagination to scrape all the urls, until there are no more pages.
    Prints the urls to the command line.
    """
    
    product_count = 0
    url = 'https://eishockey-onlineshop.de/Composite-Sticks'
    next_page = True
    
    while next_page:
        response = requests.get(url)
        if response.status_code == 200:
            soup = BeautifulSoup(response.content, 'html.parser')
            row_gallery = soup.find_all('div', class_='row gallery')
            products = row_gallery[0].find_all('div', class_='product-wrapper col-xs-6 col-sm-4 col-md-3 product-wrapper--hover-enabled')
            
            # get product urls
            for product in products:
                product_count += 1
                print(product.find('a', class_='image-wrapper')['href'])
               
        next_page = soup.find('li', class_='next')
        if next_page:
            url = next_page.find('a')['href']
        else:
            next_page = False


if __name__ == "__main__":
    scrape_eishockey_onlineshop_urls()
   