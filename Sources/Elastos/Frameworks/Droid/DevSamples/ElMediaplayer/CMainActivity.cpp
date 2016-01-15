
#include "CMainActivity.h"
#include <stdio.h>
//#include <tstcom.h>
#include <unistd.h>
#include <utils/Timers.h>

typedef struct
{
    CMainActivity* mActivity;
    String filename;
    int bufferSize;
} PlayAudioParams;

Boolean s_beQuit = true;
sem_t g_sem;


void* CMainActivity::EntryRoutine(void *arg)
{
    CMainActivity* activity = (CMainActivity*)arg;
    do {
        activity->PostcppCallback();
        sem_wait(&g_sem);
    } while (!s_beQuit);

    return reinterpret_cast<void*>(0);
}

ECode CMainActivity::HandleMessage()
{
    int TEST_SR = 44100;
    int TEST_CONF = IAudioFormat::CHANNEL_OUT_STEREO;
    int TEST_FORMAT = IAudioFormat::ENCODING_PCM_16BIT;
    int TEST_MODE = IAudioTrack::MODE_STREAM;
    int TEST_STREAM_TYPE = IAudioManager::STREAM_MUSIC;
    ECode ec = NOERROR;

    int minBufferSize  = 0;
    ec = mAudioTrackHelper->GetMinBufferSize(TEST_SR, TEST_CONF, TEST_FORMAT, &minBufferSize);
    assert(ec == NOERROR);

    ec = CAudioTrack::New(TEST_STREAM_TYPE, TEST_SR, TEST_CONF, TEST_FORMAT, minBufferSize, TEST_MODE, (IAudioTrack**)&mAudioTrack);
    assert(ec == NOERROR);

    mAudioTrack->Play();

    String filename("/data/data/com.elastos.runtime/elastos/ElMediaplayer/data/10s.wav");
    //String filename("/data/data/com.elastos.runtime/elastos/ElMediaplayer/data/rainbow.mp3");
    PlayAudio(filename, minBufferSize);
    mAudioTrack->Stop();

    sem_post(&g_sem);
    return NOERROR;
}

ECode CMainActivity::PostcppCallback()
{
    ECode (STDCALL CMainActivity::*pHandlerFunc)();
    pHandlerFunc = &CMainActivity::HandleMessage;

    Boolean result = FALSE;
    return mDefaultHandler->PostCppCallback((Handle32)this, *(Handle32*)&pHandlerFunc, NULL, 0, FALSE, &result);
}

ECode CMainActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    SetContentView(0x7f030000);

    ECode ec = CAudioTrackHelper::AcquireSingleton((IAudioTrackHelper**)&mAudioTrackHelper);

    sem_init(&g_sem, 0, 0);

    CHandler::New((IHandler**)&mDefaultHandler);

    pthread_t   threadMessage;
    pthread_create(&threadMessage, NULL, EntryRoutine, (void*)this);

    return ec;
}

ECode CMainActivity::NewAudioTest(int streamType, int mode, int config, int format, int stateForMode, int channelCount)
{
    AutoPtr<IAudioTrack> audioTrackTest;
    ECode ec = NOERROR;

    int nSampleRates[] = {8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000};
    int nStateGet, nSampleGet = 0, nStreamTypeGet = 0, nChannelCountGet = 0;

    for (int i = 0; i < 9; i++) {
        int minBufferSize  = 0;
        ec = mAudioTrackHelper->GetMinBufferSize(nSampleRates[i], config, format, &minBufferSize);
        assert(ec == NOERROR);

        ec = CAudioTrack::New(streamType, nSampleRates[i], config, format, minBufferSize * 4, mode, (IAudioTrack**)&mAudioTrack);
        assert(ec == NOERROR);

        ec = mAudioTrack->GetSampleRate(&nSampleGet);
        assert(ec == NOERROR);
        assert(nSampleGet == nSampleRates[i]);
        ec = mAudioTrack->GetStreamType(&nStreamTypeGet);
        assert(ec == NOERROR);
        assert(nStreamTypeGet == streamType);
        ec = mAudioTrack->GetChannelCount(&nChannelCountGet);
        assert(ec == NOERROR);
        assert(nChannelCountGet == channelCount);
        ec = mAudioTrack->GetState(&nStateGet);
        assert(ec == NOERROR);
        assert(nStateGet == stateForMode);

        mAudioTrack = NULL;
    }

    return NOERROR;
}

ECode CMainActivity::CreateAudio(int *pBuffersize)
{
    int TEST_SR = 44100;
    int TEST_CONF = IAudioFormat::CHANNEL_OUT_STEREO;
    int TEST_FORMAT = IAudioFormat::ENCODING_PCM_16BIT;
    int TEST_MODE = IAudioTrack::MODE_STREAM;
    int TEST_STREAM_TYPE = IAudioManager::STREAM_MUSIC;
    ECode ec = NOERROR;

    int minBufferSize  = 0;
    ec = mAudioTrackHelper->GetMinBufferSize(TEST_SR, TEST_CONF, TEST_FORMAT, &minBufferSize);
    assert(ec == NOERROR);

    ec = CAudioTrack::New(TEST_STREAM_TYPE, TEST_SR, TEST_CONF, TEST_FORMAT, minBufferSize * 4, TEST_MODE, (IAudioTrack**)&mAudioTrack);
    assert(ec == NOERROR);

    mAudioTrack->Play();

    if (pBuffersize) *pBuffersize = minBufferSize * 4;

    return ec;
}

void* CMainActivity::PlayAudioThread(void *arg)
{
    PlayAudioParams* paramsArg = reinterpret_cast<PlayAudioParams*>(arg);
    CMainActivity* activity = paramsArg->mActivity;
    String filename = paramsArg->filename;
    int bufferSize = paramsArg->bufferSize;

    activity->PlayAudio(filename, bufferSize);

    return reinterpret_cast<void*>(0);
}

ECode CMainActivity::PlayAudio(String filename, int bufferSize)
{
    AutoPtr<IFileInputStream> pFIS;
    ECode ec = CFileInputStream::New(filename, (IFileInputStream**)&pFIS);
    assert(ec == NOERROR);

    AutoPtr<ArrayOf<Byte> > pData = ArrayOf<Byte>::Alloc(bufferSize);
    int number, nTmp;
    pFIS->ReadBytes(pData, &number);
    while (number>0)  {
        ec = mAudioTrack->Write(pData, 0, number, &nTmp);
        assert(ec == NOERROR);

        pFIS->ReadBytes(pData, &number);
        printf("ReadBuffer %d\n", number);
    }

    return NOERROR;
}

ArrayOf<Int16>* CMainActivity::createSoundDataInInt16Array(int bufferSize, int sampleRate)
{
    double frequency = 1024;
    double rad = 2 * Math::DOUBLE_PI * frequency / sampleRate;

    ArrayOf<Int16>* Int16s = ArrayOf<Int16>::Alloc(bufferSize);
    for (int j = 0; j < bufferSize; j++) {
        (*Int16s)[j] = (Int16) (Math::Sin(j * rad) * 32767);
    }
    return Int16s;
}
