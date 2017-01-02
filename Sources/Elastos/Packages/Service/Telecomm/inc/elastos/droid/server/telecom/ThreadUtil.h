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

#ifndef __ELASTOS_DROID_SERVER_TELECOM_THREADUTIL_H__
#define __ELASTOS_DROID_SERVER_TELECOM_THREADUTIL_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * Helper methods to deal with threading related tasks.
 */
class ThreadUtil
    : public Object
{
public:
    /** @return whether this method is being called from the main (UI) thread. */
    static CARAPI IsOnMainThread(
        /* [out] */ Boolean* result);

    /**
     * Checks that this is being executed on the main thread. If not, a message is logged at
     * WTF-level priority.
     */
    static CARAPI CheckOnMainThread();

    /**
     * Checks that this is not being executed on the main thread. If so, a message is logged at
     * WTF-level priority.
     */
    static CARAPI CheckNotOnMainThread();

private:
    ThreadUtil();

private:
    static const String TAG;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_THREADUTIL_H__
