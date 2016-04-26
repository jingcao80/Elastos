
#include "elastos/droid/inputmethod/pinyin/SoftKey.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

SoftKeyType::SoftKeyType(
    /* [in] */ Int32 id,
    /* [in] */ IDrawable* bg,
    /* [in] */ IDrawable* hlBg)
    : mKeyTypeId(id)
    , mKeyBg(bg)
    , mKeyHlBg(hlBg)
    , mColor(0)
    , mColorHl(0)
    , mColorBalloon(0)
{}

void SoftKeyType::SetColors(
    /* [in] */ Int32 color,
    /* [in] */ Int32 colorHl,
    /* [in] */ Int32 colorBalloon)
{
    mColor = color;
    mColorHl = colorHl;
    mColorBalloon = colorBalloon;
}


const Int32 SoftKey::MAX_MOVE_TOLERANCE_X = 0;
const Int32 SoftKey::MAX_MOVE_TOLERANCE_Y = 0;
const Int32 SoftKey::KEYMASK_REPEAT = 0x10000000;
const Int32 SoftKey::KEYMASK_BALLOON = 0x20000000;

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
    }
    else {
        mKeyMask &= (~KEYMASK_REPEAT);
    }

    if (balloon) {
        mKeyMask |= KEYMASK_BALLOON;
    }
    else {
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

Int32 SoftKey::GetKeyCode()
{
    return mKeyCode;
}

String SoftKey::GetKeyLabel()
{
    return mKeyLabel;
}

void SoftKey::ChangeCase(
    /* [in] */ Boolean upperCase)
{
    if (NULL != mKeyLabel) {
        if (upperCase) {
            mKeyLabel = mKeyLabel.ToUpperCase();
        }
        else {
            mKeyLabel = mKeyLabel.ToLowerCase();
        }
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

Boolean SoftKey::IsKeyCodeKey()
{
    if (mKeyCode > 0) return TRUE;
    return FALSE;
}

Boolean SoftKey::IsUserDefKey()
{
    if (mKeyCode < 0) return TRUE;
    return FALSE;
}

Boolean SoftKey::IsUniStrKey()
{
    if (NULL != mKeyLabel && mKeyCode == 0) return TRUE;
    return FALSE;
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

ECode SoftKey::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    StringBuilder sb("\n");
    sb += "  keyCode: "; sb += StringUtils::ToString(mKeyCode); sb += "\n";
    sb += "  keyMask: "; sb += StringUtils::ToString(mKeyMask); sb += "\n";
    sb += "  keyLabel: "; sb += mKeyLabel.IsNull()? String("NULL") : mKeyLabel; sb += "\n";
    sb += "  popupResId: "; sb += StringUtils::ToString(mPopupSkbId); sb += "\n";
    sb += "  Position: ";
    sb += StringUtils::ToString(mLeftF); sb += ", ";
    sb += StringUtils::ToString(mTopF); sb += ", ";
    sb += StringUtils::ToString(mRightF); sb += ", ";
    sb += StringUtils::ToString(mBottomF); sb += "\n";
    *info = sb.ToString();
    return NOERROR;
}

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos

