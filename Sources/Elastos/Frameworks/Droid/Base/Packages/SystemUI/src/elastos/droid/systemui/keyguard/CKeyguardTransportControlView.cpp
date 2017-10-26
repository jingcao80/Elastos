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

#include "elastos/droid/systemui/keyguard/CKeyguardTransportControlView.h"
#include "elastos/droid/systemui/keyguard/KeyguardUpdateMonitor.h"
#include "elastos/droid/text/format/DateFormat.h"
#include "elastos/droid/text/TextUtils.h"
#include "Elastos.Droid.Transition.h"
#include "Elastos.Droid.Utility.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/Math.h>
#include "R.h"

using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Graphics::IXfermode;
using Elastos::Droid::Graphics::IColorFilter;
using Elastos::Droid::Graphics::IColorMatrix;
using Elastos::Droid::Graphics::CColorMatrix;
using Elastos::Droid::Graphics::IPorterDuffXfermode;
using Elastos::Droid::Graphics::CPorterDuffXfermode;
using Elastos::Droid::Graphics::PorterDuffMode_SCREEN;
using Elastos::Droid::Graphics::IColorMatrixColorFilter;
using Elastos::Droid::Graphics::CColorMatrixColorFilter;
using Elastos::Droid::Media::CMetadata;
using Elastos::Droid::Media::CRemoteController;
using Elastos::Droid::Media::CAudioManager;
using Elastos::Droid::Media::IMediaMetadataEditor;
using Elastos::Droid::Media::IRemoteControlClient;
using Elastos::Droid::Media::IMediaMetadataRetriever;
using Elastos::Droid::Media::EIID_IRemoteControllerOnClientUpdateListener;
using Elastos::Droid::Transition::IFade;
using Elastos::Droid::Transition::CFade;
using Elastos::Droid::Transition::IFadeHelper;
using Elastos::Droid::Transition::CFadeHelper;
using Elastos::Droid::Transition::ITransition;
using Elastos::Droid::Transition::IChangeText;
using Elastos::Droid::Transition::CChangeText;
using Elastos::Droid::Transition::CTransitionSet;
using Elastos::Droid::Transition::IChangeBounds;
using Elastos::Droid::Transition::CChangeBounds;
using Elastos::Droid::Transition::ITransitionManagerHelper;
using Elastos::Droid::Transition::CTransitionManagerHelper;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::Format::DateFormat;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::CKeyEvent;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnLongClickListener;
using Elastos::Droid::Widget::IProgressBar;
using Elastos::Droid::Widget::EIID_ISeekBarOnSeekBarChangeListener;
using Elastos::Core::CoreUtils;
using Elastos::Core::Math;
using Elastos::Core::StringBuilder;
using Elastos::Utility::CDate;
using Elastos::Utility::ITimeZone;
using Elastos::Utility::ITimeZoneHelper;
using Elastos::Utility::CTimeZoneHelper;
using Elastos::Text::CSimpleDateFormat;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CAR_INTERFACE_IMPL(CKeyguardTransportControlView::MyRemoteControllerOnClientUpdateListener,
        Object, IRemoteControllerOnClientUpdateListener)

ECode CKeyguardTransportControlView::MyRemoteControllerOnClientUpdateListener::OnClientChange(
    /* [in] */ Boolean clearing)
{
    if (clearing) {
        mHost->ClearMetadata();
    }
    return NOERROR;
}

ECode CKeyguardTransportControlView::MyRemoteControllerOnClientUpdateListener::OnClientPlaybackStateUpdate(
    /* [in] */ Int32 state)
{
    mHost->UpdatePlayPauseState(state);
    return NOERROR;
}

ECode CKeyguardTransportControlView::MyRemoteControllerOnClientUpdateListener::OnClientPlaybackStateUpdate(
    /* [in] */ Int32 state,
    /* [in] */ Int64 stateChangeTimeMs,
    /* [in] */ Int64 currentPosMs,
    /* [in] */ Float speed)
{
    mHost->UpdatePlayPauseState(state);
    if (DEBUG) Logger::D(TAG, "onClientPlaybackStateUpdate(state=%d, stateChangeTimeMs=%lld, currentPosMs=%lld, "
            "speed=%f)", state, stateChangeTimeMs, currentPosMs, speed);

    Boolean res;
    mHost->RemoveCallbacks(mHost->mUpdateSeekBars, &res);
    // Since the music client may be responding to historical events that cause the
    // playback state to change dramatically, wait until things become quiescent before
    // resuming automatic scrub position update.
    Int32 visibility;
    mHost->mTransientSeek->GetVisibility(&visibility);
    if (visibility == IView::VISIBLE
            && PlaybackPositionShouldMove(mHost->mCurrentPlayState)) {
        mHost->PostDelayed(mHost->mUpdateSeekBars, QUIESCENT_PLAYBACK_FACTOR, &res);
    }
    return NOERROR;
}

ECode CKeyguardTransportControlView::MyRemoteControllerOnClientUpdateListener::OnClientTransportControlUpdate(
    /* [in] */ Int32 transportControlFlags)
{
    mHost->UpdateTransportControls(transportControlFlags);
    return NOERROR;
}

