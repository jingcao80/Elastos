/******************************************************************************************
 * filename:        CPlayActivity.cpp
 * description:     Play audio and display its detail info
 * author:          kari.zhang
 *
 * modifications:
 *  1. Implement IOnPlayCompletionListener and remove IMediaPlayerOnPlayCompletionListener.
 *     kari.zhang @ 2014-07-11
 *
 ******************************************************************************************/

#include "CMusicUtil.h"
#include "CPlayActivity.h"
#include "CPlayActivityListener.h"
#include "R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::CStringWrapper;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::App::EIID_IActivity;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Media::IMediaPlayerOnCompletionListener;
using Elastos::Droid::Media::EIID_IMediaPlayerOnCompletionListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::EIID_ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Widget::EIID_ISeekBarOnSeekBarChangeListener;

namespace MusicPlayer {

//=======================================================================
//              CPlayActivity::MyViewOnClickListener
//=======================================================================
CAR_INTERFACE_IMPL(CPlayActivity::MyViewOnClickListener, IViewOnClickListener)

CPlayActivity::MyViewOnClickListener::MyViewOnClickListener(
    /* [in] */ CPlayActivity* host)
    : mActivity(host)
{
}

ECode CPlayActivity::MyViewOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    Int32 viewId;
    v->GetId(&viewId);

    switch (viewId)
    {
    case R::play::imgview_title_back:
        mActivity->Finish();
        break;

    case R::play::btn_more:
        Int32 visibility;
        mActivity->mLayoutVolume->GetVisibility(&visibility);
        if (visibility == IView::VISIBLE) {
            mActivity->mLayoutVolume->SetVisibility(IView::INVISIBLE);
        }
        else {
            mActivity->mLayoutVolume->SetVisibility(IView::VISIBLE);
        }
        break;

    case R::play::btn_play:
        if (mActivity->mPlayController != NULL) {
            mActivity->mPlayController->Play();
            Int32 duration;
            mActivity->mPlayController->GetDuration(&duration);
            mActivity->SetDuration(duration);
        }
        mActivity->mBtnPlay->SetVisibility(IView::INVISIBLE);
        mActivity->mBtnPause->SetVisibility(IView::VISIBLE);
        break;

    case R::play::btn_pause:
        if (mActivity->mPlayController != NULL) {
            mActivity->mPlayController->Pause();
        }
        mActivity->mBtnPlay->SetVisibility(IView::VISIBLE);
        mActivity->mBtnPause->SetVisibility(IView::INVISIBLE);
        break;

    case R::play::btn_previous:
        if (mActivity->mPlayController != NULL) {
            mActivity->mPlayController->PlayPrevious();
        }
        mActivity->InitStatus();
        break;

    case R::play::btn_next:
        if (mActivity->mPlayController != NULL) {
            mActivity->mPlayController->PlayNext();
        }
        mActivity->InitStatus();
        break;

        // next will be random play
    case R::play::btn_mode_single_loop:
        mActivity->mBtnModeAllLoop->SetVisibility(IView::INVISIBLE);
        mActivity->mBtnModeSingleLoop->SetVisibility(IView::INVISIBLE);
        mActivity->mBtnModeRandom->SetVisibility(IView::VISIBLE);
        mActivity->mBtnModeOrder->SetVisibility(IView::INVISIBLE);
        if (NULL != mActivity->mPlayController) {
            mActivity->mPlayController->SetPlayMode(IPlayController::MODE_SINGLE_LOOP);
        }
        break;

        // next will be single-loop mode
    case R::play::btn_mode_all_loop:
        mActivity->mBtnModeAllLoop->SetVisibility(IView::INVISIBLE);
        mActivity->mBtnModeSingleLoop->SetVisibility(IView::VISIBLE);
        mActivity->mBtnModeRandom->SetVisibility(IView::INVISIBLE);
        mActivity->mBtnModeOrder->SetVisibility(IView::INVISIBLE);
        if (NULL != mActivity->mPlayController) {
            mActivity->mPlayController->SetPlayMode(IPlayController::MODE_ALL_LOOP);
        }
        break;

        // next will be order mode
    case R::play::btn_mode_random:
        mActivity->mBtnModeAllLoop->SetVisibility(IView::INVISIBLE);
        mActivity->mBtnModeSingleLoop->SetVisibility(IView::INVISIBLE);
        mActivity->mBtnModeRandom->SetVisibility(IView::INVISIBLE);
        mActivity->mBtnModeOrder->SetVisibility(IView::VISIBLE);
        if (NULL != mActivity->mPlayController) {
            mActivity->mPlayController->SetPlayMode(IPlayController::MODE_RANDOM_PLAY);
        }
        break;

        // next will be all-loop mode
    case R::play::btn_mode_order:
        mActivity->mBtnModeAllLoop->SetVisibility(IView::VISIBLE);
        mActivity->mBtnModeSingleLoop->SetVisibility(IView::INVISIBLE);
        mActivity->mBtnModeRandom->SetVisibility(IView::INVISIBLE);
        mActivity->mBtnModeOrder->SetVisibility(IView::INVISIBLE);
        if (NULL != mActivity->mPlayController) {
            mActivity->mPlayController->SetPlayMode(IPlayController::MODE_ORDER_PLAY);
        }
        break;
    }

