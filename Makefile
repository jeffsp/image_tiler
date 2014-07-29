CCFILES=$(shell ls -1tr *.cc)
TARGETS=$(basename $(CCFILES))

default: run

waf:
	waf configure
	waf

check: waf
	$(MAKE) -C tests

run: waf
	./build/debug/image_tiler -h
	./build/debug/image_tiler -l
	./build/debug/image_tiler --list
	#./build/debug/image_tiler image.jpg > /dev/null
	#./build/debug/image_tiler --tile-index 10 image.jpg > /dev/null
	#./build/debug/image_tiler --scale 50.5 image.jpg > /dev/null
	#./build/debug/image_tiler --angle 10 image.jpg > /dev/null
	./build/debug/image_tiler -t 10 -s 300 -a 10 image.jpg tmp.jpg
	google-chrome tmp.jpg
