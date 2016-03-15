
#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.View.h"
#include "elastos/droid/content/res/CConfiguration.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Os::Build;
using Elastos::Droid::View::IView;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::CLocale;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

static AutoPtr<IConfiguration> InitEmpty()
{
    AutoPtr<CConfiguration> config;
    ASSERT_SUCCEEDED(CConfiguration::NewByFriend((CConfiguration**)&config));
    return (IConfiguration*)config.Get();
}

const AutoPtr<IConfiguration> CConfiguration::EMPTY = InitEmpty();

// /** @hide Native-specific bit mask for MCC config; DO NOT USE UNLESS YOU ARE SURE. */
// const Int32 CConfiguration::NATIVE_CONFIG_MCC = 0x0001;
// /** @hide Native-specific bit mask for MNC config; DO NOT USE UNLESS YOU ARE SURE. */
// const Int32 CConfiguration::NATIVE_CONFIG_MNC = 0x0002;
// /** @hide Native-specific bit mask for LOCALE config; DO NOT USE UNLESS YOU ARE SURE. */
// const Int32 CConfiguration::NATIVE_CONFIG_LOCALE = 0x0004;
// /** @hide Native-specific bit mask for TOUCHSCREEN config; DO NOT USE UNLESS YOU ARE SURE. */
// const Int32 CConfiguration::NATIVE_CONFIG_TOUCHSCREEN = 0x0008;
// /** @hide Native-specific bit mask for KEYBOARD config; DO NOT USE UNLESS YOU ARE SURE. */
// const Int32 CConfiguration::NATIVE_CONFIG_KEYBOARD = 0x0010;
// /** @hide Native-specific bit mask for KEYBOARD_HIDDEN config; DO NOT USE UNLESS YOU
//  * ARE SURE. */
// const Int32 CConfiguration::NATIVE_CONFIG_KEYBOARD_HIDDEN = 0x0020;
// /** @hide Native-specific bit mask for NAVIGATION config; DO NOT USE UNLESS YOU ARE SURE. */
// const Int32 CConfiguration::NATIVE_CONFIG_NAVIGATION = 0x0040;
// /** @hide Native-specific bit mask for ORIENTATION config; DO NOT USE UNLESS YOU ARE SURE. */
// const Int32 CConfiguration::NATIVE_CONFIG_ORIENTATION = 0x0080;
// /** @hide Native-specific bit mask for DENSITY config; DO NOT USE UNLESS YOU ARE SURE. */
// const Int32 CConfiguration::NATIVE_CONFIG_DENSITY = 0x0100;
// /** @hide Native-specific bit mask for SCREEN_SIZE config; DO NOT USE UNLESS YOU ARE SURE. */
// const Int32 CConfiguration::NATIVE_CONFIG_SCREEN_SIZE = 0x0200;
// /** @hide Native-specific bit mask for VERSION config; DO NOT USE UNLESS YOU ARE SURE. */
// const Int32 CConfiguration::NATIVE_CONFIG_VERSION = 0x0400;
// /** @hide Native-specific bit mask for SCREEN_LAYOUT config; DO NOT USE UNLESS YOU ARE SURE. */
// const Int32 CConfiguration::NATIVE_CONFIG_SCREEN_LAYOUT = 0x0800;
// /** @hide Native-specific bit mask for UI_MODE config; DO NOT USE UNLESS YOU ARE SURE. */
// const Int32 CConfiguration::NATIVE_CONFIG_UI_MODE = 0x1000;
// /** @hide Native-specific bit mask for SMALLEST_SCREEN_SIZE config; DO NOT USE UNLESS YOU
//  * ARE SURE. */
// const Int32 CConfiguration::NATIVE_CONFIG_SMALLEST_SCREEN_SIZE = 0x2000;
// /** @hide Native-specific bit mask for LAYOUTDIR config ; DO NOT USE UNLESS YOU ARE SURE.*/
// const Int32 CConfiguration::NATIVE_CONFIG_LAYOUTDIR = 0x4000;

const String CConfiguration::XML_ATTR_FONT_SCALE("fs");
const String CConfiguration::XML_ATTR_MCC("mcc");
const String CConfiguration::XML_ATTR_MNC("mnc");
const String CConfiguration::XML_ATTR_LOCALE("locale");
const String CConfiguration::XML_ATTR_TOUCHSCREEN("touch");
const String CConfiguration::XML_ATTR_KEYBOARD("key");
const String CConfiguration::XML_ATTR_KEYBOARD_HIDDEN("keyHid");
const String CConfiguration::XML_ATTR_HARD_KEYBOARD_HIDDEN("hardKeyHid");
const String CConfiguration::XML_ATTR_NAVIGATION("nav");
const String CConfiguration::XML_ATTR_NAVIGATION_HIDDEN("navHid");
const String CConfiguration::XML_ATTR_ORIENTATION("ori");
const String CConfiguration::XML_ATTR_SCREEN_LAYOUT("scrLay");
const String CConfiguration::XML_ATTR_UI_MODE("ui");
const String CConfiguration::XML_ATTR_SCREEN_WIDTH("width");
const String CConfiguration::XML_ATTR_SCREEN_HEIGHT("height");
const String CConfiguration::XML_ATTR_SMALLEST_WIDTH("sw");
const String CConfiguration::XML_ATTR_DENSITY("density");

CAR_INTERFACE_IMPL(CConfiguration, Object, IConfiguration)

CAR_OBJECT_IMPL(CConfiguration)

CConfiguration::CConfiguration()
    : mFontScale(0.0f)
    , mMcc(0)
    , mMnc(0)
    , mUserSetLocale(FALSE)
    , mScreenLayout(0)
    , mTouchscreen(0)
    , mKeyboard(0)
    , mKeyboardHidden(0)
    , mHardKeyboardHidden(0)
    , mNavigation(0)
    , mNavigationHidden(0)
    , mOrientation(0)
    , mUiMode(0)
    , mScreenWidthDp(0)
    , mScreenHeightDp(0)
    , mSmallestScreenWidthDp(0)
    , mDensityDpi(0)
    , mCompatScreenWidthDp(0)
    , mCompatScreenHeightDp(0)
    , mCompatSmallestScreenWidthDp(0)
    , mSeq(0)
{}

Int32 CConfiguration::ResetScreenLayout(
    /* [in] */ Int32 curLayout)
{
    return (curLayout & ~(SCREENLAYOUT_LONG_MASK | SCREENLAYOUT_SIZE_MASK
                    | SCREENLAYOUT_COMPAT_NEEDED))
            | (SCREENLAYOUT_LONG_YES | SCREENLAYOUT_SIZE_XLARGE);
}

Int32 CConfiguration::ReduceScreenLayout(
    /* [in] */ Int32 curLayout,
    /* [in] */ Int32 longSizeDp,
    /* [in] */ Int32 shortSizeDp)
{
    Int32 screenLayoutSize;
    Boolean screenLayoutLong;
    Boolean screenLayoutCompatNeeded;

    // These semi-magic numbers define our compatibility modes for
    // applications with different screens.  These are guarantees to
    // app developers about the space they can expect for a particular
    // configuration.  DO NOT CHANGE!
    if (longSizeDp < 470) {
        // This is shorter than an HVGA normal density screen (which
        // is 480 pixels on its long side).
        screenLayoutSize = SCREENLAYOUT_SIZE_SMALL;
        screenLayoutLong = FALSE;
        screenLayoutCompatNeeded = FALSE;
    }
    else {
        // What size is this screen screen?
        if (longSizeDp >= 960 && shortSizeDp >= 720) {
            // 1.5xVGA or larger screens at medium density are the point
            // at which we consider it to be an extra large screen.
            screenLayoutSize = SCREENLAYOUT_SIZE_XLARGE;
        }
        else if (longSizeDp >= 640 && shortSizeDp >= 480) {
            // VGA or larger screens at medium density are the point
            // at which we consider it to be a large screen.
            screenLayoutSize = SCREENLAYOUT_SIZE_LARGE;
        }
        else {
            screenLayoutSize = SCREENLAYOUT_SIZE_NORMAL;
        }

        // If this screen is wider than normal HVGA, or taller
        // than FWVGA, then for old apps we want to run in size
        // compatibility mode.
        if (shortSizeDp > 321 || longSizeDp > 570) {
            screenLayoutCompatNeeded = TRUE;
        }
        else {
            screenLayoutCompatNeeded = FALSE;
        }

        // Is this a long screen?
        if (((longSizeDp * 3) / 5) >= (shortSizeDp - 1)) {
            // Anything wider than WVGA (5:3) is considering to be long.
            screenLayoutLong = TRUE;
        }
        else {
            screenLayoutLong = FALSE;
        }
    }

    // Now reduce the last screenLayout to not be better than what we
    // have found.
    if (!screenLayoutLong) {
        curLayout = (curLayout & ~SCREENLAYOUT_LONG_MASK) | SCREENLAYOUT_LONG_NO;
    }
    if (screenLayoutCompatNeeded) {
        curLayout |= SCREENLAYOUT_COMPAT_NEEDED;
    }
    Int32 curSize = curLayout & SCREENLAYOUT_SIZE_MASK;
    if (screenLayoutSize < curSize) {
        curLayout = (curLayout & ~SCREENLAYOUT_SIZE_MASK) | screenLayoutSize;
    }
    return curLayout;
}

ECode CConfiguration::IsLayoutSizeAtLeast(
    /* [in] */ Int32 size,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    Int32 cur = mScreenLayout & SCREENLAYOUT_SIZE_MASK;
    if (cur == SCREENLAYOUT_SIZE_UNDEFINED) {
        *value = FALSE;
        return NOERROR;
    }
    *value = cur >= size;
    return NOERROR;
}

ECode CConfiguration::SetTo(
    /* [in] */ IConfiguration* o)
{
    assert(o != NULL);

    o->GetFontScale(&mFontScale);
    o->GetMcc(&mMcc);
    o->GetMnc(&mMnc);

    AutoPtr<ILocale> locale;
    o->GetLocale((ILocale**)&locale);
    if (locale != NULL) {
        AutoPtr<IInterface> obj;
        locale->Clone((IInterface**)&obj);
        mLocale = ILocale::Probe(obj);
    }

    o->IsUserSetLocale(&mUserSetLocale);
    o->GetTouchscreen(&mTouchscreen);
    o->GetKeyboard(&mKeyboard);
    o->GetKeyboardHidden(&mKeyboardHidden);
    o->GetHardKeyboardHidden(&mHardKeyboardHidden);
    o->GetNavigation(&mNavigation);
    o->GetNavigationHidden(&mNavigationHidden);
    o->GetOrientation(&mOrientation);
    o->GetScreenLayout(&mScreenLayout);
    o->GetUiMode(&mUiMode);
    o->GetScreenWidthDp(&mScreenWidthDp);
    o->GetScreenHeightDp(&mScreenHeightDp);
    o->GetSmallestScreenWidthDp(&mSmallestScreenWidthDp);
    o->GetDensityDpi(&mDensityDpi);
    o->GetCompatScreenWidthDp(&mCompatScreenWidthDp);
    o->GetCompatScreenHeightDp(&mCompatScreenHeightDp);
    o->GetCompatSmallestScreenWidthDp(&mCompatSmallestScreenWidthDp);
    o->GetSeq(&mSeq);

    return NOERROR;
}

ECode CConfiguration::SetToDefaults()
{
    mFontScale = 1.0f;
    // mFontScale = StringUtils::ParseFloat(SystemProperties.get("ro.fontScale","1"));
    // TODO
    // String scale = Build::DEFAULT_FONTSCALE;
    // if (scale.EqualsIgnoreCase(Build::UNKNOWN) == FALSE ){
    //     mFontScale = StringUtils::ParseFloat(scale);
    // }
    // else {
    //     mFontScale = 1.0f;
    // }

    mMcc = mMnc = 0;
    mLocale = NULL;
    mUserSetLocale = FALSE;
    mTouchscreen = TOUCHSCREEN_UNDEFINED;
    mKeyboard = KEYBOARD_UNDEFINED;
    mKeyboardHidden = KEYBOARDHIDDEN_UNDEFINED;
    mHardKeyboardHidden = HARDKEYBOARDHIDDEN_UNDEFINED;
    mNavigation = NAVIGATION_UNDEFINED;
    mNavigationHidden = NAVIGATIONHIDDEN_UNDEFINED;
    mOrientation = ORIENTATION_UNDEFINED;
    mScreenLayout = SCREENLAYOUT_SIZE_UNDEFINED;
    mUiMode = UI_MODE_TYPE_UNDEFINED;
    mSeq = 0;
    return NOERROR;
}

ECode CConfiguration::MakeDefault()
{
    return SetToDefaults();
}

ECode CConfiguration::UpdateFrom(
    /* [in] */ IConfiguration* delta,
    /* [out] */ Int32* changes)
{
    VALIDATE_NOT_NULL(changes)

    AutoPtr<CConfiguration> config = (CConfiguration*)delta;
    if (config->mFontScale > 0 && mFontScale != config->mFontScale) {
        *changes |= IActivityInfo::CONFIG_FONT_SCALE;
        mFontScale = config->mFontScale;
    }

    if (config->mMcc != 0 && mMcc != config->mMcc) {
        *changes |= IActivityInfo::CONFIG_MCC;
        mMcc = config->mMcc;
    }

    if (config->mMnc != 0 && mMnc != config->mMnc) {
        *changes |= IActivityInfo::CONFIG_MNC;
        mMnc = config->mMnc;
    }

    Boolean isEqual;
    if (config->mLocale != NULL
            && (mLocale == NULL || (mLocale->Equals(config->mLocale, &isEqual), !isEqual))) {
        *changes |= IActivityInfo::CONFIG_LOCALE;
        mLocale = config->mLocale;
        // If locale has changed, then layout direction is also changed ...
        *changes |= IActivityInfo::CONFIG_LAYOUT_DIRECTION;
        // ... and we need to update the layout direction (represented by the first
        // 2 most significant bits in screenLayout).
        SetLayoutDirection(mLocale);
    }

    Int32 deltaScreenLayoutDir = config->mScreenLayout & SCREENLAYOUT_LAYOUTDIR_MASK;
    if (deltaScreenLayoutDir != SCREENLAYOUT_LAYOUTDIR_UNDEFINED &&
            deltaScreenLayoutDir != (mScreenLayout & SCREENLAYOUT_LAYOUTDIR_MASK)) {
        mScreenLayout = (mScreenLayout & ~SCREENLAYOUT_LAYOUTDIR_MASK) | deltaScreenLayoutDir;
        *changes |= IActivityInfo::CONFIG_LAYOUT_DIRECTION;
    }

    if (config->mUserSetLocale && (!mUserSetLocale ||
            ((*changes & IActivityInfo::CONFIG_LOCALE) != 0))) {
        mUserSetLocale = TRUE;
        *changes |= IActivityInfo::CONFIG_LOCALE;
    }

    if (config->mTouchscreen != TOUCHSCREEN_UNDEFINED
            && mTouchscreen != config->mTouchscreen) {
        *changes |= IActivityInfo::CONFIG_TOUCHSCREEN;
        mTouchscreen = config->mTouchscreen;
    }

    if (config->mKeyboard != KEYBOARD_UNDEFINED
            && mKeyboard != config->mKeyboard) {
        *changes |= IActivityInfo::CONFIG_KEYBOARD;
        mKeyboard = config->mKeyboard;
    }

    if (config->mKeyboardHidden != KEYBOARDHIDDEN_UNDEFINED
            && mKeyboardHidden != config->mKeyboardHidden) {
        *changes |= IActivityInfo::CONFIG_KEYBOARD_HIDDEN;
        mKeyboardHidden = config->mKeyboardHidden;
    }

    if (config->mHardKeyboardHidden != HARDKEYBOARDHIDDEN_UNDEFINED
            && mHardKeyboardHidden != config->mHardKeyboardHidden) {
        *changes |= IActivityInfo::CONFIG_KEYBOARD_HIDDEN;
        mHardKeyboardHidden = config->mHardKeyboardHidden;
    }

    if (config->mNavigation != NAVIGATION_UNDEFINED
            && mNavigation != config->mNavigation) {
        *changes |= IActivityInfo::CONFIG_NAVIGATION;
        mNavigation = config->mNavigation;
    }

    if (config->mNavigationHidden != NAVIGATIONHIDDEN_UNDEFINED
            && mNavigationHidden != config->mNavigationHidden) {
        *changes |= IActivityInfo::CONFIG_KEYBOARD_HIDDEN;
        mNavigationHidden = config->mNavigationHidden;
    }

    if (config->mOrientation != ORIENTATION_UNDEFINED
            && mOrientation != config->mOrientation) {
        *changes |= IActivityInfo::CONFIG_ORIENTATION;
        mOrientation = config->mOrientation;
    }

    if (GetScreenLayoutNoDirection(config->mScreenLayout) !=
                    (SCREENLAYOUT_SIZE_UNDEFINED | SCREENLAYOUT_LONG_UNDEFINED)
            && GetScreenLayoutNoDirection(mScreenLayout) !=
                    GetScreenLayoutNoDirection(config->mScreenLayout)) {
        *changes |= IActivityInfo::CONFIG_SCREEN_LAYOUT;
        // We need to preserve the previous layout dir bits if they were defined
        if ((config->mScreenLayout & SCREENLAYOUT_LAYOUTDIR_MASK) == 0) {
            mScreenLayout = (mScreenLayout & SCREENLAYOUT_LAYOUTDIR_MASK) | config->mScreenLayout;
        }
        else {
            mScreenLayout = config->mScreenLayout;
        }
    }

    if (config->mUiMode != (UI_MODE_TYPE_UNDEFINED | UI_MODE_NIGHT_UNDEFINED)
            && mUiMode != config->mUiMode) {
        *changes |= IActivityInfo::CONFIG_UI_MODE;
        if ((config->mUiMode & UI_MODE_TYPE_MASK) != UI_MODE_TYPE_UNDEFINED) {
            mUiMode = (mUiMode & ~UI_MODE_TYPE_MASK)
                    | (config->mUiMode & UI_MODE_TYPE_MASK);
        }
        if ((config->mUiMode & UI_MODE_NIGHT_MASK) != UI_MODE_NIGHT_UNDEFINED) {
            mUiMode = (mUiMode & ~UI_MODE_NIGHT_MASK)
                    | (config->mUiMode & UI_MODE_NIGHT_MASK);
        }
    }

    if (config->mScreenWidthDp != SCREEN_WIDTH_DP_UNDEFINED
            && mScreenWidthDp != config->mScreenWidthDp) {
        *changes |= IActivityInfo::CONFIG_SCREEN_SIZE;
        mScreenWidthDp = config->mScreenWidthDp;
    }

    if (config->mScreenHeightDp != SCREEN_HEIGHT_DP_UNDEFINED
            && mScreenHeightDp != config->mScreenHeightDp) {
        *changes |= IActivityInfo::CONFIG_SCREEN_SIZE;
        mScreenHeightDp = config->mScreenHeightDp;
    }

    if (config->mSmallestScreenWidthDp != SMALLEST_SCREEN_WIDTH_DP_UNDEFINED
            && mSmallestScreenWidthDp != config->mSmallestScreenWidthDp) {
        *changes |= IActivityInfo::CONFIG_SMALLEST_SCREEN_SIZE;
        mSmallestScreenWidthDp = config->mSmallestScreenWidthDp;
    }

    if (config->mDensityDpi != DENSITY_DPI_UNDEFINED
            && mDensityDpi != config->mDensityDpi) {
        *changes |= IActivityInfo::CONFIG_DENSITY;
        mDensityDpi = config->mDensityDpi;
    }

    if (config->mCompatScreenWidthDp != SCREEN_WIDTH_DP_UNDEFINED) {
        mCompatScreenWidthDp = config->mCompatScreenWidthDp;
    }

    if (config->mCompatScreenHeightDp != SCREEN_HEIGHT_DP_UNDEFINED) {
        mCompatScreenHeightDp = config->mCompatScreenHeightDp;
    }

    if (config->mCompatSmallestScreenWidthDp != SMALLEST_SCREEN_WIDTH_DP_UNDEFINED) {
        mCompatSmallestScreenWidthDp = config->mCompatSmallestScreenWidthDp;
    }

    if (config->mSeq != 0) {
        mSeq = config->mSeq;
    }

    return NOERROR;
}