ECode CKeyguardTransportControlView::MyRemoteControllerOnClientUpdateListener::OnClientMetadataUpdate(
    /* [in] */ IRemoteControllerMetadataEditor* metadataEditor)
{
    return mHost->UpdateMetadata(metadataEditor);
}

ECode CKeyguardTransportControlView::MyRemoteControllerOnClientUpdateListener::OnClientFolderInfoBrowsedPlayer(
    /* [in] */ const String& stringUri)
{
    return NOERROR;
}

ECode CKeyguardTransportControlView::MyRemoteControllerOnClientUpdateListener::OnClientUpdateNowPlayingEntries(
    /* [in] */ ArrayOf<Int64>* playList)
{
    return NOERROR;
}

ECode CKeyguardTransportControlView::MyRemoteControllerOnClientUpdateListener::OnClientNowPlayingContentChange()
{
    return NOERROR;
}

ECode CKeyguardTransportControlView::MyRemoteControllerOnClientUpdateListener::OnClientPlayItemResponse(
    /* [in] */ Boolean success)
{
    return NOERROR;
}

ECode CKeyguardTransportControlView::UpdateSeekBarRunnable::Run()
{
    Boolean seekAble;
    UpdateOnce(&seekAble);
    if (seekAble) {
        Boolean res;
        mHost->RemoveCallbacks(this, &res);
        mHost->PostDelayed(this, 1000, &res);
    }
    return NOERROR;
}

ECode CKeyguardTransportControlView::UpdateSeekBarRunnable::UpdateOnce(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return mHost->UpdateSeekBars(result);
}

ECode CKeyguardTransportControlView::MyRunnable::Run()
{
    return mHost->ResetToMetadata();
}

CAR_INTERFACE_IMPL(CKeyguardTransportControlView::MyViewOnClickListener,
        Object, IViewOnClickListener)

ECode CKeyguardTransportControlView::MyViewOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    Int32 keyCode = -1;
    if (TO_IINTERFACE(v) == TO_IINTERFACE(mHost->mBtnPrev)) {
        keyCode = IKeyEvent::KEYCODE_MEDIA_PREVIOUS;
    }
    else if (TO_IINTERFACE(v) == TO_IINTERFACE(mHost->mBtnNext)) {
        keyCode = IKeyEvent::KEYCODE_MEDIA_NEXT;
    }
    else if (TO_IINTERFACE(v) == TO_IINTERFACE(mHost->mBtnPlay)) {
        keyCode = IKeyEvent::KEYCODE_MEDIA_PLAY_PAUSE;
    }
    if (keyCode != -1) {
        mHost->SendMediaButtonClick(keyCode);
        mHost->DelayResetToMetadata(); // if the scrub bar is showing, keep showing it.
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(CKeyguardTransportControlView::MyViewOnLongClickListener,
        Object, IViewOnLongClickListener)

CARAPI CKeyguardTransportControlView::MyViewOnLongClickListener::OnLongClick(
    /* [in] */ IView* v,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (mHost->mSeekEnabled) {
        return mHost->TryToggleSeekBar(result);
    }
    *result = FALSE;
    return NOERROR;
}

ECode CKeyguardTransportControlView::FutureSeekRunnable::Run()
{
    mHost->ScrubTo(mProgress);
    mPending = FALSE;
    return NOERROR;
}

ECode CKeyguardTransportControlView::FutureSeekRunnable::SetProgress(
    /* [in] */  Int32 progress)
{
    mProgress = progress;
    if (!mPending) {
        mPending = TRUE;
        Boolean res;
        mHost->PostDelayed(this, 30, &res);
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(CKeyguardTransportControlView::MySeekBarOnSeekBarChangeListener, Object,
        ISeekBarOnSeekBarChangeListener)

ECode CKeyguardTransportControlView::MySeekBarOnSeekBarChangeListener::OnProgressChanged(
    /* [in] */ ISeekBar* seekBar,
    /* [in] */ Int32 progress,
    /* [in] */ Boolean fromUser)
{
    if (fromUser) {
        ((FutureSeekRunnable*)(mHost->mFutureSeekRunnable).Get())->SetProgress(
                progress);
        mHost->DelayResetToMetadata();
        mHost->mTempDate->SetTime(progress);
        String str;
        mHost->mFormat->Format(mHost->mTempDate, &str);
        AutoPtr<ICharSequence> cchar = CoreUtils::Convert(str);
        mHost->mTransientSeekTimeElapsed->SetText(cchar);
    }
    else {
        mHost->UpdateSeekDisplay();
    }
    return NOERROR;
}

ECode CKeyguardTransportControlView::MySeekBarOnSeekBarChangeListener::OnStartTrackingTouch(
    /* [in] */ ISeekBar* seekBar)
{
    mHost->DelayResetToMetadata();
    Boolean res;
    mHost->RemoveCallbacks(mHost->mUpdateSeekBars, &res); // don't update during user interaction
    return NOERROR;
}

ECode CKeyguardTransportControlView::MySeekBarOnSeekBarChangeListener::OnStopTrackingTouch(
    /* [in] */ ISeekBar* seekBar)
{
    return NOERROR;
}

ECode CKeyguardTransportControlView::MyKeyguardUpdateMonitorCallback::OnScreenTurnedOff(
    /* [in] */ Int32 why)
{
    mHost->SetEnableMarquee(FALSE);
    return NOERROR;
}

ECode CKeyguardTransportControlView::MyKeyguardUpdateMonitorCallback::OnScreenTurnedOn()
{
    mHost->SetEnableMarquee(TRUE);
    return NOERROR;
}

ECode CKeyguardTransportControlView::Metadata::Clear()
{
    mArtist = NULL;
    mTrackTitle = NULL;
    mAlbumTitle = NULL;
    mBitmap = NULL;
    mDuration = -1;
    return NOERROR;
}

// ECode CKeyguardTransportControlView::Metadata::ToString(
//     /* [out] */ String* str)
// {
//     VALIDATE_NOT_NULL(str);

//     StringBuilder sb;
//     sb += "Metadata[artist=";
//     sb += mArtist;
//     sb += " trackTitle=";
//     sb += mTrackTitle;
//     sb += " albumTitle=";
//     sb += mAlbumTitle;
//     sb += " duration=";
//     sb += mDuration;
//     sb += "]";

//     return sb.ToString(str);
// }

CAR_INTERFACE_IMPL(CKeyguardTransportControlView::SavedState,
        View::BaseSavedState, IKeyguardTransportControlViewSavedState)

ECode CKeyguardTransportControlView::SavedState::constructor(
    /* [in] */ IParcelable* superState)
{
    return View::BaseSavedState::constructor(superState);
}

ECode CKeyguardTransportControlView::SavedState::WriteToParcel(
    /* [in] */ IParcel* out)
{
    assert(0);
    //View::BaseSavedState::WriteToParcel(out, flags);
    out->WriteInt32(mClientPresent ? 1 : 0);
    out->WriteString(mArtist);
    out->WriteString(mTrackTitle);
    out->WriteString(mAlbumTitle);
    out->WriteInt64(mDuration);
    assert(0);
    //return IParcelable::Probe(mBitmap)->WriteToParcel(out, flags);
    return NOERROR;
}

ECode CKeyguardTransportControlView::SavedState::constructor(
    /* [in] */ IParcel* in)
{
    assert(0);
    //View::BaseSavedState::constructor(in);

    Int32 num;
    in->ReadInt32(&num);
    mClientPresent = num != 0;
    in->ReadString(&mArtist);
    in->ReadString(&mTrackTitle);
    in->ReadString(&mAlbumTitle);
    in->ReadInt64(&mDuration);
    assert(0);
    //mBitmap = Bitmap.CREATOR.createFromParcel(in);
    return NOERROR;
}

const Boolean CKeyguardTransportControlView::DEBUG = IKeyguardConstants::DEBUG;
const String CKeyguardTransportControlView::TAG("TransportControlView");

const Int64 CKeyguardTransportControlView::QUIESCENT_PLAYBACK_FACTOR = 1000;

const Int32 CKeyguardTransportControlView::RESET_TO_METADATA_DELAY = 5000;

const Boolean CKeyguardTransportControlView::ANIMATE_TRANSITIONS = TRUE;

const Int32 CKeyguardTransportControlView::TRANSITION_DURATION = 200;

CAR_OBJECT_IMPL(CKeyguardTransportControlView)

CAR_INTERFACE_IMPL(CKeyguardTransportControlView,
        Elastos::Droid::Widget::FrameLayout, IKeyguardTransportControlView)

CKeyguardTransportControlView::CKeyguardTransportControlView()
    : mTransportControlFlags(0)
    , mCurrentPlayState(0)
    , mSeekEnabled(FALSE)
{
}

Boolean CKeyguardTransportControlView::PlaybackPositionShouldMove(
    /* [in] */ Int32 playstate)
{
    switch(playstate) {
        case IRemoteControlClient::PLAYSTATE_STOPPED:
        case IRemoteControlClient::PLAYSTATE_PAUSED:
        case IRemoteControlClient::PLAYSTATE_BUFFERING:
        case IRemoteControlClient::PLAYSTATE_ERROR:
        case IRemoteControlClient::PLAYSTATE_SKIPPING_FORWARDS:
        case IRemoteControlClient::PLAYSTATE_SKIPPING_BACKWARDS:
            return FALSE;
        case IRemoteControlClient::PLAYSTATE_PLAYING:
        case IRemoteControlClient::PLAYSTATE_FAST_FORWARDING:
        case IRemoteControlClient::PLAYSTATE_REWINDING:
        default:
            return TRUE;
    }
}

ECode CKeyguardTransportControlView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    FrameLayout::constructor(context, attrs);

    AutoPtr<IMetadata> md;
    CMetadata::New((IMetadata**)&md);
    mMetadata = (Metadata*)md.Get();

    CDate::New((IDate**)&mTempDate);

    mRCClientUpdateListener = new MyRemoteControllerOnClientUpdateListener(this);
    mUpdateSeekBars = new UpdateSeekBarRunnable(this);
    mResetToMetadata = new MyRunnable(this);
    mTransportCommandListener = new MyViewOnClickListener(this);
    mTransportShowSeekBarListener = new MyViewOnLongClickListener(this);
    mFutureSeekRunnable = new FutureSeekRunnable(this);
    mOnSeekBarChangeListener = new MySeekBarOnSeekBarChangeListener(this);
    mUpdateMonitor = new MyKeyguardUpdateMonitorCallback(this);

    //if (DEBUG) Logger::V(TAG, "Create TCV %s", TO_CSTR(this));
    CAudioManager::New(mContext, (IAudioManager**)&mAudioManager);
    mCurrentPlayState = IRemoteControlClient::PLAYSTATE_NONE; // until we get a callback
    CRemoteController::New(context, mRCClientUpdateListener,
            (IRemoteController**)&mRemoteController);

    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr<IDisplayMetrics> dm;
    resources->GetDisplayMetrics((IDisplayMetrics**)&dm);

    Int32 width;
    dm->GetWidthPixels(&width);
    Int32 height;
    dm->GetHeightPixels(&height);
    Int32 dim = Elastos::Core::Math::Max(width, height);
    Boolean res;
    mRemoteController->SetArtworkConfiguration(TRUE, dim, dim, &res);

    AutoPtr<IChangeText> tc;
    CChangeText::New((IChangeText**)&tc);
    tc->SetChangeBehavior(IChangeText::CHANGE_BEHAVIOR_OUT_IN);
    AutoPtr<ITransitionSet> inner;
    CTransitionSet::New((ITransitionSet**)&inner);

    AutoPtr<IChangeBounds> bounds;
    CChangeBounds::New((IChangeBounds**)&bounds);
    inner->AddTransition(ITransition::Probe(tc));
    inner->AddTransition(ITransition::Probe(bounds));

    AutoPtr<ITransitionSet> tg;
    CTransitionSet::New((ITransitionSet**)&tg);
    AutoPtr<IFadeHelper> helper;
    CFadeHelper::AcquireSingleton((IFadeHelper**)&helper);
    Int32 _out;
    helper->GetOUT(&_out);
    AutoPtr<IFade> fo;
    CFade::New(_out, (IFade**)&fo);
    tg->AddTransition(ITransition::Probe(fo));
    tg->AddTransition(ITransition::Probe(inner));
    Int32 _in;
    helper->GetIN(&_in);
    AutoPtr<IFade> fi;
    CFade::New(_in, (IFade**)&fi);
    tg->AddTransition(ITransition::Probe(fi));
    tg->SetOrdering(ITransitionSet::ORDERING_SEQUENTIAL);
    ITransition::Probe(tg)->SetDuration(TRANSITION_DURATION);
    mMetadataChangeTransition = tg;
    return NOERROR;
}

void CKeyguardTransportControlView::UpdateTransportControls(
    /* [in] */ Int32 transportControlFlags)
{
    mTransportControlFlags = transportControlFlags;
    SetSeekBarsEnabled(
            (transportControlFlags & IRemoteControlClient::FLAG_KEY_MEDIA_POSITION_UPDATE) != 0);
}

ECode CKeyguardTransportControlView::SetSeekBarsEnabled(
    /* [in] */ Boolean enabled)
{
    if (enabled == mSeekEnabled) return NOERROR;

    mSeekEnabled = enabled;
    Int32 visibility;
    mTransientSeek->GetVisibility(&visibility);
    if (visibility == VISIBLE && !enabled) {
        mTransientSeek->SetVisibility(INVISIBLE);
        IView::Probe(mMetadataContainer)->SetVisibility(VISIBLE);
        CancelResetToMetadata();
    }
    return NOERROR;
}

ECode CKeyguardTransportControlView::SetTransportControlCallback(
    /* [in] */ IKeyguardHostViewTransportControlCallback* transportControlCallback)
{
    mTransportControlCallback = transportControlCallback;
    return NOERROR;
}

void CKeyguardTransportControlView::SetEnableMarquee(
    /* [in] */ Boolean enabled)
{
    if (DEBUG) Logger::V(TAG, (enabled ? String("Enable") : String("Disable")) + String(" transport text marquee"));
    if (mTrackTitle != NULL) IView::Probe(mTrackTitle)->SetSelected(enabled);
    if (mTrackArtistAlbum != NULL) IView::Probe(mTrackTitle)->SetSelected(enabled);
}

ECode CKeyguardTransportControlView::OnFinishInflate()
{
    FrameLayout::OnFinishInflate();

    AutoPtr<IView> view1;
    FindViewById(R::id::info_container, (IView**)&view1);
    mInfoContainer = IViewGroup::Probe(view1);

    AutoPtr<IView> view2;
    FindViewById(R::id::metadata_container, (IView**)&view2);
    mMetadataContainer = IViewGroup::Probe(view2);

    AutoPtr<IView> view3;
    FindViewById(R::id::badge, (IView**)&view3);
    mBadge = IImageView::Probe(view3);

    AutoPtr<IView> view4;
    FindViewById(R::id::title, (IView**)&view4);
    mTrackTitle = ITextView::Probe(view4);

    AutoPtr<IView> view5;
    FindViewById(R::id::artist_album, (IView**)&view5);
    mTrackArtistAlbum = ITextView::Probe(view5);

    FindViewById(R::id::transient_seek, (IView**)&mTransientSeek);

    AutoPtr<IView> view6;
    FindViewById(R::id::transient_seek_bar, (IView**)&view6);
    mTransientSeekBar = ISeekBar::Probe(view6);

    mTransientSeekBar->SetOnSeekBarChangeListener(mOnSeekBarChangeListener);

    AutoPtr<IView> view7;
    FindViewById(R::id::transient_seek_time_elapsed, (IView**)&view7);
    mTransientSeekTimeElapsed = ITextView::Probe(view7);

    AutoPtr<IView> view8;
    FindViewById(R::id::transient_seek_time_remaining, (IView**)&view8);
    mTransientSeekTimeTotal = ITextView::Probe(view8);

    AutoPtr<IView> view9;
    FindViewById(R::id::btn_prev, (IView**)&view9);
    mBtnPrev = IImageView::Probe(view9);

    AutoPtr<IView> view10;
    FindViewById(R::id::btn_play, (IView**)&view10);
    mBtnPlay = IImageView::Probe(view10);

    AutoPtr<IView> view11;
    FindViewById(R::id::btn_next, (IView**)&view11);
    mBtnNext = IImageView::Probe(view11);

    // final View buttons[] = { mBtnPrev, mBtnPlay, mBtnNext };
    // for (View view : buttons) {
    //     view.setOnClickListener(mTransportCommandListener);
    //     view.setOnLongClickListener(mTransportShowSeekBarListener);
    // }
    IView::Probe(mBtnPrev)->SetOnClickListener(mTransportCommandListener);
    IView::Probe(mBtnPrev)->SetOnLongClickListener(mTransportShowSeekBarListener);

    IView::Probe(mBtnPlay)->SetOnClickListener(mTransportCommandListener);
    IView::Probe(mBtnPlay)->SetOnLongClickListener(mTransportShowSeekBarListener);

    IView::Probe(mBtnNext)->SetOnClickListener(mTransportCommandListener);
    IView::Probe(mBtnNext)->SetOnLongClickListener(mTransportShowSeekBarListener);

    Boolean screenOn;
    KeyguardUpdateMonitor::GetInstance(mContext)->IsScreenOn(&screenOn);
    SetEnableMarquee(screenOn);
    // Allow long-press anywhere else in this view to show the seek bar
    return SetOnLongClickListener(mTransportShowSeekBarListener);
}

ECode CKeyguardTransportControlView::OnAttachedToWindow()
{
    FrameLayout::OnAttachedToWindow();
    if (DEBUG) Logger::V(TAG, "onAttachToWindow()");
    if (mPopulateMetadataWhenAttached != NULL) {
        UpdateMetadata(mPopulateMetadataWhenAttached);
        mPopulateMetadataWhenAttached = NULL;
    }
    //if (DEBUG) Logger::V(TAG, "Registering TCV %s", TO_CSTR(this));
    mMetadata->Clear();
    Boolean res;
    mAudioManager->RegisterRemoteController(mRemoteController, &res);
    return KeyguardUpdateMonitor::GetInstance(mContext)->RegisterCallback(mUpdateMonitor);
}

ECode CKeyguardTransportControlView::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    FrameLayout::OnConfigurationChanged(newConfig);

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr<IDisplayMetrics> dm;
    resources->GetDisplayMetrics((IDisplayMetrics**)&dm);

    Int32 width;
    dm->GetWidthPixels(&width);
    Int32 height;
    dm->GetHeightPixels(&height);
    Int32 dim = Elastos::Core::Math::Max(width, height);
    Boolean res;
    return mRemoteController->SetArtworkConfiguration(TRUE, dim, dim, &res);
}

ECode CKeyguardTransportControlView::OnDetachedFromWindow()
{
    if (DEBUG) Logger::V(TAG, "onDetachFromWindow()");
    FrameLayout::OnDetachedFromWindow();
    //if (DEBUG) Logger::V(TAG, "Unregistering TCV %s", TO_CSTR(this));
    mAudioManager->UnregisterRemoteController(mRemoteController);
    KeyguardUpdateMonitor::GetInstance(mContext)->RemoveCallback(mUpdateMonitor);
    mMetadata->Clear();
    Boolean res;
    return RemoveCallbacks(mUpdateSeekBars, &res);
}

AutoPtr<IParcelable> CKeyguardTransportControlView::OnSaveInstanceState()
{
    AutoPtr<IParcelable> p = FrameLayout::OnSaveInstanceState();
    AutoPtr<SavedState> ss = new SavedState();
    ss->constructor(p);
    ss->mArtist = mMetadata->mArtist;
    ss->mTrackTitle = mMetadata->mTrackTitle;
    ss->mAlbumTitle = mMetadata->mAlbumTitle;
    ss->mDuration = mMetadata->mDuration;
    ss->mBitmap = mMetadata->mBitmap;
    return IParcelable::Probe(ss);
}

ECode CKeyguardTransportControlView::OnRestoreInstanceState(
    /* [in] */ IParcelable* state)
{
    if (IKeyguardTransportControlViewSavedState::Probe(state) == NULL) {
        FrameLayout::OnRestoreInstanceState(state);
        return NOERROR;
    }
    AutoPtr<SavedState> ss = (SavedState*)IKeyguardTransportControlViewSavedState::Probe(state);
    assert(0);
    //FrameLayout::OnRestoreInstanceState(ss.getSuperState());
    mMetadata->mArtist = ss->mArtist;
    mMetadata->mTrackTitle = ss->mTrackTitle;
    mMetadata->mAlbumTitle = ss->mAlbumTitle;
    mMetadata->mDuration = ss->mDuration;
    mMetadata->mBitmap = ss->mBitmap;
    PopulateMetadata();
    return NOERROR;
}

ECode CKeyguardTransportControlView::SetBadgeIcon(
    /* [in] */ IDrawable* bmp)
{
    mBadge->SetImageDrawable(bmp);

    AutoPtr<IColorMatrix> cm;
    CColorMatrix::New((IColorMatrix**)&cm);
    cm->SetSaturation(0);

    AutoPtr<IColorMatrixColorFilter> filter;
    CColorMatrixColorFilter::New(cm, (IColorMatrixColorFilter**)&filter);
    mBadge->SetColorFilter(IColorFilter::Probe(filter));
    AutoPtr<IPorterDuffXfermode> mode;
    CPorterDuffXfermode::New(PorterDuffMode_SCREEN, (IPorterDuffXfermode**)&mode);
    mBadge->SetXfermode(IXfermode::Probe(mode));
    return mBadge->SetImageAlpha(0xef);
}

ECode CKeyguardTransportControlView::ClearMetadata()
{
    mPopulateMetadataWhenAttached = NULL;
    mMetadata->Clear();
    PopulateMetadata();
    return NOERROR;
}

ECode CKeyguardTransportControlView::UpdateMetadata(
    /* [in] */ IRemoteControllerMetadataEditor* data)
{
    Boolean res;
    if (IsAttachedToWindow(&res), res) {
        IMediaMetadataEditor::Probe(data)->GetString(
                IMediaMetadataRetriever::METADATA_KEY_ALBUMARTIST,
                mMetadata->mArtist, &(mMetadata->mArtist));
        IMediaMetadataEditor::Probe(data)->GetString(
                IMediaMetadataRetriever::METADATA_KEY_TITLE,
                mMetadata->mTrackTitle, &(mMetadata->mTrackTitle));
        IMediaMetadataEditor::Probe(data)->GetString(
                IMediaMetadataRetriever::METADATA_KEY_ALBUM,
                mMetadata->mAlbumTitle, &(mMetadata->mAlbumTitle));
        IMediaMetadataEditor::Probe(data)->GetInt64(
                IMediaMetadataRetriever::METADATA_KEY_DURATION,
                -1, &(mMetadata->mDuration));
        IMediaMetadataEditor::Probe(data)->GetBitmap(
                IMediaMetadataEditor::BITMAP_KEY_ARTWORK,
                mMetadata->mBitmap, (IBitmap**)&(mMetadata->mBitmap));
        PopulateMetadata();
    }
    else {
        mPopulateMetadataWhenAttached = data;
    }
    return NOERROR;
}

void CKeyguardTransportControlView::PopulateMetadata()
{
    Int32 visibility;
    Boolean res;
    if (ANIMATE_TRANSITIONS && (IsLaidOut(&res), res) &&
            (IView::Probe(mMetadataContainer)->GetVisibility(&visibility), visibility) == VISIBLE) {
        AutoPtr<ITransitionManagerHelper> helper;
        CTransitionManagerHelper::AcquireSingleton((ITransitionManagerHelper**)&helper);
        helper->BeginDelayedTransition(mMetadataContainer,
                ITransition::Probe(mMetadataChangeTransition));
    }

    String remoteClientPackage;
    mRemoteController->GetRemoteControlClientPackageName(&remoteClientPackage);
    AutoPtr<IDrawable> badgeIcon;
    //try {
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    ECode ec = pm->GetApplicationIcon(remoteClientPackage, (IDrawable**)&badgeIcon);
    //} catch (PackageManager.NameNotFoundException e) {
    if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
        Logger::E(TAG, "Couldn't get remote control client package icon %d", ec);
    }
    SetBadgeIcon(badgeIcon);
    AutoPtr<ICharSequence> cchar;
    if (!TextUtils::IsEmpty(mMetadata->mTrackTitle)) {
        cchar = CoreUtils::Convert(mMetadata->mTrackTitle);
    }
    else {
        cchar = CoreUtils::Convert(String(NULL));
    }
    mTrackTitle->SetText(cchar);

    StringBuilder sb;
    if (!TextUtils::IsEmpty(mMetadata->mArtist)) {
        if (sb.GetLength() != 0) {
            sb += " - ";
        }
        sb += mMetadata->mArtist;
    }
    if (!TextUtils::IsEmpty(mMetadata->mAlbumTitle)) {
        if (sb.GetLength() != 0) {
            sb += " - ";
        }
        sb += mMetadata->mAlbumTitle;
    }

    String trackArtistAlbum;
    sb.ToString(&trackArtistAlbum);
    AutoPtr<ICharSequence> cchar2;
    if (!TextUtils::IsEmpty(trackArtistAlbum)) {
        cchar2 = CoreUtils::Convert(trackArtistAlbum);
    }
    else {
        cchar2 = CoreUtils::Convert(String(NULL));
    }
    mTrackArtistAlbum->SetText(cchar2);

    if (mMetadata->mDuration >= 0) {
        SetSeekBarsEnabled(TRUE);
        SetSeekBarDuration(mMetadata->mDuration);

        String skeleton;

        if (mMetadata->mDuration >= 86400000) {
            skeleton = String("DDD kk mm ss");
        }
        else if (mMetadata->mDuration >= 3600000) {
            skeleton = String("kk mm ss");
        }
        else {
            skeleton = String("mm ss");
        }

        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<IResources> resources;
        context->GetResources((IResources**)&resources);
        AutoPtr<IConfiguration> config;
        resources->GetConfiguration((IConfiguration**)&config);
        AutoPtr<ILocale> locale;
        config->GetLocale((ILocale**)&locale);
        String pattern = DateFormat::GetBestDateTimePattern(locale, skeleton);
        CSimpleDateFormat::New(pattern, (IDateFormat**)&mFormat);
        AutoPtr<ITimeZoneHelper> helper;
        CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&helper);
        AutoPtr<ITimeZone> ppTimezone;
        helper->GetTimeZone(String("GMT+0"), (ITimeZone**)&ppTimezone);
        mFormat->SetTimeZone(ppTimezone);
    }
    else {
        SetSeekBarsEnabled(FALSE);
    }

    AutoPtr<IContext> context2;
    GetContext((IContext**)&context2);
    KeyguardUpdateMonitor::GetInstance(context2)->DispatchSetBackground(mMetadata->mBitmap);
    Int32 flags = mTransportControlFlags;
    SetVisibilityBasedOnFlag(IView::Probe(mBtnPrev),
            flags, IRemoteControlClient::FLAG_KEY_MEDIA_PREVIOUS);
    SetVisibilityBasedOnFlag(IView::Probe(mBtnNext),
            flags, IRemoteControlClient::FLAG_KEY_MEDIA_NEXT);
    SetVisibilityBasedOnFlag(IView::Probe(mBtnPlay),
            flags,
            IRemoteControlClient::FLAG_KEY_MEDIA_PLAY
            | IRemoteControlClient::FLAG_KEY_MEDIA_PAUSE
            | IRemoteControlClient::FLAG_KEY_MEDIA_PLAY_PAUSE
            | IRemoteControlClient::FLAG_KEY_MEDIA_STOP);

    UpdatePlayPauseState(mCurrentPlayState);
}

