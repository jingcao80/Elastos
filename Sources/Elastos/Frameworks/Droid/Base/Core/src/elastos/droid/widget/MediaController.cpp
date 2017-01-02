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

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Media.h"
#include "elastos/droid/widget/MediaController.h"
#include "elastos/droid/widget/CFrameLayoutLayoutParams.h"
#include "elastos/droid/view/CWindowManagerLayoutParams.h"
//#include "elastos/droid/impl/CPolicyManager.h"
#include "elastos/droid/R.h"

#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Internal::Policy::IPolicyManager;
//using Elastos::Droid::Internal::Policy::CPolicyManager;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::EIID_IViewOnTouchListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnLayoutChangeListener;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::CWindowManagerLayoutParams;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;

using Elastos::Core::CString;
using Elastos::Core::StringBuilder;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Widget {

const Int32 MediaController::sDefaultTimeout = 3000;
const Int32 MediaController::FADE_OUT = 1;
const Int32 MediaController::SHOW_PROGRESS = 2;
const String MediaController::MEDIACONTROLLER_NAME("MediaController");

//==============================================================================
//            MediaController::MyHandler
//==============================================================================
ECode MediaController::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what = 0;
    msg->GetWhat(&what);

    switch(what) {
        case MediaController::FADE_OUT:
            mHost->Hide();
            break;
        case MediaController::SHOW_PROGRESS: {
            mHost->HandleShowProgress();
            break;
        }
    }

    return NOERROR;
}

//==============================================================================================
//          MediaController::MediaControllerOnTouchListener
//==============================================================================================
CAR_INTERFACE_IMPL(MediaController::MediaControllerOnTouchListener, Object, IViewOnTouchListener)

MediaController::MediaControllerOnTouchListener::MediaControllerOnTouchListener(
    /* [in] */ MediaController* host)
    : mHost(host)
{
}

ECode MediaController::MediaControllerOnTouchListener::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    Int32 action = 0;
    event->GetAction(&action);
    if(action == IMotionEvent::ACTION_DOWN) {
        if(mHost->mShowing) {
            mHost->Hide();
        }
    }
    *res = FALSE;
    return NOERROR;
}

//==============================================================================================
//          MediaController::MediaControllerOnLayoutChangeListener
//==============================================================================================
CAR_INTERFACE_IMPL(MediaController::MediaControllerOnLayoutChangeListener, Object, IViewOnLayoutChangeListener)

MediaController::MediaControllerOnLayoutChangeListener::MediaControllerOnLayoutChangeListener(
    /* [in] */ MediaController* host)
    : mHost(host)
{
}

ECode MediaController::MediaControllerOnLayoutChangeListener::OnLayoutChange(
    /* [in] */ IView* v,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom,
    /* [in] */ Int32 oldLeft,
    /* [in] */ Int32 oldTop,
    /* [in] */ Int32 oldRight,
    /* [in] */ Int32 oldBottom)
{
    mHost->UpdateFloatingWindowLayout();
    if(mHost->mShowing) {
        IViewManager::Probe(mHost->mWindowManager)->UpdateViewLayout(mHost->mDecor, IViewGroupLayoutParams::Probe(mHost->mDecorLayoutParams));
    }
    return NOERROR;
}

//==============================================================================================
//          MediaController::PauseOnClickListener
//==============================================================================================
CAR_INTERFACE_IMPL(MediaController::PauseOnClickListener, Object, IViewOnClickListener)

MediaController::PauseOnClickListener::PauseOnClickListener(
    /* [in] */ MediaController* host)
    : mHost(host)
{
}

ECode MediaController::PauseOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    mHost->DoPauseResume();
    mHost->Show(mHost->sDefaultTimeout);
    return NOERROR;
}

//==============================================================================================
//          MediaController::FfwdOnClickListener
//==============================================================================================
CAR_INTERFACE_IMPL(MediaController::FfwdOnClickListener, Object, IViewOnClickListener)

MediaController::FfwdOnClickListener::FfwdOnClickListener(
    /* [in] */ MediaController* host)
    : mHost(host)
{
}

ECode MediaController::FfwdOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    Int32 pos = 0;
    mHost->mPlayer->GetCurrentPosition(&pos);
    pos += 15000; // milliseconds
    mHost->mPlayer->SeekTo(pos);
    mHost->SetProgress();

    mHost->Show(mHost->sDefaultTimeout);
    return NOERROR;
}

//==============================================================================================
//          MediaController::RewOnClickListener
//==============================================================================================
CAR_INTERFACE_IMPL(MediaController::RewOnClickListener, Object, IViewOnClickListener)

MediaController::RewOnClickListener::RewOnClickListener(
    /* [in] */ MediaController* host)
    : mHost(host)
{
}

ECode MediaController::RewOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    Int32 pos = 0;
    mHost->mPlayer->GetCurrentPosition(&pos);
    pos -= 5000; // milliseconds
    mHost->mPlayer->SeekTo(pos);
    mHost->SetProgress();

    mHost->Show(mHost->sDefaultTimeout);
    return NOERROR;
}

//==============================================================================================
//          MediaController::MediaControllerOnSeekBarChangeListener
//==============================================================================================
CAR_INTERFACE_IMPL(MediaController::MediaControllerOnSeekBarChangeListener, Object, ISeekBarOnSeekBarChangeListener)

MediaController::MediaControllerOnSeekBarChangeListener::MediaControllerOnSeekBarChangeListener(
    /* [in] */ MediaController* host)
    : mHost(host)
{
}

ECode MediaController::MediaControllerOnSeekBarChangeListener::OnStartTrackingTouch(
    /* [in] */ ISeekBar* seekBar)
{
    mHost->Show(3600000);
    mHost->mDragging = TRUE;

    // By removing these pending progress messages we make sure
    // that a) we won't update the progress while the user adjusts
    // the seekbar and b) once the user is done dragging the thumb
    // we will post one of these messages to the queue again and
    // this ensures that there will be exactly one message queued up.
    mHost->mHandler->RemoveMessages(MediaController::SHOW_PROGRESS);
    return NOERROR;
}

ECode MediaController::MediaControllerOnSeekBarChangeListener::OnProgressChanged(
    /* [in] */ ISeekBar* seekBar,
    /* [in] */ Int32 progress,
    /* [in] */ Boolean fromUser)
{
    if(!fromUser) {
        return NOERROR;
    }
    Int32 duration = 0;
    mHost->mPlayer->GetDuration(&duration);
    Int64 newPosition = (duration * progress) / 1000L;
    mHost->mPlayer->SeekTo((Int32)newPosition);
    if(mHost->mCurrentTime != NULL) {
        AutoPtr<ICharSequence> csq;
        CString::New(mHost->StringForTime(Int32(newPosition)), (ICharSequence**)&csq);
        mHost->mCurrentTime->SetText(csq);
    }
    return NOERROR;
}

ECode MediaController::MediaControllerOnSeekBarChangeListener::OnStopTrackingTouch(
    /* [in] */ ISeekBar* seekBar)
{
    mHost->mDragging = FALSE;
    mHost->SetProgress();
    mHost->UpdatePausePlay();
    mHost->Show(sDefaultTimeout);

    // Ensure that progress is properly updated in the future,
    // the call to show() does not guarantee this because it is a
    // no-op if we are already showing.
    Boolean result = FALSE;
    mHost->mHandler->SendEmptyMessage(MediaController::SHOW_PROGRESS, &result);
    return NOERROR;
}

//==============================================================================
//            MediaController::
//==============================================================================
CAR_INTERFACE_IMPL(MediaController, FrameLayout, IMediaController)

MediaController::MediaController()
    : mShowing(FALSE)
    , mDragging(FALSE)
    , mUseFastForward(FALSE)
    , mFromXml(FALSE)
    , mListenersSet(FALSE)
{
    mLayoutChangeListener = new MediaControllerOnLayoutChangeListener(this);
    mTouchListener = new MediaControllerOnTouchListener(this);
    mHandler = new MyHandler(this);
    mPauseListener = new PauseOnClickListener(this);
    mSeekListener = new MediaControllerOnSeekBarChangeListener(this);
    mRewListener = new RewOnClickListener(this);
    mFfwdListener = new FfwdOnClickListener(this);
}

MediaController::~MediaController()
{
}

ECode MediaController::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    FrameLayout::constructor(context, attrs);
    mRoot = this;
    mContext = context;
    mUseFastForward = TRUE;
    mFromXml = TRUE;
    return NOERROR;
}

ECode MediaController::OnFinishInflate()
{
    if (mRoot != NULL) {
        InitControllerView(mRoot);
    }
    return NOERROR;
}

ECode MediaController::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Boolean useFastForWard)
{
    FrameLayout::constructor(context);
    mContext = context;
    mUseFastForward = useFastForWard;
    InitFloatingWindowLayout();
    InitFloatingWindow();
    return NOERROR;
}

ECode MediaController::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, TRUE);
}

void MediaController::InitFloatingWindow()
{
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&obj);
    mWindowManager = IWindowManager::Probe(obj);
    AutoPtr<IPolicyManager> policy;
    assert(0 && "TODO");
//    CPolicyManager::AcquireSingleton((IPolicyManager**)&policy);
    policy->MakeNewWindow(mContext, (IWindow**)&mWindow);
    mWindow->SetWindowManager(mWindowManager, NULL, String(NULL));
    Boolean result = FALSE;
    mWindow->RequestFeature(IWindow::FEATURE_NO_TITLE, &result);
    mWindow->GetDecorView((IView**)&mDecor);
    mDecor->SetOnTouchListener(mTouchListener);
    mWindow->SetContentView(this);
    mWindow->SetBackgroundDrawableResource(R::color::transparent);

    // While the media controller is up, the volume control keys should
    // affect the media stream type
    mWindow->SetVolumeControlStream(IAudioManager::STREAM_MUSIC);

    SetFocusable(TRUE);
    SetFocusableInTouchMode(TRUE);
    SetDescendantFocusability(IViewGroup::FOCUS_AFTER_DESCENDANTS);
    Boolean b = FALSE;
    View::RequestFocus(&b);
}

void MediaController::InitFloatingWindowLayout()
{
    CWindowManagerLayoutParams::New((
        IWindowManagerLayoutParams**)&mDecorLayoutParams);
    CWindowManagerLayoutParams* cp = (CWindowManagerLayoutParams*)mDecorLayoutParams.Get();
    cp->mGravity = IGravity::TOP | IGravity::LEFT;
    cp->mHeight = IViewGroupLayoutParams::WRAP_CONTENT;
    cp->SetX(0);
    cp->SetFormat(IPixelFormat::TRANSLUCENT);
    cp->mType = IWindowManagerLayoutParams::TYPE_APPLICATION_PANEL;
    cp->mFlags |= IWindowManagerLayoutParams::FLAG_ALT_FOCUSABLE_IM
        | IWindowManagerLayoutParams::FLAG_NOT_TOUCH_MODAL
        | IWindowManagerLayoutParams::FLAG_SPLIT_TOUCH;
    cp->mToken = NULL;
    cp->SetWindowAnimations(0); // android.R.style.DropDownAnimationDown;
}

void MediaController::UpdateFloatingWindowLayout()
{
    AutoPtr<ArrayOf<Int32> > anchorPos = ArrayOf<Int32>::Alloc(2);
    mAnchor->GetLocationOnScreen(anchorPos);

    // we need to know the size of the controller so we can properly position it
    // within its space
    Int32 w = 0, h = 0, mesH = 0;
    mAnchor->GetWidth(&w);
    mAnchor->GetHeight(&h);
    mDecor->GetMeasuredHeight(&mesH);

    mDecor->Measure(MeasureSpec::MakeMeasureSpec(w, MeasureSpec::AT_MOST),
                MeasureSpec::MakeMeasureSpec(h, MeasureSpec::AT_MOST));

    CWindowManagerLayoutParams* cp = (CWindowManagerLayoutParams*)mDecorLayoutParams.Get();
    cp->mWidth = w;
    cp->SetX((*anchorPos)[0] + (w - cp->mWidth) / 2);
    cp->SetY((*anchorPos)[1] + h - mesH);
}

ECode MediaController::SetMediaPlayer(
        /* [in] */ IMediaPlayerControl* player)
{
    mPlayer = player;
    UpdatePausePlay();
    return NOERROR;
}

ECode MediaController::SetAnchorView(
        /* [in] */ IView* view)
{
    AutoPtr<MediaControllerOnLayoutChangeListener> mLayoutChangeListener = new MediaControllerOnLayoutChangeListener(this);
    if(mAnchor != NULL) {
        mAnchor->RemoveOnLayoutChangeListener(mLayoutChangeListener);
    }
    mAnchor = view;
    if(mAnchor != NULL) {
        mAnchor->AddOnLayoutChangeListener(mLayoutChangeListener);
    }
    AutoPtr<IFrameLayoutLayoutParams> frameParams;
    FAIL_RETURN(CFrameLayoutLayoutParams::New(IViewGroupLayoutParams::MATCH_PARENT,
         IViewGroupLayoutParams::MATCH_PARENT, (IFrameLayoutLayoutParams**)&frameParams));

    RemoveAllViews();
    AutoPtr<IView> v = MakeControllerView();
    AddView(v, IViewGroupLayoutParams::Probe(frameParams));
    return NOERROR;
}

AutoPtr<IView> MediaController::MakeControllerView()
{
    mRoot = NULL;
    AutoPtr<IInterface> p;
    mContext->GetSystemService(
            IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&p);
    AutoPtr<ILayoutInflater> inflate = ILayoutInflater::Probe(p);
    inflate->Inflate(R::layout::media_controller, NULL, (IView**)&mRoot);

    InitControllerView(mRoot);

    return mRoot;
}

void MediaController::InitControllerView(
    /* [in] */ IView* view)
{
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    res->GetText(R::string::lockscreen_transport_play_description, (ICharSequence**)&mPlayDescription);
    res->GetText(R::string::lockscreen_transport_pause_description, (ICharSequence**)&mPauseDescription);
    AutoPtr<IView> _view;
    view->FindViewById(R::id::pause, (IView**)&_view);
    mPauseButton = IImageButton::Probe(_view);
    if (mPauseButton != NULL) {
        Boolean result = FALSE;
        IView::Probe(mPauseButton)->RequestFocus(&result);
        AutoPtr<PauseOnClickListener> mPauseListener = new PauseOnClickListener(this);
        IView::Probe(mPauseButton)->SetOnClickListener(mPauseListener);
    }

    _view = NULL;
    view->FindViewById(R::id::ffwd, (IView**)&_view);
    mFfwdButton = IImageButton::Probe(_view);
    if (mFfwdButton != NULL) {
        AutoPtr<FfwdOnClickListener> mFfwdListener = new FfwdOnClickListener(this);
        IView::Probe(mFfwdButton)->SetOnClickListener(mFfwdListener);
        if (!mFromXml) {
            IView::Probe(mFfwdButton)->SetVisibility(mUseFastForward ? IView::VISIBLE : IView::GONE);
        }
    }

    _view = NULL;
    view->FindViewById(R::id::rew, (IView**)&_view);
    mRewButton = IImageButton::Probe(_view);
    if (mRewButton != NULL) {
        AutoPtr<RewOnClickListener> mRewListener = new RewOnClickListener(this);
        IView::Probe(mRewButton)->SetOnClickListener(mRewListener);
        if (!mFromXml) {
            IView::Probe(mRewButton)->SetVisibility(mUseFastForward ? IView::VISIBLE : IView::GONE);
        }
    }

    // By default these are hidden. They will be enabled when setPrevNextListeners() is called
    _view = NULL;
    view->FindViewById(R::id::next, (IView**)&_view);
    mNextButton = IImageButton::Probe(_view);
    if (mNextButton != NULL && !mFromXml && !mListenersSet) {
        IView::Probe(mNextButton)->SetVisibility(IView::GONE);
    }

    _view = NULL;
    view->FindViewById(R::id::prev, (IView**)&_view);
    mPrevButton = IImageButton::Probe(_view);
    if (mPrevButton != NULL && !mFromXml && !mListenersSet) {
        IView::Probe(mPrevButton)->SetVisibility(IView::GONE);
    }

    AutoPtr<IView> p;
    view->FindViewById(R::id::mediacontroller_progress, (IView**)&p);
    mProgress = IProgressBar::Probe(p);
    if (mProgress != NULL) {
        if (ISeekBar::Probe(mProgress) != NULL) {
            AutoPtr<ISeekBar> seeker = ISeekBar::Probe(mProgress);
            AutoPtr<MediaControllerOnSeekBarChangeListener> mSeekListener = new MediaControllerOnSeekBarChangeListener(this);
            seeker->SetOnSeekBarChangeListener(mSeekListener);
        }
        mProgress->SetMax(1000);
    }

    AutoPtr<IView> t, ct;
    view->FindViewById(R::id::time, (IView**)&t);
    mEndTime = ITextView::Probe(t);
    view->FindViewById(R::id::time_current, (IView**)&ct);
    mCurrentTime = ITextView::Probe(ct);
    mFormatBuilder = new StringBuilder();
    assert(0 && "TODO");
    //mFormatter = new Formatter(mFormatBuilder, Local.getDefault());

    InstallPrevNextListeners();
}

ECode MediaController::Show()
{
    return Show(sDefaultTimeout);
}

void MediaController::DisableUnsupportedButtons()
{
    Boolean canPause = FALSE;
    mPlayer->CanPause(&canPause);
    if(mPauseButton != NULL && !canPause) {
        IView::Probe(mPauseButton)->SetEnabled(FALSE);
    }
    Boolean canSeekBackward = FALSE;
    mPlayer->CanSeekBackward(&canSeekBackward);
    if(mRewButton != NULL && !canSeekBackward) {
        IView::Probe(mRewButton)->SetEnabled(FALSE);
    }
    Boolean canSeekForward = FALSE;
    mPlayer->CanSeekForward(&canSeekForward);
    if(mFfwdButton != NULL && !canSeekForward) {
        IView::Probe(mFfwdButton)->SetEnabled(FALSE);
    }
}

ECode MediaController::Show(
    /* [in] */ Int32 timeout)
{
    if(!mShowing && mAnchor != NULL) {
        SetProgress();
        if(mPauseButton != NULL) {
            Boolean res = FALSE;
            IView::Probe(mPauseButton)->RequestFocus(&res);
        }

        DisableUnsupportedButtons();
        UpdateFloatingWindowLayout();
        IViewManager::Probe(mWindowManager)->AddView(mDecor, IViewGroupLayoutParams::Probe(mDecorLayoutParams));
        mShowing = TRUE;
    }
    UpdatePausePlay();

    // cause the progress bar to be updated even if mShowing
    // was already true.  This happens, for example, if we're
    // paused with the progress bar showing the user hits play.
    Boolean result = FALSE;
    mHandler->SendEmptyMessage(SHOW_PROGRESS, &result);

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(FADE_OUT, (IMessage**)&msg);
    if (timeout != 0) {
        mHandler->RemoveMessages(FADE_OUT);
        mHandler->SendEmptyMessageDelayed(FADE_OUT, timeout, &result);
    }

    return NOERROR;
}

ECode MediaController::IsShowing(
    /* [out] */ Boolean* isShowing)
{
    VALIDATE_NOT_NULL(isShowing)
    *isShowing = mShowing;
    return NOERROR;
}

ECode MediaController::Hide()
{
    if (mAnchor == NULL) {
        return NOERROR;
    }

    if (mShowing) {
        //try {
        mHandler->RemoveMessages(SHOW_PROGRESS);
        IViewManager::Probe(mWindowManager)->RemoveView(mDecor);
        //} catch (IllegalArgumentException ex) {
        //    Log.w("MediaController", "already removed");
        //}
        mShowing = FALSE;
    }

    return NOERROR;
}

String MediaController::StringForTime(
    /* [in] */ Int32 timeMs)
{
    Int32 totalSeconds = timeMs / 1000;
    Int32 seconds = totalSeconds % 60;
    Int32 minutes = (totalSeconds / 60) % 60;
    Int32 hours = totalSeconds / 3600;
    AutoPtr<IInteger32> h;
    CInteger32::New(hours, (IInteger32**)&h);
    AutoPtr<IInteger32> m;
    CInteger32::New(minutes, (IInteger32**)&m);
    AutoPtr<IInteger32> s;
    CInteger32::New(seconds, (IInteger32**)&s);

    mFormatBuilder->SetLength(0);
    if (hours > 0) {
        AutoPtr<ArrayOf<IInterface*> > arr = ArrayOf<IInterface*>::Alloc(3);
        (*arr)[0] = h;
        (*arr)[1] = m;
        (*arr)[2] = s;
        mFormatter->Format(String("%d:%02d:%02d"), arr);
        String str;
        mFormatter->ToString(&str);
        return str;
    }
    else {
        AutoPtr<ArrayOf<IInterface*> > arr = ArrayOf<IInterface*>::Alloc(2);
        (*arr)[0] = m;
        (*arr)[1] = s;
        mFormatter->Format(String("%02d:%02d"), arr);
        String str;
        mFormatter->ToString(&str);
        return str;
    }
}

