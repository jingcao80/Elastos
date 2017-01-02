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

#ifndef __ELASTOS_DROID_SYSTEM_CSTRUCTUCRED_H__
#define __ELASTOS_DROID_SYSTEM_CSTRUCTUCRED_H__

#include "core/Object.h"
#include "_Elastos_Droid_System_CStructUcred.h"


namespace Elastos {
namespace Droid {
namespace System {
CarClass(CStructUcred)
    , public Object
    , public IStructUcred
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
    Int32 mPid;
    Int32 mUid;
    Int32 mGid;
};

}// namespace System
}// namespace Droid
}// namespace Elastos
#endif
