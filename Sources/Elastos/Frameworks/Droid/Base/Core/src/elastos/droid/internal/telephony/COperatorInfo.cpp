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

#include "elastos/droid/internal/telephony/COperatorInfo.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CAR_INTERFACE_IMPL_2(COperatorInfo, Object, IOperatorInfo, IParcelable)
CAR_OBJECT_IMPL(COperatorInfo)
COperatorInfo::COperatorInfo()
    : mState(OperatorInfoState_UNKNOWN)
    , mRadioTech("")
{
}

ECode COperatorInfo::constructor()
{
    return NOERROR;
}

ECode COperatorInfo::GetOperatorAlphaLong(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOperatorAlphaLong;
    return NOERROR;
}

ECode COperatorInfo::GetOperatorAlphaShort(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOperatorAlphaShort;
    return NOERROR;
}

ECode COperatorInfo::GetOperatorNumeric(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOperatorNumeric;
    return NOERROR;
}

ECode COperatorInfo::GetState(
    /* [out] */ IOperatorInfoState* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mState;
    return NOERROR;
}

ECode COperatorInfo::GetRadioTech(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mRadioTech;
    return NOERROR;
}

ECode COperatorInfo::constructor(
    /* [in] */ const String& operatorAlphaLong,
    /* [in] */ const String& operatorAlphaShort,
    /* [in] */ const String& operatorNumeric,
    /* [in] */ IOperatorInfoState state)
{
    mOperatorAlphaLong = operatorAlphaLong;
    mOperatorAlphaShort = operatorAlphaShort;

    mOperatorNumeric = operatorNumeric;
    mRadioTech = "";
    /* operatorNumeric format: PLMN+RAT or PLMN */
    if (NULL != operatorNumeric) {
        AutoPtr<ArrayOf<String> > values;
        StringUtils::Split(operatorNumeric, String("\\+"), (ArrayOf<String>**)&values);
        mOperatorNumeric = (*values)[0];
        if (values->GetLength() > 1)
            mRadioTech = (*values)[1];
    }

    mState = state;
    return NOERROR;
}

ECode COperatorInfo::constructor(
    /* [in] */ const String& operatorAlphaLong,
    /* [in] */ const String& operatorAlphaShort,
    /* [in] */ const String& operatorNumeric,
    /* [in] */ const String& stateString)
{
    IOperatorInfoState state;
    FAIL_RETURN(RilStateToState(stateString, &state));
    return constructor(operatorAlphaLong, operatorAlphaShort, operatorNumeric, state);
}

/**
 * See state strings defined in ril.h RIL_REQUEST_QUERY_AVAILABLE_NETWORKS
 */
ECode COperatorInfo::RilStateToState(
    /* [in] */ const String& s,
    /* [out] */ IOperatorInfoState* result)
{
    VALIDATE_NOT_NULL(result);
    if (s.Equals("unknown")) {
        *result = OperatorInfoState_UNKNOWN;
        return NOERROR;
    }
    else if (s.Equals("available")) {
        *result = OperatorInfoState_AVAILABLE;
        return NOERROR;
    }
    else if (s.Equals("current")) {
        *result = OperatorInfoState_CURRENT;
        return NOERROR;
    }
    else if (s.Equals("forbidden")) {
        *result = OperatorInfoState_FORBIDDEN;
        return NOERROR;
    }

    // throw new RuntimeException(
    //     "RIL impl error: Invalid network state '" + s + "'");
    return E_RUNTIME_EXCEPTION;
}

// @Override
ECode COperatorInfo::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String("OperatorInfo ") + mOperatorAlphaLong
           + "/" + mOperatorAlphaShort
           + "/" + mOperatorNumeric
           + "/" + StringUtils::ToString(mState)
           + "/" + mRadioTech;
    return NOERROR;
}

ECode COperatorInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    String operatorAlphaLong, operatorAlphaShort, operatorNumeric;
    source->ReadString(&operatorAlphaLong);
    source->ReadString(&operatorAlphaShort);
    source->ReadString(&operatorNumeric);
    IOperatorInfoState state;
    source->ReadInt32(&state);
    return constructor(operatorAlphaLong, operatorAlphaShort, operatorNumeric, state);
}

/**
 * Implement the Parcelable interface.
 * Method to serialize a OperatorInfo object.
 */
// @Override
ECode COperatorInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteString(mOperatorAlphaLong);
    dest->WriteString(mOperatorAlphaShort);
    dest->WriteString(mOperatorNumeric + "+" + mRadioTech);
    dest->WriteInt32(mState);
    return NOERROR;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos