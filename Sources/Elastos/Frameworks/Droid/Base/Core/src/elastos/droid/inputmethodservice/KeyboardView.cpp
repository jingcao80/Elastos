//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/inputmethodservice/KeyboardView.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/graphics/CCanvas.h"
#include "elastos/droid/graphics/CRectF.h"
#include "elastos/droid/graphics/Typeface.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/R.h"
#include "elastos/droid/view/ViewConfiguration.h"
#include "elastos/droid/view/MotionEvent.h"
#include "elastos/droid/view/CGestureDetector.h"
#include "elastos/droid/view/accessibility/CAccessibilityEventHelper.h"
#include "elastos/droid/view/accessibility/CAccessibilityManagerHelper.h"
#include "elastos/droid/provider/CSettingsSecure.h"
#include "elastos/droid/widget/CPopupWindow.h"
#include "elastos/droid/utility/CDisplayMetrics.h"
#include <elastos/core/Character.h>
#include <elastos/core/Math.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Graphics::CRectF;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::PorterDuffMode_CLEAR;
using Elastos::Droid::Graphics::RegionOp_REPLACE;
using Elastos::Droid::Graphics::Typeface;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::R;
using Elastos::Droid::View::CGestureDetector;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::MotionEvent;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::IGestureDetectorOnGestureListener;
using Elastos::Droid::View::ViewConfiguration;
using Elastos::Droid::View::Accessibility::IAccessibilityEventHelper;
using Elastos::Droid::View::Accessibility::CAccessibilityEventHelper;
using Elastos::Droid::View::Accessibility::IAccessibilityManagerHelper;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::Accessibility::CAccessibilityManagerHelper;
using Elastos::Droid::Widget::CPopupWindow;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::Utility::CDisplayMetrics;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Core::Character;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace InputMethodService {


const Int32 KeyboardView::SwipeTracker::NUM_PAST = 4;
const Int32 KeyboardView::SwipeTracker::LONGEST_PAST_TIME = 200;
const Boolean KeyboardView::DEBUG = FALSE;
const Int32 KeyboardView::NOT_A_KEY = -1;
Int32 KeyboardView::KEY_DELETE[] = { IKeyboard::KEYCODE_DELETE };
Int32 KeyboardView::LONG_PRESSABLE_STATE_SET[] = { R::attr::state_long_pressable};

const Int32 KeyboardView::MSG_SHOW_PREVIEW = 1;
const Int32 KeyboardView::MSG_REMOVE_PREVIEW = 2;
const Int32 KeyboardView::MSG_REPEAT = 3;
const Int32 KeyboardView::MSG_LONGPRESS = 4;

const Int32 KeyboardView::DELAY_BEFORE_PREVIEW = 0;
const Int32 KeyboardView::DELAY_AFTER_PREVIEW = 70;
const Int32 KeyboardView::DEBOUNCE_TIME = 70;

const Int32 KeyboardView::REPEAT_INTERVAL = 50; // ~20 keys per second
const Int32 KeyboardView::REPEAT_START_DELAY = 400;
const Int32 KeyboardView::LONGPRESS_TIMEOUT = ViewConfiguration::GetLongPressTimeout();
const Int32 KeyboardView::MAX_NEARBY_KEYS = 12;
const Int32 KeyboardView::MULTITAP_INTERVAL = 800;

ECode KeyboardView::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what, arg1;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);

    switch(what) {
        case KeyboardView::MSG_SHOW_PREVIEW:
            mHost->ShowKey(arg1);
            break;
        case KeyboardView::MSG_REMOVE_PREVIEW:
            IView::Probe(mHost->mPreviewText)->SetVisibility(IView::INVISIBLE);
            break;
        case KeyboardView::MSG_REPEAT:
            if (mHost->RepeatKey()) {
                Boolean result;
                SendEmptyMessageDelayed(KeyboardView::MSG_REPEAT,
                    KeyboardView::REPEAT_INTERVAL, &result);
            }
            break;
        case KeyboardView::MSG_LONGPRESS: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            IMotionEvent* event = IMotionEvent::Probe(obj);
            mHost->OpenPopupIfRequired(event);
            break;
        }
    }

    return NOERROR;
}

CAR_INTERFACE_IMPL(KeyboardView::_OnKeyboardActionListener, Object, IOnKeyboardActionListener);
KeyboardView::_OnKeyboardActionListener::_OnKeyboardActionListener(
    /* [in] */ KeyboardView* host)
    : mHost(host)
{}

KeyboardView::_OnKeyboardActionListener::~_OnKeyboardActionListener()
{}

ECode KeyboardView::_OnKeyboardActionListener::OnPress(
    /* [in] */ Int32 primaryCode)
{
    assert(mHost->mKeyboardActionListener != NULL);
    return mHost->mKeyboardActionListener->OnPress(primaryCode);
}

ECode KeyboardView::_OnKeyboardActionListener::OnRelease(
    /* [in] */ Int32 primaryCode)
{
    assert(mHost->mKeyboardActionListener != NULL);
    return mHost->mKeyboardActionListener->OnRelease(primaryCode);
}

ECode KeyboardView::_OnKeyboardActionListener::OnKey(
    /* [in] */ Int32 primaryCode,
    /* [in] */ ArrayOf<Int32>* keyCodes)
{
    assert(mHost->mKeyboardActionListener != NULL);
    mHost->mKeyboardActionListener->OnKey(primaryCode, keyCodes);
    mHost->DismissPopupKeyboard();
    return NOERROR;
}

ECode KeyboardView::_OnKeyboardActionListener::OnText(
    /* [in] */ ICharSequence* text)
{
    assert(mHost->mKeyboardActionListener != NULL);
    mHost->mKeyboardActionListener->OnText(text);
    mHost->DismissPopupKeyboard();
    return NOERROR;
}

ECode KeyboardView::_OnKeyboardActionListener::SwipeLeft()
{
    return NOERROR;
}

ECode KeyboardView::_OnKeyboardActionListener::SwipeRight()
{
    return NOERROR;
}

ECode KeyboardView::_OnKeyboardActionListener::SwipeDown()
{
    return NOERROR;
}

ECode KeyboardView::_OnKeyboardActionListener::SwipeUp()
{
    return NOERROR;
}

KeyboardView::SwipeTracker::SwipeTracker()
{
    mPastX = ArrayOf<Float>::Alloc(NUM_PAST);
    mPastY = ArrayOf<Float>::Alloc(NUM_PAST);
    mPastTime = ArrayOf<Int64>::Alloc(NUM_PAST);
    mYVelocity = 0.0;
    mXVelocity = 0.0;
}

KeyboardView::SwipeTracker::~SwipeTracker()
{
    mPastX = NULL;
    mPastY = NULL;
    mPastTime = NULL;
}

void KeyboardView::SwipeTracker::Clear()
{
    (*mPastTime)[0] = 0;
}

void KeyboardView::SwipeTracker::AddMovement(
    /* [in] */ IMotionEvent* ev)
{
    Int64 time = 0;
    IInputEvent::Probe(ev)->GetEventTime(&time);
    Int32 N = 0;
    ev->GetHistorySize(&N);
    Float x = 0.0, y = 0.0;
    for (Int32 i=0; i<N; i++) {
        ev->GetHistoricalX(i, &x);
        ev->GetHistoricalY(i, &y);
        Int64 tmpTime = 0;
        ev->GetHistoricalEventTime(i, &tmpTime);
        AddPoint(x, y, tmpTime);
    }
    ev->GetX(&x);
    ev->GetY(&y);
    AddPoint(x, y, time);
}

void KeyboardView::SwipeTracker::AddPoint(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Int64 time)
{
    Int32 drop = -1;
    Int32 i = 0;
    assert(mPastTime != NULL);

    for (i=0; i<NUM_PAST; i++) {
        if ((*mPastTime)[i] == 0) {
            break;
        }
        else if ((*mPastTime)[i] < time - LONGEST_PAST_TIME) {
            drop = i;
        }
    }

    if (i == NUM_PAST && drop < 0) {
        drop = 0;
    }

    if (drop == i) {
        drop--;
    }

    if (drop >= 0) {
        const Int32 start = drop + 1;
        const Int32 count = NUM_PAST - drop - 1;
        mPastX->Copy(0, mPastX, start, count);
        mPastY->Copy(0, mPastY, start, count);
        mPastTime->Copy(0, mPastTime, start, count);

        i -= (drop+1);
    }

    (*mPastX)[i] = x;
    (*mPastY)[i] = y;
    (*mPastTime)[i] = time;
    i++;

    if (i < NUM_PAST) {
        (*mPastTime)[i] = 0;
    }
}

void KeyboardView::SwipeTracker::ComputeCurrentVelocity(
    /* [in] */ Int32 units)
{
    ComputeCurrentVelocity(units, Elastos::Core::Math::FLOAT_MAX_VALUE);
}

void KeyboardView::SwipeTracker::ComputeCurrentVelocity(
    /* [in] */ Int32 units,
    /* [in] */ Float maxVelocity)
{
    Float oldestX = (*mPastX)[0];
    Float oldestY = (*mPastY)[0];
    Int64 oldestTime = (*mPastTime)[0];
    Float accumX = 0;
    Float accumY = 0;
    Int32 N = 0;
    while (N < NUM_PAST) {
        if ((*mPastTime)[N] == 0) {
            break;
        }
        N++;
    }

    for (Int32 i=1; i < N; i++) {
        Int32 dur = (Int32)((*mPastTime)[i] - oldestTime);
        if (dur == 0) {
            continue;
        }

        Float dist = (*mPastX)[i] - oldestX;
        Float vel = (dist / dur) * units;   // pixels/frame.
        if (accumX == 0) {
            accumX = vel;
        }
        else {
            accumX = (accumX + vel) * .5f;
        }

        dist = (*mPastY)[i] - oldestY;
        vel = (dist / dur) * units;   // pixels/frame.
        if (accumY == 0) {
            accumY = vel;
        }
        else {
            accumY = (accumY + vel) * .5f;
        }
    }
    mXVelocity = accumX < 0.0f ? Elastos::Core::Math::Max(accumX, -maxVelocity)
            : Elastos::Core::Math::Min(accumX, maxVelocity);
    mYVelocity = accumY < 0.0f ? Elastos::Core::Math::Max(accumY, -maxVelocity)
            : Elastos::Core::Math::Min(accumY, maxVelocity);
}

Float KeyboardView::SwipeTracker::GetXVelocity()
{
    return mXVelocity;
}

Float KeyboardView::SwipeTracker::GetYVelocity()
{
    return mYVelocity;
}

KeyboardView::_SimpleOnGestureListener::_SimpleOnGestureListener(
    /* [in] */ KeyboardView* host)
    : mHost(host)
{}

KeyboardView::_SimpleOnGestureListener::~_SimpleOnGestureListener()
{}

ECode KeyboardView::_SimpleOnGestureListener::OnFling(
    /* [in] */ IMotionEvent* e1,
    /* [in] */ IMotionEvent* e2,
    /* [in] */ Float velocityX,
    /* [in] */ Float velocityY,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = TRUE;
    if (mHost->mPossiblePoly) {
        *res = FALSE;
        return NOERROR;
    }

    const Float absX = Elastos::Core::Math::Abs(velocityX);
    const Float absY = Elastos::Core::Math::Abs(velocityY);

    Float tm1 = 0.0, tm2 = 0.0;
    e2->GetX(&tm2);
    e1->GetX(&tm1);
    Float deltaX = tm2 - tm1;

    e2->GetY(&tm2);
    e1->GetY(&tm1);
    Float deltaY = tm2 - tm1;

    Int32 tmpI = 0;
    Int32 travelX = (mHost->GetWidth(&tmpI), tmpI) / 2; // Half the keyboard width
    Int32 travelY = (mHost->GetHeight(&tmpI), tmpI) / 2; // Half the keyboard height

    mHost->mSwipeTracker->ComputeCurrentVelocity(1000);
    const Float endingVelocityX = mHost->mSwipeTracker->GetXVelocity();
    const Float endingVelocityY = mHost->mSwipeTracker->GetYVelocity();
    Boolean sendDownKey = FALSE;

    if (velocityX > mHost->mSwipeThreshold && absY < absX && deltaX > travelX) {
        if (mHost->mDisambiguateSwipe && endingVelocityX < velocityX / 4) {
            sendDownKey = TRUE;
        }
        else {
            mHost->SwipeRight();
            return NOERROR;
        }
    }
    else if (velocityX < - mHost->mSwipeThreshold && absY < absX && deltaX < -travelX) {
        if (mHost->mDisambiguateSwipe && endingVelocityX > velocityX / 4) {
            sendDownKey = TRUE;
        }
        else {
            mHost->SwipeLeft();
            return NOERROR;
        }
    }
    else if (velocityY < - mHost->mSwipeThreshold && absX < absY && deltaY < -travelY) {
        if (mHost->mDisambiguateSwipe && endingVelocityY > velocityY / 4) {
            sendDownKey = TRUE;
        }
        else {
            mHost->SwipeUp();
            return NOERROR;
        }
    }
    else if (velocityY > mHost->mSwipeThreshold && absX < absY / 2 && deltaY > travelY) {
        if (mHost->mDisambiguateSwipe && endingVelocityY < velocityY / 4) {
            sendDownKey = TRUE;
        }
        else {
            mHost->SwipeDown();
            return NOERROR;
        }
    }

    if (sendDownKey) {
        Int64 time = 0;
        IInputEvent::Probe(e1)->GetEventTime(&time);
        mHost->DetectAndSendKey(mHost->mDownKey, mHost->mStartX, mHost->mStartY, time);
    }

    *res = FALSE;
    return NOERROR;
}

CAR_INTERFACE_IMPL(KeyboardView::CloseButtonClickListener, Object, IViewOnClickListener)

KeyboardView::CloseButtonClickListener::CloseButtonClickListener(
    /* [in] */ KeyboardView* host)
    : mHost(host)
{}

ECode KeyboardView::CloseButtonClickListener::OnClick(
    /* [in] */ IView* view)
{
    mHost->DismissPopupKeyboard();
    return NOERROR;
}

CAR_INTERFACE_IMPL(KeyboardView, View, IKeyboardView);
KeyboardView::KeyboardView()
    : mCurrentKeyIndex(NOT_A_KEY)
    , mLabelTextSize(0)
    , mKeyTextSize(0)
    , mKeyTextColor(0)
    , mShadowRadius(0.0)
    , mShadowColor(0)
    , mBackgroundDimAmount(0.0)
    , mPreviewTextSizeLarge(0)
    , mPreviewOffset(0)
    , mPreviewHeight(0)
    , mMiniKeyboardOnScreen(FALSE)
    , mMiniKeyboardOffsetX(0)
    , mMiniKeyboardOffsetY(0)
    , mKeys(NULL)
    , mVerticalCorrection(0)
    , mProximityThreshold(0)
    , mPreviewCentered(FALSE)
    , mShowPreview(TRUE)
    , mShowTouchPoints(TRUE)
    , mPopupPreviewX(0)
    , mPopupPreviewY(0)
    , mLastX(0)
    , mLastY(0)
    , mStartX(0)
    , mStartY(0)
    , mProximityCorrectOn(FALSE)
    , mDownTime(0)
    , mLastMoveTime(0)
    , mLastKey(0)
    , mLastCodeX(0)
    , mLastCodeY(0)
    , mCurrentKey(NOT_A_KEY)
    , mDownKey(NOT_A_KEY)
    , mLastKeyTime(0)
    , mCurrentKeyTime(0)
    , mKeyIndices(ArrayOf<Int32>::Alloc(12))
    , mPopupX(0)
    , mPopupY(0)
    , mRepeatKeyIndex(NOT_A_KEY)
    , mPopupLayout(0)
    , mAbortKey(FALSE)
    , mPossiblePoly(FALSE)
    , mSwipeTracker(new SwipeTracker())
    , mSwipeThreshold(0)
    , mDisambiguateSwipe(FALSE)
    , mOldPointerCount(1)
    , mOldPointerX(0.0)
    , mOldPointerY(0.0)
    , mDistances(ArrayOf<Int32>::Alloc(MAX_NEARBY_KEYS))
    , mLastSentIndex(0)
    , mTapCount(0)
    , mLastTapTime(0)
    , mInMultiTap(FALSE)
    , mDrawPending(FALSE)
    , mKeyboardChanged(FALSE)
    , mHeadsetRequiredToHearPasswordsAnnounced(FALSE)
    , mOldEventTime(0)
    , mUsedVelocity(FALSE)
{
}

ECode KeyboardView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, R::attr::keyboardViewStyle);
}

