
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/view/CWindowManagerLayoutParams.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/Insets.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::Insets;
using Elastos::Droid::Graphics::IInsets;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::StringUtils;
using Elastos::Core::CString;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace View {

CAR_OBJECT_IMPL(CWindowManagerLayoutParams);
CAR_INTERFACE_IMPL_2(CWindowManagerLayoutParams, ViewGroup::LayoutParams, IWindowManagerLayoutParams, IParcelable);
CWindowManagerLayoutParams::CWindowManagerLayoutParams()
    : mX(0)
    , mY(0)
    , mHorizontalWeight(0.0f)
    , mVerticalWeight(0.0f)
    , mType(0)
    , mMemoryType(0)
    , mFlags(0)
    , mPrivateFlags(0)
    , mSoftInputMode(0)
    , mGravity(0)
    , mHorizontalMargin(0.0f)
    , mVerticalMargin(0.0f)
    , mFormat(0)
    , mWindowAnimations(0)
    , mAlpha(1.0f)
    , mDimAmount(1.0f)
    , mScreenBrightness(BRIGHTNESS_OVERRIDE_NONE)
    , mButtonBrightness(BRIGHTNESS_OVERRIDE_NONE)
    , mRotationAnimation(ROTATION_ANIMATION_ROTATE)
    , mScreenOrientation(IActivityInfo::SCREEN_ORIENTATION_UNSPECIFIED)
    , mPreferredRefreshRate(0)
    , mSystemUiVisibility(0)
    , mSubtreeSystemUiVisibility(0)
    , mHasSystemUiListeners(FALSE)
    , mInputFeatures(0)
    , mUserActivityTimeout(-1)
{
    ASSERT_SUCCEEDED(CRect::New((IRect**)&mSurfaceInsets));
    ASSERT_SUCCEEDED(CString::New(String(""), (ICharSequence**)&mTitle));
}

Boolean CWindowManagerLayoutParams::MayUseInputMethod(
    /* [in] */ Int32 flags)
{
    switch (flags & (FLAG_NOT_FOCUSABLE | FLAG_ALT_FOCUSABLE_IM)) {
    case 0:
    case FLAG_NOT_FOCUSABLE |
        FLAG_ALT_FOCUSABLE_IM:
        return TRUE;
    }

    return FALSE;
}

ECode CWindowManagerLayoutParams::SetTitle(
    /* [in] */ ICharSequence* _title)
{
    AutoPtr<ICharSequence> title = _title;
    if (NULL == title)
       FAIL_RETURN(CString::New(String(""), (ICharSequence**)&title));

    mTitle = TextUtils::StringOrSpannedString(title);

    return NOERROR;
}

ECode CWindowManagerLayoutParams::GetTitle(
    /* [out] */ ICharSequence** title)
{
    VALIDATE_NOT_NULL(title);

    *title = mTitle;
    REFCOUNT_ADD(*title);

    return NOERROR;
}

ECode CWindowManagerLayoutParams::CopyFrom(
    /* [in] */ IWindowManagerLayoutParams* source,
    /* [out] */ Int32* changes)
{
    VALIDATE_NOT_NULL(changes);
    if (!source) {
        return E_INVALID_ARGUMENT;
    }

    CWindowManagerLayoutParams* src = (CWindowManagerLayoutParams*)source;
    *changes = 0;

    if (mWidth != src->mWidth) {
        mWidth = src->mWidth;
        *changes |= LAYOUT_CHANGED;
    }

    if (mHeight != src->mHeight) {
        mHeight = src->mHeight;
        *changes |= LAYOUT_CHANGED;
    }

    if (mX != src->mX) {
        mX = src->mX;
        *changes |= LAYOUT_CHANGED;
    }

    if (mY != src->mY) {
        mY = src->mY;
        *changes |= LAYOUT_CHANGED;
    }

    if (mHorizontalWeight != src->mHorizontalWeight) {
        mHorizontalWeight = src->mHorizontalWeight;
        *changes |= LAYOUT_CHANGED;
    }

    if (mVerticalWeight != src->mVerticalWeight) {
        mVerticalWeight = src->mVerticalWeight;
        *changes |= LAYOUT_CHANGED;
    }

    if (mHorizontalMargin != src->mHorizontalMargin) {
        mHorizontalMargin = src->mHorizontalMargin;
        *changes |= LAYOUT_CHANGED;
    }

    if (mVerticalMargin != src->mVerticalMargin) {
        mVerticalMargin = src->mVerticalMargin;
        *changes |= LAYOUT_CHANGED;
    }

    if (mType != src->mType) {
        mType = src->mType;
        *changes |= TYPE_CHANGED;
    }

    if (mFlags != src->mFlags) {
        Int32 diff = mFlags ^ src->mFlags;
        if ((diff & (FLAG_TRANSLUCENT_STATUS | FLAG_TRANSLUCENT_NAVIGATION)) != 0) {
            *changes |= TRANSLUCENT_FLAGS_CHANGED;
        }

        mFlags = src->mFlags;
        *changes |= FLAGS_CHANGED;
    }

    if (mPrivateFlags != src->mPrivateFlags) {
        mPrivateFlags = src->mPrivateFlags;
        *changes |= PRIVATE_FLAGS_CHANGED;
    }

    if (mSoftInputMode != src->mSoftInputMode) {
        mSoftInputMode = src->mSoftInputMode;
        *changes |= SOFT_INPUT_MODE_CHANGED;
    }

    if (mGravity != src->mGravity) {
        mGravity = src->mGravity;
        *changes |= LAYOUT_CHANGED;
    }

    if (mFormat != src->mFormat) {
        mFormat = src->mFormat;
        *changes |= FORMAT_CHANGED;
    }

    if (mWindowAnimations != src->mWindowAnimations) {
        mWindowAnimations = src->mWindowAnimations;
        *changes |= ANIMATION_CHANGED;
    }

    if (mToken == NULL) {
        // NOTE: mToken only copied if the recipient doesn't
        // already have one.
        mToken = src->mToken;
    }

    if (mPackageName.IsNull()) {
        // NOTE: packageName only copied if the recipient doesn't
        // already have one.
        mPackageName = src->mPackageName;
    }

    String title1, title2;
    mTitle->ToString(&title1);
    src->mTitle->ToString(&title2);
    if (!title1.Equals(title2)) {
        mTitle = src->mTitle;
        *changes |= TITLE_CHANGED;
    }

    if (mAlpha != src->mAlpha) {
        mAlpha = src->mAlpha;
        *changes |= ALPHA_CHANGED;
    }

    if (mDimAmount != src->mDimAmount) {
        mDimAmount = src->mDimAmount;
        *changes |= DIM_AMOUNT_CHANGED;
    }

    if (mScreenBrightness != src->mScreenBrightness) {
        mScreenBrightness = src->mScreenBrightness;
        *changes |= SCREEN_BRIGHTNESS_CHANGED;
    }

    if (mButtonBrightness != src->mButtonBrightness) {
        mButtonBrightness = src->mButtonBrightness;
        *changes |= BUTTON_BRIGHTNESS_CHANGED;
    }

    if (mRotationAnimation != src->mRotationAnimation) {
        mRotationAnimation = src->mRotationAnimation;
        *changes |= ROTATION_ANIMATION_CHANGED;
    }

    if (mScreenOrientation != src->mScreenOrientation) {
        mScreenOrientation = src->mScreenOrientation;
        *changes |= SCREEN_ORIENTATION_CHANGED;
    }

    if (mPreferredRefreshRate != src->mPreferredRefreshRate) {
        mPreferredRefreshRate = src->mPreferredRefreshRate;
        *changes |= PREFERRED_REFRESH_RATE_CHANGED;
    }

    if (mSystemUiVisibility != src->mSystemUiVisibility
        || mSubtreeSystemUiVisibility != src->mSubtreeSystemUiVisibility) {
        mSystemUiVisibility = src->mSystemUiVisibility;
        mSubtreeSystemUiVisibility = src->mSubtreeSystemUiVisibility;
        *changes |= SYSTEM_UI_VISIBILITY_CHANGED;
    }

    if (mHasSystemUiListeners != src->mHasSystemUiListeners) {
        mHasSystemUiListeners = src->mHasSystemUiListeners;
        *changes |= SYSTEM_UI_LISTENER_CHANGED;
    }

    if (mInputFeatures != src->mInputFeatures) {
        mInputFeatures = src->mInputFeatures;
        *changes |= INPUT_FEATURES_CHANGED;
    }

    if (mUserActivityTimeout != src->mUserActivityTimeout) {
        mUserActivityTimeout = src->mUserActivityTimeout;
        *changes |= USER_ACTIVITY_TIMEOUT_CHANGED;
    }

    Boolean equals = FALSE;
    if (mSurfaceInsets->Equals(src->mSurfaceInsets, &equals), !equals) {
        mSurfaceInsets->Set(src->mSurfaceInsets);
        *changes |= SURFACE_INSETS_CHANGED;
    }

    return NOERROR;
}

ECode CWindowManagerLayoutParams::Scale(
    /* [in] */ Float scale)
{
    mX = (Int32)(mX * scale + 0.5f);
    mY = (Int32)(mY * scale + 0.5f);

    if (mWidth > 0) {
        mWidth = (Int32)(mWidth * scale + 0.5f);
    }

    if (mHeight > 0) {
        mHeight = (Int32)(mHeight * scale + 0.5f);
    }

    return NOERROR;
}

ECode CWindowManagerLayoutParams::Backup()
{
    if (mCompatibilityParamsBackup == NULL) {
        // we backup 4 elements, x, y, mWidth, mHeight
        mCompatibilityParamsBackup = ArrayOf<Int32>::Alloc(4);
    }
    (*mCompatibilityParamsBackup)[0] = mX;
    (*mCompatibilityParamsBackup)[1] = mY;
    (*mCompatibilityParamsBackup)[2] = mWidth;
    (*mCompatibilityParamsBackup)[3] = mHeight;

    return NOERROR;
}

ECode CWindowManagerLayoutParams::Restore()
{
    if (mCompatibilityParamsBackup != NULL) {
        mX = (*mCompatibilityParamsBackup)[0];
        mY = (*mCompatibilityParamsBackup)[1];
        mWidth = (*mCompatibilityParamsBackup)[2];
        mHeight = (*mCompatibilityParamsBackup)[3];
    }

    return NOERROR;
}

ECode CWindowManagerLayoutParams::GetX(
    /* [out] */ Int32* x)
{
    VALIDATE_NOT_NULL(x);
    *x = mX;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::SetX(
    /* [in] */ Int32 x)
{
    mX = x;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::GetY(
    /* [out] */ Int32* y)
{
    VALIDATE_NOT_NULL(y);
    *y = mY;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::SetY(
    /* [in] */ Int32 y)
{
    mY = y;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::GetType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);
    *type = mType;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::SetType(
    /* [in] */ Int32 type)
{
    mType = type;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::GetMemoryType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);
    *type = mMemoryType;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::SetMemoryType(
    /* [in] */ Int32 type)
{
    mMemoryType = type;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::GetFlags(
    /* [out] */ Int32* flags)
{
    VALIDATE_NOT_NULL(flags);
    *flags = mFlags;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::SetFlags(
    /* [in] */ Int32 flags)
{
    mFlags = flags;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::GetPrivateFlags(
    /* [out] */ Int32* privateFlags)
{
    VALIDATE_NOT_NULL(privateFlags);
    *privateFlags = mPrivateFlags;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::SetPrivateFlags(
    /* [in] */ Int32 privateFlags)
{
    mPrivateFlags = privateFlags;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::SetSoftInputMode(
    /* [in] */ Int32 mode)
{
    mSoftInputMode = mode;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::GetSoftInputMode(
    /* [out] */ Int32* mode)
{
    VALIDATE_NOT_NULL(mode);
    *mode = mSoftInputMode;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::GetGravity(
    /* [out] */ Int32* gravity)
{
    VALIDATE_NOT_NULL(gravity);
    *gravity = mGravity;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::SetGravity(
    /* [in] */ Int32 gravity)
{
    mGravity = gravity;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::GetHorizontalMargin(
    /* [out] */ Float* horizontalMargin)
{
    VALIDATE_NOT_NULL(horizontalMargin);
    *horizontalMargin = mHorizontalMargin;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::SetHorizontalMargin(
    /* [in] */ Float horizontalMargin)
{
    mHorizontalMargin = horizontalMargin;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::GetVerticalMargin(
    /* [out] */ Float* verticalMargin)
{
    VALIDATE_NOT_NULL(verticalMargin);
    *verticalMargin = mVerticalMargin;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::SetVerticalMargin(
    /* [in] */ Float verticalMargin)
{
    mVerticalMargin = verticalMargin;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::GetSurfaceInsets(
    /* [out] */ IRect** surfaceInsets)
{
    VALIDATE_NOT_NULL(surfaceInsets)
    *surfaceInsets = mSurfaceInsets;
    REFCOUNT_ADD(*surfaceInsets)
    return NOERROR;
}

ECode CWindowManagerLayoutParams::SetSurfaceInsets(
    /* [in] */ IRect* surfaceInsets)
{
    mSurfaceInsets = surfaceInsets;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::GetFormat(
    /* [out] */ Int32* format)
{
    VALIDATE_NOT_NULL(format);
    *format = mFormat;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::SetFormat(
    /* [in] */ Int32 format)
{
    mFormat = format;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::GetWindowAnimations(
     /* [out] */ Int32* animations)
{
    VALIDATE_NOT_NULL(animations);

    *animations = mWindowAnimations;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::SetWindowAnimations(
    /* [in] */ Int32 animations)
{
    mWindowAnimations = animations;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::GetAlpha(
    /* [out] */ Float* alpha)
{
    VALIDATE_NOT_NULL(alpha);
    *alpha = mAlpha;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::SetAlpha(
    /* [in] */ Float alpha)
{
    mAlpha = alpha;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::GetDimAmount(
    /* [out] */ Float* dimAmount)
{
    VALIDATE_NOT_NULL(dimAmount);
    *dimAmount = mDimAmount;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::SetDimAmount(
    /* [in] */ Float dimAmount)
{
    mDimAmount = dimAmount;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::GetScreenBrightness(
    /* [out] */ Float* brightness)
{
    VALIDATE_NOT_NULL(brightness);
    *brightness = mScreenBrightness;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::SetScreenBrightness(
    /* [in] */ Float brightness)
{
    mScreenBrightness = brightness;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::GetButtonBrightness(
    /* [out] */ Float* brightness)
{
    VALIDATE_NOT_NULL(brightness);
    *brightness = mButtonBrightness;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::SetButtonBrightness(
    /* [in] */ Float brightness)
{
    mButtonBrightness = brightness;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::GetToken(
    /* [out] */ IBinder** token)
{
    VALIDATE_NOT_NULL(token);
    *token = mToken;
    REFCOUNT_ADD(*token);
    return NOERROR;
}

ECode CWindowManagerLayoutParams::SetToken(
    /* [in] */ IBinder* token)
{
    mToken = token;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::GetPackageName(
    /* [out] */ String* packageName)
{
    VALIDATE_NOT_NULL(packageName);
    *packageName = mPackageName;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::SetPackageName(
    /* [in] */ const String& packageName)
{
    mPackageName = packageName;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::GetScreenOrientation(
    /* [out] */ Int32* orientation)
{
    VALIDATE_NOT_NULL(orientation);
    *orientation = mScreenOrientation;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::SetScreenOrientation(
    /* [in] */ Int32 orientation)
{
    mScreenOrientation = orientation;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::GetPreferredRefreshRate(
    /* [out] */ Float* rate)
{
    VALIDATE_NOT_NULL(rate)
    *rate = mPreferredRefreshRate;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::SetPreferredRefreshRate(
    /* [in] */ Float rate)
{
    mPreferredRefreshRate = rate;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::GetSystemUiVisibility(
    /* [out] */ Int32* systemUiVisibility)
{
    VALIDATE_NOT_NULL(systemUiVisibility);
    *systemUiVisibility = mSystemUiVisibility;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::SetSystemUiVisibility(
    /* [in] */ Int32 systemUiVisibility)
{
    mSystemUiVisibility = systemUiVisibility;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::GetSubtreeSystemUiVisibility(
    /* [out] */ Int32* subtreeSystemUiVisibility)
{
    VALIDATE_NOT_NULL(subtreeSystemUiVisibility);
    *subtreeSystemUiVisibility = mSubtreeSystemUiVisibility;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::SetSubtreeSystemUiVisibility(
    /* [in] */ Int32 subtreeSystemUiVisibility)
{
    mSubtreeSystemUiVisibility = subtreeSystemUiVisibility;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::GetHasSystemUiListeners(
    /* [out] */ Boolean* hasSystemUiListeners)
{
    VALIDATE_NOT_NULL(hasSystemUiListeners);
    *hasSystemUiListeners = mHasSystemUiListeners;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::SetHasSystemUiListeners(
    /* [in] */ Boolean hasSystemUiListeners)
{
    mHasSystemUiListeners = hasSystemUiListeners;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::GetInputFeatures(
    /* [out] */ Int32* inputFeatures)
{
    VALIDATE_NOT_NULL(inputFeatures);
    *inputFeatures = mInputFeatures;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::SetInputFeatures(
    /* [in] */ Int32 inputFeatures)
{
    mInputFeatures = inputFeatures;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::GetUserActivityTimeout(
    /* [out] */ Int32* userActivityTimeout)
{
    VALIDATE_NOT_NULL(userActivityTimeout);
    *userActivityTimeout = mUserActivityTimeout;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::SetUserActivityTimeout(
    /* [in] */ Int32 userActivityTimeout)
{
    mUserActivityTimeout = userActivityTimeout;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::ReadFromParcel(
    /* [in] */ IParcel *source)
{
    if (!source) {
        return E_INVALID_ARGUMENT;
    }

    source->ReadInt32(&mWidth);
    source->ReadInt32(&mHeight);
    source->ReadInt32(&mX);
    source->ReadInt32(&mY);
    source->ReadInt32(&mType);
    source->ReadInt32(&mFlags);
    source->ReadInt32(&mPrivateFlags);
    source->ReadInt32(&mSoftInputMode);
    source->ReadInt32(&mGravity);
    source->ReadFloat(&mHorizontalMargin);
    source->ReadFloat(&mVerticalMargin);
    source->ReadInt32(&mFormat);
    source->ReadInt32(&mWindowAnimations);
    source->ReadFloat(&mAlpha);
    source->ReadFloat(&mDimAmount);
    source->ReadFloat(&mScreenBrightness);
    source->ReadFloat(&mButtonBrightness);
    source->ReadInt32(&mRotationAnimation);
    source->ReadInterfacePtr((Handle32*)&mToken);
    source->ReadString(&mPackageName);
    mTitle = NULL;
    source->ReadInterfacePtr((Handle32*)&mTitle);
    source->ReadInt32(&mScreenOrientation);
    source->ReadFloat(&mPreferredRefreshRate);
    source->ReadInt32(&mSystemUiVisibility);
    source->ReadInt32(&mSubtreeSystemUiVisibility);
    source->ReadBoolean(&mHasSystemUiListeners);
    source->ReadInt32(&mInputFeatures);
    source->ReadInt64(&mUserActivityTimeout);
    Int32 left = 0, top = 0, right = 0, bottom = 0;
    source->ReadInt32(&left);
    source->ReadInt32(&top);
    source->ReadInt32(&right);
    source->ReadInt32(&bottom);
    mSurfaceInsets->Set(left, top, right, bottom);

    return NOERROR;
}

ECode CWindowManagerLayoutParams::WriteToParcel(
    /* [in] */ IParcel *dest)
{
    if (!dest) {
        return E_INVALID_ARGUMENT;
    }

    dest->WriteInt32(mWidth);
    dest->WriteInt32(mHeight);
    dest->WriteInt32(mX);
    dest->WriteInt32(mY);
    dest->WriteInt32(mType);
    dest->WriteInt32(mFlags);
    dest->WriteInt32(mPrivateFlags);
    dest->WriteInt32(mSoftInputMode);
    dest->WriteInt32(mGravity);
    dest->WriteFloat(mHorizontalMargin);
    dest->WriteFloat(mVerticalMargin);
    dest->WriteInt32(mFormat);
    dest->WriteInt32(mWindowAnimations);
    dest->WriteFloat(mAlpha);
    dest->WriteFloat(mDimAmount);
    dest->WriteFloat(mScreenBrightness);
    dest->WriteFloat(mButtonBrightness);
    dest->WriteInt32(mRotationAnimation);
    dest->WriteInterfacePtr((IInterface*)mToken.Get());
    dest->WriteString(mPackageName);
    dest->WriteInterfacePtr((IInterface*)mTitle.Get());
    dest->WriteInt32(mScreenOrientation);
    dest->WriteFloat(mPreferredRefreshRate);
    dest->WriteInt32(mSystemUiVisibility);
    dest->WriteInt32(mSubtreeSystemUiVisibility);
    dest->WriteBoolean(mHasSystemUiListeners);
    dest->WriteInt32(mInputFeatures);
    dest->WriteInt64(mUserActivityTimeout);
    Int32 left = 0, top = 0, right = 0, bottom = 0;
    mSurfaceInsets->Get(&left, &top, &right, &bottom);
    dest->WriteInt32(left);
    dest->WriteInt32(top);
    dest->WriteInt32(right);
    dest->WriteInt32(bottom);

    return NOERROR;
}

ECode CWindowManagerLayoutParams::constructor()
{
    ViewGroup::LayoutParams::constructor(MATCH_PARENT, MATCH_PARENT);

    mType = TYPE_APPLICATION;
    mFormat = IPixelFormat::OPAQUE;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::constructor(
    /* [in] */ Int32 type)
{
    ViewGroup::LayoutParams::constructor(MATCH_PARENT, MATCH_PARENT);

    mType = type;
    mFormat = IPixelFormat::OPAQUE;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::constructor(
    /* [in] */ Int32 type,
    /* [in] */ Int32 flags)
{
    ViewGroup::LayoutParams::constructor(MATCH_PARENT, MATCH_PARENT);

    mType = type;
    mFlags = flags;
    mFormat = IPixelFormat::OPAQUE;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::constructor(
    /* [in] */ Int32 type,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 format)
{
    ViewGroup::LayoutParams::constructor(MATCH_PARENT, MATCH_PARENT);

    mType = type;
    mFlags = flags;
    mFormat = format;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::constructor(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 type,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 format)
{
    ViewGroup::LayoutParams::constructor(w, h);

    mType = type;
    mFlags = flags;
    mFormat = format;
    return NOERROR;
}

ECode CWindowManagerLayoutParams::constructor(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 xpos,
    /* [in] */ Int32 ypos,
    /* [in] */ Int32 type,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 format)
{
    ViewGroup::LayoutParams::constructor(w, h);
    mX = xpos;
    mY = ypos;
    mType = type;
    mFlags = flags;
    mFormat = format;
    return NOERROR;
}

// ECode CWindowManagerLayoutParams::constructor(
//    /* [in] */ IParcel *in)
// {
//     return ReadFromParcel(in);
// }

ECode CWindowManagerLayoutParams::ToString(
    /* [out] */ String* description)
{
    VALIDATE_NOT_NULL(description);
    AutoPtr<StringBuilder> sb = new StringBuilder(256);
    sb->Append("WM.LayoutParams{");
    sb->Append("(");
    sb->Append(mX);
    sb->Append(",");
    sb->Append(mY);
    sb->Append(")(");
    if (mWidth == MATCH_PARENT) {
        sb->Append("fill");
    }
    else if (mWidth == WRAP_CONTENT) {
        sb->Append("wrap");
    }
    else {
        sb->Append(mWidth);
    }
    sb->Append("x");
    if (mHeight == MATCH_PARENT) {
        sb->Append("fill");
    }
    else if (mHeight == WRAP_CONTENT) {
        sb->Append("wrap");
    }
    else {
        sb->Append(mHeight);
    }
    sb->Append(")");
    if (mHorizontalMargin != 0) {
        sb->Append(" hm=");
        sb->Append(mHorizontalMargin);
    }
    if (mVerticalMargin != 0) {
        sb->Append(" vm=");
        sb->Append(mVerticalMargin);
    }
    if (mGravity != 0) {
        sb->Append(" gr=#");
        sb->Append(StringUtils::ToString(mGravity, 16));
    }
    if (mSoftInputMode != 0) {
        sb->Append(" sim=#");
        sb->Append(StringUtils::ToString(mSoftInputMode, 16));
    }
    sb->Append(" ty=");
    sb->Append(mType);
    sb->Append(" fl=#");
    sb->Append(StringUtils::ToString(mFlags, 16));
    if (mPrivateFlags != 0) {
        if ((mPrivateFlags & PRIVATE_FLAG_COMPATIBLE_WINDOW) != 0) {
            sb->Append(" compatible=true");
        }
        sb->Append(" pfl=0x");
        sb->Append(StringUtils::ToString(mPrivateFlags, 16));
    }
    if (mFormat != IPixelFormat::OPAQUE) {
        sb->Append(" fmt=");
        sb->Append(mFormat);
    }
    if (mWindowAnimations != 0) {
        sb->Append(" wanim=0x");
        sb->Append(StringUtils::ToString(mWindowAnimations, 16));
    }
    if (mScreenOrientation != IActivityInfo::SCREEN_ORIENTATION_UNSPECIFIED) {
        sb->Append(" or=");
        sb->Append(mScreenOrientation);
    }
    if (mAlpha != 1.0f) {
        sb->Append(" alpha=");
        sb->Append(mAlpha);
    }
    if (mScreenBrightness != BRIGHTNESS_OVERRIDE_NONE) {
        sb->Append(" sbrt=");
        sb->Append(mScreenBrightness);
    }
    if (mButtonBrightness != BRIGHTNESS_OVERRIDE_NONE) {
        sb->Append(" bbrt=");
        sb->Append(mButtonBrightness);
    }
    if (mRotationAnimation != ROTATION_ANIMATION_ROTATE) {
        sb->Append(" rotAnim=");
        sb->Append(mRotationAnimation);
    }
    if (mPreferredRefreshRate != 0) {
        sb->Append(" preferredRefreshRate=");
        sb->Append(mPreferredRefreshRate);
    }
    if (mSystemUiVisibility != 0) {
        sb->Append(" sysui=0x");
        sb->Append(StringUtils::ToString(mSystemUiVisibility, 16));
    }
    if (mSubtreeSystemUiVisibility != 0) {
        sb->Append(" vsysui=0x");
        sb->Append(StringUtils::ToString(mSubtreeSystemUiVisibility, 16));
    }
    if (mHasSystemUiListeners) {
        sb->Append(" sysuil=");
        sb->Append(mHasSystemUiListeners);
    }
    if (mInputFeatures != 0) {
        sb->Append(" if=0x");
        sb->Append(StringUtils::ToString(mInputFeatures, 16));
    }
    if (mUserActivityTimeout >= 0) {
        sb->Append(" userActivityTimeout=");
        sb->Append(mUserActivityTimeout);
    }
    Boolean equals = FALSE;
    if (mSurfaceInsets->Equals((IInsets*)Insets::NONE, &equals), !equals) {
        sb->Append(" surfaceInsets=");
        sb->Append(mSurfaceInsets.Get());
    }
    sb->Append("}");
    *description = sb->ToString();

    return NOERROR;
}

} // namespace View
} // namepsace Droid
} // namespace Elastos
