/**************************************************************************************
 * filename:        CMuiscService.h
 * description:     Declare functions which will be implemented in CMusicService.cpp
 * author:          kari.zhang
 *
 * modifications:
 *
 *
 **************************************************************************************/

#ifndef __CMUSICSERVICE_H__
#define __CMUSICSERVICE_H__

#include "_CMusicService.h"
#include <app/Service.h>
#include <ext/frameworkdef.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Core::IRunnable;
using Elastos::Core::IThread;
using Elastos::Utility::Etl::HashMap;
using Elastos::Droid::App::Service;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Media::EIID_IMediaPlayerOnCompletionListener;
using Elastos::Droid::Media::IMediaPlayer;
using Elastos::Droid::Media::IMediaPlayerOnCompletionListener;

namespace MusicPlayer {

class CMusicService
    : public IBinder
    , public IMediaPlayerOnCompletionListener
    , public IPlayController
    , public IRunnable
    , public Service
{
public:
    class RefreshRunnable
        : public ElRefBase
        , public IRunnable
    {
    public:
        RefreshRunnable(
            /* [in] */ CMusicService* service);

        CAR_INTERFACE_DECL()

        ECode Run();

        ECode Stop();

    private:
        CMusicService* mService;
        Boolean mRunFlag;
    };

public:
    CMusicService();

    ~CMusicService();

    CAR_INTERFACE_DECL()

    CARAPI GetClassID(
        /* [out] */ ClassID* clsid);

    ECode Run();

    CARAPI OnCreate();

    CARAPI OnStart(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 startId);

    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** binder);

    CARAPI OnUnbind(
        /* [in] */ IIntent* intent,
        /* [out] */ Boolean* succeeded);

    CARAPI OnRebind(
        /* [in] */ IIntent* intent);

    CARAPI OnDestroy();

    // open audio file by specified path
    CARAPI OpenFile(
        /* [in] */ const String& path);

    // open audio file by the specified position in play list
    CARAPI Open(
        /* [in] */ const ArrayOf<Int32>& list,
        /* [in] */ Int32 pos);

    CARAPI GetAudioId(
        /* [out] */ Int64* id);

    CARAPI SetAudioId(
        /* [in] */ Int64 id);

    CARAPI GetTrackName(
        /* [out] */ ICharSequence** name);

    CARAPI RemoveTrack(
        /* [in] */ Int64 id);

    CARAPI RemoveTracks(
        /* [in] */ Int32 first,
        /* [in] */ Int32 last);

    CARAPI GetAlbumId(
        /* [out] */ Int64* id);

    CARAPI GetAlubmName(
        /* [out] */ String* name);

    CARAPI GetAritistId(
        /* [out] */ Int64* id);

    CARAPI GetAritistName(
        /* [out] */ String* name);

    // enqueue
    CARAPI Enqueue(
        /* [in] */ const ArrayOf<Int32>& list,
        /* [in] */ Int32 action);

    // move a song from a queue
    CARAPI MoveQueueItem(
        /* [in] */ Int32 id);

    CARAPI GetQueue(
        /* [out, callee] */ ArrayOf<Int64>** queue);

    // chage the position of the queue
    CARAPI SetQueuePosition(
        /* [in] */ Int32 index);

    // get the position in the play list
    CARAPI GetQueuePostion(
        /* [out] */ Int32* pos);

    // get the status of player
    CARAPI IsPlaying(
        /* [out] */ Boolean* isPlaying);

    CARAPI Play();

    CARAPI Stop();

    // seek to the specified postion
    CARAPI SeekTo(
        /* [in] */ Int32 pos);

    // get the totoal time of the audio file
    CARAPI GetDuration(
        /* [out] */ Int32* duration);

    CARAPI Pause();

    CARAPI PlayNext();

    CARAPI PlayPrevious();

    CARAPI GetProgress(
        /* [out] */ Int32* progress);

    CARAPI GetAudioList(
        /* [in] */ const String& type,
        /* [out, callee] */ ArrayOf<IInterface*>** mDataList);

    CARAPI SetOnTickListener(
        /* [in] */ Int32 keyId,
        /* [in] */ IOnTickListener* tickListener);

    CARAPI OnCompletion(
        /* [in] */ IMediaPlayer* mp);

    CARAPI SetPlayCompletionListener(
        /* [in] */ Int32 keyId,
        /* [in] */ IOnPlayCompletionListener* listener);

    CARAPI RemoveOnTickListener(
    /* [in] */ Int32 keyId);

    CARAPI RemovePlayCompletionListener(
    /* [in] */ Int32 keyId);

    // set play mode
    CARAPI SetPlayMode(
        /* [in] */ Int32 mode);

    // get play mode
    CARAPI GetPlayMode(
        /* [out] */ Int32* mode);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI GetHashCode(
        /* [out] */ Int32* hash);

private:
    AutoPtr<ArrayOf<String> > GetMusicFileList();

private:
    AutoPtr<IMediaPlayer> mMediaPlayer;
    Boolean mReadyFlag;
    AutoPtr<IThread> mThread;
    AutoPtr<RefreshRunnable> mRunnable;
    String mFilePath;

    Int32 mPlayMode;
    Int64 mAudioId;

    HashMap<Int32, AutoPtr<IOnTickListener> > mTickListenerMap;
    HashMap<Int32, AutoPtr<IOnPlayCompletionListener> > mCompletionListenerMap;
    AutoPtr<IHandler> mTickListenerHandler;

    AutoPtr<ArrayOf<String> > mAudioList;
};

} //end namespace MusicPlayer
#endif // __CMUSICSERVICE_H__