ECode KeyboardView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode KeyboardView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    View::constructor(context, attrs, defStyleAttr, defStyleRes);

    CRect::New(0, 0, 0, 0, (IRect**)&mClipRegion);
    CRectF::New((IRectF**)&mDirtyRect);
    mHandler = new MyHandler(this);
    mHandler->constructor();
    mCoordinates = ArrayOf<Int32>::Alloc(2);

    AutoPtr<ITypedArray> a;
    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::KeyboardView);
    context->ObtainStyledAttributes(attrs, attrIds,
            defStyleAttr, defStyleRes, (ITypedArray**)&a);

    AutoPtr<IInterface> service;
    FAIL_RETURN(context->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&service))
    AutoPtr<ILayoutInflater> inflate = ILayoutInflater::Probe(service);

    Int32 previewLayout = 0;
    Int32 keyTextSize = 0;

    Int32 n = 0;
    a->GetIndexCount(&n);

    for (Int32 i = 0; i < n; i++) {
        Int32 attr = 0;
        a->GetIndex(i, &attr);

        switch (attr) {
            case R::styleable::KeyboardView_keyBackground:
                a->GetDrawable(attr, (IDrawable**)&mKeyBackground);
                break;
            case R::styleable::KeyboardView_verticalCorrection:
                a->GetDimensionPixelOffset(attr, 0, &mVerticalCorrection);
                break;
            case R::styleable::KeyboardView_keyPreviewLayout:
                a->GetResourceId(attr, 0, &previewLayout);
                break;
            case R::styleable::KeyboardView_keyPreviewOffset:
                a->GetDimensionPixelOffset(attr, 0, &mPreviewOffset);
                break;
            case R::styleable::KeyboardView_keyPreviewHeight:
                a->GetDimensionPixelSize(attr, 80, &mPreviewHeight);
                break;
            case R::styleable::KeyboardView_keyTextSize:
                a->GetDimensionPixelSize(attr, 18, &mKeyTextSize);
                break;
            case R::styleable::KeyboardView_keyTextColor:
                a->GetColor(attr, 0xFF000000, &mKeyTextColor);
                break;
            case R::styleable::KeyboardView_labelTextSize:
                a->GetDimensionPixelSize(attr, 14, &mLabelTextSize);
                break;
            case R::styleable::KeyboardView_popupLayout:
                a->GetResourceId(attr, 0, &mPopupLayout);
                break;
            case R::styleable::KeyboardView_shadowColor:
                a->GetColor(attr, 0, &mShadowColor);
                break;
            case R::styleable::KeyboardView_shadowRadius:
                a->GetFloat(attr, 0.0, &mShadowRadius);
                break;
        }
    }

    a->Recycle();

    a = NULL;
    mContext->ObtainStyledAttributes(attrIds, (ITypedArray**)&a);
    a->GetFloat(R::styleable::Theme_backgroundDimAmount, 0.5f, &mBackgroundDimAmount);
    a->Recycle();

    CPopupWindow::New(context, (IPopupWindow**)&mPreviewPopup);
    if (previewLayout != 0) {
        AutoPtr<IView> tv;
        inflate->Inflate(previewLayout, NULL, (IView**)&tv);
        mPreviewText = ITextView::Probe(tv);
        Float textSize = 0.0;
        mPreviewText->GetTextSize(&textSize);
        mPreviewTextSizeLarge = (Int32)textSize;
        mPreviewPopup->SetContentView(IView::Probe(mPreviewText));
        mPreviewPopup->SetBackgroundDrawable(NULL);
    }
    else {
        mShowPreview = FALSE;
    }

    mPreviewPopup->SetTouchable(FALSE);

    CPopupWindow::New(context, (IPopupWindow**)&mPopupKeyboard);
    mPopupKeyboard->SetBackgroundDrawable(NULL);
    //mPopupKeyboard.setClippingEnabled(FALSE);

    mPopupParent = this;
    //mPredicting = TRUE;

    CPaint::New((IPaint**)&mPaint);
    mPaint->SetAntiAlias(TRUE);
    mPaint->SetTextSize(keyTextSize);

    mPaint->SetTextAlign(Elastos::Droid::Graphics::PaintAlign_CENTER /*Align.CENTER*/);
    mPaint->SetAlpha(255);

    CRect::New(0, 0, 0, 0, (IRect**)&mPadding);
    mMiniKeyboardCache = new HashMap<AutoPtr<IKeyboardKey>, AutoPtr<IView> >();

    Boolean isPadding = FALSE;
    mKeyBackground->GetPadding(mPadding, &isPadding);

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);

    AutoPtr<IDisplayMetrics> dis;
    res->GetDisplayMetrics((IDisplayMetrics**)&dis);
    Float density;
    dis->GetDensity(&density);
    mSwipeThreshold = (Int32) (500 * density);
    res->GetBoolean(R::bool_::config_swipeDisambiguation,
            &mDisambiguateSwipe);

    AutoPtr<IAccessibilityManagerHelper> helper;
    CAccessibilityManagerHelper::AcquireSingleton((IAccessibilityManagerHelper**)&helper);
    helper->GetInstance(context, (IAccessibilityManager**)&mAccessibilityManager);

    AutoPtr<IInterface> audioService;
    context->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&audioService);
    mAudioManager = IAudioManager::Probe(audioService);

    ResetMultiTap();
    InitGestureDetector();

    return NOERROR;
}

void KeyboardView::InitGestureDetector()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IGestureDetectorOnGestureListener> listener = new _SimpleOnGestureListener(this);
    CGestureDetector::New(context, listener, (IGestureDetector**)&mGestureDetector);
    mGestureDetector->SetIsLongpressEnabled(FALSE);
}

ECode KeyboardView::SetOnKeyboardActionListener(
    /* [in] */ IOnKeyboardActionListener* listener)
{
    mKeyboardActionListener = listener;
    return NOERROR;
}

ECode KeyboardView::GetOnKeyboardActionListener(
    /* [out] */ IOnKeyboardActionListener** listener)
{
    assert(listener != NULL);
    *listener = mKeyboardActionListener;
    REFCOUNT_ADD(*listener);

    return NOERROR;
}

ECode KeyboardView::SetKeyboard(
    /* [in] */ IKeyboard* keyboard)
{
    if (mKeyboard != NULL) {
        ShowPreview(NOT_A_KEY);
    }
    // Remove any pending messages
    RemoveMessages();
    mKeyboard = keyboard;
    AutoPtr<IList> keys;
    mKeyboard->GetKeys((IList**)&keys);
    Int32 size = 0;
    keys->GetSize(&size);
    mKeys = ArrayOf<IKeyboardKey*>::Alloc(size);
    for (Int32 pos = 0; pos < size; pos++) {
        AutoPtr<IInterface> key;
        keys->Get(pos, ((IInterface**)&key));
        mKeys->Set(pos, IKeyboardKey::Probe(key));
    }

    RequestLayout();

    // Hint to reallocate the buffer if the size changed
    mKeyboardChanged = TRUE;
    InvalidateAllKeys();
    ComputeProximityThreshold(keyboard);
    mMiniKeyboardCache->Clear(); // Not really necessary to do every time, but will free up views
    // Switching to a different keyboard should abort any pending keys so that the key up
    // doesn't get delivered to the old or new keyboard
    mAbortKey = TRUE; // Until the next ACTION_DOWN
    return NOERROR;
}

ECode KeyboardView::GetKeyboard(
    /* [out] */ IKeyboard** keyboard)
{
    assert(keyboard != NULL);
    *keyboard = mKeyboard;
    REFCOUNT_ADD(*keyboard);

    return NOERROR;
}

ECode KeyboardView::SetShifted(
    /* [in] */ Boolean shifted,
    /* [out] */ Boolean* res)
{
    assert(res != NULL);
    if (mKeyboard != NULL) {
        Boolean succeeded = FALSE;
        if (mKeyboard->SetShifted(shifted, &succeeded), succeeded) {
            // The whole keyboard probably needs to be redrawn
            InvalidateAllKeys();
            *res = TRUE;
            return NOERROR;
        }
    }
    *res = FALSE;
    return NOERROR;
}

ECode KeyboardView::IsShifted(
    /* [out] */ Boolean* shifted)
{
    assert(shifted != NULL);
    if (mKeyboard != NULL) {
        return mKeyboard->IsShifted(shifted);
    }
    *shifted = FALSE;
    return NOERROR;
}

ECode KeyboardView::SetPreviewEnabled(
    /* [in] */ Boolean previewEnabled)
{
    mShowPreview = previewEnabled;
    return NOERROR;
}

ECode KeyboardView::IsPreviewEnabled(
    /* [out] */ Boolean* res)
{
    assert(res != NULL);
    *res = mShowPreview;
    return NOERROR;
}

ECode KeyboardView::SetVerticalCorrection(
    /* [in] */ Int32 verticalOffset)
{
    return NOERROR;
}

ECode KeyboardView::SetPopupParent(
    /* [in] */ IView* v)
{
    mPopupParent = v;
    return NOERROR;
}

ECode KeyboardView::SetPopupOffset(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    mMiniKeyboardOffsetX = x;
    mMiniKeyboardOffsetY = y;
    Boolean showing = FALSE;
    mPreviewPopup->IsShowing(&showing);
    if (showing) {
        mPreviewPopup->Dismiss();
    }
    return NOERROR;
}

ECode KeyboardView::SetProximityCorrectionEnabled(
    /* [in] */ Boolean enabled)
{
    mProximityCorrectOn = enabled;
    return NOERROR;
}

/**
 * Returns TRUE if proximity correction is enabled.
 */
ECode KeyboardView::IsProximityCorrectionEnabled(
    /* [out] */ Boolean* res)
{
    assert(res != NULL);
    *res = mProximityCorrectOn;
    return NOERROR;
}

/**
 * Popup keyboard close button clicked.
 * @hide
 */
ECode KeyboardView::OnClick(
    /* [in] */ IView* v)
{
    DismissPopupKeyboard();
    return NOERROR;
}

AutoPtr<ICharSequence> KeyboardView::AdjustCase(
    /* [in] */ ICharSequence* label)
{
    Boolean shifted;
    if ((mKeyboard->IsShifted(&shifted), shifted) && label != NULL) {
        Int32 len = 0;
        Char32 c = 0;
        label->GetLength(&len);
        label->GetCharAt(0, &c);
        if (len < 3 && Character::IsLowerCase(c)) {
            String str;
            label->ToString(&str);
            str = str.ToUpperCase();
            AutoPtr<ICharSequence> label2;
            CString::New(str, (ICharSequence**)&label2);
            return label2;
        }
    }
    return label;
}

ECode KeyboardView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    // Round up a little
    if (mKeyboard == NULL) {
        SetMeasuredDimension(mPaddingLeft + mPaddingRight, mPaddingTop + mPaddingBottom);
    }
    else {
        Int32 mw = 0;
        mKeyboard->GetMinWidth(&mw);
        Int32 width = mw + mPaddingLeft + mPaddingRight;

        if (View::MeasureSpec::GetSize(widthMeasureSpec) < width + 10) {
            width = View::MeasureSpec::GetSize(widthMeasureSpec);
        }
        Int32 th = 0;
        mKeyboard->GetHeight(&th);
        SetMeasuredDimension(width, th + mPaddingTop + mPaddingBottom);
    }
    return NOERROR;
}

void KeyboardView::ComputeProximityThreshold(
    /* [in] */ IKeyboard* keyboard)
{
    if (keyboard == NULL) return;
    if (mKeys == NULL) return;

    Int32 length = mKeys->GetLength();
    Int32 dimensionSum = 0;
    for (Int32 i = 0; i < length; i++) {
        IKeyboardKey* key = (*mKeys)[i];
        Int32 keyW, keyH, keyG;
        key->GetWidth(&keyW);
        key->GetHeight(&keyH);
        key->GetGap(&keyG);
        dimensionSum += Elastos::Core::Math::Min(keyW, keyH) + keyG;
    }
    if (dimensionSum < 0 || length == 0) return;
    mProximityThreshold = (Int32) (dimensionSum * 1.4f / length);
    mProximityThreshold *= mProximityThreshold; // Square it
}

ECode KeyboardView::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 oldw,
    /* [in] */ Int32 oldh)
{
    View::OnSizeChanged(w, h, oldw, oldh);
    if (mKeyboard != NULL) {
        ((Keyboard*)mKeyboard.Get())->Resize(w, h);
    }

    // Release the buffer, if any and it will be reallocated on the next draw
    mBuffer = NULL;
    return NOERROR;
}

void KeyboardView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    View::OnDraw(canvas);
    if (mDrawPending || mBuffer == NULL || mKeyboardChanged) {
        OnBufferDraw();
    }
    canvas->DrawBitmap(mBuffer, 0.f, 0.f, NULL);
}

void KeyboardView::OnBufferDraw()
{
    if (mBuffer == NULL || mKeyboardChanged) {
        Int32 bufferW = 0, bufferH = 0;
        if (mBuffer != NULL) {
            mBuffer->GetWidth(&bufferW);
            mBuffer->GetHeight(&bufferH);
        }

        Int32 sw = 0, sh = 0;
        if (mBuffer == NULL || (mKeyboardChanged &&
                (bufferW != (GetWidth(&sw), sw) || bufferH != (GetHeight(&sh), sh)))) {
            // Make sure our bitmap is at least 1x1
            const Int32 width = Elastos::Core::Math::Max(1, (GetWidth(&sw), sw));
            const Int32 height = Elastos::Core::Math::Max(1, (GetHeight(&sh), sh));

            mBuffer = NULL;
            CBitmap::CreateBitmap(width, height,
                BitmapConfig_ARGB_8888, (IBitmap**)&mBuffer);

            mCanvas = NULL;
            CCanvas::New(mBuffer, (ICanvas**)&mCanvas);
        }
        InvalidateAllKeys();
        mKeyboardChanged = FALSE;
    }
    Boolean isNotEmpty = FALSE;
    mCanvas->ClipRect(mDirtyRect, RegionOp_REPLACE, &isNotEmpty);

    if (mKeyboard == NULL) return;

    const Int32 kbdPaddingLeft = mPaddingLeft;
    const Int32 kbdPaddingTop = mPaddingTop;

    mPaint->SetColor(mKeyTextColor);
    Boolean drawSingleKey = FALSE;
    Boolean isNonEmpty = FALSE;
    if (mInvalidatedKey != NULL && (mCanvas->GetClipBounds(mClipRegion, &isNonEmpty), isNonEmpty)) {
        // Is clipRegion completely contained within the invalidated key?
        Int32 keyX, keyY, keyW, keyH;
        mInvalidatedKey->GetX(&keyX);
        mInvalidatedKey->GetY(&keyY);
        mInvalidatedKey->GetWidth(&keyW);
        mInvalidatedKey->GetHeight(&keyH);
        CRect* clipRegion = (CRect*)mClipRegion.Get();
        if (keyX + kbdPaddingLeft - 1 <= clipRegion->mLeft &&
              keyY + kbdPaddingTop - 1 <= clipRegion->mTop &&
              keyX + keyW + kbdPaddingLeft + 1 >= clipRegion->mRight &&
              keyY + keyH + kbdPaddingTop + 1 >= clipRegion->mBottom) {
            drawSingleKey = TRUE;
        }
    }
    mCanvas->DrawColor(0x00000000, PorterDuffMode_CLEAR);
    CRect* padding = (CRect*)mPadding.Get();
    const Int32 keyCount = mKeys->GetLength();
    for (Int32 i = 0; i < keyCount; i++) {
        AutoPtr<IKeyboardKey> key = (*mKeys)[i];
        if (drawSingleKey && mInvalidatedKey != key) {
            continue;
        }
        AutoPtr< ArrayOf<Int32> > drawableState;
        key->GetCurrentDrawableState((ArrayOf<Int32>**)&drawableState);
        Boolean isStateful = FALSE;
        mKeyBackground->SetState(drawableState.Get(), &isStateful);

        // Switch the character to uppercase if shift is pressed
        String label;
        AutoPtr<ICharSequence> l;
        key->GetLabel((ICharSequence**)&l);
        if (l != NULL) {
            AdjustCase(l)->ToString(&label);
        }

        Int32 keyW, keyH;
        key->GetWidth(&keyW);
        key->GetHeight(&keyH);
        AutoPtr<IRect> bounds;
        mKeyBackground->GetBounds((IRect**)&bounds);
        if (keyW != ((CRect*)bounds.Get())->mRight ||
                keyH != ((CRect*)bounds.Get())->mBottom) {
            mKeyBackground->SetBounds(0, 0, keyW, keyH);
        }
        Int32 keyX, keyY;
        key->GetX(&keyX);
        key->GetY(&keyY);
        mCanvas->Translate(keyX + kbdPaddingLeft, keyY + kbdPaddingTop);
        mKeyBackground->Draw(mCanvas);

        if (!label.IsNull()) {
            // For characters, use large font. For labels like "Done", use small font.
            if (label.GetLength() > 1 /*&& ((CKeyboardKey*)key.Get())->mCodes->GetLength() < 2*/) {
                mPaint->SetTextSize(mLabelTextSize);
                mPaint->SetTypeface(Typeface::DEFAULT_BOLD);
            }
            else {
                mPaint->SetTextSize(mKeyTextSize);
                mPaint->SetTypeface(Typeface::DEFAULT);
            }
            // Draw a drop shadow for the text
            mPaint->SetShadowLayer(mShadowRadius, 0, 0, mShadowColor);
            // Draw the text
            Float textSize = 0.0, tmpDescent = 0.0;
            mPaint->GetTextSize(&textSize);
            mPaint->Descent(&tmpDescent);
            mCanvas->DrawText(label,
                (keyW - padding->mLeft - padding->mRight) / 2 + padding->mLeft,
                (keyH - padding->mTop - padding->mBottom) / 2 + (textSize - tmpDescent) / 2 + padding->mTop,
                mPaint);
            // Turn off drop shadow
            mPaint->SetShadowLayer(0, 0, 0, 0);
        }
        else {
            AutoPtr<IDrawable> icon;
            key->GetIcon((IDrawable**)&icon);
            if (icon != NULL) {
                Int32 intrinsicWidth = 0, intrinsicHeight = 0;
                icon->GetIntrinsicWidth(&intrinsicWidth);
                icon->GetIntrinsicHeight(&intrinsicHeight);
                const Int32 drawableX = (keyW - padding->mLeft - padding->mRight
                        - intrinsicWidth) / 2 + padding->mLeft;
                const Int32 drawableY = (keyH - padding->mTop - padding->mBottom
                        - intrinsicHeight) / 2 + padding->mTop;
                mCanvas->Translate(drawableX, drawableY);
                icon->SetBounds(0, 0, intrinsicWidth, intrinsicHeight);
                icon->Draw(mCanvas);
                mCanvas->Translate(-drawableX, -drawableY);
            }
        }
        mCanvas->Translate(-keyX - kbdPaddingLeft, -keyY - kbdPaddingTop);
    }
    mInvalidatedKey = NULL;
    // Overlay a dark rectangle to dim the keyboard
    if (mMiniKeyboardOnScreen) {
        Int32 sw = 0, sh = 0;
        GetWidth(&sw);
        GetHeight(&sh);
        mPaint->SetColor((Int32)(mBackgroundDimAmount * 0xFF) << 24);
        mCanvas->DrawRect(0, 0, sw, sh, mPaint);
    }

    if (DEBUG && mShowTouchPoints) {
        mPaint->SetAlpha(128);
        mPaint->SetColor(0xFFFF0000);
        mCanvas->DrawCircle(mStartX, mStartY, 3, mPaint);
        mCanvas->DrawLine(mStartX, mStartY, mLastX, mLastY, mPaint);
        mPaint->SetColor(0xFF0000FF);
        mCanvas->DrawCircle(mLastX, mLastY, 3, mPaint);
        mPaint->SetColor(0xFF00FF00);
        mCanvas->DrawCircle((mStartX + mLastX) / 2, (mStartY + mLastY) / 2, 2, mPaint);
    }

    mDrawPending = FALSE;
    mDirtyRect->SetEmpty();
}

Int32 KeyboardView::GetKeyIndices(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ ArrayOf<Int32>* allKeys)
{
    Int32 primaryIndex = NOT_A_KEY;
    Int32 closestKey = NOT_A_KEY;
    Int32 closestKeyDist = mProximityThreshold + 1;
    for (Int32 i = 0; i < mDistances->GetLength(); i++) {
        (*mDistances)[i] = Elastos::Core::Math::INT32_MAX_VALUE;
    }
    AutoPtr< ArrayOf<Int32> > nearestKeyIndices;
    mKeyboard->GetNearestKeys(x, y, (ArrayOf<Int32>**)&nearestKeyIndices);
    const Int32 keyCount = nearestKeyIndices->GetLength();
    for (Int32 i = 0; i < keyCount; i++) {
        AutoPtr<IKeyboardKey> key = (*mKeys)[(*nearestKeyIndices)[i]];
        Int32 dist = 0;
        Boolean isInside = FALSE;
        key->IsInside(x, y, &isInside);
        if (isInside) {
            primaryIndex = (*nearestKeyIndices)[i];
        }

        key->SquaredDistanceFrom(x, y, &dist);
        AutoPtr< ArrayOf<Int32> > codes;
        key->GetCodes((ArrayOf<Int32>**)&codes);
        if (((mProximityCorrectOn
                && dist < mProximityThreshold)
                || isInside)
                && (*codes.Get())[0] > 32) {
            // Find insertion point
            const Int32 nCodes = codes->GetLength();
            if (dist < closestKeyDist) {
                closestKeyDist = dist;
                closestKey = (*nearestKeyIndices)[i];
            }

            if (allKeys == NULL) continue;

            for (Int32 j = 0; j < mDistances->GetLength(); j++) {
                if ((*mDistances)[j] > dist) {
                    // Make space for nCodes codes
                    mDistances->Copy(j + nCodes, mDistances, j, (mDistances->GetLength() - j - nCodes));
                    allKeys->Copy(j + nCodes, allKeys, j, (allKeys->GetLength() - j - nCodes));
                    for (Int32 c = 0; c < nCodes; c++) {
                        (*allKeys)[j + c] = (*codes.Get())[c];
                        (*mDistances)[j + c] = dist;
                    }
                    break;
                }
            }
        }
    }
    if (primaryIndex == NOT_A_KEY) {
        primaryIndex = closestKey;
    }
    return primaryIndex;
}

void KeyboardView::DetectAndSendKey(
    /* [in] */ Int32 index,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int64 eventTime)
{
    if (index != NOT_A_KEY && index < mKeys->GetLength()) {
        AutoPtr<IKeyboardKey> key = (*mKeys)[index];
        AutoPtr<ICharSequence> text;
        key->GetText((ICharSequence**)&text);
        if (text != NULL) {
            mKeyboardActionListener->OnText(text);
            mKeyboardActionListener->OnRelease(NOT_A_KEY);
        }
        else {
            AutoPtr< ArrayOf<Int32> > _codes;
            key->GetCodes((ArrayOf<Int32>**)&_codes);
            Int32 code = (*_codes.Get())[0];
            //TextEntryState.keyPressedAt(key, x, y);
            AutoPtr<ArrayOf<Int32> > codes = ArrayOf<Int32>::Alloc(MAX_NEARBY_KEYS);
            for (Int32 i = 0; i < MAX_NEARBY_KEYS; ++i) {
                   (*codes)[i] = NOT_A_KEY;
            }
            GetKeyIndices(x, y, codes);
            // Multi-tap
            if (mInMultiTap) {
                if (mTapCount != -1) {
                    AutoPtr<ArrayOf<Int32> > tmpArr = ArrayOf<Int32>::Alloc(1);
                    (*tmpArr)[0] = KEY_DELETE[0];
                    mKeyboardActionListener->OnKey(IKeyboard::KEYCODE_DELETE, tmpArr);
                }
                else {
                    mTapCount = 0;
                }
                code = (*_codes.Get())[mTapCount];
            }
            mKeyboardActionListener->OnKey(code, codes);
            mKeyboardActionListener->OnRelease(code);
        }
        mLastSentIndex = index;
        mLastTapTime = eventTime;
    }
}

/**
 * Handle multi-tap keys by producing the key label for the current multi-tap state.
 */
AutoPtr<ICharSequence> KeyboardView::GetPreviewText(
    /* [in] */ IKeyboardKey* key)
{
    if (mInMultiTap) {
        // Multi-tap
        AutoPtr< ArrayOf<Int32> > codes;
        key->GetCodes((ArrayOf<Int32>**)&codes);
        // mPreviewLabel.SetLength(0);
        mPreviewLabel += (Char32)(*codes.Get())[mTapCount < 0 ? 0 : mTapCount];
        AutoPtr<ICharSequence> text;
        CString::New(mPreviewLabel.ToString(), (ICharSequence**)&text);
        return AdjustCase(text);
    }
    else {
        AutoPtr<ICharSequence> label;
        key->GetLabel((ICharSequence**)&label);
        return AdjustCase(label);
    }
}

void KeyboardView::ShowPreview(
    /* [in] */ Int32 keyIndex)
{
    Int32 oldKeyIndex = mCurrentKeyIndex;

    mCurrentKeyIndex = keyIndex;
    // Release the old key and press the new key
    if (oldKeyIndex != mCurrentKeyIndex) {
        if (oldKeyIndex != NOT_A_KEY && mKeys->GetLength() > oldKeyIndex) {
            AutoPtr<IKeyboardKey> oldKey = (*mKeys)[oldKeyIndex];
            oldKey->OnReleased(mCurrentKeyIndex == NOT_A_KEY);
            InvalidateKey(oldKeyIndex);

            AutoPtr<ArrayOf<Int32> > codes;
            oldKey->GetCodes((ArrayOf<Int32>**)&codes);
            Int32 keyCode = (*codes)[0];
            SendAccessibilityEventForUnicodeCharacter(
                IAccessibilityEvent::TYPE_VIEW_HOVER_EXIT, keyCode);
            // TODO: We need to implement AccessibilityNodeProvider for this view.
            SendAccessibilityEventForUnicodeCharacter(
                IAccessibilityEvent::TYPE_VIEW_ACCESSIBILITY_FOCUS_CLEARED, keyCode);
        }
        if (mCurrentKeyIndex != NOT_A_KEY && mKeys->GetLength() > mCurrentKeyIndex) {
            AutoPtr<IKeyboardKey> newKey = (*mKeys)[mCurrentKeyIndex];
            newKey->OnPressed();
            InvalidateKey(mCurrentKeyIndex);

            AutoPtr<ArrayOf<Int32> > codes;
            newKey->GetCodes((ArrayOf<Int32>**)&codes);
            Int32 keyCode = (*codes)[0];
            SendAccessibilityEventForUnicodeCharacter(
                IAccessibilityEvent::TYPE_VIEW_HOVER_ENTER, keyCode);
            // TODO: We need to implement AccessibilityNodeProvider for this view.
            SendAccessibilityEventForUnicodeCharacter(
                IAccessibilityEvent::TYPE_VIEW_ACCESSIBILITY_FOCUSED, keyCode);
        }
    }
    // If key changed and preview is on ...
    if (oldKeyIndex != mCurrentKeyIndex && mShowPreview) {
        mHandler->RemoveMessages(MSG_SHOW_PREVIEW);
        Boolean showing = FALSE;
        if (mPreviewPopup->IsShowing(&showing), showing) {
            if (keyIndex == NOT_A_KEY) {
                Boolean result;
                mHandler->SendEmptyMessageDelayed(MSG_REMOVE_PREVIEW,
                    DELAY_AFTER_PREVIEW, &result);
            }
        }
        if (keyIndex != NOT_A_KEY) {
            mPreviewPopup->IsShowing(&showing);
            Int32 v = 0;
            IView::Probe(mPreviewText)->GetVisibility(&v);
            if (showing && v == IView::VISIBLE) {
                // Show right away, if it's already visible and finger is moving around
                ShowKey(keyIndex);
            }
            else {
                AutoPtr<IMessage> msg;
                mHandler->ObtainMessage(MSG_REMOVE_PREVIEW, keyIndex, 0, (IMessage**)&msg);
                Boolean result;
                mHandler->SendMessageDelayed(msg, DELAY_BEFORE_PREVIEW, &result);
            }
        }
    }
}

void KeyboardView::ShowKey(
    /* [in] */ Int32 keyIndex)
{
    if (keyIndex < 0 || keyIndex >= mKeys->GetLength()) return;
    AutoPtr<IKeyboardKey> key = (*mKeys)[keyIndex];
    AutoPtr<IDrawable> icon;
    key->GetIcon((IDrawable**)&icon);
    if (icon != NULL) {
        AutoPtr<IDrawable> iconPreview;
        key->GetIconPreview((IDrawable**)&iconPreview);
        mPreviewText->SetCompoundDrawables(NULL, NULL, NULL,
                iconPreview != NULL ? iconPreview : icon);
        mPreviewText->SetText((ICharSequence*)NULL);
    }
    else {
        mPreviewText->SetCompoundDrawables(NULL, NULL, NULL, NULL);
        AutoPtr<ICharSequence> text = GetPreviewText(key);
        mPreviewText->SetText(text);

        AutoPtr<ICharSequence> label;
        key->GetLabel((ICharSequence**)&label);
        Int32 len = 0;
        label->GetLength(&len);
        AutoPtr< ArrayOf<Int32> > codes;
        key->GetCodes((ArrayOf<Int32>**)&codes);
        if (len > 1 && codes->GetLength() < 2) {
            mPreviewText->SetTextSize(ITypedValue::COMPLEX_UNIT_PX, mKeyTextSize);
            mPreviewText->SetTypeface(Typeface::DEFAULT_BOLD);
        }
        else {
            mPreviewText->SetTextSize(ITypedValue::COMPLEX_UNIT_PX, mPreviewTextSizeLarge);
            mPreviewText->SetTypeface(Typeface::DEFAULT);
        }
    }
    IView::Probe(mPreviewText)->Measure(MeasureSpec::MakeMeasureSpec(0, MeasureSpec::UNSPECIFIED),
            MeasureSpec::MakeMeasureSpec(0, MeasureSpec::UNSPECIFIED));
    Int32 tmp = 0, tmpL = 0, tmpR = 0;
    IView::Probe(mPreviewText)->GetMeasuredWidth(&tmp);
    IView::Probe(mPreviewText)->GetPaddingLeft(&tmpL);
    IView::Probe(mPreviewText)->GetPaddingRight(&tmpR);
    Int32 keyW;
    key->GetWidth(&keyW);
    Int32 popupWidth = Elastos::Core::Math::Max(tmp, keyW + tmpL + tmpR);
    const Int32 popupHeight = mPreviewHeight;
    AutoPtr<IViewGroupLayoutParams> lp;
    IView::Probe(mPreviewText)->GetLayoutParams((IViewGroupLayoutParams**)&lp);
    if (lp != NULL) {
        lp->SetWidth(popupWidth);
        lp->SetHeight(popupHeight);
    }
    if (!mPreviewCentered) {
        IView::Probe(mPreviewText)->GetPaddingLeft(&tmpL);
        Int32 keyX, keyY;
        key->GetX(&keyX);
        key->GetY(&keyY);
        mPopupPreviewX = keyX - tmpL + mPaddingLeft;
        mPopupPreviewY = keyY - popupHeight + mPreviewOffset;
    }
    else {
        // TODO: Fix this if centering is brought back
        IView::Probe(mPreviewText)->GetMeasuredWidth(&tmp);
        mPopupPreviewX = 160 - tmp / 2;
        IView::Probe(mPreviewText)->GetMeasuredHeight(&tmp);
        mPopupPreviewY = - tmp;
    }

    mHandler->RemoveMessages(MSG_REMOVE_PREVIEW);

    GetLocationInWindow(mCoordinates);
    (*mCoordinates)[0] += mMiniKeyboardOffsetX; // Offset may be zero
    (*mCoordinates)[1] += mMiniKeyboardOffsetY; // Offset may be zero

    // Set the preview background state
    AutoPtr<IDrawable> drawable;
    IView::Probe(mPreviewText)->GetBackground((IDrawable**)&drawable);
    AutoPtr< ArrayOf<Int32> >tmpArr;
    Int32 resId;
    key->GetPopupResId(&resId);
    if (resId != 0) {
        tmpArr = ArrayOf<Int32>::Alloc(1);
        (*tmpArr)[0] = LONG_PRESSABLE_STATE_SET[0];
    }
    else {
        tmpArr = ArrayOf<Int32>::Alloc(0);
    }
    Boolean isStateful = FALSE;
    drawable->SetState(tmpArr, &isStateful);
    mPopupPreviewX += (*mCoordinates)[0];
    mPopupPreviewY += (*mCoordinates)[1];

    // If the popup cannot be shown above the key, put it on the side
    GetLocationOnScreen(mCoordinates);
    if (mPopupPreviewY + (*mCoordinates)[1] < 0) {
        // If the key you're pressing is on the left side of the keyboard, show the popup on
        // the right, offset by enough to see at least one key to the left/right.
        Int32 keyX, keyW;
        key->GetX(&keyX);
        key->GetWidth(&keyW);
        Int32 tmp = 0;
        GetWidth(&tmp);
        if (keyX + keyW <= tmp / 2) {
            mPopupPreviewX += (Int32)(keyW * 2.5);
        }
        else {
            mPopupPreviewX -= (Int32)(keyW * 2.5);
        }
        mPopupPreviewY += popupHeight;
    }

    Boolean showing = FALSE;
    if (mPreviewPopup->IsShowing(&showing), showing) {
        mPreviewPopup->Update(mPopupPreviewX, mPopupPreviewY,
                popupWidth, popupHeight);
    }
    else {
        mPreviewPopup->SetWidth(popupWidth);
        mPreviewPopup->SetHeight(popupHeight);
        mPreviewPopup->ShowAtLocation(mPopupParent, IGravity::NO_GRAVITY,
                mPopupPreviewX, mPopupPreviewY);
    }
    IView::Probe(mPreviewText)->SetVisibility(IView::VISIBLE);
}

void KeyboardView::SendAccessibilityEventForUnicodeCharacter(
    /* [in] */ Int32 eventType,
    /* [in] */ Int32 code)
{
    Boolean enabled = FALSE;
    if (mAccessibilityManager->IsEnabled(&enabled), enabled) {
        AutoPtr<IAccessibilityEventHelper> helper;
        CAccessibilityEventHelper::AcquireSingleton((IAccessibilityEventHelper**)&helper);
        AutoPtr<IAccessibilityEvent> event;
        helper->Obtain(eventType, (IAccessibilityEvent**)&event);
        OnInitializeAccessibilityEvent(event);

        // This is very efficient since the properties are cached.
        AutoPtr<IContentResolver> cr;
        mContext->GetContentResolver((IContentResolver**)&cr);
        AutoPtr<ISettingsSecure> ss;
        CSettingsSecure::AcquireSingleton((ISettingsSecure**)&ss);
        Int32 value;
        ss->GetInt32(cr, ISettingsSecure::ACCESSIBILITY_SPEAK_PASSWORD, 0, &value);
        Boolean speakPassword = value != 0;

        // Add text only if password announcement is enabled or if headset is
        // used to avoid leaking passwords.
        String text;
        Boolean bval1, bval2;
        if (speakPassword
            || (mAudioManager->IsBluetoothA2dpOn(&bval1), bval1)
            || (mAudioManager->IsWiredHeadsetOn(&bval2), bval2))
        {
            switch (code) {
                case IKeyboard::KEYCODE_ALT:
                    mContext->GetString(R::string::keyboardview_keycode_alt, &text);
                    break;
                case IKeyboard::KEYCODE_CANCEL:
                    mContext->GetString(R::string::keyboardview_keycode_cancel, &text);
                    break;
                case IKeyboard::KEYCODE_DELETE:
                    mContext->GetString(R::string::keyboardview_keycode_delete, &text);
                    break;
                case IKeyboard::KEYCODE_DONE:
                    mContext->GetString(R::string::keyboardview_keycode_done, &text);
                    break;
                case IKeyboard::KEYCODE_MODE_CHANGE:
                    mContext->GetString(R::string::keyboardview_keycode_mode_change, &text);
                    break;
                case IKeyboard::KEYCODE_SHIFT:
                    mContext->GetString(R::string::keyboardview_keycode_shift, &text);
                    break;
                case '\n':
                    mContext->GetString(R::string::keyboardview_keycode_enter, &text);
                    break;
                default: {
                    StringBuilder info;
                    info.AppendChar(code);
                    text = info.ToString();
                }
            }
        }
        else if (!mHeadsetRequiredToHearPasswordsAnnounced) {
            // We want the waring for required head set to be send with both the
            // hover enter and hover exit event, so set the flag after the exit.
            if (eventType == IAccessibilityEvent::TYPE_VIEW_HOVER_EXIT) {
                mHeadsetRequiredToHearPasswordsAnnounced = true;
            }
            mContext->GetString(R::string::keyboard_headset_required_to_hear_password, &text);
        }
        else {
            mContext->GetString(R::string::keyboard_password_character_no_headset, &text);
        }

        AutoPtr<ICharSequence> seq;
        CString::New(text, (ICharSequence**)&seq);
        AutoPtr<IList> bc;
        IAccessibilityRecord::Probe(event)->GetText((IList**)&bc);
        bc->Add(seq);
        mAccessibilityManager->SendAccessibilityEvent(event);
    }
}

ECode KeyboardView::InvalidateAllKeys()
{
    Int32 w = 0, h = 0;
    GetWidth(&w);
    GetHeight(&h);
    mDirtyRect->Union(0, 0, w, h);
    mDrawPending = TRUE;
    Invalidate();
    return NOERROR;
}

ECode KeyboardView::InvalidateKey(
    /* [in] */ Int32 keyIndex)
{
    if (mKeys == NULL) return NOERROR;
    if (keyIndex < 0 || keyIndex >= mKeys->GetLength()) {
        return NOERROR;
    }
    IKeyboardKey* key = (*mKeys)[keyIndex];
    mInvalidatedKey = key;
    Int32 keyX, keyY, keyW, keyH;
    key->GetX(&keyX);
    key->GetY(&keyY);
    key->GetWidth(&keyW);
    key->GetHeight(&keyH);
    mDirtyRect->Union(keyX + mPaddingLeft, keyY + mPaddingTop,
            keyX + keyW + mPaddingLeft, keyY + keyH + mPaddingTop);
    OnBufferDraw();
    Invalidate(keyX + mPaddingLeft, keyY + mPaddingTop,
            keyX + keyW + mPaddingLeft, keyY + keyH + mPaddingTop);
    return NOERROR;
}

Boolean KeyboardView::OpenPopupIfRequired(
    /* [in] */ IMotionEvent* me)
{
    // Check if we have a popup layout specified first.
    if (mPopupLayout == 0) {
        return FALSE;
    }
    assert(mKeys != NULL);
    if (mCurrentKey < 0 || mCurrentKey >= mKeys->GetLength()) {
        return FALSE;
    }

    AutoPtr<IKeyboardKey> popupKey = (*mKeys)[mCurrentKey];
    Boolean result = FALSE;
    OnLongPress(popupKey, &result);
    if (result) {
        mAbortKey = TRUE;
        ShowPreview(NOT_A_KEY);
    }
    return result;
}

ECode KeyboardView::OnLongPress(
    /* [in] */ IKeyboardKey* popupKey,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    Int32 popupKeyboardId = 0;
    popupKey->GetPopupResId(&popupKeyboardId);

    if (popupKeyboardId != 0) {
        mMiniKeyboardContainer = (*mMiniKeyboardCache)[popupKey];
        if (mMiniKeyboardContainer == NULL) {
            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            AutoPtr<IInterface> service;
            FAIL_RETURN(context->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&service))
            AutoPtr<ILayoutInflater> inflater = ILayoutInflater::Probe(service);
            inflater->Inflate(mPopupLayout, NULL, (IView**)&mMiniKeyboardContainer);
            AutoPtr<IView> tempView;
            mMiniKeyboardContainer->FindViewById(R::id::keyboardView, (IView**)&tempView);
            mMiniKeyboard = IKeyboardView::Probe(tempView);
            AutoPtr<IView> closeButton;
            mMiniKeyboardContainer->FindViewById(R::id::closeButton, (IView**)&closeButton);
            if (closeButton != NULL) {
                AutoPtr<IViewOnClickListener> closeListener = new CloseButtonClickListener(this);
                closeButton->SetOnClickListener(closeListener);
            }
            AutoPtr<_OnKeyboardActionListener> listener = new _OnKeyboardActionListener(this);
            mMiniKeyboard->SetOnKeyboardActionListener(listener);
            //mInputView.setSuggest(mSuggest);
            AutoPtr<Keyboard> keyboard;
            AutoPtr<ICharSequence> popupCharacters;
            popupKey->GetPopupCharacters((ICharSequence**)&popupCharacters);
            if (popupCharacters != NULL) {
                Int32 l = 0, r = 0;
                GetPaddingLeft(&l);
                GetPaddingRight(&r);
                keyboard = new Keyboard();
                keyboard->constructor(context, popupKeyboardId, popupCharacters, -1, l + r);
            }
            else {
                keyboard = new Keyboard();
                keyboard->constructor(context, popupKeyboardId);
            }
            ((KeyboardView*)mMiniKeyboard.Get())->SetKeyboard((IKeyboard*)keyboard);
            ((KeyboardView*)mMiniKeyboard.Get())->SetPopupParent(this);
            Int32 w = 0, h = 0;
            GetWidth(&w);
            GetHeight(&h);
            mMiniKeyboardContainer->Measure(
                    View::MeasureSpec::MakeMeasureSpec(w, View::MeasureSpec::AT_MOST),
                    View::MeasureSpec::MakeMeasureSpec(h, View::MeasureSpec::AT_MOST));

            (*mMiniKeyboardCache)[popupKey] = mMiniKeyboardContainer;
        }
        else {
            AutoPtr<IView> tempView;
            mMiniKeyboardContainer->FindViewById(R::id::keyboardView, (IView**)&tempView);
            mMiniKeyboard = IKeyboardView::Probe(tempView);
        }

        GetLocationInWindow(mCoordinates);
        Int32 keyX, keyY, keyW;
        popupKey->GetX(&keyX);
        popupKey->GetY(&keyY);
        popupKey->GetWidth(&keyW);
        mPopupX = keyX + mPaddingLeft;
        mPopupY = keyY + mPaddingTop;
        Int32 meauseredW = 0, meauseredH = 0;
        mMiniKeyboardContainer->GetMeasuredWidth(&meauseredW);
        mPopupX = mPopupX + keyW - meauseredW;
        mMiniKeyboardContainer->GetMeasuredHeight(&meauseredH);
        mPopupY = mPopupY - meauseredH;
        Int32 paddingRight = 0, paddingBottom = 0;
        mMiniKeyboardContainer->GetPaddingRight(&paddingRight);
        Int32 x = mPopupX + paddingRight + (*mCoordinates)[0];
        mMiniKeyboardContainer->GetPaddingBottom(&paddingBottom);
        Int32 y = mPopupY + paddingBottom + (*mCoordinates)[1];
        mMiniKeyboard->SetPopupOffset(x < 0 ? 0 : x, y);
        Boolean shifted = FALSE;
        IsShifted(&shifted);
        Boolean temp = FALSE;
        mMiniKeyboard->SetShifted(shifted, &temp);
        mPopupKeyboard->SetContentView(IView::Probe(mMiniKeyboardContainer));
        mPopupKeyboard->SetWidth(meauseredW);
        mPopupKeyboard->SetHeight(meauseredH);
        mPopupKeyboard->ShowAtLocation(this, IGravity::NO_GRAVITY, x, y);
        mMiniKeyboardOnScreen = TRUE;
        //mMiniKeyboard.onTouchEvent(getTranslatedEvent(me));
        InvalidateAllKeys();
        *res = TRUE;
        return NOERROR;
    }
    *res = FALSE;
    return NOERROR;
}

ECode KeyboardView::OnHoverEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    Boolean enabled = TRUE;
    Int32 count = 0;
    if ((mAccessibilityManager->IsTouchExplorationEnabled(&enabled), enabled)
        && (event->GetPointerCount(&count), count) == 1) {
        Int32 action = 0;
        event->GetAction(&action);
        switch (action) {
            case IMotionEvent::ACTION_HOVER_ENTER: {
                event->SetAction(IMotionEvent::ACTION_DOWN);
            } break;
            case IMotionEvent::ACTION_HOVER_MOVE: {
                event->SetAction(IMotionEvent::ACTION_MOVE);
            } break;
            case IMotionEvent::ACTION_HOVER_EXIT: {
                event->SetAction(IMotionEvent::ACTION_UP);
            } break;
        }

        return OnTouchEvent(event, res);
    }

    *res = TRUE;
    return NOERROR;
}

ECode KeyboardView::OnTouchEvent(
    /* [in] */ IMotionEvent* me,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    // Convert multi-pointer up/down events to single up/down events to
    // deal with the typical multi-pointer behavior of two-thumb typing
    Int32 pointerCount = 0;
    me->GetPointerCount(&pointerCount);
    Int32 action = 0;
    me->GetAction(&action);
    Boolean result = FALSE;
    Int64 now = 0;
    IInputEvent::Probe(me)->GetEventTime(&now);

    if (pointerCount != mOldPointerCount) {
        if (pointerCount == 1) {
            // Send a down event for the latest pointer
            Float tX = 0.0, tY = 0.0;
            me->GetX(&tX);
            me->GetY(&tY);
            Int32 tState = 0;
            me->GetMetaState(&tState);
            AutoPtr<IMotionEvent> down;
            MotionEvent::Obtain(now, now, IKeyEvent::ACTION_DOWN,
                    tX, tY, tState, (IMotionEvent**)&down);
            result = OnModifiedTouchEvent(down, FALSE);
            IInputEvent::Probe(down)->Recycle();

            // If it's an up action, then deliver the up as well.
            if (action == IKeyEvent::ACTION_UP) {
                result = OnModifiedTouchEvent(me, TRUE);
            }
        }
        else {
            Int32 tState = 0;
            me->GetMetaState(&tState);
            // Send an up event for the last pointer
            AutoPtr<IMotionEvent> up;
            MotionEvent::Obtain(now, now, IKeyEvent::ACTION_UP,
                    mOldPointerX, mOldPointerY, tState, (IMotionEvent**)&up);
            result = OnModifiedTouchEvent(up, TRUE);
            IInputEvent::Probe(up)->Recycle();
        }
    }
    else {
        if (pointerCount == 1) {
            result = OnModifiedTouchEvent(me, FALSE);
            me->GetX(&mOldPointerX);
            me->GetY(&mOldPointerY);
        }
        else {
            // Don't do anything when 2 pointers are down and moving.
            result = TRUE;
        }
    }
    mOldPointerCount = pointerCount;

    *res = result;
    return NOERROR;
}

