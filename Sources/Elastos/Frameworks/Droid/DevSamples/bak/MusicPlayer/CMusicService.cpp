/**************************************************************************************
 * filename:        CMuiscService.cpp
 *
 * description:     1. Play music in backstage
 *                  2. Define public functions for controlling playing music
 *
 * author:          kari.zhang
 *
 * modifications:
 *  1. Fixed runtime crash when call mMediaPlayer->SetOnCompletionListener(completionListener)
 *     in OnCreate().
 *     kari.zhang @ 2014-07-11
 *
 **************************************************************************************/
#include "CMusicService.h"
#include "CmdDefine.h"
#include "R.h"
#include <elastos/core/Thread.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringBuilder.h>
#include <dirent.h>

using Elastos::Utility::CRandom;
using Elastos::Core::CStringWrapper;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::IRandom;
using Elastos::Core::StringBuilder;
using Elastos::Core::Thread;
using Elastos::Core::CThread;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::App::EIID_IService;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::ISharedPreferencesEditor;
using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Content::EIID_IComponentCallbacks2;
using Elastos::Droid::Content::EIID_IContextWrapper;
using Elastos::Droid::Media::EIID_IMediaPlayerOnCompletionListener;
using Elastos::Droid::Media::CMediaPlayer;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::ILooperHelper;
using Elastos::Droid::Os::CLooperHelper;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Provider::IMediaStoreAudioAudioColumns;

// #define MEDIA_MUSIC_PATH    "/mnt/sdcard/Music/"
#define SHARE_PERF          "shpref_music_settings"
#define KEY_PLAY_MODE       "key_play_mode"
#define KEY_AUDIO_ID        "key_audio_id"

