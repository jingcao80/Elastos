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

#ifndef __ELASTOS_DROID_OS_DEBUG_H__
#define __ELASTOS_DROID_OS_DEBUG_H__

#include "elastos/droid/ext/frameworkdef.h"

using Elastos::IO::IFileDescriptor;

namespace Elastos {
namespace Droid {
namespace Os {

class Debug
{
public:
    static CARAPI DumpHeap(
        /* [in] */ IFileDescriptor* fd);

    /**
     * Retrieves /proc/meminfo.  outSizes is filled with fields
     * as defined by MEMINFO_* offsets.
     * @hide
     */
    static CARAPI GetMemInfo(
        /* [out] */ ArrayOf<Int64>* outSizes);

    /**
     * Retrieves information about this processes memory usages. This information is broken down by
     * how much is in use by dalivk, the native heap, and everything else.
     */
    static CARAPI GetMemoryInfo(
        /* [out] */ IDebugMemoryInfo** memoryInfo);

    /**
     * Note: currently only works when the requested pid has the same UID
     * as the caller.
     * @hide
     */
    static CARAPI GetMemoryInfo(
        /* [in] */ Int32 pid,
        /* [out] */ IDebugMemoryInfo** memoryInfo);

    /**
     * Retrieves the PSS memory used by the process as given by the
     * smaps.
     */
    static CARAPI GetPss(
        /* [out] */ Int64* pss);

    /**
     * Retrieves the PSS memory used by the process as given by the
     * smaps. @hide
     */
    static CARAPI GetPss(
        /* [in] */ Int32 pid,
        /* [out, callee] */ ArrayOf<Int64>** outUss,
        /* [out] */ Int64* pss);

    /**
     * Have the stack traces of the given native process dumped to the
     * specified file.  Will be appended to the file.
     * @hide
     */
    static CARAPI DumpNativeBacktraceToFile(
        /* [in] */ Int32 pid,
        /* [in] */ const String& file);

    static CARAPI DumpBacktrace();

private:
    static CARAPI DoDumpHeap(
        /* [in] */ FILE* fp);
};

}
}
}

#endif // __ELASTOS_DROID_OS_DEBUG_H__
