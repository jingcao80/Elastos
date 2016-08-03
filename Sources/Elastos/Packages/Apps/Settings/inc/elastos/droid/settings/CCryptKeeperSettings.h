#ifndef __ELASTOS_DROID_SETTINGS_CCRYPTKEEPERSETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_CCRYPTKEEPERSETTINGS_H__

#include "Elastos.Droid.Widget.h"
#include "_Elastos_Droid_Settings_CCryptKeeperSettings.h"
#include "elastos/droid/app/Fragment.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::App::Fragment;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::IButton;

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CCryptKeeperSettings)
    , public Fragment
{
private:
    class InitBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        InitBroadcastReceiver(
            /* [in] */ CCryptKeeperSettings* host);

        ~InitBroadcastReceiver();

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        CCryptKeeperSettings* mHost;
    };

    class InitButtonOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        InitButtonOnClickListener(
            /* [in] */ CCryptKeeperSettings* host);

        ~InitButtonOnClickListener();

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CCryptKeeperSettings* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CCryptKeeperSettings();

    ~CCryptKeeperSettings();

    CARAPI constructor();

    //@Override
    CARAPI OnCreateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container,
        /* [in] */ IBundle* savedState,
        /* [out] */ IView** result);

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnPause();

    /**
     * If encryption is already started, and this launched via a "start encryption" intent,
     * then exit immediately - it's already up and running, so there's no point in "starting" it.
     */
    //@Override
    CARAPI OnActivityCreated(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* data);

private:
    /**
     * Keyguard validation is run using the standard {@link ConfirmLockPattern}
     * component as a subactivity
     * @param request the request code to be returned once confirmation finishes
     * @return TRUE if confirmation launched
     */
    CARAPI_(Boolean) RunKeyguardConfirmation(
        /* [in] */ Int32 request);

    CARAPI_(void) ShowFinalConfirmation(
        /* [in] */ Int32 type,
        /* [in] */ const String& password);

private:
    static const String TAG;

    static const Int32 KEYGUARD_REQUEST;

    // Minimum battery charge level (in percent) to launch encryption.  If the battery charge is
    // lower than this, encryption should not be activated.
    static const Int32 MIN_BATTERY_LEVEL;

    AutoPtr<IView> mContentView;
    AutoPtr<IButton> mInitiateButton;
    AutoPtr<IView> mPowerWarning;
    AutoPtr<IView> mBatteryWarning;
    AutoPtr<IIntentFilter> mIntentFilter;

    AutoPtr<IBroadcastReceiver> mIntentReceiver;

    /**
     * If the user clicks to begin the reset sequence, we next require a
     * keyguard confirmation if the user has currently enabled one.  If there
     * is no keyguard available, we prompt the user to set a password.
     */
    AutoPtr<InitButtonOnClickListener> mInitiateListener;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CCRYPTKEEPERSETTINGS_H__