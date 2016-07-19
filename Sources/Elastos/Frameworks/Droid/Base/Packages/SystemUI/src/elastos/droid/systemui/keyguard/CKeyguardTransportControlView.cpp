
#include "elastos/droid/systemui/keyguard/CKeyguardTransportControlView.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

ECode CKeyguardTransportControlView::MyRemoteControllerOnClientUpdateListener::OnClientChange(
    /* [in] */ Boolean clearing)
{
    if (clearing) {
        ClearMetadata();
    }
    return NOERROR;
}

ECode CKeyguardTransportControlView::MyRemoteControllerOnClientUpdateListener::OnClientPlaybackStateUpdate(
    /* [in] */ Int32 state)
{
    return UpdatePlayPauseState(state);
}

ECode CKeyguardTransportControlView::MyRemoteControllerOnClientUpdateListener::OnClientPlaybackStateUpdate(
    /* [in] */ Int32 state,
    /* [in] */ Int64 stateChangeTimeMs,
    /* [in] */ Int64 currentPosMs,
    /* [in] */ Float speed)
{
    UpdatePlayPauseState(state);
    if (DEBUG) Logger::D(TAG, "onClientPlaybackStateUpdate(state=%d, stateChangeTimeMs=%lld, currentPosMs=%lld, "
            "speed=%f)", state, stateChangeTimeMs, currentPosMs, speed);

    RemoveCallbacks(mUpdateSeekBars);
    // Since the music client may be responding to historical events that cause the
    // playback state to change dramatically, wait until things become quiescent before
    // resuming automatic scrub position update.
    Int32 visibility;
    mTransientSeek->GetVisibility(&visibility);
    if (visibility == IView::VISIBLE
            && PlaybackPositionShouldMove(mCurrentPlayState)) {
        PostDelayed(mUpdateSeekBars, QUIESCENT_PLAYBACK_FACTOR);
    }
    return NOERROR;
}

ECode CKeyguardTransportControlView::MyRemoteControllerOnClientUpdateListener::OnClientTransportControlUpdate(
    /* [in] */ Int32 transportControlFlags)
{
    return mHost->UpdateTransportControls(transportControlFlags);
}

ECode CKeyguardTransportControlView::MyRemoteControllerOnClientUpdateListener::OnClientMetadataUpdate(
    /* [in] */ IRemoteControllerMetadataEditor* metadataEditor)
{
    return mHost->UpdateMetadata(metadataEditor);
}

ECode CKeyguardTransportControlView::UpdateSeekBarRunnable::Run()
{
    Boolean seekAble = UpdateOnce();
    if (seekAble) {
        RemoveCallbacks(this);
        PostDelayed(this, 1000);
    }
    return NOERROR;
}

ECode CKeyguardTransportControlView::UpdateSeekBarRunnable::UpdateOnce(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return UpdateSeekBars(result);
}

ECode CKeyguardTransportControlView::MyRunnable::Run()
{
    return mHost->ResetToMetadata();
}

CAR_INTERFACE_IMPL(CKeyguardTransportControlView::MyViewOnClickListener, Object, IViewOnClickListener)

ECode CKeyguardTransportControlView::MyViewOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    Int32 keyCode = -1;
    if (TO_IINTERFACE(v) == TO_IINTERFACE(mBtnPrev)) {
        keyCode = IKeyEvent::KEYCODE_MEDIA_PREVIOUS;
    }
    else if (TO_IINTERFACE(v) == TO_IINTERFACE(mBtnNext)) {
        keyCode = IKeyEvent::KEYCODE_MEDIA_NEXT;
    }
    else if (TO_IINTERFACE(v) == TO_IINTERFACE(mBtnPlay)) {
        keyCode = IKeyEvent::KEYCODE_MEDIA_PLAY_PAUSE;
    }
    if (keyCode != -1) {
        SendMediaButtonClick(keyCode);
        DelayResetToMetadata(); // if the scrub bar is showing, keep showing it.
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(CKeyguardTransportControlView::MyViewOnLongClickListener, Object, IViewOnLongClickListener)

CARAPI MyViewOnLongClickListener::OnLongClick(
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
    mHost->ScrubTo(mHost->mProgress);
    mHost->mPending = FALSE;
    return NOERROR;
}

ECode CKeyguardTransportControlView::FutureSeekRunnable::SetProgress(
    /* [in] */  Int32 progress)
{
    mHost->mProgress = progress;
    if (!mHost->mPending) {
        mHost->mPending = TRUE;
        PostDelayed(this, 30);
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
        mHost->mFutureSeekRunnable->SetProgress(progress);
        mHost->DelayResetToMetadata();
        mHost->mTempDate->SetTime(progress);
        mHost->mTransientSeekTimeElapsed->SetText(mFormat.format(mTempDate));
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
    mHost->RemoveCallbacks(mUpdateSeekBars); // don't update during user interaction
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
    return mHost->SetEnableMarquee(FALSE);
}

ECode CKeyguardTransportControlView::MyKeyguardUpdateMonitorCallback::OnScreenTurnedOn()
{
    return mHost->SetEnableMarquee(TRUE);
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

ECode CKeyguardTransportControlView::Metadata::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder sb;
    sb += "Metadata[artist=";
    sb += mArtist;
    sb += " trackTitle=";
    sb += mTrackTitle;
    sb += " albumTitle=";
    sb += mAlbumTitle;
    sb += " duration=";
    sb += mDuration;
    sb += "]";

    return sb.ToString(str);
}

ECode CKeyguardTransportControlView::SavedState::constructor(
    /* [in] */ IParcelable* superState)
{
    return View::BaseSavedState::constructor(superState)
}

ECode CKeyguardTransportControlView::SavedState::WriteToParcel(
    /* [in] */ IParcel* out,
    /* [in] */ Int32 flags)
{
    View::BaseSavedState::WriteToParcel(out, flags);
    out->WriteInt32(clientPresent ? 1 : 0);
    out->WriteString(artist);
    out->WriteString(trackTitle);
    out->WriteString(albumTitle);
    out->WriteInt64(duration);
    return bitmap->WriteToParcel(out, flags);
}

ECode CKeyguardTransportControlView::SavedState::constructor(
    /* [in] */ IParcel* in)
{
    View::BaseSavedState::constructor(in);

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

CKeyguardTransportControlView::CKeyguardTransportControlView()
    : mTransportControlFlags(0)
    , mCurrentPlayState(0)
    , mSeekEnabled(FALSE)
{
    CMetadata::New((IMetadata**)&mMetadata);

    CDate::New((IDate**)&mTempDate);

    mRCClientUpdateListener = new MyRemoteControllerOnClientUpdateListener(this);

    mUpdateSeekBars = new UpdateSeekBarRunnable(this);

    mResetToMetadata = new MyRunnable(this);

    mTransportCommandListener = new MyViewOnClickListener(this);

    mTransportShowSeekBarListener = new MyViewOnLongClickListener(this);

    mFutureSeekRunnable = new FutureSeekRunnable(this);

    mOnSeekBarChangeListener = new MySeekBarOnSeekBarChangeListener(this);

    mUpdateMonitor = new MyKeyguardUpdateMonitorCallback(this);
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

    if (DEBUG) Logger::V(TAG, "Create TCV %s", TO_CSRE(this));
    CAudioManager::New(mContext, (IAudioManager**)&mAudioManager);
    mCurrentPlayState = IRemoteControlClient::PLAYSTATE_NONE; // until we get a callback
    CRemoteController::New(context, mRCClientUpdateListener, (IRemoteController**)&mRemoteController);

    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr<IDisplayMetrics> dm;
    resources->GetDisplayMetrics((IDisplayMetrics**)&dm);

    Int32 width;
    dm->GetWidthPixels(&width);
    Int32 height;
    dm->GetHeightPixels(&height);
    Int32 dim = Math::Max(width, height);
    mRemoteController->SetArtworkConfiguration(TRUE, dim, dim);

    AutoPtr<IChangeText> tc;
    CChangeText::New((IChangeText**)&tc);
    tc->SetChangeBehavior(IChangeText::CHANGE_BEHAVIOR_OUT_IN);
    AutoPtr<ITransitionSet> inner;
    CTransitionSet::New((ITransitionSet**)&inner);

    AutoPtr<IChangeBounds> bounds;
    CChangeBounds::New((IChangeBounds**)&bounds);
    inner->AddTransition(tc);
    inner->AddTransition(bounds);

    AutoPtr<ITransitionSet> tg;
    CTransitionSet::New((ITransitionSet**)&tg);
    AutoPtr<IFade> fo;
    CFade::New(IFade::OUT, (IFade**)&fo);
    tg->AddTransition(fo);
    tg->AddTransition(inner);
    AutoPtr<IFade> fi;
    CFade::New(IFade::IN, (IFade**)&fi);
    tg->AddTransition(fi);
    tg->SetOrdering(ITransitionSet::ORDERING_SEQUENTIAL);
    tg->SetDuration(TRANSITION_DURATION);
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
        mMetadataContainer->SetVisibility(VISIBLE);
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
    if (mTrackTitle != NULL) mTrackTitle->SetSelected(enabled);
    if (mTrackArtistAlbum != NULL) mTrackTitle->SetSelected(enabled);
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
    mBtnPrev->SetOnClickListener(mTransportCommandListener);
    mBtnPrev->SetOnLongClickListener(mTransportShowSeekBarListener);

    mBtnPlay->SetOnClickListener(mTransportCommandListener);
    mBtnPlay->SetOnLongClickListener(mTransportShowSeekBarListener);

    mBtnNext->SetOnClickListener(mTransportCommandListener);
    mBtnNext->SetOnLongClickListener(mTransportShowSeekBarListener);

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
    if (DEBUG) Logger::V(TAG, "Registering TCV %s", TO_CSTR(this));
    mMetadata->Clear();
    mAudioManager->RegisterRemoteController(mRemoteController);
    return KeyguardUpdateMonitor::GetInstance(mContext)->RegisterCallback(mUpdateMonitor);
}

ECode CKeyguardTransportControlView::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    FrameLayout::OnConfigurationChanged(newConfig);

    AutoPtr<IContext> context;
    GetContext((IContext**)context);
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr<IDisplayMetrics> dm;
    resources->GetDisplayMetrics((IDisplayMetrics**)&dm);

    Int32 width;
    dm->GetWidthPixels(&width);
    Int32 height;
    dm->GetHeightPixels(&height);
    Int32 dim = Math::Max(width, height);
    return mRemoteController->SetArtworkConfiguration(TRUE, dim, dim);
}

ECode CKeyguardTransportControlView::OnDetachedFromWindow()
{
    if (DEBUG) Logger::V(TAG, "onDetachFromWindow()");
    FrameLayout::OnDetachedFromWindow();
    if (DEBUG) Logger::V(TAG, "Unregistering TCV %s", TO_CSTR(this));
    mAudioManager->UnregisterRemoteController(mRemoteController);
    KeyguardUpdateMonitor::GetInstance(mContext)->RemoveCallback(mUpdateMonitor);
    mMetadata->Clear();
    return RemoveCallbacks(mUpdateSeekBars);
}

ECode CKeyguardTransportControlView::OnSaveInstanceState(
    /* [out] */ IParcelable** p)
{
    VALIDATE_NOT_NULL(p)

    AutoPtr<IParcelable> _p = FrameLayout::OnSaveInstanceState();
    AutoPtr<SavedState> ss = new SavedState(_p);
    ss->mArtist = mMetadata->mArtist;
    ss->mTrackTitle = mMetadata->mTrackTitle;
    ss->mAlbumTitle = mMetadata->mAlbumTitle;
    ss->mDuration = mMetadata->mDuration;
    ss->mBitmap = mMetadata->mBitmap;
    *p = IParcelable::Probe(ss);
    return NOERROR;
}

ECode CKeyguardTransportControlView::OnRestoreInstanceState(
    /* [in] */ IParcelable* state)
{
    if (ISavedState::Probe(state) == NULL) {
        FrameLayout::OnRestoreInstanceState(state);
        return NOERROR;
    }
    AutoPtr<SavedState> ss = (SavedState*)ISavedState::Probe(state);
    FrameLayout::OnRestoreInstanceState(ss.getSuperState());
    mMetadata->mArtist = ss->mArtist;
    mMetadata->mTrackTitle = ss->mTrackTitle;
    mMetadata->mAlbumTitle = ss->mAlbumTitle;
    mMetadata->mDuration = ss->mDuration;
    mMetadata->mBitmap = ss->mBitmap;
    return PopulateMetadata();
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
    mBadge->SetColorFilter(filter);
    AutoPtr<IPorterDuffXfermode> mode;
    CPorterDuffXfermode::New(PorterDuffMode_SCREEN, (IPorterDuffXfermode**)&mode);
    mBadge->SetXfermode(mode);
    return mBadge->SetImageAlpha(0xef);
}

ECode CKeyguardTransportControlView::ClearMetadata()
{
    mPopulateMetadataWhenAttached = NULL;
    mMetadata->Clear();
    return PopulateMetadata();
}

ECode CKeyguardTransportControlView::UpdateMetadata(
    /* [in] */ IRemoteControllerMetadataEditor* data)
{
    if (IsAttachedToWindow()) {
        data->GetString(IMediaMetadataRetriever::METADATA_KEY_ALBUMARTIST,mMetadata->mArtist, &(mMetadata->mArtist));
        data->GetString(IMediaMetadataRetriever::METADATA_KEY_TITLE, mMetadata->mTrackTitle, &(mMetadata->mTrackTitle));
        data->GetString(IMediaMetadataRetriever::METADATA_KEY_ALBUM, mMetadata->mAlbumTitle, &(mMetadata->mAlbumTitle));
        data->GetInt64(IMediaMetadataRetriever::METADATA_KEY_DURATION, -1, &(mMetadata->mDuration));
        data->GetBitmap(MediaMetadataEditor.BITMAP_KEY_ARTWORK, mMetadata->mBitmap, (IBitmap**)&(mMetadata->mBitmap));
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
    if (ANIMATE_TRANSITIONS && IsLaidOut() && (mMetadataContainer->GetVisibility(&visibility), visibility) == VISIBLE) {
        TransitionManager::BeginDelayedTransition(mMetadataContainer, mMetadataChangeTransition);
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
    if (ec == (ECode)NameNotFoundException) {
        Logger::E(TAG, "Couldn't get remote control client package icon %d", ec);
    }
    SetBadgeIcon(badgeIcon);
    mTrackTitle->SetText(!TextUtils::IsEmpty(mMetadata->mTrackTitle)
            ? mMetadata->mTrackTitle : NULL);

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
    sb->ToString(&trackArtistAlbum);
    mTrackArtistAlbum->SetText(!TextUtils::IsEmpty(trackArtistAlbum) ?
            trackArtistAlbum : NULL);

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
        CSimpleDateFormat::New(pattern, (ISimpleDateFormat**)&mFormat);
        AutoPtr<ITimeZoneHelper> helper;
        CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&helper);
        AutoPtr<ITimeZone> ppTimezone;
        helper->GetTimeZone(String("GMT+0"), (ITimeZone**)&ppTimezone);
        mFormat->SetTimeZone(ppTimezone);
    }
    else {
        SetSeekBarsEnabled(FALSE);
    }

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    KeyguardUpdateMonitor::GetInstance(context)->DispatchSetBackground(mMetadata->mBitmap);
    Int32 flags = mTransportControlFlags;
    SetVisibilityBasedOnFlag(mBtnPrev, flags, IRemoteControlClient::FLAG_KEY_MEDIA_PREVIOUS);
    SetVisibilityBasedOnFlag(mBtnNext, flags, IRemoteControlClient::FLAG_KEY_MEDIA_NEXT);
    SetVisibilityBasedOnFlag(mBtnPlay, flags,
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
        mTransientSeekTimeElapsed->SetText(mFormat.format(mTempDate));
        mTempDate->SetTime(mMetadata->mDuration);
        mTransientSeekTimeTotal->SetText(mFormat.format(mTempDate));

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
        TransitionManager::BeginDelayedTransition(mInfoContainer);
    }

    Int32 visibility;
    mTransientSeek->GetVisibility(&visibility);
    if (visibility == VISIBLE) {
        mTransientSeek->SetVisibility(INVISIBLE);
        mMetadataContainer->SetVisibility(VISIBLE);
        CancelResetToMetadata();
        RemoveCallbacks(mUpdateSeekBars); // don't update if scrubber isn't visible
    }
    else {
        mTransientSeek->SetVisibility(VISIBLE);
        mMetadataContainer->SetVisibility(INVISIBLE);
        DelayResetToMetadata();
        if (PlaybackPositionShouldMove(mCurrentPlayState)) {
            mUpdateSeekBars->Run();
        }
        else {
            mUpdateSeekBars->UpdateOnce();
        }
    }
    mTransportControlCallback->UserActivity();
    *result = TRUE;
    return NOERROR;
}

ECode CKeyguardTransportControlView::ResetToMetadata()
{
    if (ANIMATE_TRANSITIONS) {
        TransitionManager::BeginDelayedTransition(mInfoContainer);
    }
    Int32 visibility;
    mTransientSeek->GetVisibility(&visibility);
    if (visibility == VISIBLE) {
        mTransientSeek->SetVisibility(INVISIBLE);
        mMetadataContainer->SetVisibility(VISIBLE);
    }
    // TODO Also hide ratings, if applicable
    return NOERROR;
}

ECode CKeyguardTransportControlView::DelayResetToMetadata()
{
    RemoveCallbacks(mResetToMetadata);
    return PostDelayed(mResetToMetadata, RESET_TO_METADATA_DELAY);
}

ECode CKeyguardTransportControlView::CancelResetToMetadata()
{
    return RemoveCallbacks(mResetToMetadata);
}

ECode CKeyguardTransportControlView::SetSeekBarDuration(
    /* [in] */ Int64 duration)
{
    return mTransientSeekBar->SetMax((Int32)duration);
}

ECode CKeyguardTransportControlView::ScrubTo(
    /* [in] */ Int32 progress)
{
    mRemoteController->SeekTo(progress);
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
    mBtnPlay->SetContentDescription(str);
    mCurrentPlayState = state;
}

ECode CKeyguardTransportControlView::UpdateSeekBars(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int64 position;
    mRemoteController->GetEstimatedMediaPosition(&position);
    Int32 position = (Int32)position;
    if (DEBUG) Logger::V(TAG, "Estimated time:%d", position);
    if (position >= 0) {
        mTransientSeekBar->SetProgress(position);
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
    mRemoteController->SendMediaKeyEvent(event1);

    AutoPtr<IKeyEvent> event2;
    CKeyEvent::New(IKeyEvent::ACTION_UP, keyCode, (IKeyEvent**)&event2);
    mRemoteController->SendMediaKeyEvent(event2);

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
