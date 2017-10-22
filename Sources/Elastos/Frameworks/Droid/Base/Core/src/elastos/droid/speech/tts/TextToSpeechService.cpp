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

#include "elastos/droid/speech/tts/TextToSpeechService.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/content/CIntent.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/speech/tts/CSynthesisRequest.h"
#include "elastos/droid/speech/tts/EventLoggerV1.h"
#include "elastos/droid/speech/tts/PlaybackSynthesisCallback.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/speech/tts/FileSynthesisCallback.h"
#include "elastos/droid/speech/tts/SilencePlaybackQueueItem.h"
#include "elastos/droid/os/CBinder.h"
#include "elastos/droid/speech/tts/CTtsEngines.h"
#include "elastos/droid/content/pm/CApplicationInfo.h"
//#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/speech/tts/TextToSpeech.h"
#include <elastos/core/StringUtils.h>
#include "elastos/droid/speech/tts/AudioOutputParams.h"
#include <elastos/core/AutoLock.h>
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.Droid.Media.h"

using Elastos::Core::StringUtils;
using Elastos::IO::CFile;
using Elastos::Droid::Os::EIID_IIdleHandler;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::CString;
using Elastos::Droid::Os::CBinder;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::IMessageQueue;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Content::Pm::CApplicationInfo;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::EIID_IHandler;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Content::IContentResolver;

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Tts {

const Boolean TextToSpeechService::DBG = FALSE;
const String TextToSpeechService::TAG("TextToSpeechService");

const Int32 TextToSpeechService::MAX_SPEECH_ITEM_CHAR_LENGTH = 4000;
const String TextToSpeechService::SYNTH_THREAD_NAME("SynthThread");

/******************
 * TextToSpeechService::SynthThread
 *******************************************************************************************************/

CAR_INTERFACE_IMPL(TextToSpeechService::SynthThread, Object, IIdleHandler)

TextToSpeechService::SynthThread::SynthThread()
{}

TextToSpeechService::SynthThread::~SynthThread()
{}

ECode TextToSpeechService::SynthThread::constructor()
{
    return NOERROR;
}

ECode TextToSpeechService::SynthThread::constructor(
    /* [in] */ TextToSpeechService* ttss)
{
    mFirstIdle = TRUE;
    mTtss = ttss;

    return NOERROR;
}

void TextToSpeechService::SynthThread::OnLooperPrepared()
{
    AutoPtr<ILooper> looper;
    // TODO
    // GetLooper((ILooper**)&looper);
    AutoPtr<IMessageQueue> messageQueue;
    looper->GetQueue((IMessageQueue**)&messageQueue);
    messageQueue->AddIdleHandler(this);
}

ECode TextToSpeechService::SynthThread::QueueIdle(
    /* [out] */ Boolean* result)
{
    if (mFirstIdle) {
        mFirstIdle = FALSE;
    } else {
        BroadcastTtsQueueProcessingCompleted();
    }
    return TRUE;
}

void TextToSpeechService::SynthThread::BroadcastTtsQueueProcessingCompleted()
{
    AutoPtr<IIntent> i;
    CIntent::New(ITextToSpeech::ACTION_TTS_QUEUE_PROCESSING_COMPLETED, (IIntent**)&i);
    if (DBG){
        //Java:    Log.d(TAG, "Broadcasting: " + i);
        String strIntent;
        i->ToString(&strIntent);
        Logger::D(TAG, String("Broadcasting: ")+strIntent+String("\n"));
    }
    mTtss->SendBroadcast(i);
}

/******************************TextToSpeechService::SynthHandler::RunnableSynthHandlerEnqueueSpeechItem*************************/

TextToSpeechService::SynthHandler::RunnableSynthHandlerEnqueueSpeechItem::RunnableSynthHandlerEnqueueSpeechItem(
    /* [in] */ SynthHandler* sh,
    /* [in] */ SpeechItem* si)
{
    mSh = sh;
    mSi = si;
}

ECode TextToSpeechService::SynthHandler::RunnableSynthHandlerEnqueueSpeechItem::Run()
{
    mSh->SetCurrentSpeechItem(mSi);
    mSi->Play();
    mSh->SetCurrentSpeechItem(NULL);
    return NOERROR;
}

/******************************TextToSpeechService::SynthHandler*************************/

ECode TextToSpeechService::SynthHandler::HandleMessage(
    /* [in] */ IMessage* message)
{
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

TextToSpeechService::SynthHandler::SynthHandler(
    /* [in] */ ILooper* looper,
    /* [in] */ TextToSpeechService* ttss)
//    : HandlerBase(looper)
{
    mTtss = ttss;
}

AutoPtr<TextToSpeechService::SpeechItem> TextToSpeechService::SynthHandler::GetCurrentSpeechItem()
{
    AutoLock lock(mLock);
    return mCurrentSpeechItem;
}

AutoPtr<TextToSpeechService::SpeechItem> TextToSpeechService::SynthHandler::SetCurrentSpeechItem(
    /* [in] */ TextToSpeechService::SpeechItem* speechItem)
{
    AutoLock lock(mLock);
    AutoPtr<TextToSpeechService::SpeechItem> old = mCurrentSpeechItem;
    mCurrentSpeechItem = speechItem;
    return old;
}

AutoPtr<TextToSpeechService::SpeechItem> TextToSpeechService::SynthHandler::MaybeRemoveCurrentSpeechItem(
    /* [in] */ IInterface* callerIdentity)
{
    AutoLock lock(mLock);

    if ((mCurrentSpeechItem != NULL) && (((SpeechItem*)mCurrentSpeechItem)->GetCallerIdentity()).Get() == callerIdentity) {
        AutoPtr<TextToSpeechService::SpeechItem> current = mCurrentSpeechItem;
        mCurrentSpeechItem = NULL;
        return current;
    }
    return NULL;
}

Boolean TextToSpeechService::SynthHandler::IsSpeaking()
{
    return GetCurrentSpeechItem() != NULL;
}

void TextToSpeechService::SynthHandler::Quit()
{
    // Don't process any more speech items
//    (Handler::mLooper)->Quit();    //Java:    Stop the current speech item
    AutoPtr<TextToSpeechService::SpeechItem> current = SetCurrentSpeechItem((TextToSpeechService::SpeechItem*)NULL);
    if (current != NULL) {
        current->Stop();
    }

    // The AudioPlaybackHandler will be destroyed by the caller.
}

Int32 TextToSpeechService::SynthHandler::EnqueueSpeechItem(
    /* [in] */ Int32 queueMode,
    /* [in] */ TextToSpeechService::SpeechItem* speechItem)
{
    IUtteranceProgressDispatcher* utterenceProgress = NULL;

    if (IUtteranceProgressDispatcher::Probe(speechItem) != NULL) {
        utterenceProgress = (IUtteranceProgressDispatcher*)speechItem;
    }

    if (!speechItem->IsValid()) {
        if (utterenceProgress != NULL) {
            utterenceProgress->DispatchOnError(
                    ITextToSpeech::ERROR_INVALID_REQUEST);
        }
        return ITextToSpeech::TTS_ERROR;
    }

    if (queueMode == ITextToSpeech::QUEUE_FLUSH) {
        StopForApp( ( speechItem->GetCallerIdentity() ).Get() );
    } else if (queueMode == TextToSpeech::QUEUE_DESTROY) {
        StopAll();
    }

    AutoPtr<IInterface> callerIdentity = speechItem->GetCallerIdentity();
    AutoPtr<IParcel> params;
    CCallbackParcel::New((IParcel**)&params);
    params->WriteInterfacePtr((IInterface*)(callerIdentity.Get()));

    AutoPtr<IRunnable> runnable = new RunnableSynthHandlerEnqueueSpeechItem(this, speechItem);
    //Boolean result;
    // TODO
    ECode ec = E_FAIL;   // = this->Post(runnable, &result);

    if (!FAILED(ec)) {
        return ITextToSpeech::TTS_SUCCESS;
    }
    else {
        //Java:    Log.w(TAG, "SynthThread has quit");
        Logger::W(TAG, "SynthThread has quit\n");
        utterenceProgress->DispatchOnError(ITextToSpeech::ERROR_SERVICE);
        return ITextToSpeech::TTS_ERROR;
    }
}

Int32 TextToSpeechService::SynthHandler::StopForApp(
    /* [in] */ IInterface* callerIdentity)
{
    if (callerIdentity == NULL) {
        return ITextToSpeech::TTS_ERROR;
    }

    //Java:    removeCallbacksAndMessages(callerIdentity);

    // This stops writing data to the file / or publishing
    // items to the audio playback handler.
    //
    // Note that the current speech item must be removed only if it
    // belongs to the callingApp, else the item will be "orphaned" and
    // not stopped correctly if a stop request comes along for the item
    // from the app it belongs to.
    AutoPtr<TextToSpeechService::SpeechItem> current = MaybeRemoveCurrentSpeechItem(callerIdentity);
    if (current != NULL) {
        current->Stop();
    }

    // Remove any enqueued audio too.
    (mTtss->mAudioPlaybackHandler)->StopForApp(callerIdentity);

    return ITextToSpeech::TTS_SUCCESS;
}

Int32 TextToSpeechService::SynthHandler::StopAll()
{
    // Stop the current speech item unconditionally.
    AutoPtr<TextToSpeechService::SpeechItem> current = SetCurrentSpeechItem(NULL);
    if (current != NULL) {
        current->Stop();
    }
    // Remove all other items from the queue.
    //Java:    removeCallbacksAndMessages(null);

    // Remove all pending playback as well.
    (mTtss->mAudioPlaybackHandler)->Stop();

    return ITextToSpeech::TTS_SUCCESS;
}

/******************
 * TextToSpeechService::SpeechItem
 *******************************************************************************************************/

CAR_INTERFACE_IMPL(TextToSpeechService::SpeechItem, Object, IUtteranceProgressDispatcher)

TextToSpeechService::SpeechItem::SpeechItem()
{}

TextToSpeechService::SpeechItem::~SpeechItem()
{}

ECode TextToSpeechService::SpeechItem::constructor()
{
    return NOERROR;
}

ECode TextToSpeechService::SpeechItem::constructor(
    /* [in] */ IInterface* caller,
    /* [in] */ Int32 callerUid,
    /* [in] */ Int32 callerPid,
    /* [in] */ TextToSpeechService* ttss)
{
    mStarted = FALSE;
    mStopped = FALSE;

    mCallerIdentity = caller;
    mCallerUid = callerUid;
    mCallerPid = callerPid;
    mTtss = ttss;

    return NOERROR;
}


AutoPtr<IInterface> TextToSpeechService::SpeechItem::GetCallerIdentity()
{
    return mCallerIdentity;
}

ECode TextToSpeechService::SpeechItem::GetCallerUid(
    /* [out] */ Int32* ret)
{
    *ret = mCallerUid;
    return NOERROR;
}

ECode TextToSpeechService::SpeechItem::GetCallerPid(
    /* [out] */ Int32* ret)
{
    *ret = mCallerPid;
    return NOERROR;
}

ECode TextToSpeechService::SpeechItem::Play()
{
    if (TRUE) {
        AutoLock lock(mLock);
        if (mStarted) {
            //throw new IllegalStateException("play() called twice");
            Logger::E(TAG, String("play() called twice\n"));
            return 0;//E_ILLEGAL_STATE_EXCEPTION
        }
        mStarted = TRUE;
    }
    PlayImpl();

    return NOERROR;
}

ECode TextToSpeechService::SpeechItem::Stop()
{
    if (TRUE) {
        AutoLock lock(mLock);
        if (mStopped) {
            //throw new IllegalStateException("stop() called twice");
            Logger::E(TAG, String("stop() called twice\n"));
            //E_ILLEGAL_STATE_EXCEPTION
        }
        mStopped = TRUE;
    }
    StopImpl();

    return NOERROR;
}

Boolean TextToSpeechService::SpeechItem::IsStopped()
{
    return mStopped;
}

/******************************TextToSpeechService::UtteranceSpeechItem*************************/
CAR_INTERFACE_IMPL(TextToSpeechService::UtteranceSpeechItem, SpeechItem, IUtteranceProgressDispatcher)

TextToSpeechService::UtteranceSpeechItem::UtteranceSpeechItem()
{}

TextToSpeechService::UtteranceSpeechItem::UtteranceSpeechItem(
    /* [in] */ IInterface* caller,
    /* [in] */ Int32 callerUid,
    /* [in] */ Int32 callerPid)
{}

ECode TextToSpeechService::UtteranceSpeechItem::DispatchOnSuccess()
{
#if 0
    final String utteranceId = getUtteranceId();
    if (utteranceId != null) {
        mCallbacks.dispatchOnSuccess(getCallerIdentity(), utteranceId);
    }
#endif
    return NOERROR;
}

ECode TextToSpeechService::UtteranceSpeechItem::DispatchOnStop()
{
#if 0
    final String utteranceId = getUtteranceId();
    if (utteranceId != null) {
        mCallbacks.dispatchOnStop(getCallerIdentity(), utteranceId);
    }
#endif
    return NOERROR;
}

ECode TextToSpeechService::UtteranceSpeechItem::DispatchOnStart()
{
#if 0
    final String utteranceId = getUtteranceId();
    if (utteranceId != null) {
        mCallbacks.dispatchOnStart(getCallerIdentity(), utteranceId);
    }
#endif
    return NOERROR;
}

ECode TextToSpeechService::UtteranceSpeechItem::DispatchOnError(
    /* [in] */ Int32 errorCode)
{
#if 0
    final String utteranceId = getUtteranceId();
    if (utteranceId != null) {
        mCallbacks.dispatchOnError(getCallerIdentity(), utteranceId, errorCode);
    }
#endif
    return NOERROR;
}

//
// abstract public
//
String TextToSpeechService::UtteranceSpeechItem::GetUtteranceId()
{
    return String("");
}


String TextToSpeechService::UtteranceSpeechItem::GetStringParam(
    /* [in] */ IBundle* params,
    /* [in] */ const String& key,
    /* [in] */ const String& defaultValue)
{
    String value;

    if (params == NULL)
        return defaultValue;

    params->GetString(key, defaultValue, &value);
    return value;
}


Int32 TextToSpeechService::UtteranceSpeechItem::GetIntParam(
    /* [in] */ IBundle* params,
    /* [in] */ const String& key,
    /* [in] */ Int32 defaultValue)
{
    Int32 value;

    if (params == NULL)
        return defaultValue;

    params->GetInt32(key, defaultValue, &value);
    return value;
}


Float TextToSpeechService::UtteranceSpeechItem::GetFloatParam(
    /* [in] */ IBundle* params,
    /* [in] */ const String& key,
    /* [in] */ Float defaultValue)
{
    Float value;

    if (params == NULL)
        return defaultValue;

    params->GetFloat(key, defaultValue, &value);
    return value;
}

/******************************TextToSpeechService::SpeechItemV1*************************/

TextToSpeechService::SpeechItemV1::SpeechItemV1(
    /* [in] */ IInterface* callerIdentity,
    /* [in] */ Int32 callerUid,
    /* [in] */ Int32 callerPid,
    /* [in] */ IBundle* params,
    /* [in] */ const String& utteranceId,
    /* [in] */ TextToSpeechService* ttss)
    : UtteranceSpeechItem(callerIdentity, callerUid, callerPid)
{
    mParams = params;
    mUtteranceId = utteranceId;
}

Boolean TextToSpeechService::SpeechItemV1::HasLanguage()
{
//    return !TextUtils.isEmpty(getStringParam(mParams, Engine.KEY_PARAM_LANGUAGE, null));
    return FALSE;
}

Int32 TextToSpeechService::SpeechItemV1::GetSpeechRate()
{
//    getIntParam(mParams, Engine.KEY_PARAM_RATE, getDefaultSpeechRate());
    return 0;
}

Int32 TextToSpeechService::SpeechItemV1::GetPitch()
{
//    GetIntParam(mParams, Engine.KEY_PARAM_PITCH, Engine.DEFAULT_PITCH);
    return 0;
}

String TextToSpeechService::SpeechItemV1::GetUtteranceId()
{
    return mUtteranceId;
}

AudioOutputParams* TextToSpeechService::SpeechItemV1::GetAudioParams()
{
//    return AudioOutputParams.createFromV1ParamsBundle(mParams, true);
    return NULL;
}

/******************************AudioOutputParams*************************/
AudioOutputParams::AudioOutputParams()
{
#if 0
    mSessionId = AudioSystem.AUDIO_SESSION_ALLOCATE;
    mVolume = Engine.DEFAULT_VOLUME;
    mPan = Engine.DEFAULT_PAN;
#endif
    mAudioAttributes = NULL;
}

AudioOutputParams::AudioOutputParams(
    /* [in] */ Int32 sessionId,
    /* [in] */ Float volume,
    /* [in] */ Float pan,
    /* [in] */ IAudioAttributes* audioAttributes)
{
    mSessionId = sessionId;
    mVolume = volume;
    mPan = pan;
    mAudioAttributes = audioAttributes;
}

/** Create AudioOutputParams from A {@link SynthesisRequest#getParams()} bundle */
AudioOutputParams* AudioOutputParams::CreateFromV1ParamsBundle(
    /* [in] */ IBundle* paramsBundle,
    /* [in] */ Boolean isSpeech)
{
#if 0
            if (paramsBundle == null) {
                return new AudioOutputParams();
            }

            AudioAttributes audioAttributes =
                    (AudioAttributes) paramsBundle.getParcelable(
                            Engine.KEY_PARAM_AUDIO_ATTRIBUTES);
            if (audioAttributes == null) {
                int streamType = paramsBundle.getInt(
                        Engine.KEY_PARAM_STREAM, Engine.DEFAULT_STREAM);
                audioAttributes = (new AudioAttributes.Builder())
                        .setLegacyStreamType(streamType)
                        .setContentType((isSpeech ?
                                AudioAttributes.CONTENT_TYPE_SPEECH :
                                AudioAttributes.CONTENT_TYPE_SONIFICATION))
                        .build();
            }

            return new AudioOutputParams(
                    paramsBundle.getInt(
                            Engine.KEY_PARAM_SESSION_ID,
                            AudioSystem.AUDIO_SESSION_ALLOCATE),
                    paramsBundle.getFloat(
                            Engine.KEY_PARAM_VOLUME,
                            Engine.DEFAULT_VOLUME),
                    paramsBundle.getFloat(
                            Engine.KEY_PARAM_PAN,
                            Engine.DEFAULT_PAN),
                    audioAttributes);
#endif
//    if (paramsBundle == NULL) {
        return new AudioOutputParams();
//    }
}


/******************************TextToSpeechService::SynthesisToFileOutputStreamSpeechItemV1*************************/
TextToSpeechService::SynthesisToFileOutputStreamSpeechItemV1::SynthesisToFileOutputStreamSpeechItemV1(
    /* [in] */ IInterface* callerIdentity,
    /* [in] */ Int32 callerUid,
    /* [in] */ Int32 callerPid,
    /* [in] */ IBundle* params,
    /* [in] */ const String& utteranceId,
    /* [in] */ IFileOutputStream* fileOutputStream,
    /* [in] */ TextToSpeechService* ttss)
    : SynthesisSpeechItemV1(callerIdentity, callerUid, callerPid, params, utteranceId, ttss)
{
    //super(callerIdentity, callerUid, callerPid, params, utteranceId, text);
    mFileOutputStream = fileOutputStream;
}

AbstractSynthesisCallback* TextToSpeechService::SynthesisToFileOutputStreamSpeechItemV1::createSynthesisCallback()
{
#if 0
     return new FileSynthesisCallback(mFileOutputStream.getChannel(),
                    this, getCallerIdentity(), false);
#endif
    return NULL;
}

void TextToSpeechService::SynthesisToFileOutputStreamSpeechItemV1::PlayImpl()
{
#if 0
    dispatchOnStart();
    super.playImpl();
    try {
      mFileOutputStream.close();
    } catch(IOException e) {
      Log.w(TAG, "Failed to close output file", e);
    }
#endif
    DispatchOnStart();
    SynthesisSpeechItemV1::PlayImpl();
}

/******************************TextToSpeechService::AudioSpeechItemV1*************************/
TextToSpeechService::AudioSpeechItemV1::AudioSpeechItemV1(
    /* [in] */ IInterface* callerIdentity,
    /* [in] */ Int64 callerUid,
    /* [in] */ Int64 callerPid,
    /* [in] */ IBundle* params,
    /* [in] */ const String& utteranceId,
    /* [in] */ IURI* uri,
    /* [in] */ TextToSpeechService* ttss)
    : SpeechItemV1(callerIdentity, callerUid, callerPid, params, utteranceId, ttss)
{
#if 0
                super(callerIdentity, callerUid, callerPid, params, utteranceId);
            mItem = new AudioPlaybackQueueItem(this, getCallerIdentity(),
                    TextToSpeechService.this, uri, getAudioParams());
#endif
}

Boolean TextToSpeechService::AudioSpeechItemV1::IsValid()
{
    return true;
}

void TextToSpeechService::AudioSpeechItemV1::PlayImpl()
{
//    mAudioPlaybackHandler->Enqueue(mItem);
}

void TextToSpeechService::AudioSpeechItemV1::StopImpl() {
    // Do nothing.
}

String TextToSpeechService::AudioSpeechItemV1::GetUtteranceId()
{
    return mUtteranceId;
}

AudioOutputParams* TextToSpeechService::AudioSpeechItemV1::GetAudioParams()
{
    return AudioOutputParams::CreateFromV1ParamsBundle(mParams, FALSE);
}

/******************************TextToSpeechService::SynthesisSpeechItemV1*************************/
TextToSpeechService::SynthesisSpeechItemV1::SynthesisSpeechItemV1(
    /* [in] */ IInterface* callerIdentity,
    /* [in] */ Int32 callerUid,
    /* [in] */ Int32 callerPid,
    /* [in] */ IBundle* params,
    /* [in] */ const String& utteranceId,
    /* [in] */ TextToSpeechService* ttss)
    : SpeechItemV1(callerIdentity, callerUid, callerPid, params, utteranceId, ttss)
{
    AutoPtr<ICharSequence> str;

    CString::New(utteranceId, (ICharSequence**)&str);
    mText = str;
    CSynthesisRequest::New(mText, mParams, (ISynthesisRequest**)&mSynthesisRequest);
    mDefaultLocale = mTtss->GetSettingsLocale();
    SetRequestParams(mSynthesisRequest);
    mEventLogger = new EventLoggerV1();
    mEventLogger->constructor(mSynthesisRequest, callerUid, callerPid, mTtss->mPackageName);
}

ICharSequence* TextToSpeechService::SynthesisSpeechItemV1::GetText()
{
    return mText;
}

Boolean TextToSpeechService::SynthesisSpeechItemV1::IsValid()
{
    if (mText == NULL) {
        //Java:    Log.e(TAG, "null synthesis text");
        Logger::E(TAG, String("null synthesis text\n"));
        return FALSE;
    }

    Int32 number;
    mText->GetLength(&number);
    if (number >= TextToSpeechService::MAX_SPEECH_ITEM_CHAR_LENGTH) {
        //Java:    Log.w(TAG, "Text too long: " + mText.length() + " chars");
        Logger::W(TAG, "Text too long: %d chars\n", number);
        return FALSE;
    }
    return TRUE;
}

void TextToSpeechService::SynthesisSpeechItemV1::PlayImpl()
{
    AutoPtr<AbstractSynthesisCallback> synthesisCallback;
    mEventLogger->OnRequestProcessingStart();
    if (TRUE) {
        AutoLock lock(mLock);
        // stop() might have been called before we enter this
        // synchronized block.
        if (IsStopped()) {
            return;
        }
        mSynthesisCallback = CreateSynthesisCallback();
        synthesisCallback = mSynthesisCallback;
    }
    mTtss->OnSynthesizeText(mSynthesisRequest.Get(), (ISynthesisCallback*)(synthesisCallback.Get()));

    // Fix for case where client called .start() & .error(), but did not called .done()
    Boolean b;
    Boolean b2;
    synthesisCallback->HasStarted(&b);
    synthesisCallback->HasFinished(&b2);
    if (b && !b2) {
        Int32 ret;
        synthesisCallback->Done(&ret);
    }
}

AbstractSynthesisCallback* TextToSpeechService::SynthesisSpeechItemV1::CreateSynthesisCallback()
{
    AutoPtr<AbstractSynthesisCallback> asCallback = new PlaybackSynthesisCallback(GetAudioParams(),
                    (AudioPlaybackHandler*)mTtss->mAudioPlaybackHandler, this, GetCallerIdentity(), mEventLogger, FALSE);
#if 0
    PlaybackSynthesisCallback(
        /* [in] */ AudioOutputParams* audioParams,
        /* [in] */ AudioPlaybackHandler* audioTrackHandler,
        /* [in] */ IUtteranceProgressDispatcher* dispatcher,
        /* [in] */ IInterface* callerIdentity,
        /* [in] */ EventLoggerV1* logger,
        /* [in] */ Boolean clientIsUsingV2);
#endif
    return asCallback;
}

void TextToSpeechService::SynthesisSpeechItemV1::SetRequestParams(
    /* [in] */ ISynthesisRequest* request)
{
    ((CSynthesisRequest*)request)->SetLanguage(GetLanguage(), GetCountry(), GetVariant());
    ((CSynthesisRequest*)request)->SetSpeechRate(GetSpeechRate());

    ((CSynthesisRequest*)request)->SetPitch(GetPitch());
}

void TextToSpeechService::SynthesisSpeechItemV1::StopImpl()
{
    AutoPtr<AbstractSynthesisCallback> synthesisCallback;
    if(TRUE){
        AutoLock lock(mLock);
        synthesisCallback = mSynthesisCallback;
    }
    if (synthesisCallback != NULL) {
        // If the synthesis callback is null, it implies that we haven't
        // entered the synchronized(this) block in playImpl which in
        // turn implies that synthesis would not have started.
        synthesisCallback->Stop();
        mTtss->OnStop();
    }
}

String TextToSpeechService::SynthesisSpeechItemV1::GetLanguage()
{
    return GetStringParam(mParams, ITextToSpeechEngine::KEY_PARAM_LANGUAGE, (*mDefaultLocale)[0]);
}

Boolean TextToSpeechService::SynthesisSpeechItemV1::HasLanguage()
{
    String stringParam = GetStringParam(mParams, ITextToSpeechEngine::KEY_PARAM_LANGUAGE, String(NULL));
    AutoPtr<ICharSequence> cs;
    CString::New(stringParam, (ICharSequence**)&cs);
    return !TextUtils::IsEmpty(cs.Get());
}

String TextToSpeechService::SynthesisSpeechItemV1::GetCountry()
{
    if (!HasLanguage()){
        return (*mDefaultLocale)[1];
    }
    return GetStringParam(mParams, ITextToSpeechEngine::KEY_PARAM_COUNTRY, String(""));
}

String TextToSpeechService::SynthesisSpeechItemV1::GetVariant()
{
    if (!HasLanguage()){
        return (*mDefaultLocale)[2];
    }
    return GetStringParam(mParams, ITextToSpeechEngine::KEY_PARAM_VARIANT, String(""));
}


Int32 TextToSpeechService::SynthesisSpeechItemV1::GetSpeechRate()
{
    return GetIntParam(mParams, ITextToSpeechEngine::KEY_PARAM_RATE, mTtss->GetDefaultSpeechRate());
}

Int32 TextToSpeechService::SynthesisSpeechItemV1::GetPitch()
{
    return GetIntParam(mParams, ITextToSpeechEngine::KEY_PARAM_PITCH, ITextToSpeechEngine::DEFAULT_PITCH);
}

/******************************TextToSpeechService::SilenceSpeechItem*************************/
TextToSpeechService::SilenceSpeechItem::SilenceSpeechItem(
    /* [in] */ IInterface* callerIdentity,
    /* [in] */ Int32 callerUid,
    /* [in] */ Int32 callerPid,
    /* [in] */ const String& utteranceId,
    /* [in] */ Int64 duration,
    /* [in] */ TextToSpeechService* ttss)
{
    mDuration = duration;
    constructor(callerIdentity, callerUid, callerPid, ttss);
}

Boolean TextToSpeechService::SilenceSpeechItem::IsValid()
{
    return TRUE;
}

void TextToSpeechService::SilenceSpeechItem::PlayImpl()
{
    AutoPtr<SilencePlaybackQueueItem> spqi = new SilencePlaybackQueueItem();
    spqi->constructor(this, GetCallerIdentity(), mDuration);
    (mTtss->mAudioPlaybackHandler)->Enqueue(spqi.Get());
}

void TextToSpeechService::SilenceSpeechItem::StopImpl()
{
    // Do nothing, handled by AudioPlaybackHandler#stopForApp
}

/******************
 * TextToSpeechService::TextToSpeechServiceStub
 *******************************************************************************************************/

CAR_INTERFACE_IMPL(TextToSpeechService::TextToSpeechServiceStub, Object, IITextToSpeechService)

TextToSpeechService::TextToSpeechServiceStub::TextToSpeechServiceStub()
{}

TextToSpeechService::TextToSpeechServiceStub::~TextToSpeechServiceStub()
{}

ECode TextToSpeechService::TextToSpeechServiceStub::constructor()
{
    return NOERROR;
}

ECode TextToSpeechService::TextToSpeechServiceStub::constructor(
    /* [in] */ TextToSpeechService* ttss)
{
    mTtss = ttss;
    return NOERROR;
}

ECode TextToSpeechService::TextToSpeechServiceStub::Speak(
    /* [in] */ IBinder* caller,
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 queueMode,
    /* [in] */ IBundle* params,
    /* [in] */ const String& utteranceld,
    /* [out] */ Int32* result)
{
    AutoPtr< ArrayOf<IInterface*> > aryInterface = ArrayOf<IInterface*>::Alloc(3);
    aryInterface->Set(0, caller);
    aryInterface->Set(1, text);
    aryInterface->Set(2, params);

    if (!CheckNonNull( aryInterface.Get() )) {
        *result = ITextToSpeech::TTS_ERROR;
        return NOERROR;
    }

    // AutoPtr<SpeechItem> item = new SynthesisSpeechItemV1(caller,
    //                 CBinder::GetCallingUid(), CBinder::GetCallingPid(), params, text, mTtss.Get());
    // *result = (mTtss->mSynthHandler)->EnqueueSpeechItem(queueMode, item.Get());
    return NOERROR;
}

ECode TextToSpeechService::TextToSpeechServiceStub::SynthesizeToFileDescriptor(
    /* [in] */ IBinder* caller,
    /* [in] */ ICharSequence* text,
    /* [in] */ IParcelFileDescriptor* fd,
    /* [in] */ IBundle* params,
    /* [out] */ Int32* result)
{
    AutoPtr< ArrayOf<IInterface*> > aryInterface = ArrayOf<IInterface*>::Alloc(4);
    aryInterface->Set(0, caller);
    aryInterface->Set(1, text);
    aryInterface->Set(2, fd);
    aryInterface->Set(3, params);
    if (!CheckNonNull( aryInterface.Get() )) {
        *result = ITextToSpeech::TTS_ERROR;
        return NOERROR;
    }

    // AutoPtr<IFile> file;
    // CFile::New(filename, (IFile**)&file);
//    AutoPtr<SpeechItem> item = new SynthesisToFileSpeechItem(caller, CBinder::GetCallingUid(), CBinder::GetCallingPid(), params, text, file.Get(), mTtss.Get());
//    *result = (mTtss->mSynthHandler)->EnqueueSpeechItem(ITextToSpeech::QUEUE_ADD, item.Get());
    return NOERROR;
}

ECode TextToSpeechService::TextToSpeechServiceStub::PlayAudio(
    /* [in] */ IBinder* caller,
    /* [in] */ IUri* audioUri,
    /* [in] */ Int32 queueMode,
    /* [in] */ IBundle* params,
    /* [in] */ const String& utteranceId,
    /* [out] */ Int32* result)
{
    AutoPtr< ArrayOf<IInterface*> > aryInterface = ArrayOf<IInterface*>::Alloc(3);
    aryInterface->Set(0, caller);
    aryInterface->Set(1, audioUri);
    aryInterface->Set(2, params);
    if (!CheckNonNull(aryInterface.Get())) {
        *result = ITextToSpeech::TTS_ERROR;
        return NOERROR;
    }

/*
    AutoPtr<SpeechItem> item = new AudioSpeechItem(caller,
                    CBinder::GetCallingUid(), CBinder::GetCallingPid(), params, audioUri, mTtss.Get());
    *result = (mTtss->mSynthHandler)->EnqueueSpeechItem(queueMode, item.Get());
*/
    return NOERROR;
}

ECode TextToSpeechService::TextToSpeechServiceStub::PlaySilence(
    /* [in] */ IBinder* caller,
    /* [in] */ Int64 duration,
    /* [in] */ Int32 queueMode,
    /* [in] */ IBundle* params,
    /* [in] */ const String& utteranceId,
    /* [out] */ Int32* result)
{
    AutoPtr<ArrayOf<IInterface*> > aryInterface = ArrayOf<IInterface*>::Alloc(2);
    aryInterface->Set(0, caller);
    aryInterface->Set(1, params);
    if (!CheckNonNull(aryInterface.Get())) {
        *result = ITextToSpeech::TTS_ERROR;
        return NOERROR;
    }

/*
    AutoPtr<SpeechItem> item = new SilenceSpeechItem(caller,
                    CBinder::GetCallingUid(), CBinder::GetCallingPid(), params, duration, mTtss.Get());
    *result = (mTtss->mSynthHandler)->EnqueueSpeechItem(queueMode, item.Get());
*/
    return NOERROR;
}

ECode TextToSpeechService::TextToSpeechServiceStub::IsSpeaking(
    /* [out] */ Boolean* result)
{
    return (mTtss->mSynthHandler)->IsSpeaking() || (mTtss->mAudioPlaybackHandler)->IsSpeaking();
}

ECode TextToSpeechService::TextToSpeechServiceStub::Stop(
    /* [in] */ IBinder* caller,
    /* [out] */ Int32* result)
{
    AutoPtr< ArrayOf<IInterface*> > aryInterface = ArrayOf<IInterface*>::Alloc(1);
    aryInterface->Set(0, caller);
    if (!CheckNonNull(aryInterface.Get())) {
        *result = ITextToSpeech::TTS_ERROR;
        return NOERROR;
    }
    *result = (mTtss->mSynthHandler)->StopForApp(caller);
    return NOERROR;
}

ECode TextToSpeechService::TextToSpeechServiceStub::GetLanguage(
    /* [out] */ ArrayOf<String>** result)
{
    *result = mTtss->OnGetLanguage();
    return NOERROR;
}

ECode TextToSpeechService::TextToSpeechServiceStub::IsLanguageAvailable(
    /* [in] */ const String & lang,
    /* [in] */ const String & country,
    /* [in] */ const String & variant,
    /* [out] */ Int32* result)
{
    AutoPtr< ArrayOf<IInterface*> > aryInterface = ArrayOf<IInterface*>::Alloc(1);
    AutoPtr<ICharSequence> cs;
    CString::New(lang, (ICharSequence**)&cs);
    aryInterface->Set(0, cs);

    if (!CheckNonNull(aryInterface.Get())) {
        *result = ITextToSpeech::TTS_ERROR;
        return NOERROR;
    }
    *result = mTtss->OnIsLanguageAvailable(lang, country, variant);
    return NOERROR;
}

ECode TextToSpeechService::TextToSpeechServiceStub::GetFeaturesForLanguage(
    /* [in] */ const String & lang,
    /* [in] */ const String & country,
    /* [in] */ const String & variant,
    /* [out] */ ArrayOf<String>** result)
{
    AutoPtr< Set<String> > features = mTtss->OnGetFeaturesForLanguage(lang, country, variant);
    AutoPtr< ArrayOf<String> > featuresArray;
    if (!features->IsEmpty()) {
        featuresArray = ArrayOf<String>::Alloc(features->GetSize());

        Set<String>::Iterator item = features->Begin();
        Int32 i = 0;
        for(; item != features->End(); item++) {
            (*featuresArray)[i++] = *item;
        }
    }
    else {
        featuresArray = ArrayOf<String>::Alloc(0);
    }

    *result = featuresArray;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode TextToSpeechService::TextToSpeechServiceStub::LoadLanguage(
    /* [in] */ IBinder* caller,
    /* [in] */ const String & lang,
    /* [in] */ const String & country,
    /* [in] */ const String & variant,
    /* [out] */ Int32* result)
{
    AutoPtr< ArrayOf<IInterface*> > aryInterface = ArrayOf<IInterface*>::Alloc(1);
    AutoPtr<ICharSequence> cs;
    CString::New(lang, (ICharSequence**)&cs);
    aryInterface->Set(0, cs);

    if (!CheckNonNull(aryInterface.Get())) {
        *result = ITextToSpeech::TTS_ERROR;
        return NOERROR;
    }

    *result = mTtss->OnLoadLanguage(lang, country, variant);
    return NOERROR;
}

ECode TextToSpeechService::TextToSpeechServiceStub::SetCallback(
    /* [in] */ /*IIBinder*/IBinder* caller,
    /* [in] */ IITextToSpeechCallback* cb)
{
    AutoPtr< ArrayOf<IInterface*> > aryInterface = ArrayOf<IInterface*>::Alloc(1);
    aryInterface->Set(0, caller);
    // Note that passing in a null callback is a valid use case.
    if (!CheckNonNull(aryInterface.Get())) {
        return NOERROR;
    }

    (mTtss->mCallbacks)->SetCallback(caller, cb);
    return NOERROR;
}

String TextToSpeechService::TextToSpeechServiceStub::Intern(
    /* [in] */ const String& in)
{
    // The input parameter will be non null.
    //return in.intern();
    return in;
}

Boolean TextToSpeechService::TextToSpeechServiceStub::CheckNonNull(
    /* [in] */ ArrayOf<IInterface*>* args)
{
    Int32 aryLen = args->GetLength();
    for (Int32 i = 0; i < aryLen; i++) {
        if( (*args)[i] == NULL ) {
            return FALSE;
        }
    }
    return TRUE;
}

/******************************TextToSpeechService::CallbackMap*************************/
TextToSpeechService::CallbackMap::CallbackMap(
            /* [in] */ TextToSpeechService* ttss)
{
    mTtss = ttss;
}

void TextToSpeechService::CallbackMap::SetCallback(
    /* [in] */ /*IIBinder*/IBinder* caller,
    /* [in] */ IITextToSpeechCallback* cb)
{
    AutoLock lock(mCallerToCallbackLock);
    AutoPtr<IITextToSpeechCallback> old = NULL;
    HashMap< AutoPtr</*IIBinder*/IBinder>, AutoPtr<IITextToSpeechCallback> >::Iterator oldI;
    if (cb != NULL) {
//        Register(cb, caller);
//        oldI = ( mCallerToCallback.Insert( HashMap< AutoPtr</*IIBinder*/IBinder>, AutoPtr<IITextToSpeechCallback> >::ValueType(caller, cb) ) ).mFirst;
        old = oldI->mSecond;
    } else {
        //Java:    old = mCallerToCallback.remove(caller);
//        mCallerToCallback.Erase(caller);
    }
    if (old != NULL && old.Get() != cb) {
//        Unregister(old);
    }
}

void TextToSpeechService::CallbackMap::DispatchOnDone(
    /* [in] */ IInterface* callerIdentity,
    /* [in] */ const String& utteranceId)
{
    AutoPtr<IITextToSpeechCallback> cb = GetCallbackFor(callerIdentity);
    if (cb == NULL)
        return;
    //try {
        cb->OnStop(utteranceId);
    //} catch (RemoteException e) {
        //Java:    Log.e(TAG, "Callback onDone failed: " + e);
        //Logger::E(TAG, String("Callback onDone failed: \n"));
    //}
}

void TextToSpeechService::CallbackMap::DispatchOnStart(
    /* [in] */ IInterface* callerIdentity,
    /* [in] */ const String& utteranceId)
{
    AutoPtr<IITextToSpeechCallback> cb = GetCallbackFor(callerIdentity);
    if (cb == NULL)
        return;
    //try {
        cb->OnStart(utteranceId);
    //} catch (RemoteException e) {
        //Java:    Log.e(TAG, "Callback onStart failed: " + e);
        //Logger::E(TAG, String("Callback onStart failed: \n"));
    //}
}

void TextToSpeechService::CallbackMap::DispatchOnError(
    /* [in] */ IInterface* callerIdentity,
    /* [in] */ const String& utteranceId,
    /* [in] */ Int32 errorCode)
{
    AutoPtr<IITextToSpeechCallback> cb = GetCallbackFor(callerIdentity);
    if (cb == NULL) return;
    //try {
        cb->OnError(utteranceId, errorCode);
    //} catch (RemoteException e) {
        //Log.e(TAG, "Callback onError failed: " + e);
        //Logger::E(TAG, String("Callback onError failed: \n"));
    //}
}

void TextToSpeechService::CallbackMap::OnCallbackDied(
    /* [in] */ IITextToSpeechCallback* callback,
    /* [in] */ IInterface* cookie)
{
    AutoPtr</*IIBinder*/IBinder> caller = (/*IIBinder*/IBinder*) cookie;
    if (TRUE) {
        AutoLock lock(mCallerToCallbackLock);
//        mCallerToCallback.Erase(caller);
    }
    (mTtss->mSynthHandler)->StopForApp(caller);
}

void TextToSpeechService::CallbackMap::Kill()
{
    AutoLock lock(mCallerToCallbackLock);
    mCallerToCallback.Clear();
    //Java:    super.kill();
}

AutoPtr<IITextToSpeechCallback> TextToSpeechService::CallbackMap::GetCallbackFor(
    /* [in] */ IInterface* caller)
{
    AutoPtr<IITextToSpeechCallback> cb;
    AutoPtr</*IIBinder*/IBinder> asBinder = (/*IIBinder*/IBinder*) caller;
    if (TRUE) {
        AutoLock lock(mCallerToCallbackLock);
//        cb = (mCallerToCallback.Find(asBinder.Get()))->mSecond;
    }
    return cb;
}

/******************************TextToSpeechService*************************/
CAR_INTERFACE_IMPL(TextToSpeechService::TextToSpeechService, Object, ITextToSpeechService)

TextToSpeechService::TextToSpeechService()
{
//    mBinder = (IITextToSpeechService*)(new TextToSpeechServiceStub(this));
}

TextToSpeechService::~TextToSpeechService()
{
}

ECode TextToSpeechService::constructor()
{
    return NOERROR;
}

ECode TextToSpeechService::OnCreate()
{
    if (DBG) {
        //Java:    Log.d(TAG, "onCreate()");
        Logger::D(TAG, String("onCreate() \n"));
    }
    Service::OnCreate();

/*    AutoPtr<SynthThread> synthThread = new SynthThread(this);
//    synthThread->Start();
//    mSynthHandler = new SynthHandler(synthThread->GetLooper());

    mAudioPlaybackHandler = new AudioPlaybackHandler();
    mAudioPlaybackHandler->Start();

    CTtsEngines::New(this, (ITtsEngines**)&mEngineHelper);//Java:    mEngineHelper = new TtsEngines(this);

    mCallbacks = new CallbackMap(this);

    //Java:    mPackageName = getApplicationInfo().packageName;
    AutoPtr<IApplicationInfo> appInfo;
    GetApplicationInfo((IApplicationInfo**)&appInfo);
    ((CApplicationInfo*)(appInfo.Get()))->GetPackageName(&mPackageName);

    AutoPtr< ArrayOf<String> > defaultLocale = GetSettingsLocale();
    // Load default language
    OnLoadLanguage((*defaultLocale)[0], (*defaultLocale)[1], (*defaultLocale)[2]);
*/
    return NOERROR;
}

ECode TextToSpeechService::OnDestroy()
{
    if (DBG) {
        //Java:    Log.d(TAG, "onDestroy()");
        Logger::D(TAG, "onDestroy() \n");
    }

    // Tell the synthesizer to stop
    mSynthHandler->Quit();
    // Tell the audio playback thread to stop.
    mAudioPlaybackHandler->Quit();
    // Unregister all callbacks.
    mCallbacks->Kill();

    Service::OnDestroy();
    return NOERROR;
}

AutoPtr<Set<String> > TextToSpeechService::OnGetFeaturesForLanguage(
    /* [in] */ const String& lang,
    /* [in] */ const String& country,
    /* [in] */ const String& variant)
{
    AutoPtr<Set<String> > ret = new Set<String>();
    return ret;
}

Int32 TextToSpeechService::GetDefaultSpeechRate()
{
    return GetSecureSettingInt(/*ISettingsSecure::TTS_DEFAULT_RATE*/String("tts_default_rate"), ITextToSpeechEngine::DEFAULT_RATE);
}

AutoPtr<ArrayOf<String> > TextToSpeechService::GetSettingsLocale()
{
    AutoPtr<ILocale> locale;

    mEngineHelper->GetLocalePrefForEngine(mPackageName, (ILocale**)&locale);
    return CTtsEngines::ToOldLocaleStringFormat(locale);
}

Int32 TextToSpeechService::GetSecureSettingInt(
    /* [in] */ const String& name,
    /* [in] */ Int32 defaultValue)
{
    AutoPtr<IContentResolver> cr;
    GetContentResolver((IContentResolver**)&cr);
    Int32 secureSettingInt = 0;
    // TODO
//    Settings::Secure::GetInt32(cr.Get(), name, defaultValue, &secureSettingInt);
    return secureSettingInt;
}


ECode TextToSpeechService::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** service)
{
    VALIDATE_NOT_NULL(service);
    String strAction;
    intent->GetAction(&strAction);
    if (ITextToSpeechEngine::INTENT_ACTION_TTS_SERVICE.Equals(strAction)) {
        *service = IBinder::Probe(mBinder);
        REFCOUNT_ADD(*service);
        return NOERROR;
    }
    *service = NULL;
    return NOERROR;
}

Int32 GetExpectedLanguageAvailableStatus(
    /* [in] */ ILocale* locale)
{
#if 0
        int expectedStatus = TextToSpeech.LANG_COUNTRY_VAR_AVAILABLE;
        if (locale.getVariant().isEmpty()) {
            if (locale.getCountry().isEmpty()) {
                expectedStatus = TextToSpeech.LANG_AVAILABLE;
            } else {
                expectedStatus = TextToSpeech.LANG_COUNTRY_AVAILABLE;
            }
        }
        return expectedStatus;
#endif
    return 0;
}

ECode OnGetVoices(
    /* [out] */ IList** voices)
{
    return NOERROR;
}

ECode OnGetDefaultVoiceNameFor(
    /* [in]  */ String lang,
    /* [in]  */ String country,
    /* [in]  */ String variant,
    /* [out] */ String* name)
{
    return NOERROR;
}

ECode OnLoadVoice(
    /* [in]  */ String voiceName,
    /* [out] */ Int32* voice)
{
    return NOERROR;
}

ECode OnIsValidVoiceName(
    /* [in]  */ String voiceName,
    /* [out] */ Int32* ret)
{
    return NOERROR;
}

} // namespace Tts
} // namespace Speech
} // namepsace Droid
} // namespace Elastos
