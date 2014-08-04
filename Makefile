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
	#./build/debug/image_tiler image1.jpg > /dev/null
	#./build/debug/image_tiler --tile-index 10 image1.jpg > /dev/null
	#./build/debug/image_tiler --scale 50.5 image1.jpg > /dev/null
	#./build/debug/image_tiler --angle 10 image1.jpg > /dev/null
	# create html containing fullpage svg
	./build/debug/image_tiler -v -t 10 -s 30 -a 10 image2.jpg tmp.svg
	echo "<!DOCTYPE html>" > tmp.html
	echo "<html>" >> tmp.html
	echo "<style>" >> tmp.html
	echo "svg { padding: 0px; margin:0px;}" >> tmp.html
	echo "</style>" >> tmp.html
	echo "<body>" >> tmp.html
	cat tmp.svg >> tmp.html
	echo "</body>" >> tmp.html
	echo "</html>" >> tmp.html
	# create jpg
	./build/debug/image_tiler --jpeg -t 10 -s 30 -a 10 image2.jpg tmp.jpg
	google-chrome tmp.jpg tmp.html image2.jpg
