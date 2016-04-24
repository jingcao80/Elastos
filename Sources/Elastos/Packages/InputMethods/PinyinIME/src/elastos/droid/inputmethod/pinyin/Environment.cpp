
#include "elastos/droid/ext/frameworkdef.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/inputmethod/pinyin/Environment.h"

using Elastos::Droid::Content::Res::CConfiguration;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::IWindowManager;

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

const Float Environment::KEY_HEIGHT_RATIO_PORTRAIT = 0.105f;
const Float Environment::KEY_HEIGHT_RATIO_LANDSCAPE = 0.147f;
const Float Environment::CANDIDATES_AREA_HEIGHT_RATIO_PORTRAIT = 0.084f;
const Float Environment::CANDIDATES_AREA_HEIGHT_RATIO_LANDSCAPE = 0.125f;
const Float Environment::KEY_BALLOON_WIDTH_PLUS_RATIO = 0.08f;
const Float Environment::KEY_BALLOON_HEIGHT_PLUS_RATIO = 0.07f;
const Float Environment::NORMAL_KEY_TEXT_SIZE_RATIO = 0.075f;
const Float Environment::FUNCTION_KEY_TEXT_SIZE_RATIO = 0.055f;
const Float Environment::NORMAL_BALLOON_TEXT_SIZE_RATIO = 0.14f;
const Float Environment::FUNCTION_BALLOON_TEXT_SIZE_RATIO = 0.085f;

Environment::Environment()
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

ECode Environment::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig,
    /* [in] */ IContext* context)
{
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
        }
        else {
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

AutoPtr<IConfiguration> Environment::GetConfiguration()
{
    return mConfig;
}

Int32 Environment::GetScreenWidth()
{
    return mScreenWidth;
}

Int32 Environment::GetScreenHeight()
{
    return mScreenHeight;
}

Int32 Environment::GetHeightForCandidates()
{
    return mCandidatesAreaHeight;
}

Float Environment::GetKeyXMarginFactor()
{
    return 1.0f;
}

Float Environment::GetKeyYMarginFactor()
{
    Int32 orientation = 0;
    mConfig->GetOrientation(&orientation);
    if (IConfiguration::ORIENTATION_LANDSCAPE == orientation) {
        return 0.7f;
    }
    return 1.0f;
}

Int32 Environment::GetKeyHeight()
{
    return mKeyHeight;
}

Int32 Environment::GetKeyBalloonWidthPlus()
{
    return mKeyBalloonWidthPlus;
}

Int32 Environment::GetKeyBalloonHeightPlus()
{
    return mKeyBalloonHeightPlus;
}

Int32 Environment::GetSkbHeight()
{
    Int32 orientation = 0;
    mConfig->GetOrientation(&orientation);
    if (IConfiguration::ORIENTATION_PORTRAIT == orientation) {
        return mKeyHeight * 4;
    }
    else if (IConfiguration::ORIENTATION_LANDSCAPE == orientation) {
        return mKeyHeight * 4;
    }
    return 0;
}

Int32 Environment::GetKeyTextSize(
    /* [in] */ Boolean isFunctionKey)
{
    if (isFunctionKey) {
        return mFunctionKeyTextSize;
    }
    else {
        return mNormalKeyTextSize;
    }
}

Int32 Environment::GetBalloonTextSize(
    /* [in] */ Boolean isFunctionKey)
{
    if (isFunctionKey) {
        return mFunctionBalloonTextSize;
    }
    else {
        return mNormalBalloonTextSize;
    }
}

Boolean Environment::HasHardKeyboard()
{
    Int32 keyboard = 0, hardKeyboardHidden = 0;
    mConfig->GetKeyboard(&keyboard);
    mConfig->GetHardKeyboardHidden(&hardKeyboardHidden);
    if (keyboard == IConfiguration::KEYBOARD_NOKEYS
            || hardKeyboardHidden == IConfiguration::HARDKEYBOARDHIDDEN_YES) {
        return FALSE;
    }
    return TRUE;
}

Boolean Environment::NeedDebug()
{
    return mDebug;
}

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos
