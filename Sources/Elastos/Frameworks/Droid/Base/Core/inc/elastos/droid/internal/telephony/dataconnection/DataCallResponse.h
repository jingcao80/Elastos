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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DATACALLRESPONSE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DATACALLRESPONSE_H__

#include "_Elastos.Droid.Internal.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Net::ILinkProperties;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

/**
 * This is RIL_Data_Call_Response_v5 from ril.h
 */
class DataCallResponse
    : public Object
    , public IDataCallResponse
{
public:
    class SetupResult
        : public Object
        , public IDataCallResponseSetupResult
    {
    public:
        CAR_INTERFACE_DECL()

        SetupResult();

        CARAPI ToString(
            /* [out] */ String* result);
    public:
        AutoPtr<IDcFailCause> mFailCause;
        EnumDataCallResponseSetupResult mEnumValue;
    };
public:
    CAR_INTERFACE_DECL()

    DataCallResponse();

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI SetLinkProperties(
        /* [in] */ ILinkProperties* linkProperties,
        /* [in] */ Boolean okToUseSystemPropertyDns,
        /* [out] */ IDataCallResponseSetupResult** result);

    CARAPI GetVersion(
        /* [out] */ Int32* result);

    CARAPI SetVersion(
        /* [in] */ Int32 version);

    CARAPI GetStatus(
        /* [out] */ Int32* result);

    CARAPI SetStatus(
        /* [in] */ Int32 status);

    CARAPI GetCid(
        /* [out] */ Int32* result);

    CARAPI SetCid(
        /* [in] */ Int32 cid);

    CARAPI GetActive(
        /* [out] */ Int32* result);

    CARAPI SetActive(
        /* [in] */ Int32 active);

    CARAPI GetType(
        /* [out] */ String* result);

    CARAPI SetType(
        /* [in] */ const String& type);

    CARAPI GetIfname(
        /* [out] */ String* result);

    CARAPI SetIfname(
        /* [in] */ const String& ifname);

    CARAPI GetAddresses(
        /* [out, callee] */ ArrayOf<String>** result);

    CARAPI SetAddresses(
        /* [in] */ ArrayOf<String>* addresses);

    CARAPI GetDnses(
        /* [out, callee] */ ArrayOf<String>** result);

    CARAPI SetDnses(
        /* [in] */ ArrayOf<String>* dnses);

    CARAPI GetGateways(
        /* [out, callee] */ ArrayOf<String>** result);

    CARAPI SetGateways(
        /* [in] */ ArrayOf<String>* gateways);

    CARAPI GetSuggestedRetryTime(
        /* [out] */ Int32* result);

    CARAPI SetSuggestedRetryTime(
        /* [in] */ Int32 suggestedRetryTime);

    CARAPI GetPcscf(
        /* [out, callee] */ ArrayOf<String>** result);

    CARAPI SetPcscf(
        /* [in] */ ArrayOf<String>* pcscf);

    CARAPI GetMtu(
        /* [out] */ Int32* result);

    CARAPI SetMtu(
        /* [in] */ Int32 mtu);

private:
    const Boolean DBG;

    const String LOG__TAG;

    Int32 mVersion;

    Int32 mStatus;

    Int32 mCid;

    Int32 mActive;

    String mType;

    String mIfname;

    AutoPtr<ArrayOf<String> > mAddresses;

    AutoPtr<ArrayOf<String> > mDnses;

    AutoPtr<ArrayOf<String> > mGateways;

    Int32 mSuggestedRetryTime;

    AutoPtr<ArrayOf<String> > mPcscf;

    Int32 mMtu;
};

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DATACALLRESPONSE_H__