Int32 MediaController::SetProgress()
{
    if (mPlayer == NULL || mDragging) {
        return 0;
    }
    Int32 position = 0, duration = 0;
    mPlayer->GetCurrentPosition(&position);
    mPlayer->GetDuration(&duration);
    if (mProgress != NULL) {
        if (duration > 0) {
            // use long to avoid overflow
            Int64 pos = 1000LL * position / duration;
            mProgress->SetProgress((Int32)pos);
        }
        Int32 percent = 0;
        mPlayer->GetBufferPercentage(&percent);
        mProgress->SetSecondaryProgress(percent * 10);
    }

    if (mEndTime != NULL) {
        AutoPtr<ICharSequence> charS;
        ASSERT_SUCCEEDED(CString::New(
                StringForTime(duration), (ICharSequence**)&charS));
        mEndTime->SetText(charS);
    }
    if (mCurrentTime != NULL) {
        AutoPtr<ICharSequence> charS;
        ASSERT_SUCCEEDED(CString::New(
                StringForTime(position), (ICharSequence**)&charS));
        mCurrentTime->SetText(charS);
    }

    return position;
}

ECode MediaController::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 act = 0;
    event->GetAction(&act);
    switch (act) {
        case IMotionEvent::ACTION_DOWN:
            Show(0); // show until hide is called
            break;
        case IMotionEvent::ACTION_UP:
            Show(sDefaultTimeout); // start timeout
            break;
        case IMotionEvent::ACTION_CANCEL:
            Hide();
            break;
        default:
            break;
    }
    *result = TRUE;
    return NOERROR;
}

ECode MediaController::OnTrackballEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Show(sDefaultTimeout);
    *result = FALSE;
    return NOERROR;
}

ECode MediaController::DispatchKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 keyCode = 0;
    event->GetKeyCode(&keyCode);
    Int32 repeatCount = 0;
    event->GetRepeatCount(&repeatCount);
    Int32 action = 0;
    event->GetAction(&action);
    Boolean uniqueDown = repeatCount == 0 && action == IKeyEvent::ACTION_DOWN;

    Boolean isPlaying = FALSE;
    mPlayer->IsPlaying(&isPlaying);

    if(keyCode == IKeyEvent::KEYCODE_HEADSETHOOK
        || keyCode == IKeyEvent::KEYCODE_MEDIA_PLAY_PAUSE
        || keyCode == IKeyEvent::KEYCODE_SPACE) {
        if(uniqueDown) {
            DoPauseResume();
            Show(sDefaultTimeout);
            if(mPauseButton != NULL) {
                Boolean res = FALSE;
                IView::Probe(mPauseButton)->RequestFocus(&res);
            }
        }
        *result = TRUE;
        return NOERROR;
    }
    else if (keyCode == IKeyEvent::KEYCODE_MEDIA_PLAY_PAUSE) {
        if(uniqueDown && !isPlaying) {
            mPlayer->Start();
            UpdatePausePlay();
            Show(sDefaultTimeout);
        }
        *result = TRUE;
        return NOERROR;
    }
    else if (keyCode == IKeyEvent::KEYCODE_MEDIA_STOP
        || keyCode == IKeyEvent::KEYCODE_MEDIA_PAUSE) {
        if(uniqueDown && !isPlaying) {
            mPlayer->Pause();
            UpdatePausePlay();
            Show(sDefaultTimeout);
        }
        *result = TRUE;
        return NOERROR;
    }
    else if (keyCode == IKeyEvent::KEYCODE_VOLUME_DOWN
        || keyCode == IKeyEvent::KEYCODE_VOLUME_UP
        || keyCode == IKeyEvent::KEYCODE_VOLUME_MUTE
        || keyCode == IKeyEvent::KEYCODE_CAMERA) {
        return FrameLayout::DispatchKeyEvent(event, result);
    }
    else if (keyCode == IKeyEvent::KEYCODE_BACK
        || keyCode == IKeyEvent::KEYCODE_MENU) {
        if(uniqueDown) {
            Hide();
        }
        *result = TRUE;
        return NOERROR;
    }
    Show(sDefaultTimeout);
    return FrameLayout::DispatchKeyEvent(event, result);
}

