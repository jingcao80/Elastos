
#ifndef __ELASTOS_DROID_PRIVACY_CPRIVACYSETTINGSMANAGER_H__
#define __ELASTOS_DROID_PRIVACY_CPRIVACYSETTINGSMANAGER_H__

#include "_Elastos_Droid_Privacy_CPrivacySettingsManager.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Privacy::IIPrivacySettingsManager;

namespace Elastos {
namespace Droid {
namespace Privacy {

CarClass(CPrivacySettingsManager)
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetSettings(
        /* [in] */ const String& packageName,
        /* [out] */ Elastos::Droid::Privacy::IPrivacySettings** privactySettings);

    CARAPI GetSettings(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 uid,
        /* [out] */ Elastos::Droid::Privacy::IPrivacySettings** privactySettings);

    CARAPI SaveSettings(
        /* [in] */ Elastos::Droid::Privacy::IPrivacySettings* settings,
        /* [out] */ Boolean* result);

    CARAPI DeleteSettings(
        /* [in] */ const String& packageName,
        /* [out] */ Boolean* result);

    CARAPI DeleteSettings(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 uid,
        /* [out] */ Boolean* result);

    CARAPI IsServiceAvailable(
        /* [out] */ Boolean* isServiceAvailable);

    CARAPI Notification(
        /* [in] */ const String& packageName,
        /* [in] */ Byte accessMode,
        /* [in] */ const String& dataType,
        /* [in] */ const String& output);

    CARAPI Notification(
        /* [in] */ const String& packageName,
        /* [in] */ Byte accessMode,
        /* [in] */ const String& dataType,
        /* [in] */ const String& output,
        /* [in] */ Elastos::Droid::Privacy::IPrivacySettings* set);

    CARAPI Notification(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 uid,
        /* [in] */ Byte accessMode,
        /* [in] */ const String& dataType,
        /* [in] */ const String& output,
        /* [in] */ Elastos::Droid::Privacy::IPrivacySettings* set);

    CARAPI RegisterObservers();

    CARAPI AddObserver(
        /* [in] */ const String& packageName);

    CARAPI PurgeSettings(
        /* [out] */ Boolean* result);

    CARAPI GetVersion(
        /* [out] */ Double* result);

    CARAPI GetApiVersion(
        /* [out] */ Double* apiVersion);

    CARAPI GetModVersion(
        /* [out] */ Double* modVersion);

    CARAPI GetModDetails(
        /* [out] */ String* modDetails);

    CARAPI SetEnabled(
        /* [in] */ Boolean isEnable,
        /* [out] */ Boolean* result);

    CARAPI SetNotificationsEnabled(
        /* [in] */ Boolean isEnable,
        /* [out] */ Boolean* result);

    CARAPI SetBootCompleted();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IIPrivacySettingsManager* service);

private:
    static const String TAG;// = "PrivacySettingsManager";
    AutoPtr<IIPrivacySettingsManager> mService;

    static const Double API_VERSION;// = 1.51;
    static const Double MOD_VERSION;// = 1.0;
    static const String MOD_DETAILS;
public:
    static const String ACTION_PRIVACY_NOTIFICATION;// = "com.privacy.pdroid.PRIVACY_NOTIFICATION";

    static const String ACTION_PRIVACY_NOTIFICATION_ADDON;// = "com.privacy.pdroid.PRIVACY_NOTIFICATION_ADDON";


};

}
}
}

#endif // __ELASTOS_DROID_PRIVACY_CPRIVACYSETTINGSMANAGER_H__
