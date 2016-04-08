/********************************************************
 * filename:        CMainActivity.cpp
 * description:     Display and categorize audio files
 * author:          kari.zhang
 *
 * modifications:
 *  1. Implement IOnPlayCompletionListener
 *     kari.zhang @ 2014-07-11
 *
 *  2. Use IView instead of IButton & Use Layout instead
 *     of play mode buttons.
 *     kari.zhang@2014-07-17
 *
 *******************************************************/

#include "CMainActivity.h"
#include "CMusicUtil.h"
#include "CMainActivityListener.h"
#include "R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::EIID_IRunnable;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::App::EIID_IActivity;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::EIID_IServiceConnection;
// using Elastos::Droid::Os::CHandler;
// using Elastos::Droid::Os::CLooperHelper;
// using Elastos::Droid::Os::ILooperHelper;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::CArrayAdapter;
using Elastos::Droid::Widget::EIID_ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Widget::EIID_ISeekBarOnSeekBarChangeListener;
using Elastos::Droid::Widget::EIID_IAdapterViewOnItemClickListener;

using Elastos::Droid::Os::IDebug;
using Elastos::Droid::Os::CDebug;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::CFileDescriptor;
using Libcore::IO::IOsConstants;
using Libcore::IO::COsConstants;
using Libcore::IO::ILibcore;
using Libcore::IO::CLibcore;
using Libcore::IO::IOs;
using Libcore::IO::IIoBridge;
using Libcore::IO::CIoBridge;

namespace MusicPlayer {

static void MyDumpMemery()
{
    AutoPtr<IOsConstants> osConstans;
    COsConstants::AcquireSingleton((IOsConstants**)&osConstans);
    Int32 m1, m2, m3;
    osConstans->GetOsConstant(String("O_RDWR"), &m1);
    osConstans->GetOsConstant(String("O_CREAT"), &m2);
    osConstans->GetOsConstant(String("O_TRUNC"), &m3);

    AutoPtr<IFile> file;
    CFile::New(String("/data/debug"), (IFile**)&file);
    Boolean bval;
    file->Exists(&bval);
    if (!bval) {
        file->Mkdirs(&bval);
    }

    AutoPtr<IFileDescriptor> ifd;
    CFileDescriptor::New((IFileDescriptor**)&ifd);
    AutoPtr<IIoBridge> ioBridge;
    CIoBridge::AcquireSingleton((IIoBridge**)&ioBridge);
    Int32 fd;
    ioBridge->Open(String("/data/debug/musicplayer.txt"), m1 | m2 | m3, &fd);
    ifd->SetDescriptor(fd);

    AutoPtr<IDebug> dbg;
    CDebug::AcquireSingleton((IDebug**)&dbg);
    dbg->DumpHeap(ifd);
}

//=======================================================================
//              CMainActivity::MyListener
//=======================================================================
CAR_INTERFACE_IMPL(CMainActivity::MyListener, IViewOnClickListener)

CMainActivity::MyListener::MyListener(
    /* [in] */ CMainActivity* host)
    : mActivity(host)
{
}

ECode CMainActivity::MyListener::OnClick(
    /* [in] */ IView* v)
{
    Int32 viewId;
    v->GetId(&viewId);

    AutoPtr<IIntent> intent;

    switch (viewId) {
        case R::main::title_btn_about:
            CIntent::New((IIntent**)&intent);
            intent->SetClassName(String("MusicPlayer"),
                String("MusicPlayer.CAboutActivity"));
            mActivity->StartActivity(intent);
            break;

        case R::main::title_btn_search:
            CIntent::New((IIntent**)&intent);
            intent->SetClassName(mActivity,
                String("MusicPlayer.CSearchActivity"));
            mActivity->StartActivity(intent);
            break;

        case R::main::tab_btn_edit:
            CIntent::New((IIntent**)&intent);
            intent->SetClassName(mActivity,
                String("MusicPlayer.CEditActivity"));
            mActivity->StartActivity(intent);
            break;

        case R::main::foot_btn_play:
            mActivity->mBtnPlay->SetVisibility(IView::INVISIBLE);
            mActivity->mBtnPause->SetVisibility(IView::VISIBLE);

            if (NULL != mActivity->mPlayController) {
                mActivity->mPlayController->Play();
                Int32 duration;
                mActivity->mPlayController->GetDuration(&duration);
                mActivity->mSeekBarMusic->SetMax(duration);

                Logger::D("CMainActivity", "Play");
            }
            else {
                Logger::D("CMainActivity", "Play:mPlayController=NULL");
            }

            break;

        case R::main::foot_btn_pause:
            mActivity->mBtnPause->SetVisibility(IView::INVISIBLE);
            mActivity->mBtnPlay->SetVisibility(IView::VISIBLE);

            if (NULL != mActivity->mPlayController) {
                mActivity->mPlayController->Pause();
                Logger::D("CMainActivity", "Pause");
            }
            else {
                Logger::D("CMainActivity", "Pause:mPlayController=NULL");
            }
            break;

            // next will be single-loop
        case R::main::tab_layout_all_loop:
        case R::main::tab_btn_all_loop:
            mActivity->mLayoutAllLoop->SetVisibility(IView::INVISIBLE);
            mActivity->mLayoutSingleLoop->SetVisibility(IView::VISIBLE);
            mActivity->mLayoutRandomPlay->SetVisibility(IView::INVISIBLE);
            mActivity->mLayoutOrderPlay->SetVisibility(IView::INVISIBLE);
            if (NULL != mActivity->mPlayController) {
                mActivity->mPlayController->SetPlayMode(IPlayController::MODE_ALL_LOOP);
            }
            break;

            // next will be random-play
        case R::main::tab_layout_single_loop:
        case R::main::tab_btn_single_loop:
            mActivity->mLayoutAllLoop->SetVisibility(IView::INVISIBLE);
            mActivity->mLayoutSingleLoop->SetVisibility(IView::INVISIBLE);
            mActivity->mLayoutRandomPlay->SetVisibility(IView::VISIBLE);
            mActivity->mLayoutOrderPlay->SetVisibility(IView::INVISIBLE);
            if (NULL != mActivity->mPlayController) {
                mActivity->mPlayController->SetPlayMode(IPlayController::MODE_SINGLE_LOOP);
            }
            break;

            // next will be order-play
        case R::main::tab_layout_random_play:
        case R::main::tab_btn_random_play:
            mActivity->mLayoutAllLoop->SetVisibility(IView::INVISIBLE);
            mActivity->mLayoutSingleLoop->SetVisibility(IView::INVISIBLE);
            mActivity->mLayoutRandomPlay->SetVisibility(IView::INVISIBLE);
            mActivity->mLayoutOrderPlay->SetVisibility(IView::VISIBLE);
            if (NULL != mActivity->mPlayController) {
                mActivity->mPlayController->SetPlayMode(IPlayController::MODE_RANDOM_PLAY);
            }
            break;

            // next will be all-loop
        case R::main::tab_layout_order_play:
        case R::main::tab_btn_order_play:
            mActivity->mLayoutAllLoop->SetVisibility(IView::VISIBLE);
            mActivity->mLayoutSingleLoop->SetVisibility(IView::INVISIBLE);
            mActivity->mLayoutRandomPlay->SetVisibility(IView::INVISIBLE);
            mActivity->mLayoutOrderPlay->SetVisibility(IView::INVISIBLE);
            if (NULL != mActivity->mPlayController) {
                mActivity->mPlayController->SetPlayMode(IPlayController::MODE_ORDER_PLAY);
            }
            break;

            // enter CPlayActivity after click foot layout
        case R::main::foot_layout:
            CIntent::New((IIntent**)&intent);
            intent->SetClassName(mActivity, String("MusicPlayer.CPlayActivity"));
            mActivity->StartActivity(intent);
            break;

    }

    return NOERROR;
}

//=======================================================================
//              CMainActivity::TabChangedListener
//=======================================================================

CAR_INTERFACE_IMPL(CMainActivity::TabChangedListener, ICompoundButtonOnCheckedChangeListener)

CMainActivity::TabChangedListener::TabChangedListener(
    /* [in] */ CMainActivity* host)
    : mActivity(host)
{
}

ECode CMainActivity::TabChangedListener::OnCheckedChanged(
    /* [in] */ ICompoundButton* btn,
    /* [in] */ Boolean isChecked)
{
    if (!isChecked) {
        return NOERROR;
    }

    Int32 viewId;
    btn->GetId(&viewId);

    switch (viewId)
    {
        case R::main::tab_btn_song:
            mActivity->mLayoutPlayMode->SetVisibility(IView::VISIBLE);
            mActivity->mLayoutTabRight->SetVisibility(IView::VISIBLE);
            mActivity->mBtnAddPlaylist->SetVisibility(IView::GONE);
            mActivity->mBtnEdit->SetVisibility(IView::VISIBLE);
            break;

        case R::main::tab_btn_singer:
            mActivity->mLayoutPlayMode->SetVisibility(IView::INVISIBLE);
            mActivity->mLayoutTabRight->SetVisibility(IView::INVISIBLE);
            break;

        case R::main::tab_btn_album:
            mActivity->mLayoutPlayMode->SetVisibility(IView::INVISIBLE);
            mActivity->mLayoutTabRight->SetVisibility(IView::INVISIBLE);
            break;

        case R::main::tab_btn_play_list:
            mActivity->mLayoutPlayMode->SetVisibility(IView::INVISIBLE);
            mActivity->mLayoutTabRight->SetVisibility(IView::VISIBLE);
            mActivity->mBtnAddPlaylist->SetVisibility(IView::VISIBLE);
            mActivity->mBtnEdit->SetVisibility(IView::INVISIBLE);
            break;
    }

    return NOERROR;
}

//=======================================================================
//              CMainActivity::ItemOnClickListener
//=======================================================================

CAR_INTERFACE_IMPL(CMainActivity::ItemOnClickListener, IAdapterViewOnItemClickListener)

CMainActivity::ItemOnClickListener::ItemOnClickListener(
    /* [in] */ CMainActivity* host)
    : mActivity(host)
{
}

ECode CMainActivity::ItemOnClickListener::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    if (NULL != mActivity->mPlayController) {
        Int64 audioId;
        mActivity->mPlayController->GetAudioId(&audioId);
        if (position != audioId) {
            mActivity->mPlayController->SetAudioId(position);

            Logger::D("CMainActivity", "OnItemClick(), (position, mAudioId)=(%d,%d)", position, audioId);
        }
    }

    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->SetClassName(mActivity, String("MusicPlayer.CPlayActivity"));
    mActivity->StartActivity(intent);

    return NOERROR;
}

//=======================================================================
//              CMainActivity::SeekBarChangedListener
//=======================================================================

CAR_INTERFACE_IMPL(CMainActivity::SeekBarChangedListener, ISeekBarOnSeekBarChangeListener)

CMainActivity::SeekBarChangedListener::SeekBarChangedListener(
    /* [in] */ CMainActivity* activity)
    : mActivity(activity)
{
}

ECode CMainActivity::SeekBarChangedListener::OnProgressChanged(
    /* [in] */ ISeekBar* seekBar,
    /* [in] */ Int32 progress,
    /* [in] */ Boolean fromUser)
{
    return NOERROR;
}

ECode CMainActivity::SeekBarChangedListener::OnStartTrackingTouch(
    /* [in] */ ISeekBar* seekBar)
{
    mActivity->mIsSeekBarMoving = TRUE;
    return NOERROR;
}

ECode CMainActivity::SeekBarChangedListener::OnStopTrackingTouch(
    /* [in] */ ISeekBar* seekBar)
{
    if (NULL != mActivity->mPlayController){
        Int32 progress;
        seekBar->GetProgress(&progress);

        mActivity->mPlayController->SeekTo(progress);
        mActivity->mSeekBarMusic->SetProgress(progress);
    }

    mActivity->mIsSeekBarMoving = FALSE;
    return NOERROR;
}

