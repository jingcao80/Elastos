#==========================================================================
# Copyright (c) 2000-2009,  Elastos, Inc.  All Rights Reserved.
#==========================================================================

systypes.cls: $(MAKEDIR)/systypes.car
	$(V)echo Compiling $< ...
	$(V)carc -s -c $@ $<
