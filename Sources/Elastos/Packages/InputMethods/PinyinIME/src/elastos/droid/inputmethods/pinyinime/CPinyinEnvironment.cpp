
#include "elastos/droid/ext/frameworkdef.h"
#include "CPinyinEnvironment.h"
#include "elastos/droid/content/res/CConfiguration.h"

using Elastos::Droid::Content::Res::CConfiguration;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::IWindowManager;

namespace Elastos {
namespace Droid {
namespace Inputmethods {
namespace PinyinIME {

const Float CPinyinEnvironment::KEY_HEIGHT_RATIO_PORTRAIT = 0.105f;
const Float CPinyinEnvironment::KEY_HEIGHT_RATIO_LANDSCAPE = 0.147f;
const Float CPinyinEnvironment::CANDIDATES_AREA_HEIGHT_RATIO_PORTRAIT = 0.084f;
const Float CPinyinEnvironment::CANDIDATES_AREA_HEIGHT_RATIO_LANDSCAPE = 0.125f;
const Float CPinyinEnvironment::KEY_BALLOON_WIDTH_PLUS_RATIO = 0.08f;
const Float CPinyinEnvironment::KEY_BALLOON_HEIGHT_PLUS_RATIO = 0.07f;
const Float CPinyinEnvironment::NORMAL_KEY_TEXT_SIZE_RATIO = 0.075f;
const Float CPinyinEnvironment::FUNCTION_KEY_TEXT_SIZE_RATIO = 0.055f;
const Float CPinyinEnvironment::NORMAL_BALLOON_TEXT_SIZE_RATIO = 0.14f;
const Float CPinyinEnvironment::FUNCTION_BALLOON_TEXT_SIZE_RATIO = 0.085f;

CAR_OBJECT_IMPL(CPinyinEnvironment);
CAR_INTERFACE_IMPL(CPinyinEnvironment, Object, IPinyinEnvironment);

CPinyinEnvironment::CPinyinEnvironment()
    : mScreenWidth(0)
    , mScreenHeight(0)
    , mKeyHeight(0)
    , mCandidatesAreaHeight(0)
    , mKeyBalloonWidthPlus(0)
    , mKeyBalloonHeightPlus(0)
    , mNormalKeyTextSize(0)
    , mFunctionKeyTextSize(0)
    , mNormalBalloonTextSize(0)
    , mFunctionBalloonTextSize(0)
    , mDebug(FALSE)
{
    CConfiguration::New((IConfiguration**)&mConfig);
}

ECode CPinyinEnvironment::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig,
    /* [in] */ IContext* context)
{
    assert(newConfig != NULL);
    Int32 ori1 = 0, ori2 = 0;
    mConfig->GetOrientation(&ori1);
    newConfig->GetOrientation(&ori2);
    if (ori1 != ori2) {
        AutoPtr<IInterface> service;
        FAIL_RETURN(context->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&service))
        AutoPtr<IWindowManager> wm = IWindowManager::Probe(service);
        AutoPtr<IDisplay> d;
        wm->GetDefaultDisplay((IDisplay**)&d);
        d->GetWidth(&mScreenWidth);
        d->GetHeight(&mScreenHeight);

        Int32 scale;
        if (mScreenHeight > mScreenWidth) {
            mKeyHeight = (Int32) (mScreenHeight * KEY_HEIGHT_RATIO_PORTRAIT);
            mCandidatesAreaHeight = (Int32) (mScreenHeight * CANDIDATES_AREA_HEIGHT_RATIO_PORTRAIT);
            scale = mScreenWidth;
        } else {
            mKeyHeight = (Int32) (mScreenHeight * KEY_HEIGHT_RATIO_LANDSCAPE);
            mCandidatesAreaHeight = (Int32) (mScreenHeight * CANDIDATES_AREA_HEIGHT_RATIO_LANDSCAPE);
            scale = mScreenHeight;
        }
        mNormalKeyTextSize = (Int32) (scale * NORMAL_KEY_TEXT_SIZE_RATIO);
        mFunctionKeyTextSize = (Int32) (scale * FUNCTION_KEY_TEXT_SIZE_RATIO);
        mNormalBalloonTextSize = (Int32) (scale * NORMAL_BALLOON_TEXT_SIZE_RATIO);
        mFunctionBalloonTextSize = (Int32) (scale * FUNCTION_BALLOON_TEXT_SIZE_RATIO);
        mKeyBalloonWidthPlus = (Int32) (scale * KEY_BALLOON_WIDTH_PLUS_RATIO);
        mKeyBalloonHeightPlus = (Int32) (scale * KEY_BALLOON_HEIGHT_PLUS_RATIO);
    }