ECode CConfiguration::Diff(
    /* [in] */ IConfiguration* delta,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = 0;

    AutoPtr<CConfiguration> config = (CConfiguration*)delta;
    if (config->mFontScale > 0 && mFontScale != config->mFontScale) {
        *result |= IActivityInfo::CONFIG_FONT_SCALE;
    }

    if (config->mMcc != 0 && mMcc != config->mMcc) {
        *result |= IActivityInfo::CONFIG_MCC;
    }

    if (config->mMnc != 0 && mMnc != config->mMnc) {
        *result |= IActivityInfo::CONFIG_MNC;
    }

    Boolean isSameLocale = FALSE;
    if (config->mLocale != NULL
            && (mLocale == NULL || (mLocale->Equals(config->mLocale, &isSameLocale), !isSameLocale))) {
        *result |= IActivityInfo::CONFIG_LOCALE;
        *result |= IActivityInfo::CONFIG_LAYOUT_DIRECTION;
    }

    Int32 deltaScreenLayoutDir = config->mScreenLayout & SCREENLAYOUT_LAYOUTDIR_MASK;
    if (deltaScreenLayoutDir != SCREENLAYOUT_LAYOUTDIR_UNDEFINED &&
            deltaScreenLayoutDir != (mScreenLayout & SCREENLAYOUT_LAYOUTDIR_MASK)) {
        *result |= IActivityInfo::CONFIG_LAYOUT_DIRECTION;
    }

    if (config->mTouchscreen != TOUCHSCREEN_UNDEFINED
            && mTouchscreen != config->mTouchscreen) {
        *result |= IActivityInfo::CONFIG_TOUCHSCREEN;
    }

    if (config->mKeyboard != KEYBOARD_UNDEFINED
            && mKeyboard != config->mKeyboard) {
        *result |= IActivityInfo::CONFIG_KEYBOARD;
    }

    if (config->mKeyboardHidden != KEYBOARDHIDDEN_UNDEFINED
            && mKeyboardHidden != config->mKeyboardHidden) {
        *result |= IActivityInfo::CONFIG_KEYBOARD_HIDDEN;
    }

    if (config->mHardKeyboardHidden != HARDKEYBOARDHIDDEN_UNDEFINED
            && mHardKeyboardHidden != config->mHardKeyboardHidden) {
        *result |= IActivityInfo::CONFIG_KEYBOARD_HIDDEN;
    }

    if (config->mNavigation != NAVIGATION_UNDEFINED
            && mNavigation != config->mNavigation) {
        *result |= IActivityInfo::CONFIG_NAVIGATION;
    }

    if (config->mNavigationHidden != NAVIGATIONHIDDEN_UNDEFINED
            && mNavigationHidden != config->mNavigationHidden) {
        *result |= IActivityInfo::CONFIG_KEYBOARD_HIDDEN;
    }

    if (config->mOrientation != ORIENTATION_UNDEFINED
            && mOrientation != config->mOrientation) {
        *result |= IActivityInfo::CONFIG_ORIENTATION;
    }

    if (GetScreenLayoutNoDirection(config->mScreenLayout) !=
                    (SCREENLAYOUT_SIZE_UNDEFINED | SCREENLAYOUT_LONG_UNDEFINED)
            && GetScreenLayoutNoDirection(mScreenLayout) !=
                    GetScreenLayoutNoDirection(config->mScreenLayout)) {
        *result |= IActivityInfo::CONFIG_SCREEN_LAYOUT;
    }

    if (config->mUiMode != (UI_MODE_TYPE_UNDEFINED | UI_MODE_NIGHT_UNDEFINED)
            && mUiMode != config->mUiMode) {
        *result |= IActivityInfo::CONFIG_UI_MODE;
    }

    if (config->mScreenWidthDp != SCREEN_WIDTH_DP_UNDEFINED
            && mScreenWidthDp != config->mScreenWidthDp) {
        *result |= IActivityInfo::CONFIG_SCREEN_SIZE;
    }

    if (config->mScreenHeightDp != SCREEN_HEIGHT_DP_UNDEFINED
            && mScreenHeightDp != config->mScreenHeightDp) {
        *result |= IActivityInfo::CONFIG_SCREEN_SIZE;
    }

    if (config->mSmallestScreenWidthDp != SMALLEST_SCREEN_WIDTH_DP_UNDEFINED
            && mSmallestScreenWidthDp != config->mSmallestScreenWidthDp) {
        *result |= IActivityInfo::CONFIG_SMALLEST_SCREEN_SIZE;
    }

    if (config->mDensityDpi != DENSITY_DPI_UNDEFINED
            && mDensityDpi != config->mDensityDpi) {
        *result |= IActivityInfo::CONFIG_DENSITY;
    }

    return NOERROR;
}

Boolean CConfiguration::NeedNewResources(
    /* [in] */ Int32 configChanges,
    /* [in] */ Int32 interestingChanges)
{
    return (configChanges & (interestingChanges | IActivityInfo::CONFIG_FONT_SCALE)) != 0;
}

ECode CConfiguration::IsOtherSeqNewer(
    /* [in] */ IConfiguration* other,
    /* [out] */ Boolean* isBetter)
{
    VALIDATE_NOT_NULL(isBetter);

    if (other == NULL) {
        // Sanity check.
        *isBetter = FALSE;
        return NOERROR;
    }

    Int32 otherSeq;
    other->GetSeq(&otherSeq);
    if (otherSeq == 0) {
        // If the other sequence is not specified, then we must assume
        // it is newer since we don't know any better.
        *isBetter = TRUE;
        return NOERROR;
    }

    if (mSeq == 0) {
        // If this sequence is not specified, then we also consider the
        // other is better.  Yes we have a preference for other.  Sue us.
        *isBetter = TRUE;
        return NOERROR;
    }

    Int32 diff = otherSeq - mSeq;
    if (diff > 0x10000) {
        // If there has been a sufficiently large jump, assume the
        // sequence has wrapped around.
        *isBetter = FALSE;
        return NOERROR;
    }

    *isBetter = diff > 0;
    return NOERROR;
}

ECode CConfiguration::WriteToParcel(
    /* [in] */ IParcel * dest)
{
    dest->WriteFloat(mFontScale);
    dest->WriteInt32(mMcc);
    dest->WriteInt32(mMnc);
    if (mLocale == NULL) {
        dest->WriteInt32(0);
    }
    else {
        dest->WriteInt32(1);
        String language;
        String country;
        String variant;
        mLocale->GetLanguage(&language);
        mLocale->GetCountry(&country);
        mLocale->GetVariant(&variant);
        dest->WriteString(language);
        dest->WriteString(country);
        dest->WriteString(variant);
    }
    if (mUserSetLocale) {
        dest->WriteInt32(1);
    }
    else {
        dest->WriteInt32(0);
    }
    dest->WriteInt32(mTouchscreen);
    dest->WriteInt32(mKeyboard);
    dest->WriteInt32(mKeyboardHidden);
    dest->WriteInt32(mHardKeyboardHidden);
    dest->WriteInt32(mNavigation);
    dest->WriteInt32(mNavigationHidden);
    dest->WriteInt32(mOrientation);
    dest->WriteInt32(mScreenLayout);
    dest->WriteInt32(mUiMode);
    dest->WriteInt32(mScreenWidthDp);
    dest->WriteInt32(mScreenHeightDp);
    dest->WriteInt32(mSmallestScreenWidthDp);
    dest->WriteInt32(mDensityDpi);
    dest->WriteInt32(mCompatScreenWidthDp);
    dest->WriteInt32(mCompatScreenHeightDp);
    dest->WriteInt32(mCompatSmallestScreenWidthDp);
    dest->WriteInt32(mSeq);

    return NOERROR;
}

ECode CConfiguration::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadFloat(&mFontScale);
    source->ReadInt32(&mMcc);
    source->ReadInt32(&mMnc);

    Int32 val = 0;
    source->ReadInt32(&val);
    if (val != 0) {
        String language;
        source->ReadString(&language);
        String country;
        source->ReadString(&country);
        String variant;
        source->ReadString(&variant);
        CLocale::New(language, country, variant, (ILocale**)&mLocale);
    }

    Int32 tmpUsl = 0;
    source->ReadInt32(&tmpUsl);
    mUserSetLocale = (tmpUsl == 1);
    source->ReadInt32(&mTouchscreen);
    source->ReadInt32(&mKeyboard);
    source->ReadInt32(&mKeyboardHidden);
    source->ReadInt32(&mHardKeyboardHidden);
    source->ReadInt32(&mNavigation);
    source->ReadInt32(&mNavigationHidden);
    source->ReadInt32(&mOrientation);
    source->ReadInt32(&mScreenLayout);
    source->ReadInt32(&mUiMode);
    source->ReadInt32(&mScreenWidthDp);
    source->ReadInt32(&mScreenHeightDp);
    source->ReadInt32(&mSmallestScreenWidthDp);
    source->ReadInt32(&mDensityDpi);
    source->ReadInt32(&mCompatScreenWidthDp);
    source->ReadInt32(&mCompatScreenHeightDp);
    source->ReadInt32(&mCompatSmallestScreenWidthDp);
    source->ReadInt32(&mSeq);
    return NOERROR;
}

ECode CConfiguration::CompareTo(
    /* [in] */ IInterface* that,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -1;

    if (!that) {
        return NOERROR;
    }

    AutoPtr<IConfiguration> iconfig = IConfiguration::Probe(that);
    if (!iconfig) {
        return NOERROR;
    }

    AutoPtr<CConfiguration> config = (CConfiguration*)iconfig.Get();
    Float a = mFontScale;
    Float b = config->mFontScale;
    if (a < b) {
        *result = -1;
        return NOERROR;
    }
    if (a > b) {
        *result = 1;
        return NOERROR;
    }

    *result = mMcc - config->mMcc;
    if (*result != 0) {
        return NOERROR;
    }

    *result = mMnc - config->mMnc;
    if (*result != 0) {
        return NOERROR;
    }

    if (mLocale == NULL) {
        if (config->mLocale != NULL) {
            *result = 1;
            return NOERROR;
        }
    }
    else if (config->mLocale == NULL) {
        *result = -1;
        return NOERROR;
    }
    else {
        String language;
        mLocale->GetLanguage(&language);
        String languageThat;
        config->mLocale->GetLanguage(&languageThat);
        *result = language.Compare(languageThat);
        if (*result != 0) {
            return NOERROR;
        }

        String country;
        mLocale->GetCountry(&country);
        String countryThat;
        config->mLocale->GetCountry(&countryThat);
        *result = country.Compare(countryThat);
        if (*result != 0) {
            return NOERROR;
        }

        String variant;
        mLocale->GetVariant(&variant);
        String variantThat;
        config->mLocale->GetVariant(&variantThat);
        *result = variant.Compare(variantThat);
        if (*result != 0) {
            return NOERROR;
        }
    }

    *result = mTouchscreen - config->mTouchscreen;
    if (*result != 0) {
        return NOERROR;
    }

    *result = mKeyboard - config->mKeyboard;
    if (*result != 0) {
        return NOERROR;
    }

    *result = mKeyboardHidden - config->mKeyboardHidden;
    if (*result != 0) {
        return NOERROR;
    }

    *result = mHardKeyboardHidden - config->mHardKeyboardHidden;
    if (*result != 0) {
        return NOERROR;
    }

    *result = mNavigation - config->mNavigation;
    if (*result != 0) {
        return NOERROR;
    }

    *result = mNavigationHidden - config->mNavigationHidden;
    if (*result != 0) {
        return NOERROR;
    }

    *result = mOrientation - config->mOrientation;
    if (*result != 0) {
        return NOERROR;
    }

    *result = mScreenLayout - config->mScreenLayout;
    if (*result != 0) {
        return NOERROR;
    }

    *result = mUiMode - config->mUiMode;
    if (*result != 0) {
        return NOERROR;
    }

    *result = mScreenWidthDp - config->mScreenWidthDp;
    if (*result != 0) {
        return NOERROR;
    }

    *result = mScreenHeightDp - config->mScreenHeightDp;
    if (*result != 0) {
        return NOERROR;
    }

    *result = mSmallestScreenWidthDp - config->mSmallestScreenWidthDp;
    if (*result != 0) {
        return NOERROR;
    }

    *result = mDensityDpi - config->mDensityDpi;
    //if (n != 0) return n;

    return NOERROR;
}

ECode CConfiguration::Equals(
    /* [in] */ IInterface* that,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return Equals(IConfiguration::Probe(that), result);
}

ECode CConfiguration::Equals(
    /* [in] */ IConfiguration * that,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (that == NULL) {
        *result = FALSE;
        return NOERROR;

    }
    if (that == this) {
        *result = TRUE;
        return NOERROR;
    }

    Int32 res;
    CompareTo(that, &res);
    *result = (res == 0);
    return NOERROR;
}

