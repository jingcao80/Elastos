#==========================================================================
# Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
#==========================================================================
$(SPECIAL_TARGET) : $(XDK_TARGETS)/AudioTrack.eco
	if [ -f $(XDK_TARGETS)/AudioTrack.eco ]; then \
	 $(CP) $(XDK_TARGETS)/AudioTrack.eco $(XDK_TARGETS)/Launcher; \
	fi
	touch $(SPECIAL_TARGET)
	if [ -d $(XDK_TARGETS)/Launcher/ui ]; then \
		$(RMDIR) $(XDK_TARGETS)/Launcher/ui; \
	fi
	$(CPDIR) $(MAKEDIR)/ui/tv $(XDK_TARGETS)/Launcher/ui 2>/dev/null
	@touch $(XDK_TARGETS)/Launcher/ui
