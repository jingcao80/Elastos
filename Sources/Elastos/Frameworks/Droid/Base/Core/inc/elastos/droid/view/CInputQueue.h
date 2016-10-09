
#ifndef __ELASTOS_DROID_VIEW_CINPUTQUEUE_H__
#define __ELASTOS_DROID_VIEW_CINPUTQUEUE_H__

#include "_Elastos_Droid_View_CInputQueue.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/utility/Pools.h"
#include <elastos/core/Mutex.h>
#include <input/Input.h>
#include <utils/Errors.h>
#include <utils/Looper.h>
#include <utils/TypeHelpers.h>
#include <utils/Vector.h>

using Elastos::Droid::Os::IMessageQueue;
using Elastos::Droid::Utility::IInt64SparseArray;
using Elastos::Droid::Utility::Pools;
using Elastos::Core::ICloseGuard;
using Elastos::Core::Mutex;

namespace Elastos {
namespace Droid {
namespace View {

class InputQueue;

/**
 * An input queue provides a mechanism for an application to receive incoming
 * input events.  Currently only usable from native code.
 */
CarClass(CInputQueue)
    , public Object
    , public IInputQueue
{
private:
    class ActiveInputEvent
        : public Object
    {
    public:
        CARAPI_(void) Recycle();

    public:
        AutoPtr<IInterface> mToken;
        AutoPtr<IInputQueueFinishedInputEventCallback> mCallback;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /** @hide */
    CInputQueue();

    ~CInputQueue();

    CARAPI constructor();

    /** @hide */
    CARAPI Dispose();

    /** @hide */
    CARAPI Dispose(
        /* [in] */ Boolean finalized);

    /** @hide */
    CARAPI GetNativePtr(
        /* [out] */ Int64* native);

    /** @hide */
    CARAPI SendInputEvent(
        /* [in] */ IInputEvent* e,
        /* [in] */ IInterface* token,
        /* [in] */ Boolean predispatch,
        /* [in] */ IInputQueueFinishedInputEventCallback* callback);

private:
    CARAPI_(void) FinishInputEvent(
        /* [in] */ Int64 id,
        /* [in] */ Boolean handled);

    CARAPI_(AutoPtr<ActiveInputEvent>) ObtainActiveInputEvent(
        /* [in] */ IInterface* token,
        /* [in] */ IInputQueueFinishedInputEventCallback* callback);

    CARAPI_(void) RecycleActiveInputEvent(
        /* [in] */ ActiveInputEvent* e);

private:
    // Native method.
    static CARAPI NativeInit(
        /* [in] */ IWeakReference/*<InputQueue>*/* weakQueue,
        /* [in] */ IMessageQueue* messageQueue,
        /* [out] */ Int64* ptr);

    static CARAPI_(Int64) NativeSendKeyEvent(
        /* [in] */ Int64 ptr,
        /* [in] */ IKeyEvent* e,
        /* [in] */ Boolean preDispatch);

    static CARAPI NativeSendMotionEvent(
        /* [in] */ Int64 ptr,
        /* [in] */ IMotionEvent* e,
        /* [out] */ Int64* value);

    static CARAPI_(void) NativeDispose(
        /* [in] */ Int64 ptr);

private:
    // private final LongSparseArray<ActiveInputEvent> mActiveEventArray = new LongSparseArray<ActiveInputEvent>(20);
    AutoPtr<IInt64SparseArray> mActiveEventArray;
    AutoPtr<Pools::Pool<ActiveInputEvent> > mActiveInputEventPool;
    AutoPtr<ICloseGuard> mCloseGuard;
    Int64 mPtr;
    friend class InputQueue;
};

/*
 * Declare a concrete type for the NDK's AInputQueue forward declaration
 */
struct AInputQueue{
};

class InputQueue
    : public AInputQueue
    , public android::MessageHandler
{
public:
    virtual ~InputQueue();

    CARAPI_(void) AttachLooper(
        /* [in] */ android::Looper* looper,
        /* [in] */ int ident,
        /* [in] */ ALooper_callbackFunc callback,
        /* [in] */ void* data);

    CARAPI_(void) DetachLooper();

    CARAPI_(Boolean) HasEvents();

    CARAPI_(android::status_t) GetEvent(
        /* [in] */ android::InputEvent** outEvent);

    CARAPI_(Boolean) PreDispatchEvent(
        /* [in] */ android::InputEvent* event);

    CARAPI_(void) FinishEvent(
        /* [in] */ android::InputEvent* event,
        /* [in] */ Boolean handled);

    CARAPI_(android::KeyEvent*) CreateKeyEvent();

    CARAPI_(android::MotionEvent*) CreateMotionEvent();

    CARAPI_(void) RecycleInputEvent(
        /* [in] */ android::InputEvent* event);

    CARAPI_(void) EnqueueEvent(
        /* [in] */ android::InputEvent* event);

    static CARAPI_(InputQueue*) CreateQueue(
        /* [in] */ IWeakReference* inputQueueObj,
        /* [in] */ const android::sp<android::Looper>& looper);

protected:
    virtual void handleMessage(
        /* [in] */ const android::Message& message);

private:
    InputQueue(
        /* [in] */ IWeakReference* inputQueueObj,
        /* [in] */ const android::sp<android::Looper>& looper,
        /* [in] */ Int32 readDispatchFd,
        /* [in] */ Int32 writeDispatchFd);

    void DetachLooperLocked();

    AutoPtr<IWeakReference> mInputQueueWeakGlobal;
    Int32 mDispatchReadFd;
    Int32 mDispatchWriteFd;
    android::Vector<android::Looper*> mAppLoopers;
    android::sp<android::Looper> mDispatchLooper;
    android::sp<android::WeakMessageHandler> mHandler;
    android::PooledInputEventFactory mPooledInputEventFactory;
    // Guards the pending and finished event vectors
    mutable Mutex mLock;
    android::Vector<android::InputEvent*> mPendingEvents;
    android::Vector<android::key_value_pair_t<android::InputEvent*, Boolean> > mFinishedEvents;
};

} // namespace View
} // namepsace Droid
} // namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::View::CInputQueue::ActiveInputEvent, IInterface);

#endif //__ELASTOS_DROID_VIEW_CINPUTQUEUE_H__
