
#include "CSoftKeyboardView.h"
#include "CPinyinSettings.h"
#include "SoftKeyboard.h"
#include "SoftKey.h"
#include "CPinyinEnvironmentHelper.h"
#include "CSoundManagerHelper.h"
#include "elastos/droid/graphics/CPaint.h"
#include "CBalloonHint.h"

using Elastos::Droid::Graphics::CPaint;

namespace Elastos {
namespace Droid {
namespace Inputmethods {
namespace PinyinIME {

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
    memset(mOffsetToSkbContainer, 0, sizeof(mOffsetToSkbContainer));
    memset(mHintLocationToSkbContainer, 0, sizeof(mHintLocationToSkbContainer));
    mVibratePattern = ArrayOf<Int64>::Alloc(2);
    (*mVibratePattern)[0] = 1;
    (*mVibratePattern)[1] = 20;
    CRect::New((IRect**)&mDirtyRect);
}

ECode CSoftKeyboardView::SetSoftKeyboard(
    /* [in] */ ISoftKeyboard* softSkb,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (NULL == softSkb) {
        *result = FALSE;
        return NOERROR;
    }
    mSoftKeyboard = (SoftKeyboard*)softSkb;
    AutoPtr<IDrawable> bg = mSoftKeyboard->GetSkbBackground();
    if (NULL != bg) SetBackgroundDrawable(bg);
    *result = TRUE;
    return NOERROR;
}

ECode CSoftKeyboardView::ResizeKeyboard(
    /* [in] */ Int32 skbWidth,
    /* [in] */ Int32 skbHeight)
{
    mSoftKeyboard->SetSkbCoreSize(skbWidth, skbHeight);
    return NOERROR;
}

ECode CSoftKeyboardView::SetBalloonHint(
    /* [in] */ IBalloonHint* balloonOnKey,
    /* [in] */ IBalloonHint* balloonPopup,
    /* [in] */ Boolean movingNeverHidePopup)
{
    mBalloonOnKey = balloonOnKey;
    mBalloonPopup = balloonPopup;
    mMovingNeverHidePopupBalloon = movingNeverHidePopup;
    return NOERROR;
}

ECode CSoftKeyboardView::SetOffsetToSkbContainer(
    /* [in] */ ArrayOf<Int32>* offsetToSkbContainer)
{
    assert(offsetToSkbContainer != NULL);
    mOffsetToSkbContainer[0] = (*offsetToSkbContainer)[0];
    mOffsetToSkbContainer[1] = (*offsetToSkbContainer)[1];
    return NOERROR;
}

void CSoftKeyboardView::OnMeasure(
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
}

void CSoftKeyboardView::ShowBalloon(
    /* [in] */ IBalloonHint* balloon,
    /* [in] */ ArrayOf<Int32>* balloonLocationToSkb,
    /* [in] */ Boolean movePress)
{
    assert(balloon != NULL);
    Int64 delay = CBalloonHint::TIME_DELAY_SHOW;
    if (movePress) delay = 0;

    Boolean result = FALSE;
    if (balloon->NeedForceDismiss(&result), result) {
        balloon->DelayedDismiss(0);
    }
    if (!(balloon->IsShowing(&result), result)) {
        balloon->DelayedShow(delay, balloonLocationToSkb);
    } else {
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

ECode CSoftKeyboardView::ResetKeyPress(
    /* [in] */ Int64 balloonDelay)
{
    if (!mKeyPressed) return NOERROR;
    mKeyPressed = FALSE;
    if (NULL != mBalloonOnKey) {
        mBalloonOnKey->DelayedDismiss(balloonDelay);
    } else {
        if (NULL != mSoftKeyDown) {
            Boolean empty = FALSE;
            if (mDirtyRect->IsEmpty(&empty), empty) {
                mDirtyRect->Set(mSoftKeyDown->mLeft, mSoftKeyDown->mTop,
                        mSoftKeyDown->mRight, mSoftKeyDown->mBottom);
            }
            View::Invalidate(mDirtyRect);
        } else {
            View::Invalidate();
        }
    }
    return mBalloonPopup->DelayedDismiss(balloonDelay);
}

ECode CSoftKeyboardView::OnKeyPress(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ /*SkbContainer.LongPressTimer*/ IHandler* longPressTimer,
    /* [in] */ Boolean movePress,
    /* [out] */ ISoftKey** key)
{
    VALIDATE_NOT_NULL(key);
    mKeyPressed = FALSE;
    Boolean moveWithinPreviousKey = FALSE;
    if (movePress) {
        AutoPtr<SoftKey> newKey = mSoftKeyboard->MapToKey(x, y);
        if (newKey == mSoftKeyDown) moveWithinPreviousKey = TRUE;
        mSoftKeyDown = newKey;
    } else {
        mSoftKeyDown = mSoftKeyboard->MapToKey(x, y);
    }
    if (moveWithinPreviousKey || NULL == mSoftKeyDown) {
        *key = mSoftKeyDown;
        REFCOUNT_ADD(*key);
        return NOERROR;
    }

    mKeyPressed = TRUE;

    if (!movePress) {
        TryPlayKeyDown();
        TryVibrate();
    }

    mLongPressTimer = (SkbContainer::LongPressTimer*)longPressTimer;

    if (!movePress) {
        if (mSoftKeyDown->GetPopupResId() > 0 || mSoftKeyDown->Repeatable()) {
            mLongPressTimer->StartTimer();
        }
    } else {
        mLongPressTimer->RemoveTimer();
    }

    Int32 desired_width;
    Int32 desired_height;
    Float textSize;
    AutoPtr<IPinyinEnvironmentHelper> helper;
    CPinyinEnvironmentHelper::AcquireSingleton((IPinyinEnvironmentHelper**)&helper);
    AutoPtr<IPinyinEnvironment> env;
    helper->GetInstance((IPinyinEnvironment**)&env);

    if (NULL != mBalloonOnKey) {
        AutoPtr<IDrawable> keyHlBg = mSoftKeyDown->GetKeyHlBg();
        mBalloonOnKey->SetBalloonBackground(keyHlBg);

        // Prepare the on-key balloon
        Int32 keyXMargin = mSoftKeyboard->GetKeyXMargin();
        Int32 keyYMargin = mSoftKeyboard->GetKeyYMargin();
        desired_width = mSoftKeyDown->GetWidth() - 2 * keyXMargin;
        desired_height = mSoftKeyDown->GetHeight() - 2 * keyYMargin;
        Int32 size = 0;
        env->GetKeyTextSize(SoftKeyType::KEYTYPE_ID_NORMAL_KEY != mSoftKeyDown->mKeyType->mKeyTypeId, &size);
        textSize = size;
        AutoPtr<IDrawable> icon = mSoftKeyDown->GetKeyIcon();
        if (NULL != icon) {
            mBalloonOnKey->SetBalloonConfig(icon, desired_width,
                    desired_height);
        } else {
            String label;
            mSoftKeyDown->GetKeyLabel(&label);
            mBalloonOnKey->SetBalloonConfig(label, textSize, TRUE, mSoftKeyDown->GetColorHl(),
                    desired_width, desired_height);
        }

        Int32 value = 0;
        mHintLocationToSkbContainer[0] = mPaddingLeft + mSoftKeyDown->mLeft
                - ((mBalloonOnKey->GetWidth(&value), value) - mSoftKeyDown->GetWidth()) / 2;
        mHintLocationToSkbContainer[0] += mOffsetToSkbContainer[0];
        mHintLocationToSkbContainer[1] = mPaddingTop
                + (mSoftKeyDown->mBottom - keyYMargin)
                - (mBalloonOnKey->GetHeight(&value), value);
        mHintLocationToSkbContainer[1] += mOffsetToSkbContainer[1];

        AutoPtr<ArrayOf<Int32> > loc = ArrayOf<Int32>::Alloc(2);
        (*loc)[0] = mHintLocationToSkbContainer[0];
        (*loc)[1] = mHintLocationToSkbContainer[1];
        ShowBalloon(mBalloonOnKey, loc, movePress);
    } else {
        mDirtyRect->Union(mSoftKeyDown->mLeft, mSoftKeyDown->mTop,
                mSoftKeyDown->mRight, mSoftKeyDown->mBottom);
        View::Invalidate(mDirtyRect);
    }

    // Prepare the popup balloon
    if (mSoftKeyDown->NeedBalloon()) {
        AutoPtr<IDrawable> balloonBg = mSoftKeyboard->GetBalloonBackground();
        mBalloonPopup->SetBalloonBackground(balloonBg);

        Int32 plus = 0, size = 0;
        desired_width = mSoftKeyDown->GetWidth() + (env->GetKeyBalloonWidthPlus(&plus), plus);
        desired_height = mSoftKeyDown->GetHeight() + (env->GetKeyBalloonHeightPlus(&plus), plus);
        env->GetBalloonTextSize(SoftKeyType::KEYTYPE_ID_NORMAL_KEY != mSoftKeyDown->mKeyType->mKeyTypeId, &size);
        textSize = size;
        AutoPtr<IDrawable> iconPopup = mSoftKeyDown->GetKeyIconPopup();
        if (NULL != iconPopup) {
            mBalloonPopup->SetBalloonConfig(iconPopup, desired_width, desired_height);
        } else {
            String label;
            mSoftKeyDown->GetKeyLabel(&label);
            mBalloonPopup->SetBalloonConfig(label, textSize, mSoftKeyDown->NeedBalloon(),
                mSoftKeyDown->GetColorBalloon(), desired_width, desired_height);
        }

        // The position to show.
        Int32 value = 0;
        mHintLocationToSkbContainer[0] = mPaddingLeft + mSoftKeyDown->mLeft
                + -((mBalloonPopup->GetWidth(&value), value) - mSoftKeyDown->GetWidth()) / 2;
        mHintLocationToSkbContainer[0] += mOffsetToSkbContainer[0];
        mHintLocationToSkbContainer[1] = mPaddingTop + mSoftKeyDown->mTop - (mBalloonPopup->GetHeight(&value), value);
        mHintLocationToSkbContainer[1] += mOffsetToSkbContainer[1];

        AutoPtr<ArrayOf<Int32> > loc = ArrayOf<Int32>::Alloc(2);
        (*loc)[0] = mHintLocationToSkbContainer[0];
        (*loc)[1] = mHintLocationToSkbContainer[1];
        ShowBalloon(mBalloonPopup, loc, movePress);
    } else {
        mBalloonPopup->DelayedDismiss(0);
    }

    if (mRepeatForLongPress) {
        ((SkbContainer::LongPressTimer*)longPressTimer)->StartTimer();
    }
    *key = mSoftKeyDown;
    REFCOUNT_ADD(*key);
    return NOERROR;
}

ECode CSoftKeyboardView::OnKeyRelease(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [out] */ ISoftKey** key)
{
    VALIDATE_NOT_NULL(key);
    mKeyPressed = FALSE;
    if (NULL == mSoftKeyDown) {
        *key = NULL;
        return NOERROR;
    }

    mLongPressTimer->RemoveTimer();

    if (NULL != mBalloonOnKey) {
        mBalloonOnKey->DelayedDismiss(CBalloonHint::TIME_DELAY_DISMISS);
    } else {
        mDirtyRect->Union(mSoftKeyDown->mLeft, mSoftKeyDown->mTop,
                mSoftKeyDown->mRight, mSoftKeyDown->mBottom);
        View::Invalidate(mDirtyRect);
    }

    if (mSoftKeyDown->NeedBalloon()) {
        mBalloonPopup->DelayedDismiss(CBalloonHint::TIME_DELAY_DISMISS);
    }

    if (mSoftKeyDown->MoveWithinKey(x - mPaddingLeft, y - mPaddingTop)) {
        *key = mSoftKeyDown;
        REFCOUNT_ADD(*key);
        return NOERROR;
    }
    *key = NULL;
    return NOERROR;
}

ECode CSoftKeyboardView::OnKeyMove(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [out] */ ISoftKey** key)
{
    VALIDATE_NOT_NULL(key);
    if (NULL == mSoftKeyDown) {
        *key = NULL;
        return NOERROR;
    }

    if (mSoftKeyDown->MoveWithinKey(x - mPaddingLeft, y - mPaddingTop)) {
        *key = mSoftKeyDown;
        REFCOUNT_ADD(*key);
        return NOERROR;
    }

    // The current key needs to be updated.
    mDirtyRect->Union(mSoftKeyDown->mLeft, mSoftKeyDown->mTop,
            mSoftKeyDown->mRight, mSoftKeyDown->mBottom);

    if (mRepeatForLongPress) {
        if (mMovingNeverHidePopupBalloon) {
            return OnKeyPress(x, y, mLongPressTimer, TRUE, key);
        }

        if (NULL != mBalloonOnKey) {
            mBalloonOnKey->DelayedDismiss(0);
        } else {
            View::Invalidate(mDirtyRect);
        }

        if (mSoftKeyDown->NeedBalloon()) {
            mBalloonPopup->DelayedDismiss(0);
        }

        if (NULL != mLongPressTimer) {
            mLongPressTimer->RemoveTimer();
        }
        return OnKeyPress(x, y, mLongPressTimer, TRUE, key);
    }

    // When user moves between keys, repeated response is disabled.
    return OnKeyPress(x, y, mLongPressTimer, TRUE, key);
}

void CSoftKeyboardView::TryVibrate()
{
    AutoPtr<ISettings> settings;
    CPinyinSettings::AcquireSingleton((ISettings**)&settings);
    Boolean res = FALSE;
    if (!(settings->GetVibrate(&res), res)) {
        return;
    }
    if (mVibrator == NULL) {
        AutoPtr<IInterface> service;
        GetContext()->GetSystemService(IContext::VIBRATOR_SERVICE, (IInterface**)&service);
        mVibrator = IVibrator::Probe(service);
    }

    assert(mVibrator != NULL);
    mVibrator->Vibrate(*mVibratePattern, -1);
}

void CSoftKeyboardView::TryPlayKeyDown()
{
    AutoPtr<ISettings> settings;
    CPinyinSettings::AcquireSingleton((ISettings**)&settings);
    Boolean res = FALSE;
    if (settings->GetKeySound(&res), res) {
        mSoundManager->PlayKeyDown();
    }
}

ECode CSoftKeyboardView::DimSoftKeyboard(
    /* [in] */ Boolean dimSkb)
{
    mDimSkb = dimSkb;
    return Invalidate();
}

void CSoftKeyboardView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    if (NULL == mSoftKeyboard) return;

    canvas->Translate(mPaddingLeft, mPaddingTop);

    AutoPtr<IPinyinEnvironmentHelper> helper;
    CPinyinEnvironmentHelper::AcquireSingleton((IPinyinEnvironmentHelper**)&helper);
    AutoPtr<IPinyinEnvironment> env;
    helper->GetInstance((IPinyinEnvironment**)&env);

    env->GetKeyTextSize(FALSE, &mNormalKeyTextSize);
    env->GetKeyTextSize(TRUE, &mFunctionKeyTextSize);
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
            } else {
                mPaint->SetTextSize(mFunctionKeyTextSize);
            }
            DrawSoftKey(canvas, softKey, keyXMargin, keyYMargin);
        }
    }

