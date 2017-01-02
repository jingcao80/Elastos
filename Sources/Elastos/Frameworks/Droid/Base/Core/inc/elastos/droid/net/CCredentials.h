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

#ifndef __ELASTOS_DROID_NET_CCREDENTIALS_H__
#define __ELASTOS_DROID_NET_CCREDENTIALS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Net_CCredentials.h"
#include <elastos/core/Object.h>


namespace Elastos {
namespace Droid {
namespace Net {

/**
 * A class for representing UNIX credentials passed via ancillary data
 * on UNIX domain sockets. See "man 7 unix" on a desktop linux distro.
 */
CarClass(CCredentials)
    , public Object
    , public ICredentials
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 gid);

    CARAPI GetPid(
        /* [out] */ Int32* pid);

    CARAPI GetUid(
        /* [out] */ Int32* uid);

    CARAPI GetGid(
        /* [out] */ Int32* gid);

private:
    /** pid of process. root peers may lie. */
    Int32 mPid;
    /** uid of process. root peers may lie. */
    Int32 mUid;
    /** gid of process. root peers may lie. */
    Int32 mGid;
};

} // namespace Net
} // namepsace Droid
} // namespace Elastos
#endif //__ELASTOS_DROID_NET_CCREDENTIALS_H__
