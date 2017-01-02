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

#ifndef __ELASTOS_DROID_TELEPHONY_CVOLTESERVICESTATE_H__
#define __ELASTOS_DROID_TELEPHONY_CVOLTESERVICESTATE_H__

#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_Telephony_CVoLteServiceState.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CVoLteServiceState)
    , public Object
    , public IVoLteServiceState
    , public IParcelable
{
public:
    CVoLteServiceState();

    virtual ~CVoLteServiceState();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 srvccState);

    CARAPI constructor(
        /* [in] */ IVoLteServiceState* s);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ValidateInput();

    CARAPI FillInNotifierBundle(
        /* [in] */ IBundle* m);

    CARAPI GetSrvccState(
        /* [out] */ Int32* state);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* res);

    CARAPI ToString(
        /* [out] */ String* str);

    static CARAPI NewFromBundle(
        /* [in] */ IBundle* m,
        /* [out] */ IVoLteServiceState** state);

protected:
    CARAPI_(void) CopyFrom(
        /* [in] */ IVoLteServiceState* s);

private:
    CARAPI_(void) Initialize();

    CARAPI_(void) SetFromNotifierBundle(
        /* [in] */ IBundle* m);

private:
    static const String TAG;
    static const Boolean DBG = FALSE;

    Int32 mSrvccState;
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CVOLTESERVICESTATE_H__
