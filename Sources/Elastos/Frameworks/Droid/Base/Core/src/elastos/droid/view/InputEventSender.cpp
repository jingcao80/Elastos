
#include "Elastos.Droid.Os.h"
#include "elastos/droid/view/InputEventSender.h"

using Elastos::Core::CCloseGuard;

namespace Elastos {
namespace Droid {
namespace View {

//=========================================================
//                  NativeInputChannel
//=========================================================

/*typedef void (*InputChannelObjDisposeCallback)(IInputChannel* inputChannelObj,
        const sp<IInputChannel>& inputChannel, void* data)

class NativeInputChannel
{
public:
    NativeInputChannel(const sp<IInputChannel>& inputChannel);
    ~NativeInputChannel();

    inline sp<IInputChannel> getInputChannel() { return mInputChannel; }

    void setDisposeCallback(InputChannelObjDisposeCallback callback, void* data);
    void invokeAndRemoveDisposeCallback(IInputChannel* obj);

private:
    sp<IInputChannel> mInputChannel;
    InputChannelObjDisposeCallback mDisposeCallback;
    void* mDisposeData;
};

NativeInputChannel::NativeInputChannel(const sp<IInputChannel>& inputChannel) :
    mInputChannel(inputChannel), mDisposeCallback(NULL)
{}

NativeInputChannel::~NativeInputChannel() {
}

void NativeInputChannel::setDisposeCallback(InputChannelObjDisposeCallback callback, void* data)
{
    mDisposeCallback = callback;
    mDisposeData = data;
}

void NativeInputChannel::invokeAndRemoveDisposeCallback(IInputChannel obj) {
    if (mDisposeCallback) {
        mDisposeCallback(obj, mInputChannel, mDisposeData);
        mDisposeCallback = NULL;
        mDisposeData = NULL;
    }
}*/

//==========================================================
//              NativeInputEventSender
//==========================================================

/*class NativeInputEventSender
    : public LooperCallback
{
public:
    NativeInputEventSender(
        AutoPtr<IWeakReference> senderWeak,
        const sp<InputChannel>& inputChannel,
        const sp<MessageQueue>& messageQueue);

    status_t initialize();
    void dispose();
    status_t sendKeyEvent(uint32_t seq, const KeyEvent* event);
    status_t sendMotionEvent(uint32_t seq, const MotionEvent* event);

protected:
    virtual ~NativeInputEventSender();

private:
    AutoPtr<IWeakReference> mSenderWeakGlobal;
    InputPublisher mInputPublisher;
    sp<MessageQueue> mMessageQueue;
    KeyedVector<uint32_t, uint32_t> mPublishedSeqMap;
    uint32_t mNextPublishedSeq;

    const char* getInputChannelName() {
        return mInputPublisher.getChannel()->getName().string();
    }

    virtual int handleEvent(int receiveFd, int events, void* data);
    status_t receiveFinishedSignals();
};


NativeInputEventSender::NativeInputEventSender(
    IWeakReference* senderWeak,
    const sp<InputChannel>& inputChannel,
    const sp<MessageQueue>& messageQueue)
    : mSenderWeakGlobal(senderWeak)
    , mInputPublisher(inputChannel)
    , mMessageQueue(messageQueue)
    , mNextPublishedSeq(1)
{
#if DEBUG_DISPATCH_CYCLE
    Slogger::D("channel '%s' ~ Initializing input event sender.", getInputChannelName());
#endif
}

NativeInputEventSender::~NativeInputEventSender()
{
}

status_t NativeInputEventSender::initialize()
{
    int receiveFd = mInputPublisher.getChannel()->getFd();
    mMessageQueue->getLooper()->addFd(receiveFd, 0, ALOOPER_EVENT_INPUT, this, NULL);
    return OK;
}

void NativeInputEventSender::dispose()
{
#if DEBUG_DISPATCH_CYCLE
    Slogger::D("channel '%s' ~ Disposing input event sender.", getInputChannelName());
#endif

    mMessageQueue->getLooper()->removeFd(mInputPublisher.getChannel()->getFd());
}

status_t NativeInputEventSender::sendKeyEvent(uint32_t seq, const KeyEvent* event)
{
#if DEBUG_DISPATCH_CYCLE
    Slogger::D("channel '%s' ~ Sending key event, seq=%u.", getInputChannelName(), seq);
#endif

    uint32_t publishedSeq = mNextPublishedSeq++;
    status_t status = mInputPublisher.publishKeyEvent(publishedSeq,
            event->getDeviceId(), event->getSource(), event->getAction(), event->getFlags(),
            event->getKeyCode(), event->getScanCode(), event->getMetaState(),
            event->getRepeatCount(), event->getDownTime(), event->getEventTime());
    if (status) {
        ALOGW("Failed to send key event on channel '%s'.  status=%d",
                getInputChannelName(), status);
        return status;
    }
    mPublishedSeqMap.add(publishedSeq, seq);
    return OK;
}

status_t NativeInputEventSender::sendMotionEvent(uint32_t seq, const MotionEvent* event)
{
#if DEBUG_DISPATCH_CYCLE
    ALOGD("channel '%s' ~ Sending motion event, seq=%u.", getInputChannelName(), seq);
#endif

    uint32_t publishedSeq;
    for (size_t i = 0; i <= event->getHistorySize(); i++) {
        publishedSeq = mNextPublishedSeq++;
        status_t status = mInputPublisher.publishMotionEvent(publishedSeq,
                event->getDeviceId(), event->getSource(), event->getAction(), event->getFlags(),
                event->getEdgeFlags(), event->getMetaState(), event->getButtonState(),
                event->getXOffset(), event->getYOffset(),
                event->getXPrecision(), event->getYPrecision(),
                event->getDownTime(), event->getHistoricalEventTime(i),
                event->getPointerCount(), event->getPointerProperties(),
                event->getHistoricalRawPointerCoords(0, i));
        if (status) {
            ALOGW("Failed to send motion event sample on channel '%s'.  status=%d",
                    getInputChannelName(), status);
            return status;
        }
    }
    mPublishedSeqMap.add(publishedSeq, seq);
    return OK;
}

int NativeInputEventSender::handleEvent(int receiveFd, int events, void* data)
{
    if (events & (ALOOPER_EVENT_ERROR | ALOOPER_EVENT_HANGUP)) {
#if DEBUG_DISPATCH_CYCLE
        // This error typically occurs when the consumer has closed the input channel
        // as part of finishing an IME session, in which case the publisher will
        // soon be disposed as well.
        ALOGD("channel '%s' ~ Consumer closed input channel or an error occurred.  "
                "events=0x%x", getInputChannelName(), events);
#endif
        return 0; // remove the callback
    }

    if (!(events & ALOOPER_EVENT_INPUT)) {
        ALOGW("channel '%s' ~ Received spurious callback for unhandled poll event.  "
                "events=0x%x", getInputChannelName(), events);
        return 1;
    }

    JNIEnv* env = AndroidRuntime::getJNIEnv();
    status_t status = receiveFinishedSignals(env);
    mMessageQueue->raiseAndClearException(env, "handleReceiveCallback");
    return status == OK || status == NO_MEMORY ? 1 : 0;
}

status_t NativeInputEventSender::receiveFinishedSignals()
{
#if DEBUG_DISPATCH_CYCLE
    ALOGD("channel '%s' ~ Receiving finished signals.", getInputChannelName());
#endif

    ScopedLocalRef<jobject> senderObj(env, NULL);
    bool skipCallbacks = false;
    for (;;) {
        uint32_t publishedSeq;
        bool handled;
        status_t status = mInputPublisher.receiveFinishedSignal(&publishedSeq, &handled);
        if (status) {
            if (status == WOULD_BLOCK) {
                return OK;
            }
            ALOGE("channel '%s' ~ Failed to consume finished signals.  status=%d",
                    getInputChannelName(), status);
            return status;
        }

        ssize_t index = mPublishedSeqMap.indexOfKey(publishedSeq);
        if (index >= 0) {
            uint32_t seq = mPublishedSeqMap.valueAt(index);
            mPublishedSeqMap.removeItemsAt(index);

#if DEBUG_DISPATCH_CYCLE
            ALOGD("channel '%s' ~ Received finished signal, seq=%u, handled=%s, "
                    "pendingEvents=%u.",
                    getInputChannelName(), seq, handled ? "true" : "false",
                    mPublishedSeqMap.size());
#endif

            if (!skipCallbacks) {
                if (!senderObj.get()) {
                    senderObj.reset(jniGetReferent(env, mSenderWeakGlobal));
                    if (!senderObj.get()) {
                        ALOGW("channel '%s' ~ Sender object was finalized "
                                "without being disposed.", getInputChannelName());
                        return DEAD_OBJECT;
                    }
                }

                env->CallVoidMethod(senderObj.get(),
                        gInputEventSenderClassInfo.dispatchInputEventFinished,
                        jint(seq), jboolean(handled));
                if (env->ExceptionCheck()) {
                    ALOGE("Exception dispatching finished signal.");
                    skipCallbacks = true;
                }
            }
        }
    }
}*/

//==========================================================
//          NativeMessageQueue
//==========================================================

/*class NativeMessageQueue
    : public MessageQueue
{
public:
    NativeMessageQueue();
    virtual ~NativeMessageQueue();

    virtual void raiseException(const char* msg, ECode exceptionObj);

    void pollOnce(int timeoutMillis);

    void wake();

private:
    bool mInCallback;
    ECode mExceptionObj;
};


MessageQueue::MessageQueue() {
}

MessageQueue::~MessageQueue() {
}

NativeMessageQueue::NativeMessageQueue()
    : mInCallback(FALSE)
    , mExceptionObj(NULL)
{
    mLooper = Looper::getForThread();
    if (mLooper == NULL) {
        mLooper = new Looper(false);
        Looper::setForThread(mLooper);
    }
}

NativeMessageQueue::~NativeMessageQueue() {
}

void NativeMessageQueue::raiseException(const char* msg, ECode exceptionObj) {
    if (exceptionObj) {
        if (mInCallback) {
            mExceptionObj = exceptionObj);
        } else {
        }
    }
}

void NativeMessageQueue::pollOnce(int timeoutMillis) {
    mInCallback = true;
    mLooper->pollOnce(timeoutMillis);
    mInCallback = false;
    if (mExceptionObj) {
        mExceptionObj = NULL;
    }
}

void NativeMessageQueue::wake() {
    mLooper->wake();
}*/

//==========================================================

//==========================================================

const String InputEventSender::TAG("InputEventSender");

CAR_INTERFACE_IMPL(InputEventSender, Object, IInputEventSender)

InputEventSender::InputEventSender()
    : mSenderPtr(0)
{
    //mCloseGuard = CCloseGuard::Get();
}

ECode InputEventSender::constructor(
    /* [in] */ IInputChannel* inputChannel,
    /* [in] */ ILooper* looper)
{
    if (inputChannel == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (looper == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mInputChannel = inputChannel;
    looper->GetQueue((IMessageQueue**)&mMessageQueue);
    /*mSenderPtr = NativeInit(new WeakReference<InputEventSender>(this),
            inputChannel, mMessageQueue);*/

    mCloseGuard->Open(String("InputEventSender::Dispose"));
    return NOERROR;
}

ECode InputEventSender::Finalize()
{
    ECode ec = Dispose(TRUE);
    if (FAILED(ec)) {
        //super.finalize();
    }
    return ec;
}

/**
 * Disposes the receiver.
 */
ECode InputEventSender::Dispose()
{
    return Dispose(FALSE);
}

ECode InputEventSender::Dispose(
    /* [in] */ Boolean finalized)
{
    if (mCloseGuard != NULL) {
        if (finalized) {
            mCloseGuard->WarnIfOpen();
        }
        mCloseGuard->Close();
    }

    if (mSenderPtr != 0) {
        NativeDispose(mSenderPtr);
        mSenderPtr = 0;
    }
    mInputChannel = NULL;
    mMessageQueue = NULL;
    return NOERROR;
}

/**
 * Called when an input event is finished.
 *
 * @param seq The input event sequence number.
 * @param handled True if the input event was handled.
 */
ECode InputEventSender::OnInputEventFinished(
    /* [in] */ Int32 seq,
    /* [in] */ Boolean handled)
{
    return NOERROR;
}

/**
 * Sends an input event.
 * Must be called on the same Looper thread to which the sender is attached.
 *
 * @param seq The input event sequence number.
 * @param event The input event to send.
 * @return True if the entire event was sent successfully.  May return false
 * if the input channel buffer filled before all samples were dispatched.
 */
ECode InputEventSender::SendInputEvent(
    /* [in] */ Int32 seq,
    /* [in] */ IInputEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    if (event == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mSenderPtr == 0) {
        /*Log.w(TAG, "Attempted to send an input event but the input event "
                + "sender has already been disposed.");*/
        *res = FALSE;
        return NOERROR;
    }

    if (IKeyEvent::Probe(event)) {
        return NativeSendKeyEvent(mSenderPtr, seq, IKeyEvent::Probe(event));
    }
    else {
        return NativeSendMotionEvent(mSenderPtr, seq, IMotionEvent::Probe(event));
    }
}

// Called from native code.
ECode InputEventSender::DispatchInputEventFinished(
    /* [in] */ Int32 seq,
    /* [in] */ Boolean handled)
{
    return OnInputEventFinished(seq, handled);
}

ECode InputEventSender::NativeInit(
    /* [in] */ IWeakReference* senderWeak,
    /* [in] */ IInputChannel* inputChannelObj,
    /* [in] */ IMessageQueue* messageQueueObj,
    /* [out] */ Int64 res)
{
    assert(0 && "TODO");
    /*sp<InputChannel> inputChannel = GetInputChannel(inputChannelObj);

    if (inputChannel == NULL) {
        Slogger::D("InputChannel is not initialized.");
        *res = 0;
        return E_RUNTIME_EXCEPTION;
    }

    sp<MessageQueue> messageQueue = GetMessageQueue(messageQueueObj);
    if (inputChannel == NULL) {
        Slogger::D("InputChannel is not initialized.");
        *res = 0;
        return E_RUNTIME_EXCEPTION;
    }

    sp<MessageQueue> messageQueue = GetMessageQueue( messageQueueObj);
    if (messageQueue == NULL) {
        Slogger::D("MessageQueue is not initialized.");
        *res = 0;
        return E_RUNTIME_EXCEPTION;
    }

    sp<NativeInputEventSender> sender =
        new NativeInputEventSender(senderWeak, inputChannel, messageQueue);

    status_t status = sender->initialize();
    if (status) {
        String message("Failed to initialize input event sender.  status=");
        message += String(StringUtils::ToString(status));
        Slogger::D("%s\n", message.string());
        *res = 0;
        return E_RUNTIME_EXCEPTION;
    }

    sender->incStrong(gInputEventSenderClassInfo.clazz); // retain a reference for the object
    *res = reinterpret_cast<Int64>(sender.get());*/
    return NOERROR;
}

void InputEventSender::NativeDispose(
    /* [in] */ Int64 senderPtr)
{
    assert(0 && "TODO");
    /*sp<NativeInputEventSender> sender =
            reinterpret_cast<NativeInputEventSender*>(senderPtr);
    sender->dispose();
    sender->decStrong(gInputEventSenderClassInfo.clazz);*/
}

Boolean InputEventSender::NativeSendKeyEvent(
    /* [in] */ Int64 senderPtr,
    /* [in] */ Int32 seq,
    /* [in] */ IKeyEvent* eventObj)
{
    assert(0 && "TODO");
    /*sp<NativeInputEventSender> sender =
            reinterpret_cast<NativeInputEventSender*>(senderPtr);
    AutoPtr<IKeyEvent> event;
    GetKeyEvent(eventObj, (IKeyEvent**)&event);
    status_t status = sender->sendKeyEvent(seq, &event);
    return !status;*/
    return FALSE;
}

Boolean InputEventSender::NativeSendMotionEvent(
    /* [in] */ Int64 senderPtr,
    /* [in] */ Int32 seq,
    /* [in] */ IMotionEvent* eventObj)
{
    assert(0 && "TODO");
    /*sp<NativeInputEventSender> sender =
            reinterpret_cast<NativeInputEventSender*>(senderPtr);
    AutoPtr<IMotionEvent> event;
    GetMotionEvent(eventObj, (IMotionEvent**)&event);
    status_t status = sender->sendMotionEvent(seq, event);
    return !status;*/
    return FALSE;
}

AutoPtr<IInputChannel> InputEventSender::GetInputChannel(
    /* [in] */ IInputChannel* inputChannelObj)
{
    assert(0 && "TODO");
    /*Handle64 longPtr = ((InputChannel*)inputChannelObj)->mNative;
    AutoPtr<NativeInputChannel> nativeInputChannel =
        reinterpret_cast<NativeInputChannel>(longPtr);
    return nativeInputChannel != NULL ? nativeInputChannel->getInputChannel() : NULL;*/
    return NULL;
}

android::sp<android::MessageQueue>& InputEventSender::GetMessageQueue(
    /* [in] */ IMessageQueue* messageQueueObj)
{
    assert(0 && "TODO");
    /*Int64 ptr = ((CMessageQueue*)messageQueueObj)->mPtr;
    return reinterpret_cast<NativeMessageQueue*>(ptr);*/
}

ECode InputEventSender::GetKeyEvent(
    /* [in] */ IKeyEvent* eventObj,
    /* [out] */ IKeyEvent** result)
{
    assert(0 && "TODO");
    /*VALIDATE_NOT_NULL(result)
    KeyEvent* temp = (KeyEvent*)eventObj;
    CKeyEvent::New(temp->mDownTime, temp->mEventTime, temp->mAction, temp->mKeyCode, temp->mRepeatCount,
        temp->mMetaState, temp->mDeviceId, temp->mScanCode, temp->mFlags, temp->mSource, result);*/
    return NOERROR;
}

AutoPtr<IMotionEvent> InputEventSender::GetMotionEvent(
    /* [in] */ IMotionEvent* eventObj)
{
    assert(0 && "TODO");
    /*VALIDATE_NOT_NULL(result)
    MotionEvent* temp = (MotionEvent*)eventObj;
    return reinterpret_cast<IMotionEvent*>(temp->mNativePtr);*/
    return NULL;
}

} // namespace View
} // namespace Droid
} // namespace Elastos
