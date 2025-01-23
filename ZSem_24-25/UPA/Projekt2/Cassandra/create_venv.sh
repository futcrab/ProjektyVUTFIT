#!/bin/bash

python3 -m venv UPA2
source ./UPA2/bin/activate
pip install requests
pip install cassandra-driver
pip install urllib3==1.26.20
