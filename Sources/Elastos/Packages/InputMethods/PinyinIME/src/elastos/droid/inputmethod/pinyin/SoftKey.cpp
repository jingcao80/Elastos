
#include "SoftKey.h"
#include "SkbTemplate.h"

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

const Int32 SoftKey::MAX_MOVE_TOLERANCE_X = 0;
const Int32 SoftKey::MAX_MOVE_TOLERANCE_Y = 0;
const Int32 SoftKey::KEYMASK_REPEAT = 0x10000000;
const Int32 SoftKey::KEYMASK_BALLOON = 0x20000000;

CAR_INTERFACE_IMPL(SoftKey, Object, ISoftKey);

SoftKey::SoftKey()
    : mPopupSkbId(0)
    , mLeftF(0.f)
    , mRightF(0.f)
    , mTopF(0.f)
    , mBottomF(0.f)
    , mLeft(0)
    , mRight(0)
    , mTop(0)
    , mBottom(0)
    , mKeyMask(0)
    , mKeyCode(0)
{}

void SoftKey::SetKeyType(
    /* [in] */ SoftKeyType* keyType,
    /* [in] */ IDrawable* keyIcon,
    /* [in] */ IDrawable* keyIconPopup)
{
    mKeyType = keyType;
    mKeyIcon = keyIcon;
    mKeyIconPopup = keyIconPopup;
}

void SoftKey::SetKeyDimensions(
    /* [in] */ Float left,
    /* [in] */ Float top,
    /* [in] */ Float right,
    /* [in] */ Float bottom)
{
    mLeftF = left;
    mTopF = top;
    mRightF = right;
    mBottomF = bottom;
}

void SoftKey::SetKeyAttribute(
    /* [in] */ Int32 keyCode,
    /* [in] */ const String& label,
    /* [in] */ Boolean repeat,
    /* [in] */ Boolean balloon)
{
    mKeyCode = keyCode;
    mKeyLabel = label;

    if (repeat) {
        mKeyMask |= KEYMASK_REPEAT;
    } else {
        mKeyMask &= (~KEYMASK_REPEAT);
    }

    if (balloon) {
        mKeyMask |= KEYMASK_BALLOON;
    } else {
        mKeyMask &= (~KEYMASK_BALLOON);
    }
}

void SoftKey::SetPopupSkbId(
    /* [in] */ Int32 popupSkbId)
{
    mPopupSkbId = popupSkbId;
}

void SoftKey::SetSkbCoreSize(
    /* [in] */ Int32 skbWidth,
    /* [in] */ Int32 skbHeight)
{
    mLeft = (Int32) (mLeftF * skbWidth);
    mRight = (Int32) (mRightF * skbWidth);
    mTop = (Int32) (mTopF * skbHeight);
    mBottom = (Int32) (mBottomF * skbHeight);
}

AutoPtr<IDrawable> SoftKey::GetKeyIcon()
{
    return mKeyIcon;
}

AutoPtr<IDrawable> SoftKey::GetKeyIconPopup()
{
    if (NULL != mKeyIconPopup) {
        return mKeyIconPopup;
    }
    return mKeyIcon;
}

ECode SoftKey::GetKeyCode(
    /* [out] */ Int32* code)
{
    VALIDATE_NOT_NULL(code);
    *code = mKeyCode;
    return NOERROR;
}

ECode SoftKey::GetKeyLabel(
    /* [out] */ String* label)
{
    VALIDATE_NOT_NULL(label);
    *label = mKeyLabel;
    return NOERROR;
}

void SoftKey::ChangeCase(
    /* [in] */ Boolean upperCase)
{
    if (NULL != mKeyLabel) {
        if (upperCase)
            mKeyLabel = mKeyLabel.ToUpperCase();
        else
            mKeyLabel = mKeyLabel.ToLowerCase();
    }
}

AutoPtr<IDrawable> SoftKey::GetKeyBg()
{
    return mKeyType->mKeyBg;
}

AutoPtr<IDrawable> SoftKey::GetKeyHlBg()
{
    return mKeyType->mKeyHlBg;
}

Int32 SoftKey::GetColor()
{
    return mKeyType->mColor;
}

Int32 SoftKey::GetColorHl()
{
    return mKeyType->mColorHl;
}

Int32 SoftKey::GetColorBalloon()
{
    return mKeyType->mColorBalloon;
}

ECode SoftKey::IsKeyCodeKey(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mKeyCode > 0) {
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode SoftKey::IsUserDefKey(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mKeyCode < 0) {
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode SoftKey::IsUniStrKey(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (NULL != mKeyLabel && mKeyCode == 0) {
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

Boolean SoftKey::NeedBalloon()
{
    return (mKeyMask & KEYMASK_BALLOON) != 0;
}

Boolean SoftKey::Repeatable()
{
    return (mKeyMask & KEYMASK_REPEAT) != 0;
}

Int32 SoftKey::GetPopupResId()
{
    return mPopupSkbId;
}

Int32 SoftKey::GetWidth()
{
    return mRight - mLeft;
}

Int32 SoftKey::GetHeight()
{
    return mBottom - mTop;
}

Boolean SoftKey::MoveWithinKey(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    if (mLeft - MAX_MOVE_TOLERANCE_X <= x
            && mTop - MAX_MOVE_TOLERANCE_Y <= y
            && mRight + MAX_MOVE_TOLERANCE_X > x
            && mBottom + MAX_MOVE_TOLERANCE_Y > y) {
        return TRUE;
    }
    return FALSE;
}

ECode NodeSet::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    String str("\n");
    str += String("  keyCode: ") + StringUtils::ToString(mKeyCode) + String("\n");
    str += String("  keyMask: ") + StringUtils::ToString(mKeyMask) + String("\n");
    str += String("  keyLabel: ") + (mKeyLabel == NULL ? String("null") : mKeyLabel) + String("\n");
    str += String("  popupResId: ") + StringUtils::ToString(mPopupSkbId) + String("\n");
    str += String("  Position: ") + StringUtils::ToString(mLeftF) + String(", ")
            + StringUtils::ToString(mTopF) + String(", ") + StringUtils::ToString(mRightF) + String(", ")
            + StringUtils::ToString(mBottomF) + String("\n");

    *info = str;
    return NOERROR;
}

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos

