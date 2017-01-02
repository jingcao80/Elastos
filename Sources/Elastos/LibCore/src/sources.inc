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

CAR_FLAGS += -n -u
LUBE_FLAGS += -n -u

C_DEFINES += -DHAVE_ANDROID_PTHREAD_SETNAME_NP
C_DEFINES += -DHAVE_GETTID
C_DEFINES += -DHAVE_SELINUX
C_DEFINES += -DHAVE_ANDROID_OS
C_DEFINES += -DHAVE_PRCTL
C_DEFINES += -DHAVE_SYS_UIO_H
C_DEFINES += -DHAVE_OFF64_T
C_DEFINES += -fpermissive