ECode CConfiguration::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder sb(128);
    sb += "{";
    sb += mFontScale;
    sb += " ";
    if (mMcc != 0) {
        sb += mMcc;
        sb += "mcc";
    }
    else {
        sb += "?mcc";
    }
    if (mMnc != 0) {
        sb += mMnc;
        sb += "mnc";
    }
    else {
        sb += "?mnc";
    }
    if (mLocale != NULL) {
        sb += " ";
        String strLocale;
        mLocale->ToString(&strLocale);
        sb += strLocale;
    }
    else {
        sb += " ?locale";
    }
    Int32 layoutDir = (mScreenLayout & SCREENLAYOUT_LAYOUTDIR_MASK);
    switch (layoutDir) {
        case SCREENLAYOUT_LAYOUTDIR_UNDEFINED: sb += " ?layoutDir"; break;
        case SCREENLAYOUT_LAYOUTDIR_LTR: sb += " ldltr"; break;
        case SCREENLAYOUT_LAYOUTDIR_RTL: sb += " ldrtl"; break;
        default: sb += " layoutDir=";
            sb += layoutDir >> SCREENLAYOUT_LAYOUTDIR_SHIFT; break;
    }
    if (mSmallestScreenWidthDp != SMALLEST_SCREEN_WIDTH_DP_UNDEFINED) {
        sb += " sw"; sb += mSmallestScreenWidthDp; sb += "dp";
    }
    else {
        sb += " ?swdp";
    }
    if (mScreenWidthDp != SCREEN_WIDTH_DP_UNDEFINED) {
        sb += " w"; sb += mScreenWidthDp; sb += "dp";
    }
    else {
        sb += " ?wdp";
    }
    if (mScreenHeightDp != SCREEN_HEIGHT_DP_UNDEFINED) {
        sb += " h"; sb += mScreenHeightDp; sb += "dp";
    }
    else {
        sb += " ?hdp";
    }
    if (mDensityDpi != DENSITY_DPI_UNDEFINED) {
        sb += " "; sb += mDensityDpi; sb += "dpi";
    }
    else {
        sb += " ?density";
    }
    switch ((mScreenLayout & SCREENLAYOUT_SIZE_MASK)) {
        case SCREENLAYOUT_SIZE_UNDEFINED: sb += " ?lsize"; break;
        case SCREENLAYOUT_SIZE_SMALL: sb += " smll"; break;
        case SCREENLAYOUT_SIZE_NORMAL: sb += " nrml"; break;
        case SCREENLAYOUT_SIZE_LARGE: sb += " lrg"; break;
        case SCREENLAYOUT_SIZE_XLARGE: sb += " xlrg"; break;
        default: sb += " layoutSize=";
                sb += mScreenLayout & SCREENLAYOUT_SIZE_MASK; break;
    }
    switch ((mScreenLayout & SCREENLAYOUT_LONG_MASK)) {
        case SCREENLAYOUT_LONG_UNDEFINED: sb += " ?long"; break;
        case SCREENLAYOUT_LONG_NO: /* not-long is not interesting to print */ break;
        case SCREENLAYOUT_LONG_YES: sb += " long"; break;
        default: sb += " layoutLong=";
                sb += mScreenLayout & SCREENLAYOUT_LONG_MASK; break;
    }
    switch (mOrientation) {
        case ORIENTATION_UNDEFINED: sb += " ?orien"; break;
        case ORIENTATION_LANDSCAPE: sb += " land"; break;
        case ORIENTATION_PORTRAIT: sb += " port"; break;
        default: sb += " orien="; sb += mOrientation; break;
    }
    switch ((mUiMode & UI_MODE_TYPE_MASK)) {
        case UI_MODE_TYPE_UNDEFINED: sb += " ?uimode"; break;
        case UI_MODE_TYPE_NORMAL: /* normal is not interesting to print */ break;
        case UI_MODE_TYPE_DESK: sb += " desk"; break;
        case UI_MODE_TYPE_CAR: sb += " car"; break;
        case UI_MODE_TYPE_TELEVISION: sb += " television"; break;
        case UI_MODE_TYPE_APPLIANCE: sb += " appliance"; break;
        case UI_MODE_TYPE_WATCH: sb += " watch"; break;
        default: sb += " uimode="; sb += mUiMode & UI_MODE_TYPE_MASK; break;
    }
    switch ((mUiMode & UI_MODE_NIGHT_MASK)) {
        case UI_MODE_NIGHT_UNDEFINED: sb += " ?night"; break;
        case UI_MODE_NIGHT_NO: /* not-night is not interesting to print */ break;
        case UI_MODE_NIGHT_YES: sb += " night"; break;
        default: sb += " night="; sb += mUiMode & UI_MODE_NIGHT_MASK; break;
    }
    switch (mTouchscreen) {
        case TOUCHSCREEN_UNDEFINED: sb += " ?touch"; break;
        case TOUCHSCREEN_NOTOUCH: sb += " -touch"; break;
        case TOUCHSCREEN_STYLUS: sb += " stylus"; break;
        case TOUCHSCREEN_FINGER: sb += " finger"; break;
        default: sb += " touch="; sb += mTouchscreen; break;
    }
    switch (mKeyboard) {
        case KEYBOARD_UNDEFINED: sb += " ?keyb"; break;
        case KEYBOARD_NOKEYS: sb += " -keyb"; break;
        case KEYBOARD_QWERTY: sb += " qwerty"; break;
        case KEYBOARD_12KEY: sb += " 12key"; break;
        default: sb += " keys="; sb += mKeyboard; break;
    }
    switch (mKeyboardHidden) {
        case KEYBOARDHIDDEN_UNDEFINED: sb += "/?"; break;
        case KEYBOARDHIDDEN_NO: sb += "/v"; break;
        case KEYBOARDHIDDEN_YES: sb += "/h"; break;
        case KEYBOARDHIDDEN_SOFT: sb += "/s"; break;
        default: sb += "/"; sb += mKeyboardHidden; break;
    }
    switch (mHardKeyboardHidden) {
        case HARDKEYBOARDHIDDEN_UNDEFINED: sb += "/?"; break;
        case HARDKEYBOARDHIDDEN_NO: sb += "/v"; break;
        case HARDKEYBOARDHIDDEN_YES: sb += "/h"; break;
        default: sb += "/"; sb += mHardKeyboardHidden; break;
    }
    switch (mNavigation) {
        case NAVIGATION_UNDEFINED: sb += " ?nav"; break;
        case NAVIGATION_NONAV: sb += " -nav"; break;
        case NAVIGATION_DPAD: sb += " dpad"; break;
        case NAVIGATION_TRACKBALL: sb += " tball"; break;
        case NAVIGATION_WHEEL: sb += " wheel"; break;
        default: sb += " nav="; sb += mNavigation; break;
    }
    switch (mNavigationHidden) {
        case NAVIGATIONHIDDEN_UNDEFINED: sb += "/?"; break;
        case NAVIGATIONHIDDEN_NO: sb += "/v"; break;
        case NAVIGATIONHIDDEN_YES: sb += "/h"; break;
        default: sb += "/"; sb += mNavigationHidden; break;
    }
    if (mSeq != 0) {
        sb += " s.";
        sb += mSeq;
    }
    sb += '}';
    *str = sb.ToString();
    return NOERROR;
}


ECode CConfiguration::SetLocale(
    /* [in] */ ILocale* locale)
{
    mLocale = locale;
    mUserSetLocale = TRUE;
    return SetLayoutDirection(mLocale);
}

ECode CConfiguration::GetThemeConfig(
    /* [out] */ IThemeConfig** themeConfig)
{
    assert(0);
    return NOERROR;
}

ECode CConfiguration::SetThemeConfig(
    /* [in] */ IThemeConfig* themeConfig)
{
    assert(0);
    return NOERROR;
}

ECode CConfiguration::GetLayoutDirection(
    /* [out] */ Int32* dir)
{
    VALIDATE_NOT_NULL(dir);
    *dir = (mScreenLayout & SCREENLAYOUT_LAYOUTDIR_MASK) == SCREENLAYOUT_LAYOUTDIR_RTL
            ? IView::LAYOUT_DIRECTION_RTL : IView::LAYOUT_DIRECTION_LTR;
    return NOERROR;
}

ECode CConfiguration::SetLayoutDirection(
    /* [in] */ ILocale* locale)
{
    // There is a "1" difference between the configuration values for
    // layout direction and View constants for layout direction, just add "1".
    Int32 layoutDirection = 1 + TextUtils::GetLayoutDirectionFromLocale(locale);
    mScreenLayout = (mScreenLayout & ~SCREENLAYOUT_LAYOUTDIR_MASK)|
            (layoutDirection << SCREENLAYOUT_LAYOUTDIR_SHIFT);
    return NOERROR;
}

Int32 CConfiguration::GetScreenLayoutNoDirection(
    /* [in] */ Int32 screenLayout)
{
    return screenLayout & ~SCREENLAYOUT_LAYOUTDIR_MASK;
}

ECode CConfiguration::GetFontScale(
    /* [out] */ Float* fontScale)
{
    VALIDATE_NOT_NULL(fontScale);
    *fontScale = mFontScale;
    return NOERROR;
}

ECode CConfiguration::SetFontScale(
    /* [in] */ Float fontScale)
{
    mFontScale = fontScale;
    return NOERROR;
}

ECode CConfiguration::GetMcc(
    /* [out] */ Int32* mcc)
{
    VALIDATE_NOT_NULL(mcc);
    *mcc = mMcc;
    return NOERROR;
}

ECode CConfiguration::SetMcc(
    /* [in] */ Int32 mcc)
{
    mMcc = mcc;
    return NOERROR;
}

ECode CConfiguration::GetMnc(
    /* [out] */ Int32* mnc)
{
    VALIDATE_NOT_NULL(mnc);
    *mnc = mMnc;
    return NOERROR;
}

ECode CConfiguration::SetMnc(
    /* [in] */ Int32 mnc)
{
    mMnc = mnc;
    return NOERROR;
}

ECode CConfiguration::GetLocale(
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale);
    *locale = mLocale;
    REFCOUNT_ADD(*locale);
    return NOERROR;
}

ECode CConfiguration::IsUserSetLocale(
    /* [out] */ Boolean* userSetLocale)
{
    VALIDATE_NOT_NULL(userSetLocale);
    *userSetLocale = mUserSetLocale;
    return NOERROR;
}

ECode CConfiguration::SetUserSetLocale(
    /* [in] */ Boolean userSetLocale)
{
    mUserSetLocale = userSetLocale;
    return NOERROR;
}

