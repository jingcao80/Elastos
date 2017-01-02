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

Before text the Wifi code, we can use the wpa_cli/wpa_supplicant to test the driver and hardware.

example:
wpa_supplicant -Dnl80211 -iwlan0 -c/data/misc/wifi/wpa_supplicant.conf -p/data/misc/wifi/sockets

wpa_cli -iwlan0 -p/data/misc/wifi/sockets
