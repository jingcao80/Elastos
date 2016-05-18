
#ifndef __ELASTOS_DROID_VIEW_INPUTEVENTRECEIVER_H__
#define __ELASTOS_DROID_VIEW_INPUTEVENTRECEIVER_H__

#include "Elastos.Droid.View.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Os::IMessageQueue;
using Elastos::Droid::Os::ILooper;
using Elastos::Core::ICloseGuard;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace View {

/**
 * Provides a low-level mechanism for an application to receive input events.
 * @hide
 */
class ECO_PUBLIC InputEventReceiver
    : public Object
    , public IInputEventReceiver
{
public:
    CAR_INTERFACE_DECL()

    InputEventReceiver();

    virtual ~InputEventReceiver();

    CARAPI constructor(
        /* [in] */ IInputChannel* inputChannel,
        /* [in] */ ILooper* looper);

    virtual CARAPI Dispose();

    virtual CARAPI OnInputEvent(
        /* [in] */ IInputEvent* event);

    virtual CARAPI OnBatchedInputEventPending();

    CARAPI FinishInputEvent(
        /* [in] */ IInputEvent* event,
        /* [in] */ Boolean handled);

    CARAPI ConsumeBatchedInputEvents(
        /* [in] */ Int64 frameTimeNanos,
        /* [out] */ Boolean* result);

    ECO_LOCAL CARAPI DispatchInputEvent(
        /* [in] */ Int32 seq,
        /* [in] */ IInputEvent* event);

    ECO_LOCAL CARAPI DispatchBatchedInputEventPending();

private:
    ECO_LOCAL static CARAPI NativeInit(
        /* [in] */ IWeakReference* receiver,
        /* [in] */ IInputChannel* inputChannel,
        /* [in] */ IMessageQueue* messageQueue,
        /* [out] */ Int64* receiverPtr);

    ECO_LOCAL static CARAPI NativeDispose(
        /* [in] */ Int64 receiverPtr);

    ECO_LOCAL static CARAPI NativeFinishInputEvent(
        /* [in] */ Int64 receiverPtr,
        /* [in] */ Int32 seq,
        /* [in] */ Boolean handled);

    ECO_LOCAL static CARAPI NativeConsumeBatchedInputEvents(
        /* [in] */ Int64 receiverPtr,
        /* [in] */ Int64 frameTimeNanos,
        /* [out] */ Boolean* result);

private:

    AutoPtr<ICloseGuard> mCloseGuard;// = CloseGuard.get();

    // We keep references to the input channel and message queue objects here so that
    // they are not GC'd while the native peer of the receiver is using them.
    AutoPtr<IInputChannel> mInputChannel;
    AutoPtr<IMessageQueue> mMessageQueue;

    // Map from InputEvent sequence numbers to dispatcher sequence numbers.
    HashMap<Int32, Int32> mSeqMap;
    Int64 mReceiverPtr;
};

} // namespace View
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_INPUTEVENTRECEIVER_H__
