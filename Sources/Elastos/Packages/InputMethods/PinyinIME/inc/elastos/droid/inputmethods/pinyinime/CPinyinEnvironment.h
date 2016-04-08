
#ifndef __ELASTOS_DROID_INPUTMETHODS_PINYINIME_CPINYINENVIRONMENT_H__
#define __ELASTOS_DROID_INPUTMETHODS_PINYINIME_CPINYINENVIRONMENT_H__

#include "_Elastos_Droid_Inputmethods_PinyinIME_CPinyinEnvironment.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IConfiguration;

namespace Elastos {
namespace Droid {
namespace Inputmethods {
namespace PinyinIME {

/**
 * Global environment configurations for showing soft keyboard and candidate
 * view. All original dimension values are defined in Float, and the real size
 * is calculated from the Float values of and screen size. In this way, this
 * input method can work even when screen size is changed.
 */
CarClass(CPinyinEnvironment)
    , public Object
    , public IPinyinEnvironment
{
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CPinyinEnvironment();

    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig,
        /* [in] */ IContext* context);

    CARAPI GetConfiguration(
        /* [out] */ IConfiguration** config);

    CARAPI GetScreenWidth(
        /* [out] */ Int32* width);

    CARAPI GetScreenHeight(
        /* [out] */ Int32* height);

    CARAPI GetHeightForCandidates(
        /* [out] */ Int32* height);

    CARAPI GetKeyXMarginFactor(
        /* [out] */ Float* factor);

    CARAPI GetKeyYMarginFactor(
        /* [out] */ Float* factor);

    CARAPI GetKeyHeight(
        /* [out] */ Int32* height);

    CARAPI GetKeyBalloonWidthPlus(
        /* [out] */ Int32* plus);

    CARAPI GetKeyBalloonHeightPlus(
        /* [out] */ Int32* plus);

    CARAPI GetSkbHeight(
        /* [out] */ Int32* height);

    CARAPI GetKeyTextSize(
        /* [in] */ Boolean isFunctionKey,
        /* [out] */ Int32* size);

    CARAPI GetBalloonTextSize(
        /* [in] */ Boolean isFunctionKey,
        /* [out] */ Int32* size);

    CARAPI HasHardKeyboard(
        /* [out] */ Boolean* has);

    CARAPI NeedDebug(
        /* [out] */ Boolean* debug);

private:
    /**
     * The key height for portrait mode. It is relative to the screen height.
     */
    static const Float KEY_HEIGHT_RATIO_PORTRAIT;

    /**
     * The key height for landscape mode. It is relative to the screen height.
     */
    static const Float KEY_HEIGHT_RATIO_LANDSCAPE;

    /**
     * The height of the candidates area for portrait mode. It is relative to
     * screen height.
     */
    static const Float CANDIDATES_AREA_HEIGHT_RATIO_PORTRAIT;

    /**
     * The height of the candidates area for portrait mode. It is relative to
     * screen height.
     */
    static const Float CANDIDATES_AREA_HEIGHT_RATIO_LANDSCAPE;

    /**
     * How much should the balloon width be larger than width of the real key.
     * It is relative to the smaller one of screen width and height.
     */
    static const Float KEY_BALLOON_WIDTH_PLUS_RATIO;

    /**
     * How much should the balloon height be larger than that of the real key.
     * It is relative to the smaller one of screen width and height.
     */
    static const Float KEY_BALLOON_HEIGHT_PLUS_RATIO;

    /**
     * The text size for normal keys. It is relative to the smaller one of
     * screen width and height.
     */
    static const Float NORMAL_KEY_TEXT_SIZE_RATIO;

    /**
     * The text size for function keys. It is relative to the smaller one of
     * screen width and height.
     */
    static const Float FUNCTION_KEY_TEXT_SIZE_RATIO;

    /**
     * The text size balloons of normal keys. It is relative to the smaller one
     * of screen width and height.
     */
    static const Float NORMAL_BALLOON_TEXT_SIZE_RATIO;

    /**
     * The text size balloons of function keys. It is relative to the smaller
     * one of screen width and height.
     */
    static const Float FUNCTION_BALLOON_TEXT_SIZE_RATIO;

    Int32 mScreenWidth;
    Int32 mScreenHeight;
    Int32 mKeyHeight;
    Int32 mCandidatesAreaHeight;
    Int32 mKeyBalloonWidthPlus;
    Int32 mKeyBalloonHeightPlus;
    Int32 mNormalKeyTextSize;
    Int32 mFunctionKeyTextSize;
    Int32 mNormalBalloonTextSize;
    Int32 mFunctionBalloonTextSize;
    AutoPtr<IConfiguration> mConfig;
    Boolean mDebug;
};

} // namespace PinyinIME
} // namespace Inputmethods
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_INPUTMETHODS_PINYINIME_CPINYINENVIRONMENT_H__
