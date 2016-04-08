#==========================================================================
# Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
#==========================================================================
$(SPECIAL_TARGET) : $(XDK_TARGETS)/AudioTrack.eco
	if [ ! -d $(XDK_TARGETS)/Snowfox ]; then \
		$(MKDIR) $(XDK_TARGETS)/Snowfox; \
	fi
	if [ -f $(XDK_TARGETS)/AudioTrack.eco ]; then \
		$(CP) $(XDK_TARGETS)/AudioTrack.eco $(XDK_TARGETS)/Snowfox; \
	fi
	touch $(SPECIAL_TARGET)

