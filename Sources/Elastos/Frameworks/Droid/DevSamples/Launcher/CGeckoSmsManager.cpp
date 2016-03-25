
#include "CGeckoSmsManager.h"
#include "GeckoAppShell.h"
#include "GeckoApp.h"
#include "GeckoRunnable.h"
#include "elastos/droid/ext/frameworkext.h"
#include "stdio.h"
#include <elastos/core/Math.h>
#include <elastos/System.h>
//#include <net/Uri.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IPendingIntentHelper;
//using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IContentValues;
//using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Os::CBundle;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Launcher {

Int32 PendingIntentUID::sUID = Math::INT32_MIN_VALUE;

Int32 PendingIntentUID::Generate()
{
    return sUID++;
}

Envelope::Envelope(
    /* [in] */ Int32 aId,
    /* [in] */ Int32 aParts)
{
    mId = aId;
    mMessageId = -1;
    mMessageTimestamp = 0;
    mError = GeckoSmsManager_kNoError;

    Int32 size = SubParts_Values_length;
    mRemainingParts = ArrayOf<Int32>::Alloc(size);
    mFailing = ArrayOf<Boolean>::Alloc(size);

    for (Int32 i=0; i<size; ++i) {
        (*mRemainingParts)[i] = aParts;
        (*mFailing)[i] = false;
    }
}

Void Envelope::DecreaseRemainingParts(
    /* [in] */ Envelope::SubParts aType)
{
    --(*mRemainingParts)[aType];

    if ((*mRemainingParts)[SENT_PART] >
        (*mRemainingParts)[DELIVERED_PART]) {
        printf("GeckoSmsManager" "Delivered more parts than we sent!?");
    }
}

Boolean Envelope::ArePartsRemaining(
    /* [in] */ Envelope::SubParts aType)
{
    return (*mRemainingParts)[aType] != 0;
}

Void Envelope::MarkAsFailed(
    /* [in] */ Envelope::SubParts aType)
{
    (*mFailing)[aType] = true;
}

Boolean Envelope::IsFailing(
    /* [in] */ Envelope::SubParts aType)
{
    return (*mFailing)[aType];
}

Int32 Envelope::GetMessageId()
{
    return mMessageId;
}

Void Envelope::SetMessageId(
    /* [in] */ Int32 aMessageId)
{
    mMessageId = aMessageId;
}

Int64 Envelope::GetMessageTimestamp()
{
    return mMessageTimestamp;
}

Void Envelope::SetMessageTimestamp(
    /* [in] */ Int64 aMessageTimestamp)
{
    mMessageTimestamp = aMessageTimestamp;
}

Int32 Envelope::GetError()
{
    return mError;
}

Void Envelope::SetError(
    /* [in] */ Int32 aError)
{
    mError = aError;
}

Envelope::~Envelope()
{
}

AutoPtr<Postman> Postman::sInstance;

Postman * Postman::GetInstance()
{
    return sInstance;
}


Int32 Postman::CreateEnvelope(
    /* [in] */ Int32 aParts)
{
    Int32 size = mEnvelopes->GetUsed();

    for (Int32 i = 0; i < size; ++i) {
        if ((*mEnvelopes)[i] == NULL) {
            (*mEnvelopes)[i] = new Envelope(i, aParts);
            return i;
        }
    }

    mEnvelopes->Append(new Envelope(size, aParts));
    return size;
}


Envelope * Postman::GetEnvelope(
    /* [in] */ Int32 aId)
{
    /*
    * We are going to create the envelope in the first empty slot in the array
    * list. If there is no empty slot, we create a new one.
    */
    if (aId < 0 || mEnvelopes->GetUsed() <= aId) {
        printf("GeckoSmsManager" "Trying to get an unknown Envelope!");
        return NULL;
    }

    AutoPtr<Envelope> envelope = (*mEnvelopes)[aId];

    if (envelope == NULL) {
        printf("GeckoSmsManager" "Trying to get an empty Envelope!");
    }
    return envelope;
}


Void Postman::DestroyEnvelope(
    /* [in] */ Int32 aId)
{
    if (aId < 0 || mEnvelopes->GetUsed() <= aId) {
        printf("GeckoSmsManager" "Trying to destroy an unknown Envelope!");
        return;
    }

    AutoPtr<Envelope> envelope = (*mEnvelopes)[aId];
    (*mEnvelopes)[aId] = NULL;

    if (envelope == NULL) {
        printf("GeckoSmsManager" "Trying to destroy an empty Envelope!");
    }
}

Postman::~Postman()
{
    BufferOf< AutoPtr<Envelope> >::Free(mEnvelopes);
}

SmsIOThread* InitSmsIOThread()
{
    return NULL;//return new SmsIOThread();
}

AutoPtr<SmsIOThread> SmsIOThread::sInstance = InitSmsIOThread();

SmsIOThread * SmsIOThread::GetInstance()
{
  return sInstance;
}


Boolean SmsIOThread::Execute(IRunnable* r)
{
    Boolean successed;
    //ASSERT_SUCCEEDED(mHandler->Post(r), &successed);
    return successed;
}


ECode SmsIOThread::Run()
{
  //Looper::Prepare();
  //ASSERT_SUCCEEDED(CHandler::New(mHandler));
  //Looper::Loop();
  return NOERROR;
}

PInterface SmsIOThread::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_IThread) {
        return this;
    }

    return NULL;
}

Object* SmsIOThread::GetSelfLock()
{
    return &mSync;
}

UInt32 SmsIOThread::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 SmsIOThread::Release()
{
    return ElRefBase::Release();
}

ECode SmsIOThread::GetInterfaceID(
    /* [in] */ IInterface* pObject,
    /* [in] */ InterfaceID* pIID)
{
    return E_NOT_IMPLEMENTED;
}

