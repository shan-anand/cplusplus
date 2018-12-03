SUBDIRS = common scsi

.PHONY: subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

.PHONY: clean
clean:
	for dir in $(SUBDIRS); do $(MAKE) -C $$dir clean; done

.PHONY: unmake
unmake:
	for dir in $(SUBDIRS); do $(MAKE) -C $$dir unmake; done