ECode CConfiguration::GetScreenLayout(
    /* [out] */ Int32* layout)
{
    VALIDATE_NOT_NULL(layout);
    *layout = mScreenLayout;
    return NOERROR;
}

ECode CConfiguration::SetScreenLayout(
    /* [in] */ Int32 layout)
{
    mScreenLayout = layout;
    return NOERROR;
}

ECode CConfiguration::GetTouchscreen(
    /* [out] */ Int32* touchscreen)
{
    VALIDATE_NOT_NULL(touchscreen);
    *touchscreen = mTouchscreen;
    return NOERROR;
}

ECode CConfiguration::SetTouchscreen(
    /* [in] */ Int32 touchscreen)
{
    mTouchscreen = touchscreen;
    return NOERROR;
}

ECode CConfiguration::GetKeyboard(
    /* [out] */ Int32* keyboard)
{
    VALIDATE_NOT_NULL(keyboard);
    *keyboard = mKeyboard;
    return NOERROR;
}

ECode CConfiguration::SetKeyboard(
    /* [in] */ Int32 keyboard)
{
    mKeyboard = keyboard;
    return NOERROR;
}

ECode CConfiguration::GetKeyboardHidden(
    /* [out] */ Int32* keyboardHidden)
{
    VALIDATE_NOT_NULL(keyboardHidden);
    *keyboardHidden = mKeyboardHidden;
    return NOERROR;
}

ECode CConfiguration::SetKeyboardHidden(
    /* [in] */ Int32 keyboardHidden)
{
    mKeyboardHidden = keyboardHidden;
    return NOERROR;
}

ECode CConfiguration::GetHardKeyboardHidden(
    /* [out] */ Int32* hardKeyboardHidden)
{
    VALIDATE_NOT_NULL(hardKeyboardHidden);
    *hardKeyboardHidden = mHardKeyboardHidden;
    return NOERROR;
}

ECode CConfiguration::SetHardKeyboardHidden(
    /* [in] */ Int32 hardKeyboardHidden)
{
    mHardKeyboardHidden = hardKeyboardHidden;
    return NOERROR;
}

ECode CConfiguration::GetNavigation(
    /* [out] */ Int32* navigation)
{
    VALIDATE_NOT_NULL(navigation);
    *navigation = mNavigation;
    return NOERROR;
}

ECode CConfiguration::SetNavigation(
    /* [in] */ Int32 navigation)
{
    mNavigation = navigation;
    return NOERROR;
}

ECode CConfiguration::GetNavigationHidden(
    /* [out] */ Int32* navigationHidden)
{
    VALIDATE_NOT_NULL(navigationHidden);
    *navigationHidden = mNavigationHidden;
    return NOERROR;
}

ECode CConfiguration::SetNavigationHidden(
    /* [in] */ Int32 navigationHidden)
{
    mNavigationHidden = navigationHidden;
    return NOERROR;
}

ECode CConfiguration::GetOrientation(
    /* [out] */ Int32* orientation)
{
    VALIDATE_NOT_NULL(orientation);
    *orientation = mOrientation;
    return NOERROR;
}

ECode CConfiguration::SetOrientation(
    /* [in] */ Int32 orientation)
{
    mOrientation = orientation;
    return NOERROR;
}

ECode CConfiguration::GetUiMode(
    /* [out] */ Int32* uiMode)
{
    VALIDATE_NOT_NULL(uiMode);
    *uiMode = mUiMode;
    return NOERROR;
}

ECode CConfiguration::SetUiMode(
    /* [in] */ Int32 uiMode)
{
    mUiMode = uiMode;
    return NOERROR;
}

ECode CConfiguration::GetScreenWidthDp(
    /* [out] */ Int32* screenWidthDp)
{
    VALIDATE_NOT_NULL(screenWidthDp);
    *screenWidthDp = mScreenWidthDp;
    return NOERROR;
}

ECode CConfiguration::SetScreenWidthDp(
    /* [in] */ Int32 screenWidthDp)
{
    mScreenWidthDp = screenWidthDp;
    return NOERROR;
}

ECode CConfiguration::GetScreenHeightDp(
    /* [out] */ Int32* screenHeightDp)
{
    VALIDATE_NOT_NULL(screenHeightDp);
    *screenHeightDp = mScreenHeightDp;
    return NOERROR;
}

ECode CConfiguration::SetScreenHeightDp(
    /* [in] */ Int32 screenHeightDp)
{
    mScreenHeightDp = screenHeightDp;
    return NOERROR;
}

ECode CConfiguration::GetSmallestScreenWidthDp(
    /* [out] */ Int32* smallestScreenWidthDp)
{
    VALIDATE_NOT_NULL(smallestScreenWidthDp);
    *smallestScreenWidthDp = mSmallestScreenWidthDp;
    return NOERROR;
}

ECode CConfiguration::SetSmallestScreenWidthDp(
    /* [in] */ Int32 smallestScreenWidthDp)
{
    mSmallestScreenWidthDp = smallestScreenWidthDp;
    return NOERROR;
}

ECode CConfiguration::GetDensityDpi(
    /* [out] */ Int32* densityDpi)
{
    VALIDATE_NOT_NULL(densityDpi);
    *densityDpi = mDensityDpi;
    return NOERROR;
}

ECode CConfiguration::SetDensityDpi(
    /* [in] */ Int32 densityDpi)
{
    mDensityDpi = densityDpi;
    return NOERROR;
}

ECode CConfiguration::GetSeq(
    /* [out] */ Int32* seq)
{
    VALIDATE_NOT_NULL(seq);
    *seq= mSeq;
    return NOERROR;
}

ECode CConfiguration::SetSeq(
    /* [in] */ Int32 seq)
{
    mSeq = seq;
    return NOERROR;
}

ECode CConfiguration::GetCompatScreenWidthDp(
    /* [out] */ Int32* dp)
{
    VALIDATE_NOT_NULL(dp);
    *dp = mCompatScreenWidthDp;
    return NOERROR;
}

ECode CConfiguration::SetCompatScreenWidthDp(
    /* [in] */ Int32 dp)
{
    mCompatScreenWidthDp = dp;
    return NOERROR;
}

ECode CConfiguration::GetCompatScreenHeightDp(
    /* [out] */ Int32* dp)
{
    VALIDATE_NOT_NULL(dp);
    *dp = mCompatScreenHeightDp;
    return NOERROR;
}

ECode CConfiguration::SetCompatScreenHeightDp(
    /* [in] */ Int32 dp)
{
    mCompatScreenHeightDp = dp;
    return NOERROR;
}

ECode CConfiguration::GetCompatSmallestScreenWidthDp(
    /* [out] */ Int32* dp)
{
    VALIDATE_NOT_NULL(dp);
    *dp = mCompatSmallestScreenWidthDp;
    return NOERROR;
}

ECode CConfiguration::SetCompatSmallestScreenWidthDp(
    /* [in] */ Int32 dp)
{
    mCompatSmallestScreenWidthDp = dp;
    return NOERROR;
}

ECode CConfiguration::constructor()
{
    return SetToDefaults();
}

ECode CConfiguration::constructor(
    /* [in] */ IConfiguration* config)
{
    return SetTo(config);
}

String CConfiguration::LocaleToResourceQualifier(
    /* [in] */ ILocale* locale)
{
    String language, country, script, variant;
    locale->GetLanguage(&language);
    locale->GetCountry(&country);
    locale->GetScript(&script);
    locale->GetVariant(&variant);

    StringBuilder sb;
    Boolean l = !language.IsNullOrEmpty();
    Boolean c = !country.IsNullOrEmpty();
    Boolean s = !script.IsNullOrEmpty();
    Boolean v = !variant.IsNullOrEmpty();

    if (l) {
        sb.Append(language);
        if (c) {
            sb.Append("-r");
            sb.Append(country);
            if (s) {
                sb.Append("-s");
                sb.Append(script);
                if (v) {
                    sb.Append("-v");
                    sb.Append(variant);
                }
            }
        }
    }
    return sb.ToString();
}

String CConfiguration::ResourceQualifierString(
    /* [in] */ IConfiguration* config)
{
    List<String> parts;

    StringBuilder sb;

    Int32 ival;
    config->GetMcc(&ival);
    if (ival != 0) {
        sb += ival;
        sb += "mcc";
        parts.PushBack(sb.ToString());
        config->GetMnc(&ival);
        if (ival != 0) {
            sb.Reset();
            sb += ival;
            sb += "mnc";
            parts.PushBack(sb.ToString());
        }
    }

    AutoPtr<ILocale> locale;
    config->GetLocale((ILocale**)&locale);
    String str;
    locale->GetLanguage(&str);
    if (!str.IsNullOrEmpty()) {
        parts.PushBack(LocaleToResourceQualifier(locale));
    }

    config->GetScreenLayout(&ival);
    switch (ival & IConfiguration::SCREENLAYOUT_LAYOUTDIR_MASK) {
        case IConfiguration::SCREENLAYOUT_LAYOUTDIR_LTR:
            parts.PushBack(String("ldltr"));
            break;
        case IConfiguration::SCREENLAYOUT_LAYOUTDIR_RTL:
            parts.PushBack(String("ldrtl"));
            break;
        default:
            break;
    }

    config->GetSmallestScreenWidthDp(&ival);
    if (ival != 0) {
        sb.Reset();
        sb += "sw";
        sb += ival;
        sb += "dp";
        parts.PushBack(sb.ToString());
    }

    config->GetScreenWidthDp(&ival);
    if (ival != 0) {
        sb.Reset();
        sb += "w";
        sb += ival;
        sb += "dp";
        parts.PushBack(sb.ToString());
    }

    config->GetScreenHeightDp(&ival);
    if (ival != 0) {
        sb.Reset();
        sb += "h";
        sb += ival;
        sb += "dp";
        parts.PushBack(sb.ToString());
    }

    config->GetScreenLayout(&ival);
    switch (ival & IConfiguration::SCREENLAYOUT_SIZE_MASK) {
        case IConfiguration::SCREENLAYOUT_SIZE_SMALL:
            parts.PushBack(String("small"));
            break;
        case IConfiguration::SCREENLAYOUT_SIZE_NORMAL:
            parts.PushBack(String("normal"));
            break;
        case IConfiguration::SCREENLAYOUT_SIZE_LARGE:
            parts.PushBack(String("large"));
            break;
        case IConfiguration::SCREENLAYOUT_SIZE_XLARGE:
            parts.PushBack(String("xlarge"));
            break;
        default:
            break;
    }

    switch (ival & IConfiguration::SCREENLAYOUT_LONG_MASK) {
        case IConfiguration::SCREENLAYOUT_LONG_YES:
            parts.PushBack(String("long"));
            break;
        case IConfiguration::SCREENLAYOUT_LONG_NO:
            parts.PushBack(String("notlong"));
            break;
        default:
            break;
    }

    config->GetOrientation(&ival);
    switch (ival) {
        case IConfiguration::ORIENTATION_LANDSCAPE:
            parts.PushBack(String("land"));
            break;
        case IConfiguration::ORIENTATION_PORTRAIT:
            parts.PushBack(String("port"));
            break;
        default:
            break;
    }

    config->GetUiMode(&ival);
    switch (ival & IConfiguration::UI_MODE_TYPE_MASK) {
        case IConfiguration::UI_MODE_TYPE_APPLIANCE:
            parts.PushBack(String("appliance"));
            break;
        case IConfiguration::UI_MODE_TYPE_DESK:
            parts.PushBack(String("desk"));
            break;
        case IConfiguration::UI_MODE_TYPE_TELEVISION:
            parts.PushBack(String("television"));
            break;
        case IConfiguration::UI_MODE_TYPE_CAR:
            parts.PushBack(String("car"));
            break;
        case IConfiguration::UI_MODE_TYPE_WATCH:
            parts.PushBack(String("watch"));
            break;
        default:
            break;
    }

    switch (ival & IConfiguration::UI_MODE_NIGHT_MASK) {
        case IConfiguration::UI_MODE_NIGHT_YES:
            parts.PushBack(String("night"));
            break;
        case IConfiguration::UI_MODE_NIGHT_NO:
            parts.PushBack(String("notnight"));
            break;
        default:
            break;
    }

    config->GetDensityDpi(&ival);
    switch (ival) {
        case DENSITY_DPI_UNDEFINED:
            break;
        case 120:
            parts.PushBack(String("ldpi"));
            break;
        case 160:
            parts.PushBack(String("mdpi"));
            break;
        case 213:
            parts.PushBack(String("tvdpi"));
            break;
        case 240:
            parts.PushBack(String("hdpi"));
            break;
        case 320:
            parts.PushBack(String("xhdpi"));
            break;
        case 480:
            parts.PushBack(String("xxhdpi"));
            break;
        case 640:
            parts.PushBack(String("xxxhdpi"));
            break;
        case DENSITY_DPI_ANY:
            parts.PushBack(String("anydpi"));
            break;
        case DENSITY_DPI_NONE:
            parts.PushBack(String("nodpi"));
        default: {
            sb.Reset();
            sb += ival;
            sb += "dpi";
            parts.PushBack(sb.ToString());
            break;
        }
    }

    config->GetTouchscreen(&ival);
    switch (ival) {
        case IConfiguration::TOUCHSCREEN_NOTOUCH:
            parts.PushBack(String("notouch"));
            break;
        case IConfiguration::TOUCHSCREEN_FINGER:
            parts.PushBack(String("finger"));
            break;
        default:
            break;
    }

    config->GetKeyboardHidden(&ival);
    switch (ival) {
        case IConfiguration::KEYBOARDHIDDEN_NO:
            parts.PushBack(String("keysexposed"));
            break;
        case IConfiguration::KEYBOARDHIDDEN_YES:
            parts.PushBack(String("keyshidden"));
            break;
        case IConfiguration::KEYBOARDHIDDEN_SOFT:
            parts.PushBack(String("keyssoft"));
            break;
        default:
            break;
    }

    config->GetKeyboard(&ival);
    switch (ival) {
        case IConfiguration::KEYBOARD_NOKEYS:
            parts.PushBack(String("nokeys"));
            break;
        case IConfiguration::KEYBOARD_QWERTY:
            parts.PushBack(String("qwerty"));
            break;
        case IConfiguration::KEYBOARD_12KEY:
            parts.PushBack(String("12key"));
            break;
        default:
            break;
    }

    config->GetNavigationHidden(&ival);
    switch (ival) {
        case IConfiguration::NAVIGATIONHIDDEN_NO:
            parts.PushBack(String("navexposed"));
            break;
        case IConfiguration::NAVIGATIONHIDDEN_YES:
            parts.PushBack(String("navhidden"));
            break;
        default:
            break;
    }

    config->GetNavigation(&ival);
    switch (ival) {
        case IConfiguration::NAVIGATION_NONAV:
            parts.PushBack(String("nonav"));
            break;
        case IConfiguration::NAVIGATION_DPAD:
            parts.PushBack(String("dpad"));
            break;
        case IConfiguration::NAVIGATION_TRACKBALL:
            parts.PushBack(String("trackball"));
            break;
        case IConfiguration::NAVIGATION_WHEEL:
            parts.PushBack(String("wheel"));
            break;
        default:
            break;
    }

    sb.Reset();
    sb += "v";
    sb += Build::VERSION::RESOURCES_SDK_INT;
    parts.PushBack(sb.ToString());

    sb.Reset();
    List<String>::Iterator it = parts.Begin();
    for (; it != parts.End(); ++it) {
        if (it != parts.Begin()) {
            sb += "-";
        }

        sb += *it;
    }

    return sb.ToString();
}

AutoPtr<IConfiguration> CConfiguration::GenerateDelta(
    /* [in] */ IConfiguration* inBase,
    /* [in] */ IConfiguration* inChange)
{
    AutoPtr<CConfiguration> delta;
    CConfiguration::NewByFriend((CConfiguration**)&delta);
    CConfiguration* base = (CConfiguration*)inBase;
    CConfiguration* change = (CConfiguration*)inChange;

    if (base->mFontScale != change->mFontScale) {
        delta->mFontScale = change->mFontScale;
    }

    if (base->mMcc != change->mMcc) {
        delta->mMcc = change->mMcc;
    }

    if (base->mMnc != change->mMnc) {
        delta->mMnc = change->mMnc;
    }

    if ((base->mLocale == NULL && change->mLocale != NULL) ||
            (base->mLocale != NULL && !Object::Equals(base->mLocale, change->mLocale)))  {
        delta->mLocale = change->mLocale;
    }

    if (base->mTouchscreen != change->mTouchscreen) {
        delta->mTouchscreen = change->mTouchscreen;
    }

    if (base->mKeyboard != change->mKeyboard) {
        delta->mKeyboard = change->mKeyboard;
    }

    if (base->mKeyboardHidden != change->mKeyboardHidden) {
        delta->mKeyboardHidden = change->mKeyboardHidden;
    }

    if (base->mNavigation != change->mNavigation) {
        delta->mNavigation = change->mNavigation;
    }

    if (base->mNavigationHidden != change->mNavigationHidden) {
        delta->mNavigationHidden = change->mNavigationHidden;
    }

    if (base->mOrientation != change->mOrientation) {
        delta->mOrientation = change->mOrientation;
    }

    if ((base->mScreenLayout & SCREENLAYOUT_SIZE_MASK) !=
            (change->mScreenLayout & SCREENLAYOUT_SIZE_MASK)) {
        delta->mScreenLayout |= change->mScreenLayout & SCREENLAYOUT_SIZE_MASK;
    }

    if ((base->mScreenLayout & SCREENLAYOUT_LAYOUTDIR_MASK) !=
            (change->mScreenLayout & SCREENLAYOUT_LAYOUTDIR_MASK)) {
        delta->mScreenLayout |= change->mScreenLayout & SCREENLAYOUT_LAYOUTDIR_MASK;
    }

    if ((base->mScreenLayout & SCREENLAYOUT_LONG_MASK) !=
            (change->mScreenLayout & SCREENLAYOUT_LONG_MASK)) {
        delta->mScreenLayout |= change->mScreenLayout & SCREENLAYOUT_LONG_MASK;
    }

    if ((base->mUiMode & UI_MODE_TYPE_MASK) != (change->mUiMode & UI_MODE_TYPE_MASK)) {
        delta->mUiMode |= change->mUiMode & UI_MODE_TYPE_MASK;
    }

    if ((base->mUiMode & UI_MODE_NIGHT_MASK) != (change->mUiMode & UI_MODE_NIGHT_MASK)) {
        delta->mUiMode |= change->mUiMode & UI_MODE_NIGHT_MASK;
    }

    if (base->mScreenWidthDp != change->mScreenWidthDp) {
        delta->mScreenWidthDp = change->mScreenWidthDp;
    }

    if (base->mScreenHeightDp != change->mScreenHeightDp) {
        delta->mScreenHeightDp = change->mScreenHeightDp;
    }

    if (base->mSmallestScreenWidthDp != change->mSmallestScreenWidthDp) {
        delta->mSmallestScreenWidthDp = change->mSmallestScreenWidthDp;
    }

    if (base->mDensityDpi != change->mDensityDpi) {
        delta->mDensityDpi = change->mDensityDpi;
    }
    return (IConfiguration*)delta.Get();
}

