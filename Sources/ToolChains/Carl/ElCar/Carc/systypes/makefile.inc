
systypes.cls: $(MAKEDIR)/systypes.car
	$(V)echo Compiling $< ...
	$(V)carc -s -c $@ $<