namespace MusicPlayer {

static const String MUSIC_SUPPORTED_LIST[] = {
    String(".mp3"), String(".wav"), String(".aac"), String(".ogg"),
    String(".mp4"), String(".m4a"), String(".3gp"), String(".flac"),
    String(".mid"), String(".xmf"), String(".mxmf"),String(".rtttl"),
    String(".rtx"), String(".ota"), String(".imy"), String(".mkv"),
    String("amr"),  String("wma")
};

static Boolean IsFileSupported(
    /* [in] */ const String& fileName)
{
    for (UInt32 i = 0; i < ArraySize(MUSIC_SUPPORTED_LIST); ++i) {
        if (fileName.EndWithIgnoreCase(MUSIC_SUPPORTED_LIST[i])) {
            return TRUE;
        }
    }

    return FALSE;
}

// static AutoPtr<ArrayOf<String> > GetMusicFileList()
// {
//     DIR* dir = opendir(MEDIA_MUSIC_PATH);
//     if (dir == NULL) {
//         Logger::E("CMusicService", "Failed read dir:%s\n", MEDIA_MUSIC_PATH);
//         return NULL;
//     }

//     List<String> mp3List;
//     struct dirent* ptr = NULL;
//     while ((ptr = readdir(dir)) != NULL) {
//         String fileName(ptr->d_name);
//         if (IsFileSupported(fileName)) {
//             StringBuilder sb(MEDIA_MUSIC_PATH);
//             sb.Append(fileName);
//             mp3List.PushBack(sb.ToString());
//         }
//     }
//     closedir(dir);
//     dir = NULL;

//     AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(mp3List.GetSize());
//     List<String>::Iterator it = mp3List.Begin();
//     for (Int32 i = 0; it != mp3List.End(); ++it, ++i) {
//         array->Set(i, *it);
//         Logger::D("CMusicService", " >> %d, %s", i, (*it).string());
//     }

//     return array;
// }

//=======================================================================
//              CMusicService::RefreshRunnable
//=======================================================================
CAR_INTERFACE_IMPL(CMusicService::RefreshRunnable, IRunnable)

CMusicService::RefreshRunnable::RefreshRunnable(
    /* [in] */ CMusicService* service)
    : mService(service)
    , mRunFlag(TRUE)
{
}

ECode CMusicService::RefreshRunnable::Run()
{
    Boolean isPlaying = FALSE;
    Boolean bRet;
    while (mRunFlag) {
        mService->mMediaPlayer->IsPlaying(&isPlaying);
        if (isPlaying) {
            mService->mTickListenerHandler->Post(IRunnable::Probe(mService), &bRet);
        }

        Thread::Sleep(300);
    }

    return NOERROR;
}

ECode CMusicService::RefreshRunnable::Stop()
{
    mRunFlag = FALSE;
    return NOERROR;
}

//=======================================================================
//              CMusicService
//=======================================================================
CMusicService::CMusicService()
    : mReadyFlag(FALSE)
    , mPlayMode(IPlayController::MODE_ALL_LOOP)
    , mAudioId(-1)
{
    Logger::V("CMusicService", "Create CMusicService() %p", this);
}

CMusicService::~CMusicService()
{
    Logger::V("CMusicService", "Destory ~CMusicService() %p", this);
}

PInterface CMusicService::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(IService*)this;
    }
    else if (riid == EIID_IObject) {
        return (IObject*)this;
    }
    if (riid == EIID_IService) {
        return (IService*)this;
    }
    else if (riid == EIID_IContextWrapper) {
       return (IContextWrapper*)this;
    }
    else if (riid == EIID_IContext) {
       return (IContext*)this;
    }
    else if (riid == EIID_IComponentCallbacks2) {
       return (IComponentCallbacks2*)this;
    }
    else if (riid == EIID_IWeakReferenceSource) {
        return (IWeakReferenceSource*)this;
    }
    else if ( riid == EIID_IPlayController) {
        return (IPlayController *)this;
    }
    else if (riid == EIID_IBinder) {
        return (IBinder*)this;
    }
    else if (riid == EIID_IService) {
        return (IService*)this;
    }
    else if (EIID_IRunnable == riid) {
        return (IRunnable*)this;
    }
    else if (EIID_IMediaPlayerOnCompletionListener == riid) {
        return (IMediaPlayerOnCompletionListener*)this;
    }
    else if (EIID_IWeakReferenceSource == riid) {
        return (IWeakReferenceSource*)this;
    }

    return Service::Probe(riid);
}

UInt32 CMusicService::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CMusicService::Release()
{
    return ElRefBase::Release();
}

ECode CMusicService::GetInterfaceID(
    /* [in] */ IInterface* pObject,
    /* [out] */ InterfaceID* pIID)
{
    VALIDATE_NOT_NULL(pIID);

    if (pObject == (IInterface *)(IService *)this) {
        *pIID = EIID_IService;
        return NOERROR;
    }
    else if (pObject == (IInterface *)(IObject *)this) {
        *pIID = EIID_IObject;
        return NOERROR;
    }
    else if (pObject == (IContextWrapper *)(IObject *)this) {
        *pIID = EIID_IContextWrapper;
        return NOERROR;
    }
    else if (pObject == (IContext *)(IObject *)this) {
        *pIID = EIID_IContext;
        return NOERROR;
    }
    else if (pObject == (IComponentCallbacks2 *)(IObject *)this) {
        *pIID = EIID_IComponentCallbacks2;
        return NOERROR;
    }
    else if (pObject == (IInterface *)(IWeakReferenceSource *)this) {
        *pIID = EIID_IWeakReferenceSource;
        return NOERROR;
    }
    else if (pObject == (IInterface *)(IPlayController *)this) {
        *pIID = EIID_IPlayController;
        return NOERROR;
    }
    else if (pObject == (IInterface *)(IBinder *)this) {
        *pIID = EIID_IBinder;
        return NOERROR;
    }
    else if (pObject == (IInterface *)(IRunnable *)this) {
        *pIID = EIID_IRunnable;
        return NOERROR;
    }
    else if (pObject == (IInterface *)(IMediaPlayerOnCompletionListener *)this) {
        *pIID = EIID_IMediaPlayerOnCompletionListener;
        return NOERROR;
    }

    return Service::GetInterfaceID(pObject, pIID);
}

