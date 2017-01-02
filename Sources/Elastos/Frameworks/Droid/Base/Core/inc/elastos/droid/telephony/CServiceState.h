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

#ifndef __ELASTOS_DROID_TELEPHONY_CSERVICESTATE_H__
#define __ELASTOS_DROID_TELEPHONY_CSERVICESTATE_H__

#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_Telephony_CServiceState.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CServiceState)
    , public Object
    , public IServiceState
    , public IParcelable
{
public:
    CServiceState();

    virtual ~CServiceState();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IServiceState* s);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetState(
        /* [out] */ Int32* state);

    CARAPI GetVoiceRegState(
        /* [out] */ Int32* state);

    CARAPI GetDataRegState(
        /* [out] */ Int32* state);

    CARAPI GetRoaming(
        /* [out] */ Boolean* roaming);

    CARAPI IsEmergencyOnly(
        /* [out] */ Boolean* res);

    CARAPI GetCdmaRoamingIndicator(
        /* [out] */ Int32* cdmaRoamingIndicator);

    CARAPI GetCdmaDefaultRoamingIndicator(
        /* [out] */ Int32* cdmaDefaultRoamingIndicator);

    CARAPI GetCdmaEriIconIndex(
        /* [out] */ Int32* cdmaEriIconIndex);

    CARAPI GetCdmaEriIconMode(
        /* [out] */ Int32* cdmaEriIconMode);

    CARAPI GetOperatorAlphaLong(
        /* [out] */ String* operatorAlphaLong);

    CARAPI GetOperatorAlphaShort(
        /* [out] */ String* operatorAlphaShort);

    CARAPI GetOperatorNumeric(
        /* [out] */ String* operatorNumeric);

    CARAPI GetIsManualSelection(
        /* [out] */ Boolean* isManualSelection);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* res);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI SetStateOutOfService();

    CARAPI SetStateOff();

    CARAPI SetState(
        /* [in] */ Int32 state);

    CARAPI SetVoiceRegState(
        /* [in] */ Int32 state);

    CARAPI SetDataRegState(
        /* [in] */ Int32 state);

    CARAPI SetRoaming(
        /* [in] */ Boolean roaming);

    CARAPI SetEmergencyOnly(
        /* [in] */ Boolean emergencyOnly);

    CARAPI SetCdmaRoamingIndicator(
        /* [in] */ Int32 roaming);

    CARAPI SetCdmaDefaultRoamingIndicator(
        /* [in] */ Int32 roaming);

    CARAPI SetCdmaEriIconIndex(
        /* [in] */ Int32 index);

    CARAPI SetCdmaEriIconMode(
        /* [in] */ Int32 mode);

    CARAPI SetOperatorName(
        /* [in] */ const String& longName,
        /* [in] */ const String& shortName,
        /* [in] */ const String& numeric);

    CARAPI SetOperatorAlphaLong(
        /* [in] */ const String& longName);

    CARAPI SetIsManualSelection(
        /* [in] */ Boolean isManual);

    CARAPI FillInNotifierBundle(
        /* [in] */ IBundle* m);

    CARAPI SetRilVoiceRadioTechnology(
        /* [in] */ Int32 state);

    CARAPI SetRilDataRadioTechnology(
        /* [in] */ Int32 state);

    CARAPI SetCssIndicator(
        /* [in] */ Int32 css);

    CARAPI SetSystemAndNetworkId(
        /* [in] */ Int32 systemId,
        /* [in] */ Int32 networkId);

    CARAPI GetRilVoiceRadioTechnology(
        /* [out] */ Int32* rilVoiceRadioTechnology);

    CARAPI GetRilDataRadioTechnology(
        /* [out] */ Int32* rilDataRadioTechnology);

    CARAPI GetRadioTechnology(
        /* [out] */ Int32* radioTechnology);

    CARAPI GetNetworkType(
        /* [out] */ Int32* type);

    CARAPI GetDataNetworkType(
        /* [out] */ Int32* type);

    CARAPI GetVoiceNetworkType(
        /* [out] */ Int32* type);

    CARAPI GetCssIndicator(
        /* [out] */ Int32* cssIndicator);

    CARAPI GetNetworkId(
        /* [out] */ Int32* networkId);

    CARAPI GetSystemId(
        /* [out] */ Int32* systemId);

    static CARAPI NewFromBundle(
        /* [in] */ IBundle* m,
        /* [out] */ IServiceState** res);

    static CARAPI RilRadioTechnologyToString(
        /* [in] */ Int32 rt,
        /* [out] */ String* str);

    static CARAPI IsGsm(
        /* [in] */ Int32 radioTechnology,
        /* [out] */ Boolean* result);

    static CARAPI IsCdma(
        /* [in] */ Int32  radioTechnology,
        /* [out] */ Boolean* result);

protected:
    CARAPI CopyFrom(
        /* [in] */ IServiceState* s);

private:
    CARAPI SetNullState(
        /* [in] */ Int32 state);

    static CARAPI_(Boolean) EqualsHandlesNulls(
        /* [in] */ const String& a,
        /* [in] */ const String& b);

    CARAPI SetFromNotifierBundle(
        /* [in] */ IBundle* m);

    CARAPI RilRadioTechnologyToNetworkType(
        /* [in] */ Int32 rt,
        /* [out] */ Int32* networkType);

private:
    static const String TAG;
    static const Boolean DBG;

    Int32 mVoiceRegState;
    Int32 mDataRegState;
    Boolean mRoaming;
    String mOperatorAlphaLong;
    String mOperatorAlphaShort;
    String mOperatorNumeric;
    Boolean mIsManualNetworkSelection;

    Boolean mIsEmergencyOnly;

    Int32 mRilVoiceRadioTechnology;
    Int32 mRilDataRadioTechnology;

    Boolean mCssIndicator;
    Int32 mNetworkId;
    Int32 mSystemId;
    Int32 mCdmaRoamingIndicator;
    Int32 mCdmaDefaultRoamingIndicator;
    Int32 mCdmaEriIconIndex;
    Int32 mCdmaEriIconMode;
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CSERVICESTATE_H__
