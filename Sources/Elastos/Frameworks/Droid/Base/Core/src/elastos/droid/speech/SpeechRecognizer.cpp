#include "elastos/droid/speech/SpeechRecognizer.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/content/CComponentName.h"
#include "elastos/droid/content/CIntent.h"
//#include "elastos/droid/provider/CSettings.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/CLooperHelper.h"
#include "elastos/droid/speech/CSpeechRecognizer.h"
#include "elastos/droid/os/Looper.h"
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Core::IFloat;
using Elastos::Core::CFloat;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::CString;
using Elastos::Droid::Os::CLooperHelper;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::ILooperHelper;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::IList;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::Looper;

namespace Elastos {
namespace Droid {
namespace Speech {

const Boolean SpeechRecognizer::DBG = FALSE;
const String SpeechRecognizer::TAG("SpeechRecognizer");

const Int32 SpeechRecognizer::MSG_START = 1;
const Int32 SpeechRecognizer::MSG_STOP = 2;
const Int32 SpeechRecognizer::MSG_CANCEL = 3;
const Int32 SpeechRecognizer::MSG_CHANGE_LISTENER = 4;

/******************************SpeechRecognizer::SpeechRecognizerHandler*************************/
SpeechRecognizer::SpeechRecognizerHandler::SpeechRecognizerHandler()
{}

SpeechRecognizer::SpeechRecognizerHandler::SpeechRecognizerHandler(
    /* [in] */ SpeechRecognizer* sr)
    : mHost(sr)
{}

ECode SpeechRecognizer::SpeechRecognizerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    switch (what) {
        case MSG_START:
            mHost->HandleStartListening(IIntent::Probe(obj));
            break;
        case MSG_STOP:
            mHost->HandleStopMessage();
            break;
        case MSG_CANCEL:
            mHost->HandleCancelMessage();
            break;
        case MSG_CHANGE_LISTENER:
            mHost->HandleChangeListener(IRecognitionListener::Probe(obj));
            break;
    }
    return NOERROR;
}

/******************************SpeechRecognizer::SpeechRecognizerConnection*************************/

CAR_INTERFACE_IMPL(SpeechRecognizer::SpeechRecognizerConnection, Object, IServiceConnection);

SpeechRecognizer::SpeechRecognizerConnection::SpeechRecognizerConnection()
{}

SpeechRecognizer::SpeechRecognizerConnection::SpeechRecognizerConnection(
    /* [in] */ SpeechRecognizer* sr)
{
    mHost = sr;
}

SpeechRecognizer::SpeechRecognizerConnection::~SpeechRecognizerConnection()
{}

ECode SpeechRecognizer::SpeechRecognizerConnection::constructor()
{
    return NOERROR;
}


ECode SpeechRecognizer::SpeechRecognizerConnection::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    // always done on the application main thread, so no need to send message to mHandler
    mHost->mService = (IIRecognitionService*)service;
    if (mHost->DBG) {
        Logger::D(TAG, String("onServiceConnected - Success\n"));
    }

    Boolean result;
    while (!(mHost->mPendingTasks).IsEmpty()) {
        AutoPtr<IMessage> msg = mHost->mPendingTasks.GetFront();
        mHost->mPendingTasks.PopFront();

        mHost->mHandler->SendMessage(msg, &result);
    }
    return NOERROR;
}

ECode SpeechRecognizer::SpeechRecognizerConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    // always done on the application main thread, so no need to send message to mHandler
    mHost->mService = NULL;
    mHost->mConnection = NULL;
    (mHost->mPendingTasks).Clear();
    if (mHost->DBG){
        Logger::D(TAG, String("onServiceDisconnected - Success\n"));
    }
    return NOERROR;
}

