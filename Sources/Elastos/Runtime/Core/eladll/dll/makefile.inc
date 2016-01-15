#==========================================================================
# Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
#==========================================================================
$(SPECIAL_TARGET) : elastos.exp
ifeq "$(SystemRoot)" ""
	if [ -f $(XDK_USER_LIB)/Elastos.Runtime.CarRuntime.lib ]; then \
	 $(MV) $(XDK_USER_LIB)/Elastos.Runtime.CarRuntime.lib .; \
	fi
else
	if exist $(XDK_USER_LIB)/Elastos.Runtime.CarRuntime.lib \
		$(MV) $(XDK_USER_LIB)/Elastos.Runtime.CarRuntime.lib .
endif
#ifneq "$(XDK_TARGET_PLATFORM)" "win32"
#	@echo Copying \$$XDK_TARGETS/$(TARGET) to \$$XDK_TARGETS/$(TARGET_NAME).$(PLATFORM_BUILD).$(TARGET_TYPE)
#	$(CP) $(XDK_TARGETS)/$(TARGET) $(XDK_TARGETS)/$(TARGET_NAME).$(PLATFORM_BUILD).$(TARGET_TYPE)
#ifneq "$(DEBUG_INFO)" ""
#	$(CP) $(XDK_TARGETS)/dbg_info/$(TARGET) $(XDK_TARGETS)/dbg_info/$(TARGET_NAME).$(PLATFORM_BUILD).$(TARGET_TYPE)
#endif
#endif
	touch $(SPECIAL_TARGET)

