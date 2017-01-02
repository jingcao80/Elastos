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

#include "Elastos.Droid.Widget.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/R.h"
#include "elastos/droid/widget/AdapterViewFlipper.h"
#include "elastos/core/StringUtils.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::EIID_IBroadcastReceiver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::EIID_IHandler;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::Process;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Core::CString;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Widget {

//=====================================================================
//              AdapterViewFlipper::InnerBroadcastReceiver
//=====================================================================
CAR_INTERFACE_IMPL(AdapterViewFlipper::InnerBroadcastReceiver, Object, IBroadcastReceiver)

AdapterViewFlipper::InnerBroadcastReceiver::InnerBroadcastReceiver(
    /* [in] */ AdapterViewFlipper* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;

    assert(mOwner);
}

ECode AdapterViewFlipper::InnerBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(intent);
    // ==================before translated======================
    // final String action = intent.getAction();
    // if (Intent.ACTION_SCREEN_OFF.equals(action)) {
    //     mUserPresent = false;
    //     updateRunning();
    // } else if (Intent.ACTION_USER_PRESENT.equals(action)) {
    //     mUserPresent = true;
    //     updateRunning(false);
    // }

    assert(0);
    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_SCREEN_OFF.Equals(action)) {
        mOwner->mUserPresent = FALSE;
        mOwner->UpdateRunning();
    }
    else if (IIntent::ACTION_USER_PRESENT.Equals(action)) {
        mOwner->mUserPresent = TRUE;
        mOwner->UpdateRunning(FALSE);
    }
    return NOERROR;
}

//=====================================================================
//                   AdapterViewFlipper::InnerHandler
//=====================================================================
CAR_INTERFACE_IMPL(AdapterViewFlipper::InnerHandler, Object, IHandler)

AdapterViewFlipper::InnerHandler::InnerHandler(
    /* [in] */ AdapterViewFlipper* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;

    assert(mOwner);
}

ECode AdapterViewFlipper::InnerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    VALIDATE_NOT_NULL(msg);
    // ==================before translated======================
    // if (msg.what == FLIP_MSG) {
    //     if (mRunning) {
    //         showNext();
    //     }
    // }

    assert(0);
    Int32 what = 0;
    msg->GetWhat(&what);
    if (what == mOwner->FLIP_MSG) {
        if (mOwner->mRunning) {
            mOwner->ShowNext();
        }
    }
    return NOERROR;
}

//=====================================================================
//                          AdapterViewFlipper
//=====================================================================
const String AdapterViewFlipper::TAG("ViewFlipper");
const Boolean AdapterViewFlipper::LOGD = FALSE;
const Int32 AdapterViewFlipper::DEFAULT_INTERVAL;

AdapterViewFlipper::AdapterViewFlipper()
    : mFlipInterval(0)
    , mAutoStart(FALSE)
    , mRunning(FALSE)
    , mStarted(FALSE)
    , mVisible(FALSE)
    , mUserPresent(FALSE)
    , mAdvancedByHost(FALSE)
    , mReceiver(NULL)
    , FLIP_MSG(1)
{
}

ECode AdapterViewFlipper::constructor(
    /* [in] */ IContext* context)
{
    // ==================before translated======================
    // super(context);

    return AdapterViewAnimator::constructor(context);
}

ECode AdapterViewFlipper::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    // ==================before translated======================
    // this(context, attrs, 0);

    return constructor(context, attrs, 0);
}

ECode AdapterViewFlipper::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    // ==================before translated======================
    // this(context, attrs, defStyleAttr, 0);

    return constructor(context, attrs, defStyleAttr, 0);
}

ECode AdapterViewFlipper::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    // ==================before translated======================
    // super(context, attrs, defStyleAttr, defStyleRes);
    //
    // final TypedArray a = context.obtainStyledAttributes(attrs,
    //         com.android.internal.R.styleable.AdapterViewFlipper, defStyleAttr, defStyleRes);
    // mFlipInterval = a.getInt(
    //         com.android.internal.R.styleable.AdapterViewFlipper_flipInterval, DEFAULT_INTERVAL);
    // mAutoStart = a.getBoolean(
    //         com.android.internal.R.styleable.AdapterViewFlipper_autoStart, false);
    //
    // // A view flipper should cycle through the views
    // mLoopViews = true;
    //
    // a.recycle();

    AdapterViewAnimator::constructor(context, attrs, defStyleAttr, defStyleRes);

    AutoPtr< ArrayOf<Int32> > styleFlipper = TO_ATTRS_ARRAYOF(R::styleable::AdapterViewFlipper);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, styleFlipper, defStyleAttr, defStyleRes, (ITypedArray**)&a);
    a->GetInt32(R::styleable::AdapterViewFlipper_flipInterval, DEFAULT_INTERVAL, &mFlipInterval);
    a->GetBoolean(R::styleable::AdapterViewFlipper_autoStart, FALSE, &mAutoStart);

    // A view flipper should cycle through the views
    mLoopViews = TRUE;
    a->Recycle();
    return NOERROR;
}

ECode AdapterViewFlipper::SetAdapter(
    /* [in] */ IAdapter* adapter)
{
    VALIDATE_NOT_NULL(adapter);
    // ==================before translated======================
    // super.setAdapter(adapter);
    // updateRunning();

    AdapterViewAnimator::SetAdapter(adapter);
    UpdateRunning();
    return NOERROR;
}

ECode AdapterViewFlipper::GetFlipInterval(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mFlipInterval;

    *result = mFlipInterval;
    return NOERROR;
}

ECode AdapterViewFlipper::SetFlipInterval(
    /* [in] */ Int32 flipInterval)
{
    // ==================before translated======================
    // mFlipInterval = flipInterval;

    mFlipInterval = flipInterval;
    return NOERROR;
}

ECode AdapterViewFlipper::StartFlipping()
{
    // ==================before translated======================
    // mStarted = true;
    // updateRunning();

    mStarted = TRUE;
    UpdateRunning();
    return NOERROR;
}

ECode AdapterViewFlipper::StopFlipping()
{
    // ==================before translated======================
    // mStarted = false;
    // updateRunning();

    mStarted = FALSE;
    UpdateRunning();
    return NOERROR;
}

ECode AdapterViewFlipper::ShowNext()
{
    // ==================before translated======================
    // // if the flipper is currently flipping automatically, and showNext() is called
    // // we should we should make sure to reset the timer
    // if (mRunning) {
    //     mHandler.removeMessages(FLIP_MSG);
    //     Message msg = mHandler.obtainMessage(FLIP_MSG);
    //     mHandler.sendMessageDelayed(msg, mFlipInterval);
    // }
    // super.showNext();

    if (mRunning) {
        mHandler->RemoveMessages(FLIP_MSG);
        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(FLIP_MSG, (IMessage**)&msg);
        Boolean resTmp = FALSE;
        mHandler->SendMessageDelayed(msg, mFlipInterval, &resTmp);
    }

    AdapterViewAnimator::ShowNext();
    return NOERROR;
}

ECode AdapterViewFlipper::ShowPrevious()
{
    // ==================before translated======================
    // // if the flipper is currently flipping automatically, and showPrevious() is called
    // // we should we should make sure to reset the timer
    // if (mRunning) {
    //     mHandler.removeMessages(FLIP_MSG);
    //     Message msg = mHandler.obtainMessage(FLIP_MSG);
    //     mHandler.sendMessageDelayed(msg, mFlipInterval);
    // }
    // super.showPrevious();

    if (mRunning) {
        mHandler->RemoveMessages(FLIP_MSG);
        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(FLIP_MSG, (IMessage**)&msg);
        Boolean resTmp = FALSE;
        mHandler->SendMessageDelayed(msg, mFlipInterval, &resTmp);
    }

    AdapterViewAnimator::ShowPrevious();
    return NOERROR;
}

ECode AdapterViewFlipper::IsFlipping(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mStarted;

    *result = mStarted;
    return NOERROR;
}

ECode AdapterViewFlipper::SetAutoStart(
    /* [in] */ Boolean autoStart)
{
    // ==================before translated======================
    // mAutoStart = autoStart;

    mAutoStart = autoStart;
    return NOERROR;
}

ECode AdapterViewFlipper::IsAutoStart(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mAutoStart;

    *result = mAutoStart;
    return NOERROR;
}

ECode AdapterViewFlipper::FyiWillBeAdvancedByHostKThx()
{
    // ==================before translated======================
    // mAdvancedByHost = true;
    // updateRunning(false);

    mAdvancedByHost = TRUE;
    UpdateRunning(FALSE);
    return NOERROR;
}

ECode AdapterViewFlipper::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    VALIDATE_NOT_NULL(event);
    // ==================before translated======================
    // super.onInitializeAccessibilityEvent(event);
    // event.setClassName(AdapterViewFlipper.class.getName());

    AdapterViewAnimator::OnInitializeAccessibilityEvent(event);
    IAccessibilityRecord* recordTmp = IAccessibilityRecord::Probe(event);
    AutoPtr<ICharSequence> charSequenceTmp;
    CString::New(String("AdapterViewFlipper")/*AdapterViewFlipper.class.getName()*/, (ICharSequence**)&charSequenceTmp);
    recordTmp->SetClassName(charSequenceTmp);
    return NOERROR;
}

ECode AdapterViewFlipper::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    VALIDATE_NOT_NULL(info);
    // ==================before translated======================
    // super.onInitializeAccessibilityNodeInfo(info);
    // info.setClassName(AdapterViewFlipper.class.getName());

    AdapterViewAnimator::OnInitializeAccessibilityNodeInfo(info);
    IAccessibilityRecord* recordTmp = IAccessibilityRecord::Probe(info);
    AutoPtr<ICharSequence> charSequenceTmp;
    CString::New(String("AdapterViewFlipper")/*AdapterViewFlipper.class.getName()*/, (ICharSequence**)&charSequenceTmp);
    recordTmp->SetClassName(charSequenceTmp);
    return NOERROR;
}

