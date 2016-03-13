
#ifndef __ELASTOS_DROID_ACCOUNTS_CAUTHENTICATORDESCRIPTION_H__
#define __ELASTOS_DROID_ACCOUNTS_CAUTHENTICATORDESCRIPTION_H__

#include "_Elastos_Droid_Accounts_CAuthenticatorDescription.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Accounts {

CarClass(CAuthenticatorDescription)
    , public Object
    , public IAuthenticatorDescription
    , public IParcelable
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CAuthenticatorDescription();

    /**
     * A factory method for creating an AuthenticatorDescription that can be used as a key
     * to identify the authenticator by its type.
     */
    static CARAPI NewKey(
        /* [in] */ const String& type,
        /* [out] */ IAuthenticatorDescription** description);

    CARAPI GetType(
        /* [out] */ String* type);

    CARAPI SetType(
        /* [in] */ const String& type);

    CARAPI GetLabelId(
        /* [out] */ Int32* labelId);

    CARAPI SetLabelId(
        /* [in] */ Int32 labelId);

    CARAPI GetIconId(
        /* [out] */ Int32* iconId);

    CARAPI SetIconId(
        /* [in] */ Int32 iconId);

    CARAPI GetSmallIconId(
        /* [out] */ Int32* smallIconId);

    CARAPI SetSmallIconId(
        /* [in] */ Int32 smallIconId);

    CARAPI GetAccountPreferencesId(
        /* [out] */ Int32* preferencesId);

    CARAPI SetAccountPreferencesId(
        /* [in] */ Int32 preferencesId);

    CARAPI GetPackageName(
        /* [out] */ String* packageName);

    CARAPI SetPackageName(
        /* [in] */ const String& packageName);

    CARAPI GetCustomTokens(
        /* [out] */ Boolean* customTokens);

    CARAPI SetCustomTokens(
        /* [in] */ Boolean customTokens);

    CARAPI ToString(
        /* [out] */ String* s);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI constructor(
        /* [in] */ const String& type,
        /* [in] */ const String& packageName,
        /* [in] */ Int32 labelId,
        /* [in] */ Int32 iconId,
        /* [in] */ Int32 smallIconId,
        /* [in] */ Int32 prefId,
        /* [in] */ Boolean customTokens);

    CARAPI constructor(
        /* [in] */ const String& type,
        /* [in] */ const String& packageName,
        /* [in] */ Int32 labelId,
        /* [in] */ Int32 iconId,
        /* [in] */ Int32 smallIconId,
        /* [in] */ Int32 prefId);

    CARAPI constructor(
        /* [in] */ const String& type);

    CARAPI constructor();

public:
    /** The string that uniquely identifies an authenticator */
    String mType;

    /** A resource id of a label for the authenticator that is suitable for displaying */
    Int32 mLabelId;

    /** A resource id of a icon for the authenticator */
    Int32 mIconId;

    /** A resource id of a smaller icon for the authenticator */
    Int32 mSmallIconId;

    /**
     * A resource id for a hierarchy of PreferenceScreen to be added to the settings page for the
     * account. See {@link AbstractAccountAuthenticator} for an example.
     */
    Int32 mAccountPreferencesId;

    /** The package name that can be used to lookup the resources from above. */
    String mPackageName;

    /** Authenticator handles its own token caching and permission screen */
    Boolean mCustomTokens;

private:
    static const String TAG;
};

} // namespace Accounts
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_ACCOUNTS_CAUTHENTICATORDESCRIPTION_H__