const Int32 SpeechRecognizer::SpeechRecognizerInternalListener::MSG_BEGINNING_OF_SPEECH = 1;
const Int32 SpeechRecognizer::SpeechRecognizerInternalListener::MSG_BUFFER_RECEIVED = 2;
const Int32 SpeechRecognizer::SpeechRecognizerInternalListener::MSG_END_OF_SPEECH = 3;
const Int32 SpeechRecognizer::SpeechRecognizerInternalListener::MSG_ERROR = 4;
const Int32 SpeechRecognizer::SpeechRecognizerInternalListener::MSG_READY_FOR_SPEECH = 5;
const Int32 SpeechRecognizer::SpeechRecognizerInternalListener::MSG_RESULTS = 6;
const Int32 SpeechRecognizer::SpeechRecognizerInternalListener::MSG_PARTIAL_RESULTS = 7;
const Int32 SpeechRecognizer::SpeechRecognizerInternalListener::MSG_RMS_CHANGED = 8;
const Int32 SpeechRecognizer::SpeechRecognizerInternalListener::MSG_ON_EVENT = 9;

/******************************SpeechRecognizer::SpeechRecognizerInternalListener::SpeechRecognizerInternalListenerHandler*************************/
SpeechRecognizer::SpeechRecognizerInternalListener::SpeechRecognizerInternalListenerHandler::SpeechRecognizerInternalListenerHandler()
{}

SpeechRecognizer::SpeechRecognizerInternalListener::SpeechRecognizerInternalListenerHandler::SpeechRecognizerInternalListenerHandler(
    /* [in] */ SpeechRecognizerInternalListener* sil)
    : mHost(sil)
{}

ECode SpeechRecognizer::SpeechRecognizerInternalListener::SpeechRecognizerInternalListenerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    if (mHost->mInternalListener == NULL) {
        return NOERROR;
    }

    Int32 what, arg1;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    switch (what) {
        case SpeechRecognizer::SpeechRecognizerInternalListener::MSG_BEGINNING_OF_SPEECH:
            mHost->mInternalListener->OnBeginningOfSpeech();
            break;
        case SpeechRecognizer::SpeechRecognizerInternalListener::MSG_BUFFER_RECEIVED:
            assert(0);
            // mHost->mInternalListener->OnBufferReceived((byte[]) msg.obj);
            break;
        case SpeechRecognizer::SpeechRecognizerInternalListener::MSG_END_OF_SPEECH:
            mHost->mInternalListener->OnEndOfSpeech();
            break;
        case SpeechRecognizer::SpeechRecognizerInternalListener::MSG_ERROR:
            mHost->mInternalListener->OnError(arg1);
            break;
        case SpeechRecognizer::SpeechRecognizerInternalListener::MSG_READY_FOR_SPEECH:
            mHost->mInternalListener->OnReadyForSpeech(IBundle::Probe(obj));
            break;
        case SpeechRecognizer::SpeechRecognizerInternalListener::MSG_RESULTS:
            mHost->mInternalListener->OnResults(IBundle::Probe(obj));
            break;
        case SpeechRecognizer::SpeechRecognizerInternalListener::MSG_PARTIAL_RESULTS:
            mHost->mInternalListener->OnPartialResults(IBundle::Probe(obj));
            break;
        case SpeechRecognizer::SpeechRecognizerInternalListener::MSG_RMS_CHANGED: {
                IFloat* fobj = IFloat::Probe(obj);
                Float fval;
                fobj->GetValue(&fval);
                mHost->mInternalListener->OnRmsChanged(fval);
            }
            break;
        case SpeechRecognizer::SpeechRecognizerInternalListener::MSG_ON_EVENT:
            mHost->mInternalListener->OnEvent(arg1, IBundle::Probe(obj));
            break;
    }

    return NOERROR;
}

/******************************SpeechRecognizer::SpeechRecognizerInternalListener*************************/
CAR_INTERFACE_IMPL(SpeechRecognizer::SpeechRecognizerInternalListener, Object, IIRecognitionListener)

SpeechRecognizer::SpeechRecognizerInternalListener::SpeechRecognizerInternalListener()
{
//    mInternalHandler = new SpeechRecognizerInternalListenerHandler(this);
}

