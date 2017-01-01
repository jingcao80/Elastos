#=========================================================================
# Copyright (C) 2012 The Elastos Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#=========================================================================

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

