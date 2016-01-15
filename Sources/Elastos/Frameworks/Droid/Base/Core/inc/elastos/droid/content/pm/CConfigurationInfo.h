
#ifndef __ELASTOS_DROID_CONTENT_PM_CCONFIGURATIONINFO_H__
#define __ELASTOS_DROID_CONTENT_PM_CCONFIGURATIONINFO_H__

#include "_Elastos_Droid_Content_Pm_CConfigurationInfo.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

/**
 * Information you can retrieve about hardware configuration preferences
 * declared by an application. This corresponds to information collected from the
 * AndroidManifest.xml's &lt;uses-configuration&gt; and &lt;uses-feature&gt; tags.
 */
CarClass(CConfigurationInfo)
    , public Object
    , public IConfigurationInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CConfigurationInfo();

    virtual ~CConfigurationInfo();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IConfigurationInfo* orig);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    /**
     * This method extracts the major and minor version of reqGLEsVersion attribute
     * and returns it as a string. Say reqGlEsVersion value of 0x00010002 is returned
     * as 1.2
     * @return String representation of the reqGlEsVersion attribute
     */
    CARAPI GetGlEsVersion(
        /* [out] */ String* ver);

    CARAPI GetReqTouchScreen(
        /* [out] */ Int32* reqTouchScreen);

    CARAPI SetReqTouchScreen(
        /* [in] */ Int32 reqTouchScreen);

    CARAPI GetReqKeyboardType(
        /* [out] */ Int32* reqKeyboardType);

    CARAPI SetReqKeyboardType(
        /* [in] */ Int32 reqKeyboardType);

    CARAPI GetReqNavigation(
        /* [out] */ Int32* reqNavigation);

    CARAPI SetReqNavigation(
        /* [in] */ Int32 reqNavigation);

    CARAPI GetReqInputFeatures(
        /* [out] */ Int32* reqInputFeatures);

    CARAPI SetReqInputFeatures(
        /* [in] */ Int32 reqInputFeatures);

    CARAPI GetReqGlEsVersion(
        /* [out] */ Int32* reqGlEsVersion);

    CARAPI SetReqGlEsVersion(
        /* [in] */ Int32 reqGlEsVersion);

public:
    /**
     * The kind of touch screen attached to the device.
     * One of: {@link android.content.res.Configuration#TOUCHSCREEN_NOTOUCH},
     * {@link android.content.res.Configuration#TOUCHSCREEN_STYLUS},
     * {@link android.content.res.Configuration#TOUCHSCREEN_FINGER}.
     */
    Int32 mReqTouchScreen;

    /**
     * Application's input method preference.
     * One of: {@link android.content.res.Configuration#KEYBOARD_UNDEFINED},
     * {@link android.content.res.Configuration#KEYBOARD_NOKEYS},
     * {@link android.content.res.Configuration#KEYBOARD_QWERTY},
     * {@link android.content.res.Configuration#KEYBOARD_12KEY}
     */
    Int32 mReqKeyboardType;

    /**
     * A flag indicating whether any keyboard is available.
     * one of: {@link android.content.res.Configuration#NAVIGATION_UNDEFINED},
     * {@link android.content.res.Configuration#NAVIGATION_DPAD},
     * {@link android.content.res.Configuration#NAVIGATION_TRACKBALL},
     * {@link android.content.res.Configuration#NAVIGATION_WHEEL}
     */
    Int32 mReqNavigation;

    /**
     * Flags associated with the input features.  Any combination of
     * {@link #INPUT_FEATURE_HARD_KEYBOARD},
     * {@link #INPUT_FEATURE_FIVE_WAY_NAV}
     */
    Int32 mReqInputFeatures;

    /**
     * The GLES version used by an application. The upper order 16 bits represent the
     * major version and the lower order 16 bits the minor version.
     */
    Int32 mReqGlEsVersion;
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_CCONFIGURATIONINFO_H__
