SUBDIRS = src

.PHONY: subdirs

subdirs:
	$(MAKE) -C src

.PHONY: clean
clean:
	rm -f *~
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done

.PHONY: unmake
unmake: clean
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir unmake; \
	done
	rm -f *~ lib/*
