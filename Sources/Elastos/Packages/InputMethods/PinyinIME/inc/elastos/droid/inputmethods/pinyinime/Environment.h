
#ifndef __ELASTOS_DROID_INPUTMETHODS_PINYINIME_CPINYINENVIRONMENT_H__
#define __ELASTOS_DROID_INPUTMETHODS_PINYINIME_CPINYINENVIRONMENT_H__

#include "Elastos.Droid.Inputmethods.Pinyin.h"
#include "Elastos.Droid.Content.h"

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
class Environment : public Object
{
public:
    staitc CARAPI_(AutoPtr<Environment>) GetInstance();

    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig,
        /* [in] */ IContext* context);

    CARAPI_(AutoPtr<IConfiguration>) GetConfiguration();

    CARAPI_(Int32) GetScreenWidth();

    CARAPI_(Int32) GetScreenHeight();

    CARAPI_(Int32) GetHeightForCandidates();

    CARAPI_(Float) GetKeyXMarginFactor();

    CARAPI_(Float) GetKeyYMarginFactor();

    CARAPI_(Int32) GetKeyHeight();

    CARAPI_(Int32) GetKeyBalloonWidthPlus();

    CARAPI_(Int32) GetKeyBalloonHeightPlus();

    CARAPI_(Int32) GetSkbHeight();

    CARAPI_(Int32) GetKeyTextSize(
        /* [in] */ Boolean isFunctionKey);

    CARAPI_(Int32) GetBalloonTextSize(
        /* [in] */ Boolean isFunctionKey);

    CARAPI_(Boolean) HasHardKeyboard();

    CARAPI_(Boolean) NeedDebug();

private:
    Environment();

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

    static AutoPtr<IEnvironment> sInstance;

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