//=======================================================================
//              CMainActivity
//=======================================================================

CMainActivity::CMainActivity()
    : mIsSeekBarMoving(FALSE)
{
    Logger::V("CMainActivity", "CMainActivity(): %p", this);
}

CMainActivity::~CMainActivity()
{
    Logger::V("CMainActivity", "~CMainActivity(): %p", this);
}

ECode CMainActivity::InitView()
{
    AutoPtr<IView> view;

    mBtnAbout = FindViewById(R::main::title_btn_about);
    assert(mBtnAbout != NULL);

    mBtnSearch = FindViewById(R::main::title_btn_search);
    assert(mBtnSearch != NULL);

    mBtnEdit = FindViewById(R::main::tab_btn_edit);
    assert(mBtnEdit != NULL);

    view = FindViewById(R::main::foot_btn_play);
    assert(view != NULL);
    mBtnPlay = IButton::Probe(view);
    assert(mBtnPlay != NULL);

    view = FindViewById(R::main::foot_btn_pause);
    assert(view != NULL);
    mBtnPause = IButton::Probe(view);
    assert(mBtnPause != NULL);

    mLayoutTabRight = FindViewById(R::main::tab_layout_right);
    assert(mLayoutTabRight != NULL);

    mBtnAddPlaylist = FindViewById(R::main::tab_layout_add_playlist);
    assert(mBtnAddPlaylist != NULL);

    mLayoutPlayMode = FindViewById(R::main::tab_layout_play_mode);
    assert(mLayoutPlayMode != NULL);

    mLayoutAllLoop = FindViewById(R::main::tab_layout_all_loop);
    assert(mLayoutAllLoop != NULL);

    mLayoutSingleLoop = FindViewById(R::main::tab_layout_single_loop);
    assert(mLayoutSingleLoop != NULL);

    mLayoutRandomPlay = FindViewById(R::main::tab_layout_random_play);
    assert(mLayoutRandomPlay != NULL);

    mLayoutOrderPlay = FindViewById(R::main::tab_layout_order_play);
    assert(mLayoutOrderPlay != NULL);

    view = FindViewById(R::main::tab_btn_song);
    assert(view != NULL);
    mBtnTabSong = IRadioButton::Probe(view);
    assert(mBtnTabSong != NULL);

    view = FindViewById(R::main::tab_btn_singer);
    assert(view != NULL);
    mBtnTabSinger = IRadioButton::Probe(view);
    assert(mBtnTabSinger != NULL);

    view = FindViewById(R::main::tab_btn_album);
    assert(view != NULL);
    mBtnTabAlbum = IRadioButton::Probe(view);
    assert(mBtnTabAlbum != NULL);

    view = FindViewById(R::main::tab_btn_play_list);
    assert(view != NULL);
    mBtnTabPlayList = IRadioButton::Probe(view);
    assert(mBtnTabPlayList != NULL);

    view = FindViewById(R::main::content_listview);
    assert(view != NULL);
    mContentListView = IListView::Probe(view);
    assert(mContentListView != NULL);

    view = FindViewById(R::main::seekbar_music);
    assert(view != NULL);
    mSeekBarMusic = ISeekBar::Probe(view);
    assert(mSeekBarMusic != NULL);

    mLayoutFoot = FindViewById(R::main::foot_layout);
    assert(mLayoutFoot != NULL);

    view = FindViewById(R::main::foot_imgview_album_art);
    assert(view != NULL);
    mImgViewAlbumArt = IImageView::Probe(view);
    assert(mImgViewAlbumArt != NULL);

    view = FindViewById(R::main::foot_tv_song);
    assert(view != NULL);
    mTextViewSong = ITextView::Probe(view);
    assert(mTextViewSong != NULL);

    view = FindViewById(R::main::foot_tv_singer);
    assert(view != NULL);
    mTextViewArtist = ITextView::Probe(view);
    assert(mTextViewArtist != NULL);

    return NOERROR;
}

ECode CMainActivity::SetupListener()
{
    mBtnAbout->SetOnClickListener(mActionListener);
    mBtnSearch->SetOnClickListener(mActionListener);
    mBtnEdit->SetOnClickListener(mActionListener);
    mBtnPlay->SetOnClickListener(mActionListener);
    mBtnPause->SetOnClickListener(mActionListener);
    mLayoutTabRight->SetOnClickListener(mActionListener);
    mBtnAddPlaylist->SetOnClickListener(mActionListener);

    mBtnTabSong->SetOnCheckedChangeListener(mTabListener);
    mBtnTabSinger->SetOnCheckedChangeListener(mTabListener);
    mBtnTabAlbum->SetOnCheckedChangeListener(mTabListener);
    mBtnTabPlayList->SetOnCheckedChangeListener(mTabListener);

    mLayoutAllLoop->SetOnClickListener(mActionListener);
    mLayoutSingleLoop->SetOnClickListener(mActionListener);
    mLayoutRandomPlay->SetOnClickListener(mActionListener);
    mLayoutOrderPlay->SetOnClickListener(mActionListener);
    mLayoutFoot->SetOnClickListener(mActionListener);

    mSeekBarMusic->SetOnSeekBarChangeListener(mSeekBarListener);
    mContentListView->SetOnItemClickListener(mItemListener);

    return NOERROR;
}

PInterface CMainActivity::Probe(
    /* [in] */REIID riid)
{
    if (riid == EIID_IInterface) {
        return (IInterface*)(IActivity *)this;
    }
    else if (riid == EIID_IActivity) {
        return (IActivity *)this;
    }
    else if (riid == EIID_IServiceConnection) {
        return (IServiceConnection*)this;
    }

    return Activity::Probe(riid);
}

UInt32 CMainActivity::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CMainActivity::Release()
{
    return ElRefBase::Release();
}

ECode CMainActivity::GetInterfaceID(
    /* [in] */ IInterface* pObject,
    /* [out] */ InterfaceID* pIID)
{
    VALIDATE_NOT_NULL(pIID);

    if (pObject == (IInterface *)(IServiceConnection *)this) {
        *pIID = EIID_IServiceConnection;
        return NOERROR;
    }

    return Activity::GetInterfaceID(pObject, pIID);
}

ECode CMainActivity::GetClassID(
    /* [out] */ ClassID* clsid)
{
    if (NULL == clsid) {
        return E_INVALID_ARGUMENT;
    }

    *clsid = ECLSID_CMainActivity;
    return NOERROR;
}

ECode CMainActivity::OnTick(
    /* [in] */ Int32 progress)
{
    if (!mIsSeekBarMoving) {
        mSeekBarMusic->SetProgress(progress);
    }

    return NOERROR;
}

ECode CMainActivity::OnCreate(
/* [in] */IBundle* savedInstanceState)
{
    Logger::V("CMainActivity", "OnCreate");

    // MyDumpMemery();

    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::activity_main);

    assert(mOnPlayCompletionListener == NULL);
    AutoPtr<IWeakReference> wr;
    GetWeakReference((IWeakReference**)&wr);
    CMainActivityListener::New(wr, (IOnPlayCompletionListener**)&mOnPlayCompletionListener);
    mOnTickListner = IOnTickListener::Probe(mOnPlayCompletionListener);
    assert(mOnTickListner != NULL);

    mActionListener = new MyListener(this);
    mTabListener = new TabChangedListener(this);
    mSeekBarListener = new SeekBarChangedListener(this);
    mItemListener = new ItemOnClickListener(this);

    InitView();
    SetupListener();

    CMusicUtil::BindToService(IContext::Probe(this), IServiceConnection::Probe(this));

    mIsSeekBarMoving = FALSE;

    CArrayAdapter::New(this, R::layout::view_textview, (IArrayAdapter**)&mAdapter);

    return NOERROR;
}