ECode AdapterViewFlipper::OnAttachedToWindow()
{
    // ==================before translated======================
    // super.onAttachedToWindow();
    //
    // // Listen for broadcasts related to user-presence
    // final IntentFilter filter = new IntentFilter();
    // filter.addAction(Intent.ACTION_SCREEN_OFF);
    // filter.addAction(Intent.ACTION_USER_PRESENT);
    //
    // // OK, this is gross but needed. This class is supported by the
    // // remote views machanism and as a part of that the remote views
    // // can be inflated by a context for another user without the app
    // // having interact users permission - just for loading resources.
    // // For exmaple, when adding widgets from a user profile to the
    // // home screen. Therefore, we register the receiver as the current
    // // user not the one the context is for.
    // getContext().registerReceiverAsUser(mReceiver, android.os.Process.myUserHandle(),
    //         filter, null, mHandler);
    //
    //
    // if (mAutoStart) {
    //     // Automatically start when requested
    //     startFlipping();
    // }

    AdapterViewAnimator::OnAttachedToWindow();
    // Listen for broadcasts related to user-presence
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_SCREEN_OFF);
    filter->AddAction(IIntent::ACTION_USER_PRESENT);

    // OK, this is gross but needed. This class is supported by the
    // remote views machanism and as a part of that the remote views
    // can be inflated by a context for another user without the app
    // having interact users permission - just for loading resources.
    // For exmaple, when adding widgets from a user profile to the
    // home screen. Therefore, we register the receiver as the current
    // user not the one the context is for.
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IUserHandle> userHandle;
    Process::MyUserHandle((IUserHandle**)&userHandle);
    AutoPtr<IIntent> intent;
    context->RegisterReceiverAsUser(mReceiver, userHandle, filter, String(""), mHandler, (IIntent**)&intent);

    if (mAutoStart) {
        // Automatically start when requested
        StartFlipping();
    }
    return NOERROR;
}

ECode AdapterViewFlipper::OnDetachedFromWindow()
{
    // ==================before translated======================
    // super.onDetachedFromWindow();
    // mVisible = false;
    //
    // getContext().unregisterReceiver(mReceiver);
    // updateRunning();

    AdapterViewAnimator::OnDetachedFromWindow();
    mVisible = FALSE;

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    context->UnregisterReceiver(mReceiver);
    UpdateRunning();
    return NOERROR;
}

ECode AdapterViewFlipper::OnWindowVisibilityChanged(
    /* [in] */ Int32 visibility)
{
    // ==================before translated======================
    // super.onWindowVisibilityChanged(visibility);
    // mVisible = (visibility == VISIBLE);
    // updateRunning(false);

    AdapterViewAnimator::OnWindowVisibilityChanged(visibility);
    mVisible = (visibility == VISIBLE);
    UpdateRunning(FALSE);
    return NOERROR;
}

ECode AdapterViewFlipper::UpdateRunning()
{
    // ==================before translated======================
    // // by default when we update running, we want the
    // // current view to animate in
    // updateRunning(true);

    UpdateRunning(TRUE);
    return NOERROR;
}

ECode AdapterViewFlipper::UpdateRunning(
    /* [in] */ Boolean flipNow)
{
    // ==================before translated======================
    // boolean running = !mAdvancedByHost && mVisible && mStarted && mUserPresent
    //         && mAdapter != null;
    // if (running != mRunning) {
    //     if (running) {
    //         showOnly(mWhichChild, flipNow);
    //         Message msg = mHandler.obtainMessage(FLIP_MSG);
    //         mHandler.sendMessageDelayed(msg, mFlipInterval);
    //     } else {
    //         mHandler.removeMessages(FLIP_MSG);
    //     }
    //     mRunning = running;
    // }
    // if (LOGD) {
    //     Log.d(TAG, "updateRunning() mVisible=" + mVisible + ", mStarted=" + mStarted
    //             + ", mUserPresent=" + mUserPresent + ", mRunning=" + mRunning);
    // }

    Boolean running = !mAdvancedByHost && mVisible && mStarted && mUserPresent && mAdapter != NULL;
    if (running != mRunning) {
        if (running) {
            ShowOnly(mWhichChild, flipNow);
            AutoPtr<IMessage> msg;
            mHandler->ObtainMessage(FLIP_MSG, (IMessage**)&msg);
            Boolean resTmp = FALSE;
            mHandler->SendMessageDelayed(msg, mFlipInterval, &resTmp);
        }
        else {
            mHandler->RemoveMessages(FLIP_MSG);
        }
        mRunning = running;
    }
    if (LOGD) {
        String sVisible = StringUtils::BooleanToString(mVisible);
        String sStarted = StringUtils::BooleanToString(mStarted);
        String sUserPresent = StringUtils::BooleanToString(mUserPresent);
        String sRunning = StringUtils::BooleanToString(mRunning);
        Logger::D(TAG, String("updateRunning() mVisible=") + sVisible + String(", mStarted=") + sStarted
                + String(", mUserPresent=") + sUserPresent + String(", mRunning=") + sRunning);
    }
    return NOERROR;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos


