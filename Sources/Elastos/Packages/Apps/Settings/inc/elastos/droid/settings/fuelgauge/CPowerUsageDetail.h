#ifndef __ELASTOS_DROID_SETTINGS_FUELGAUGE_CPOWERUSAGEDETAIL_H__
#define __ELASTOS_DROID_SETTINGS_FUELGAUGE_CPOWERUSAGEDETAIL_H__

#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Widget.h"
#include "_Elastos_Droid_Settings_Fuelgauge_CPowerUsageDetail.h"
#include "elastos/droid/settings/fuelgauge/BatteryEntry.h"
#include "elastos/droid/settings/CSettingsActivity.h"
#include "elastos/droid/app/Fragment.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::Settings::CSettingsActivity;

using Elastos::Droid::App::Fragment;
using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Internal::Os::IBatteryStatsHelper;
using Elastos::Droid::Internal::Os::BatterySipperDrainType;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Fuelgauge {

CarClass(CPowerUsageDetail)
    , public Fragment
{
public:
    class InnerListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        TO_STRING_IMPL("CPowerUsageDetail::InnerListener")

        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ CPowerUsageDetail* host);

        //@Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CPowerUsageDetail* mHost;
    };

private:
    class MyBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("CPowerUsageDetail::MyBroadcastReceiver")

        MyBroadcastReceiver(
            /* [in] */ CPowerUsageDetail* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        CPowerUsageDetail* mHost;
    };

public:
    TO_STRING_IMPL("CPowerUsageDetail")

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CPowerUsageDetail();

    virtual ~CPowerUsageDetail();

    CARAPI constructor();

    static CARAPI_(void) StartBatteryDetailPage(
        /* [in] */ CSettingsActivity* caller,
        /* [in] */ IBatteryStatsHelper* helper,
        /* [in] */ Int32 statsType,
        /* [in] */ BatteryEntry* entry,
        /* [in] */ Boolean showLocationButton);

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    //@Override
    CARAPI OnCreateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container,
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IView** result);

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnPause();

    CARAPI OnClick(
        /* [in] */ IView* v);

private:
    CARAPI_(void) CreateDetails();

    // utility method used to start sub activity
    CARAPI_(void) StartApplicationDetailsActivity();

    CARAPI_(void) DoAction(
        /* [in] */ Int32 action);

    CARAPI_(void) FillDetailsSection();

    CARAPI_(void) FillControlsSection(
        /* [in] */ Int32 uid);

    CARAPI_(void) AddControl(
        /* [in] */ Int32 title,
        /* [in] */ Int32 summary,
        /* [in] */ Int32 action);

    CARAPI_(void) FillMessagesSection(
        /* [in] */ Int32 uid);

    CARAPI_(void) AddMessage(
        /* [in] */ Int32 message);

    CARAPI_(void) RemovePackagesSection();

    CARAPI_(void) KillProcesses();

    CARAPI_(void) CheckForceStop();

    CARAPI_(void) ReportBatteryUse();

    CARAPI_(void) FillPackagesSection(
        /* [in] */ Int32 uid);

    CARAPI_(String) GetDescriptionForDrainType();

public:
    static const Int32 ACTION_DISPLAY_SETTINGS;
    static const Int32 ACTION_WIFI_SETTINGS;
    static const Int32 ACTION_BLUETOOTH_SETTINGS;
    static const Int32 ACTION_WIRELESS_SETTINGS;
    static const Int32 ACTION_APP_DETAILS;
    static const Int32 ACTION_LOCATION_SETTINGS;
    static const Int32 ACTION_FORCE_STOP;
    static const Int32 ACTION_REPORT;

    static const Int32 USAGE_SINCE_UNPLUGGED;
    static const Int32 USAGE_SINCE_RESET;

    static const String EXTRA_TITLE;
    static const String EXTRA_PERCENT;
    static const String EXTRA_GAUGE;
    static const String EXTRA_UID;
    static const String EXTRA_USAGE_SINCE;
    static const String EXTRA_USAGE_DURATION;
    static const String EXTRA_REPORT_DETAILS;
    static const String EXTRA_REPORT_CHECKIN_DETAILS;
    static const String EXTRA_DETAIL_TYPES; // Array of usage types (cpu, gps, etc)
    static const String EXTRA_DETAIL_VALUES; // Array of doubles
    static const String EXTRA_DRAIN_TYPE; // DrainType
    static const String EXTRA_ICON_PACKAGE; // String
    static const String EXTRA_NO_COVERAGE;
    static const String EXTRA_ICON_ID; // Int
    static const String EXTRA_SHOW_LOCATION_BUTTON;  // Boolean

    AutoPtr<IApplicationInfo> mApp;
    AutoPtr<IComponentName> mInstaller;

private:
    // Note: Must match the sequence of the DrainType
    static AutoPtr< ArrayOf<Int32> > sDrainTypeDesciptions;

    static const String TAG;

    AutoPtr<IPackageManager> mPm;
    AutoPtr<IDevicePolicyManager> mDpm;
    String mTitle;
    Int32 mUsageSince;
    AutoPtr< ArrayOf<Int32> > mTypes;
    Int32 mUid;
    AutoPtr< ArrayOf<Double> > mValues;
    AutoPtr<IView> mRootView;
    AutoPtr<ITextView> mTitleView;
    AutoPtr<IViewGroup> mTwoButtonsPanel;
    AutoPtr<IButton> mForceStopButton;
    AutoPtr<IButton> mReportButton;
    AutoPtr<IViewGroup> mDetailsParent;
    AutoPtr<IViewGroup> mControlsParent;
    AutoPtr<IViewGroup> mMessagesParent;
    Int64 mStartTime;
    BatterySipperDrainType mDrainType;
    AutoPtr<IDrawable> mAppIcon;
    Double mNoCoverage; // Percentage of time that there was no coverage

    Boolean mUsesGps;
    Boolean mShowLocationButton;

    AutoPtr< ArrayOf<String> > mPackages;

    AutoPtr<MyBroadcastReceiver> mCheckKillProcessesReceiver;
};

} // namespace Fuelgauge
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_FUELGAUGE_CPOWERUSAGEDETAIL_H__