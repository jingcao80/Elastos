
#ifndef __CGECKOSMSMANAGER_H__
#define __CGECKOSMSMANAGER_H__

#include "_CGeckoSmsManager.h"
#include "GeckoErrors.h"
#include <elastos/core/Thread.h>

//using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Core::IRunnable;
using Elastos::Core::IInteger32;
using Elastos::Core::ICharSequence;
using Elastos::Core::IThread;
using Elastos::Core::Thread;
using Elastos::Core::ThreadState;
using Elastos::Core::IThreadGroup;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ICursor;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Launcher {

/**
 * This class is returning unique ids for PendingIntent requestCode attribute.
 * There are only |Integer.MAX_VALUE - Integer.MIN_VALUE| unique IDs available,
 * and they wrap around.
 */
class PendingIntentUID
{
private:
    static Int32 sUID;

public:
    PendingIntentUID();
    static Int32 Generate();
};

#define SubParts_Values_length 2

/**
 * The envelope class contains all information that are needed to keep track of
 * a sent SMS.
 */
class Envelope:public ElRefBase
{
public:
    enum SubParts {
        SENT_PART,
        DELIVERED_PART
    };


protected:
    Int32   mId;
    Int32   mMessageId;
    Int64   mMessageTimestamp;

    /**
    * Number of sent/delivered remaining parts.
    * @note The array has much slots as SubParts items.
    */
    AutoPtr<ArrayOf<Int32> > mRemainingParts;

    /**
    * Whether sending/delivering is currently failing.
    * @note The array has much slots as SubParts items.
    */
    AutoPtr<ArrayOf<Boolean> > mFailing;

    /**
    * Error type (only for sent).
    */
    Int32 mError;

public:
    Envelope(Int32 aId, Int32 aParts);

    Void DecreaseRemainingParts(Envelope::SubParts aType);

    Boolean ArePartsRemaining(Envelope::SubParts aType);

    Void MarkAsFailed(Envelope::SubParts aType);

    Boolean IsFailing(Envelope::SubParts aType);

    Int32 GetMessageId();

    Void SetMessageId(Int32 aMessageId);

    Int64 GetMessageTimestamp();

    Void SetMessageTimestamp(Int64 aMessageTimestamp);

    Int32 GetError();

    Void SetError(Int32 aError);

    ~Envelope();
};


#define Postman_kUnknownEnvelopeId -1
/**
 * Postman class is a singleton that manages Envelope instances.
 */
class Postman:public ElRefBase {
private:
    BufferOf< AutoPtr<Envelope> >* mEnvelopes;

    static Int32 kUnknownEnvelopeId;

    static AutoPtr<Postman> sInstance;

    Postman();

public:
    static Postman * GetInstance();

    Int32 CreateEnvelope(Int32 aParts);

    Envelope * GetEnvelope(Int32 aId);

    Void DestroyEnvelope(Int32 aId);

    ~Postman();

};

class SmsIOThread : public IThread
                , public Thread
                , public ElRefBase {
private:
    AutoPtr<IHandler> mHandler;
    static AutoPtr<SmsIOThread> sInstance;
    Object mSync;

public:
    static SmsIOThread * GetInstance();

    Boolean Execute(IRunnable* r);

    ECode Run();

    CARAPI_(PInterface) Probe(
            /* [in]  */ REIID riid);

    CARAPI_(Object*) GetSelfLock();

    UInt32 AddRef();

    UInt32 Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface* pObject,
        /* [in] */ InterfaceID* pIID);

    CARAPI CheckAccess();

    CARAPI CountStackFrames(
        /* [out] */ Int32* number);

    CARAPI Destroy();

    CARAPI GetId(
        /* [out] */ Int64* id);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI GetPriority(
        /* [out] */ Int32* priority);

    CARAPI GetState(
        /* [out] */ ThreadState* state);

    CARAPI GetThreadGroup(
        /* [out] */ IThreadGroup** group);

    CARAPI Interrupt();

    CARAPI IsAlive(
        /* [out] */ Boolean* isAlive);

    CARAPI IsDaemon(
        /* [out] */ Boolean* isDaemon);

    CARAPI IsInterrupted(
        /* [out] */ Boolean* isInterrupted);

    CARAPI Join();

    CARAPI Join(
        /* [in] */ Int64 millis);

    CARAPI Join(
        /* [in] */ Int64 millis,
        /* [in] */ Int32 nanos);

    CARAPI Resume();

    CARAPI SetDaemon(
        /* [in] */ Boolean isDaemon);

    CARAPI SetName(
        /* [in] */ const String& threadName);

    CARAPI SetPriority(
        /* [in] */ Int32 priority);

    CARAPI Start();

    CARAPI Stop();

    CARAPI Suspend();

    CARAPI Wait(
        /* [in] */ Int64 time,
        /* [in] */ Int32 frac);

};

