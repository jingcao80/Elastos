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

#ifndef __ELASTOS_DROID_SYSTEM_CZYGOTEHOOKS_H__
#define __ELASTOS_DROID_SYSTEM_CZYGOTEHOOKS_H__

#include "core/Object.h"
#include "_Elastos_Droid_System_CZygoteHooks.h"

using Elastos::Net::IInetAddress;

namespace Elastos {
namespace Droid {
namespace System {

/**
 * Corresponds to C's {@code struct group_req}.
 */
CarClass(CZygoteHooks)
    , public Object
    , public IZygoteHooks
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CZygoteHooks();

    CARAPI constructor();

    CARAPI PreFork();

    CARAPI PostForkChild(
        /* [in] */ Int32 debugFlags,
        /* [in] */ const String& instructionSet);

    CARAPI PostForkCommon();

private:
    static CARAPI_(Int64) NativePreFork();

    static CARAPI_(void) NativePostForkChild(
        /* [in] */ Int64 token,
        /* [in] */ Int32 debugFlags,
        /* [in] */ const String& instructionSet);

    /**
     * We must not fork until we're single-threaded again. Wait until /proc shows we're
     * down to just one thread.
     */
    static CARAPI_(void) WaitUntilAllThreadsStopped();

private:
    Int64 mToken;
};

} // namespace System
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEM_CZYGOTEHOOKS_H__
