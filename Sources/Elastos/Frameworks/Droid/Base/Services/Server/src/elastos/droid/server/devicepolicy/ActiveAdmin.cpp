
#include "ActiveAdmin.h"
#include <elastos/utility/logging/Logger.h>

namespace Elastos {
namespace Droid {
namespace Server {

const Int32 ActiveAdmin::DEF_MINIMUM_PASSWORD_LENGTH = 0;

const Int32 ActiveAdmin::DEF_PASSWORD_HISTORY_LENGTH = 0;

const Int32 ActiveAdmin::DEF_MINIMUM_PASSWORD_UPPER_CASE = 0;

const Int32 ActiveAdmin::DEF_MINIMUM_PASSWORD_LOWER_CASE = 0;

const Int32 ActiveAdmin::DEF_MINIMUM_PASSWORD_LETTERS = 1;

const Int32 ActiveAdmin::DEF_MINIMUM_PASSWORD_NUMERIC = 1;

const Int32 ActiveAdmin::DEF_MINIMUM_PASSWORD_SYMBOLS = 1;

const Int32 ActiveAdmin::DEF_MINIMUM_PASSWORD_NON_LETTER = 0;

const Int64 ActiveAdmin::DEF_MAXIMUM_TIME_TO_UNLOCK = 0;

const Int32 ActiveAdmin::DEF_MAXIMUM_FAILED_PASSWORDS_FOR_WIPE = 0;

const Int64 ActiveAdmin::DEF_PASSWORD_EXPIRATION_TIMEOUT = 0;

const Int64 ActiveAdmin::DEF_PASSWORD_EXPIRATION_DATE = 0;

const Int32 ActiveAdmin::DEF_KEYGUARD_FEATURES_DISABLED = 0;

ActiveAdmin::ActiveAdmin(
    /* [in] */ IDeviceAdminInfo* info)
{
    mInfo = info;

    mPasswordQuality = IDevicePolicyManager::PASSWORD_QUALITY_UNSPECIFIED;
    mMinimumPasswordLength = DEF_MINIMUM_PASSWORD_LENGTH;
    mPasswordHistoryLength = DEF_PASSWORD_HISTORY_LENGTH;
    mMinimumPasswordUpperCase = DEF_MINIMUM_PASSWORD_UPPER_CASE;
    mMinimumPasswordLowerCase = DEF_MINIMUM_PASSWORD_LOWER_CASE;
    mMinimumPasswordLetters = DEF_MINIMUM_PASSWORD_LETTERS;
    mMinimumPasswordNumeric = DEF_MINIMUM_PASSWORD_NUMERIC;
    mMinimumPasswordSymbols = DEF_MINIMUM_PASSWORD_SYMBOLS;
    mMinimumPasswordNonLetter = DEF_MINIMUM_PASSWORD_NON_LETTER;
    mMaximumTimeToUnlock = DEF_MAXIMUM_TIME_TO_UNLOCK;
    mMaximumFailedPasswordsForWipe = DEF_MAXIMUM_FAILED_PASSWORDS_FOR_WIPE;
    mPasswordExpirationTimeout = DEF_PASSWORD_EXPIRATION_TIMEOUT;
    mPasswordExpirationDate = DEF_PASSWORD_EXPIRATION_DATE;
    mDisabledKeyguardFeatures = DEF_KEYGUARD_FEATURES_DISABLED;

    mEncryptionRequested = FALSE;
    mDisableCamera = FALSE;

    // TODO: review implementation decisions with frameworks team
    mSpecifiesGlobalProxy = FALSE;
    mGlobalProxySpec = String();
    mGlobalProxyExclusionList = String();
}

UInt32 ActiveAdmin::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 ActiveAdmin::Release()
{
    return ElRefBase::Release();
}

ECode ActiveAdmin::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return E_NOT_IMPLEMENTED;
}

Int32 ActiveAdmin::GetUid()
{
    AutoPtr<IActivityInfo> activityInfo;
    mInfo->GetActivityInfo((IActivityInfo**)&activityInfo);

    AutoPtr<IApplicationInfo> appInfo;
    activityInfo->GetApplicationInfo((IApplicationInfo**)&appInfo);

    Int32 uid;
    appInfo->GetUid(&uid);
    return uid;
}

AutoPtr<IUserHandle> ActiveAdmin::GetUserHandle()
{
    AutoPtr<IUserHandleHelper> handleHelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&handleHelper);