ECode CConfiguration::ReadXmlAttrs(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IConfiguration* configOut)
{
    assert(0 && "TODO");
    // configOut->mFontScale = Float.intBitsToFloat(
    //         XmlUtils.readIntAttribute(parser, XML_ATTR_FONT_SCALE, 0));
    // configOut.mcc = XmlUtils.readIntAttribute(parser, XML_ATTR_MCC, 0);
    // configOut.mnc = XmlUtils.readIntAttribute(parser, XML_ATTR_MNC, 0);

    // final String localeStr = XmlUtils.readStringAttribute(parser, XML_ATTR_LOCALE);
    // if (localeStr != NULL) {
    //     configOut->mLocale = Locale.forLanguageTag(localeStr);
    // }

    // configOut.touchscreen = XmlUtils.readIntAttribute(parser, XML_ATTR_TOUCHSCREEN,
    //         TOUCHSCREEN_UNDEFINED);
    // configOut.keyboard = XmlUtils.readIntAttribute(parser, XML_ATTR_KEYBOARD,
    //         KEYBOARD_UNDEFINED);
    // configOut.keyboardHidden = XmlUtils.readIntAttribute(parser, XML_ATTR_KEYBOARD_HIDDEN,
    //         KEYBOARDHIDDEN_UNDEFINED);
    // configOut.hardKeyboardHidden =
    //         XmlUtils.readIntAttribute(parser, XML_ATTR_HARD_KEYBOARD_HIDDEN,
    //                 HARDKEYBOARDHIDDEN_UNDEFINED);
    // configOut.navigation = XmlUtils.readIntAttribute(parser, XML_ATTR_NAVIGATION,
    //         NAVIGATION_UNDEFINED);
    // configOut.navigationHidden = XmlUtils.readIntAttribute(parser, XML_ATTR_NAVIGATION_HIDDEN,
    //         NAVIGATIONHIDDEN_UNDEFINED);
    // configOut.orientation = XmlUtils.readIntAttribute(parser, XML_ATTR_ORIENTATION,
    //         ORIENTATION_UNDEFINED);
    // configOut.screenLayout = XmlUtils.readIntAttribute(parser, XML_ATTR_SCREEN_LAYOUT,
    //         SCREENLAYOUT_UNDEFINED);
    // configOut.uiMode = XmlUtils.readIntAttribute(parser, XML_ATTR_UI_MODE, 0);
    // configOut.screenWidthDp = XmlUtils.readIntAttribute(parser, XML_ATTR_SCREEN_WIDTH,
    //         SCREEN_WIDTH_DP_UNDEFINED);
    // configOut.screenHeightDp = XmlUtils.readIntAttribute(parser, XML_ATTR_SCREEN_HEIGHT,
    //         SCREEN_HEIGHT_DP_UNDEFINED);
    // configOut.smallestScreenWidthDp =
    //         XmlUtils.readIntAttribute(parser, XML_ATTR_SMALLEST_WIDTH,
    //                 SMALLEST_SCREEN_WIDTH_DP_UNDEFINED);
    // configOut.densityDpi = XmlUtils.readIntAttribute(parser, XML_ATTR_DENSITY,
    //         DENSITY_DPI_UNDEFINED);
    return NOERROR;
}

ECode CConfiguration::WriteXmlAttrs(
    /* [in] */ IXmlSerializer* xml,
    /* [in] */ IConfiguration* config)
{
    assert(0 && "TODO");
    // XmlUtils.writeIntAttribute(xml, XML_ATTR_FONT_SCALE,
    //         Float.floatToIntBits(config.fontScale));
    // if (config.mcc != 0) {
    //     XmlUtils.writeIntAttribute(xml, XML_ATTR_MCC, config.mcc);
    // }
    // if (config.mnc != 0) {
    //     XmlUtils.writeIntAttribute(xml, XML_ATTR_MNC, config.mnc);
    // }
    // if (config->mLocale != NULL) {
    //     XmlUtils.writeStringAttribute(xml, XML_ATTR_LOCALE, config->mLocale.toLanguageTag());
    // }
    // if (config.touchscreen != TOUCHSCREEN_UNDEFINED) {
    //     XmlUtils.writeIntAttribute(xml, XML_ATTR_TOUCHSCREEN, config.touchscreen);
    // }
    // if (config.keyboard != KEYBOARD_UNDEFINED) {
    //     XmlUtils.writeIntAttribute(xml, XML_ATTR_KEYBOARD, config.keyboard);
    // }
    // if (config.keyboardHidden != KEYBOARDHIDDEN_UNDEFINED) {
    //     XmlUtils.writeIntAttribute(xml, XML_ATTR_KEYBOARD_HIDDEN, config.keyboardHidden);
    // }
    // if (config.hardKeyboardHidden != HARDKEYBOARDHIDDEN_UNDEFINED) {
    //     XmlUtils.writeIntAttribute(xml, XML_ATTR_HARD_KEYBOARD_HIDDEN,
    //             config.hardKeyboardHidden);
    // }
    // if (config.navigation != NAVIGATION_UNDEFINED) {
    //     XmlUtils.writeIntAttribute(xml, XML_ATTR_NAVIGATION, config.navigation);
    // }
    // if (config.navigationHidden != NAVIGATIONHIDDEN_UNDEFINED) {
    //     XmlUtils.writeIntAttribute(xml, XML_ATTR_NAVIGATION_HIDDEN, config.navigationHidden);
    // }
    // if (config.orientation != ORIENTATION_UNDEFINED) {
    //     XmlUtils.writeIntAttribute(xml, XML_ATTR_ORIENTATION, config.orientation);
    // }
    // if (config.screenLayout != SCREENLAYOUT_UNDEFINED) {
    //     XmlUtils.writeIntAttribute(xml, XML_ATTR_SCREEN_LAYOUT, config.screenLayout);
    // }
    // if (config.uiMode != 0) {
    //     XmlUtils.writeIntAttribute(xml, XML_ATTR_UI_MODE, config.uiMode);
    // }
    // if (config.screenWidthDp != SCREEN_WIDTH_DP_UNDEFINED) {
    //     XmlUtils.writeIntAttribute(xml, XML_ATTR_SCREEN_WIDTH, config.screenWidthDp);
    // }
    // if (config.screenHeightDp != SCREEN_HEIGHT_DP_UNDEFINED) {
    //     XmlUtils.writeIntAttribute(xml, XML_ATTR_SCREEN_HEIGHT, config.screenHeightDp);
    // }
    // if (config.smallestScreenWidthDp != SMALLEST_SCREEN_WIDTH_DP_UNDEFINED) {
    //     XmlUtils.writeIntAttribute(xml, XML_ATTR_SMALLEST_WIDTH, config.smallestScreenWidthDp);
    // }
    // if (config.densityDpi != DENSITY_DPI_UNDEFINED) {
    //     XmlUtils.writeIntAttribute(xml, XML_ATTR_DENSITY, config.densityDpi);
    // }
    return NOERROR;
}

} // res
} // Content
} // Droid
} // Elastos

