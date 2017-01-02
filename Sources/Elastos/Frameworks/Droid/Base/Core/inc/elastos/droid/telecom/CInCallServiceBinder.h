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

#ifndef __ELASTOS_DROID_TELECOM_CINCALLSERVICEBINDER_H__
#define __ELASTOS_DROID_TELECOM_CINCALLSERVICEBINDER_H__

#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_Telecom_CInCallServiceBinder.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Internal::Telecom::IIInCallAdapter;
using Elastos::Droid::Internal::Telecom::IIInCallService;
using Elastos::Droid::Os::IBinder;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Telecom {

/** Manages the binder calls so that the implementor does not need to deal with it. */
CarClass(CInCallServiceBinder)
    , public Object
    , public IIInCallService
    , public IBinder
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IHandler* handler);

    CARAPI SetInCallAdapter(
        /* [in] */ IIInCallAdapter* inCallAdapter);

    CARAPI AddCall(
        /* [in] */ IParcelableCall* call);

    CARAPI UpdateCall(
        /* [in] */ IParcelableCall* call);

    CARAPI SetPostDial(
        /* [in] */ const String& callId,
        /* [in] */ const String& remaining);

    CARAPI SetPostDialWait(
        /* [in] */ const String& callId,
        /* [in] */ const String& remaining);

    CARAPI OnAudioStateChanged(
        /* [in] */ IAudioState* audioState);

    CARAPI BringToForeground(
        /* [in] */ Boolean showDialpad);

    TO_STRING_IMPL("CInCallServiceBinder");
public:
    AutoPtr<IHandler> mHandler;
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_CINCALLSERVICEBINDER_H__