    Int32 userId;
    handleHelper->GetUserId(this->GetUid(), &userId);

    AutoPtr<IUserHandle> handle;
    CUserHandle::New(userId, (IUserHandle**)&handle);
    return handle;
}

void ActiveAdmin::WriteToXml(
    /* [in] */ IXmlSerializer* out)
{
    out->WriteStartTag(String(), String("policies"));
    mInfo->WritePoliciesToXml(out);
    out->WriteEndTag(String(), String("policies"));

    if (mPasswordQuality != IDevicePolicyManager::PASSWORD_QUALITY_UNSPECIFIED) {
        out->WriteStartTag(String(), String("password-quality"));
        out->WriteAttribute(String(), String("value"), StringUtils::Int32ToString(mPasswordQuality));
        out->WriteEndTag(String(), String("password-quality"));

        if (mMinimumPasswordLength != DEF_MINIMUM_PASSWORD_LENGTH) {
            out->WriteStartTag(String(), String("min-password-length"));
            out->WriteAttribute(String(), String("value"), StringUtils::Int32ToString(mMinimumPasswordLength));
            out->WriteEndTag(String(), String("min-password-length"));
        }

        if(mPasswordHistoryLength != DEF_PASSWORD_HISTORY_LENGTH) {
            out->WriteStartTag(String(), String("password-history-length"));
            out->WriteAttribute(String(), String("value"), StringUtils::Int32ToString(mPasswordHistoryLength));
            out->WriteEndTag(String(), String("password-history-length"));
        }

        if (mMinimumPasswordUpperCase != DEF_MINIMUM_PASSWORD_UPPER_CASE) {
            out->WriteStartTag(String(), String("min-password-uppercase"));
            out->WriteAttribute(String(), String("value"), StringUtils::Int32ToString(mMinimumPasswordUpperCase));
            out->WriteEndTag(String(), String("min-password-uppercase"));
        }

        if (mMinimumPasswordLowerCase != DEF_MINIMUM_PASSWORD_LOWER_CASE) {
            out->WriteStartTag(String(), String("min-password-lowercase"));
            out->WriteAttribute(String(), String("value"), StringUtils::Int32ToString(mMinimumPasswordLowerCase));
            out->WriteEndTag(String(), String("min-password-lowercase"));
        }

        if (mMinimumPasswordLetters != DEF_MINIMUM_PASSWORD_LETTERS) {
            out->WriteStartTag(String(), String("min-password-letters"));
            out->WriteAttribute(String(), String("value"), StringUtils::Int32ToString(mMinimumPasswordLetters));
            out->WriteEndTag(String(), String("min-password-letters"));
        }

        if (mMinimumPasswordNumeric != DEF_MINIMUM_PASSWORD_NUMERIC) {
            out->WriteStartTag(String(), String("min-password-numeric"));
            out->WriteAttribute(String(), String("value"), StringUtils::Int32ToString(mMinimumPasswordNumeric));
            out->WriteEndTag(String(), String("min-password-numeric"));
        }

        if (mMinimumPasswordSymbols != DEF_MINIMUM_PASSWORD_SYMBOLS) {
            out->WriteStartTag(String(), String("min-password-symbols"));
            out->WriteAttribute(String(), String("value"), StringUtils::Int32ToString(mMinimumPasswordSymbols));
            out->WriteEndTag(String(), String("min-password-symbols"));
        }

        if (mMinimumPasswordNonLetter > DEF_MINIMUM_PASSWORD_NON_LETTER) {
            out->WriteStartTag(String(), String("min-password-nonletter"));
            out->WriteAttribute(String(), String("value"), StringUtils::Int32ToString(mMinimumPasswordNonLetter));
            out->WriteEndTag(String(), String("min-password-nonletter"));
        }
    }

    if (mMaximumTimeToUnlock != DEF_MAXIMUM_TIME_TO_UNLOCK) {
        out->WriteStartTag(String(), "max-time-to-unlock");
        out->WriteAttribute(String(), String("value"), StringUtils::Int64ToString(mMaximumTimeToUnlock));
        out->WriteEndTag(String(), "max-time-to-unlock");
    }

    if (mMaximumFailedPasswordsForWipe != DEF_MAXIMUM_FAILED_PASSWORDS_FOR_WIPE) {
        out->WriteStartTag(String(), String("max-failed-password-wipe"));
        out->WriteAttribute(String(), String("value"), StringUtils::Int32ToString(mMaximumFailedPasswordsForWipe));
        out->WriteEndTag(String(), String("max-failed-password-wipe"));
    }

    if (mSpecifiesGlobalProxy) {
        out->WriteStartTag(String(), String("specifies-global-proxy"));
        out->WriteAttribute(String(), String("value"), StringUtils::BooleanToString(mSpecifiesGlobalProxy));
        out->WriteEndTag(String(), String("specifies-global-proxy"));

        if (!mGlobalProxySpec.IsNull()) {
            out->WriteStartTag(String(), String("global-proxy-spec"));
            out->WriteAttribute(String(), String("value"), mGlobalProxySpec);
            out->WriteEndTag(String(), String("global-proxy-spec"));
        }

        if (!mGlobalProxyExclusionList.IsNull()) {
            out->WriteStartTag(String(), String("global-proxy-exclusion-list"));
            out->WriteAttribute(String(), String("value"), mGlobalProxyExclusionList);
            out->WriteEndTag(String(), String("global-proxy-exclusion-list"));
        }
    }

    if (mPasswordExpirationTimeout != DEF_PASSWORD_EXPIRATION_TIMEOUT) {
        out->WriteStartTag(String(), String("password-expiration-timeout"));
        out->WriteAttribute(String(), String("value"), StringUtils::Int64ToString(mPasswordExpirationTimeout));
        out->WriteEndTag(String(), String("password-expiration-timeout"));
    }

    if (mPasswordExpirationDate != DEF_PASSWORD_EXPIRATION_DATE) {
        out->WriteStartTag(String(), String("password-expiration-date"));
        out->WriteAttribute(String(), String("value"), StringUtils::Int64ToString(mPasswordExpirationDate));
        out->WriteEndTag(String(), String("password-expiration-date"));
    }

    if (mEncryptionRequested) {
        out->WriteStartTag(String(), String("encryption-requested"));
        out->WriteAttribute(String(), String("value"), StringUtils::BooleanToString(mEncryptionRequested));
        out->WriteEndTag(String(), String("encryption-requested"));
    }

    if (mDisableCamera) {
        out->WriteStartTag(String(), String("disable-camera"));
        out->WriteAttribute(String(), String("value"), StringUtils::BooleanToString(mDisableCamera));
        out->WriteEndTag(String(), String("disable-camera"));
    }

    if (mDisabledKeyguardFeatures != DEF_KEYGUARD_FEATURES_DISABLED) {
        out->WriteStartTag(String(), String("disable-keyguard-features"));
        out->WriteAttribute(String(), String("value"), StringUtils::Int32ToString(mDisabledKeyguardFeatures));
        out->WriteEndTag(String(), String("disable-keyguard-features"));
    }
}

ECode ActiveAdmin::ReadFromXml(
    /* [in] */ IXmlPullParser* parser)
{
    Int32 outerDepth;
    parser->GetDepth(&outerDepth);

    ECode ec;
    while (true) {
        Int32 type;
        ec = parser->Next(&type);
        if (FAILED(ec)) {
            return E_XML_PULL_PARSER_EXCEPTION;
        }

        Int32 currentDepth;
        parser->GetDepth(&currentDepth);

        if (type == IXmlPullParser::END_DOCUMENT) {
            return NOERROR;
        }

        if ((type == IXmlPullParser::END_TAG && currentDepth <= outerDepth)) {
            return NOERROR;
        }

        if (type == IXmlPullParser::END_TAG || type == IXmlPullParser::TEXT) {
            continue;
        }

        String tag;
        parser->GetName(&tag);

        if (tag.Equals("policies")) {
            ec = mInfo->ReadPoliciesFromXml(parser);

            if (FAILED(ec)) {
                return E_XML_PULL_PARSER_EXCEPTION;
            }
        } else if (tag.Equals("password-quality")) {
            String val;
            parser->GetAttributeValue(String(), "value", &val);
            mPasswordQuality = StringUtils::ParseInt32(val);
        } else if (tag.Equals("min-password-length")) {
            String val;
            parser->GetAttributeValue(String(), "value", &val);
            mMinimumPasswordLength = StringUtils::ParseInt32(val);
        } else if (tag.Equals("password-history-length")) {
            String val;
            parser->GetAttributeValue(String(), "value", &val);
            mPasswordHistoryLength = StringUtils::ParseInt32(val);
        } else if (tag.Equals("min-password-uppercase")) {
            String val;
            parser->GetAttributeValue(String(), "value", &val);
            mMinimumPasswordUpperCase = StringUtils::ParseInt32(val);
        } else if (tag.Equals("min-password-lowercase")) {
            String val;
            parser->GetAttributeValue(String(), "value", &val);
            mMinimumPasswordLowerCase = StringUtils::ParseInt32(val);
        } else if (tag.Equals("min-password-letters")) {
            String val;
            parser->GetAttributeValue(String(), "value", &val);
            mMinimumPasswordLetters = StringUtils::ParseInt32(val);
        } else if (tag.Equals("min-password-numeric")) {
            String val;
            parser->GetAttributeValue(String(), "value", &val);
            mMinimumPasswordNumeric = StringUtils::ParseInt32(val);
        } else if (tag.Equals("min-password-symbols")) {
            String val;
            parser->GetAttributeValue(String(), "value", &val);
            mMinimumPasswordSymbols = StringUtils::ParseInt32(val);
        } else if (tag.Equals("min-password-nonletter")) {
            String val;
            parser->GetAttributeValue(String(), "value", &val);
            mMinimumPasswordNonLetter = StringUtils::ParseInt32(val);
        } else if (tag.Equals("max-time-to-unlock")) {
            String val;
            parser->GetAttributeValue(String(), "value", &val);
            mMaximumTimeToUnlock = StringUtils::ParseInt64(val);
        } else if (tag.Equals("max-failed-password-wipe")) {
            String val;
            parser->GetAttributeValue(String(), "value", &val);
            mMaximumFailedPasswordsForWipe = StringUtils::ParseInt32(val);
        } else if (tag.Equals("specifies-global-proxy")) {
            String val;
            parser->GetAttributeValue(String(), "value", &val);

            Int32 result = StringUtils::ParseInt32(val);
            mSpecifiesGlobalProxy = (result != 0) ? TRUE : FALSE;
        } else if (tag.Equals("global-proxy-spec")) {
            String val;
            parser->GetAttributeValue(String(), "value", &val);
            mGlobalProxySpec = val;
        } else if (tag.Equals("global-proxy-exclusion-list")) {
            String val;
            parser->GetAttributeValue(String(), "value", &val);
            mGlobalProxyExclusionList = val;
        } else if (tag.Equals("password-expiration-timeout")) {
            String val;
            parser->GetAttributeValue(String(), "value", &val);
            mPasswordExpirationTimeout = StringUtils::ParseInt64(val);
        } else if (tag.Equals("password-expiration-date")) {
            String val;
            parser->GetAttributeValue(String(), "value", &val);
            mPasswordExpirationDate = StringUtils::ParseInt64(val);
        } else if (tag.Equals("encryption-requested")) {
            String val;
            parser->GetAttributeValue(String(), "value", &val);

            Int32 result = StringUtils::ParseInt32(val);
            mEncryptionRequested = (result != 0) ? TRUE : FALSE;
        } else if (tag.Equals("disable-camera")) {
            String val;
            parser->GetAttributeValue(String(), "value", &val);

            Int32 result = StringUtils::ParseInt32(val);
            mDisableCamera = (result != 0) ? TRUE : FALSE;
        } else if (tag.Equals("disable-keyguard-features")) {
            String val;
            parser->GetAttributeValue(String(), "value", &val);
            mDisabledKeyguardFeatures = StringUtils::ParseInt32(val);
        } else {
            StringBuffer buf;
            buf += "Unknown admin tag: ";
            buf += tag;
            Logger::W("DevicePolicyManagerService", (String)buf);
        }

        XmlUtils::SkipCurrentTag(parser);
    }

    return NOERROR;
}

void ActiveAdmin::SetDeviceAdminInfo(
    /* [in] */ AutoPtr<IDeviceAdminInfo> info)
{
    mInfo = info;
}

AutoPtr<IDeviceAdminInfo> ActiveAdmin::GetDeviceAdminInfo()
{
    return mInfo;
}

void ActiveAdmin::SetPasswordQuality(
    /* [in] */ Int32 quality)
{
    mPasswordQuality = quality;
}

Int32 ActiveAdmin::GetPasswordQuality()
{
    return mPasswordQuality;
}

void ActiveAdmin::SetMinPasswordLength(
    /* [in] */ Int32 length)
{
    mMinimumPasswordLength = length;
}

Int32 ActiveAdmin::GetMinPasswordLength()
{
    return mMinimumPasswordLength;
}

void ActiveAdmin::SetPasswordHistoryLength(
    /* [in] */ Int32 length)
{
    mPasswordHistoryLength = length;
}

Int32 ActiveAdmin::GetPasswordHistoryLength()
{
    return mPasswordHistoryLength;
}

void ActiveAdmin::SetMinPasswordUpperCase(
    /* [in] */ Int32 upperCase)
{
    mMinimumPasswordUpperCase = upperCase;
}

Int32 ActiveAdmin::GetMinPasswordUpperCase()
{
    return mMinimumPasswordUpperCase;
}

void ActiveAdmin::SetMinPasswordLowerCase(
    /* [in] */ Int32 lowerCase)
{
    mMinimumPasswordLowerCase = lowerCase;
}

Int32 ActiveAdmin::GetMinPasswordLowerCase()
{
    return mMinimumPasswordLowerCase;
}

void ActiveAdmin::SetMinPasswordLetters(
    /* [in] */ Int32 letters)
{
    mMinimumPasswordLetters = letters;
}

Int32 ActiveAdmin::GetMinPasswordLetters()
{
    return mMinimumPasswordLetters;
}

void ActiveAdmin::SetMinPasswordNumeric(
    /* [in] */ Int32 numeric)
{
    mMinimumPasswordNumeric = numeric;
}

Int32 ActiveAdmin::GetMinPasswordNumeric()
{
    return mMinimumPasswordNumeric;
}

void ActiveAdmin::SetMinPasswordSymbols(
    /* [in] */ Int32 symbols)
{
    mMinimumPasswordSymbols = symbols;
}

Int32 ActiveAdmin::GetMinPasswordSymbols()
{
    return mMinimumPasswordSymbols;
}

void ActiveAdmin::SetMinPasswordNonLetter(
    /* [in] */ Int32 nonLetter)
{
    mMinimumPasswordNonLetter = nonLetetr;
}

Int32 ActiveAdmin::GetMinPasswordNonLetter()
{
    return mMinimumPasswordNonLetter;
}

void ActiveAdmin::SetMaxTimeToUnlock(
    /* [in] */ Int64 timeToUnlock)
{
    mMaximumTimeToUnlock = timeToUnlock;
}

Int64 ActiveAdmin::GetMaxTimeToUnlock()
{
    return mMaximumTimeToUnlock;
}

void ActiveAdmin::SetMaxFailedPasswordsForWipe(
    /* [in] */ Int32 pwdForWipe)
{
    mMaximumFailedPasswordsForWipe = pwdForWipe;
}

Int32 ActiveAdmin::GetMaxFailedPasswordsForWipe()
{
    return mMaximumFailedPasswordsForWipe;
}

void ActiveAdmin::SetPasswordExpirationTimeout(
    /* [in] */ Int64 timeout)
{
    mPasswordExpirationTimeout = timeout;
}

Int64 ActiveAdmin::GetPasswordExpirationTimeout()
{
    return mPasswordExpirationTimeout;
}

void ActiveAdmin::SetPasswordExpirationDate(
    /* [in] */ Int64 date)
{
    mPasswordExpirationDate = date;
}

Int64 ActiveAdmin::GetPasswordExpirationDate()
{
    return mPasswordExpirationDate;
}

void ActiveAdmin::SetDisabledKeyguardFeatures(
    /* [in] */ Int32 features)
{
    mDisabledKeyguardFeatures = features;
}

Int32 ActiveAdmin::GetDisabledKeyguardFeatures()
{
    return mDisabledKeyguardFeatures;
}

void ActiveAdmin::SetEncryptionRequested(
    /* [in] */ Boolean requested)
{
    mEncryptionRequested = requested;
}

Boolean ActiveAdmin::IsEncryptionRequested()
{
    return mEncryptionRequested;
}

void ActiveAdmin::SetCameraDisabled(
    /* [in] */ Boolean disabled)
{
    mDisableCamera = disabled;
}

Boolean ActiveAdmin::IsCameraDisabled()
{
    return mDisableCamera;
}

void ActiveAdmin::SpecifiesGlobalProxy(
    /* [in] */ Boolean globalProxy)
{
    mSpecifiesGlobalProxy = globalProxy;
}

Boolean ActiveAdmin::IsSpecifiedGlobalProxy()
{
    return mSpecifiesGlobalProxy;
}

void ActiveAdmin::SetGlobalProxySpec(
    /* [in] */ const String& spec)
{
    mGlobalProxySpec = spec;
}

String ActiveAdmin::GetGlobalProxySpec()
{
    return mGlobalProxySpec;
}

void ActiveAdmin::SetGlobalProxyExclusionList(
    /* [in] */ const String& list)
{
    mGlobalProxyExclusionList = list;
}

String ActiveAdmin::GetGlobalProxyExclusionList()
{
    return mGlobalProxyExclusionList;
}

Boolean ActiveAdmin::Equals(
    /* [in] */ ActiveAdmin* other)
{
    if (mPasswordQuality != other->GetPasswordQuality()) {
        return FALSE;
    }

    if (mMinimumPasswordLength != other->GetMinPasswordLength()) {
        return FALSE;
    }

    if (mPasswordHistoryLength != other->GetPasswordHistoryLength()) {
        return FALSE;
    }

    if (mMinimumPasswordUpperCase != other->GetMinPasswordUpperCase()) {
        return FALSE;
    }

    if (mMinimumPasswordLowerCase != other->GetMinPasswordLowerCase()) {
        return FALSE;
    }

    if (mMinimumPasswordLetters != other->GetMinPasswordLetters()) {
        return FALSE;
    }

    if (mMinimumPasswordNumeric != other->GetMinPasswordNumeric()) {
        return FALSE;
    }

    if (mMinimumPasswordSymbols != other->GetMinPasswordSymbols()) {
        return FALSE;
    }

    if (mMinimumPasswordNonLetter != other->GetMinPasswordNonLetter()) {
        return FALSE;
    }

    if (mMaximumTimeToUnlock != other->GetMaxTimeToUnlock()) {
        return FALSE;
    }

    if (mMaximumFailedPasswordsForWipe != other->GetMaxFailedPasswordsForWipe()) {
        return FALSE;
    }

    if (mPasswordExpirationTimeout != other->GetPasswordExpirationTimeout()) {
        return FALSE;
    }

    if (mPasswordExpirationDate != other->GetPasswordExpirationDate()) {
        return FALSE;
    }

    if (mDisabledKeyguardFeatures != other->GetDisabledKeyguardFeatures()) {
        return FALSE;
    }

    if (mEncryptionRequested != other->IsEncryptionRequested()) {
        return FALSE;
    }

    if (mDisableCamera != other->IsCameraDisabled()) {
        return FALSE;
    }

    if (mSpecifiesGlobalProxy != other->IsSpecifiedGlobalProxy()) {
        return FALSE;
    }

    if (!mGlobalProxySpec.Equals(other->GetGlobalProxySpec())) {
        return FALSE;
    }

    if (!mGlobalProxyExclusionList.Equals(other->GetGlobalProxyExclusionList())) {
        return FALSE;
    }

    return TRUE;
}

}// Server
}// Droid
}// Elastos