ECode CKeyguardTransportControlView::UpdateSeekDisplay()
{
    if (mMetadata != NULL && mRemoteController != NULL && mFormat != NULL) {
        Int64 position;
        mRemoteController->GetEstimatedMediaPosition(&position);
        mTempDate->SetTime(position);

        String str;
        mFormat->Format(mTempDate, &str);
        AutoPtr<ICharSequence> cchar = CoreUtils::Convert(str);
        mTransientSeekTimeElapsed->SetText(cchar);
        mTempDate->SetTime(mMetadata->mDuration);
        mTransientSeekTimeTotal->SetText(cchar);

        if (DEBUG) Logger::D(TAG, "updateSeekDisplay timeElapsed=%s duration=%d",
                TO_CSTR(mTempDate), mMetadata->mDuration);
    }
    return NOERROR;
}

ECode CKeyguardTransportControlView::TryToggleSeekBar(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (ANIMATE_TRANSITIONS) {
        AutoPtr<ITransitionManagerHelper> helper;
        CTransitionManagerHelper::AcquireSingleton((ITransitionManagerHelper**)&helper);
        helper->BeginDelayedTransition(mInfoContainer);
    }

    Int32 visibility;
    mTransientSeek->GetVisibility(&visibility);
    if (visibility == VISIBLE) {
        mTransientSeek->SetVisibility(INVISIBLE);
        IView::Probe(mMetadataContainer)->SetVisibility(VISIBLE);
        CancelResetToMetadata();
        Boolean res;
        RemoveCallbacks(mUpdateSeekBars, &res); // don't update if scrubber isn't visible
    }
    else {
        mTransientSeek->SetVisibility(VISIBLE);
        IView::Probe(mMetadataContainer)->SetVisibility(INVISIBLE);
        DelayResetToMetadata();
        if (PlaybackPositionShouldMove(mCurrentPlayState)) {
            mUpdateSeekBars->Run();
        }
        else {
            assert(0);
            //mUpdateSeekBars->UpdateOnce();
        }
    }
    mTransportControlCallback->UserActivity();
    *result = TRUE;
    return NOERROR;
}

ECode CKeyguardTransportControlView::ResetToMetadata()
{
    if (ANIMATE_TRANSITIONS) {
        AutoPtr<ITransitionManagerHelper> helper;
        CTransitionManagerHelper::AcquireSingleton((ITransitionManagerHelper**)&helper);
        helper->BeginDelayedTransition(mInfoContainer);
    }
    Int32 visibility;
    mTransientSeek->GetVisibility(&visibility);
    if (visibility == VISIBLE) {
        mTransientSeek->SetVisibility(INVISIBLE);
        IView::Probe(mMetadataContainer)->SetVisibility(VISIBLE);
    }
    // TODO Also hide ratings, if applicable
    return NOERROR;
}

ECode CKeyguardTransportControlView::DelayResetToMetadata()
{
    Boolean res;
    RemoveCallbacks(mResetToMetadata, &res);
    return PostDelayed(mResetToMetadata, RESET_TO_METADATA_DELAY, &res);
}

ECode CKeyguardTransportControlView::CancelResetToMetadata()
{
    Boolean res;
    return RemoveCallbacks(mResetToMetadata, &res);
}

ECode CKeyguardTransportControlView::SetSeekBarDuration(
    /* [in] */ Int64 duration)
{
    return IProgressBar::Probe(mTransientSeekBar)->SetMax((Int32)duration);
}

