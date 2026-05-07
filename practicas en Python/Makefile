run: setup
	py src/main.py

freeze:
	pip freeze > requirements.txt

install:
	pip install -r requirements.txt

setup:
	if not exist data mkdir data
	if not exist data\plaintext.txt type nul > data\plaintext.txt
	if not exist data\ciphertext.txt type nul > data\ciphertext.txt
 