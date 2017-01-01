//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __LOOPER_H__
#define __LOOPER_H__

#include <eltypes.h>
#include <utils/KeyedVector.h>
#include <utils/Mutex.h>
#include <sys/epoll.h>

namespace Elastos {
namespace RPC {

enum {
    /**
     * Result from ALooper_pollOnce() and ALooper_pollAll():
     * The poll was awoken using wake() before the timeout expired
     * and no callbacks were executed and no other file descriptors were ready.
     */
    LOOPER_POLL_WAKE = -1,

    /**
     * Result from ALooper_pollOnce() and ALooper_pollAll():
     * One or more callbacks were executed.
     */
    LOOPER_POLL_CALLBACK = -2,

    /**
     * Result from ALooper_pollOnce() and ALooper_pollAll():
     * The timeout expired.
     */
    LOOPER_POLL_TIMEOUT = -3,

    /**
     * Result from ALooper_pollOnce() and ALooper_pollAll():
     * An error occurred.
     */
    LOOPER_POLL_ERROR = -4,
};

/**
 * Flags for file descriptor events that a looper can monitor.
 *
 * These flag bits can be combined to monitor multiple events at once.
 */
enum {
    /**
     * The file descriptor is available for read operations.
     */
    LOOPER_EVENT_INPUT = 1 << 0,

    /**
     * The file descriptor is available for write operations.
     */
    LOOPER_EVENT_OUTPUT = 1 << 1,

    /**
     * The file descriptor has encountered an error condition.
     *
     * The looper always sends notifications about errors; it is not necessary
     * to specify this event flag in the requested event set.
     */
    LOOPER_EVENT_ERROR = 1 << 2,

    /**
     * The file descriptor was hung up.
     * For example, indicates that the remote end of a pipe or socket was closed.
     *
     * The looper always sends notifications about hangups; it is not necessary
     * to specify this event flag in the requested event set.
     */
    LOOPER_EVENT_HANGUP = 1 << 3,

    /**
     * The file descriptor is invalid.
     * For example, the file descriptor was closed prematurely.
     *
     * The looper always sends notifications about invalid file descriptors; it is not necessary
     * to specify this event flag in the requested event set.
     */
    LOOPER_EVENT_INVALID = 1 << 4,
};

typedef Int32 (*Looper_callbackFunc)(Int32 fd, Int32 events, void* data);

class LooperCallback : public ElLightRefBase
{
protected:
    virtual ~LooperCallback() {}

public:
    virtual CARAPI_(Int32) HandleEvent(
        /* [in] */ Int32 fd,
        /* [in] */ Int32 events,
        /* [in] */ void* data) = 0;
};

class SimpleLooperCallback : public LooperCallback
{
protected:
    virtual ~SimpleLooperCallback();

public:
    SimpleLooperCallback(
        /* [in] */ Looper_callbackFunc callback);

    virtual CARAPI_(Int32) HandleEvent(
        /* [in] */ Int32 fd,
        /* [in] */ Int32 events,
        /* [in] */ void* data);

private:
    Looper_callbackFunc mCallback;
};

class Looper : public ElLightRefBase
{
private:
    struct Request
    {
        Int32 mFd;
        Int32 mIdent;
        AutoPtr<LooperCallback> mCallback;
        void* mData;
    };

    struct Response
    {
        Int32 mEvents;
        Request mRequest;
    };

public:
    static CARAPI_(AutoPtr<Looper>) GetLooper();

    static CARAPI_(AutoPtr<Looper>) Prepare();

    static CARAPI_(void) Loop();

    static CARAPI StartLooper();

    /**
     * Waits for events to be available, with optional timeout in milliseconds.
     * Invokes callbacks for all file descriptors on which an event occurred.
     *
     * If the timeout is zero, returns immediately without blocking.
     * If the timeout is negative, waits indefinitely until an event appears.
     *
     * Returns ALOOPER_POLL_WAKE if the poll was awoken using wake() before
     * the timeout expired and no callbacks were invoked and no other file
     * descriptors were ready.
     *
     * Returns ALOOPER_POLL_CALLBACK if one or more callbacks were invoked.
     *
     * Returns ALOOPER_POLL_TIMEOUT if there was no data before the given
     * timeout expired.
     *
     * Returns ALOOPER_POLL_ERROR if an error occurred.
     *
     * Returns a value >= 0 containing an identifier if its file descriptor has data
     * and it has no callback function (requiring the caller here to handle it).
     * In this (and only this) case outFd, outEvents and outData will contain the poll
     * events and data associated with the fd, otherwise they will be set to NULL.
     *
     * This method does not return until it has finished invoking the appropriate callbacks
     * for all file descriptors that were signalled.
     */
    CARAPI_(Int32) PollOnce(
        /* [in] */ Int32 timeoutMillis,
        /* [in] */ Int32* outFd,
        /* [in] */ Int32* outEvents,
        /* [in] */ void** outData);

