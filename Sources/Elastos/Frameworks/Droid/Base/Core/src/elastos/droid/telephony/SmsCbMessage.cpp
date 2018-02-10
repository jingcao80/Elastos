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

#include "elastos/droid/telephony/SmsCbMessage.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Telephony {

//=====================================================================
//                             SmsCbMessage
//=====================================================================
CAR_INTERFACE_IMPL_2(SmsCbMessage, Object, ISmsCbMessage, IParcelable);

const String SmsCbMessage::LOGTAG("SMSCB");

SmsCbMessage::SmsCbMessage()
    : mMessageFormat(0)
    , mGeographicalScope(0)
    , mSerialNumber(0)
    , mServiceCategory(0)
    , mPriority(0)
{
}

ECode SmsCbMessage::constructor(
    /* [in] */ Int32 messageFormat,
    /* [in] */ Int32 geographicalScope,
    /* [in] */ Int32 serialNumber,
    /* [in] */ ISmsCbLocation* location,
    /* [in] */ Int32 serviceCategory,
    /* [in] */ const String& language,
    /* [in] */ const String& body,
    /* [in] */ Int32 priority,
    /* [in] */ ISmsCbEtwsInfo* etwsWarningInfo,
    /* [in] */ ISmsCbCmasInfo* cmasWarningInfo)
{
    mMessageFormat = messageFormat;
    mGeographicalScope = geographicalScope;
    mSerialNumber = serialNumber;
    mLocation = location;
    mServiceCategory = serviceCategory;
    mLanguage = language;
    mBody = body;
    mPriority = priority;
    mEtwsWarningInfo = etwsWarningInfo;
    mCmasWarningInfo = cmasWarningInfo;
    return NOERROR;
}

ECode SmsCbMessage::constructor()
{
    return NOERROR;
}

ECode SmsCbMessage::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);
    dest->WriteInt32(mMessageFormat);
    dest->WriteInt32(mGeographicalScope);
    dest->WriteInt32(mSerialNumber);
    dest->WriteInterfacePtr(mLocation);
    dest->WriteInt32(mServiceCategory);
    dest->WriteString(mLanguage);
    dest->WriteString(mBody);
    dest->WriteInt32(mPriority);
    dest->WriteInterfacePtr(mEtwsWarningInfo);
    dest->WriteInterfacePtr(mCmasWarningInfo);
    return NOERROR;
}

ECode SmsCbMessage::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mMessageFormat);
    source->ReadInt32(&mGeographicalScope);
    source->ReadInt32(&mSerialNumber);

    AutoPtr<IInterface> obj;
    source->ReadInterfacePtr((HANDLE*)&obj);
    mLocation = ISmsCbLocation::Probe(obj);

    source->ReadInt32(&mServiceCategory);
    source->ReadString(&mLanguage);
    source->ReadString(&mBody);
    source->ReadInt32(&mPriority);

    obj = NULL;
    source->ReadInterfacePtr((HANDLE*)&obj);
    mEtwsWarningInfo = ISmsCbEtwsInfo::Probe(obj);

    obj = NULL;
    source->ReadInterfacePtr((HANDLE*)&obj);
    mCmasWarningInfo = ISmsCbCmasInfo::Probe(obj);
    return NOERROR;
}

ECode SmsCbMessage::GetGeographicalScope(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mGeographicalScope;
    return NOERROR;
}

ECode SmsCbMessage::GetSerialNumber(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSerialNumber;
    return NOERROR;
}

ECode SmsCbMessage::GetLocation(
    /* [out] */ ISmsCbLocation** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mLocation;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SmsCbMessage::GetServiceCategory(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mServiceCategory;
    return NOERROR;
}

ECode SmsCbMessage::GetLanguageCode(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mLanguage;
    return NOERROR;
}

ECode SmsCbMessage::GetMessageBody(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mBody;
    return NOERROR;
}

ECode SmsCbMessage::GetMessageFormat(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mMessageFormat;
    return NOERROR;
}

ECode SmsCbMessage::GetMessagePriority(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPriority;
    return NOERROR;
}

ECode SmsCbMessage::GetEtwsWarningInfo(
    /* [out] */ ISmsCbEtwsInfo** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mEtwsWarningInfo;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SmsCbMessage::GetCmasWarningInfo(
    /* [out] */ ISmsCbCmasInfo** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCmasWarningInfo;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SmsCbMessage::IsEmergencyMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPriority == MESSAGE_PRIORITY_EMERGENCY;
    return NOERROR;
}

ECode SmsCbMessage::IsEtwsMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mEtwsWarningInfo != NULL;
    return NOERROR;
}

ECode SmsCbMessage::IsCmasMessage(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCmasWarningInfo != NULL;
    return NOERROR;
}

ECode SmsCbMessage::ToString(
    /* [out] */ String* result)
{
    StringBuilder sb("SmsCbMessage{geographicalScope=");
    sb += mGeographicalScope;
    sb += ", serialNumber=";
    sb += mSerialNumber;
    sb += ", location=";
    sb += mLocation;
    sb += ", serviceCategory=";
    sb += mServiceCategory;
    sb += ", language=";
    sb += mLanguage;
    sb += ", body=";
    sb += mBody;
    sb += ", priority=";
    sb += mPriority;
    if (mEtwsWarningInfo != NULL) {
        sb += ", ";
        sb += Object::ToString(mEtwsWarningInfo);
    }
    if (mCmasWarningInfo != NULL) {
        sb += ", ";
        sb += Object::ToString(mCmasWarningInfo);
    }
    sb += "}";
    return sb.ToString(result);
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos


