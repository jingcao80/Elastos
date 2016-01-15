
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/os/NativeMessageQueue.h"

namespace Elastos {
namespace Droid {
namespace Os {

NativeMessageQueue::NativeMessageQueue()
{
    mLooper = android::Looper::getForThread();
    if (mLooper == NULL) {
        mLooper = new android::Looper(false);
        android::Looper::setForThread(mLooper);
    }
}

NativeMessageQueue::~NativeMessageQueue()
{}

void NativeMessageQueue::PollOnce(
    /* [in] */ Int32 timeoutMillis)
{
    mLooper->pollOnce(timeoutMillis);
}

void NativeMessageQueue::Wake()
{
    mLooper->wake();
}

AutoPtr<NativeMessageQueue> Elastos_Os_MessageQueue_GetMessageQueue(
    /* [in] */ IMessageQueue* messageQueueObj)
{
    Int64 ptr = 0;
    messageQueueObj->GetMPtr(&ptr);
    return reinterpret_cast<NativeMessageQueue*>(ptr);
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
