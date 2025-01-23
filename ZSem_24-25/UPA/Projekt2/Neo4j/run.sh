#!/bin/bash

source ./UPA2/bin/activate
python3 download.py
python3 neo4jdb.py
deactivate