#ifndef __ELASTOS_DROID_SERVER_ACTIVEADMIN_H__
#define __ELASTOS_DROID_SERVER_ACTIVEADMIN_H__

namespace Elastos {
namespace Droid {
namespace Server {

class ActiveAdmin : public ElRefBase
{
public:
    ActiveAdmin(
        /* [in] */ IDeviceAdminInfo* info);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface *pObject,
        /* [out] */ InterfaceID *pIID);

    CARAPI_(Int32) GetUid();

    CARAPI_(AutoPtr<IUserHandle>) GetUserHandle();

    CARAPI_(void) WriteToXml(
        /* [in] */ IXmlSerializer* out);

    CARAPI ReadFromXml(
        /* [in] */ IXmlPullParser* parser);

    CARAPI_(void) SetDeviceAdminInfo(
        /* [in] */ AutoPtr<IDeviceAdminInfo> info);

    CARAPI_(AutoPtr<IDeviceAdminInfo>) GetDeviceAdminInfo();

    CARAPI_(void) SetPasswordQuality(
        /* [in] */ Int32 quality);

    CARAPI_(Int32) GetPasswordQuality();

    CARAPI_(void) SetMinPasswordLength(
        /* [in] */ Int32 length);

    CARAPI_(Int32) GetMinPasswordLength();

    CARAPI_(void) SetPasswordHistoryLength(
        /* [in] */ Int32 length);

    CARAPI_(Int32) GetPasswordHistoryLength();

    CARAPI_(void) SetMinPasswordUpperCase(
        /* [in] */ Int32 upperCase);

    CARAPI_(Int32) GetMinPasswordUpperCase();

    CARAPI_(void) SetMinPasswordLowerCase(
        /* [in] */ Int32 lowerCase);

    CARAPI_(Int32) GetMinPasswordLowerCase();

    CARAPI_(void) SetMinPasswordLetters(
        /* [in] */ Int32 letters);

    CARAPI_(Int32) GetMinPasswordLetters();

    CARAPI_(void) SetMinPasswordNumeric(
        /* [in] */ Int32 numeric);

    CARAPI_(Int32) GetMinPasswordNumeric();

    CARAPI_(void) SetMinPasswordSymbols(
        /* [in] */ Int32 symbols);

    CARAPI_(Int32) GetMinPasswordSymbols();

    CARAPI_(void) SetMinPasswordNonLetter(
        /* [in] */ Int32 nonLetter);

    CARAPI_(Int32) GetMinPasswordNonLetter();

    CARAPI_(void) SetMaxTimeToUnlock(
        /* [in] */ Int64 timeToUnlock);

    CARAPI_(Int64) GetMaxTimeToUnlock();

    CARAPI_(void) SetMaxFailedPasswordsForWipe(
        /* [in] */ Int32 pwdForWipe);

    CARAPI_(Int32) GetMaxFailedPasswordsForWipe();

    CARAPI_(void) SetPasswordExpirationTimeout(
        /* [in] */ Int64 timeout);

    CARAPI_(Int64) GetPasswordExpirationTimeout();

    CARAPI_(void) SetPasswordExpirationDate(
        /* [in] */ Int64 date);

    CARAPI_(Int64) GetPasswordExpirationDate();

    CARAPI_(void) SetDisabledKeyguardFeatures(
        /* [in] */ Int32 features);

    CARAPI_(Int32) GetDisabledKeyguardFeatures();

    CARAPI_(void) SetEncryptionRequested(
        /* [in] */ Boolean requested);

    CARAPI_(Boolean) IsEncryptionRequested();

    CARAPI_(void) SetCameraDisabled(
        /* [in] */ Boolean disabled);

    CARAPI_(Boolean) IsCameraDisabled();

    CARAPI_(void) SpecifiesGlobalProxy(
        /* [in] */ Boolean globalProxy);

    CARAPI_(Boolean) IsSpecifiedGlobalProxy();

    CARAPI_(void) SetGlobalProxySpec(
        /* [in] */ const String& spec);

    CARAPI_(String) GetGlobalProxySpec();

    CARAPI_(void) SetGlobalProxyExclusionList(
        /* [in] */ const String& list);

    CARAPI_(String) GetGlobalProxyExclusionList();

    CARAPI_(Boolean) Equals(
        /* [in] */ ActiveAdmin* other);

public:
    static const Int32 DEF_MINIMUM_PASSWORD_LENGTH;

    static const Int32 DEF_PASSWORD_HISTORY_LENGTH;

    static const Int32 DEF_MINIMUM_PASSWORD_UPPER_CASE;

    static const Int32 DEF_MINIMUM_PASSWORD_LOWER_CASE;

    static const Int32 DEF_MINIMUM_PASSWORD_LETTERS;

    static const Int32 DEF_MINIMUM_PASSWORD_NUMERIC;

    static const Int32 DEF_MINIMUM_PASSWORD_SYMBOLS;

    static const Int32 DEF_MINIMUM_PASSWORD_NON_LETTER;

    static const Int64 DEF_MAXIMUM_TIME_TO_UNLOCK;

    static const Int32 DEF_MAXIMUM_FAILED_PASSWORDS_FOR_WIPE;

    static const Int64 DEF_PASSWORD_EXPIRATION_TIMEOUT;

    static const Int64 DEF_PASSWORD_EXPIRATION_DATE;

    static const Int32 DEF_KEYGUARD_FEATURES_DISABLED;

private:
    AutoPtr<IDeviceAdminInfo> mInfo;

    Int32 mPasswordQuality;
    Int32 mMinimumPasswordLength;
    Int32 mPasswordHistoryLength;
    Int32 mMinimumPasswordUpperCase;
    Int32 mMinimumPasswordLowerCase;
    Int32 mMinimumPasswordLetters;
    Int32 mMinimumPasswordNumeric;
    Int32 mMinimumPasswordSymbols;
    Int32 mMinimumPasswordNonLetter;
    Int64 mMaximumTimeToUnlock;
    Int32 mMaximumFailedPasswordsForWipe;
    Int64 mPasswordExpirationTimeout;
    Int64 mPasswordExpirationDate;
    Int32 mDisabledKeyguardFeatures;

    Boolean mEncryptionRequested;
    Boolean mDisableCamera;

    // TODO: review implementation decisions with frameworks team
    Boolean mSpecifiesGlobalProxy;
    String mGlobalProxySpec;
    String mGlobalProxyExclusionList;
};

_ELASTOS_NAMESPACE_BEGIN

template<> struct Hash< AutoPtr<ActiveAdmin> >
{
    size_t operator()(AutoPtr<ActiveAdmin> name) const
    {
        assert(name != NULL);
        return (size_t)name.Get();
    }
};

template<> struct EqualTo< AutoPtr<ActiveAdmin> >
{
    Boolean operator()(const AutoPtr<ActiveAdmin>& x,
                       const AutoPtr<ActiveAdmin>& y) const
    {
        return x.Get() == y.Get();
    }
};

_ELASTOS_NAMESPACE_END

}// Server
}// Droid
}// Elastos

#endif //__ELASTOS_DROID_SERVER_ACTIVEADMIN_H__
