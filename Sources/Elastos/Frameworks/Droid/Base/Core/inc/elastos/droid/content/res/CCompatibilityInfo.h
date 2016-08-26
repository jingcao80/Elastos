
#ifndef __ELASTOS_DROID_CONTENT_RES_CCOMPATIBILITYINFO_H__
#define __ELASTOS_DROID_CONTENT_RES_CCOMPATIBILITYINFO_H__

#include "_Elastos_Droid_Content_Res_CCompatibilityInfo.h"
#include "elastos/droid/content/res/CConfiguration.h"

using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IRegion;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPointF;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IWindowManagerLayoutParams;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

/**
 * CompatibilityInfo class keeps the information about compatibility mode that the application is
 * running under.
 *
 *  {@hide}
 */
CarClass(CCompatibilityInfo)
    , public Object
    , public ICompatibilityInfo
    , public IParcelable
{
public:
    /**
    * A helper object to translate the screen and window coordinates back and forth.
    * @hide
    */
    class Translator
        : public Object
        , public ICompatibilityInfoTranslator
    {
    public:
        CAR_INTERFACE_DECL()

        Translator(
            /* [in] */ Float applicationScale,
            /* [in] */ Float applicationInvertedScale);

        Translator();

        CARAPI TranslateRectInScreenToAppWinFrame(
            /* [in] */ IRect* rect);

        CARAPI TranslateRegionInWindowToScreen(
            /* [in] */ IRegion* transparentRegion);

        CARAPI TranslateCanvas(
            /* [in] */ ICanvas* canvas);

        CARAPI TranslateEventInScreenToAppWindow(
            /* [in] */ IMotionEvent* event);

        CARAPI TranslateWindowLayout(
            /* [in] */ IWindowManagerLayoutParams* params);

        CARAPI TranslateRectInAppWindowToScreen(
            /* [in] */ IRect* rect);

        CARAPI TranslateRectInScreenToAppWindow(
            /* [in] */ IRect* rect);

        /**
         * Translate a Point in screen coordinates into the app window's coordinates.
         */
        CARAPI TranslatePointInScreenToAppWindow(
            /* [in] */ IPointF* point);

        CARAPI TranslateLayoutParamsInAppWindowToScreen(
           /* [in] */ IWindowManagerLayoutParams* params);

        CARAPI GetTranslatedContentInsets(
            /* [in] */ IRect* contentInsets,
            /* [out] */ IRect** rect);

        CARAPI GetTranslatedVisibleInsets(
            /* [in] */ IRect* visibleInsets,
            /* [out] */ IRect** rect);

        CARAPI GetTranslatedTouchableArea(
            /* [in] */ IRegion* touchableArea,
            /* [out] */ IRegion** region);

        CARAPI GetApplicationScale(
            /* [out] */ Float* appScale);

        CARAPI GetApplicationInvertedScale(
            /* [out] */ Float* appInvertedScale);

    public:
        Float mApplicationScale;
        Float mApplicationInvertedScale;

    private:
        friend class CompatibilityInfo;

        AutoPtr<IRect> mContentInsetsBuffer;
        AutoPtr<IRect> mVisibleInsetsBuffer;
        AutoPtr<IRegion> mTouchableAreaBuffer;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CCompatibilityInfo();

    ~CCompatibilityInfo();

    /**
     * @return true if the scaling is required
     */
    CARAPI IsScalingRequired(
        /* [out] */ Boolean* isRequire);

    CARAPI SupportsScreen(
        /* [out] */ Boolean* isSupport);

    CARAPI NeverSupportsScreen(
        /* [out] */ Boolean* value);

    CARAPI AlwaysSupportsScreen(
        /* [out] */ Boolean* value);

    /**
     * Returns the translator which translates the coordinates in compatibility mode.
     * @param params the window's parameter
     */
    CARAPI GetTranslator(
        /* [out] */ ICompatibilityInfoTranslator** translator);

    CARAPI ApplyToDisplayMetrics(
        /* [in] */ IDisplayMetrics* inoutDm);

    CARAPI ApplyToConfiguration(
        /* [in] */ Int32 displayDensity,
        /* [in] */ IConfiguration* inoutConfig);

    /**
     * Compute the frame Rect for applications runs under compatibility mode.
     *
     * @param dm the display metrics used to compute the frame size.
     * @param orientation the orientation of the screen.
     * @param outRect the output parameter which will contain the result.
     * @return Returns the scaling factor for the window.
     */
    static CARAPI_(Float) ComputeCompatibleScaling(
        /* [in] */ IDisplayMetrics* dm,
        /* [in] */ IDisplayMetrics* outDm);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI GetApplicationDensity(
        /* [out] */ Int32* applicationDensity);

    CARAPI SetApplicationDensity(
        /* [in] */ Int32 applicationDensity);

    CARAPI GetApplicationScale(
        /* [out] */ Float* applicationScale);

    CARAPI SetApplicationScale(
        /* [in] */ Float applicationScale);

    CARAPI GetApplicationInvertedScale(
        /* [out] */ Float* applicationInvertedScale);

    CARAPI SetApplicationInvertedScale(
        /* [in] */ Float applicationInvertedScale);

    CARAPI GetIsThemeable(
        /* [out] */ Boolean* isThemeable);

    CARAPI SetIsThemeable(
        /* [in] */ Boolean isThemeable);

    CARAPI constructor(
        /* [in] */ IApplicationInfo* appInfo,
        /* [in] */ Int32 screenLayout,
        /* [in] */ Int32 sw,
        /* [in] */ Boolean forceCompat);

    CARAPI constructor();

private:
    CARAPI constructor(
        /* [in] */ Int32 compFlags,
        /* [in] */ Int32 dens,
        /* [in] */ Float scale,
        /* [in] */ Float invertedScale,
        /* [in] */ Boolean isThemeable);

public:
    /** default compatibility info object for compatible applications */
    static const AutoPtr<ICompatibilityInfo> DEFAULT_COMPATIBILITY_INFO;

    /**
     * The effective screen density we have selected for this application.
     */
    Int32 mApplicationDensity;

    /**
     * Application's scale.
     */
    Float mApplicationScale;

    /**
     * Application's inverted scale.
     */
    Float mApplicationInvertedScale;

    /**
     * Whether the application supports third-party theming.
     */
    Boolean mIsThemeable;

private:

    /**
     *  A compatibility flags
     */
    Int32 mCompatibilityFlags;

    /**
     * A flag mask to tell if the application needs scaling (when mApplicationScale != 1.0f)
     * {@see compatibilityFlag}
     */
    static const Int32 SCALING_REQUIRED;

    /**
     * Application must always run in compatibility mode?
     */
    static const Int32 ALWAYS_NEEDS_COMPAT;

    /**
     * Application never should run in compatibility mode?
     */
    static const Int32 NEVER_NEEDS_COMPAT;

    /**
     * Set if the application needs to run in screen size compatibility mode.
     */
    static const Int32 NEEDS_SCREEN_COMPAT;


};

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_RES_CCOMPATIBILITYINFO_H__
