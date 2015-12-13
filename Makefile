CCFILES=$(shell ls -1tr *.cc)
TARGETS=$(basename $(CCFILES))

default: waf
	./build/debug/image_tiler_interactive images/image2.jpg

waf:
	waf configure
	waf

check: waf
	$(MAKE) -C tests

run: waf
	./build/debug/image_tiler -h
	./build/debug/image_tiler -l
	./build/debug/image_tiler --list
	./build/debug/image_tiler --tile-index 10 --angle 45 --scale 50 images/image1.jpg tmp1.jpg
	display tmp1.jpg
	# create html containing fullpage svg
	./build/debug/image_tiler -v -t 10 -s 30 -a 10 images/image2.jpg tmp2.svg
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
	./build/debug/image_tiler --jpeg -t 10 -s 30 -a 10 images/image2.jpg tmp2.jpg
	firefox tmp2.html
	rm -f tmp1.jpg
	rm -f tmp2.html
	rm -f tmp2.jpg
	rm -f tmp2.svg

IN=images/a

montage1:
	./build/release/image_tiler_interactive $(IN).png
	mv out.png $(IN)1.png
	./build/release/image_tiler_interactive $(IN).png
	mv out.png $(IN)2.png
	./build/release/image_tiler_interactive $(IN).png
	mv out.png $(IN)3.png
	./build/release/image_tiler_interactive $(IN).png
	mv out.png $(IN)4.png
	./build/release/image_tiler_interactive $(IN).png
	mv out.png $(IN)5.png
	./build/release/image_tiler_interactive $(IN).png
	mv out.png $(IN)6.png

montage2:
	montage $(IN)?.png -geometry +1+1 $(IN)_montage.png
	display $(IN)_montage.png
