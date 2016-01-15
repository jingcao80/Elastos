
#include "elastos/droid/content/CSyncAdapterType.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL_2(CSyncAdapterType, Object, ISyncAdapterType, IParcelable)

CAR_OBJECT_IMPL(CSyncAdapterType)

CSyncAdapterType::CSyncAdapterType()
    : mIsKey(FALSE)
    , mUserVisible(FALSE)
    , mSupportsUploading(FALSE)
    , mIsAlwaysSyncable(FALSE)
    , mAllowParallelSyncs(FALSE)
{}

CSyncAdapterType::~CSyncAdapterType()
{}

ECode CSyncAdapterType::GetAuthority(
    /* [out] */ String* authority)
{
    VALIDATE_NOT_NULL(authority)
    *authority = mAuthority;
    return NOERROR;
}

ECode CSyncAdapterType::SetAuthority(
    /* [in] */ const String& authority)
{
    mAuthority = authority;
    return NOERROR;
}

ECode CSyncAdapterType::GetAccountType(
    /* [out] */ String* accountType)
{
    VALIDATE_NOT_NULL(accountType)
    *accountType = mAccountType;
    return NOERROR;
}

ECode CSyncAdapterType::SetAccountType(
    /* [in] */ const String&  accountType)
{
    mAccountType = accountType;
    return NOERROR;
}

ECode CSyncAdapterType::SetKey(
    /* [in] */ Boolean isKey)
{
    mIsKey = isKey;
    return NOERROR;
}

ECode CSyncAdapterType::IsKey(
    /* [out] */ Boolean* isKey)
{
    VALIDATE_NOT_NULL(isKey)
    *isKey = mIsKey;
    return NOERROR;
}

