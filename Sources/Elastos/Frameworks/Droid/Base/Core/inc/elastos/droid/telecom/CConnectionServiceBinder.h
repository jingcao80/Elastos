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

#ifndef __ELASTOS_DROID_TELECOM_CCONNECTIONSERVICEBINDER_H__
#define __ELASTOS_DROID_TELECOM_CCONNECTIONSERVICEBINDER_H__

#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_Telecom_CConnectionServiceBinder.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Internal::Telecom::IIConnectionServiceAdapter;
using Elastos::Droid::Internal::Telecom::IIConnectionService;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Telecom {

CarClass(CConnectionServiceBinder)
    , public Object
    , public IIConnectionService
    , public IBinder
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IHandler* handler);

    CARAPI AddConnectionServiceAdapter(
        /* [in] */ IIConnectionServiceAdapter* adapter);

    CARAPI RemoveConnectionServiceAdapter(
        /* [in] */ IIConnectionServiceAdapter* adapter);

    CARAPI CreateConnection(
        /* [in] */ IPhoneAccountHandle* connectionManagerPhoneAccount,
        /* [in] */ const String& id,
        /* [in] */ IConnectionRequest* request,
        /* [in] */ Boolean isIncoming,
        /* [in] */ Boolean isUnknown);

    CARAPI Abort(
        /* [in] */ const String& callId);

    CARAPI AnswerVideo(
        /* [in] */ const String& callId,
        /* [in] */ Int32 videoState);

    CARAPI Answer(
        /* [in] */ const String& callId);

    CARAPI Reject(
        /* [in] */ const String& callId);

    CARAPI Disconnect(
        /* [in] */ const String& callId);

    CARAPI Hold(
        /* [in] */ const String& callId);

    CARAPI Unhold(
        /* [in] */ const String& callId);

    CARAPI OnAudioStateChanged(
        /* [in] */ const String& callId,
        /* [in] */ IAudioState* audioState);

    CARAPI PlayDtmfTone(
        /* [in] */ const String& callId,
        /* [in] */ Char32 digit);

    CARAPI StopDtmfTone(
        /* [in] */ const String& callId);

    CARAPI Conference(
        /* [in] */ const String& callId1,
        /* [in] */ const String& callId2);

    CARAPI SplitFromConference(
        /* [in] */ const String& callId);

    CARAPI MergeConference(
        /* [in] */ const String& callId);

    CARAPI SwapConference(
        /* [in] */ const String& callId);

    CARAPI OnPostDialContinue(
        /* [in] */ const String& callId,
        /* [in] */ Boolean proceed);

    CARAPI ToString(
        /* [out] */ String* str);

public:
    AutoPtr<IHandler> mHandler;
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_CCONNECTIONSERVICEBINDER_H__