ECode CMusicService::GetClassID(
    /* [out] */ ClassID* clsid)
{
    if (NULL == clsid) return E_INVALID_ARGUMENT;

    *clsid = ECLSID_CMusicService;
    return NOERROR;
}

ECode CMusicService::OnCreate()
{
    Logger::V("CMusicService", "OnCreate()");
    assert(mMediaPlayer == NULL);
    ECode ec = CMediaPlayer::New((IMediaPlayer**)&mMediaPlayer);
    if (FAILED(ec) || mMediaPlayer == NULL) {
        Logger::E("CMusicService", "Failed Create MediaPlayer");
        return NOERROR;
    }

    AutoPtr<IMediaPlayerOnCompletionListener> completionListener = IMediaPlayerOnCompletionListener::Probe(this);
    if (NULL == completionListener) {
        Logger::E("CMusicService", "Failed probe IMediaPlayerOnCompletionListener");
        return E_RUNTIME_EXCEPTION;
    }
    mMediaPlayer->SetOnCompletionListener(completionListener);

    mRunnable = new RefreshRunnable(this);
    CThread::New(mRunnable, (IThread**)&mThread);
    mThread->Start();

#if 0
    AutoPtr<ISharedPreferences> shPref = NULL;
    ECode ec = GetSharedPreferences(String(SHARE_PERF), 0, (ISharedPreferences**)&shPref);
    if (FAILED(ec) || NULL == shPref) {
        Logger::D("CMusicService", "OnCreate() Failed GetSharedPreference");
        return E_NULL_POINTER_EXCEPTION;
    }

    Int64 id;
    ec = shPref->GetInt64(String(KEY_AUDIO_ID), -1, &id);
    if (FAILED(ec)) {
        Logger::D("CMusicService", "OnCreate() Failed Get KEY_AUDIO_ID");
        return E_RUNTIME_EXCEPTION;
    }

    Logger::D("CMusicService", "OnCreate() Get AudioId=%d", id);
    SetAudioId(id);


    Int32 mode;
    ec = shPref->GetInt32(String(KEY_PLAY_MODE), MODE_ALL_LOOP, &mode);
    if (FAILED(ec)) {
        Logger::D("CMusicService", "OnCreate() Failed Get KEY_PLAY_MODE");
        return E_RUNTIME_EXCEPTION;
    }

    Logger::D("CMusicService", "OnCreate() Get PlayMode=%d", mode);
    SetPlayMode(mode);
#endif

    AutoPtr<ILooperHelper> looperHelper;
    CLooperHelper::AcquireSingleton((ILooperHelper**)&looperHelper);
    AutoPtr<ILooper> looper;
    looperHelper->GetMainLooper((ILooper**)&looper);
    CHandler::New(looper, (IHandler**)&mTickListenerHandler);

    mAudioList = GetMusicFileList();
    if (mAudioList == NULL || mAudioList->GetLength() == 0) {
        AutoPtr<IToastHelper> toastHelper;
        CToastHelper::AcquireSingleton((IToastHelper**)&toastHelper);

        StringBuilder sb("Not Found Audio Files");
        AutoPtr<ICharSequence> seq = sb.ToCharSequence();

        AutoPtr<IToast> toast;
        toastHelper->MakeText(this, seq, IToast::LENGTH_LONG, (IToast**)&toast);
        toast->Show();

        Logger::E("CMusicService", "MusicService Not Found audio files");
    }

    return NOERROR;
}

ECode CMusicService::OnStart(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 startId)
{
    Logger::D("CMusicService", "OnStart()");

    return NOERROR;
}