ECode CKeyguardTransportControlView::ScrubTo(
    /* [in] */ Int32 progress)
{
    Boolean res;
    mRemoteController->SeekTo(progress, &res);
    return mTransportControlCallback->UserActivity();
}

void CKeyguardTransportControlView::SetVisibilityBasedOnFlag(
    /* [in] */ IView* view,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 flag)
{
    if ((flags & flag) != 0) {
        view->SetVisibility(IView::VISIBLE);
    }
    else {
        view->SetVisibility(IView::INVISIBLE);
    }
}

void CKeyguardTransportControlView::UpdatePlayPauseState(
    /* [in] */ Int32 state)
{
    if (DEBUG) Logger::V(TAG,
            "updatePlayPauseState(), old=%d, state=%d", mCurrentPlayState, state);
    if (state == mCurrentPlayState) {
        return;
    }
    Int32 imageResId;
    Int32 imageDescId;
    switch (state) {
        case IRemoteControlClient::PLAYSTATE_ERROR:
            imageResId = R::drawable::stat_sys_warning;
            // TODO use more specific image description string for warning, but here the "play"
            //      message is still valid because this button triggers a play command.
            imageDescId = R::string::keyguard_transport_play_description;
            break;

        case IRemoteControlClient::PLAYSTATE_PLAYING:
            imageResId = R::drawable::ic_media_pause;
            imageDescId = R::string::keyguard_transport_pause_description;
            break;

        case IRemoteControlClient::PLAYSTATE_BUFFERING:
            imageResId = R::drawable::ic_media_stop;
            imageDescId = R::string::keyguard_transport_stop_description;
            break;

        case IRemoteControlClient::PLAYSTATE_PAUSED:
        default:
            imageResId = R::drawable::ic_media_play;
            imageDescId = R::string::keyguard_transport_play_description;
            break;
    }

    Boolean clientSupportsSeek = mMetadata != NULL && mMetadata->mDuration > 0;
    SetSeekBarsEnabled(clientSupportsSeek);

    mBtnPlay->SetImageResource(imageResId);
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    String str;
    resources->GetString(imageDescId, &str);
    AutoPtr<ICharSequence> cchar = CoreUtils::Convert(str);
    IView::Probe(mBtnPlay)->SetContentDescription(cchar);
    mCurrentPlayState = state;
}

