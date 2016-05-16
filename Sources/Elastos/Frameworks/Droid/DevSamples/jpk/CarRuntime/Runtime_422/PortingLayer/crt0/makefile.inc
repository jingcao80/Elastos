
$(SPECIAL_TARGET) : $(TARGET) $(OBJECTS)
	@echo copy dllentry.o to $XDK_USER_LIB ...
	cp -f dllentry.o $(XDK_USER_LIB)
#	@echo copy exeentry.o to $XDK_USER_LIB ...
#	cp -f exeentry.o $(XDK_USER_LIB)
	touch $(SPECIAL_TARGET)