    if (mDimSkb) {
        mPaint->SetColor(0xa0000000);
        canvas->DrawRect(0, 0, GetWidth(), GetHeight(), mPaint);
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
    assert(softKey != NULL);
    if (mKeyPressed && softKey == mSoftKeyDown) {
        bg = softKey->GetKeyHlBg();
        textColor = softKey->GetColorHl();
    } else {
        bg = softKey->GetKeyBg();
        textColor = softKey->GetColor();
    }

    if (NULL != bg) {
        bg->SetBounds(softKey->mLeft + keyXMargin, softKey->mTop + keyYMargin,
                softKey->mRight - keyXMargin, softKey->mBottom - keyYMargin);
        bg->Draw(canvas);
    }

    String keyLabel;
    softKey->GetKeyLabel(&keyLabel);
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
    } else if (NULL != keyLabel) {
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

PInterface CSoftKeyboardView::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_ISoftKeyboardView) {
        return (IInterface*)(ISoftKeyboardView*)this;
    }

    return View::Probe(riid);
}

ECode CSoftKeyboardView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    View::constructor(context, attrs);

    AutoPtr<ISoundManagerHelper> helper;
    CSoundManagerHelper::AcquireSingleton((ISoundManagerHelper**)&helper);
    helper->GetInstance(mContext, (ISoundManager**)&mSoundManager);

    CPaint::New((IPaint**)&mPaint);
    mPaint->SetAntiAlias(TRUE);
    return mPaint->GetFontMetricsInt((IPaintFontMetricsInt**)&mFmi);
}

ECode CSoftKeyboardView::GetSoftKeyboard(
    /* [out] */ ISoftKeyboard** keyboard)
{
    VALIDATE_NOT_NULL(keyboard);
    *keyboard = mSoftKeyboard;
    REFCOUNT_ADD(*keyboard);
    return NOERROR;
}

} // namespace PinyinIME
} // namespace Inputmethods
} // namespace Droid
} // namespace Elastos
