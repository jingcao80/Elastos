#ifndef __ELASTOS_DROID_SETTINGS_FUELGAUGE_CPOWERUSAGESUMMARY_H__
#define __ELASTOS_DROID_SETTINGS_FUELGAUGE_CPOWERUSAGESUMMARY_H__

#include "_Elastos_Droid_Settings_Fuelgauge_CPowerUsageSummary.h"
#include "elastos/droid/preference/PreferenceFragment.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Internal::Os::IBatteryStatsHelper;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::PreferenceFragment;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IMenuItem;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Fuelgauge {

/**
 * Displays a list of apps and subsystems that consume power, ordered by how much power was
 * consumed since the last time it was unplugged.
 */
CarClass(CPowerUsageSummary)
    , public PreferenceFragment
{
private:
    class MyBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("CPowerUsageSummary::MyBroadcastReceiver")

        MyBroadcastReceiver(
            /* [in] */ CPowerUsageSummary* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        CPowerUsageSummary* mHost;
    };

    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CPowerUsageSummary::MyHandler")

        MyHandler(
            /* [in] */ CPowerUsageSummary* host);

        CARAPI constructor();

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CPowerUsageSummary* mHost;
    };

public:
    TO_STRING_IMPL("CPowerUsageSummary")

    CAR_OBJECT_DECL()

    CPowerUsageSummary();

    ~CPowerUsageSummary();

    CARAPI constructor();

    //@Override
    CARAPI OnAttach(
        /* [in] */ IActivity* activity);

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    //@Override
    CARAPI OnStart();

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnPause();

    //@Override
    CARAPI OnStop();

    //@Override
    CARAPI OnDestroy();

    //@Override
    CARAPI OnPreferenceTreeClick(
        /* [in] */ IPreferenceScreen* preferenceScreen,
        /* [in] */ IPreference* preference,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnCreateOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [in] */ IMenuInflater* inflater);

    //@Override
    CARAPI OnOptionsItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result);

private:
    CARAPI_(void) AddNotAvailableMessage();

    CARAPI_(Boolean) UpdateBatteryStatus(
        /* [in] */ IIntent* intent);

    CARAPI_(void) RefreshStats();

public:
    static const String TAG;

private:
    static const Boolean DEBUG;

    static const String KEY_APP_LIST;

    static const String BATTERY_HISTORY_FILE;

    static const Int32 MENU_STATS_TYPE;
    static const Int32 MENU_STATS_REFRESH;
    static const Int32 MENU_BATTERY_SAVER;
    static const Int32 MENU_HELP;

    AutoPtr<IUserManager> mUm;

    AutoPtr<IBatteryHistoryPreference> mHistPref;
    AutoPtr<IPreferenceGroup> mAppListGroup;
    String mBatteryLevel;
    String mBatteryStatus;

    Int32 mStatsType;

    static const Int32 MIN_POWER_THRESHOLD_MILLI_AMP;
    static const Int32 MAX_ITEMS_TO_LIST;
    static const Int32 MIN_AVERAGE_POWER_THRESHOLD_MILLI_AMP;
    static const Int32 SECONDS_IN_HOUR;

    AutoPtr<IBatteryStatsHelper> mStatsHelper;

    AutoPtr<MyBroadcastReceiver> mBatteryInfoReceiver;

    static const Int32 MSG_REFRESH_STATS = 100;

    AutoPtr<MyHandler> mHandler;
};

} // namespace Fuelgauge
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_FUELGAUGE_CPOWERUSAGESUMMARY_H__