SpeechRecognizer::SpeechRecognizerInternalListener::~SpeechRecognizerInternalListener()
{}

ECode SpeechRecognizer::SpeechRecognizerInternalListener::constructor()
{
    return NOERROR;
}

ECode SpeechRecognizer::SpeechRecognizerInternalListener::OnBeginningOfSpeech()
{
    Boolean result;
    return mInternalHandler->SendEmptyMessage(MSG_BEGINNING_OF_SPEECH, &result);
}

ECode SpeechRecognizer::SpeechRecognizerInternalListener::OnBufferReceived(
    /* [in] */ ArrayOf<Byte>* buffer)
{
    assert(0);
    // Message.obtain(mInternalHandler, MSG_BUFFER_RECEIVED, buffer).sendToTarget();
    // AutoPtr<IMessage> msg;
    // mInternalHandler->ObtainMessage(MSG_BUFFER_RECEIVED, obj, (IMessage**)&msg);
    // msg->SendToTarget();
    return NOERROR;
}

ECode SpeechRecognizer::SpeechRecognizerInternalListener::OnEndOfSpeech()
{
    Boolean result;
    return mInternalHandler->SendEmptyMessage(MSG_END_OF_SPEECH, &result);
}

ECode SpeechRecognizer::SpeechRecognizerInternalListener::OnError(
    /* [in] */ Int32 error)
{
    AutoPtr<IMessage> msg;
    mInternalHandler->ObtainMessage(MSG_ERROR, error, 0, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode SpeechRecognizer::SpeechRecognizerInternalListener::OnReadyForSpeech(
    /* [in] */ IBundle* noiseParams)
{
    AutoPtr<IMessage> msg;
    mInternalHandler->ObtainMessage(MSG_READY_FOR_SPEECH, noiseParams, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode SpeechRecognizer::SpeechRecognizerInternalListener::OnResults(
    /* [in] */ IBundle* results)
{
    AutoPtr<IMessage> msg;
    mInternalHandler->ObtainMessage(MSG_RESULTS, results, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode SpeechRecognizer::SpeechRecognizerInternalListener::OnPartialResults(
    /* [in] */ IBundle* results)
{
    AutoPtr<IMessage> msg;
    mInternalHandler->ObtainMessage(MSG_PARTIAL_RESULTS, results, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode SpeechRecognizer::SpeechRecognizerInternalListener::OnRmsChanged(
    /* [in] */ Float rmsdB)
{
    AutoPtr<IFloat> fobj;
    CFloat::New(rmsdB, (IFloat**)&fobj);
    AutoPtr<IMessage> msg;
    mInternalHandler->ObtainMessage(MSG_RMS_CHANGED,fobj, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode SpeechRecognizer::SpeechRecognizerInternalListener::OnEvent(
    /* [in] */ Int32 eventType,
    /* [in] */ IBundle* bParams)
{
    AutoPtr<IMessage> msg;
    mInternalHandler->ObtainMessage(MSG_ON_EVENT, eventType, eventType, bParams, (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

/******************************SpeechRecognizer*************************/
CAR_INTERFACE_IMPL(SpeechRecognizer, Object, ISpeechRecognizer);

SpeechRecognizer::SpeechRecognizer()
{
//    mHandler = new SpeechRecognizerHandler(this);
    mListener = new SpeechRecognizerInternalListener();
}

SpeechRecognizer::~SpeechRecognizer()
{}

ECode SpeechRecognizer::constructor()
{
    return NOERROR;
}

ECode SpeechRecognizer::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IComponentName* serviceComponent)
{
    mContext = context;
    mServiceComponent = serviceComponent;
//    mHandler = new SpeechRecognizerHandler(this);
    mListener = new SpeechRecognizerInternalListener();

    return NOERROR;
}

Boolean SpeechRecognizer::IsRecognitionAvailable(
    /* [in] */ IContext* context)
{
    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IIntent> intentT;
    CIntent::New(IRecognitionService::SERVICE_INTERFACE, (IIntent**)&intentT);
    AutoPtr<IList> list;
    pm -> QueryIntentServices(intentT, 0, (IList**)&list);
    Boolean b;
    list->IsEmpty(&b);
    return !b;
}

 AutoPtr<ISpeechRecognizer> SpeechRecognizer::CreateSpeechRecognizer(
     /* [in] */ IContext* context)
 {
     return CreateSpeechRecognizer(context, NULL);
 }

 AutoPtr<ISpeechRecognizer> SpeechRecognizer::CreateSpeechRecognizer(
     /* [in] */ IContext* context,
     /* [in] */ IComponentName* serviceComponent)
 {
     if (context == NULL) {
         // Java:    throw new IllegalArgumentException("Context cannot be null)");
         Logger::E(TAG, String("Context cannot be null\n"));
         return NULL;
     }
     CheckIsCalledFromMainThread();

     AutoPtr<CSpeechRecognizer> sr;
     CSpeechRecognizer::NewByFriend(context, serviceComponent, (CSpeechRecognizer**)&sr);
     return sr;
 }

ECode SpeechRecognizer::SetRecognitionListener(
    /* [in] */ IRecognitionListener* listener)
{
    CheckIsCalledFromMainThread();

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_CHANGE_LISTENER, listener, (IMessage**)&msg);
    PutMessage(msg);

    return NOERROR;
}

ECode SpeechRecognizer::StartListening(
    /* [in] */ IIntent* recognizerIntent)
{
    if (recognizerIntent == NULL) {
        //Java:    throw new IllegalArgumentException("intent must not be null");
        Logger::E(TAG, String("intent must not be null\n"));
    }
    CheckIsCalledFromMainThread();
    if (mConnection == NULL) { // first time connection
        mConnection = new SpeechRecognizerConnection(this);

        AutoPtr<IIntent> serviceIntent;
        CIntent::New(IRecognitionService::SERVICE_INTERFACE, (IIntent**)&serviceIntent);

        if (mServiceComponent == NULL) {
            AutoPtr<IContentResolver> cr;
            mContext->GetContentResolver( (IContentResolver**)&cr );
            String serviceComponent;// = CSettings::Secure::GetString(cr.Get(), ISettingsSecure::VOICE_RECOGNITION_SERVICE);

            AutoPtr<ICharSequence> cs;
            CString::New(serviceComponent, (ICharSequence**)&cs);
            if (TextUtils::IsEmpty(cs)) {
                //Java:    Log.e(TAG, "no selected voice recognition service");
                Logger::E(TAG, String("no selected voice recognition service\n"));
                mListener->OnError(ERROR_CLIENT);
                return NOERROR;
            }

            AutoPtr<IComponentName> componentNameT;
            CComponentName::UnflattenFromString(serviceComponent, (IComponentName**)&componentNameT);
            serviceIntent->SetComponent(componentNameT.Get());
        } else {
            serviceIntent->SetComponent(mServiceComponent);
        }

        Boolean bBindService;
        mContext -> BindService(serviceIntent, mConnection, IContext::BIND_AUTO_CREATE, &bBindService);

        if (!bBindService) {
            Logger::E(TAG, String("bind to recognition service failed\n"));
            mConnection = NULL;
            mService = NULL;
            mListener->OnError(ERROR_CLIENT);
            return NOERROR;
        }
    }

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_START, recognizerIntent, (IMessage**)&msg);
    PutMessage(msg);

    return NOERROR;
}

ECode SpeechRecognizer::StopListening()
{
    CheckIsCalledFromMainThread();

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_STOP, (IMessage**)&msg);
    PutMessage(msg);

    return NOERROR;
}

ECode SpeechRecognizer::Cancel()
{
    CheckIsCalledFromMainThread();
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_CANCEL, (IMessage**)&msg);
    PutMessage(msg);

    return NOERROR;
}

void SpeechRecognizer::CheckIsCalledFromMainThread()
{
    /**
     * Java:
     *   if (Looper.myLooper() != Looper.getMainLooper()) {
     *       throw new RuntimeException(
     *               "SpeechRecognizer should be used only from the application's main thread");
     *   }
     */
    AutoPtr<ILooperHelper> myLooperHelper;
    CLooperHelper::AcquireSingletonByFriend((CLooperHelper**)&myLooperHelper);
    AutoPtr<ILooper> myLooper;
    myLooper = Looper::GetMyLooper();
    //myLooperHelper->MyLooper((ILooper**)&myLooper);
    if (/*myLooper==mainLooper*/FALSE) {
        Logger::E(TAG, String("SpeechRecognizer should be used only from the application's main thread\n"));
    }
}

void SpeechRecognizer::PutMessage(
    /* [in] */ IMessage* msg)
{
    if (mService == NULL) {
        mPendingTasks.PushFront(msg);
    }
    else {
        Boolean result;
        mHandler->SendMessage(msg, &result);
    }
}

/** sends the actual message to the service */
void SpeechRecognizer::HandleStartListening(
    /* [in] */ IIntent* recognizerIntent)
{
    if (!CheckOpenConnection()) {
        return;
    }
    //try {
        mService->StartListening(recognizerIntent, mListener);
        if (DBG) {
            //Java:    Log.d(TAG, "service start listening command succeded");
            Logger::D(TAG, String("service start listening command succeded\n"));
        }
    //} catch (final RemoteException e) {
        //Java:    Log.e(TAG, "startListening() failed", e);
        /*
        Logger::E(TAG, String("startListening() failed\n"));
        mListener->OnError(ERROR_CLIENT);
        */
    //}
}

/** sends the actual message to the service */
void SpeechRecognizer::HandleStopMessage()
{
    if (!CheckOpenConnection()) {
        return;
    }
    //try {
        mService->StopListening(mListener);
        if (DBG){
            //Java:    Log.d(TAG, "service stop listening command succeded");
            Logger::D(TAG, String("service stop listening command succeded\n"));
        }
    //} catch (final RemoteException e) {
        //Java:    Log.e(TAG, "stopListening() failed", e);
        /*
        Logger::E(TAG, String("stopListening() failed\n"));
        mListener->OnError(ERROR_CLIENT);
        */
    //}
}

/** sends the actual message to the service */
void SpeechRecognizer::HandleCancelMessage()
{
    if (!CheckOpenConnection()) {
        return;
    }
    //try {
        mService->Cancel(mListener);
        if (DBG){
            //Java:    Log.d(TAG, "service cancel command succeded");
            Logger::D(TAG, String("service cancel command succeded\n"));
        }
    //} catch (final RemoteException e) {
        //Java:    Log.e(TAG, "cancel() failed", e);
        /*
        Logger::E(TAG, String("cancel() failed\n"));
        mListener->OnError(ERROR_CLIENT);
        */
    //}
}

Boolean SpeechRecognizer::CheckOpenConnection()
{
    if (mService != NULL) {
        return TRUE;
    }
    mListener->OnError(ERROR_CLIENT);
    //Java:    Log.e(TAG, "not connected to the recognition service");
    Logger::D(TAG, String("not connected to the recognition service\n"));
    return FALSE;
}

void SpeechRecognizer::HandleChangeListener(
    /* [in] */ IRecognitionListener* listener)
{
    if (DBG){
        //Java:    Log.d(TAG, "handleChangeListener, listener=" + listener);
        Logger::D(TAG, String("handleChangeListener, listener=") + String("\n"));
    }
    mListener->mInternalListener = listener;
}

ECode SpeechRecognizer::Destroy()
{
    if (mConnection != NULL) {
        mContext->UnbindService(mConnection);
    }
    mPendingTasks.Clear();
    mService = NULL;
    mConnection = NULL;
    mListener->mInternalListener = NULL;

    return NOERROR;
}

} // namespace Speech
} // namepsace Droid
} // namespace Elastos