void MediaController::UpdatePausePlay()
{
    if(mRoot == NULL || mPauseButton == NULL) {
        return;
    }

    Boolean isPlaying = FALSE;
    mPlayer->IsPlaying(&isPlaying);
    if(isPlaying) {
        IImageView::Probe(mPauseButton)->SetImageResource(R::drawable::ic_media_pause);
        IView::Probe(mPauseButton)->SetContentDescription(mPauseDescription);
    }
    else {
        IImageView::Probe(mPauseButton)->SetImageResource(R::drawable::ic_media_play);
        IView::Probe(mPauseButton)->SetContentDescription(mPlayDescription);
    }
}

void MediaController::DoPauseResume()
{
    Boolean isPlaying = FALSE;
    mPlayer->IsPlaying(&isPlaying);
    if (isPlaying) {
        mPlayer->Pause();
    }
    else {
        mPlayer->Start();
    }
    UpdatePausePlay();
}

ECode MediaController::SetEnabled(
    /* [in] */ Boolean enabled)
{
    if (mPauseButton != NULL) {
        IView::Probe(mPauseButton)->SetEnabled(enabled);
    }
    if (mFfwdButton != NULL) {
        IView::Probe(mFfwdButton)->SetEnabled(enabled);
    }
    if (mRewButton != NULL) {
        IView::Probe(mRewButton)->SetEnabled(enabled);
    }
    if (mNextButton != NULL) {
        IView::Probe(mNextButton)->SetEnabled(enabled && mNextListener != NULL);
    }
    if (mPrevButton != NULL) {
        IView::Probe(mPrevButton)->SetEnabled(enabled && mPrevListener != NULL);
    }
    if (mProgress != NULL) {
        IView::Probe(mProgress)->SetEnabled(enabled);
    }
    DisableUnsupportedButtons();
    return FrameLayout::SetEnabled(enabled);
}

ECode MediaController::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    FrameLayout::OnInitializeAccessibilityEvent(event);
    AutoPtr<ICharSequence> seq;
    FAIL_RETURN(CString::New(MEDIACONTROLLER_NAME, (ICharSequence**)&seq));
    IAccessibilityRecord::Probe(event)->SetClassName(seq);
    return NOERROR;
}

ECode MediaController::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    FrameLayout::OnInitializeAccessibilityNodeInfo(info);
    AutoPtr<ICharSequence> seq;
    FAIL_RETURN(CString::New(MEDIACONTROLLER_NAME, (ICharSequence**)&seq));
    info->SetClassName(seq);
    return NOERROR;
}

void MediaController::InstallPrevNextListeners()
{
    if (mNextButton != NULL) {
        IView::Probe(mNextButton)->SetOnClickListener(mNextListener);
        IView::Probe(mNextButton)->SetEnabled(mNextListener != NULL);
    }

    if (mPrevButton != NULL) {
        IView::Probe(mPrevButton)->SetOnClickListener(mPrevListener);
        IView::Probe(mPrevButton)->SetEnabled(mPrevListener != NULL);
    }
}

ECode MediaController::SetPrevNextListeners(
    /* [in] */ IViewOnClickListener* next,
    /* [in] */ IViewOnClickListener* prev)
{
    mNextListener = next;
    mPrevListener = prev;
    mListenersSet = TRUE;

    if (mRoot != NULL) {
        InstallPrevNextListeners();

        if (mNextButton != NULL && !mFromXml) {
            IView::Probe(mNextButton)->SetVisibility(IView::VISIBLE);
        }
        if (mPrevButton != NULL && !mFromXml) {
            IView::Probe(mPrevButton)->SetVisibility(IView::VISIBLE);
        }
    }

    return NOERROR;
}

ECode MediaController::HandleShowProgress()
{
    Int32 pos = SetProgress();
    Boolean isPlaying = FALSE;
    if (!mDragging && mShowing && (mPlayer->IsPlaying(&isPlaying), isPlaying)) {
        Boolean result = FALSE;
        mHandler->SendEmptyMessageDelayed(SHOW_PROGRESS, 1000 - (pos % 1000), &result);
    }
    return NOERROR;
}

}// namespace Widget
}// namespace Droid
}// namespace Elastos
