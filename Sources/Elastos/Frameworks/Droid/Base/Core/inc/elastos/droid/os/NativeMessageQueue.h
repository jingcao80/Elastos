
#ifndef __ELASTOS_DROID_OS_NATIVEMESSAGEQUEUE_H__
#define __ELASTOS_DROID_OS_NATIVEMESSAGEQUEUE_H__

#include "elastos/droid/ext/frameworkext.h"
#include <utils/Looper.h>
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Os {

class MessageQueue : public Object
{
public:
    /* Gets the message queue's looper. */
    inline CARAPI_(android::sp<android::Looper>) GetLooper() const
    {
        return mLooper;
    }

protected:
    MessageQueue() {}
    virtual ~MessageQueue() {}

public:
    android::sp<android::Looper> mLooper;
};

class NativeMessageQueue : public MessageQueue
{
public:
    NativeMessageQueue();
    virtual ~NativeMessageQueue();

    CARAPI_(void) PollOnce(
        /* [in] */ Int32 timeoutMillis);

    CARAPI_(void) Wake();
};

/* Gets the native object associated with a MessageQueue. */
extern AutoPtr<NativeMessageQueue> Elastos_Os_MessageQueue_GetMessageQueue(
    /* [in] */ IMessageQueue* messageQueueObj);

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_NATIVEMESSAGEQUEUE_H__
