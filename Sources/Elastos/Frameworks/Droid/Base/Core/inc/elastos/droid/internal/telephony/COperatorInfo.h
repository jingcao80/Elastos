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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_COPERATORINFO_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_COPERATORINFO_H__

#include "_Elastos_Droid_Internal_Telephony_COperatorInfo.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

/**
 * {@hide}
 */
CarClass(COperatorInfo)
    , public Object
    , public IOperatorInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    COperatorInfo();

    CARAPI constructor();

    CARAPI GetOperatorAlphaLong(
        /* [out] */ String* result);

    CARAPI GetOperatorAlphaShort(
        /* [out] */ String* result);

    CARAPI GetOperatorNumeric(
        /* [out] */ String* result);

    CARAPI GetState(
        /* [out] */ IOperatorInfoState* result);

    CARAPI GetRadioTech(
        /* [out] */ String* result);

    CARAPI constructor(
        /* [in] */ const String& operatorAlphaLong,
        /* [in] */ const String& operatorAlphaShort,
        /* [in] */ const String& operatorNumeric,
        /* [in] */ IOperatorInfoState state);

    CARAPI constructor(
        /* [in] */ const String& operatorAlphaLong,
        /* [in] */ const String& operatorAlphaShort,
        /* [in] */ const String& operatorNumeric,
        /* [in] */ const String& stateString);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    /**
     * Implement the Parcelable interface.
     * Method to serialize a OperatorInfo object.
     */
    // @Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    /**
     * See state strings defined in ril.h RIL_REQUEST_QUERY_AVAILABLE_NETWORKS
     */
    static CARAPI RilStateToState(
        /* [in] */ const String& s,
        /* [out] */ IOperatorInfoState* result);

private:
    String mOperatorAlphaLong;
    String mOperatorAlphaShort;
    String mOperatorNumeric;
    IOperatorInfoState mState;
    String mRadioTech;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_COPERATORINFO_H__
