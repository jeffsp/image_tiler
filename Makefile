CCFILES=$(shell ls -1tr *.cc)
TARGETS=$(basename $(CCFILES))

default: run

waf:
	waf configure
	waf

check: waf
	$(MAKE) -C tests

run: check