ECode CMainActivity::SetCurrentAudioInfo()
{
    Logger::D("CMainActivity", "SetCurrentAudioInfo");
    if (NULL != mPlayController) {
        AutoPtr<ICharSequence> trackName;
        mPlayController->GetTrackName((ICharSequence**)&trackName);
        mTextViewSong->SetText(trackName);

        Int32 duration;
        mPlayController->GetDuration(&duration);
        mSeekBarMusic->SetMax(duration);

        Int32 progress;
        mPlayController->GetProgress(&progress);
        mSeekBarMusic->SetProgress(progress);

        Boolean isPlaying;
        mPlayController->IsPlaying(&isPlaying);
        SetPlayBtnStatus(isPlaying);

        Int32 mode;
        mPlayController->GetPlayMode(&mode);
        ++mode;
        if (mode >= IPlayController::MODE_INVALID) {
            mode = 0;
        }
        SetPlayModeStatus(mode);
    }
    else {
        mSeekBarMusic->SetProgress(0);
        mSeekBarMusic->SetMax(0);
    }

    return NOERROR;
}

ECode CMainActivity::InitStatus()
{
    Logger::D("CMainActivity", "InitStatus()");

    if (mPlayController == NULL) {
        Logger::D("CMainActivity", "mPlayController is NULL.");
        return NOERROR;
    }

    Boolean isPlaying;
    mPlayController->IsPlaying(&isPlaying);
    SetPlayBtnStatus(isPlaying);

    mDataList = NULL;
    mPlayController->GetAudioList(String("type"), (ArrayOf<IInterface*>**)&mDataList);
    mContentListView->SetAdapter(mAdapter);

    if (NULL != mDataList) {
        Logger::D("CMainActivity", "InitStatus setup mAdapter");
        mAdapter->AddAll(mDataList);
        mAdapter->NotifyDataSetChanged();

        mSeekBarMusic->SetVisibility(IView::VISIBLE);
        mLayoutFoot->SetVisibility(IView::VISIBLE);
    }
    else {
        Logger::D("CMainActivity", "Get mDataList = NULL");
        mSeekBarMusic->SetVisibility(IView::INVISIBLE);
        mLayoutFoot->SetVisibility(IView::INVISIBLE);
    }

    SetCurrentAudioInfo();

    return NOERROR;
}

ECode CMainActivity::OnResume()
{
    Logger::D("CMainActivity", "OnResume()");
    Activity::OnResume();
    SetCurrentAudioInfo();

    return NOERROR;
}

ECode CMainActivity::OnStop()
{
    Logger::D("CMainActivity", "OnStop()");
    Activity::OnStop();
    return NOERROR;
}

ECode CMainActivity::OnDestroy()
{
    Logger::D("CMainActivity", "OnDestroy()");
    Activity::OnDestroy();

    CMusicUtil::UnbindFromService(IContext::Probe(this));

    if (mPlayController != NULL) {
        mPlayController->RemoveOnTickListener((Int32)this);
        mPlayController->RemovePlayCompletionListener((Int32)this);
        mPlayController = NULL;
    }

    mAdapter = NULL;
    return NOERROR;
}

ECode CMainActivity::Run()
{
    Logger::D("CMainActivity", "Run()");

    return NOERROR;
}