ECode CSyncAdapterType::SupportsUploading(
    /* [out] */ Boolean* isSupport)
{
    VALIDATE_NOT_NULL(isSupport)
    if (mIsKey) {
        //throw new IllegalStateException("this method is not allowed to be called when this is a key");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    *isSupport = mSupportsUploading;
    return NOERROR;
}

ECode CSyncAdapterType::IsUserVisible(
    /* [out] */ Boolean* isUserVisible)
{
    VALIDATE_NOT_NULL(isUserVisible)
    if (mIsKey) {
        //throw new IllegalStateException("this method is not allowed to be called when this is a key");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    *isUserVisible = mUserVisible;
    return NOERROR;
}

ECode CSyncAdapterType::AllowParallelSyncs(
    /* [out] */ Boolean* isAllow)
{
    VALIDATE_NOT_NULL(isAllow)
    if (mIsKey) {
        //throw new IllegalStateException("this method is not allowed to be called when this is a key");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    *isAllow = mAllowParallelSyncs;
    return NOERROR;
}

ECode CSyncAdapterType::IsAlwaysSyncable(
    /* [out] */ Boolean* isAlways)
{
    VALIDATE_NOT_NULL(isAlways)
    if (mIsKey) {
        //throw new IllegalStateException("this method is not allowed to be called when this is a key");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    *isAlways = mIsAlwaysSyncable;
    return NOERROR;
}

ECode CSyncAdapterType::GetSettingsActivity(
    /* [out] */ String* settingsActivity)
{
    VALIDATE_NOT_NULL(settingsActivity)
    if (mIsKey) {
        //throw new IllegalStateException("this method is not allowed to be called when this is a key");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    *settingsActivity = mSettingsActivity;
    return NOERROR;
}

ECode CSyncAdapterType::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* isEqual)
{
    VALIDATE_NOT_NULL(isEqual)

    if (_CObject_Compare(obj, (ISyncAdapterType*) this)) {
        *isEqual = TRUE;
        return NOERROR;
    }

    if (!(ISyncAdapterType::Probe(obj) != NULL)) {
        *isEqual = FALSE;
        return NOERROR;
    }

    AutoPtr<ISyncAdapterType> other = (ISyncAdapterType*) obj;
    // don't include userVisible or supportsUploading in the equality check
    String authority;
    String accountType;
    FAIL_RETURN(other->GetAuthority(&authority))
    FAIL_RETURN(other->GetAccountType(&accountType))
    *isEqual = mAuthority.Equals(authority) && mAccountType.Equals(accountType);
    return NOERROR;
}

ECode CSyncAdapterType::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    Int32 result = 17;
   result = 31 * result + mAuthority.GetHashCode();
   result = 31 * result + mAccountType.GetHashCode();
    // don't include userVisible or supportsUploading  the hash
    *hashCode =  result;
    return E_NOT_IMPLEMENTED;
}

ECode CSyncAdapterType::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    if (mIsKey) {
        *str = "SyncAdapterType Key {name=";
        *str += mAuthority;
        *str += ", type=";
        *str += mAccountType;
        *str += "}";
    }
    else {
        *str = "SyncAdapterType {name=";
        *str += mAuthority;
        *str += ", type=";
        *str += mAccountType;
        *str += ", userVisible=";
        *str += StringUtils::BooleanToString(mUserVisible);
        *str += ", supportsUploading=";
        *str += StringUtils::BooleanToString(mSupportsUploading);
        *str += ", isAlwaysSyncable=";
        *str += StringUtils::BooleanToString(mIsAlwaysSyncable);
        *str += ", allowParallelSyncs=";
        *str += StringUtils::BooleanToString(mAllowParallelSyncs);
        *str += ", settingsActivity=";
        *str += mSettingsActivity;
        *str += "}";
    }

    return NOERROR;
}

ECode CSyncAdapterType::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    Int32 value = 0;
    source->ReadString(&mAuthority);
    source->ReadString(&mAccountType);
    source->ReadInt32(&value);
    mUserVisible = value != 0;
    source->ReadInt32(&value);
    mSupportsUploading = value != 0;
    source->ReadInt32(&value);
    mIsAlwaysSyncable = value != 0;
    source->ReadInt32(&value);
    mAllowParallelSyncs = value != 0;
    source->ReadString(&mSettingsActivity);
    return NOERROR;
}

ECode CSyncAdapterType::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    if (mIsKey) {
        //throw new IllegalStateException("keys aren't parcelable");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    dest->WriteString(mAuthority);
    dest->WriteString(mAccountType);
    dest->WriteInt32(mUserVisible ? 1 : 0);
    dest->WriteInt32(mSupportsUploading ? 1 : 0);
    dest->WriteInt32(mIsAlwaysSyncable ? 1 : 0);
    dest->WriteInt32(mAllowParallelSyncs ? 1 : 0);
    dest->WriteString(mSettingsActivity);
    return NOERROR;
}

ECode CSyncAdapterType::constructor()
{
    return NOERROR;
}

ECode CSyncAdapterType::constructor(
    /* [in] */ const String& authority,
    /* [in] */ const String& accountType,
    /* [in] */ Boolean userVisible,
    /* [in] */ Boolean supportsUploading)
{
    if (authority.IsNull() || authority.GetLength() == 0) {
        //throw new IllegalArgumentException("the authority must not be empty: " + authority);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (accountType.IsNull() || accountType.GetLength() == 0) {
        //throw new IllegalArgumentException("the accountType must not be empty: " + accountType);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    mAuthority = authority;
    mAccountType = accountType;
    mUserVisible = userVisible;
    mSupportsUploading = supportsUploading;
    mIsAlwaysSyncable = FALSE;
    mAllowParallelSyncs = FALSE;
    mSettingsActivity = String(NULL);
    mIsKey = FALSE;
    return NOERROR;
}

ECode CSyncAdapterType::constructor(
    /* [in] */ const String& authority,
    /* [in] */ const String& accountType,
    /* [in] */ Boolean userVisible,
    /* [in] */ Boolean supportsUploading,
    /* [in] */ Boolean isAlwaysSyncable,
    /* [in] */ Boolean allowParallelSyncs,
    /* [in] */ const String& settingsActivity)
{
    if (authority.IsNull() || authority.GetLength() == 0) {
        //throw new IllegalArgumentException("the authority must not be empty: " + authority);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (accountType.IsNull() || accountType.GetLength() == 0) {
        //throw new IllegalArgumentException("the accountType must not be empty: " + accountType);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    mAuthority = authority;
    mAccountType = accountType;
    mUserVisible = userVisible;
    mSupportsUploading = supportsUploading;
    mIsAlwaysSyncable = isAlwaysSyncable;
    mAllowParallelSyncs = allowParallelSyncs;
    mSettingsActivity = settingsActivity;
    mIsKey = FALSE;
    return NOERROR;
}

ECode CSyncAdapterType::constructor(
    /* [in] */ const String& authority,
    /* [in] */ const String& accountType)
{
    if (authority.IsNull() || authority.GetLength() == 0) {
        //throw new IllegalArgumentException("the authority must not be empty: " + authority);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (accountType.IsNull() || accountType.GetLength() == 0) {
        //throw new IllegalArgumentException("the accountType must not be empty: " + accountType);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    mAuthority = authority;
    mAccountType = accountType;
    mUserVisible = TRUE;
    mSupportsUploading = TRUE;
    mIsAlwaysSyncable = FALSE;
    mAllowParallelSyncs = FALSE;
    mSettingsActivity = String(NULL);
    mIsKey = TRUE;
    return NOERROR;
}

AutoPtr<ISyncAdapterType> CSyncAdapterType::NewKey(
    /* [in] */ const String& authority,
    /* [in] */ const String& accountType)
{
    AutoPtr<CSyncAdapterType> type;
    CSyncAdapterType::NewByFriend((CSyncAdapterType**)&type);
    type->constructor(authority, accountType);
    return (ISyncAdapterType*)type.Get();
}


}
}
}