    inline CARAPI_(Int32) PollOnce(
        /* [in] */ Int32 timeoutMillis)
    {
        return PollOnce(timeoutMillis, NULL, NULL, NULL);
    }

    /**
     * Like pollOnce(), but performs all pending callbacks until all
     * data has been consumed or a file descriptor is available with no callback.
     * This function will never return ALOOPER_POLL_CALLBACK.
     */
    CARAPI_(Int32) PollAll(
        /* [in] */ Int32 timeoutMillis,
        /* [in] */ Int32* outFd,
        /* [in] */ Int32* outEvents,
        /* [in] */ void** outData);

    inline CARAPI_(Int32) PollAll(
        /* [in] */ Int32 timeoutMillis)
    {
        return PollAll(timeoutMillis, NULL, NULL, NULL);
    }


    /**
     * Adds a new file descriptor to be polled by the looper.
     * If the same file descriptor was previously added, it is replaced.
     *
     * "fd" is the file descriptor to be added.
     * "ident" is an identifier for this event, which is returned from pollOnce().
     * The identifier must be >= 0, or ALOOPER_POLL_CALLBACK if providing a non-NULL callback.
     * "events" are the poll events to wake up on.  Typically this is ALOOPER_EVENT_INPUT.
     * "callback" is the function to call when there is an event on the file descriptor.
     * "data" is a private data pointer to supply to the callback.
     *
     * There are two main uses of this function:
     *
     * (1) If "callback" is non-NULL, then this function will be called when there is
     * data on the file descriptor.  It should execute any events it has pending,
     * appropriately reading from the file descriptor.  The 'ident' is ignored in this case.
     *
     * (2) If "callback" is NULL, the 'ident' will be returned by ALooper_pollOnce
     * when its file descriptor has data available, requiring the caller to take
     * care of processing it.
     *
     * Returns 1 if the file descriptor was added, 0 if the arguments were invalid.
     *
     * This method can be called on any thread.
     * This method may block briefly if it needs to wake the poll.
     *
     * The callback may either be specified as a bare function pointer or as a smart
     * pointer callback object.  The smart pointer should be preferred because it is
     * easier to avoid races when the callback is removed from a different thread.
     * See removeFd() for details.
     */
    CARAPI_(Int32) AddFd(
        /* [in] */ Int32 fd,
        /* [in] */ Int32 ident,
        /* [in] */ Int32 events,
        /* [in] */ Looper_callbackFunc callback,
        /* [in] */ void* data);

    CARAPI_(Int32) AddFd(
        /* [in] */ Int32 fd,
        /* [in] */ Int32 ident,
        /* [in] */ Int32 events,
        /* [in] */ LooperCallback* callback,
        /* [in] */ void* data);

    /**
     * Removes a previously added file descriptor from the looper.
     *
     * When this method returns, it is safe to close the file descriptor since the looper
     * will no longer have a reference to it.  However, it is possible for the callback to
     * already be running or for it to run one last time if the file descriptor was already
     * signalled.  Calling code is responsible for ensuring that this case is safely handled.
     * For example, if the callback takes care of removing itself during its own execution either
     * by returning 0 or by calling this method, then it can be guaranteed to not be invoked
     * again at any later time unless registered anew.
     *
     * A simple way to avoid this problem is to use the version of addFd() that takes
     * a sp<LooperCallback> instead of a bare function pointer.  The LooperCallback will
     * be released at the appropriate time by the Looper.
     *
     * Returns 1 if the file descriptor was removed, 0 if none was previously registered.
     *
     * This method can be called on any thread.
     * This method may block briefly if it needs to wake the poll.
     */
    CARAPI_(Int32) RemoveFd(
        /* [in] */ Int32 fd);

private:
    /**
     * Creates a looper.
     *
     * If allowNonCallbaks is true, the looper will allow file descriptors to be
     * registered without associated callbacks.  This assumes that the caller of
     * pollOnce() is prepared to handle callback-less events itself.
     */
    Looper(
        /* [in] */ Boolean allowNonCallbacks);

    ~Looper();

    CARAPI_(Int32) PollInner(
        /* [in] */ Int32 timeoutMillis);

    CARAPI_(void) PushResponse(
        /* [in] */ Int32 events,
        /* [in] */ const Request& request);

    static void* ThreadEntry(void* arg);

private:
    const Boolean mAllowNonCallbacks; // immutable

    android::Mutex mLock;

    Int32 mEpollFd;

    // Locked list of file descriptor monitoring requests.
    android::KeyedVector<Int32, Request> mRequests;  // guarded by mLock

    // This state is only used privately by pollOnce and does not require a lock since
    // it runs on a single thread.
    android::Vector<Response> mResponses;
    size_t mResponseIndex;

    Boolean mEnterLoop;
};

} // namespace RPC
} // namespace Elastos

#endif //__LOOPER_H__