ECode CMainActivity::OnCompletion()
{
    Logger::D("CMainActivity", "OnCompletion()");

    SetPlayBtnStatus(FALSE);
    mSeekBarMusic->SetProgress(0);

    if (NULL != mPlayController) {
        Int32 duration;
        mPlayController->GetDuration(&duration);
        mSeekBarMusic->SetMax(duration);

        AutoPtr<ICharSequence> trackName;
        mPlayController->GetTrackName((ICharSequence**)&trackName);
        mTextViewSong->SetText(trackName);

        Boolean playing;
        mPlayController->IsPlaying(&playing);
        SetPlayBtnStatus(playing);
    }

    return NOERROR;
}

ECode CMainActivity::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    Logger::D("CMusicActivity", "OnServiceConnected() %p, playerController %p", service, IPlayController::Probe(service));

    mPlayController = IPlayController::Probe(service);
    if (mPlayController == NULL) {
        Logger::E("CMusicActivity::OnServiceConnected", "mBinder is not a IPlayController.");
        return E_RUNTIME_EXCEPTION;
    }

    InitStatus();

#if 0
    // Make sure run it in UI thread
    AutoPtr<ILooperHelper> looperHelper;
    CLooperHelper::AcquireSingleton((ILooperHelper**)&looperHelper);
    AutoPtr<ILooper> looper;
    looperHelper->GetMainLooper((ILooper**)&looper);
    AutoPtr<IHandler> handler;
    CHandler::New(looper.Get(), (IHandler**)&handler);
    Boolean bRet;
    handler->Post(IRunnable::Probe(this), &bRet);
#endif

    if (mPlayController != NULL) {
        mPlayController->SetOnTickListener((Int32)this, mOnTickListner);
        mPlayController->SetPlayCompletionListener((Int32)this, mOnPlayCompletionListener);
    }

    return NOERROR;
}

ECode CMainActivity::OnServiceDisconnected(
    /* [in] */ IComponentName * pName)
{
    Logger::D("CMusicActivity", "OnServiceDisconnected()");
    return NOERROR;
}

ECode CMainActivity::SetPlayBtnStatus(
    /* [in] */ Boolean isPlaying)
{
    if (isPlaying) {
        mBtnPlay->SetVisibility(IView::GONE);
        mBtnPause->SetVisibility(IView::VISIBLE);
    }
    else {
        mBtnPlay->SetVisibility(IView::VISIBLE);
        mBtnPause->SetVisibility(IView::GONE);
    }

    return NOERROR;
}

ECode CMainActivity::SetPlayModeStatus(
    /* [in] */ Int32 mode)
{
    if (IPlayController::MODE_ALL_LOOP == mode) {
        mLayoutAllLoop->SetVisibility(IView::VISIBLE);
        mLayoutSingleLoop->SetVisibility(IView::INVISIBLE);
        mLayoutRandomPlay->SetVisibility(IView::INVISIBLE);
        mLayoutOrderPlay->SetVisibility(IView::INVISIBLE);
    }
    else if(IPlayController::MODE_SINGLE_LOOP == mode) {
        mLayoutAllLoop->SetVisibility(IView::INVISIBLE);
        mLayoutSingleLoop->SetVisibility(IView::VISIBLE);
        mLayoutRandomPlay->SetVisibility(IView::INVISIBLE);
        mLayoutOrderPlay->SetVisibility(IView::INVISIBLE);
    }
    else if(IPlayController::MODE_RANDOM_PLAY == mode) {
        mLayoutAllLoop->SetVisibility(IView::INVISIBLE);
        mLayoutSingleLoop->SetVisibility(IView::INVISIBLE);
        mLayoutRandomPlay->SetVisibility(IView::VISIBLE);
        mLayoutOrderPlay->SetVisibility(IView::INVISIBLE);
    }
    else if(IPlayController::MODE_ORDER_PLAY == mode) {
        mLayoutAllLoop->SetVisibility(IView::INVISIBLE);
        mLayoutSingleLoop->SetVisibility(IView::INVISIBLE);
        mLayoutRandomPlay->SetVisibility(IView::INVISIBLE);
        mLayoutOrderPlay->SetVisibility(IView::VISIBLE);
    }

    return NOERROR;
}

} // namespace MusicPlayer
