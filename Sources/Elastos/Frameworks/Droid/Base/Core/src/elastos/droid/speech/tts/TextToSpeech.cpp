#include "elastos/droid/speech/tts/TextToSpeech.h"
#include "elastos/droid/speech/tts/CTtsEngines.h"
#include "elastos/droid/speech/tts/UtteranceProgressListener.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/text/TextUtils.h"
//#include "elastos/droid/net/Uri.h"
//#include "elastos/droid/net/CUriBuilder.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/AutoLock.h>
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Core::StringUtils;
using Elastos::Utility::CLocale;
using Elastos::Core::CString;
using Elastos::Utility::Logging::Logger;

using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::/*IIBinder*/IBinder;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Net::IUriBuilder;

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Tts {

/******************
 * TextToSpeech::TextToSpeechEngineInfo
 *******************************************************************************************************/

CAR_INTERFACE_IMPL(TextToSpeech::TextToSpeechEngineInfo, Object, ITextToSpeechEngineInfo)

TextToSpeech::TextToSpeechEngineInfo::TextToSpeechEngineInfo()
{}

TextToSpeech::TextToSpeechEngineInfo::~TextToSpeechEngineInfo()
{}

ECode TextToSpeech::TextToSpeechEngineInfo::constructor()
{
    return NOERROR;
}

ECode TextToSpeech::TextToSpeechEngineInfo::ToString(
    /* [out] */ String* ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = String("EngineInfo{name=") + name + String("}");
    return NOERROR;
}

/******************************TextToSpeech::TextToSpeechActionR*************************/
//None

/******************************TextToSpeech::TextToSpeechActionRShutdown*************************/
TextToSpeech::TextToSpeechActionRShutdown::TextToSpeechActionRShutdown(
    /* [in] */ TextToSpeech* tts)
{
    mTts = tts;
}

Handle32 TextToSpeech::TextToSpeechActionRShutdown::Run(
    /* [in] */ IITextToSpeechService* service)
{
    service->SetCallback((/*IIBinder*/IBinder*)((mTts->GetCallerIdentity()).Get()), NULL);
    Int32 stopRet;
    service->Stop((/*IIBinder*/IBinder*)((mTts->GetCallerIdentity()).Get()), &stopRet);
    (mTts->mServiceConnection)->Disconnect();
    // Context#unbindService does not result in a call to
    // ServiceConnection#onServiceDisconnected. As a result, the
    // service ends up being destroyed (if there are no other open
    // connections to it) but the process lives on and the
    // ServiceConnection continues to refer to the destroyed service.
    //
    // This leads to tons of log spam about SynthThread being dead.
    (mTts->mServiceConnection) = NULL;
    (mTts->mCurrentEngine) = String(NULL);
    return (Handle32)NULL;
}

/******************************TextToSpeech::TextToSpeechActionRSpeak*************************/
TextToSpeech::TextToSpeechActionRSpeak::TextToSpeechActionRSpeak(
    /* [in] */ TextToSpeech* tts,
    /* [in] */ const String& text,
    /* [in] */ Int32 queueMode,
    /* [in] */ IHashMap* params)
{
    mTts = tts;
    mText = text;
    mQueueMode = queueMode;
    mParams = params;
}

Handle32 TextToSpeech::TextToSpeechActionRSpeak::Run(
    /* [in] */ IITextToSpeechService* service)
{
#if 0
    AutoPtr<IUri> utteranceUri = (*((mTts->mUtterances).Find(mText))).mSecond;
    if (utteranceUri != NULL) {
        Int32 nRet;
        service->PlayAudio((/*IIBinder*/IBinder*)((mTts->GetCallerIdentity()).Get()), utteranceUri.Get(), mQueueMode, (mTts->ConvertParamsHashMaptoBundle(mParams)).Get(), &nRet);
        return (Handle32)nRet;
    } else {
        Int32 nRet;
        service->Speak((/*IIBinder*/IBinder*)((mTts->GetCallerIdentity()).Get()), mText, mQueueMode, (mTts->ConvertParamsHashMaptoBundle(mParams)).Get(), &nRet);
        return (Handle32)nRet;
    }
#endif
    return (Handle32)NULL;
}

/******************************TextToSpeech::TextToSpeechActionRPlayEarcon*************************/
TextToSpeech::TextToSpeechActionRPlayEarcon::TextToSpeechActionRPlayEarcon(
    /* [in] */ TextToSpeech* tts,
    /* [in] */ const String& earcon,
    /* [in] */ Int32 queueMode,
    /* [in] */ IHashMap* params)
{
    mTts = tts;
    mEarcon = earcon;
    mQueueMode = queueMode;
    mParams = params;
}

Handle32 TextToSpeech::TextToSpeechActionRPlayEarcon::Run(
    /* [in] */ IITextToSpeechService* service)
{
#if 0
    AutoPtr<IUri> earconUri = (*((mTts->mEarcons).Find(mEarcon))).mSecond;
    if (earconUri == NULL) {
        return (Handle32)ITextToSpeech::TTS_ERROR;
    }
    Int32 nRet;
    service->PlayAudio((/*IIBinder*/IBinder*)((mTts->GetCallerIdentity()).Get()), earconUri, mQueueMode, (mTts->ConvertParamsHashMaptoBundle(mParams)).Get(), &nRet);
    return (Handle32)nRet;
#endif
    return (Handle32)NULL;
}

/******************************TextToSpeech::TextToSpeechActionRPlaySilence*************************/
TextToSpeech::TextToSpeechActionRPlaySilence::TextToSpeechActionRPlaySilence(
    /* [in] */ TextToSpeech* tts,
    /* [in] */ Int64 durationInMs,
    /* [in] */ Int32 queueMode,
    /* [in] */ IHashMap* params)
{
    mTts = tts;
    mDurationInMs = durationInMs;
    mQueueMode = queueMode;
    mParams = params;
}

Handle32 TextToSpeech::TextToSpeechActionRPlaySilence::Run(
    /* [in] */ IITextToSpeechService* service)
{
    Int32 nRet;
#if 0
    service->PlaySilence((/*IIBinder*/IBinder*)((mTts->GetCallerIdentity()).Get()), mDurationInMs, mQueueMode, mTts->ConvertParamsHashMaptoBundle(mParams), &nRet);
#endif
    return (Handle32)nRet;
}

/******************************TextToSpeech::TextToSpeechActionRGetFeatures*************************/
TextToSpeech::TextToSpeechActionRGetFeatures::TextToSpeechActionRGetFeatures(
    /* [in] */ TextToSpeech* tts,
    /* [in] */ ILocale* locale)
{
    mTts = tts;
    mLocale = locale;
}

Handle32 TextToSpeech::TextToSpeechActionRGetFeatures::Run(
    /* [in] */ IITextToSpeechService* service)
{
#if 0
    String strISO3Language, strISO3Country, strVariant;
    mLocale->GetISO3Language(&strISO3Language);
    mLocale->GetISO3Country(&strISO3Country);
    mLocale->GetVariant(&strVariant);
    AutoPtr< ArrayOf<String> > features;
    service->GetFeaturesForLanguage(strISO3Language, strISO3Country, strVariant, (ArrayOf<String>**)&features);
    if (features != NULL) {
        AutoPtr<IObjectContainer> oc;
        CObjectContainer::New((IObjectContainer**)&oc);
        Int32 aryLen = features->GetLength();
        String strTemp;
        for(Int32 i = 0; i<aryLen; i++)
        {
            strTemp = (*features)[i];
            AutoPtr<ICharSequence> cs;
            CString::New(strTemp, (ICharSequence**)&cs);
            oc->Add((IInterface*)(cs.Get()));
        }
        return (Handle32)(oc.Get());
    }
#endif
    return (Handle32)NULL;
}

/******************************TextToSpeech::TextToSpeechActionRIsSpeaking*************************/
TextToSpeech::TextToSpeechActionRIsSpeaking::TextToSpeechActionRIsSpeaking(
    /* [in] */ TextToSpeech* tts)
{
    mTts = tts;
}

Handle32 TextToSpeech::TextToSpeechActionRIsSpeaking::Run(
    /* [in] */ IITextToSpeechService* service)
{
    Boolean bRet;
    service->IsSpeaking(&bRet);
    return (Handle32)bRet;
}

/******************************TextToSpeech::TextToSpeechActionRStop*************************/
TextToSpeech::TextToSpeechActionRStop::TextToSpeechActionRStop(
    /* [in] */ TextToSpeech* tts)
{
    mTts = tts;
}

Handle32 TextToSpeech::TextToSpeechActionRStop::Run(
    /* [in] */ IITextToSpeechService* service)
{
    Int32 nRet;
    service->Stop((/*IIBinder*/IBinder*)((mTts->GetCallerIdentity()).Get()), &nRet);
    return (Handle32)nRet;
}

/******************************TextToSpeech::TextToSpeechActionRSetLanguage*************************/
TextToSpeech::TextToSpeechActionRSetLanguage::TextToSpeechActionRSetLanguage(
    /* [in] */ TextToSpeech* tts,
    /* [in] */ ILocale* locale)
{
    mTts = tts;
    mLocale = locale;
}

Handle32 TextToSpeech::TextToSpeechActionRSetLanguage::Run(
    /* [in] */ IITextToSpeechService* service)
{
#if 0
    if (mLocale == NULL) {
        return ITextToSpeech::LANG_NOT_SUPPORTED;
    }
    String language, country, variant;
    mLocale->GetISO3Language(&language);
    mLocale->GetISO3Country(&country);
    mLocale->GetVariant(&variant);
    // Check if the language, country, variant are available, and cache
    // the available parts.
    // Note that the language is not actually set here, instead it is cached so it
    // will be associated with all upcoming utterances.
    Int32 result;
    service->LoadLanguage(language, country, variant, &result);
    if (result >= ITextToSpeech::LANG_AVAILABLE){
        if (result < ITextToSpeech::LANG_COUNTRY_VAR_AVAILABLE) {
            variant = "";
            if (result < ITextToSpeech::LANG_COUNTRY_AVAILABLE) {
                country = "";
            }
        }
        (mTts->mParams)->PutString(ITextToSpeechEngine::KEY_PARAM_LANGUAGE, language);
        (mTts->mParams)->PutString(ITextToSpeechEngine::KEY_PARAM_COUNTRY, country);
        (mTts->mParams)->PutString(ITextToSpeechEngine::KEY_PARAM_VARIANT, variant);
    }
    return (Handle32)result;
#endif
    return (Handle32)NULL;
}

/******************************TextToSpeech::TextToSpeechActionRGetLanguage*************************/
TextToSpeech::TextToSpeechActionRGetLanguage::TextToSpeechActionRGetLanguage(
    /* [in] */ TextToSpeech* tts)
{
    mTts = tts;
}

Handle32 TextToSpeech::TextToSpeechActionRGetLanguage::Run(
    /* [in] */ IITextToSpeechService* service)
{
    AutoPtr< ArrayOf<String> > locStrings;
    service->GetLanguage((ArrayOf<String>**)&locStrings);
    if (locStrings != NULL && locStrings->GetLength() == 3) {
        AutoPtr<ILocale> localeRet;
        CLocale::New((*locStrings)[0], (*locStrings)[1], (*locStrings)[2], (ILocale**)&localeRet);
        return (Handle32)(localeRet.Get());
    }
    return (Handle32)NULL;
}

/******************************TextToSpeech::TextToSpeechActionRIsLanguageAvailable*************************/
TextToSpeech::TextToSpeechActionRIsLanguageAvailable::TextToSpeechActionRIsLanguageAvailable(
    /* [in] */ TextToSpeech* tts,
    /* [in] */ ILocale* locale)
{
    mTts = tts;
    mLocale = locale;
}

Handle32 TextToSpeech::TextToSpeechActionRIsLanguageAvailable::Run(
    /* [in] */ IITextToSpeechService* service)
{
    String language, country, variant;
    mLocale->GetISO3Language(&language);
    mLocale->GetISO3Country(&country);
    mLocale->GetVariant(&variant);
    Int32 nRet;
    service->IsLanguageAvailable(language, country, variant, &nRet);
    return (Handle32)nRet;
}

/******************************TextToSpeech::TextToSpeechActionRSynthesizeToFile*************************/
TextToSpeech::TextToSpeechActionRSynthesizeToFile::TextToSpeechActionRSynthesizeToFile(
    /* [in] */ TextToSpeech* tts,
    /* [in] */ const String& text,
    /* [in] */ IHashMap* params,
    /* [in] */ const String& filename)
{
    mTts = tts;
    mText = text;
    mParams = params;
    mFilename = filename;
}

Handle32 TextToSpeech::TextToSpeechActionRSynthesizeToFile::Run(
    /* [in] */ IITextToSpeechService* service)
{
    Int32 nRet;
#if 0
    service->SynthesizeToFile((/*IIBinder*/IBinder*)((mTts->GetCallerIdentity()).Get()), mText, mFilename, (mTts->ConvertParamsHashMaptoBundle(mParams)).Get(), &nRet);
#endif
    return (Handle32)nRet;
}

/******************
 * TextToSpeech::TextToSpeechConnection::TextToSpeechConnectionCallback
 *******************************************************************************************************/

CAR_INTERFACE_IMPL(TextToSpeech::TextToSpeechConnection::TextToSpeechConnectionCallback, Object, IITextToSpeechCallback)

TextToSpeech::TextToSpeechConnection::TextToSpeechConnectionCallback::TextToSpeechConnectionCallback()
{}

TextToSpeech::TextToSpeechConnection::TextToSpeechConnectionCallback::~TextToSpeechConnectionCallback()
{}

ECode TextToSpeech::TextToSpeechConnection::TextToSpeechConnectionCallback::constructor()
{
    return NOERROR;
}

ECode TextToSpeech::TextToSpeechConnection::TextToSpeechConnectionCallback::constructor(
    /* [in] */ TextToSpeech* tts)
{
    mTts = tts;
    return NOERROR;
}

ECode TextToSpeech::TextToSpeechConnection::TextToSpeechConnectionCallback::OnStop(
    /* [in] */ const String& utteranceId)
{
    AutoPtr<IUtteranceProgressListener> listener = mTts->mUtteranceProgressListener;
    if (listener != NULL) {
        listener->OnDone(utteranceId);
    }
    return NOERROR;
}

ECode TextToSpeech::TextToSpeechConnection::TextToSpeechConnectionCallback::OnSuccess(
    /* [in] */ const String& utteranceId)
{
    AutoPtr<IUtteranceProgressListener> listener = mTts->mUtteranceProgressListener;
    if (listener != NULL) {
        return listener->OnDone(utteranceId);
    }
    return NOERROR;
}

ECode TextToSpeech::TextToSpeechConnection::TextToSpeechConnectionCallback::OnError(
    /* [in] */ const String& utteranceId,
    /* [in] */ Int32 errorCode)
{
    AutoPtr<IUtteranceProgressListener> listener = mTts->mUtteranceProgressListener;
    if (listener != NULL) {
        return listener->OnError(utteranceId);
    }
    return NOERROR;
}

ECode TextToSpeech::TextToSpeechConnection::TextToSpeechConnectionCallback::OnStart(
    /* [in] */ const String& utteranceId)
{
    AutoPtr<IUtteranceProgressListener> listener = mTts->mUtteranceProgressListener;
    if (listener != NULL) {
        return listener->OnStart(utteranceId);
    }
    return NOERROR;
}

/******************
 * TextToSpeech::TextToSpeechConnection
 *******************************************************************************************************/

CAR_INTERFACE_IMPL(TextToSpeech::TextToSpeechConnection, Object, IServiceConnection)

TextToSpeech::TextToSpeechConnection::TextToSpeechConnection()
{}

TextToSpeech::TextToSpeechConnection::~TextToSpeechConnection()
{}

ECode TextToSpeech::TextToSpeechConnection::constructor()
{
    return NOERROR;
}

ECode TextToSpeech::TextToSpeechConnection::constructor(
    /* [in] */ TextToSpeech* pTts)
{
    AutoPtr<TextToSpeechConnectionCallback> ttscc;
    mTts = pTts;
    ttscc = new TextToSpeechConnectionCallback();
    ttscc->constructor(mTts);
    mCallback = ttscc;
    return NOERROR;
}

ECode TextToSpeech::TextToSpeechConnection::OnServiceConnected(
    /* [in] */ IComponentName* name//,
//    /* [in] */ IIBinder* service
    )
{
    //Java:    Log.i(TAG, "Connected to " + name);
    String shortStringComponentName;
    name -> ToString(&shortStringComponentName);
    Logger::I(mTts->TAG, String("Connected to ")+shortStringComponentName+String("\n"));

    AutoLock lock(mTts->mStartLock);
    if (mTts->mServiceConnection != NULL) {
        // Disconnect any previous service connection
        (mTts->mServiceConnection)->Disconnect();
    }
    mTts->mServiceConnection = this;
//    mService = (IITextToSpeechService*)(service);
    //try {
        mService->SetCallback((/*IIBinder*/IBinder*)(GetCallerIdentity().Get()), mCallback);
        mTts->DispatchOnInit(ITextToSpeech::TTS_SUCCESS);
    //} catch (RemoteException re) {
        //Java:    Log.e(TAG, "Error connecting to service, setCallback() failed");
        /*
        Logger::E(mTts->TAG, String("Error connecting to service, setCallback() failed\n"));
        mTts->DispatchOnInit(ERROR);
        */
    //}
    return NOERROR;
}

/*AutoPtr<IIBinder>*/AutoPtr<IInterface> TextToSpeech::TextToSpeechConnection::GetCallerIdentity()
{
    return mCallback;
}

Boolean TextToSpeech::TextToSpeechConnection::ClearServiceConnection()
{
    AutoLock lock(mTts->mStartLock);
    Boolean result = false;
#if 0
    if (mOnSetupConnectionAsyncTask != NULL) {
        result = mOnSetupConnectionAsyncTask->Cancel(FALSE);
        mOnSetupConnectionAsyncTask = NULL;
    }

    mService = NULL;
    // If this is the active connection, clear it
    if (mServiceConnection == this) {
        mServiceConnection = NULL;
    }
#endif
    return result;
}

ECode TextToSpeech::TextToSpeechConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    AutoLock lock(mTts->mStartLock);
    mService = NULL;
    // If this is the active connection, clear it
    if ((mTts->mServiceConnection).Get() == this) {
        mTts->mServiceConnection = NULL;
    }

    Logger::I(TAG, String("Asked to disconnect from ") + ToString(name));
    if (ClearServiceConnection()) {
        /* We need to protect against a rare case where engine
         * dies just after successful connection - and we process onServiceDisconnected
         * before OnServiceConnectedAsyncTask.onPostExecute. onServiceDisconnected cancels
         * OnServiceConnectedAsyncTask.onPostExecute and we don't call dispatchOnInit
         * with ERROR as argument.
         */
        mTts->DispatchOnInit(TTS_ERROR);
    }

    return NOERROR;
}

