
sample.cls: $(MAKEDIR)\sample.car
	@echo Compiling $<...
	$(V)carc -e -c $@ $<
