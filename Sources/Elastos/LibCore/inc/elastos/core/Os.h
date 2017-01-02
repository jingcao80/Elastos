//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_CORE_OS_H__
#define __ELASTOS_CORE_OS_H__

/*
 * Returns the thread priority for the current thread by querying the system.
 * This is useful when attaching a thread through JNI.
 *
 * Returns a value from 1 to 10 (compatible with java.lang.Thread values).
 */
int os_getThreadPriorityFromSystem();

#endif //__ELASTOS_CORE_OS_H__