ECode SmsIOThread::CheckAccess()
{
    return E_NOT_IMPLEMENTED;
}

ECode SmsIOThread::CountStackFrames(
    /* [out] */ Int32* number)
{
    return E_NOT_IMPLEMENTED;
}

ECode SmsIOThread::Destroy()
{
    return E_NOT_IMPLEMENTED;
}

ECode SmsIOThread::GetId(
    /* [out] */ Int64* id)
{
    return E_NOT_IMPLEMENTED;
}

ECode SmsIOThread::GetName(
    /* [out] */ String* name)
{
    return E_NOT_IMPLEMENTED;
}

ECode SmsIOThread::GetPriority(
    /* [out] */ Int32* priority)
{
    return E_NOT_IMPLEMENTED;
}

ECode SmsIOThread::GetState(
    /* [out] */ ThreadState* state)
{
    return E_NOT_IMPLEMENTED;
}

ECode SmsIOThread::GetThreadGroup(
    /* [out] */ IThreadGroup** group)
{
    return E_NOT_IMPLEMENTED;
}

ECode SmsIOThread::Interrupt()
{
    return E_NOT_IMPLEMENTED;
}

ECode SmsIOThread::IsAlive(
    /* [out] */ Boolean* isAlive)
{
    return E_NOT_IMPLEMENTED;
}

ECode SmsIOThread::IsDaemon(
    /* [out] */ Boolean* isDaemon)
{
    return E_NOT_IMPLEMENTED;
}

ECode SmsIOThread::IsInterrupted(
    /* [out] */ Boolean* isInterrupted)
{
    return E_NOT_IMPLEMENTED;
}

ECode SmsIOThread::Join()
{
    return E_NOT_IMPLEMENTED;
}

ECode SmsIOThread::Join(
    /* [in] */ Int64 millis)
{
    return E_NOT_IMPLEMENTED;
}

ECode SmsIOThread::Join(
    /* [in] */ Int64 millis,
    /* [in] */ Int32 nanos)
{
    return E_NOT_IMPLEMENTED;
}

ECode SmsIOThread::Resume()
{
    return E_NOT_IMPLEMENTED;
}

ECode SmsIOThread::SetDaemon(
    /* [in] */ Boolean isDaemon)
{
    return E_NOT_IMPLEMENTED;
}

ECode SmsIOThread::SetName(
    /* [in] */ const String& threadName)
{
    return E_NOT_IMPLEMENTED;
}

ECode SmsIOThread::SetPriority(
    /* [in] */ Int32 priority)
{
    return E_NOT_IMPLEMENTED;
}

ECode SmsIOThread::Start()
{
    return Thread::Start();
}

ECode SmsIOThread::Stop()
{
    return E_NOT_IMPLEMENTED;
}

ECode SmsIOThread::Suspend()
{
    return E_NOT_IMPLEMENTED;
}

ECode SmsIOThread::Wait(
    /* [in] */ Int64 time,
    /* [in] */ Int32 frac)
{
    return E_NOT_IMPLEMENTED;
}

AutoPtr<MessagesListManager> MessagesListManager::sInstance;

MessagesListManager::MessagesListManager()
{
    mCursors = BufferOf< AutoPtr<ICursor> >::Alloc(128);
}

MessagesListManager * MessagesListManager::GetInstance()
{
  return sInstance;
}


Int32 MessagesListManager::Add(
    /* [in] */ ICursor* aCursor)
{
    Int32 size = mCursors->GetUsed();
    for (Int32 i = 0; i < size; ++i) {
        if ((*mCursors)[i] == NULL) {
            (*mCursors)[i] = aCursor;
            return i;
        }
    }

    mCursors->Append(aCursor);
    return size;
}


ICursor* MessagesListManager::Get(
    /* [in] */ Int32 aId)
{
    if (aId < 0 || mCursors->GetUsed() <= aId) {
        printf("GeckoSmsManager" "Trying to get an unknown list!");
        return NULL;
    }

    ICursor* cursor = (*mCursors)[aId];

    if (cursor == NULL) {
        printf("GeckoSmsManager" "Trying to get an empty list!");
    }

    return cursor;
}


 Void MessagesListManager::Remove(
    /* [in] */ Int32 aId)
 {
    if (aId < 0 || mCursors->GetUsed() <= aId) {
        printf("GeckoSmsManager" "Trying to destroy an unknown list!");
        return;
    }

    ICursor* cursor = (*mCursors)[aId];
    (*mCursors)[aId] = NULL;

    if (cursor == NULL) {
        printf("GeckoSmsManager" "Trying to destroy an empty list!");
        return;
    }

    ASSERT_SUCCEEDED(cursor->Close());
}


Void MessagesListManager::Clear()
{
    for (Int32 i = 0; i < mCursors->GetUsed(); ++i) {
        ICursor* c = (*mCursors)[i];
        if (c != NULL) {
            ASSERT_SUCCEEDED(c->Close());
            (*mCursors)[i] = NULL;
        }
    }
    mCursors->SetUsed(0);
}


AutoPtr<IUri> CGeckoSmsManager::kSmsContentUri;
AutoPtr<IUri> CGeckoSmsManager::kSmsSentContentUri;
ArrayOf<String>* CGeckoSmsManager::kRequiredMessageRows;

CGeckoSmsManager::CGeckoSmsManager()
{
    if (kSmsContentUri == NULL) {
        //CURI::Parse("content://sms", (IUri**)&kSmsContentUri);
    }
    if (kSmsSentContentUri == NULL) {
        //CURI::Parse("content://sms/sent", (IUri**)&kSmsSentContentUri);
    }
    if(kRequiredMessageRows == NULL)
    {
        kRequiredMessageRows = ArrayOf<String>::Alloc(5);
        (*kRequiredMessageRows)[0] = "_id";
        (*kRequiredMessageRows)[1] = "address";
        (*kRequiredMessageRows)[2] = "body";
        (*kRequiredMessageRows)[3] = "date";
        (*kRequiredMessageRows)[4] = "type";
    }

    SmsIOThread::GetInstance()->Start();
}

ECode CGeckoSmsManager::OnRecieve(
    IContext *pContext,
    IIntent *pIntent)
{
    // TODO: Try to find the receiver number to be able to populate
    //       SmsMessage.receiver.
    // TODO: Get the id and the date from the stock app saved message.
    //       Using the stock app saved message require us to wait for it to
    //       be saved which can lead to race conditions.
    String action;
    pIntent->GetAction(&action);
    if (action.Equals(GeckoSmsManager_ACTION_SMS_RECEIVED)) {
        AutoPtr<IBundle> bundle;
        pIntent->GetExtras((IBundle**)&bundle);

        if (bundle == NULL) {
          return NOERROR;
        }

        ArrayOf<ArrayOf<Byte>*>* pdus;
        bundle->Get(String("pdus"), (IInterface**)&pdus);

        for (Int32 i = 0; i < pdus->GetLength(); ++i) {
            AutoPtr<ISmsMessage> msg;
            CSmsMessage::CreateFromPdu((*pdus[i], (ISmsMessage**)&msg);
            String smsAddress;
            msg->GetDisplayOriginatingAddress(&smsAddress);
            String messageBody;
            msg->GetDisplayMessageBody(&messageBody);
            // AutoPtr<ISystem> system;
            // Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
            // Int64 now;
            // system->GetCurrentTimeMillis(&now);
            GeckoAppShell::NotifySmsReceived(smsAddress, messageBody, GetDisplayMessageBody);
        }
        return NOERROR;
    }
    if (action.Equals(GeckoSmsManager_ACTION_SMS_SENT)
        || action.Equals(GeckoSmsManager_ACTION_SMS_DELIVERED)) {

        AutoPtr<IBundle> bundle;
        pIntent->GetExtras((IBundle**)&bundle);

        Boolean hasEnvelopeId;
        bundle->ContainsKey(String("envelopeId"), &hasEnvelopeId);
        Boolean hasNumber;
        bundle->ContainsKey(String("number"), &hasNumber);
        Boolean hasMessage;
        bundle->ContainsKey(String("message"), &hasMessage);
        Boolean hasRequestId;
        bundle->ContainsKey(String("requestId"), &hasRequestId);
        Boolean hasProcessId;
        bundle->ContainsKey(String("processId"), &hasProcessId);

        if (bundle == NULL || !hasEnvelopeId || !hasNumber || !hasMessage
            || !hasRequestId || !hasProcessId) {
            printf("GeckoSmsManager" "Got an invalid ACTION_SMS_SENT/ACTION_SMS_DELIVERED!");
            return NOERROR;
        }

        Int32 envelopeId;
        bundle->GetInt32(String("envelopeId"), &envelopeId);
        AutoPtr<Postman> postman = Postman::GetInstance();

        AutoPtr<Envelope> envelope = postman->GetEnvelope(envelopeId);

        if (envelope == NULL) {
            printf("GeckoSmsManager" "Got an invalid envelope id (or Envelope has been destroyed)!");
            return NOERROR;
        }

        Envelope::SubParts part = action.Equals(GeckoSmsManager_ACTION_SMS_SENT)
            ? Envelope::SENT_PART : Envelope::DELIVERED_PART;
        envelope->DecreaseRemainingParts(part);

        Int32 resultCode;
//        GetResultCode(&resultCode);
        if ( resultCode != IActivity::RESULT_OK) {
            /*
            switch (resultCode) {
                case SmsManager_RESULT_ERROR_NULL_PDU:
                    envelope->SetError(GeckoSmsManager_kInternalError);
                    break;
                case SmsManager_RESULT_ERROR_NO_SERVICE:
                case SmsManager_RESULT_ERROR_RADIO_OFF:
                    envelope->SetError(GeckoSmsManager_kNoSignalError);
                    break;
                case SmsManager_RESULT_ERROR_GENERIC_FAILURE:
                default:
                    envelope->SetError(GeckoSmsManager_kUnknownError);
                    break;
            }
            */
            envelope->MarkAsFailed(part);
            printf("GeckoSmsManager" "SMS part sending failed!");
        }

        if (envelope->ArePartsRemaining(part)) {
            return NOERROR;
        }

        if (envelope->IsFailing(part)) {
            if (part == Envelope::SENT_PART) {
                Int32 requestId;
                bundle->GetInt32(String("requestId"), &requestId);
                Int64 processId;
                bundle->GetInt64(String("processId"), &processId);
                GeckoAppShell::NotifySmsSendFailed(
                    envelope->GetError(),
                    requestId,
                    processId);
                printf("GeckoSmsManager" "SMS sending failed!");
            }
            else {
                printf("GeckoSmsManager" "SMS failed to be delivered... is that even possible?");
            }
        }
        else {
            if (part == Envelope::SENT_PART) {
                String number;
                bundle->GetString(String("number"), &number);
                String message;
                bundle->GetString(String("message"), &message);
                AutoPtr<ISystem> system;
                Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
                Int64 timestamp;
                system->GetCurrentTimeMillis(&timestamp);

                Int32 requestId;
                bundle->GetInt32(String("requestId"), &requestId);
                Int64 processId;
                bundle->GetInt64(String("processId"), &processId);
                Int32 id = GeckoAppShell::SaveMessageInSentbox(number,message,timestamp);
                GeckoAppShell::NotifySmsSent(id,number,message,timestamp,requestId,processId);
                envelope->SetMessageId(id);
                envelope->SetMessageTimestamp(timestamp);
                printf("GeckoSmsManager" "SMS sending was successfull!");
            }
            else {
                String number;
                bundle->GetString(String("number"), &number);
                String message;
                bundle->GetString(String("message"), &message);
                GeckoAppShell::NotifySmsDelivered(
                    envelope->GetMessageId(),
                    number,
                    message,
                    envelope->GetMessageTimestamp());
                printf("GeckoSmsManager" "SMS succesfully delivered!");
            }
        }
        // Destroy the envelope object only if the SMS has been sent and delivered.
        if (!envelope->ArePartsRemaining(Envelope::SENT_PART)
            && !envelope->ArePartsRemaining(Envelope::DELIVERED_PART)) {
            postman->DestroyEnvelope(envelopeId);
        }
        return NOERROR;
    }
}

ECode CGeckoSmsManager::Start()
{
    PRINT_ENTER_LEAVE("CGeckoSmsManager::Start()");
    AutoPtr<IIntentFilter> smsFilter;
    FAIL_RETURN(CIntentFilter::New((IIntentFilter**)&smsFilter));
    FAIL_RETURN(smsFilter->AddAction(String(GeckoSmsManager_ACTION_SMS_RECEIVED)));
    FAIL_RETURN(smsFilter->AddAction(String(GeckoSmsManager_ACTION_SMS_SENT)));
    FAIL_RETURN(smsFilter->AddAction(String(GeckoSmsManager_ACTION_SMS_DELIVERED)));
    //FAIL_RETURN(GeckoApp::sAppContext->RegisterReceiver(this, smsFilter));
    return NOERROR;
}

ECode CGeckoSmsManager::Stop()
{
    return E_NOT_IMPLEMENTED;
    //return GeckoApp::sAppContext->UnregisterReceiver(this);
}

ECode CGeckoSmsManager::Shutdown()
{
    SmsIOThread::GetInstance()->Interrupt();
    MessagesListManager::GetInstance()->Clear();
    return NOERROR;
}

ECode CGeckoSmsManager::GetNumberOfMessagesForText(
    /* [in] */ const String& aText,
    /* [out] */ Int32 * pNum)
{
    AutoPtr<ISmsManager> mgr;
    //CSmsManager::GetDefault((ISmsManager**)&mgr);
    BufferOf<String>* dividedMsg;
    //mgr->DivideMessage(aText, &dividedMsg);
    *pNum = dividedMsg->GetUsed();
    return NOERROR;
}

ECode CGeckoSmsManager::Send(
    /* [in] */ const String& aNumber,
    /* [in] */ const String& aMessage,
    /* [in] */ Int32 aRequestId,
    /* [in] */ Int64 aProcessId)
{
    ECode ec;
    AutoPtr<ISmsManager> sm;
    Int32 envelopeId = Postman_kUnknownEnvelopeId;
    //CSmsManager::GetDefault((ISmsManager**)&sm);
    AutoPtr<IPendingIntentHelper> PendingIntentHelper;
//    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&PendingIntentHelper);
    AutoPtr<IIntent> sentIntent;
    ASSERT_SUCCEEDED(CIntent::New(String(GeckoSmsManager_ACTION_SMS_SENT), (IIntent**)&sentIntent));
    AutoPtr<IIntent> deliveredIntent;
    ASSERT_SUCCEEDED(CIntent::New(String(GeckoSmsManager_ACTION_SMS_DELIVERED), (IIntent**)&deliveredIntent));
    AutoPtr<IBundle> bundle;
    ASSERT_SUCCEEDED(CBundle::New((IBundle**)&bundle));
    bundle->PutString(String("number"),aNumber);
    bundle->PutString(String("message"),aMessage);
    bundle->PutInt32(String("requestId"),aRequestId);
    bundle->PutInt64(String("processId"),aProcessId);
    if (aMessage.GetLength() <= GeckoSmsManager_kMaxMessageSize) {
        envelopeId = Postman::GetInstance()->CreateEnvelope(1);
        bundle->PutInt32(String("envelopeId"), envelopeId);
        sentIntent->PutExtras(bundle);
        deliveredIntent->PutExtras(bundle);
        /*
         * There are a few things to know about getBroadcast and pending intents:
         * - the pending intents are in a shared pool maintained by the system;
         * - each pending intent is identified by a token;
         * - when a new pending intent is created, if it has the same token as
         *   another intent in the pool, one of them has to be removed.
         *
         * To prevent having a hard time because of this situation, we give a
         * unique id to all pending intents we are creating. This unique id is
         * generated by GetPendingIntentUID().
         */
        AutoPtr<IPendingIntent> sentPendingIntent;

        ec = PendingIntentHelper->GetBroadcast((IContext*)GeckoApp::sAppContext,
            PendingIntentUID::Generate(),
            sentIntent,
            IPendingIntent::FLAG_CANCEL_CURRENT, (IPendingIntent**)&sentPendingIntent);
        if (FAILED(ec)) { goto exception;}
        AutoPtr<IPendingIntent> deliveredPendingIntent;
        ec = PendingIntentHelper->GetBroadcast(
            (IContext*)GeckoApp::sAppContext,
            PendingIntentUID::Generate(),
            deliveredIntent,
            IPendingIntent::FLAG_CANCEL_CURRENT, (IPendingIntent**)&deliveredPendingIntent);
        if (FAILED(ec)) { goto exception;}
        //ec = sm->SendTextMessage(aNumber,"",aMessage,sentPendingIntent,deliveredPendingIntent);
        if (FAILED(ec)) { goto exception;}
    }
    else {
        BufferOf<String>* parts;
        //sm->DivideMessage(aMessage, *parts);
        envelopeId = Postman::GetInstance()->CreateEnvelope(parts->GetUsed());
        bundle->PutInt32(String("envelopeId"),envelopeId);
        sentIntent->PutExtras(bundle);
        deliveredIntent->PutExtras(bundle);
        BufferOf< AutoPtr<IPendingIntent> >* sentPendingIntents
            = BufferOf< AutoPtr<IPendingIntent> >::Alloc(parts->GetUsed());
        BufferOf< AutoPtr<IPendingIntent> >* deliveredPendingIntents
            = BufferOf< AutoPtr<IPendingIntent> >::Alloc(parts->GetUsed());

        for (Int32 i = 0; i < parts->GetUsed(); ++i) {
            AutoPtr<IPendingIntent> sendingPendingIntent;
            ec = PendingIntentHelper->GetBroadcast(
                GeckoApp::sAppContext,
                PendingIntentUID::Generate(),
                sentIntent,
                IPendingIntent::FLAG_CANCEL_CURRENT,
                (IPendingIntent**)&sendingPendingIntent);

            if (FAILED(ec)) { goto exception;}

            sentPendingIntents->Append(sendingPendingIntent);
            AutoPtr<IPendingIntent> deliveredPendingIntent;
            ec = PendingIntentHelper->GetBroadcast(
                GeckoApp::sAppContext,
                PendingIntentUID::Generate(),
                deliveredIntent,
                IPendingIntent::FLAG_CANCEL_CURRENT,
                (IPendingIntent**)&deliveredPendingIntent);

            if (FAILED(ec)) { goto exception;}
            deliveredPendingIntents->Append(deliveredPendingIntent);
        }

        /*ec = sm->SendMultipartTextMessage(
            aNumber,
            "",
            parts,
            sentPendingIntents,
            deliveredPendingIntents);*/

        if (FAILED(ec)) { goto exception;}
    }
    return NOERROR;

exception:
    printf("GeckoSmsManager" "Failed to send an SMS: " );
    if (envelopeId != Postman_kUnknownEnvelopeId) {
      Postman::GetInstance()->DestroyEnvelope(envelopeId);
    }
    GeckoAppShell::NotifySmsSendFailed(
        GeckoSmsManager_kUnknownError,
        aRequestId,
        aProcessId);
    return NOERROR;
}

ECode CGeckoSmsManager::SaveSentMessage(
    /* [in] */ ICharSequence* aRecipient,
    /* [in] */ ICharSequence* aBody,
    /* [in] */ IInteger32* aDate,
    /* [out] */ Int32 * pResult)
{
    ECode ec;
    AutoPtr<IContentValues> values;
    AutoPtr<IUri> uri;
    Int64 id;
//    ASSERT_SUCCEEDED(CContentValues::New((IContentValues**)&values));
    values->PutString(String("address"), aRecipient);
    values->PutString(String("body"), aBody);
    values->PutInt32(String("date"), aDate);

    AutoPtr<IContentResolver> cr;
    ec = GeckoApp::sAppContext->GetContentResolver((IContentResolver**)&cr);
    if (FAILED(ec)) { goto exception;}


    ec = cr->Insert(kSmsSentContentUri, values,(IUri**)&uri);
    if (FAILED(ec)) { goto exception;}


    //ec = CContentUris::ParseId(uri, &id);
    if (FAILED(ec)) { goto exception;}

    // The DOM API takes a 32bits unsigned Int32 for the id. It's unlikely that
    // we happen to need more than that but it doesn't cost to check.
    if (id > Math::INT32_MAX_VALUE) {
        ec = E_GECKO_ID_TOO_HIGH;
        goto exception;
    }

    *pResult = id;
    return NOERROR;

exception:
    if (ec == (ECode)E_GECKO_ID_TOO_HIGH) {
        printf("GeckoSmsManager" "The id we received is higher than the higher allowed value.");
        return ec;
    } else {
        printf("GeckoSmsManager", "Something went wrong when trying to write a sent message: ");
        return ec;
    }
}

class GetMessageRunnable: public GeckoRunnable {
private:
    Int32 mMessageId;
    Int32 mRequestId;
    Int64 mProcessId;

public:
    GetMessageRunnable(CGeckoSmsManager * host, Int32 aMessageId, Int32 aRequestId, Int32 aProcessId)
        :GeckoRunnable(host),
        mMessageId(aMessageId),
        mRequestId(aRequestId),
        mProcessId(aProcessId) {
    }

    ECode Run() {
        enum RunException{
            NotFoundException = 0x80088000,
            UnmatchingIdException,
            TooManyResultsException,
            InvalidTypeException
        };

        CGeckoSmsManager *host = (CGeckoSmsManager *)mHost;

        ECode ec;

        AutoPtr<ICursor> cursor = NULL;

        AutoPtr<IContentResolver> cr;
        AutoPtr<IUri> message;
        String sender;
        String receiver;
        String bodyString;

        ec = GeckoApp::sAppContext->GetContentResolver((IContentResolver**)&cr);
        if (FAILED(ec)) { goto exception;}

        //ec = CContentUris::WithAppendedId(host->kSmsContentUri, mMessageId, (IUri**)&message);
        if (FAILED(ec)) { goto exception;}

        ec = cr->Query((IUri*)message, host->kRequiredMessageRows,
            String(NULL), NULL, String(NULL), (ICursor**)&cursor);
        if (FAILED(ec)) { goto exception;}

        Int32 count;
        cursor->GetCount(&count);
        if (cursor == NULL || count == 0) {
            ec = NotFoundException;
            goto exception;
        }

        if (count != 1) {
            ec = TooManyResultsException;
            goto exception;
        }

        Boolean moveSuccessed;
        ec = cursor->MoveToFirst(&moveSuccessed);
        if (FAILED(ec)) { goto exception;}

        Int32 index;
        cursor->GetColumnIndex(String("_id"), &index);
        Int32 msgId;
        cursor->GetInt32(index, &msgId);
        if (msgId != mMessageId) {
            ec = UnmatchingIdException;
            goto exception;
        }

        Int32 type;
        Int32 typeIndex;
        cursor->GetColumnIndex(String("type"), &typeIndex);
        cursor->GetInt32(typeIndex, &type);

        if (type == GeckoSmsManager_kSmsTypeInbox) {
            Int32 addressIndex;
            cursor->GetColumnIndex(String("address"), &addressIndex);
            cursor->GetString(addressIndex, &sender);
        }
        else if (type == GeckoSmsManager_kSmsTypeSentbox) {
            Int32 addressIndex;
            cursor->GetColumnIndex(String("address"), &addressIndex);
            cursor->GetString(addressIndex, &receiver);
        }
        else {
            ec = InvalidTypeException;
            goto exception;
        }

        Int32 bodyIndex;
        cursor->GetColumnIndex(String("body"), &bodyIndex);
        cursor->GetString(bodyIndex, &bodyString);
        Int32 dateIndex;
        cursor->GetColumnIndex(String("date"), &dateIndex);
        Int64 dateInt64;
        cursor->GetInt64(dateIndex, &dateInt64);

        GeckoAppShell::NotifyGetSms(
            msgId,
            receiver, sender,
            bodyString,
            dateInt64,
            mRequestId,
            mProcessId);

        return NOERROR;

    exception:
        if (ec == NotFoundException) {
            printf("GeckoSmsManager" "Message id %d not found",mMessageId);
            GeckoAppShell::NotifyGetSmsFailed(GeckoSmsManager_kNotFoundError, mRequestId, mProcessId);
        }
        else if (ec == UnmatchingIdException) {
            printf("GeckoSmsManager" "Requested message id (%d) is different from the one we got.",
                 mMessageId);
            GeckoAppShell::NotifyGetSmsFailed(GeckoSmsManager_kUnknownError, mRequestId, mProcessId);
        }
        else if (ec == TooManyResultsException) {
            printf("GeckoSmsManager" "Get too many results for id %d", mMessageId);
            GeckoAppShell::NotifyGetSmsFailed(GeckoSmsManager_kUnknownError, mRequestId, mProcessId);
        }
        else if (ec == InvalidTypeException) {
            printf("GeckoSmsManager" "Message has an invalid type, we ignore it.");
            GeckoAppShell::NotifyGetSmsFailed(GeckoSmsManager_kNotFoundError, mRequestId, mProcessId);
        }
        else {
            printf("GeckoSmsManager" "Error while trying to get message: " );
            GeckoAppShell::NotifyGetSmsFailed(GeckoSmsManager_kUnknownError, mRequestId, mProcessId);
        }

        if (cursor != NULL) {
            cursor->Close();
        }

        return NOERROR;
    }
};

ECode CGeckoSmsManager::GetMessage(
    /* [in] */ Int32 aMessageId,
    /* [in] */ Int32 aRequestId,
    /* [in] */ Int64 aProcessId)
{
    if (!SmsIOThread::GetInstance()->Execute(new GetMessageRunnable(this, aMessageId, aRequestId, aProcessId))) {
      printf("GeckoSmsManager" "Failed to add GetMessageRunnable to the SmsIOThread");
      GeckoAppShell::NotifyGetSmsFailed(GeckoSmsManager_kUnknownError, aRequestId, aProcessId);
    }
    return NOERROR;
}

class DeleteMessageRunnable: public GeckoRunnable {
private:
    Int32 mMessageId;
    Int32 mRequestId;
    Int64 mProcessId;

public:
    DeleteMessageRunnable(CGeckoSmsManager * host, Int32 aMessageId, Int32 aRequestId, Int64 aProcessId):GeckoRunnable(host) {
        mMessageId = aMessageId;
        mRequestId = aRequestId;
        mProcessId = aProcessId;
    }

    ECode Run() {
        enum DeleteMessageException
        {
            TooManyResultsException = 0x80088800
        };

        AutoPtr<IUri> message;
        ECode ec;
        AutoPtr<CGeckoSmsManager> host = (CGeckoSmsManager*) mHost;

        AutoPtr<IContentResolver> cr;
        ec = GeckoApp::sAppContext->GetContentResolver((IContentResolver**)&cr);
        if (FAILED(ec)) { goto exception;}

        //ec = CContentUris::WithAppendedId(host->kSmsContentUri, mMessageId,(IUri**)&message);
        if (FAILED(ec)) { goto exception;}

        Int32 count;
        ec = cr->Delete((IUri*)message, String(NULL), NULL, &count);
        if (FAILED(ec)) { goto exception;}

        if (count > 1) {
            ec = TooManyResultsException;
            goto exception;
        }

        count = 1;
        GeckoAppShell::NotifySmsDeleted(count, mRequestId, mProcessId);

        return NOERROR;

    exception:
        if (ec == TooManyResultsException) {
            printf("GeckoSmsManager" "Delete more than one message? " );
            GeckoAppShell::NotifySmsDeleteFailed(GeckoSmsManager_kUnknownError, mRequestId, mProcessId);
        }
        else {
            printf("GeckoSmsManager""Error while trying to delete a message: ");
            GeckoAppShell::NotifySmsDeleteFailed(GeckoSmsManager_kUnknownError, mRequestId, mProcessId);
        }

        return NOERROR;
    }

};

ECode CGeckoSmsManager::DeleteMessage(
    /* [in] */ Int32 aMessageId,
    /* [in] */ Int32 aRequestId,
    /* [in] */ Int64 aProcessId)
{
    if (!SmsIOThread::GetInstance()->Execute(new DeleteMessageRunnable(this, aMessageId, aRequestId, aProcessId))) {
      printf("GeckoSmsManager" "Failed to add DeleteMessageRunnable to the SmsIOThread");
      GeckoAppShell::NotifyGetSmsFailed(
        GeckoSmsManager_kUnknownError,
        aRequestId,
        aProcessId);
    }
    return NOERROR;
}

class CreateMessageListRunnable: public GeckoRunnable {
private:
    Int64     mStartDate;
    Int64     mEndDate;
    AutoPtr<ArrayOf<String> > mNumbers;
    Int32      mNumbersCount;
    Int32      mDeliveryState;
    Boolean  mReverse;
    Int32      mRequestId;
    Int64     mProcessId;

public:
      CreateMessageListRunnable(
            CGeckoSmsManager * host,
            Int64 aStartDate,
            Int64 aEndDate,
            const ArrayOf<String>& aNumbers,
            Int32 aNumbersCount,
            Int32 aDeliveryState,
            Boolean aReverse,
            Int32 aRequestId,
            Int64 aProcessId):GeckoRunnable(host){
        mStartDate = aStartDate;
        mEndDate = aEndDate;
        mNumbers = aNumbers.Clone();
        mDeliveryState = aDeliveryState;
        mReverse = aReverse;
        mRequestId = aRequestId;
        mProcessId = aProcessId;
    }

    ECode Run() {
        enum CreateMessageListException {
            UnexpectedDeliveryStateException,
            InvalidTypeException
        };
        AutoPtr<CGeckoSmsManager> host = (CGeckoSmsManager*)mHost;

        ECode ec;
        AutoPtr<ICursor> cursor = NULL;
        Boolean closeCursor = TRUE;
        String sender;
        String receiver;
        String bodyString;
        AutoPtr<IContentResolver> cr;
        String restrictionText;
        // TODO: should use the |selectionArgs| argument in |ContentResolver.query()|.
        BufferOf_<String,128> restrictions;

        if (mStartDate != 0) {
            String in("date >= ");
            in.AppendFormat("%d", mStartDate);
            restrictions.Append(in);
        }

        if (mEndDate != 0) {
            String in("date <= ");
            in.AppendFormat("%d", mEndDate);
            restrictions.Append(in);
        }

        if (mNumbersCount > 0) {
            StringBuf_<1024> numberRestriction;
            numberRestriction<<"address IN ('" << (*mNumbers)[0]<< "'";

            for (Int32 i=1; i<mNumbersCount; ++i) {
                numberRestriction <<", '"<<(*mNumbers)[i]<<"'";
            }
            numberRestriction << ")";

            restrictions.Append(String(numberRestriction.GetPayload()));
        }

        if (mDeliveryState == GeckoSmsManager_kDeliveryStateUnknown) {
            String in("type IN ");
            in.AppendFormat("('%d', '%d')",
                GeckoSmsManager_kSmsTypeSentbox,
                GeckoSmsManager_kSmsTypeInbox);
            restrictions.Append(in);
        }
        else if (mDeliveryState == GeckoSmsManager_kDeliveryStateSent) {
            String in("type = ");
            in.AppendFormat("%d",
                GeckoSmsManager_kSmsTypeSentbox);
            restrictions.Append(in);
        }
        else if (mDeliveryState == GeckoSmsManager_kDeliveryStateReceived) {
            String in("type = ");
            in.AppendFormat("%d",
                GeckoSmsManager_kSmsTypeInbox);
            restrictions.Append(in);
        }
        else {
            ec = UnexpectedDeliveryStateException;
            goto exception;
        }

        restrictionText = restrictions.GetUsed() > 0 ? restrictions[0] : String("");

        for (Int32 i=1; i<restrictions.GetUsed(); ++i) {
            restrictionText += " AND ";
            restrictionText += restrictions[i];
        }

        GeckoApp::sAppContext->GetContentResolver((IContentResolver**)&cr);
        cr->Query((IUri*)host->kSmsContentUri, host->kRequiredMessageRows, restrictionText,
                NULL, String(mReverse ? "date DESC" : "date ASC"),
                (ICursor**)&cursor);

        Int32 count ;
        cursor->GetCount(&count);
        if (count == 0) {
            GeckoAppShell::NotifyNoMessageInList(mRequestId, mProcessId);
            return NOERROR;
        }

        Boolean moveSuccessed;
        cursor->MoveToFirst(&moveSuccessed);

        Int32 typeIndex;
        cursor->GetColumnIndex(String("type"), &typeIndex);
        Int32 type;
        cursor->GetInt32(typeIndex, &type);
        Int32 addressIndex;
        cursor->GetColumnIndex(String("address"), &addressIndex);

        if (type == GeckoSmsManager_kSmsTypeInbox) {
            cursor->GetString(addressIndex, &sender);
        }
        else if (type == GeckoSmsManager_kSmsTypeSentbox) {
            cursor->GetString(addressIndex, &receiver);
        }
        else {
            ec = UnexpectedDeliveryStateException;
            goto exception;
        }

        Int32 bodyIndex;
        cursor->GetColumnIndex(String("body"), &bodyIndex);
        cursor->GetString(bodyIndex, &bodyString);
        Int32 dateIndex;
        cursor->GetColumnIndex(String("date"), &dateIndex);
        Int64 dateInt64;
        cursor->GetInt64(dateIndex, &dateInt64);
        Int32 index;
        cursor->GetColumnIndex(String("_id"), &index);
        Int32 msgId;
        cursor->GetInt32(index, &msgId);

        Int32 listId;
        listId = MessagesListManager::GetInstance()->Add(cursor);
        closeCursor = FALSE;
        GeckoAppShell::NotifyListCreated(listId,
                                        msgId,
                                        receiver, sender,
                                        bodyString,
                                        dateInt64,
                                        mRequestId, mProcessId);
        return NOERROR;

    exception:
        if (ec == UnexpectedDeliveryStateException) {
            printf("GeckoSmsManager" "Unexcepted delivery state type: ");
            GeckoAppShell::NotifyReadingMessageListFailed(GeckoSmsManager_kUnknownError, mRequestId, mProcessId);
        }
        else {
            printf("GeckoSmsManager" "Error while trying to create a message list cursor: " );
            GeckoAppShell::NotifyReadingMessageListFailed(GeckoSmsManager_kUnknownError, mRequestId, mProcessId);
        }
        // Close the cursor if MessagesListManager isn't taking care of it.
        // We could also just check if it is in the MessagesListManager list but
        // that would be less efficient.
        if (cursor != NULL && closeCursor) {
            cursor->Close();
        }
        return NOERROR;
    }

    ~CreateMessageListRunnable()
    {
    }
};


ECode CGeckoSmsManager::CreateMessageList(
    /* [in] */ Int64 aStartDate,
    /* [in] */ Int64 aEndDate,
    /* [in] */ const ArrayOf<String> & aNumbers,
    /* [in] */ Int32 aNumbersCount,
    /* [in] */ Int32 aDeliveryState,
    /* [in] */ Boolean aReverse,
    /* [in] */ Int32 aRequestId,
    /* [in] */ Int64 aProcessId)
{
    if (!SmsIOThread::GetInstance()->Execute(
        new CreateMessageListRunnable(this,
            aStartDate, aEndDate, aNumbers, aNumbersCount, aDeliveryState,
            aReverse, aRequestId, aProcessId))) {
        printf("GeckoSmsManager" "Failed to add CreateMessageListRunnable to the SmsIOThread");
        GeckoAppShell::NotifyGetSmsFailed(
            GeckoSmsManager_kUnknownError, aRequestId, aProcessId);
    }
    return NOERROR;
}

class GetNextMessageInListRunnable: public GeckoRunnable {
private:
    Int32 mListId;
    Int32 mRequestId;
    Int64 mProcessId;

public:
    GetNextMessageInListRunnable(
                Int32 aListId,
                Int32 aRequestId,
                Int64 aProcessId)
        :GeckoRunnable(NULL) {
        mListId = aListId;
        mRequestId = aRequestId;
        mProcessId = aProcessId;
    }

    ECode Run() {
        enum GetNextMessageInListException
        {
            UnexpectedDeliveryStateException = 0x80087000
        };

        ECode ec;

        AutoPtr<ICursor> cursor = MessagesListManager::GetInstance()->Get(mListId);

        Boolean succesed;
        cursor->MoveToNext(&succesed);
        if (!succesed) {
            MessagesListManager::GetInstance()->Remove(mListId);
            GeckoAppShell::NotifyNoMessageInList(mRequestId, mProcessId);
            return NOERROR;
        }

        Int32 typeIndex;
        cursor->GetColumnIndex(String("type"), &typeIndex);
        Int32 type;
        cursor->GetInt32(typeIndex, &type);

        String sender;
        String receiver;
        String bodyString;

        Int32 addressIndex;
        cursor->GetColumnIndex(String("address"), &addressIndex);

        if (type == GeckoSmsManager_kSmsTypeInbox) {
            cursor->GetString(addressIndex, &sender);
        }
        else if (type == GeckoSmsManager_kSmsTypeSentbox) {
            cursor->GetString(addressIndex, &receiver);
        }
        else {
            ec = UnexpectedDeliveryStateException;
            goto exception;
        }

        Int32 bodyIndex;
        cursor->GetColumnIndex(String("body"), &bodyIndex);
        cursor->GetString(bodyIndex, &bodyString);
        Int32 dateIndex;
        cursor->GetColumnIndex(String("date"), &dateIndex);
        Int64 dateInt64;
        cursor->GetInt64(dateIndex, &dateInt64);
        Int32 index;
        cursor->GetColumnIndex(String("_id"), &index);
        Int32 msgId;
        cursor->GetInt32(index, &msgId);

        Int32 listId;
        listId = MessagesListManager::GetInstance()->Add(cursor);
        GeckoAppShell::NotifyGotNextMessage(msgId,receiver, sender,
                                        bodyString,dateInt64,
                                        mRequestId, mProcessId);
        return NOERROR;
    exception:
        if (ec == UnexpectedDeliveryStateException) {
            printf("GeckoSmsManager"  "Unexcepted delivery state type: " );
            GeckoAppShell::NotifyReadingMessageListFailed(
                GeckoSmsManager_kUnknownError, mRequestId, mProcessId);
        }
        else {
            printf("GeckoSmsManager" "Error while trying to get the next message of a list: ");
            GeckoAppShell::NotifyReadingMessageListFailed(GeckoSmsManager_kUnknownError, mRequestId, mProcessId);
        }
    }
};

ECode CGeckoSmsManager::GetNextMessageInList(
    /* [in] */ Int32 aListId,
    /* [in] */ Int32 aRequestId,
    /* [in] */ Int64 aProcessId)
{
    if (!SmsIOThread::GetInstance()->Execute(
        new GetNextMessageInListRunnable(aListId, aRequestId, aProcessId))) {
        printf("GeckoSmsManager" "Failed to add GetNextMessageInListRunnable to the SmsIOThread");
        GeckoAppShell::NotifyReadingMessageListFailed(
            GeckoSmsManager_kUnknownError, aRequestId, aProcessId);
    }
    return NOERROR;
}

ECode CGeckoSmsManager::ClearMessageList(
    /* [in] */ Int32 aListId)
{
    MessagesListManager::GetInstance()->Remove(aListId);
    return NOERROR;
}

AutoPtr<ISmsManager> SmsManager::sInstance;

ISmsManager* SmsManager::GetInstance()
{
    if (sInstance == NULL) {
        //CGeckoSmsManager::New((ISmsManager**)&sInstance);
    }
    return sInstance;
}

} // namespace Launcher
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
