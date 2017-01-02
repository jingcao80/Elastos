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

#ifndef __ELASTOS_DROID_INTERNAL_OS_ANDROIDPRINTSTREAM_H__
#define __ELASTOS_DROID_INTERNAL_OS_ANDROIDPRINTSTREAM_H__

#include "elastos/droid/internal/os/LoggingPrintStream.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

class AndroidPrintStream
    : public LoggingPrintStream
{
public:
    /**
     * Constructs a new logging print stream.
     *
     * @param priority from {@link android.util.Log}
     * @param tag to log
     */
    AndroidPrintStream(
        /* [in] */ Int32 priority,
        /* [in] */ const String& tag);

protected:
    CARAPI_(void) Log(
        /* [in] */ const String& line);

private:
    const Int32 mPriority;
    const String mTag;
};

} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_OS_ANDROIDPRINTSTREAM_H__
