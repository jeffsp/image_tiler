CCFILES=$(shell ls -1tr *.cc)
TARGETS=$(basename $(CCFILES))

default: waf
	./build/debug/image_tiler_interactive image2.jpg

waf:
	waf configure
	waf

check: waf
	$(MAKE) -C tests

run: waf
	./build/debug/image_tiler -h
	./build/debug/image_tiler -l
	./build/debug/image_tiler --list
	./build/debug/image_tiler --tile-index 10 --angle 45 --scale 50 image1.jpg tmp1.jpg
	open tmp1.jpg
	# create html containing fullpage svg
	./build/debug/image_tiler -v -t 10 -s 30 -a 10 image2.jpg tmp2.svg
	echo "<!DOCTYPE html>" > tmp2.html
	echo "<html>" >> tmp2.html
	echo "<style>" >> tmp2.html
	echo "svg { padding: 0px; margin:0px;}" >> tmp2.html
	echo "</style>" >> tmp2.html
	echo "<body>" >> tmp2.html
	cat tmp2.svg >> tmp2.html
	echo "</body>" >> tmp2.html
	echo "</html>" >> tmp2.html
	# create jpg
	./build/debug/image_tiler --jpeg -t 10 -s 30 -a 10 image2.jpg tmp2.jpg
	open tmp2.html
