#!/bin/bash

# Run the program
source ./UPA/bin/activate
python3 url_scraper.py > url_test.txt
python3 product_scraper.py --n_of_products 10 < url_test.txt 
deactivate