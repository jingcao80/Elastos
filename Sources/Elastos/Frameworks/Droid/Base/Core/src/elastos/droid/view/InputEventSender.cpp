
#include "Elastos.Droid.Os.h"
#include "elastos/droid/os/NativeMessageQueue.h"
#include "elastos/droid/view/InputEventSender.h"
#include "elastos/droid/view/InputChannel.h"
#include "elastos/droid/view/NativeInputChannel.h"
#include <elastos/utility/logging/Logger.h>

#include <input/Input.h>

using Elastos::Droid::Os::MessageQueue;
using Elastos::Droid::Os::NativeMessageQueue;
using Elastos::Core::ICloseGuardHelper;
using Elastos::Core::CCloseGuardHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace View {

#define DEBUG_DISPATCH_CYCLE 0

static const String TAG("InputEventSender");

static void CreateNativeKeyEvent(
    /* [in] */ IKeyEvent* keyEvent,
    /* [out] */ android::KeyEvent* nativeEvent)
{
    Int32 deviceId;
    IInputEvent::Probe(keyEvent)->GetDeviceId(&deviceId);
    Int32 source;
    IInputEvent::Probe(keyEvent)->GetSource(&source);
    Int32 metaState;
    keyEvent->GetMetaState(&metaState);
    Int32 action;
    keyEvent->GetAction(&action);
    Int32 keyCode;
    keyEvent->GetKeyCode(&keyCode);
    Int32 scanCode;
    keyEvent->GetScanCode(&scanCode);
    Int32 repeatCount;
    keyEvent->GetRepeatCount(&repeatCount);
    Int32 flags;
    keyEvent->GetFlags(&flags);
    Int64 downTime, eventTime;
    keyEvent->GetDownTime(&downTime);
    IInputEvent::Probe(keyEvent)->GetEventTime(&eventTime);

    nativeEvent->initialize(deviceId, source, action, flags, keyCode, scanCode, metaState, repeatCount,
            milliseconds_to_nanoseconds(downTime),
            milliseconds_to_nanoseconds(eventTime));
}

//==========================================================
//              NativeInputEventSender
//==========================================================

class NativeInputEventSender : public android::LooperCallback
{
public:
    NativeInputEventSender(
        IWeakReference* senderWeak,
        const android::sp<android::InputChannel>& inputChannel,
        MessageQueue* messageQueue);

    android::status_t initialize();
    void dispose();
    android::status_t sendKeyEvent(uint32_t seq, const android::KeyEvent* event);
    android::status_t sendMotionEvent(uint32_t seq, const android::MotionEvent* event);

protected:
    virtual ~NativeInputEventSender();

private:
    AutoPtr<IWeakReference> mSenderWeakGlobal;
    android::InputPublisher mInputPublisher;
    AutoPtr<MessageQueue> mMessageQueue;
    android::KeyedVector<uint32_t, uint32_t> mPublishedSeqMap;
    uint32_t mNextPublishedSeq;

    const char* getInputChannelName() {
        return mInputPublisher.getChannel()->getName().string();
    }

    virtual int handleEvent(int receiveFd, int events, void* data);
    android::status_t receiveFinishedSignals();
};

NativeInputEventSender::NativeInputEventSender(
    IWeakReference* senderWeak,
    const android::sp<android::InputChannel>& inputChannel,
    MessageQueue* messageQueue)
    : mSenderWeakGlobal(senderWeak)
    , mInputPublisher(inputChannel)
    , mMessageQueue(messageQueue)
    , mNextPublishedSeq(1)
{
#if DEBUG_DISPATCH_CYCLE
    Logger::D("channel '%s' ~ Initializing input event sender.", getInputChannelName());
#endif
}

NativeInputEventSender::~NativeInputEventSender()
{
}

android::status_t NativeInputEventSender::initialize()
{
    int receiveFd = mInputPublisher.getChannel()->getFd();
    mMessageQueue->GetLooper()->addFd(receiveFd, 0, ALOOPER_EVENT_INPUT, this, NULL);
    return android::OK;
}

void NativeInputEventSender::dispose()
{
#if DEBUG_DISPATCH_CYCLE
    Logger::D(TAG, "channel '%s' ~ Disposing input event sender.", getInputChannelName());
#endif

    mMessageQueue->GetLooper()->removeFd(mInputPublisher.getChannel()->getFd());
}

android::status_t NativeInputEventSender::sendKeyEvent(uint32_t seq, const android::KeyEvent* event)
{
#if DEBUG_DISPATCH_CYCLE
    Logger::D(TAG, "channel '%s' ~ Sending key event, seq=%u.", getInputChannelName(), seq);
#endif

    uint32_t publishedSeq = mNextPublishedSeq++;
    android::status_t status = mInputPublisher.publishKeyEvent(publishedSeq,
            event->getDeviceId(), event->getSource(), event->getAction(), event->getFlags(),
            event->getKeyCode(), event->getScanCode(), event->getMetaState(),
            event->getRepeatCount(), event->getDownTime(), event->getEventTime());
    if (status) {
        Logger::W(TAG, "Failed to send key event on channel '%s'.  status=%d",
                getInputChannelName(), status);
        return status;
    }
    mPublishedSeqMap.add(publishedSeq, seq);
    return android::OK;
}

android::status_t NativeInputEventSender::sendMotionEvent(uint32_t seq, const android::MotionEvent* event)
{
#if DEBUG_DISPATCH_CYCLE
    Logger::D(TAG, "channel '%s' ~ Sending motion event, seq=%u.", getInputChannelName(), seq);
#endif

    uint32_t publishedSeq;
    for (size_t i = 0; i <= event->getHistorySize(); i++) {
        publishedSeq = mNextPublishedSeq++;
        android::status_t status = mInputPublisher.publishMotionEvent(publishedSeq,
                event->getDeviceId(), event->getSource(), event->getAction(), event->getFlags(),
                event->getEdgeFlags(), event->getMetaState(), event->getButtonState(),
                event->getXOffset(), event->getYOffset(),
                event->getXPrecision(), event->getYPrecision(),
                event->getDownTime(), event->getHistoricalEventTime(i),
                event->getPointerCount(), event->getPointerProperties(),
                event->getHistoricalRawPointerCoords(0, i));
        if (status) {
            Logger::W(TAG, "Failed to send motion event sample on channel '%s'.  status=%d",
                    getInputChannelName(), status);
            return status;
        }
    }
    mPublishedSeqMap.add(publishedSeq, seq);
    return android::OK;
}

int NativeInputEventSender::handleEvent(int receiveFd, int events, void* data)
{
    if (events & (ALOOPER_EVENT_ERROR | ALOOPER_EVENT_HANGUP)) {
#if DEBUG_DISPATCH_CYCLE
        // This error typically occurs when the consumer has closed the input channel
        // as part of finishing an IME session, in which case the publisher will
        // soon be disposed as well.
        Logger::D(TAG, "channel '%s' ~ Consumer closed input channel or an error occurred.  "
                "events=0x%x", getInputChannelName(), events);
#endif
        return 0; // remove the callback
    }

    if (!(events & ALOOPER_EVENT_INPUT)) {
        Logger::W(TAG, "channel '%s' ~ Received spurious callback for unhandled poll event.  "
                "events=0x%x", getInputChannelName(), events);
        return 1;
    }

    android::status_t status = receiveFinishedSignals();
    return status == android::OK || status == android::NO_MEMORY ? 1 : 0;
}

android::status_t NativeInputEventSender::receiveFinishedSignals()
{
#if DEBUG_DISPATCH_CYCLE
    Logger::D(TAG, "channel '%s' ~ Receiving finished signals.", getInputChannelName());
#endif

    AutoPtr<IInputEventSender> senderObj;
    bool skipCallbacks = false;
    for (;;) {
        uint32_t publishedSeq;
        bool handled;
        android::status_t status = mInputPublisher.receiveFinishedSignal(&publishedSeq, &handled);
        if (status) {
            if (status == android::WOULD_BLOCK) {
                return android::OK;
            }
            Logger::E(TAG, "channel '%s' ~ Failed to consume finished signals.  status=%d",
                    getInputChannelName(), status);
            return status;
        }

        ssize_t index = mPublishedSeqMap.indexOfKey(publishedSeq);
        if (index >= 0) {
            uint32_t seq = mPublishedSeqMap.valueAt(index);
            mPublishedSeqMap.removeItemsAt(index);

#if DEBUG_DISPATCH_CYCLE
            Logger::D(TAG, "channel '%s' ~ Received finished signal, seq=%u, handled=%s, "
                    "pendingEvents=%u.",
                    getInputChannelName(), seq, handled ? "true" : "false",
                    mPublishedSeqMap.size());
#endif

            if (!skipCallbacks) {
                if (senderObj == NULL) {
                    mSenderWeakGlobal->Resolve(EIID_IInputEventSender, (IInterface**)&senderObj);
                    if (senderObj == NULL) {
                        Logger::W(TAG, "channel '%s' ~ Sender object was finalized "
                                "without being disposed.", getInputChannelName());
                        return android::DEAD_OBJECT;
                    }
                }

                ECode ec = ((InputEventSender*)senderObj.Get())->DispatchInputEventFinished(
                        (Int32)seq, (Boolean)handled);
                if (FAILED(ec)) {
                    Logger::E(TAG, "Exception dispatching finished signal.");
                    skipCallbacks = true;
                }
            }
        }
    }
}


//==========================================================
//  InputEventSender
//==========================================================

CAR_INTERFACE_IMPL(InputEventSender, Object, IInputEventSender)

InputEventSender::InputEventSender()
{}

ECode InputEventSender::constructor(
    /* [in] */ IInputChannel* inputChannel,
    /* [in] */ ILooper* looper)
{
    if (inputChannel == NULL) {
        Logger::E(TAG, "inputChannel must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (looper == NULL) {
        Logger::E(TAG, "looper must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<ICloseGuardHelper> helper;
    CCloseGuardHelper::AcquireSingleton((ICloseGuardHelper**)&helper);
    helper->Get((ICloseGuard**)&mCloseGuard);
    mCloseGuard->Open(String("InputEventSender::Dispose"));

    mInputChannel = inputChannel;
    looper->GetQueue((IMessageQueue**)&mMessageQueue);

    AutoPtr<IWeakReference> weakThis;
    GetWeakReference((IWeakReference**)&weakThis);
    return NativeInit(weakThis, inputChannel, mMessageQueue, &mSenderPtr);
}

InputEventSender::~InputEventSender()
{
    Dispose(TRUE);
}

/**
 * Disposes the receiver.
 */
ECode InputEventSender::Dispose()
{
    Dispose(FALSE);
    return NOERROR;
}

void InputEventSender::Dispose(
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
        Logger::E(TAG, "event must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mSenderPtr == 0) {
        Logger::W(TAG, "Attempted to send an input event but the input event "
                "sender has already been disposed.");
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
    /* [out] */ Int64* senderPtr)
{
    InputChannel* nChannel = (InputChannel*)inputChannelObj;
    Handle64 nInputChannel = nChannel->mNative;
    NativeInputChannel* nativeInputChannel = reinterpret_cast<NativeInputChannel*>(nInputChannel);
    android::sp<android::InputChannel> inputChannel =
            nativeInputChannel != NULL ? nativeInputChannel->getInputChannel() : NULL;
    if (inputChannel == NULL) {
        Logger::W(TAG, "InputChannel is not initialized.");
        *senderPtr = 0;
        return E_RUNTIME_EXCEPTION;
    }

    Handle64 msgQueuePtr = 0;
    messageQueueObj->GetNativeMessageQueue(&msgQueuePtr);
    AutoPtr<MessageQueue> messageQueue = reinterpret_cast<NativeMessageQueue*>(msgQueuePtr);
    if (messageQueue == NULL) {
        Logger::W(TAG, "MessageQueue is not initialized.");
        *senderPtr = 0;
        return E_RUNTIME_EXCEPTION;
    }

    android::sp<NativeInputEventSender> sender = new NativeInputEventSender(
            senderWeak, inputChannel, messageQueue);
    android::status_t status = sender->initialize();
    if (status) {
        Logger::E(TAG, "Failed to initialize input event receiver.  status=%d", status);
        *senderPtr = 0;
        return E_RUNTIME_EXCEPTION;
    }

    sender->incStrong(NULL); // retain a reference for the object
    *senderPtr = reinterpret_cast<Int64>(sender.get());
    return NOERROR;
}

ECode InputEventSender::NativeDispose(
    /* [in] */ Int64 senderPtr)
{
    android::sp<NativeInputEventSender> sender =
            reinterpret_cast<NativeInputEventSender*>(senderPtr);
    sender->dispose();
    sender->decStrong(NULL);
    return NOERROR;
}

Boolean InputEventSender::NativeSendKeyEvent(
    /* [in] */ Int64 senderPtr,
    /* [in] */ Int32 seq,
    /* [in] */ IKeyEvent* eventObj)
{
    android::sp<NativeInputEventSender> sender =
            reinterpret_cast<NativeInputEventSender*>(senderPtr);
    android::KeyEvent event;
    CreateNativeKeyEvent(eventObj, &event);
    android::status_t status = sender->sendKeyEvent(seq, &event);
    return !status;
}

Boolean InputEventSender::NativeSendMotionEvent(
    /* [in] */ Int64 senderPtr,
    /* [in] */ Int32 seq,
    /* [in] */ IMotionEvent* eventObj)
{
    android::sp<NativeInputEventSender> sender =
            reinterpret_cast<NativeInputEventSender*>(senderPtr);
    Handle64 nativeEvent;
    eventObj->GetNative(&nativeEvent);
    android::MotionEvent* event =
            reinterpret_cast<android::MotionEvent*>(nativeEvent);
    android::status_t status = sender->sendMotionEvent(seq, event);
    return !status;
}

} // namespace View
} // namespace Droid
} // namespace Elastos