void TextToSpeech::TextToSpeechConnection::Disconnect()
{
    (mTts->mContext)->UnbindService(this);

    ClearServiceConnection();
}

Boolean TextToSpeech::TextToSpeechConnection::IsEstablished()
{
    return mService != NULL && mEstablished;
}

Handle32 TextToSpeech::TextToSpeechConnection::RunAction(
    /* [in] */ TextToSpeechActionR* action,
    /* [in] */ Handle32 errorResult,
    /* [in] */ const String& method,
    /* [in] */ Boolean reconnect,
    /* [in] */ Boolean onlyEstablishedConnection)
{
    AutoLock lock(mTts->mStartLock);
    //try {
        if (mService == NULL) {
            //Java:    Log.w(TAG, method + " failed: not connected to TTS engine");
            Logger::W(mTts->TAG, method + String(" failed: not connected to TTS engine\n"));
            return errorResult;
        }

        if (onlyEstablishedConnection && !IsEstablished()) {
            Logger::W(TAG, method + String(" failed: TTS engine connection not fully set up"));
            return errorResult;
        }

        return action->Run(mService);
    //} catch (RemoteException ex) {
        //Java:    Log.e(TAG, method + " failed", ex);
        Logger::E(mTts->TAG, method + String(" failed\n"));
        if (reconnect) {
            Disconnect();
            mTts->InitTts();
        }
        return errorResult;
    //}
}

/******************************TextToSpeech*************************/
const Int32 TextToSpeech::QUEUE_DESTROY = 2;
const String TextToSpeech::TAG("TextToSpeech");

CAR_INTERFACE_IMPL(TextToSpeech, Object, ITextToSpeech)

TextToSpeech::TextToSpeech()
{}

TextToSpeech::~TextToSpeech()
{}

ECode TextToSpeech::constructor()
{
    return NOERROR;
}

ECode TextToSpeech::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ITextToSpeechOnInitListener* listener)
{
    return constructor(context, listener, String(NULL));
}

ECode TextToSpeech::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ITextToSpeechOnInitListener* listener,
    /* [in] */ const String& engine)
{
    return constructor(context, listener, engine, String(NULL), TRUE);
}

ECode TextToSpeech::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ITextToSpeechOnInitListener* listener,
    /* [in] */ const String& engine,
    /* [in] */ const String& packageName,
    /* [in] */ Boolean useFallback)
{
    mContext = context;
    mInitListener = listener;
    mRequestedEngine = engine;
    mUseFallback = useFallback;

    //Java:    mEarcons = new HashMap<String, Uri>();
    //Java:    mUtterances = new HashMap<String, Uri>();
    mUtteranceProgressListener = NULL;

    AutoPtr<CTtsEngines> cttsEngines;
    CTtsEngines::NewByFriend(mContext, (CTtsEngines**)&cttsEngines);  //Java:    mEnginesHelper = new TtsEngines(mContext);
    mEnginesHelper = cttsEngines;
    if (!packageName.IsNull()) {
        mPackageName = packageName;
    } else {
        mContext->GetPackageName(&mPackageName);
    }
    InitTts();

    return NOERROR;
}

Handle32 TextToSpeech::RunActionNoReconnect(
    /* [in] */ TextToSpeechActionR* action,
    /* [in] */ Handle32 errorResult,
    /* [in] */ const String& method,
    /* [in] */ Boolean onlyEstablishedConnection)
{
    return RunAction(action, errorResult, method, FALSE, onlyEstablishedConnection);
}

Handle32 TextToSpeech::RunAction(
    /* [in] */ TextToSpeechActionR* action,
    /* [in] */ Handle32 errorResult,
    /* [in] */ const String& method)
{
    return RunAction(action, errorResult, method, TRUE, TRUE);
}

Handle32 TextToSpeech::RunAction(
    /* [in] */ TextToSpeechActionR* action,
    /* [in] */ Handle32 errorResult,
    /* [in] */ const String& method,
    /* [in] */ Boolean reconnect,
    /* [in] */ Boolean onlyEstablishedConnection)
{
    AutoLock lock(mStartLock);
    if (mServiceConnection == NULL) {
        //Java:    Log.w(TAG, method + " failed: not bound to TTS engine");
        Logger::W(TAG, method + String(" failed: not bound to TTS engine\n"));
        return errorResult;
    }
    return mServiceConnection->RunAction(action, errorResult, method, reconnect, onlyEstablishedConnection);
}

Int32 TextToSpeech::InitTts()
{
    // Step 1: Try connecting to the engine that was requested.
    if (!mRequestedEngine.IsNull()) {
        Boolean bIsEngineInstalled;
        if ((mEnginesHelper->IsEngineInstalled(mRequestedEngine, &bIsEngineInstalled), bIsEngineInstalled)) {
            if (ConnectToEngine(mRequestedEngine)) {
                mCurrentEngine = mRequestedEngine;
                return ITextToSpeech::TTS_SUCCESS;
            }
            else if (!mUseFallback) {
                mCurrentEngine = String(NULL);
                DispatchOnInit(ITextToSpeech::TTS_ERROR);
                return ITextToSpeech::TTS_ERROR;
            }
        }
        else if (!mUseFallback) {
            //Java:    Log.i(TAG, "Requested engine not installed: " + mRequestedEngine);
            Logger::I(TAG, String("Requested engine not installed: ")+mRequestedEngine+String("\n"));
            mCurrentEngine = String(NULL);
            DispatchOnInit(ITextToSpeech::TTS_ERROR);
            return ITextToSpeech::TTS_ERROR;
        }
    }

    // Step 2: Try connecting to the user's default engine.
    String defaultEngine;
    GetDefaultEngine(&defaultEngine);
    if (!defaultEngine.IsNull() && !defaultEngine.Equals(mRequestedEngine)) {
        if (ConnectToEngine(defaultEngine)) {
            mCurrentEngine = defaultEngine;
            return ITextToSpeech::TTS_SUCCESS;
        }
    }

    // Step 3: Try connecting to the highest ranked engine in the
    // system.
    String highestRanked;
    mEnginesHelper->GetHighestRankedEngineName(&highestRanked);
    if (!highestRanked.IsNull() && !highestRanked.Equals(mRequestedEngine) &&
            !highestRanked.Equals(defaultEngine)) {
        if (ConnectToEngine(highestRanked)) {
            mCurrentEngine = highestRanked;
            return ITextToSpeech::TTS_SUCCESS;
        }
    }

    // NOTE: The API currently does not allow the caller to query whether
    // they are actually connected to any engine. This might fail for various
    // reasons like if the user disables all her TTS engines.

    mCurrentEngine = String(NULL);
    DispatchOnInit(ITextToSpeech::TTS_ERROR);
    return ITextToSpeech::TTS_ERROR;
}

Boolean TextToSpeech::ConnectToEngine(
    /* [in] */ const String& engine)
{
    AutoPtr<TextToSpeechConnection> connection = /*new TextToSpeechConnection(this)*/NULL;
    AutoPtr<IIntent> intent;
    CIntent::New(ITextToSpeechEngine::INTENT_ACTION_TTS_SERVICE, (IIntent**)&intent);
    intent->SetPackage(engine);
    Boolean bound;
    mContext->BindService(intent.Get(), connection.Get(), IContext::BIND_AUTO_CREATE, &bound);
    if (!bound) {
        //Java:    Log.e(TAG, "Failed to bind to " + engine);
        Logger::E(TAG, String("Failed to bind to ")+engine+String("\n"));
        return FALSE;
    } else {
        //Java:    Log.i(TAG, "Sucessfully bound to " + engine);
        Logger::I(TAG, String("Sucessfully bound to ")+engine+String("\n"));
        return TRUE;
    }
}

void TextToSpeech::DispatchOnInit(
    /* [in] */ Int32 result)
{
    AutoLock lock(mStartLock);
    if (mInitListener != NULL) {
        mInitListener->OnInit(result);
        mInitListener = NULL;
    }
}

/*AutoPtr<IIBinder>*/AutoPtr<IInterface> TextToSpeech::GetCallerIdentity()
{
    return mServiceConnection->GetCallerIdentity();
}

ECode TextToSpeech::Shutdown()
{
    AutoPtr<TextToSpeechActionR> ttsActionR  = new TextToSpeechActionRShutdown(this);
    RunActionNoReconnect(ttsActionR.Get(), (Handle32)NULL, String("shutdown"), FALSE);

    return NOERROR;
}

ECode TextToSpeech::AddSpeech(
    /* [in] */ const String& text,
    /* [in] */ const String& packagename,
    /* [in] */ Int32 resourceId,
    /* [out] */ Int32 *ret)
{
    AutoLock lock(mStartLock);
    mUtterances.Insert(Map<String, AutoPtr<IUri> >::ValueType(text, MakeResourceUri(packagename, resourceId)) );
    *ret = ITextToSpeech::TTS_SUCCESS;
    return NOERROR;
}

ECode TextToSpeech::AddSpeech(
    /* [in] */ ICharSequence* text,
    /* [in] */ const String& packagename,
    /* [in] */ Int32 resourceId,
    /* [out] */ Int32 *ret)
{
    AutoLock lock(mStartLock);
    String str;
    text->ToString(&str);
    mUtterances.Insert(Map<String, AutoPtr<IUri> >::ValueType(str, MakeResourceUri(packagename, resourceId)) );
    *ret = ITextToSpeech::TTS_SUCCESS;
    return NOERROR;
}

ECode TextToSpeech::AddSpeech(
    /* [in] */ const String& text,
    /* [in] */ const String& filename,
    /* [out] */ Int32 *ret)
{
    AutoLock lock(mStartLock);
    AutoPtr<IUri> uri = /*Uri::Parse(filename)*/NULL;
    mUtterances.Insert(Map<String, AutoPtr<IUri> >::ValueType(text, uri.Get() ) );
    *ret = ITextToSpeech::TTS_SUCCESS;
    return NOERROR;
}

ECode TextToSpeech::AddSpeech(
    /* [in] */ ICharSequence* text,
    /* [in] */ IFile* file,
    /* [out] */ Int32 *ret)
{
    AutoLock lock(mStartLock);
    String str;
    text->ToString(&str);
    AutoPtr<IUri> uri = /*Uri::fromFile(file)*/NULL;
    mUtterances.Insert(Map<String, AutoPtr<IUri> >::ValueType(str, uri.Get() ) );
    *ret = ITextToSpeech::TTS_SUCCESS;
    return NOERROR;
}

ECode TextToSpeech::AddEarcon(
    /* [in] */ const String& earcon,
    /* [in] */ const String& packagename,
    /* [in] */ Int32 resourceId,
    /* [out] */ Int32 *ret)
{
    AutoLock lock(mStartLock);
    mEarcons.Insert(Map<String, AutoPtr<IUri> >::ValueType(earcon, MakeResourceUri(packagename, resourceId) ) );
    *ret = ITextToSpeech::TTS_SUCCESS;
    return NOERROR;
}

ECode TextToSpeech::AddEarcon(
    /* [in] */ const String& earcon,
    /* [in] */ const String& filename,
    /* [out] */ Int32 *ret)
{
    AutoLock lock(mStartLock);
    AutoPtr<IUri> uri = /*Uri::Parse(filename)*/NULL;
    mEarcons.Insert(Map<String, AutoPtr<IUri> >::ValueType(earcon, uri.Get() ) );
    *ret = ITextToSpeech::TTS_SUCCESS;
    return NOERROR;
}

ECode TextToSpeech::AddEarcon(
    /* [in] */ const String& earcon,
    /* [in] */ IFile* file,
    /* [out] */ Int32* ret)
{
    AutoLock lock(mStartLock);
    AutoPtr<IUri> uri = /*Uri::FromFile(file)*/NULL;
    mEarcons.Insert(Map<String, AutoPtr<IUri> >::ValueType(earcon, uri.Get() ) );
    *ret = ITextToSpeech::TTS_SUCCESS;
    return NOERROR;
}


AutoPtr<IUri> TextToSpeech::MakeResourceUri(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 resourceId)
{
    //Java:    return new Uri.Builder().scheme(ContentResolver.SCHEME_ANDROID_RESOURCE).encodedAuthority(packageName).appendEncodedPath(String.valueOf(resourceId)).build();
    AutoPtr<IUriBuilder> ub;
//    CUriBuilder::New((IUriBuilder**)&ub);
//    ub -> Scheme(IContentResolver::SCHEME_ANDROID_RESOURCE);
//    ub -> EncodedAuthority(packageName);
//    ub -> AppendEncodedPath(StringUtils::Int32ToString(resourceId));

    AutoPtr<IUri> uRet;
//    ub -> Build((IUri**)&uRet);
    return uRet;
}

ECode TextToSpeech::Speak(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 queueMode,
    /* [in] */ IBundle* params,
    /* [in] */ const String& utteranceId,
    /* [in] */ Int32* ret)
{
    String str;
    text->ToString(&str);

/*
    AutoPtr<TextToSpeechActionR> ttsActionR = new TextToSpeechActionRSpeak(this, str, queueMode, params);
    *ret = (Int32)RunAction(ttsActionR.Get(), ITextToSpeech::TTS_ERROR, String("speak") );
*/
    return NOERROR;
}

ECode TextToSpeech::Speak(
    /* [in] */ const String& text,
    /* [in] */ Int32 queueMode,
    /* [in] */ IHashMap* params,
    /* [in] */ Int32* ret)
{
/*
    AutoPtr<TextToSpeechActionR> ttsActionR = new TextToSpeechActionRSpeak(this, text, queueMode, params, utteranceId);
    *ret = (Int32)RunAction(ttsActionR.Get(), ITextToSpeech::TTS_ERROR, String("speak") );
*/
    return NOERROR;
}

ECode TextToSpeech::PlayEarcon(
    /* [in] */ const String& earcon,
    /* [in] */ Int32 queueMode,
    /* [in] */ IBundle* params,
    /* [in] */ const String& utteranceld,
    /* [out] */ Int32* ret)
{
/*
    *ret = PlayEarcon(earcon, queueMode, ConvertParamsHashMaptoBundle(params),
                          params == NULL ? NULL : params->Get(Engine.KEY_PARAM_UTTERANCE_ID));
*/
return NOERROR;
}

ECode TextToSpeech::PlayEarcon(
    /* [in] */ const String& earcon,
    /* [in] */ Int32 queueMode,
    /* [in] */ IHashMap* params,
    /* [out] */ Int32* ret)
{
    AutoPtr<TextToSpeechActionR> ttsActionR = new TextToSpeechActionRPlayEarcon(this, earcon, queueMode, params);
    *ret = (Int32)RunAction(ttsActionR.Get(), ITextToSpeech::TTS_ERROR, String("playEarcon") );
    return NOERROR;
}

ECode TextToSpeech::PlaySilentUtterance(
    /* [in] */ Int64 durationInMs,
    /* [in] */ Int32 queueMode,
    /* [in] */ const String& utteranceId,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret)
    assert(0 && "TODO");
    // return runAction(new Action<Integer>() {
    //     @Override
    //     public Integer run(ITextToSpeechService service) throws RemoteException {
    //         return service.playSilence(getCallerIdentity(), durationInMs,
    //                                    queueMode, utteranceId);
    //     }
    // }, ERROR, "playSilentUtterance");
    return NOERROR;
}

ECode TextToSpeech::PlaySilence(
    /* [in] */ Int64 durationInMs,
    /* [in] */ Int32 queueMode,
    /* [in] */ IHashMap* params,
    /* [out] */ Int32* ret)
{
    AutoPtr<TextToSpeechActionR> ttsActionR = new TextToSpeechActionRPlaySilence(this, durationInMs, queueMode, params);
    *ret = (Int32)RunAction(ttsActionR.Get(), ITextToSpeech::TTS_ERROR, String("playSilence") );
    return NOERROR;
}

ECode TextToSpeech::GetFeatures(
    /* [in] */ ILocale* locale,
    /* [out] */ ISet** ret)
{
#if 0
    AutoPtr<TextToSpeechActionR> ttsActionR = new TextToSpeechActionRGetFeatures(this, locale);
    AutoPtr<IObjectContainer> oc;
    oc = (IObjectContainer*)RunAction(ttsActionR.Get(), (Handle32)NULL, String("getFeatures") );

    AutoPtr<Set<String> > sRet = new Set<String>();
    if(oc != NULL) {
        AutoPtr<IObjectEnumerator> it;
        oc->GetObjectEnumerator((IObjectEnumerator**)&it);
        Boolean succeeded = FALSE;
        String strTemp;
        while(it->MoveNext(&succeeded), succeeded) {
            AutoPtr<IInterface> cs;
            it->Current((IInterface**)&cs);
            ICharSequence::Probe(cs)->ToString(&strTemp);
            sRet->Insert(strTemp);
        }
    }

    return sRet;
#endif
    return NOERROR;
}

ECode TextToSpeech::IsSpeaking(
    /* [out] */ Boolean* ret)
{
    AutoPtr<TextToSpeechActionR> ttsActionR = new TextToSpeechActionRIsSpeaking(this);
    *ret = (Boolean)RunAction(ttsActionR.Get(), FALSE, String("isSpeaking") );
    return NOERROR;
}

ECode TextToSpeech::Stop(
    /* [out] */ Int32* ret)
{
    AutoPtr<TextToSpeechActionR> ttsActionR = new TextToSpeechActionRStop(this);
    *ret = (Int32)RunAction(ttsActionR.Get(), ITextToSpeech::TTS_ERROR, String("stop") );
    return NOERROR;
}

ECode TextToSpeech::SetSpeechRate(
    /* [in] */ Float speechRate,
    /* [out] */ Int32* ret)
{
    if (speechRate > 0.0f) {
        Int32 intRate = (Int32)(speechRate * 100);
        if (intRate > 0) {
            if(TRUE){
                AutoLock lock(mStartLock);
                mParams->PutInt32(ITextToSpeechEngine::KEY_PARAM_RATE, intRate);
            }
            *ret = ITextToSpeech::TTS_SUCCESS;
            return NOERROR;
        }
    }
    *ret = ITextToSpeech::TTS_ERROR;
    return NOERROR;
}

ECode TextToSpeech::SetPitch(
    /* [in] */ Float pitch,
    /* [out] */ Int32* ret)
{
    if (pitch > 0.0f) {
        Int32 intPitch = (Int32)(pitch * 100);
        if (intPitch > 0) {
            if(TRUE){
                AutoLock lock(mStartLock);
                mParams->PutInt32(ITextToSpeechEngine::KEY_PARAM_PITCH, intPitch);
            }
            *ret = ITextToSpeech::TTS_SUCCESS;
            return NOERROR;
        }
    }
    *ret = ITextToSpeech::TTS_ERROR;
    return NOERROR;
}

ECode TextToSpeech::SetAudioAttributes(
    /* [in] */ IAudioAttributes* audioAttributes,
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret)
    assert(0 && "TODO");
    // if (audioAttributes != null) {
    //     synchronized (mStartLock) {
    //         mParams.putParcelable(Engine.KEY_PARAM_AUDIO_ATTRIBUTES,
    //             audioAttributes);
    //     }
    //     return SUCCESS;
    // }
    // return ERROR;
    return NOERROR;
}

ECode TextToSpeech::GetCurrentEngine(
    /* [out] */ String* ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = mCurrentEngine;
    return NOERROR;
}

ECode TextToSpeech::GetDefaultLanguage(
    /* [out] */ ILocale** ret)
{
    VALIDATE_NOT_NULL(ret)
    assert(0 && "TODO");
    // return runAction(new Action<Locale>() {
    //     @Override
    //     public Locale run(ITextToSpeechService service) throws RemoteException {
    //         String[] defaultLanguage = service.getClientDefaultLanguage();

    //         return new Locale(defaultLanguage[0], defaultLanguage[1], defaultLanguage[2]);
    //     }
    // }, null, "getDefaultLanguage");
    return NOERROR;
}

ECode TextToSpeech::SetLanguage(
    /* [in] */ ILocale* loc,
    /* [out] */ Int32* ret)
{
    AutoPtr<TextToSpeechActionR> ttsActionR = new TextToSpeechActionRSetLanguage(this, loc);
    *ret = (Int32)RunAction(ttsActionR.Get(), ITextToSpeech::LANG_NOT_SUPPORTED, String("setLanguage") );
    return NOERROR;
}

ECode TextToSpeech::GetLanguage(
    /* [out] */ ILocale** language)
{
    AutoPtr<TextToSpeechActionR> ttsActionR = new TextToSpeechActionRGetLanguage(this);
    AutoPtr<ILocale> lRet = (ILocale*)RunAction(ttsActionR.Get(), (Handle32)NULL, String("getLanguage") );
    lRet->Release();//???
    *language = lRet;
    return NOERROR;
}

