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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DATAPROFILE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DATAPROFILE_H__

#include "_Elastos.Droid.Internal.h"
#include <Elastos.Droid.Os.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

class DataProfile
    : public Object
    , public IDataProfile
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ Int32 profileId,
        /* [in] */ const String& apn,
        /* [in] */ const String& protocol,
        /* [in] */ Int32 authType,
        /* [in] */ const String& user,
        /* [in] */ const String& password,
        /* [in] */ Int32 type,
        /* [in] */ Int32 maxConnsTime,
        /* [in] */ Int32 maxConns,
        /* [in] */ Int32 waitTime,
        /* [in] */ Boolean enabled);

    CARAPI constructor(
        /* [in] */ IApnSetting* apn,
        /* [in] */ Boolean isRoaming);

    static CARAPI ToParcel(
        /* [in] */ IParcel* pc,
        /* [in] */ ArrayOf<IDataProfile*>* dps,
        /* [out] */ IParcel** result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

    //id of the data profile
    CARAPI GetProfileId(
        /* [out] */ Int32* result);

    //the APN to connect to
    CARAPI GetApn(
        /* [out] */ String* result);

    //For example, "IP", "IPV6", "IPV4V6", or "PPP".
    CARAPI GetProtocol(
        /* [out] */ String* result);

    //(None: 0, PAP: 1, CHAP: 2, PAP&CHAP: 3)
    CARAPI GetAuthType(
        /* [out] */ Int32* result);

    //the username for APN, or NULL
    CARAPI GetUser(
        /* [out] */ String* result);

    //the password for APN, or NULL
    CARAPI GetPassword(
        /* [out] */ String* result);

    //the profile type, TYPE_COMMON, TYPE_3GPP, TYPE_3GPP2
    CARAPI GetType(
        /* [out] */ Int32* result);

    //the period in seconds to limit the maximum connections
    CARAPI GetMaxConnsTime(
        /* [out] */ Int32* result);

    //the maximum connections during maxConnsTime
    CARAPI GetMaxConns(
        /* [out] */ Int32* result);

    //a new PDN connection request for that given PDN
    CARAPI GetWaitTime(
        /* [out] */ Int32* result);

    //true to enable the profile, false to disable
    CARAPI GetEnabled(
        /* [out] */ Boolean* result);

public:
    static const Int32 TYPE_COMMON;

    static const Int32 TYPE_3GPP;

    static const Int32 TYPE_3GPP2;

private:
    //id of the data profile
    Int32 mProfileId;

    //the APN to connect to
    String mApn;

    //For example, "IP", "IPV6", "IPV4V6", or "PPP".
    String mProtocol;

    //(None: 0, PAP: 1, CHAP: 2, PAP&CHAP: 3)
    Int32 mAuthType;

    //the username for APN, or NULL
    String mUser;

    //the password for APN, or NULL
    String mPassword;

    //the profile type, TYPE_COMMON, TYPE_3GPP, TYPE_3GPP2
    Int32 mType;

    //the period in seconds to limit the maximum connections
    Int32 mMaxConnsTime;

    //the maximum connections during maxConnsTime
    Int32 mMaxConns;

    //a new PDN connection request for that given PDN
    Int32 mWaitTime;

    //true to enable the profile, false to disable
    Boolean mEnabled;
};

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DATAPROFILE_H__