Boolean KeyboardView::OnModifiedTouchEvent(
    /* [in] */ IMotionEvent* me,
    /* [in] */ Boolean possiblePoly)
{
    Float tmpX = 0, tmpY = 0;
    me->GetX(&tmpX);
    me->GetY(&tmpY);
    Int32 touchX = (Int32) tmpX - mPaddingLeft;
    Int32 touchY = (Int32) tmpY - mPaddingTop;
    if (touchY >= -mVerticalCorrection) {
        touchY += mVerticalCorrection;
    }

    Int32 action = 0;
    me->GetAction(&action);
    Int64 eventTime = 0;
    IInputEvent::Probe(me)->GetEventTime(&eventTime);
    Int32 keyIndex = GetKeyIndices(touchX, touchY, NULL);
    mPossiblePoly = possiblePoly;

    // Track the last few movements to look for spurious swipes.
    if (action == IKeyEvent::ACTION_DOWN) mSwipeTracker->Clear();
    mSwipeTracker->AddMovement(me);

    // Ignore all motion events until a DOWN.
    if (mAbortKey
            && action != IKeyEvent::ACTION_DOWN && action != IMotionEvent::ACTION_CANCEL) {
        return TRUE;
    }

    Boolean tmp = FALSE;
    if (mGestureDetector->OnTouchEvent(me, &tmp), tmp) {
        ShowPreview(NOT_A_KEY);
            mHandler->RemoveMessages(MSG_REPEAT);
            mHandler->RemoveMessages(MSG_LONGPRESS);
        return TRUE;
    }

    // Needs to be called after the gesture detector gets a turn, as it may have
    // displayed the mini keyboard
    if (mMiniKeyboardOnScreen && action != IMotionEvent::ACTION_CANCEL) {
        return TRUE;
    }

    switch (action) {
        case IKeyEvent::ACTION_DOWN: {
            mAbortKey = FALSE;
            mStartX = touchX;
            mStartY = touchY;
            mLastCodeX = touchX;
            mLastCodeY = touchY;
            mLastKeyTime = 0;
            mCurrentKeyTime = 0;
            mLastKey = NOT_A_KEY;
            mCurrentKey = keyIndex;
            mDownKey = keyIndex;
            IInputEvent::Probe(me)->GetEventTime(&mDownTime);
            mLastMoveTime = mDownTime;
            CheckMultiTap(eventTime, keyIndex);
            if (keyIndex != NOT_A_KEY) {
                AutoPtr< ArrayOf<Int32> > codes;
                (*mKeys)[keyIndex]->GetCodes((ArrayOf<Int32>**)&codes);
                mKeyboardActionListener->OnPress((*codes.Get())[0]);
            }
            else {
                mKeyboardActionListener->OnPress(0);
            }
            if (mCurrentKey >= 0) {
                Boolean repeatable;
                (*mKeys)[mCurrentKey]->IsRepeatable(&repeatable);
                if (repeatable) {
                    mRepeatKeyIndex = mCurrentKey;

                    AutoPtr<IMessage> msg;
                    mHandler->ObtainMessage(MSG_REPEAT, (IMessage**)&msg);
                    Boolean result;
                    mHandler->SendMessageDelayed(msg, REPEAT_START_DELAY, &result);

                    RepeatKey();
                    // Delivering the key could have caused an abort
                    if (mAbortKey) {
                        mRepeatKeyIndex = NOT_A_KEY;
                        break;
                    }
                }
            }
            if (mCurrentKey != NOT_A_KEY) {
                AutoPtr<IMessage> msg;
                mHandler->ObtainMessage(MSG_LONGPRESS, me, (IMessage**)&msg);
                Boolean result;
                mHandler->SendMessageDelayed(msg, LONGPRESS_TIMEOUT, &result);
            }
            ShowPreview(keyIndex);
            break;
        }

        case IMotionEvent::ACTION_MOVE: {
            Boolean continueLongPress = FALSE;
            if (keyIndex != NOT_A_KEY) {
                if (mCurrentKey == NOT_A_KEY) {
                    mCurrentKey = keyIndex;
                    mCurrentKeyTime = eventTime - mDownTime;
                }
                else {
                    if (keyIndex == mCurrentKey) {
                        mCurrentKeyTime += eventTime - mLastMoveTime;
                        continueLongPress = TRUE;
                    }
                    else if (mRepeatKeyIndex == NOT_A_KEY) {
                        ResetMultiTap();
                        mLastKey = mCurrentKey;
                        mLastCodeX = mLastX;
                        mLastCodeY = mLastY;
                        mLastKeyTime = mCurrentKeyTime + eventTime - mLastMoveTime;
                        mCurrentKey = keyIndex;
                        mCurrentKeyTime = 0;
                    }
                }
            }
            if (!continueLongPress) {
                // Cancel old longpress
                mHandler->RemoveMessages(MSG_LONGPRESS);
                // Start new longpress if key has changed
                if (keyIndex != NOT_A_KEY) {
                    AutoPtr<IMessage> msg;
                    mHandler->ObtainMessage(MSG_LONGPRESS, me, (IMessage**)&msg);
                    Boolean result;
                    mHandler->SendMessageDelayed(msg, LONGPRESS_TIMEOUT, &result);
                }
            }
            ShowPreview(mCurrentKey);
            mLastMoveTime = eventTime;
            break;
        }

        case IKeyEvent::ACTION_UP: {
            RemoveMessages();
            if (keyIndex == mCurrentKey) {
                mCurrentKeyTime += eventTime - mLastMoveTime;
            }
            else {
                ResetMultiTap();
                mLastKey = mCurrentKey;
                mLastKeyTime = mCurrentKeyTime + eventTime - mLastMoveTime;
                mCurrentKey = keyIndex;
                mCurrentKeyTime = 0;
            }
            if (mCurrentKeyTime < mLastKeyTime && mCurrentKeyTime < DEBOUNCE_TIME
                    && mLastKey != NOT_A_KEY) {
                mCurrentKey = mLastKey;
                touchX = mLastCodeX;
                touchY = mLastCodeY;
            }
            ShowPreview(NOT_A_KEY);
            for (Int32 i = 0; i < mKeyIndices->GetLength(); ++i) {
                (*mKeyIndices)[i] = NOT_A_KEY;
            }
            // If we're not on a repeating key (which sends on a DOWN event)
            if (mRepeatKeyIndex == NOT_A_KEY && !mMiniKeyboardOnScreen && !mAbortKey) {
                DetectAndSendKey(mCurrentKey, touchX, touchY, eventTime);
            }
            InvalidateKey(keyIndex);
            mRepeatKeyIndex = NOT_A_KEY;
            break;
        }

        case IMotionEvent::ACTION_CANCEL: {
            RemoveMessages();
            DismissPopupKeyboard();
            mAbortKey = TRUE;
            ShowPreview(NOT_A_KEY);
            InvalidateKey(mCurrentKey);
            break;
        }
    }
    mLastX = touchX;
    mLastY = touchY;
    return TRUE;
}

Boolean KeyboardView::RepeatKey()
{
    IKeyboardKey* key = (*mKeys)[mRepeatKeyIndex];
    Int32 keyX, keyY;
    key->GetX(&keyX);
    key->GetY(&keyY);
    DetectAndSendKey(mCurrentKey, keyX, keyY, mLastTapTime);
    return TRUE;
}

void KeyboardView::SwipeRight()
{
    assert(mKeyboardActionListener != NULL);
    mKeyboardActionListener->SwipeRight();
}

void KeyboardView::SwipeLeft()
{
    assert(mKeyboardActionListener != NULL);
    mKeyboardActionListener->SwipeLeft();
}

void KeyboardView::SwipeUp()
{
    assert(mKeyboardActionListener != NULL);
    mKeyboardActionListener->SwipeUp();
}

void KeyboardView::SwipeDown()
{
    assert(mKeyboardActionListener != NULL);
    mKeyboardActionListener->SwipeDown();
}

ECode KeyboardView::Closing()
{
    assert(mPreviewPopup != NULL);
    Boolean showing = FALSE;
    if (mPreviewPopup->IsShowing(&showing), showing) {
        mPreviewPopup->Dismiss();
    }
    RemoveMessages();

    DismissPopupKeyboard();
    mBuffer = NULL;
    mCanvas = NULL;
    mMiniKeyboardCache->Clear();
    return NOERROR;
}

void KeyboardView::RemoveMessages()
{
    mHandler->RemoveMessages(MSG_REPEAT);
    mHandler->RemoveMessages(MSG_LONGPRESS);
    mHandler->RemoveMessages(MSG_SHOW_PREVIEW);
}

ECode KeyboardView::OnDetachedFromWindow()
{
    View::OnDetachedFromWindow();
    Closing();
    return NOERROR;
}

void KeyboardView::DismissPopupKeyboard()
{
    assert(mPopupKeyboard != NULL);
    Boolean showing = FALSE;
    if (mPopupKeyboard->IsShowing(&showing), showing) {
        mPopupKeyboard->Dismiss();
        mMiniKeyboardOnScreen = FALSE;
        InvalidateAllKeys();
    }
}

ECode KeyboardView::HandleBack(
    /* [out] */ Boolean* res)
{
    assert(res != NULL && mPopupKeyboard != NULL);
    Boolean showing = FALSE;
    if (mPopupKeyboard->IsShowing(&showing), showing) {
        DismissPopupKeyboard();
        *res = TRUE;
        return NOERROR;
    }
    *res = FALSE;
    return NOERROR;
}

void KeyboardView::ResetMultiTap()
{
    mLastSentIndex = NOT_A_KEY;
    mTapCount = 0;
    mLastTapTime = -1;
    mInMultiTap = FALSE;
}

void KeyboardView::CheckMultiTap(
    /* [in] */ Int64 eventTime,
    /* [in] */ Int32 keyIndex)
{
    if (keyIndex == NOT_A_KEY) return;
    IKeyboardKey* key = (*mKeys)[keyIndex];
    AutoPtr< ArrayOf<Int32> > codes;
    key->GetCodes((ArrayOf<Int32>**)&codes);
    if (codes->GetLength() > 1) {
        mInMultiTap = TRUE;
        if (eventTime < mLastTapTime + MULTITAP_INTERVAL
                && keyIndex == mLastSentIndex) {
            mTapCount = (mTapCount + 1) % codes->GetLength();
            return;
        }
        else {
            mTapCount = -1;
            return;
        }
    }
    if (eventTime > mLastTapTime + MULTITAP_INTERVAL || keyIndex != mLastSentIndex) {
        ResetMultiTap();
    }
}


} // namespace InputMethodService
} // namespace Droid
} // namespace Elastos
