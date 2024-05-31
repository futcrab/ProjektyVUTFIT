.PHONY: install update
INPUT_IMAGE = TestFiles/obr1.png
INPUT_VIDEO = TestFiles/shorterVideo.mp4
OUTPUT = outputGIF

install:
	pip install --upgrade Pillow opencv-python numpy
demo_blocks:
	python3 gifcreator.py -i -in $(INPUT_IMAGE) -out $(OUTPUT) -b 100 -w 300
demo_iter:
	python3 gifcreator.py -i -in $(INPUT_IMAGE) -out $(OUTPUT)
demo_video:
	python3 gifcreator.py -v -in $(INPUT_VIDEO) -out $(OUTPUT) -s 00:00:03 -dur 5
demo_dither:
	python3 gifcreator.py -v -in $(INPUT_VIDEO) -out $(OUTPUT) -s 00:00:03 -dur 5 -d
help:
	python3 gifcreator.py -h
