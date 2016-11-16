
#ifndef __ELASTOS_DROID_SETTINGS_NOTIFICATION_CAPPNOTIFICATIONSETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_NOTIFICATION_CAPPNOTIFICATIONSETTINGS_H__

#include "_Elastos_Droid_Settings_Notification_CAppNotificationSettings.h"
#include "elastos/droid/settings/notification/CNotificationAppList.h"
#include "elastos/droid/settings/SettingsPreferenceFragment.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Droid::Settings::SettingsPreferenceFragment;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Preference::ISwitchPreference;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Notification {

/** These settings are per app, so should not be returned in global search results. */
CarClass(CAppNotificationSettings)
    , public SettingsPreferenceFragment
{
private:
    class MyOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        TO_STRING_IMPL("CAppNotificationSettings::MyOnClickListener")

        CAR_INTERFACE_DECL()

        MyOnClickListener(
            /* [in] */ CAppNotificationSettings* host);

        //@Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CAppNotificationSettings* mHost;
    };

    class MyOnPreferenceChangeListener
        : public Object
        , public IPreferenceOnPreferenceChangeListener
    {
    public:
        TO_STRING_IMPL("CAppNotificationSettings::MyOnPreferenceChangeListener")

        CAR_INTERFACE_DECL()

        MyOnPreferenceChangeListener(
            /* [in] */ CAppNotificationSettings* host,
            /* [in] */ Int32 id,
            /* [in] */ const String& pkg,
            /* [in] */ Int32 uid);

        //@Override
        CARAPI OnPreferenceChange(
            /* [in] */ IPreference* preference,
            /* [in] */ IInterface* newValue,
            /* [out] */ Boolean* res);

    private:
        CAppNotificationSettings* mHost;
        Int32 mId;
        String mPkg;
        Int32 mUid;
    };

public:
    CAR_OBJECT_DECL()

    CAppNotificationSettings();

    ~CAppNotificationSettings();

    CARAPI constructor();

    //@Override
    CARAPI OnActivityCreated(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

private:
    CARAPI_(Boolean) GetLockscreenNotificationsEnabled();

    CARAPI_(Boolean) GetLockscreenAllowPrivateNotifications();

    CARAPI_(void) ToastAndFinish();

    static CARAPI_(AutoPtr<IPackageInfo>) FindPackageInfo(
        /* [in] */ IPackageManager* pm,
        /* [in] */ const String& pkg,
        /* [in] */ Int32 uid);

public:
    static const String EXTRA_HAS_SETTINGS_INTENT;
    static const String EXTRA_SETTINGS_INTENT;

private:
    static const String TAG;
    static const Boolean DEBUG;

    static const String KEY_BLOCK;
    static const String KEY_PRIORITY;
    static const String KEY_SENSITIVE;

    AutoPtr<CNotificationAppList::Backend> mBackend;

    AutoPtr<IContext> mContext;
    AutoPtr<ISwitchPreference> mBlock;
    AutoPtr<ISwitchPreference> mPriority;
    AutoPtr<ISwitchPreference> mSensitive;
    AutoPtr<CNotificationAppList::AppRow> mAppRow;
    Boolean mCreated;
};

} // namespace Applications
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_NOTIFICATION_CAPPNOTIFICATIONSETTINGS_H__