
#ifndef __ELASTOS_DROID_SERVER_DISPLAY_LIVEDISPLAYCONTROLLER_H__
#define __ELASTOS_DROID_SERVER_DISPLAY_LIVEDISPLAYCONTROLLER_H__

#include "_Elastos.Droid.Server.h"
#include <Elastos.Droid.Animation.h>
#include <elastos/droid/database/ContentObserver.h>
#include <elastos/droid/os/Handler.h>
#include <elastos/core/AutoLock.h>

using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Hardware::ICmHardwareManager;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::ILowPowerModeListener;
using Elastos::Droid::Server::Twilight::ITwilightManager;
using Elastos::Droid::Server::Twilight::ITwilightListener;
using Elastos::Droid::Server::Twilight::ITwilightState;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Display {

class LiveDisplayController
    : public Object
{
private:
    class SettingsObserver : public ContentObserver
    {
    public:
        SettingsObserver(
            /* [in] */ LiveDisplayController* host);

        CARAPI_(void) Register(
            /* [in] */ Boolean r);

        CARAPI OnChange(
            /* [in] */ Boolean selfChange,
            /* [in] */ IUri* uri);

    private:
        static const AutoPtr<IUri> DISPLAY_TEMPERATURE_DAY_URI;
        static const AutoPtr<IUri> DISPLAY_TEMPERATURE_NIGHT_URI;
        static const AutoPtr<IUri> DISPLAY_TEMPERATURE_MODE_URI;
        static const AutoPtr<IUri> DISPLAY_AUTO_OUTDOOR_MODE_URI;
        static const AutoPtr<IUri> DISPLAY_LOW_POWER_URI;
        static const AutoPtr<IUri> DISPLAY_COLOR_ENHANCE_URI;
        static const AutoPtr<IUri> DISPLAY_COLOR_ADJUSTMENT_URI;

        LiveDisplayController* mHost;
    };

    class AnimatorUpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        AnimatorUpdateListener(
            /* [in] */ LiveDisplayController* host)
            : mHost(host)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        LiveDisplayController* mHost;
    };

    class TwilightListener
        : public Object
        , public ITwilightListener
    {
    public:
        TwilightListener(
            /* [in] */ LiveDisplayController* host)
            : mHost(host)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnTwilightStateChanged();

    private:
        LiveDisplayController* mHost;
    };

    class LiveDisplayHandler : public Handler
    {
    public:
        TO_STRING_IMPL("LiveDisplayController::LiveDisplayHandler")

        LiveDisplayHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ LiveDisplayController* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        LiveDisplayController* mHost;
    };

public:
    LiveDisplayController(
        /* [in] */ IContext* context,
        /* [in] */ ILooper* looper);

    CARAPI_(void) UpdateLiveDisplay();

    CARAPI_(void) UpdateLiveDisplay(
        /* [in] */ Float lux);

private:
    CARAPI_(void) UpdateSettings();

    CARAPI_(void) UpdateColorTemperature(
        /* [in] */ ITwilightState* twilight);

    CARAPI_(void) SetDisplayTemperature(
        /* [in] */ Int32 temperature);

    /**
     * Outdoor mode is optionally enabled when ambient lux > 10000 and it's daytime
     * Melt faces!
     *
     * TODO: Use the camera or RGB sensor to determine if it's really sunlight
     */
    CARAPI_(void) UpdateOutdoorMode(
        /* [in] */ ITwilightState* twilight);

    /**
     * Color enhancement is optional, but can look bad with night mode
     */
    CARAPI_(void) UpdateColorEnhancement(
        /* [in] */ ITwilightState* twilight);

    /**
     * Adaptive backlight / low power mode. Turn it off when under very bright light.
     */
    CARAPI_(void) UpdateLowPowerMode();

    /**
     * Convert a color temperature value (in Kelvin) to a RGB units as floats.
     * This can be used in a transform matrix or hardware gamma control.
     *
     * @param tempK
     * @return
     */
    static CARAPI_(AutoPtr<ArrayOf<Float> >) TemperatureToRGB(
        /* [in] */ Int32 degreesK);

    static CARAPI_(Float) Interp(
        /* [in] */ Int32 i,
        /* [in] */ Float a);

    /**
     * Where is the sun anyway? This calculation determines day or night, and scales
     * the value around sunset/sunrise for a smooth transition.
     *
     * @param now
     * @param sunset
     * @param sunrise
     * @return float between 0 and 1
     */
    static CARAPI_(Float) Adj(
        /* [in] */ Int64 now,
        /* [in] */ Int64 sunset,
        /* [in] */ Int64 sunrise);

    /**
     * Determine the color temperature we should use for the display based on
     * the position of the sun.
     *
     * @param state
     * @return color temperature in Kelvin
     */
    CARAPI_(Int32) GetTwilightK(
        /* [in] */ ITwilightState* state);

    /**
     * Tell SurfaceFlinger to repaint the screen. This is called after updating
     * hardware registers for display calibration to have an immediate effect.
     */
    static CARAPI_(void) ScreenRefresh();

    CARAPI_(void) SaveUserHint(
        /* [in] */ Int32 value);

    /**
     * Show a friendly notification to the user about the potential benefits of decreasing
     * blue light at night. Do this only once if the feature has not been used after
     * three sunsets. It would be great to enable this by default, but we don't want
     * the change of screen color to be considered a "bug" by a user who doesn't
     * understand what's happening.
     *
     * @param state
     */
    CARAPI_(void) UpdateUserHint(
        /* [in] */ ITwilightState* state);

public:
    static const Int32 MODE_OFF = 0;
    static const Int32 MODE_NIGHT = 1;
    static const Int32 MODE_AUTO = 2;
    static const Int32 MODE_OUTDOOR = 3;
    static const Int32 MODE_DAY = 4;

private:
    static const String TAG;
    static const Int64 TWILIGHT_ADJUSTMENT_TIME;

    static const Int32 OFF_TEMPERATURE = 6500;

    static const Int32 MSG_UPDATE_LIVE_DISPLAY = 1;

    /**
     * This table is a modified version of the original blackbody chart, found here:
     * http://www.vendian.org/mncharity/dir3/blackbody/UnstableURLs/bbr_color.html
     *
     * Created by Ingo Thiel.
     */
    static const AutoPtr<ArrayOf<Double> > sColorTable;

    Int32 mColorTemperature;
    Float mCurrentLux;

    Int32 mHintCounter;
    Int32 mMode;

    Boolean mOutdoorMode;
    Boolean mColorEnhancement;
    Boolean mLowPower;

    AutoPtr<IContext> mContext;
    AutoPtr<IHandler> mHandler;
    AutoPtr<ICmHardwareManager> mCmHardwareManager;

    Int32 mDayTemperature;
    Int32 mNightTemperature;

    Boolean mUseOutdoorMode;
    Boolean mUseColorEnhancement;
    Boolean mUseLowPower;

    AutoPtr<ArrayOf<Float> > mColorAdjustment;
    AutoPtr<ArrayOf<Float> > mRGB;

    AutoPtr<ITwilightManager> mTwilightManager;
    Boolean mSunset;

    AutoPtr<SettingsObserver> mObserver;

    AutoPtr<IValueAnimator> mAnimator;

    Int32 mDefaultDayTemperature;
    Int32 mDefaultNightTemperature;
    Int32 mDefaultOutdoorLux;

    // Display postprocessing can have power impact. Disable it if powersave mode is on.
    Boolean mLowPerformance;
    AutoPtr<ILowPowerModeListener> mLowPowerModeListener;

    AutoPtr<TwilightListener> mTwilightListener;

    friend class TwilightListener;
    friend class AnimatorUpdateListener;
    friend class SettingsObserver;
    friend class LiveDisplayHandler;
};

} // namespace Display
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_DISPLAY_LIVEDISPLAYCONTROLLER_H__
