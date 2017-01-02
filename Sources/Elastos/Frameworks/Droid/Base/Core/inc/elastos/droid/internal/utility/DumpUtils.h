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

#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_DUMPUTILS_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_DUMPUTILS_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Runnable.h>

using Elastos::Droid::Os::IHandler;
using Elastos::Core::Runnable;
using Elastos::IO::IPrintWriter;
using Elastos::IO::IStringWriter;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

class DumpUtils
{
private:
    class DumpRunnable : public Runnable
    {
    public:
        DumpRunnable(
            /* [in] */ IDump* dump,
            /* [in] */ IStringWriter* sw);

        CARAPI Run();

    private:
        AutoPtr<IDump> mDump;
        AutoPtr<IStringWriter> mSw;
    };

public:
    /**
     * Helper for dumping state owned by a handler thread.
     *
     * Because the caller might be holding an important lock that the handler is
     * trying to acquire, we use a short timeout to avoid deadlocks.  The process
     * is inelegant but this function is only used for debugging purposes.
     */
    static CARAPI DumpAsync(
        /* [in] */ IHandler* handler,
        /* [in] */ IDump* dump,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ Int64 timeout);

private:
    DumpUtils();
};

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_DUMPUTILS_H__
