
#include "elastos/droid/inputmethod/pinyin/CSoftKeyboardView.h"
#include "elastos/droid/inputmethod/pinyin/CBalloonHint.h"
#include "elastos/droid/inputmethod/pinyin/CSkbContainer.h"
#include "elastos/droid/inputmethod/pinyin/Environment.h"
#include "elastos/droid/inputmethod/pinyin/Settings.h"
#include "elastos/droid/inputmethod/pinyin/SoftKey.h"
#include "elastos/droid/inputmethod/pinyin/SoftKeyboard.h"
#include "elastos/droid/inputmethod/pinyin/SoundManager.h"

using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CRect;

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

CAR_OBJECT_IMPL(CSoftKeyboardView);

CAR_INTERFACE_IMPL(CSoftKeyboardView, View, ISoftKeyboardView);

CSoftKeyboardView::CSoftKeyboardView()
    : mKeyPressed(FALSE)
    , mNormalKeyTextSize(0)
    , mFunctionKeyTextSize(0)
    , mRepeatForLongPress(FALSE)
    , mMovingNeverHidePopupBalloon(FALSE)
    , mDimSkb(FALSE)
{
    mOffsetToSkbContainer = ArrayOf<Int32>::Alloc(2);
    mHintLocationToSkbContainer = ArrayOf<Int32>::Alloc(2);
    mVibratePattern = ArrayOf<Int64>::Alloc(2);
    (*mVibratePattern)[0] = 1;
    (*mVibratePattern)[1] = 20;
    CRect::New((IRect**)&mDirtyRect);
}

ECode CSoftKeyboardView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    FAIL_RETURN(View::constructor(context, attrs));

    mSoundManager = SoundManager::GetInstance(mContext);

    CPaint::New((IPaint**)&mPaint);
    mPaint->SetAntiAlias(TRUE);
    mPaint->GetFontMetricsInt((IPaintFontMetricsInt**)&mFmi);
    return NOERROR;
}

Boolean CSoftKeyboardView::SetSoftKeyboard(
    /* [in] */ SoftKeyboard* softSkb)
{
    if (NULL == softSkb) {
        return FALSE;
    }
    mSoftKeyboard = softSkb;
    AutoPtr<IDrawable> bg = mSoftKeyboard->GetSkbBackground();
    if (NULL != bg) SetBackgroundDrawable(bg);
    return TRUE;
}

AutoPtr<SoftKeyboard> CSoftKeyboardView::GetSoftKeyboard()
{
    return mSoftKeyboard;
}

void CSoftKeyboardView::ResizeKeyboard(
    /* [in] */ Int32 skbWidth,
    /* [in] */ Int32 skbHeight)
{
    mSoftKeyboard->SetSkbCoreSize(skbWidth, skbHeight);
}

void CSoftKeyboardView::SetBalloonHint(
    /* [in] */ CBalloonHint* balloonOnKey,
    /* [in] */ CBalloonHint* balloonPopup,
    /* [in] */ Boolean movingNeverHidePopup)
{
    mBalloonOnKey = balloonOnKey;
    mBalloonPopup = balloonPopup;
    mMovingNeverHidePopupBalloon = movingNeverHidePopup;
}

void CSoftKeyboardView::SetOffsetToSkbContainer(
    /* [in] */ ArrayOf<Int32>* offsetToSkbContainer)
{
    (*mOffsetToSkbContainer)[0] = (*offsetToSkbContainer)[0];
    (*mOffsetToSkbContainer)[1] = (*offsetToSkbContainer)[1];
}

ECode CSoftKeyboardView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 measuredWidth = 0;
    Int32 measuredHeight = 0;
    if (NULL != mSoftKeyboard) {
        measuredWidth = mSoftKeyboard->GetSkbCoreWidth();
        measuredHeight = mSoftKeyboard->GetSkbCoreHeight();
        measuredWidth += mPaddingLeft + mPaddingRight;
        measuredHeight += mPaddingTop + mPaddingBottom;
    }
    SetMeasuredDimension(measuredWidth, measuredHeight);
    return NOERROR;
}

void CSoftKeyboardView::ShowBalloon(
    /* [in] */ CBalloonHint* balloon,
    /* [in] */ ArrayOf<Int32>* balloonLocationToSkb,
    /* [in] */ Boolean movePress)
{
    Int64 delay = CBalloonHint::TIME_DELAY_SHOW;
    if (movePress) delay = 0;

    if (balloon->NeedForceDismiss()) {
        balloon->DelayedDismiss(0);
    }
    Boolean result = FALSE;
    if (balloon->IsShowing(&result), !result) {
        balloon->DelayedShow(delay, balloonLocationToSkb);
    }
    else {
        Int32 width = 0, height = 0;
        balloon->GetWidth(&width);
        balloon->GetHeight(&height);
        balloon->DelayedUpdate(delay, balloonLocationToSkb, width, height);
    }
    // AutoPtr<ISystem> system;
    // Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    // Int64 b;
    // system->GetCurrentTimeMillis(&b);
}

void CSoftKeyboardView::ResetKeyPress(
    /* [in] */ Int64 balloonDelay)
{
    if (!mKeyPressed) return;
    mKeyPressed = FALSE;
    if (NULL != mBalloonOnKey) {
        mBalloonOnKey->DelayedDismiss(balloonDelay);
    }
    else {
        if (NULL != mSoftKeyDown) {
            Boolean empty = FALSE;
            if (mDirtyRect->IsEmpty(&empty), empty) {
                mDirtyRect->Set(mSoftKeyDown->mLeft, mSoftKeyDown->mTop,
                        mSoftKeyDown->mRight, mSoftKeyDown->mBottom);
            }
            Invalidate(mDirtyRect);
        }
        else {
            Invalidate();
        }
    }
    return mBalloonPopup->DelayedDismiss(balloonDelay);
}

AutoPtr<SoftKey> CSoftKeyboardView::OnKeyPress(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ HandlerRunnable* longPressTimer,
    /* [in] */ Boolean movePress)
{
    mKeyPressed = FALSE;
    Boolean moveWithinPreviousKey = FALSE;
    if (movePress) {
        AutoPtr<SoftKey> newKey = mSoftKeyboard->MapToKey(x, y);
        if (newKey == mSoftKeyDown) moveWithinPreviousKey = TRUE;
        mSoftKeyDown = newKey;
    }
    else {
        mSoftKeyDown = mSoftKeyboard->MapToKey(x, y);
    }
    if (moveWithinPreviousKey || NULL == mSoftKeyDown) return mSoftKeyDown;
    mKeyPressed = TRUE;

    if (!movePress) {
        TryPlayKeyDown();
        TryVibrate();
    }

    mLongPressTimer = longPressTimer;

    if (!movePress) {
        if (mSoftKeyDown->GetPopupResId() > 0 || mSoftKeyDown->Repeatable()) {
            ((CSkbContainer::LongPressTimer*)mLongPressTimer.Get())->StartTimer();
        }
    }
    else {
        ((CSkbContainer::LongPressTimer*)mLongPressTimer.Get())->RemoveTimer();
    }

    Int32 desired_width;
    Int32 desired_height;
    Float textSize;
    AutoPtr<Environment> env = Environment::GetInstance();

    if (NULL != mBalloonOnKey) {
        AutoPtr<IDrawable> keyHlBg = mSoftKeyDown->GetKeyHlBg();
        mBalloonOnKey->SetBalloonBackground(keyHlBg);

        // Prepare the on-key balloon
        Int32 keyXMargin = mSoftKeyboard->GetKeyXMargin();
        Int32 keyYMargin = mSoftKeyboard->GetKeyYMargin();
        desired_width = mSoftKeyDown->GetWidth() - 2 * keyXMargin;
        desired_height = mSoftKeyDown->GetHeight() - 2 * keyYMargin;
        textSize = env->GetKeyTextSize(
                SoftKeyType::KEYTYPE_ID_NORMAL_KEY != mSoftKeyDown->mKeyType->mKeyTypeId);
        AutoPtr<IDrawable> icon = mSoftKeyDown->GetKeyIcon();
        if (NULL != icon) {
            mBalloonOnKey->SetBalloonConfig(icon, desired_width,
                    desired_height);
        }
        else {
            mBalloonOnKey->SetBalloonConfig(mSoftKeyDown->GetKeyLabel(),
                    textSize, TRUE, mSoftKeyDown->GetColorHl(),
                    desired_width, desired_height);
        }

        Int32 value = 0;
        (*mHintLocationToSkbContainer)[0] = mPaddingLeft + mSoftKeyDown->mLeft
                - ((mBalloonOnKey->GetWidth(&value), value) - mSoftKeyDown->GetWidth()) / 2;
        (*mHintLocationToSkbContainer)[0] += (*mOffsetToSkbContainer)[0];
        (*mHintLocationToSkbContainer)[1] = mPaddingTop
                + (mSoftKeyDown->mBottom - keyYMargin)
                - (mBalloonOnKey->GetHeight(&value), value);
        (*mHintLocationToSkbContainer)[1] += (*mOffsetToSkbContainer)[1];
        ShowBalloon(mBalloonOnKey, mHintLocationToSkbContainer, movePress);
    }
    else {
        mDirtyRect->Union(mSoftKeyDown->mLeft, mSoftKeyDown->mTop,
                mSoftKeyDown->mRight, mSoftKeyDown->mBottom);
        Invalidate(mDirtyRect);
    }

    // Prepare the popup balloon
    if (mSoftKeyDown->NeedBalloon()) {
        AutoPtr<IDrawable> balloonBg = mSoftKeyboard->GetBalloonBackground();
        mBalloonPopup->SetBalloonBackground(balloonBg);

        desired_width = mSoftKeyDown->GetWidth() + env->GetKeyBalloonWidthPlus();
        desired_height = mSoftKeyDown->GetHeight() + env->GetKeyBalloonHeightPlus();
        textSize = env->GetBalloonTextSize(
                SoftKeyType::KEYTYPE_ID_NORMAL_KEY != mSoftKeyDown->mKeyType->mKeyTypeId);
        AutoPtr<IDrawable> iconPopup = mSoftKeyDown->GetKeyIconPopup();
        if (NULL != iconPopup) {
            mBalloonPopup->SetBalloonConfig(iconPopup, desired_width, desired_height);
        }
        else {
            mBalloonPopup->SetBalloonConfig(mSoftKeyDown->GetKeyLabel(),
                    textSize, mSoftKeyDown->NeedBalloon(),
                    mSoftKeyDown->GetColorBalloon(), desired_width, desired_height);
        }

        // The position to show.
        Int32 value = 0;
        (*mHintLocationToSkbContainer)[0] = mPaddingLeft + mSoftKeyDown->mLeft
                + -((mBalloonPopup->GetWidth(&value), value) - mSoftKeyDown->GetWidth()) / 2;
        (*mHintLocationToSkbContainer)[0] += (*mOffsetToSkbContainer)[0];
        (*mHintLocationToSkbContainer)[1] = mPaddingTop + mSoftKeyDown->mTop - (mBalloonPopup->GetHeight(&value), value);
        (*mHintLocationToSkbContainer)[1] += (*mOffsetToSkbContainer)[1];
        ShowBalloon(mBalloonPopup, mHintLocationToSkbContainer, movePress);
    }
    else {
        mBalloonPopup->DelayedDismiss(0);
    }

    if (mRepeatForLongPress) {
        ((CSkbContainer::LongPressTimer*)longPressTimer)->StartTimer();
    }
    return mSoftKeyDown;
}

AutoPtr<SoftKey> CSoftKeyboardView::OnKeyRelease(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    mKeyPressed = FALSE;
    if (NULL == mSoftKeyDown) return NULL;

    ((CSkbContainer::LongPressTimer*)mLongPressTimer.Get())->RemoveTimer();

    if (NULL != mBalloonOnKey) {
        mBalloonOnKey->DelayedDismiss(CBalloonHint::TIME_DELAY_DISMISS);
    }
    else {
        mDirtyRect->Union(mSoftKeyDown->mLeft, mSoftKeyDown->mTop,
                mSoftKeyDown->mRight, mSoftKeyDown->mBottom);
        Invalidate(mDirtyRect);
    }

    if (mSoftKeyDown->NeedBalloon()) {
        mBalloonPopup->DelayedDismiss(CBalloonHint::TIME_DELAY_DISMISS);
    }

    if (mSoftKeyDown->MoveWithinKey(x - mPaddingLeft, y - mPaddingTop)) {
        return mSoftKeyDown;
    }
    return NULL;
}

AutoPtr<SoftKey> CSoftKeyboardView::OnKeyMove(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    if (NULL == mSoftKeyDown) return NULL;

    if (mSoftKeyDown->MoveWithinKey(x - mPaddingLeft, y - mPaddingTop)) {
        return mSoftKeyDown;
    }

    // The current key needs to be updated.
    mDirtyRect->Union(mSoftKeyDown->mLeft, mSoftKeyDown->mTop,
            mSoftKeyDown->mRight, mSoftKeyDown->mBottom);

    if (mRepeatForLongPress) {
        if (mMovingNeverHidePopupBalloon) {
            return OnKeyPress(x, y, mLongPressTimer, TRUE);
        }

        if (NULL != mBalloonOnKey) {
            mBalloonOnKey->DelayedDismiss(0);
        }
        else {
            Invalidate(mDirtyRect);
        }

        if (mSoftKeyDown->NeedBalloon()) {
            mBalloonPopup->DelayedDismiss(0);
        }

        if (NULL != mLongPressTimer) {
            ((CSkbContainer::LongPressTimer*)mLongPressTimer.Get())->RemoveTimer();
        }
        return OnKeyPress(x, y, mLongPressTimer, TRUE);
    }
    else {
        // When user moves between keys, repeated response is disabled.
        return OnKeyPress(x, y, mLongPressTimer, TRUE);
    }
}

void CSoftKeyboardView::TryVibrate()
{
    if (!Settings::GetVibrate()) {
        return;
    }
    if (mVibrator == NULL) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<IInterface> service;
        context->GetSystemService(IContext::VIBRATOR_SERVICE, (IInterface**)&service);
        mVibrator = IVibrator::Probe(service);
    }
    mVibrator->Vibrate(mVibratePattern, -1);
}

void CSoftKeyboardView::TryPlayKeyDown()
{
    if (Settings::GetKeySound()) {
        mSoundManager->PlayKeyDown();
    }
}

void CSoftKeyboardView::DimSoftKeyboard(
    /* [in] */ Boolean dimSkb)
{
    mDimSkb = dimSkb;
    Invalidate();
}

void CSoftKeyboardView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    if (NULL == mSoftKeyboard) return;

    canvas->Translate(mPaddingLeft, mPaddingTop);

    AutoPtr<Environment> env = Environment::GetInstance();
    mNormalKeyTextSize = env->GetKeyTextSize(FALSE);
    mFunctionKeyTextSize = env->GetKeyTextSize(TRUE);
    // Draw the last soft keyboard
    Int32 rowNum = mSoftKeyboard->GetRowNum();
    Int32 keyXMargin = mSoftKeyboard->GetKeyXMargin();
    Int32 keyYMargin = mSoftKeyboard->GetKeyYMargin();
    for (Int32 row = 0; row < rowNum; row++) {
        AutoPtr<SoftKeyboard::KeyRow> keyRow = mSoftKeyboard->GetKeyRowForDisplay(row);
        if (NULL == keyRow) continue;
        List<AutoPtr<SoftKey> >::Iterator ator = keyRow->mSoftKeys.Begin();
        for (; ator != keyRow->mSoftKeys.End(); ++ator) {
            AutoPtr<SoftKey> softKey = *ator;
            if (SoftKeyType::KEYTYPE_ID_NORMAL_KEY == softKey->mKeyType->mKeyTypeId) {
                mPaint->SetTextSize(mNormalKeyTextSize);
            }
            else {
                mPaint->SetTextSize(mFunctionKeyTextSize);
            }
            DrawSoftKey(canvas, softKey, keyXMargin, keyYMargin);
        }
    }

    if (mDimSkb) {
        mPaint->SetColor(0xa0000000);
        Int32 width, height;
        GetWidth(&width);
        GetHeight(&height);
        canvas->DrawRect(0, 0, width, height, mPaint);
    }

    mDirtyRect->SetEmpty();
}

void CSoftKeyboardView::DrawSoftKey(
    /* [in] */ ICanvas* canvas,
    /* [in] */ SoftKey* softKey,
    /* [in] */ Int32 keyXMargin,
    /* [in] */ Int32 keyYMargin)
{
    AutoPtr<IDrawable> bg;
    Int32 textColor;
    if (mKeyPressed && softKey == mSoftKeyDown) {
        bg = softKey->GetKeyHlBg();
        textColor = softKey->GetColorHl();
    }
    else {
        bg = softKey->GetKeyBg();
        textColor = softKey->GetColor();
    }

    if (NULL != bg) {
        bg->SetBounds(softKey->mLeft + keyXMargin, softKey->mTop + keyYMargin,
                softKey->mRight - keyXMargin, softKey->mBottom - keyYMargin);
        bg->Draw(canvas);
    }

    String keyLabel = softKey->GetKeyLabel();
    AutoPtr<IDrawable> keyIcon = softKey->GetKeyIcon();
    if (NULL != keyIcon) {
        AutoPtr<IDrawable> icon = keyIcon;
        Int32 value = 0;
        Int32 marginLeft = (softKey->GetWidth() - (icon->GetIntrinsicWidth(&value), value)) / 2;
        Int32 marginRight = softKey->GetWidth() - (icon->GetIntrinsicWidth(&value), value) - marginLeft;
        Int32 marginTop = (softKey->GetHeight() - (icon->GetIntrinsicHeight(&value), value)) / 2;
        Int32 marginBottom = softKey->GetHeight() - (icon->GetIntrinsicHeight(&value), value) - marginTop;
        icon->SetBounds(softKey->mLeft + marginLeft,
                softKey->mTop + marginTop, softKey->mRight - marginRight,
                softKey->mBottom - marginBottom);
        icon->Draw(canvas);
    }
    else if (NULL != keyLabel) {
        mPaint->SetColor(textColor);
        Float pw = 0.f;
        Float x = softKey->mLeft
                + (softKey->GetWidth() - (mPaint->MeasureText(keyLabel, &pw), pw)) / 2.0f;
        Int32 bottom = 0, top = 0;
        Int32 fontHeight = (mFmi->GetBottom(&bottom), bottom) - (mFmi->GetTop(&top), top);
        Float marginY = (softKey->GetHeight() - fontHeight) / 2.0f;
        Float y = softKey->mTop + marginY - top + bottom / 1.5f;
        canvas->DrawText(keyLabel, x, y + 1, mPaint);
    }
}

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos
