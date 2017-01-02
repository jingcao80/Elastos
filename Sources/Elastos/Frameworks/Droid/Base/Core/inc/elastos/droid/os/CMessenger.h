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

#ifndef __ELASTOS_DROID_OS_CMESSENGER_H__
#define __ELASTOS_DROID_OS_CMESSENGER_H__

#include "_Elastos_Droid_Os_CMessenger.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IIMessenger;

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CMessenger)
    , public Object
    , public IMessenger
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IHandler* handler);

    CARAPI constructor(
        /* [in] */ IIMessenger* target);

    CARAPI Send(
        /* [in] */ IMessage* message);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetBinder(
        /* [out] */ IBinder** mgr);

    CARAPI GetIMessenger(
        /* [out] */ IIMessenger** mgr);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashcode);

    CARAPI Equals(
        /* [in] */ IMessenger* other,
        /* [out] */ Boolean* equals);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* equals);

private:
    AutoPtr<IIMessenger> mTarget;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CMESSENGER_H__