ECode CMusicService::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** binder)
{
    VALIDATE_NOT_NULL(binder);
    *binder = this;
    REFCOUNT_ADD(*binder);

    Logger::D("CMusicService", "OnBind() %d", *binder);
    return NOERROR;
}

ECode CMusicService::OnUnbind(
    /* [in] */ IIntent* intent,
    /* [out] */ Boolean* succeeded)
{
    Logger::D("CMusicService", "OnUnbind()");

    return NOERROR;
}

ECode CMusicService::OnRebind(
    /* [in] */ IIntent* intent)
{
    Logger::D("CMusicService", "OnRebind()");

    return NOERROR;
}

ECode CMusicService::OnDestroy()
{
    Logger::D("CMusicService", "OnDestroy()");

    if (mMediaPlayer) {
        mMediaPlayer->SetOnCompletionListener(NULL);
        mMediaPlayer->ReleaseResources();
        mMediaPlayer = NULL;
    }
    mRunnable->Stop();

#if 0
    AutoPtr<ISharedPreferences> shPref;
    ECode ec = GetSharedPreferences(String(SHARE_PERF), 0, (ISharedPreferences**)&shPref);
    if (FAILED(ec)) {
        Logger::D("CMusicService", "OnDestroy() Failed GetSharedPreferences");
        return E_RUNTIME_EXCEPTION;
    }

    AutoPtr<ISharedPreferencesEditor> editor;
    ec = shPref->Edit((ISharedPreferencesEditor**)&editor);
    if (FAILED(ec)) {
        Logger::D("CMusicService", "OnDestroy() Failed GetSharedPreferencesEditor");
        return E_RUNTIME_EXCEPTION;
    }

    editor->PutInt64(String(KEY_AUDIO_ID), mAudioId);
    Boolean result;
    editor->Commit(&result);
    Logger::D("CMusicService", "OnDestroy() SaveAudioId:id=%lld, result=%d", mAudioId, result);

#endif

    return NOERROR;
}

// open audio file by specified path
ECode CMusicService::OpenFile(
    /* [in] */ const String& path)
{
    Logger::D("CMusicService", "OpenFile() %s", path.string());

    mMediaPlayer->Reset();
    mMediaPlayer->SetDataSource(path);
    mMediaPlayer->Prepare();
    mMediaPlayer->Start();
    mFilePath = path;
    mReadyFlag = TRUE;

    return NOERROR;
}

// open audio file by the specified position in play list
ECode CMusicService::Open(
    /* [in] */ const ArrayOf<Int32>& list,
    /* [in] */ Int32 pos)
{
    return E_NOT_IMPLEMENTED;
}

ECode CMusicService::GetAudioId(
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id);
    Logger::D("CMusicService", "GetAudioId()");
    *id = mAudioId;
    return NOERROR;
}

ECode CMusicService::SetAudioId(
    /* [in] */ Int64 id)
{
    Logger::D("CMusicService", "SetAudioId()");

    if (NULL == mAudioList) {
        Logger::E("CMusicService", "No audio files found");
        return E_RUNTIME_EXCEPTION;

    }

    if (id < 0 || id >= mAudioList->GetLength() ) {
        Logger::E("CMusicService", "Invalid value id");
        return E_RUNTIME_EXCEPTION;
    }

    mAudioId = id;
    String fileName = (*mAudioList)[id];
    OpenFile(fileName);

    return NOERROR;
}

