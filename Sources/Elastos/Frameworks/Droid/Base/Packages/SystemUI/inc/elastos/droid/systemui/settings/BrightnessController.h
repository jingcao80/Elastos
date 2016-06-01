
#ifndef __ELASTOS_DROID_SYSTEMUI_SETTINGS_BRIGHTNESSCONTROLLER_H__
#define __ELASTOS_DROID_SYSTEMUI_SETTINGS_BRIGHTNESSCONTROLLER_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/droid/database/ContentObserver.h>
#include "elastos/droid/systemui/settings/CurrentUserTracker.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IIPowerManager;
using Elastos::Droid::Widget::IImageView;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Settings {

class BrightnessController
    : public Object
    , public IBrightnessController
{
private:
    class ToggleSliderListener
        : public Object
        , public IToggleSliderListener
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("BrightnessController::ToggleSliderListener")

        ToggleSliderListener(
            /* [in] */ BrightnessController* host);

        // @Override
        CARAPI OnInit(
            /* [in] */ IToggleSlider* control);

        // @Override
        CARAPI OnChanged(
            /* [in] */ IToggleSlider* view,
            /* [in] */ Boolean tracking,
            /* [in] */ Boolean automatic,
            /* [in] */ Int32 value);

    private:
        BrightnessController* mHost;
    };

    /** ContentObserver to watch brightness **/
    class BrightnessObserver
        : public ContentObserver
    {
    public:
        TO_STRING_IMPL("BrightnessController::BrightnessObserver")

        BrightnessObserver(
            /* [in] */ IHandler* handler,
            /* [in] */ BrightnessController* host);

        // @Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

        // @Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange,
            /* [in] */ IUri* uri);

        CARAPI StartObserving();

        CARAPI StopObserving();

    private:
        AutoPtr<IUri> BRIGHTNESS_MODE_URI;
        AutoPtr<IUri> BRIGHTNESS_URI;
        AutoPtr<IUri> BRIGHTNESS_ADJ_URI;

        BrightnessController* mHost;
    };

    class MyCurrentUserTracker
        : public CurrentUserTracker
    {
    public:
        MyCurrentUserTracker(
            /* [in] */ IContext* context,
            /* [in] */ BrightnessController* host);

        CARAPI_(void) OnUserSwitched(
            /* [in] */ Int32 newUserId);

    private:
        BrightnessController* mHost;
    };

    class Runnable1
        : public Runnable
    {
    public:
        Runnable1(
            /* [in] */ Int32 val,
            /* [in] */ BrightnessController* host);

        CARAPI Run();

    private:
        Int32 mVal;
        BrightnessController* mHost;
    };

    class Runnable2
        : public Runnable
    {
    public:
        Runnable2(
            /* [in] */ Float adj,
            /* [in] */ BrightnessController* host);

        CARAPI Run();

    private:
        Float mAdj;
        BrightnessController* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    BrightnessController();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IImageView* icon,
        /* [in] */ IToggleSlider* control);

    CARAPI AddStateChangedCallback(
        /* [in] */ IBrightnessStateChangeCallback* cb);

    CARAPI RemoveStateChangedCallback(
        /* [in] */ IBrightnessStateChangeCallback* cb,
        /* [out] */ Boolean* result);

    CARAPI RegisterCallbacks();

    /** Unregister all call backs, both to and from the controller */
    CARAPI UnregisterCallbacks();

private:
    CARAPI_(void) SetMode(
        /* [in] */ Int32 mode);

    CARAPI SetBrightness(
        /* [in] */ Int32 brightness);

    CARAPI SetBrightnessAdj(
        /* [in] */ Float adj);

    CARAPI_(void) UpdateIcon(
        /* [in] */ Boolean automatic);

    /** Fetch the brightness mode from the system settings and update the icon */
    CARAPI_(void) UpdateMode();

    /** Fetch the brightness from the system settings and update the slider */
    CARAPI_(void) UpdateSlider();

private:
    friend class ToggleSliderListener;

    const static String TAG;
    const static Boolean SHOW_AUTOMATIC_ICON;

    /**
     * {@link android.provider.Settings.System#SCREEN_AUTO_BRIGHTNESS_ADJ} uses the range [-1, 1].
     * Using this factor, it is converted to [0, BRIGHTNESS_ADJ_RESOLUTION] for the SeekBar.
     */
    const static Float BRIGHTNESS_ADJ_RESOLUTION;

    Int32 mMinimumBacklight;
    Int32 mMaximumBacklight;

    AutoPtr<IContext> mContext;
    AutoPtr<IImageView> mIcon;
    AutoPtr<IToggleSlider> mControl;
    Boolean mAutomaticAvailable;
    AutoPtr<IIPowerManager> mPower;
    AutoPtr<CurrentUserTracker> mUserTracker;
    AutoPtr<IHandler> mHandler;
    AutoPtr<BrightnessObserver> mBrightnessObserver;

    AutoPtr<IArrayList> mChangeCallbacks;

    Boolean mAutomatic;
    Boolean mListening;
    Boolean mExternalChange;
    AutoPtr<IToggleSliderListener> mToggleSliderListener;
};

} // namespace Settings
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_SETTINGS_BRIGHTNESSCONTROLLER_H__