ECode CKeyguardTransportControlView::UpdateSeekBars(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int64 _position;
    mRemoteController->GetEstimatedMediaPosition(&_position);
    Int32 position = (Int32)_position;
    if (DEBUG) Logger::V(TAG, "Estimated time:%d", position);
    if (position >= 0) {
        IProgressBar::Probe(mTransientSeekBar)->SetProgress(position);
        *result = TRUE;
        return NOERROR;
    }
    Logger::W(TAG, "Updating seek bars; received invalid estimated media position (%d). Disabling seek."
            , position);
    SetSeekBarsEnabled(FALSE);
    *result = FALSE;
    return NOERROR;
}

void CKeyguardTransportControlView::SendMediaButtonClick(
    /* [in] */ Int32 keyCode)
{
    // TODO We should think about sending these up/down events accurately with touch up/down
    // on the buttons, but in the near term this will interfere with the long press behavior.
    AutoPtr<IKeyEvent> event1;
    CKeyEvent::New(IKeyEvent::ACTION_DOWN, keyCode, (IKeyEvent**)&event1);
    Boolean res;
    mRemoteController->SendMediaKeyEvent(event1, &res);

    AutoPtr<IKeyEvent> event2;
    CKeyEvent::New(IKeyEvent::ACTION_UP, keyCode, (IKeyEvent**)&event2);
    mRemoteController->SendMediaKeyEvent(event2, &res);

    mTransportControlCallback->UserActivity();
}

ECode CKeyguardTransportControlView::ProvidesClock(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = FALSE;
    return NOERROR;
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