ECode CMusicService::GetTrackName(
    /* [out] */ ICharSequence** name)
{
    VALIDATE_NOT_NULL(name);
    *name = NULL;
    Logger::D("CMusicService", "GetTrackName()");

    if (!mFilePath.IsNull()) {
        // Int32 start = CString(MEDIA_MUSIC_PATH).GetLength();
        Int32 start = mFilePath.LastIndexOf('/') + 1;
        String path = mFilePath.Substring(start);
        CStringWrapper::New(path, name);
        Logger::D("CMusicService", "GetTrackName , mFilePath = %s, name = %s", mFilePath.string(), path.string());
        return NOERROR;
    }

    if (mAudioList == NULL) {
        Logger::E("CMusicService", "No audio files found!");
        return E_RUNTIME_EXCEPTION;
    }

    if (mAudioId >= 0 && mAudioId < mAudioList->GetLength()) {
        // Int32 start = CString(MEDIA_MUSIC_PATH).GetLength();
        Int32 start = (*mAudioList)[mAudioId].LastIndexOf('/') + 1;
        String fileName = (*mAudioList)[mAudioId].Substring(start);
        Logger::D("CMusicService", "GetTrackName , file = %s, name = %s", (*mAudioList)[mAudioId].string(), fileName.string());
        CStringWrapper::New(fileName, name);
    }
    else {
        Logger::E("CMusicService", "Invalid mAudioId");
    }

    return NOERROR;
}

ECode CMusicService::RemoveTrack(
    /* [in] */ Int64 id)
{
    return E_NOT_IMPLEMENTED;
}

ECode CMusicService::RemoveTracks(
    /* [in] */ Int32 first,
    /* [in] */ Int32 last)
{
    return E_NOT_IMPLEMENTED;
}

ECode CMusicService::GetAlbumId(
    /* [out] */ Int64* id)
{
    return E_NOT_IMPLEMENTED;
}

ECode CMusicService::GetAlubmName(
    /* [out] */ String* name)
{
    return E_NOT_IMPLEMENTED;
}

ECode CMusicService::GetAritistId(
    /* [out] */ Int64* id)
{
    return E_NOT_IMPLEMENTED;
}

ECode CMusicService::GetAritistName(
    /* [out] */ String* name)
{
    return E_NOT_IMPLEMENTED;
}

// enqueue
ECode CMusicService::Enqueue(
    /* [in] */ const ArrayOf<Int32>& list,
    /* [in] */ Int32 action)
{
    return E_NOT_IMPLEMENTED;
}

// move a song from a queue
ECode CMusicService::MoveQueueItem(
    /* [in] */ Int32 id)
{
    return E_NOT_IMPLEMENTED;
}

ECode CMusicService::GetQueue(
    /* [out, callee] */ ArrayOf<Int64>** queue)
{
    return E_NOT_IMPLEMENTED;
}

// chage the position of the queue
ECode CMusicService::SetQueuePosition(
    /* [in] */ Int32 index)
{
    return E_NOT_IMPLEMENTED;
}

// get the position in the play list
ECode CMusicService::GetQueuePostion(
    /* [out] */ Int32* pos)
{
    return E_NOT_IMPLEMENTED;
}

// get the status of player
ECode CMusicService::IsPlaying(
    /* [in] */ Boolean* isPlaying)
{
    VALIDATE_NOT_NULL(isPlaying);
    Logger::D("CMusicServce", "IsPlaying()");
    mMediaPlayer->IsPlaying(isPlaying);

    return NOERROR;
}

// seek to the specified postion
ECode CMusicService::SeekTo(
    /* [in] */ Int32 pos)
{
    Logger::D("CMusicService", "SeekTo()", pos);
    mMediaPlayer->SeekTo(pos);

    return NOERROR;
}

// get the totoal time of the audio file
ECode CMusicService::GetDuration(
    /* [out] */ Int32* duration)
{
    VALIDATE_NOT_NULL(duration);
    Logger::D("CMusicService", "GetDuration()");

    // Record the duration last time when ISharedPreferences is available
    if (mReadyFlag) {
        mMediaPlayer->GetDuration(duration);
    }
    else {
        Logger::D("CMusicService", "mReadyFlag = FALSE");
        *duration = 0;
    }

    return NOERROR;
}