    Int32 changes = 0;
    return mConfig->UpdateFrom(newConfig, &changes);
}

ECode CPinyinEnvironment::GetConfiguration(
    /* [out] */ IConfiguration** config)
{
    VALIDATE_NOT_NULL(config);
    *config = mConfig;
    REFCOUNT_ADD(*config);
    return NOERROR;
}

ECode CPinyinEnvironment::GetScreenWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);
    *width = mScreenWidth;
    return NOERROR;
}

ECode CPinyinEnvironment::GetScreenHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    *height = mScreenHeight;
    return NOERROR;
}

ECode CPinyinEnvironment::GetHeightForCandidates(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    *height = mCandidatesAreaHeight;
    return NOERROR;
}

ECode CPinyinEnvironment::GetKeyXMarginFactor(
    /* [out] */ Float* factor)
{
    VALIDATE_NOT_NULL(factor);
    *factor = 1.0f;
    return NOERROR;
}

ECode CPinyinEnvironment::GetKeyYMarginFactor(
    /* [out] */ Float* factor)
{
    VALIDATE_NOT_NULL(factor);
    Int32 orientation = 0;
    mConfig->GetOrientation(&orientation);
    if (IConfiguration::ORIENTATION_LANDSCAPE == orientation) {
        *factor = 0.7f;
        return NOERROR;
    }
    *factor = 1.0f;
    return NOERROR;
}

ECode CPinyinEnvironment::GetKeyHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    *height = mKeyHeight;
    return NOERROR;
}

ECode CPinyinEnvironment::GetKeyBalloonWidthPlus(
    /* [out] */ Int32* plus)
{
    VALIDATE_NOT_NULL(plus);
    *plus = mKeyBalloonWidthPlus;
    return NOERROR;
}

ECode CPinyinEnvironment::GetKeyBalloonHeightPlus(
    /* [out] */ Int32* plus)
{
    VALIDATE_NOT_NULL(plus);
    *plus = mKeyBalloonHeightPlus;
    return NOERROR;
}

ECode CPinyinEnvironment::GetSkbHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    Int32 orientation = 0;
    mConfig->GetOrientation(&orientation);
    if (IConfiguration::ORIENTATION_PORTRAIT == orientation) {
        *height = mKeyHeight * 4;
        return NOERROR;
    } else if (IConfiguration::ORIENTATION_LANDSCAPE == orientation) {
        *height = mKeyHeight * 4;
        return NOERROR;
    }
    *height = 0;
    return NOERROR;
}

ECode CPinyinEnvironment::GetKeyTextSize(
    /* [in] */ Boolean isFunctionKey,
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);
    if (isFunctionKey) {
        *size = mFunctionKeyTextSize;
        return NOERROR;
    }
    *size = mNormalKeyTextSize;
    return NOERROR;
}

ECode CPinyinEnvironment::GetBalloonTextSize(
    /* [in] */ Boolean isFunctionKey,
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);
    if (isFunctionKey) {
        *size = mFunctionBalloonTextSize;
        return NOERROR;
    }
    *size = mNormalBalloonTextSize;
    return NOERROR;
}

ECode CPinyinEnvironment::HasHardKeyboard(
    /* [out] */ Boolean* has)
{
    VALIDATE_NOT_NULL(has);
    Int32 keyboard = 0, hardKeyboardHidden = 0;
    mConfig->GetKeyboard(&keyboard);
    mConfig->GetHardKeyboardHidden(&hardKeyboardHidden);
    if (keyboard == IConfiguration::KEYBOARD_NOKEYS
            || hardKeyboardHidden == IConfiguration::HARDKEYBOARDHIDDEN_YES) {
        *has = FALSE;
        return NOERROR;
    }
    *has = TRUE;
    return NOERROR;
}

ECode CPinyinEnvironment::NeedDebug(
    /* [out] */ Boolean* debug)
{
    VALIDATE_NOT_NULL(debug);
    *debug = mDebug;
    return NOERROR;
}

} // namespace PinyinIME
} // namespace Inputmethods
} // namespace Droid
} // namespace Elastos