ECode TextToSpeech::GetAvailableLanguages(
    /* [out] */ ISet** languages)
{
    VALIDATE_NOT_NULL(languages)
    assert(0 && "TODO");
    // return runAction(new Action<Set<Locale>>() {
    //     @Override
    //     public Set<Locale> run(ITextToSpeechService service) throws RemoteException {
    //         List<Voice> voices = service.getVoices();
    //         if (voices == null) {
    //             return new HashSet<Locale>();
    //         }
    //         HashSet<Locale> locales = new HashSet<Locale>();
    //         for (Voice voice : voices) {
    //             locales.add(voice.getLocale());
    //         }
    //         return locales;
    //     }
    // }, null, "getAvailableLanguages");
    return NOERROR;
}

ECode TextToSpeech::GetVoices(
    /* [out] */ ISet** voices)
{
    VALIDATE_NOT_NULL(voices)
    assert(0 && "TODO");
    // return runAction(new Action<Set<Voice>>() {
    //     @Override
    //     public Set<Voice> run(ITextToSpeechService service) throws RemoteException {
    //         List<Voice> voices = service.getVoices();
    //         return (voices != null)  ? new HashSet<Voice>(voices) : new HashSet<Voice>();
    //     }
    // }, null, "getVoices");
    return NOERROR;
}

ECode TextToSpeech::SetVoice(
    /* [in] */ IVoice* voice,
    /* [out] */ Int32* ret)
{
    assert(0 && "TODO");
    // return runAction(new Action<Integer>() {
    //     @Override
    //     public Integer run(ITextToSpeechService service) throws RemoteException {
    //         int result = service.loadVoice(getCallerIdentity(), voice.getName());
    //         if (result == SUCCESS) {
    //             mParams.putString(Engine.KEY_PARAM_VOICE_NAME, voice.getName());

    //             // Set the language/country/variant, so #getLanguage will return the voice
    //             // locale when called.
    //             String language = "";
    //             try {
    //                 language = voice.getLocale().getISO3Language();
    //             } catch (MissingResourceException e) {
    //                 Log.w(TAG, "Couldn't retrieve ISO 639-2/T language code for locale: " +
    //                         voice.getLocale(), e);
    //             }

    //             String country = "";
    //             try {
    //                 country = voice.getLocale().getISO3Country();
    //             } catch (MissingResourceException e) {
    //                 Log.w(TAG, "Couldn't retrieve ISO 3166 country code for locale: " +
    //                         voice.getLocale(), e);
    //             }
    //             mParams.putString(Engine.KEY_PARAM_LANGUAGE, language);
    //             mParams.putString(Engine.KEY_PARAM_COUNTRY, country);
    //             mParams.putString(Engine.KEY_PARAM_VARIANT, voice.getLocale().getVariant());
    //         }
    //         return result;
    //     }
    // }, LANG_NOT_SUPPORTED, "setVoice");
    return NOERROR;
}

ECode TextToSpeech::GetVoice(
    /* [out] */ IVoice** voice)
{
    VALIDATE_NOT_NULL(voice)
    assert(0 && "TODO");
    // return runAction(new Action<Voice>() {
    //     @Override
    //     public Voice run(ITextToSpeechService service) throws RemoteException {
    //         String voiceName = mParams.getString(Engine.KEY_PARAM_VOICE_NAME, "");
    //         if (TextUtils.isEmpty(voiceName)) {
    //             return null;
    //         }
    //         List<Voice> voices = service.getVoices();
    //         if (voices == null) {
    //             return null;
    //         }
    //         for (Voice voice : voices) {
    //             if (voice.getName().equals(voiceName)) {
    //                 return voice;
    //             }
    //         }
    //         return null;
    //     }
    // }, null, "getVoice");

    return NOERROR;
}

ECode TextToSpeech::GetDefaultVoice(
    /* [out] */ IVoice** voice)
{
    VALIDATE_NOT_NULL(voice)
    assert(0 && "TODO");

    // return runAction(new Action<Voice>() {
    //     @Override
    //     public Voice run(ITextToSpeechService service) throws RemoteException {

    //         String[] defaultLanguage = service.getClientDefaultLanguage();

    //         if (defaultLanguage == null || defaultLanguage.length == 0) {
    //             Log.e(TAG, "service.getClientDefaultLanguage() returned empty array");
    //             return null;
    //         }
    //         String language = defaultLanguage[0];
    //         String country = (defaultLanguage.length > 1) ? defaultLanguage[1] : "";
    //         String variant = (defaultLanguage.length > 2) ? defaultLanguage[2] : "";

    //         // Sanitize the locale using isLanguageAvailable.
    //         int result = service.isLanguageAvailable(language, country, variant);
    //         if (result >= LANG_AVAILABLE){
    //             if (result < LANG_COUNTRY_VAR_AVAILABLE) {
    //                 variant = "";
    //                 if (result < LANG_COUNTRY_AVAILABLE) {
    //                     country = "";
    //                 }
    //             }
    //         } else {
    //             // The default language is not supported.
    //             return null;
    //         }

    //         // Get the default voice name
    //         String voiceName = service.getDefaultVoiceNameFor(language, country, variant);
    //         if (TextUtils.isEmpty(voiceName)) {
    //             return null;
    //         }

    //         // Find it
    //         List<Voice> voices = service.getVoices();
    //         if (voices == null) {
    //             return null;
    //         }
    //         for (Voice voice : voices) {
    //             if (voice.getName().equals(voiceName)) {
    //                 return voice;
    //             }
    //         }
    //         return null;
    //     }
    // }, null, "getDefaultVoice");

    return NOERROR;
}

ECode TextToSpeech::IsLanguageAvailable(
    /* [in] */ ILocale* loc,
    /* [out] */ Int32* ret)
{
    AutoPtr<TextToSpeechActionR> ttsActionR = new TextToSpeechActionRIsLanguageAvailable(this, loc);
    *ret = RunAction(ttsActionR.Get(), ITextToSpeech::LANG_NOT_SUPPORTED, String("isLanguageAvailable") );

    return NOERROR;
}

ECode TextToSpeech::SynthesizeToFile(
    /* [in] */ ICharSequence* text,
    /* [in] */ IBundle* params,
    /* [in] */ IFile* filename,
    /* [in] */ const String& utteranceld,
    /* [out] */ Int32* ret)
{
#if 0
        return runAction(new Action<Integer>() {
            @Override
            public Integer run(ITextToSpeechService service) throws RemoteException {
                ParcelFileDescriptor fileDescriptor;
                int returnValue;
                try {
                    if(file.exists() && !file.canWrite()) {
                        Log.e(TAG, "Can't write to " + file);
                        return ERROR;
                    }
                    fileDescriptor = ParcelFileDescriptor.open(file,
                            ParcelFileDescriptor.MODE_WRITE_ONLY |
                            ParcelFileDescriptor.MODE_CREATE |
                            ParcelFileDescriptor.MODE_TRUNCATE);
                    returnValue = service.synthesizeToFileDescriptor(getCallerIdentity(), text,
                            fileDescriptor, getParams(params), utteranceId);
                    fileDescriptor.close();
                    return returnValue;
                } catch (FileNotFoundException e) {
                    Log.e(TAG, "Opening file " + file + " failed", e);
                    return ERROR;
                } catch (IOException e) {
                    Log.e(TAG, "Closing file " + file + " failed", e);
                    return ERROR;
                }
            }
        }, ERROR, "synthesizeToFile");
#endif
    return NOERROR;
}