ECode CMusicService::Play()
{
    Logger::D("CMusicService", "Play()");
    if (!mReadyFlag) {
        SetAudioId(mAudioId);
    }
    mMediaPlayer->Start();

    return NOERROR;
}

ECode CMusicService::Stop()
{
    Logger::D("CMusicService", "Stop()");
    mMediaPlayer->Stop();
    mReadyFlag = FALSE;

    return NOERROR;
}

ECode CMusicService::Pause()
{
    Logger::D("CMusicService", "Pause()");
    Boolean isPlaying;
    mMediaPlayer->IsPlaying(&isPlaying);
    if (isPlaying) {
        mMediaPlayer->Pause();
    }
    else {
        Logger::D("CMuiscService", "MediaPlayer is not playing.");
    }

    return NOERROR;
}

ECode CMusicService::PlayNext()
{
    VALIDATE_NOT_NULL(mAudioList);
    Logger::D("CMusicService", "PlayNext()");
    ++mAudioId;
    if (mAudioId >= mAudioList->GetLength()) {
        mAudioId = 0;
    }
    SetAudioId(mAudioId);

    return NOERROR;
}

ECode CMusicService::PlayPrevious()
{
    VALIDATE_NOT_NULL(mAudioList);
    Logger::D("CMusicService", "PlayNext()");
    --mAudioId;
    if (mAudioId < 0) {
        mAudioId = mAudioList->GetLength() - 1;
    }
    SetAudioId(mAudioId);

    return NOERROR;
}

ECode CMusicService::GetProgress(
    /* [out] */ Int32* progress)
{
    VALIDATE_NOT_NULL(progress);

    if (!mReadyFlag) {
        Logger::D("CMusicService", "mReadyFlag = FALSE");
        *progress = 0;
        return NOERROR;
    }

    Int32 duration, curation;
    mMediaPlayer->GetCurrentPosition(&curation);
    mMediaPlayer->GetDuration(&duration);
    if(curation > duration) {
        curation = duration;
    }
    *progress = curation;

    return NOERROR;
}

ECode CMusicService::GetAudioList(
    /* [in] */ const String& type,
    /* [out, callee] */ ArrayOf<IInterface*>** result)
{
    VALIDATE_NOT_NULL(result);

    Logger::D("CMusicService", "GetAudioList()");
    *result = NULL;
    if (mAudioList == NULL) {
        StopSelf();
        Logger::D("CMusicSerivce", "mAudioList = NULL");
        return E_NULL_POINTER_EXCEPTION;
    }

    // Int32 size = mAudioList->GetLength();
    // AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(size);

    // Int32 start = strlen(MEDIA_MUSIC_PATH);
    // String str;
    // for (Int32 i = 0; i< size; ++i) {
    //     str = (*mAudioList)[i].Substring(start);
    //     AutoPtr<ICharSequence> seq;
    //     CStringWrapper::New(str, (ICharSequence**)&seq);
    //     array->Set(i, seq);
    //     Logger::D("CMusicService::GetAudioList", " >> %d, %s", i, str.string());
    // }

    // *result = array;
    // REFCOUNT_ADD(*result);
    // return NOERROR;

    AutoPtr<IContentResolver> cr;
    GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ArrayOf<String> > projections = ArrayOf<String>::Alloc(2);
    projections->Set(0, IMediaStoreAudioAudioColumns::ID);
    projections->Set(1, IMediaStoreAudioAudioColumns::DISPLAY_NAME);
    AutoPtr<ICursor> c;
    AutoPtr<IUri> uri;
    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    helper->Parse(String("content://media/external/audio/media"), (IUri**)&uri);

    cr->Query(uri, projections, String(NULL), NULL, String(NULL), (ICursor**)&c);
    Boolean b;
    AutoPtr<ArrayOf<IInterface*> > array;
    if (c != NULL) {
        Int32 count;
        c->GetCount(&count);
        array = ArrayOf<IInterface*>::Alloc(count);
        for(Int32 i = 0; i < count; i++) {
            c->MoveToNext(&b);
            if(!b) break;
            Int32 id;
            String name;
            c->GetInt32(0, &id);
            c->GetString(1, &name);
            AutoPtr<ICharSequence> seq;
            CStringWrapper::New(name, (ICharSequence**)&seq);
            array->Set(i, seq);
            Logger::D("CMusicService", " Media id = %d, name = %s",id,name.string());
        }
    }
    else {
        return NULL;
    }
    if (c != NULL) {
        c->Close();
    }

    *result = array;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMusicService::SetOnTickListener(
    /* [in] */ Int32 keyId,
    /* [in] */ IOnTickListener* tickListener)
{
    VALIDATE_NOT_NULL(tickListener);
    Logger::D("CMusicService", "SetOnTickListener()");

    AutoPtr<IOnTickListener> listener = tickListener;
    mTickListenerMap[keyId] = listener;

    return NOERROR;
}

ECode CMusicService:: OnCompletion(
    /* [in] */ IMediaPlayer* mp)
{
    VALIDATE_NOT_NULL(mAudioList);
    Logger::D("CMusicService", "OnCompletion()");
    AutoPtr<IRandom> rnd;

    switch (mPlayMode) {
        case IPlayController::MODE_ALL_LOOP:
            ++mAudioId;
            if (mAudioId >= mAudioList->GetLength() ) {
                mAudioId = 0;
            }
            SetAudioId(mAudioId);
            break;

        case IPlayController::MODE_ORDER_PLAY:
            if (mAudioId + 1 < mAudioList->GetLength() ) {
                SetAudioId(++mAudioId);
            }
            else {
                Stop();
            }

            break;

        case IPlayController::MODE_RANDOM_PLAY:
            CRandom::New((IRandom**)&rnd);
            Int32 id;
            rnd->NextInt32(mAudioList->GetLength(), &id);
            Logger::D("CMusicService", "random play, random id=%d\n", id);
            SetAudioId(id);
            break;

        case IPlayController::MODE_SINGLE_LOOP:
            SetAudioId(mAudioId);
            break;
    }

    HashMap<Int32, AutoPtr<IOnPlayCompletionListener> >::Iterator it;
    for (it = mCompletionListenerMap.Begin(); it != mCompletionListenerMap.End(); ++it) {
        AutoPtr<IOnPlayCompletionListener> listener = it->mSecond;
        if (listener) {
            listener->OnCompletion();
        }
    }
    return NOERROR;
}

ECode CMusicService::SetPlayCompletionListener(
    /* [in] */ Int32 keyId,
    /* [in] */ IOnPlayCompletionListener* listener)
{
    VALIDATE_NOT_NULL(listener);
    Logger::D("CMusicService", "SetPlayCompletionListener()");

    AutoPtr<IOnPlayCompletionListener> compListener = listener;
    mCompletionListenerMap[keyId] = compListener;

    return NOERROR;
}

ECode CMusicService::RemoveOnTickListener(
    /* [in] */ Int32 keyId)
{
    Logger::D("CMusicService", "RemoveOnTickListener()");
    mTickListenerMap.Erase(keyId);
    return NOERROR;
}

ECode CMusicService::RemovePlayCompletionListener(
    /* [in] */ Int32 keyId)
{
    Logger::D("CMusicService", "RemovePlayCompletionListener()");
    mCompletionListenerMap.Erase(keyId);
    return NOERROR;
}

// set play mode
ECode CMusicService::SetPlayMode(
    /* [in] */ Int32 mode)
{
    Logger::D("CMusicService", "SetPlayMode()");

    switch (mode) {
        case MODE_SINGLE_LOOP:
            Logger::D("CMusicService", "mode: single loop");
            break;

        case MODE_ALL_LOOP:
            Logger::D("CMusicService", "mode: all loop");
            break;

        case MODE_RANDOM_PLAY:
            Logger::D("CMusicService", "mode: random play");
            break;

        case MODE_ORDER_PLAY:
            Logger::D("CMusicService", "mode: order play");
            break;

        default:
            Logger::E("CMusicService", "mode: invalid mode");
    }

    mPlayMode = mode;

#if 0
            AutoPtr<ISharedPreferences> shPref;
            ECode ecRet = GetSharedPreferences(String(SHARE_PERF), 0, (ISharedPreferences**)&shPref);
            assert(NOERROR == ecRet);

            AutoPtr<ISharedPreferencesEditor> editor;
            ecRet = shPref->Edit((ISharedPreferencesEditor**)&editor);
            assert(NOERROR == ecRet);

            editor->PutInt32(String(KEY_PLAY_MODE), mode);
            Boolean result;
            editor->Commit(&result);

            String str("SetPlayMode:");
            str.AppendFormat("mode=%d, result=%d", mode, result);
            Logger::D("CMusicService", str);

#endif

    return NOERROR;
}

// get play mode
ECode CMusicService::GetPlayMode(
    /* [out] */ Int32* mode)
{
    VALIDATE_NOT_NULL(mode);
    Logger::D("CMusicService", "GetPlayMode()");
    switch (mPlayMode) {
        case MODE_SINGLE_LOOP:
            Logger::D("CMusicService", "mode: single loop");
            break;

        case MODE_ALL_LOOP:
            Logger::D("CMusicService", "mode: all loop");
            break;

        case MODE_RANDOM_PLAY:
            Logger::D("CMusicService", "mode: random play");
            break;

        case MODE_ORDER_PLAY:
            Logger::D("CMusicService", "mode: order play");
            break;

        default:
            Logger::E("CMusicService", "mode: invalid mode");
    }

    *mode = mPlayMode;
    return NOERROR;
}

ECode CMusicService::Run()
{
    Int32 pos;
    GetProgress(&pos);
    HashMap<Int32, AutoPtr<IOnTickListener> >::Iterator it;
    for (it = mTickListenerMap.Begin(); it != mTickListenerMap.End(); ++it) {
        AutoPtr<IOnTickListener> listener = it->mSecond;
        if (listener) {
            listener->OnTick(pos);
        }
    }

    return NOERROR;
}

ECode CMusicService::ToString(
    /* [out] */ String* str)
{
    return NOERROR;
}

ECode CMusicService::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash);
    *hash = (Int32)this;
    return NOERROR;
}

