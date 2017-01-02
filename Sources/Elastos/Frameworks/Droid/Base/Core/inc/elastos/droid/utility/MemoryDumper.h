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

#ifndef __ELASTOS_DROID_UTILITY_MEMORY_DUMPER_H__
#define __ELASTOS_DROID_UTILITY_MEMORY_DUMPER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Math.h>

namespace Elastos {
namespace Droid {
namespace Utility {

/*
 * ref: http://elastos.org/elorg_common/gitbook_elastos/Elastos_Developer_Doc/elastos/memleakcheck.html
 * adb shell
 * chmod 777 /data/debug
 * adb shell setprop libc.debug.malloc 1
 * adb shell stop
 * adb shell start
 */
class ECO_PUBLIC MemoryDumper
{
public:
    static CARAPI Dump();

    static CARAPI Dump(
        /* [in] */ const String& filename);

private:
    ECO_LOCAL MemoryDumper();
    ECO_LOCAL MemoryDumper(const MemoryDumper&);
};


} // namespace Utility
} // namepsace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_UTILITY_MEMORY_DUMPER_H__