ECode TextToSpeech::SynthesizeToFile(
    /* [in] */ const String& text,
    /* [in] */ IHashMap* params,
    /* [in] */ const String& filename,
    /* [out] */ Int32* ret)
{
    AutoPtr<TextToSpeechActionR> ttsActionR = new TextToSpeechActionRSynthesizeToFile(this, text, params, filename);
    *ret = (Int32)RunAction(ttsActionR.Get(), ITextToSpeech::TTS_ERROR, String("synthesizeToFile") );
    return NOERROR;
}

AutoPtr<IBundle> TextToSpeech::ConvertParamsHashMaptoBundle(
    /* [in] */ IMap* params)
{
    #if 0
    Int32 paramsLen;
    if (params != NULL && ((params->GetSize(&paramsLen), paramsLen)!=0)) {
        AutoPtr<IBundle> bundle;
        CBundle::New(mParams, (IBundle**)&bundle);
        CopyIntParam(bundle.Get(), params, ITextToSpeechEngine::KEY_PARAM_STREAM );
        CopyStringParam(bundle.Get(), params, ITextToSpeechEngine::KEY_PARAM_UTTERANCE_ID );
        CopyFloatParam(bundle.Get(), params, ITextToSpeechEngine::KEY_PARAM_VOLUME );
        CopyFloatParam(bundle.Get(), params, ITextToSpeechEngine::KEY_PARAM_PAN );

        // Copy feature strings defined by the framework.
        CopyStringParam(bundle.Get(), params, ITextToSpeechEngine::KEY_FEATURE_NETWORK_SYNTHESIS );
        CopyStringParam(bundle.Get(), params, ITextToSpeechEngine::KEY_FEATURE_EMBEDDED_SYNTHESIS );

        // Copy over all parameters that start with the name of the
        // engine that we are currently connected to. The engine is
        // free to interpret them as it chooses.
        AutoPtr<ICharSequence> cs;
        CString::New(mCurrentEngine, (ICharSequence**)&cs);
        if (!TextUtils::IsEmpty(cs)) {
            AutoPtr< ArrayOf<String> > keys;
            AutoPtr<IObjectContainer> values;
            params -> GetAllItems((ArrayOf<String>**)&keys, (IObjectContainer**)&values);
            Int32 keysLen = keys->GetLength();
            for(Int32 i=0; i<keysLen; i++)
            {
                if( ((*keys)[i]).StartWith(mCurrentEngine) )
                {
                    String val;
                    AutoPtr<IInterface> csEx;
                    params->Get((*keys)[i], (IInterface**)&csEx);
                    bundle->PutString((*keys)[i], (ICharSequence::Probe(csEx)->ToString(&val), val));
                }
            }
        }

        return bundle;
    } else {
        return mParams;
    }
#endif
    return NULL;
}

void TextToSpeech::CopyStringParam(
    /* [in] */ IBundle* bundle,
    /* [in] */ IMap* params,
    /* [in] */ const String& key)
{
    AutoPtr<IInterface> cs;
    AutoPtr<ICharSequence> skey;
    CString::New(key, (ICharSequence**)&skey);
    params->Get(skey, (IInterface**)&cs);
    String value;
    ICharSequence::Probe(cs)->ToString(&value);
    if (!value.IsNull()) {
        bundle->PutString(key, value);
    }
}

void TextToSpeech::CopyIntParam(
    /* [in] */ IBundle* bundle,
    /* [in] */ IMap* params,
    /* [in] */ const String& key)
{
    AutoPtr<IInterface> cs;
    AutoPtr<ICharSequence> skey;
    CString::New(key, (ICharSequence**)&skey);
    params->Get(skey, (IInterface**)&cs);
    String valueString;
    ICharSequence::Probe(cs)->ToString(&valueString);
    if (!TextUtils::IsEmpty(valueString)) {
        //try {
            Int32 value = StringUtils::ParseInt32(valueString);
            bundle->PutInt32(key, value);
        //} catch (NumberFormatException ex) {
            // don't set the value in the bundle
        //}
    }
}

void TextToSpeech::CopyFloatParam(
    /* [in] */ IBundle* bundle,
    /* [in] */ IMap* params,
    /* [in] */ const String& key)
{
    AutoPtr<IInterface> cs;
    AutoPtr<ICharSequence> skey;
    CString::New(key, (ICharSequence**)&skey);
    params->Get(skey, (IInterface**)&cs);
    String valueString;
    ICharSequence::Probe(cs)->ToString(&valueString);
    if (!TextUtils::IsEmpty(valueString)) {
        //try {
            Float value = StringUtils::ParseFloat(valueString);
            bundle->PutFloat(key, value);
        //} catch (NumberFormatException ex) {
            // don't set the value in the bundle
        //}
    }
}

ECode TextToSpeech::SetOnUtteranceCompletedListener(
    /* [in] */ ITextToSpeechOnUtteranceCompletedListener* listener,
    /* [out] */ Int32* ret)
{
    mUtteranceProgressListener = UtteranceProgressListener::From(listener);
    *ret = ITextToSpeech::TTS_SUCCESS;
    return NOERROR;
}

ECode TextToSpeech::SetOnUtteranceProgressListener(
    /* [in] */ IUtteranceProgressListener* listener,
    /* [out] */ Int32* ret)
{
    mUtteranceProgressListener = listener;
    *ret = ITextToSpeech::TTS_SUCCESS;
    return NOERROR;
}

ECode TextToSpeech::SetEngineByPackageName(
    /* [in] */ const String& enginePackageName,
    /* [out] */ Int32* ret)
{
    mRequestedEngine = enginePackageName;
    *ret = InitTts();
    return NOERROR;
}

ECode TextToSpeech::GetDefaultEngine(
    /* [out] */ String* engine)
{
    //mEnginesHelper->GetDefaultEngine(&engine);
    return NOERROR;
}

ECode TextToSpeech::AreDefaultsEnforced(
    /* [out] */ Boolean* enforced)
{
    *enforced = FALSE;
    return NOERROR;
}

ECode TextToSpeech::GetEngines(
    /* [out] */ IList** ret)
{
    AutoPtr<IList> engines;

    mEnginesHelper->GetEngines((IList**)&engines);
    *ret = engines;

    return NOERROR;
}

Int32 TextToSpeech::GetMaxSpeechInputLength()
{
    return 4000;
}

} // namespace Tts
} // namespace Speech
} // namepsace Droid
} // namespace Elastos