AutoPtr<ArrayOf<String> > CMusicService::GetMusicFileList()
{
    Logger::D("CMusicService", "===GetMusicFileList===");
    AutoPtr<IContentResolver> cr;
    GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ArrayOf<String> > projections = ArrayOf<String>::Alloc(2);
    projections->Set(0, IMediaStoreAudioAudioColumns::ID);
    projections->Set(1, IMediaStoreAudioAudioColumns::DATA);
    AutoPtr<ICursor> c;
    AutoPtr<IUri> uri;
    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    helper->Parse(String("content://media/external/audio/media"), (IUri**)&uri);
    cr->Query(uri, projections, String(NULL), NULL, String(NULL), (ICursor**)&c);

    Boolean b;
    AutoPtr<ArrayOf<String> > array;
    if (c != NULL) {
        Int32 count;
        c->GetCount(&count);
        array = ArrayOf<String>::Alloc(count);
        for(Int32 i = 0; i < count; i++) {
            c->MoveToNext(&b);
            if(!b) break;
            Int32 id;
            String path;
            c->GetInt32(0, &id);
            c->GetString(1, &path);
            array->Set(i, path);
            Logger::D("CMusicService", " Media id = %d, data = %s",id,path.string());
        }
    }
    else {
        return NULL;
    }
    if (c != NULL) {
        c->Close();
    }

    return array;
}

} // namespace MusicPlayer
