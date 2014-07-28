CCFILES=$(shell ls -1tr *.cc)
TARGETS=$(basename $(CCFILES))

default: run

waf:
	waf configure
	waf

run: waf
	@for i in $(TARGETS); do \
		echo ===== $$i =====; \
		./build/debug/$$i; \
	done
