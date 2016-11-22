#ifndef __ELASTOS_DROID_SETTINGS_LOCATION_LOCATIONSETTINGSBASE_H__
#define __ELASTOS_DROID_SETTINGS_LOCATION_LOCATIONSETTINGSBASE_H__

#include "elastos/droid/settings/SettingsPreferenceFragment.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::Settings::SettingsPreferenceFragment;

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Location {

/**
 * A base class that listens to location settings change and modifies location
 * settings.
 */
class LocationSettingsBase
    : public SettingsPreferenceFragment
{
private:
    class MyBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("LocationSettingsBase::MyBroadcastReceiver")

        MyBroadcastReceiver(
            /* [in] */ LocationSettingsBase* host);

        ~MyBroadcastReceiver();

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        LocationSettingsBase* mHost;
    };

public:
    TO_STRING_IMPL("LocationSettingsBase")

    LocationSettingsBase();

    ~LocationSettingsBase();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnPause();

    /** Called when location mode has changed. */
    virtual CARAPI OnModeChanged(
        /* [in] */ Int32 mode,
        /* [in] */ Boolean restricted) = 0;

    virtual CARAPI SetLocationMode(
        /* [in] */ Int32 mode);

    virtual CARAPI RefreshLocationMode();

private:
    CARAPI_(Boolean) IsRestricted();

private:
    static const String TAG;
    /** Broadcast intent action when the location mode is about to change. */
    static const String MODE_CHANGING_ACTION;
    static const String CURRENT_MODE_KEY;
    static const String NEW_MODE_KEY;

    Int32 mCurrentMode;
    AutoPtr<BroadcastReceiver> mReceiver;

    /**
     * Whether the fragment is actively running.
     */
    Boolean mActive;
};

} // namespace Location
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_LOCATION_LOCATIONSETTINGSBASE_H__