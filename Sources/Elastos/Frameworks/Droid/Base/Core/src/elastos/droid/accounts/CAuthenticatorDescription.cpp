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

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/accounts/CAuthenticatorDescription.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Accounts {

const String CAuthenticatorDescription::TAG("AuthenticatorDescription");

CAR_OBJECT_IMPL(CAuthenticatorDescription)

CAR_INTERFACE_IMPL_2(CAuthenticatorDescription, Object, IAuthenticatorDescription, IParcelable)

CAuthenticatorDescription::CAuthenticatorDescription()
    : mLabelId(0)
    , mIconId(0)
    , mSmallIconId(0)
    , mAccountPreferencesId(0)
    , mCustomTokens(FALSE)
{
}

ECode CAuthenticatorDescription::NewKey(
    /* [in] */ const String& type,
    /* [out] */ IAuthenticatorDescription** description)
{
    VALIDATE_NOT_NULL(description);
    if (type.IsNull()) {
        Slogger::E(TAG, "type cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("type cannot be null");
    }
    AutoPtr<CAuthenticatorDescription> cdescription;
    FAIL_RETURN(CAuthenticatorDescription::NewByFriend(type,
            (CAuthenticatorDescription**)&cdescription));
    *description = cdescription;
    REFCOUNT_ADD(*description);
    return NOERROR;
}

ECode CAuthenticatorDescription::GetType(
    /* [out] */ String* type)
{
    VALIDATE_NOT_NULL(type);
    *type = mType;
    return NOERROR;
}

ECode CAuthenticatorDescription::SetType(
    /* [in] */ const String& type)
{
    mType = type;
    return NOERROR;
}

ECode CAuthenticatorDescription::GetLabelId(
    /* [out] */ Int32* labelId)
{
    VALIDATE_NOT_NULL(labelId);
    *labelId = mLabelId;
    return NOERROR;
}

ECode CAuthenticatorDescription::SetLabelId(
    /* [in] */ Int32 labelId)
{
    mLabelId = labelId;
    return NOERROR;
}

ECode CAuthenticatorDescription::GetIconId(
    /* [out] */ Int32* iconId)
{
    VALIDATE_NOT_NULL(iconId);
    *iconId = mIconId;
    return NOERROR;
}

ECode CAuthenticatorDescription::SetIconId(
    /* [in] */ Int32 iconId)
{
    mIconId = iconId;
    return NOERROR;
}

ECode CAuthenticatorDescription::GetSmallIconId(
    /* [out] */ Int32* smallIconId)
{
    VALIDATE_NOT_NULL(smallIconId);
    *smallIconId = mSmallIconId;
    return NOERROR;
}

ECode CAuthenticatorDescription::SetSmallIconId(
    /* [in] */ Int32 smallIconId)
{
    mSmallIconId = smallIconId;
    return NOERROR;
}

ECode CAuthenticatorDescription::GetAccountPreferencesId(
    /* [out] */ Int32* preferencesId)
{
    VALIDATE_NOT_NULL(preferencesId);
    *preferencesId = mAccountPreferencesId;
    return NOERROR;
}

ECode CAuthenticatorDescription::SetAccountPreferencesId(
    /* [in] */ Int32 preferencesId)
{
    mAccountPreferencesId = preferencesId;
    return NOERROR;
}

ECode CAuthenticatorDescription::GetPackageName(
    /* [out] */ String* packageName)
{
    VALIDATE_NOT_NULL(packageName);
    *packageName = mPackageName;
    return NOERROR;
}

ECode CAuthenticatorDescription::SetPackageName(
    /* [in] */ const String& packageName)
{
    mPackageName = packageName;
    return NOERROR;
}

ECode CAuthenticatorDescription::GetCustomTokens(
    /* [out] */ Boolean* customTokens)
{
    VALIDATE_NOT_NULL(customTokens);
    *customTokens = mCustomTokens;
    return NOERROR;
}

ECode CAuthenticatorDescription::SetCustomTokens(
    /* [in] */ Boolean customTokens)
{
    mCustomTokens = customTokens;
    return NOERROR;
}

ECode CAuthenticatorDescription::ToString(
    /* [out] */ String* s)
{
    VALIDATE_NOT_NULL(s);
    *s = String("AuthenticatorDescription {type=") + mType + String("}");
    return NOERROR;
}

ECode CAuthenticatorDescription::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteString(mType);
    dest->WriteString(mPackageName);
    dest->WriteInt32(mLabelId);
    dest->WriteInt32(mIconId);
    dest->WriteInt32(mSmallIconId);
    dest->WriteInt32(mAccountPreferencesId);
    dest->WriteBoolean(mCustomTokens);
    return NOERROR;
}

ECode CAuthenticatorDescription::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadString(&mType);
    source->ReadString(&mPackageName);
    source->ReadInt32(&mLabelId);
    source->ReadInt32(&mIconId);
    source->ReadInt32(&mSmallIconId);
    source->ReadInt32(&mAccountPreferencesId);
    source->ReadBoolean(&mCustomTokens);
    return NOERROR;
}

ECode CAuthenticatorDescription::constructor(
    /* [in] */ const String& type,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 labelId,
    /* [in] */ Int32 iconId,
    /* [in] */ Int32 smallIconId,
    /* [in] */ Int32 prefId,
    /* [in] */ Boolean customTokens)
{
    if (type.IsNull()) {
        Slogger::E(TAG, "type cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("type cannot be null");
    }
    if (packageName.IsNull())  {
        Slogger::E(TAG, "packageName cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("packageName cannot be null");
    }
    mType = type;
    mPackageName = packageName;
    mLabelId = labelId;
    mIconId = iconId;
    mSmallIconId = smallIconId;
    mAccountPreferencesId = prefId;
    mCustomTokens = customTokens;
    return NOERROR;
}

ECode CAuthenticatorDescription::constructor(
    /* [in] */ const String& type,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 labelId,
    /* [in] */ Int32 iconId,
    /* [in] */ Int32 smallIconId,
    /* [in] */ Int32 prefId)
{
    return constructor(type, packageName, labelId, iconId, smallIconId, prefId, FALSE);
}

ECode CAuthenticatorDescription::constructor(
    /* [in] */ const String& type)
{
    mType = type;
    return NOERROR;
}

ECode CAuthenticatorDescription::constructor()
{
    return NOERROR;
}

} // namespace Accounts
} // namespace Droid
} // namespace Elastos
