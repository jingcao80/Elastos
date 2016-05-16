
#include "looper.h"
#include <utils/Timers.h>
#include <unistd.h>
#include <cutils/log.h>

namespace Elastos {
namespace RPC {

// Hint for number of file descriptors to be associated with the epoll instance.
static const Int32 EPOLL_SIZE_HINT = 256;

// Maximum number of file descriptors for which to retrieve poll events each iteration.
static const Int32 EPOLL_MAX_EVENTS = 2048;

// --- SimpleLooperCallback ---

SimpleLooperCallback::SimpleLooperCallback(
    /* [in] */ Looper_callbackFunc callback)
    : mCallback(callback)
{}

SimpleLooperCallback::~SimpleLooperCallback()
{}

Int32 SimpleLooperCallback::HandleEvent(
    /* [in] */ Int32 fd,
    /* [in] */ Int32 events,
    /* [in] */ void* data)
{
    return mCallback(fd, events, data);
}


// --- DLooper ---

static AutoPtr<Looper> sInstance;

Looper::Looper(
    /* [in] */ Boolean allowNonCallbacks)
    : mAllowNonCallbacks(allowNonCallbacks)
    , mEnterLoop(FALSE)
{
    // Allocate the epoll instance and register the wake pipe.
    mEpollFd = epoll_create(EPOLL_SIZE_HINT);
    if (mEpollFd < 0) {
        ALOGD("Could not create epoll instance.  errno=%d", errno);
    }
}

Looper::~Looper()
{
    close(mEpollFd);
}

AutoPtr<Looper> Looper::GetLooper()
{
    assert(sInstance != NULL);
    return sInstance;
}

AutoPtr<Looper> Looper::Prepare()
{
    if (sInstance == NULL)  sInstance = new Looper(TRUE);
    return sInstance;
}

void Looper::Loop()
{
    if (sInstance == NULL) return;

    for (;;) {
        sInstance->PollOnce(-1);
    }
}

void* Looper::ThreadEntry(void* arg)
{
    Looper::Loop();
    return (void*)NOERROR;
}

CARAPI Looper::StartLooper()
{
    if (sInstance == NULL) Looper::Prepare();

    android::AutoMutex _l(sInstance->mLock);
    if (!sInstance->mEnterLoop) {
        pthread_attr_t threadAddr;
        pthread_attr_init(&threadAddr);
        pthread_attr_setdetachstate(&threadAddr, PTHREAD_CREATE_DETACHED);

        pthread_t thread;
        int ret = pthread_create(&thread, &threadAddr, Looper::ThreadEntry, NULL);
        if (ret != 0) {
            return E_FAIL;
        }
        sInstance->mEnterLoop = TRUE;
    }

    return NOERROR;
}

Int32 Looper::AddFd(
    /* [in] */ Int32 fd,
    /* [in] */ Int32 ident,
    /* [in] */ Int32 events,
    /* [in] */ Looper_callbackFunc callback,
    /* [in] */ void* data)
{
    AutoPtr<SimpleLooperCallback> cb;
    if (callback != NULL) {
        cb = new SimpleLooperCallback(callback);
    }
    return AddFd(fd, ident, events, cb, data);
}

Int32 Looper::AddFd(
    /* [in] */ Int32 fd,
    /* [in] */ Int32 ident,
    /* [in] */ Int32 events,
    /* [in] */ LooperCallback* callback,
    /* [in] */ void* data)
{
#if DEBUG_CALLBACKS
    ALOGD("%p ~ addFd - fd=%d, ident=%d, events=0x%x, callback=%p, data=%p", this, fd, ident,
            events, callback, data);
#endif

    if (callback == NULL) {
        if (!mAllowNonCallbacks) {
            ALOGE("Invalid attempt to set NULL callback but not allowed for this looper.");
            return -1;
        }

        if (ident < 0) {
            ALOGE("Invalid attempt to set NULL callback with ident < 0.");
            return -1;
        }
    }
    else {
        ident = LOOPER_POLL_CALLBACK;
    }

    Int32 epollEvents = 0;
    if (events & LOOPER_EVENT_INPUT) epollEvents |= EPOLLIN;
    if (events & LOOPER_EVENT_OUTPUT) epollEvents |= EPOLLOUT;

    { // acquire lock
        android::AutoMutex _l(mLock);

        if (mRequests.size() >= EPOLL_MAX_EVENTS) {
            ALOGE("Error adding epoll events for fd %d, reach EPOLL_MAX_EVENTS");
            return -1;
        }

        Request request;
        request.mFd = fd;
        request.mIdent = ident;
        request.mCallback = callback;
        request.mData = data;

        struct epoll_event eventItem;
        memset(&eventItem, 0, sizeof(epoll_event)); // zero out unused members of data field union
        eventItem.events = epollEvents | EPOLLET;
        eventItem.data.fd = fd;

        ssize_t requestIndex = mRequests.indexOfKey(fd);
        if (requestIndex < 0) {
            int epollResult = epoll_ctl(mEpollFd, EPOLL_CTL_ADD, fd, &eventItem);
            if (epollResult < 0) {
                ALOGE("Error adding epoll events for fd %d, errno=%d", fd, errno);
                return -1;
            }
            mRequests.add(fd, request);
        } else {
            int epollResult = epoll_ctl(mEpollFd, EPOLL_CTL_MOD, fd, &eventItem);
            if (epollResult < 0) {
                ALOGE("Error modifying epoll events for fd %d, errno=%d", fd, errno);
                return -1;
            }
            mRequests.replaceValueAt(requestIndex, request);
        }
    } // release lock
    return 1;
}

Int32 Looper::RemoveFd(
    /* [in] */ Int32 fd)
{
#if DEBUG_CALLBACKS
    ALOGD("%p ~ removeFd - fd=%d", this, fd);
#endif

    { // acquire lock
        android::AutoMutex _l(mLock);
        ssize_t requestIndex = mRequests.indexOfKey(fd);
        if (requestIndex < 0) {
            return 0;
        }

        int epollResult = epoll_ctl(mEpollFd, EPOLL_CTL_DEL, fd, NULL);
        if (epollResult < 0) {
            ALOGE("Error removing epoll events for fd %d, errno=%d", fd, errno);
            return -1;
        }

        mRequests.removeItemsAt(requestIndex);
    } // release lock
    return 1;
}

Int32 Looper::PollOnce(
    /* [in] */ Int32 timeoutMillis,
    /* [in] */ Int32* outFd,
    /* [in] */ Int32* outEvents,
    /* [in] */ void** outData)
{
    Int32 result = 0;
    for (;;) {
        while (mResponseIndex < mResponses.size()) {
            const Response& response = mResponses.itemAt(mResponseIndex++);
            Int32 ident = response.mRequest.mIdent;
            if (ident >= 0) {
                Int32 fd = response.mRequest.mFd;
                Int32 events = response.mEvents;
                void* data = response.mRequest.mData;
#if DEBUG_POLL_AND_WAKE
                ALOGD("%p ~ pollOnce - returning signalled identifier %d: "
                        "fd=%d, events=0x%x, data=%p",
                        this, ident, fd, events, data);
#endif
                if (outFd != NULL) *outFd = fd;
                if (outEvents != NULL) *outEvents = events;
                if (outData != NULL) *outData = data;
                return ident;
            }
        }

        if (result != 0) {
#if DEBUG_POLL_AND_WAKE
            ALOGD("%p ~ pollOnce - returning result %d", this, result);
#endif
            if (outFd != NULL) *outFd = 0;
            if (outEvents != NULL) *outEvents = 0;
            if (outData != NULL) *outData = NULL;
            return result;
        }

        result = PollInner(timeoutMillis);
    }
}

Int32 Looper::PollInner(
    /* [in] */ Int32 timeoutMillis)
{
#if DEBUG_POLL_AND_WAKE
    ALOGD("%p ~ pollOnce - waiting: timeoutMillis=%d", this, timeoutMillis);
#endif

//    // Adjust the timeout based on when the next message is due.
//    if (timeoutMillis != 0 && mNextMessageUptime != LLONG_MAX) {
//        nsecs_t now = systemTime(SYSTEM_TIME_MONOTONIC);
//        int messageTimeoutMillis = toMillisecondTimeoutDelay(now, mNextMessageUptime);
//        if (messageTimeoutMillis >= 0
//                && (timeoutMillis < 0 || messageTimeoutMillis < timeoutMillis)) {
//            timeoutMillis = messageTimeoutMillis;
//        }
//#if DEBUG_POLL_AND_WAKE
//        ALOGD("%p ~ pollOnce - next message in %lldns, adjusted timeout: timeoutMillis=%d",
//                this, mNextMessageUptime - now, timeoutMillis);
//#endif
//    }

    // Poll.
    Int32 result = LOOPER_POLL_WAKE;
    mResponses.clear();
    mResponseIndex = 0;

    struct epoll_event eventItems[EPOLL_MAX_EVENTS];
    int eventCount = epoll_wait(mEpollFd, eventItems, EPOLL_MAX_EVENTS, timeoutMillis);

    // Acquire lock.
    mLock.lock();

    // Check for poll error.
    if (eventCount < 0) {
        if (errno == EINTR) {
            goto Done;
        }
        ALOGW("Poll failed with an unexpected error, errno=%d", errno);
        result = LOOPER_POLL_ERROR;
        goto Done;
    }

    // Check for poll timeout.
    if (eventCount == 0) {
#if DEBUG_POLL_AND_WAKE
        ALOGD("%p ~ pollOnce - timeout", this);
#endif
        result = LOOPER_POLL_TIMEOUT;
        goto Done;
    }

    // Handle all events.
#if DEBUG_POLL_AND_WAKE
    ALOGD("%p ~ pollOnce - handling events from %d fds", this, eventCount);
#endif

    for (Int32 i = 0; i < eventCount; i++) {
        Int32 fd = eventItems[i].data.fd;
        uint32_t epollEvents = eventItems[i].events;
//        if (fd == mWakeReadPipeFd) {
//            if (epollEvents & EPOLLIN) {
//                awoken();
//            } else {
//                ALOGW("Ignoring unexpected epoll events 0x%x on wake read pipe.", epollEvents);
//            }
//        } else {
            ssize_t requestIndex = mRequests.indexOfKey(fd);
            if (requestIndex >= 0) {
                Int32 events = 0;
                if (epollEvents & EPOLLIN) events |= LOOPER_EVENT_INPUT;
                if (epollEvents & EPOLLOUT) events |= LOOPER_EVENT_OUTPUT;
                if (epollEvents & EPOLLERR) events |= LOOPER_EVENT_ERROR;
                if (epollEvents & EPOLLHUP) events |= LOOPER_EVENT_HANGUP;
                PushResponse(events, mRequests.valueAt(requestIndex));
            }
            else {
                ALOGW("Ignoring unexpected epoll events 0x%x on fd %d that is "
                        "no longer registered.", epollEvents, fd);
            }
//        }
    }
Done: ;

//    // Invoke pending message callbacks.
//    mNextMessageUptime = LLONG_MAX;
//    while (mMessageEnvelopes.size() != 0) {
//        nsecs_t now = systemTime(SYSTEM_TIME_MONOTONIC);
//        const MessageEnvelope& messageEnvelope = mMessageEnvelopes.itemAt(0);
//        if (messageEnvelope.uptime <= now) {
//            // Remove the envelope from the list.
//            // We keep a strong reference to the handler until the call to handleMessage
//            // finishes.  Then we drop it so that the handler can be deleted *before*
//            // we reacquire our lock.
//            { // obtain handler
//                sp<MessageHandler> handler = messageEnvelope.handler;
//                Message message = messageEnvelope.message;
//                mMessageEnvelopes.removeAt(0);
//                mSendingMessage = true;
//                mLock.unlock();
//
//#if DEBUG_POLL_AND_WAKE || DEBUG_CALLBACKS
//                ALOGD("%p ~ pollOnce - sending message: handler=%p, what=%d",
//                        this, handler.get(), message.what);
//#endif
//                handler->handleMessage(message);
//            } // release handler
//
//            mLock.lock();
//            mSendingMessage = false;
//            result = ALOOPER_POLL_CALLBACK;
//        } else {
//            // The last message left at the head of the queue determines the next wakeup time.
//            mNextMessageUptime = messageEnvelope.uptime;
//            break;
//        }
//    }

    // Release lock.
    mLock.unlock();

    // Invoke all response callbacks.
    for (size_t i = 0; i < mResponses.size(); i++) {
        Response& response = mResponses.editItemAt(i);
        if (response.mRequest.mIdent == LOOPER_POLL_CALLBACK) {
            Int32 fd = response.mRequest.mFd;
            Int32 events = response.mEvents;
            void* data = response.mRequest.mData;
#if DEBUG_POLL_AND_WAKE || DEBUG_CALLBACKS
            ALOGD("%p ~ pollOnce - invoking fd event callback %p: fd=%d, events=0x%x, data=%p",
                    this, response.mRequest.mCallback.Get(), fd, events, data);
#endif
            Int32 callbackResult = response.mRequest.mCallback->HandleEvent(fd, events, data);
            if (callbackResult == 0) {
                RemoveFd(fd);
            }
            // Clear the callback reference in the response structure promptly because we
            // will not clear the response vector itself until the next poll.
            response.mRequest.mCallback.Clear();
            result = LOOPER_POLL_CALLBACK;
        }
    }
    return result;
}

Int32 Looper::PollAll(
    /* [in] */ Int32 timeoutMillis,
    /* [in] */ Int32* outFd,
    /* [in] */ Int32* outEvents,
    /* [in] */ void** outData)
{
    if (timeoutMillis <= 0) {
        Int32 result;
        do {
            result = PollOnce(timeoutMillis, outFd, outEvents, outData);
        } while (result == LOOPER_POLL_CALLBACK);
        return result;
    }
    else {
        nsecs_t endTime = systemTime(SYSTEM_TIME_MONOTONIC)
                + milliseconds_to_nanoseconds(timeoutMillis);

        for (;;) {
            Int32 result = PollOnce(timeoutMillis, outFd, outEvents, outData);
            if (result != LOOPER_POLL_CALLBACK) {
                return result;
            }

            nsecs_t now = systemTime(SYSTEM_TIME_MONOTONIC);
            timeoutMillis = toMillisecondTimeoutDelay(now, endTime);
            if (timeoutMillis == 0) {
                return LOOPER_POLL_TIMEOUT;
            }
        }
    }
}

void Looper::PushResponse(
    /* [in] */ Int32 events,
    /* [in] */ const Request& request)
{
    Response response;
    response.mEvents = events;
    response.mRequest = request;
    mResponses.push(response);
}

} // namespace RPC
} // namespace Elastos
