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

#ifndef __ELASTOS_DROID_SYSTEM_CSTRUCTLINGER_H__
#define __ELASTOS_DROID_SYSTEM_CSTRUCTLINGER_H__

#include "core/Object.h"
#include "_Elastos_Droid_System_CStructLinger.h"


namespace Elastos {
namespace Droid {
namespace System {

/**
 * Corresponds to C's {@code struct linger} from
 * <a href="http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/sys_socket.h.html">&lt;sys/socket.h&gt;</a>
 */
CarClass(CStructLinger)
    , public Object
    , public IStructLinger
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetOnoff(
        /* [out] */ Int32* onoff);

    CARAPI GetLinger(
        /* [out] */ Int32* linger);

    CARAPI IsOn(
        /* [out] */ Boolean* isOn);

    CARAPI constructor(
        /* [in] */ Int32 onoff,
        /* [in] */ Int32 linger);

private:
    /** Whether or not linger is enabled. Non-zero is on. */
    Int32 mOnoff;

    /** Linger time in seconds. */
    Int32 mLinger;
};

} // namespace System
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEM_CSTRUCTLINGER_H__
