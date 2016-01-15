#==========================================================================
# Copyright (c) 2000-2009,  Elastos, Inc.  All Rights Reserved.
#==========================================================================

sample.cls: $(MAKEDIR)\sample.car
	@echo Compiling $<...
	$(V)carc -e -c $@ $<