    return NOERROR;
}

//=======================================================================
//              CPlayActivity::SeekBarChangedListener
//=======================================================================
CAR_INTERFACE_IMPL(CPlayActivity::SeekBarChangedListener, ISeekBarOnSeekBarChangeListener)

CPlayActivity::SeekBarChangedListener::SeekBarChangedListener(
    /* [in] */ CPlayActivity* activity)
    : mActivity(activity)
{
}

ECode CPlayActivity::SeekBarChangedListener::OnProgressChanged(
    /* [in] */ ISeekBar* seekBar,
    /* [in] */ Int32 progress, Boolean fromUser)
{
    return NOERROR;
}

ECode CPlayActivity::SeekBarChangedListener::OnStartTrackingTouch(
    /* [in] */ ISeekBar* seekBar)
{
    mActivity->mIsSeekBarMoving = TRUE;
    return NOERROR;
}

ECode CPlayActivity::SeekBarChangedListener::OnStopTrackingTouch(
    /* [in] */ ISeekBar* seekBar)
{
    Int32 progress;
    seekBar->GetProgress(&progress);
    if (mActivity->mPlayController != NULL) {
        mActivity->mPlayController->SeekTo(progress);
        mActivity->mSeekBarMusic->SetProgress(progress);
    }

    mActivity->mIsSeekBarMoving = FALSE;

    return NOERROR;
}

//=======================================================================
//              CPlayActivity::RadioButtonCheckedChangeListener
//=======================================================================
CAR_INTERFACE_IMPL(CPlayActivity::RadioButtonCheckedChangeListener, ICompoundButtonOnCheckedChangeListener)

CPlayActivity::RadioButtonCheckedChangeListener::RadioButtonCheckedChangeListener(
    /* [in] */ CPlayActivity* host)
    : mActivity(host)
{
}

ECode CPlayActivity::RadioButtonCheckedChangeListener::OnCheckedChanged(
    /* [in] */ ICompoundButton* btn,
    /* [in] */ Boolean isChecked)
{
    Int32 viewId;
    btn->GetId(&viewId);

    if (isChecked) {
        if (R::play::btn_switch_left == viewId) {
            mActivity->mLayoutPlayList->SetVisibility(IView::INVISIBLE);
        }
        else {
            mActivity->mLayoutPlayList->SetVisibility(IView::VISIBLE);
        }
    }

    return NOERROR;
}

//=======================================================================
//              CPlayActivity
//=======================================================================

CPlayActivity::CPlayActivity()
    : mIsSeekBarMoving(FALSE)
{
    Logger::V("CPlayActivity", "CPlayActivity(): %p", this);
}

CPlayActivity::~CPlayActivity()
{
    Logger::V("CPlayActivity", "~CPlayActivity(): %p", this);
}

PInterface CPlayActivity::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (IInterface*) (IActivity *)this;
    }
    else if (riid == EIID_IActivity)  {
        return (IActivity *)this;
    }
    else if (riid == EIID_IServiceConnection)  {
        return (IServiceConnection*)this;
    }
    else if(EIID_IOnTickListener == riid)  {
        return (IOnTickListener*)this;
    }
    else if(EIID_IOnPlayCompletionListener == riid ) {
        return (IOnPlayCompletionListener *)this;
    }

    return Activity::Probe(riid);
}

UInt32 CPlayActivity::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CPlayActivity::Release()
{
 return ElRefBase::Release();
}

ECode CPlayActivity::GetInterfaceID(
    /* [in] */ IInterface* pObject,
    /* [out] */ InterfaceID* pIID)
{
    VALIDATE_NOT_NULL(pIID);

    if (pObject == (IInterface *)(IServiceConnection *)this) {
        *pIID = EIID_IServiceConnection;
        return NOERROR;
    }
    else if (pObject == (IInterface *)(IOnTickListener *)this) {
        *pIID = EIID_IOnTickListener;
        return NOERROR;
    }
    else if (pObject == (IInterface *)(IOnPlayCompletionListener *)this) {
        *pIID = EIID_IOnPlayCompletionListener;
        return NOERROR;
    }

    return Activity::GetInterfaceID(pObject, pIID);
}

ECode CPlayActivity::GetClassID(
    /* [out] */ ClassID* clsid)
{
    if (NULL == clsid) {
        return E_INVALID_ARGUMENT;
    }

    *clsid = ECLSID_CPlayActivity;
    return NOERROR;
}

ECode CPlayActivity::OnTick(
    /* [in] */ Int32 progress)
{
    if (!mIsSeekBarMoving) {
        SetProgress(progress);
    }
    return NOERROR;
}

ECode CPlayActivity::OnCompletion()
{
    Logger::D("CPlayActivity", "OnCompletion()");

    SetPlayButtonStatus(FALSE);
    SetProgress(0);
    InitStatus();

    return NOERROR;
}

ECode CPlayActivity::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    Logger::D("CPlayActivity", "OnServiceConnected() %p, playerController %p", service, IPlayController::Probe(service));
    mPlayController = IPlayController::Probe(service);
    if (mPlayController == NULL) {
        Logger::E("CPlayActivity::OnServiceConnected", "pService is not a IPlayController.");
        return E_RUNTIME_EXCEPTION;
    }

    if (!mFilePath.IsNull()) {
        mPlayController->OpenFile(mFilePath);
    }

    InitStatus();

    mPlayController->SetOnTickListener((Int32)this, mOnTickListner);
    mPlayController->SetPlayCompletionListener((Int32)this, mOnPlayCompletionListener);

    return NOERROR;
}

ECode CPlayActivity::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    return NOERROR;
}

ECode CPlayActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Logger::D("CPlayActivity", "OnCreate()");

    Activity::OnCreate(savedInstanceState);

    SetContentView(R::layout::activity_play);

    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    if (intent != NULL) {
        AutoPtr<IUri> uri;
        intent->GetData((IUri**)&uri);
        if (uri != NULL) {
            String scheme;
            uri->GetScheme(&scheme);
            if (scheme.Equals("file")) {
                uri->GetPath(&mFilePath);
                Logger::D("CPlayActivity", "mFilePath = %s", mFilePath.string());
            }
            else {
                Logger::D("CPlayActivity", "scheme of uri of intent is not file");
            }
        }
        else {
            Logger::D("CPlayActivity", " data of intent is NULL");
        }
    }
    else {
        Logger::D("CPlayActivity", "intent is NULL");
    }


    AutoPtr<IWeakReference> wr;
    GetWeakReference((IWeakReference**)&wr);
    CPlayActivityListener::New(wr, (IOnPlayCompletionListener**)&mOnPlayCompletionListener);
    mOnTickListner = IOnTickListener::Probe(mOnPlayCompletionListener);
    assert(mOnTickListner != NULL);

    mActionListener = new MyViewOnClickListener(this);
    mSwitchListener = new RadioButtonCheckedChangeListener(this);
    mSeekBarListener = new SeekBarChangedListener(this);

    InitViews();
    SetupListeners();

    AutoPtr<IContentResolver> contentResolver;
    GetContentResolver((IContentResolver**)&contentResolver);
    AutoPtr<ICursor> cursor;
    //       contentResolver->Query(IMediaStore.Audio.Media.EXTERNAL_CONTENT_URI, NULL, NULL, NULL, IMediaStore.Audio.Media.DEFAULT_SORT_ORDER, (ICursor**)&cursor);

    CMusicUtil::BindToService(IContext::Probe(this), IServiceConnection::Probe(this));

    mIsSeekBarMoving = FALSE;

    return NOERROR;
}

ECode CPlayActivity::OnStart()
{
    Logger::D("CPlayActivity", "OnStart()");
    Activity::OnStart();

    return NOERROR;
}

ECode CPlayActivity::InitStatus()
{
    if (mPlayController) {
        AutoPtr<ICharSequence> trackName;
        mPlayController->GetTrackName((ICharSequence**)&trackName);
        mTextViewAlbumAndName->SetText(trackName);
        mTextViewTitle->SetText(trackName);

        Boolean isPlaying;
        mPlayController->IsPlaying(&isPlaying);
        SetPlayButtonStatus(isPlaying);

        Int32 duration, progress;
        mPlayController->GetDuration(&duration);
        SetDuration(duration);
        mPlayController->GetProgress(&progress);
        SetProgress(progress);

        Int32 mode;
        mPlayController->GetPlayMode(&mode);
        ++mode;
        if (mode >= IPlayController::MODE_INVALID) {
            mode = 0;
        }
        SetPlayModeStatus(mode);
    }

    return NOERROR;
}

ECode CPlayActivity::OnResume()
{
    Logger::D("CPlayActivity", "OnResume()");

    Activity::OnResume();

    return NOERROR;
}

ECode CPlayActivity::OnStop()
{
    Logger::D("CPlayActivity", "OnStop()");

    Activity::OnStop();
    return NOERROR;
}

ECode CPlayActivity::OnDestroy()
{
    Logger::D("CPlayActivity", "OnDestroy()");

    Activity::OnDestroy();

    CMusicUtil::UnbindFromService(IContext::Probe(this));

    if (mPlayController != NULL) {
        mPlayController->RemoveOnTickListener((Int32)this);
        mPlayController->RemovePlayCompletionListener((Int32)this);
        mPlayController = NULL;
    }

    return NOERROR;
}

ECode CPlayActivity::SetPlayButtonStatus(
    /* [in] */ Boolean isPlaying)
{
    if (isPlaying) {
        mBtnPlay->SetVisibility(IView::INVISIBLE);
        mBtnPause->SetVisibility(IView::VISIBLE);
    }
    else {
        mBtnPlay->SetVisibility(IView::VISIBLE);
        mBtnPause->SetVisibility(IView::INVISIBLE);
    }

    return NOERROR;
}

ECode CPlayActivity::SetPlayModeStatus(
    /* [in] */ Int32 mode)
{
    if (IPlayController::MODE_ALL_LOOP == mode) {
        mBtnModeAllLoop->SetVisibility(IView::VISIBLE);
        mBtnModeSingleLoop->SetVisibility(IView::INVISIBLE);
        mBtnModeRandom->SetVisibility(IView::INVISIBLE);
        mBtnModeOrder->SetVisibility(IView::INVISIBLE);
    }
    else if(IPlayController::MODE_SINGLE_LOOP == mode) {
        mBtnModeAllLoop->SetVisibility(IView::INVISIBLE);
        mBtnModeSingleLoop->SetVisibility(IView::VISIBLE);
        mBtnModeRandom->SetVisibility(IView::INVISIBLE);
        mBtnModeOrder->SetVisibility(IView::INVISIBLE);
    }
    else if(IPlayController::MODE_RANDOM_PLAY == mode) {
        mBtnModeAllLoop->SetVisibility(IView::INVISIBLE);
        mBtnModeSingleLoop->SetVisibility(IView::INVISIBLE);
        mBtnModeRandom->SetVisibility(IView::VISIBLE);
        mBtnModeOrder->SetVisibility(IView::INVISIBLE);
    }
    else if(IPlayController::MODE_ORDER_PLAY == mode) {
        mBtnModeAllLoop->SetVisibility(IView::INVISIBLE);
        mBtnModeSingleLoop->SetVisibility(IView::INVISIBLE);
        mBtnModeRandom->SetVisibility(IView::INVISIBLE);
        mBtnModeOrder->SetVisibility(IView::VISIBLE);
    }

    return NOERROR;
}

ECode CPlayActivity::SetDuration(
    /* [in] */ Int32 duration)
{
    mSeekBarMusic->SetMax(duration);
    Int32 dura = duration / 1000;
    AutoPtr<ICharSequence> seq;
    String str("");
    str.AppendFormat("%02d:%02d", dura / 60, dura % 60);
    CStringWrapper::New(str, (ICharSequence**)&seq);
    mTextViewDuration->SetText(seq);

    return NOERROR;
}

ECode CPlayActivity::SetProgress(
    /* [in] */ Int32 currentDuration)
{
    mSeekBarMusic->SetProgress(currentDuration);
    Int32 curr = currentDuration / 1000;
    AutoPtr<ICharSequence> seq;
    String str("");
    str.AppendFormat("%02d:%02d", curr / 60, curr % 60);
    CStringWrapper::New(str, (ICharSequence**)&seq);
    mTextViewCurrentDuration->SetText(seq);

    return NOERROR;
}

ECode CPlayActivity::InitViews()
{
    AutoPtr<IView> view;

    mViewBack = FindViewById(R::play::imgview_title_back);
    assert(mViewBack != NULL);

    view = FindViewById(R::play::tv_title_song);
    assert(NULL != view);
    mTextViewTitle = ITextView::Probe(view);
    assert(NULL != view);

    mBtnMore = FindViewById(R::play::btn_more);
    assert(mBtnMore != NULL);

    mLayoutVolume = FindViewById(R::play::layout_volume_bar);
    assert(mLayoutVolume != NULL);

    view = FindViewById(R::play::btn_switch_left);
    assert(view != NULL);
    mBtnSwitchLeft = IRadioButton::Probe(view);

    view = FindViewById(R::play::btn_switch_right);
    assert(view != NULL);
    mBtnSwitchRight = IRadioButton::Probe(view);

    mLayoutPlayList = FindViewById(R::play::layout_playlist);
    assert(mLayoutPlayList != NULL);

    mBtnPrevious = FindViewById(R::play::btn_previous);
    assert(NULL != mBtnPrevious);

    mBtnNext = FindViewById(R::play::btn_next);
    assert(NULL != mBtnNext);

    mBtnPlay = FindViewById(R::play::btn_play);
    assert(mBtnPlay != NULL);

    mBtnPause = FindViewById(R::play::btn_pause);
    assert(mBtnPause != NULL);

    mBtnModeAllLoop = FindViewById(R::play::btn_mode_all_loop);
    assert(mBtnModeAllLoop != NULL);

    mBtnModeSingleLoop = FindViewById(R::play::btn_mode_single_loop);
    assert(mBtnModeSingleLoop != NULL);

    mBtnModeRandom = FindViewById(R::play::btn_mode_random);
    assert(mBtnModeRandom != NULL);

    mBtnModeOrder = FindViewById(R::play::btn_mode_order);
    assert(mBtnModeOrder != NULL);

    view = FindViewById(R::play::tv_album_and_name);
    assert(view != NULL);
    mTextViewAlbumAndName = ITextView::Probe(view);
    assert(view != NULL);

    view = FindViewById(R::play::tv_total_time);
    assert(view != NULL);
    mTextViewDuration = ITextView::Probe(view);
    assert(mTextViewDuration != NULL);

    view = FindViewById(R::play::tv_current_time);
    assert(view != NULL);
    mTextViewCurrentDuration = ITextView::Probe(view);
    assert(mTextViewCurrentDuration);

    view = FindViewById(R::play::seekbar_music);
    assert(view != NULL);
    mSeekBarMusic = ISeekBar::Probe(view);
    assert(mSeekBarMusic != NULL);

    return NOERROR;
}

ECode CPlayActivity::SetupListeners()
{
    mViewBack->SetOnClickListener(mActionListener);
    mBtnMore->SetOnClickListener(mActionListener);
    mBtnPlay->SetOnClickListener(mActionListener);
    mBtnPause->SetOnClickListener(mActionListener);
    mBtnPrevious->SetOnClickListener(mActionListener);
    mBtnNext->SetOnClickListener(mActionListener);

    mBtnModeOrder->SetOnClickListener(mActionListener);
    mBtnModeRandom->SetOnClickListener(mActionListener);
    mBtnModeSingleLoop->SetOnClickListener(mActionListener);
    mBtnModeAllLoop->SetOnClickListener(mActionListener);

    mBtnSwitchLeft->SetOnCheckedChangeListener(mSwitchListener);
    mBtnSwitchRight->SetOnCheckedChangeListener(mSwitchListener);
    mSeekBarMusic->SetOnSeekBarChangeListener(mSeekBarListener);

    return NOERROR;
}

} //end namespace MusicPlayer
