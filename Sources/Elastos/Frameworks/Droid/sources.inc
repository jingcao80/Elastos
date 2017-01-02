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

C_DEFINES += -DHAVE_ANDROID_OS
C_DEFINES += -DHAVE_PTHREADS
C_DEFINES += -DHAVE_ENDIAN_H
C_DEFINES += -DHAVE_SYS_UIO_H
C_DEFINES += -DSK_BUILD_FOR_ANDROID
C_DEFINES += -DSK_RELEASE
C_DEFINES += -DHAVE_POSIX_CLOCKS
#C_DEFINES += -DHAVE_INOTIFY
C_DEFINES += -DHAVE_OOM_ADJ
C_DEFINES += -DHAVE_OFF64_T
C_DEFINES += -DHAVE_SELINUX
#C_DEFINES += -DTHROW_ON_BAD_ID
C_DEFINES += -DQCOM_HARDWARE
C_DEFINES += -DQCOM_BSP
C_DEFINES += -DUSE_OPENGL_RENDERER
C_DEFINES += -DGL_GLEXT_PROTOTYPES
C_DEFINES += -DEGL_EGLEXT_PROTOTYPES
C_DEFINES += -DEGL_TRACE
C_DEFINES += -DHAVE_SCHED_SETSCHEDULER