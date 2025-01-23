#!/bin/bash

python3 -m venv UPA
source ./UPA/bin/activate
chmod +x run.sh
pip install bs4
pip install requests
pip install urllib3==1.26.20
deactivate
