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

#ifndef __ELASTOS_DROID_TELECOM_PARCELABLECONTERENCE_H__
#define __ELASTOS_DROID_TELECOM_PARCELABLECONTERENCE_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Telecom.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Telecom {

/**
 * A parcelable representation of a conference connection.
 * @hide
 */
class ParcelableConference
    : public Object
    , public IParcelableConference
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IPhoneAccountHandle* phoneAccount,
        /* [in] */ Int32 state,
        /* [in] */ Int32 capabilities,
        /* [in] */ IList* connectionIds);

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI GetPhoneAccount(
        /* [out] */ IPhoneAccountHandle** result);

    CARAPI GetState(
        /* [out] */ Int32* result);

    CARAPI GetCapabilities(
        /* [out] */ Int32* result);

    CARAPI GetConnectionIds(
        /* [out] */ IList** result);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    /** Writes ParcelableConference object into a Parcel. */
    CARAPI WriteToParcel(
        /* [in] */ IParcel* destination);

private:
    AutoPtr<IPhoneAccountHandle> mPhoneAccount;
    Int32 mState;
    Int32 mCapabilities;
    AutoPtr<IList> mConnectionIds;
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_PARCELABLECONTERENCE_H__