class MessagesListManager:public ElRefBase{
private:
    BufferOf< AutoPtr<ICursor> >* mCursors;

    static AutoPtr<MessagesListManager> sInstance;



public:
    MessagesListManager();

    static MessagesListManager * GetInstance();

    Int32 Add(ICursor* aCursor);

    ICursor* Get(Int32 aId);

    Void Remove(Int32 aId);

    Void Clear();
};


#define GeckoSmsManager_kDeliveryStateEndGuard 3
#define GeckoSmsManager_kDeliveryStateReceived 1
#define GeckoSmsManager_kDeliveryStateSent 0
#define GeckoSmsManager_kDeliveryStateUnknown 2
#define GeckoSmsManager_kInternalError 4
#define GeckoSmsManager_kMaxMessageSize 160
/*
* Make sure that the following error codes are in sync with the ones
* defined in dom/sms/interfaces/nsISmsRequestManager.idl. They are owned
* owned by the interface.
*/
#define GeckoSmsManager_kNoError 0
#define GeckoSmsManager_kNoSignalError 1
#define GeckoSmsManager_kNotFoundError 2
#define GeckoSmsManager_kSmsTypeInbox 1
#define GeckoSmsManager_kSmsTypeSentbox 2
#define GeckoSmsManager_kUnknownError 3
#define GeckoSmsManager_ACTION_SMS_RECEIVED "android.provider.Telephony.SMS_RECEIVED"
#define GeckoSmsManager_ACTION_SMS_SENT "org.mozilla.gecko.SMS_SENT"
#define GeckoSmsManager_ACTION_SMS_DELIVERED "org.mozilla.gecko.SMS_DELIVERED"


CarClass(CGeckoSmsManager)//:public BroadcastReceiver
{
public:
    CGeckoSmsManager();

    CARAPI Start();

    CARAPI Stop();

    CARAPI Shutdown();

    CARAPI GetNumberOfMessagesForText(
        /* [in] */ const String& aText,
        /* [out] */ Int32 * pNum);

    CARAPI Send(
        /* [in] */ const String& aNumber,
        /* [in] */ const String& aMessage,
        /* [in] */ Int32 aRequestId,
        /* [in] */ Int64 aProcessId);

    CARAPI SaveSentMessage(
        /* [in] */ ICharSequence* aRecipient,
        /* [in] */ ICharSequence* aBody,
        /* [in] */ IInteger32* aDate,
        /* [out] */ Int32 * pResult);

    CARAPI GetMessage(
        /* [in] */ Int32 aMessageId,
        /* [in] */ Int32 aRequestId,
        /* [in] */ Int64 aProcessId);

    CARAPI DeleteMessage(
        /* [in] */ Int32 aMessageId,
        /* [in] */ Int32 aRequestId,
        /* [in] */ Int64 aProcessId);

    CARAPI CreateMessageList(
        /* [in] */ Int64 aStartDate,
        /* [in] */ Int64 aEndDate,
        /* [in] */ const ArrayOf<String> & aNumbers,
        /* [in] */ Int32 aNumbersCount,
        /* [in] */ Int32 aDeliveryState,
        /* [in] */ Boolean aReverse,
        /* [in] */ Int32 aRequestId,
        /* [in] */ Int64 aProcessId);

    CARAPI GetNextMessageInList(
        /* [in] */ Int32 aListId,
        /* [in] */ Int32 aRequestId,
        /* [in] */ Int64 aProcessId);

    CARAPI ClearMessageList(
        /* [in] */ Int32 aListId);

protected:
    CARAPI OnRecieve(
    /* [in] */ IContext *pContext,
    /* [in] */ IIntent *pIntent);

private:
    static AutoPtr<IUri> kSmsContentUri;
    static AutoPtr<IUri> kSmsSentContentUri;
    static ArrayOf<String>* kRequiredMessageRows;

friend class GetMessageRunnable;
friend class CreateMessageListRunnable;
friend class DeleteMessageRunnable;
};


class SmsManager
{
private:
  static AutoPtr<ISmsManager> sInstance;

public:
  static ISmsManager* GetInstance();
};

} // namespace Launcher
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CGECKOSMSMANAGER_H__
