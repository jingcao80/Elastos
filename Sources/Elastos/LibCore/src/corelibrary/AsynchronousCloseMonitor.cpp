/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "AsynchronousCloseMonitor"

#include "AsynchronousCloseMonitor.h"
#include <elastos/core/Mutex.h>
#include <elastos/utility/logging/Logger.h>

#include <errno.h>
#include <asm/signal.h>

using Elastos::Core::Mutex;
using Elastos::Utility::Logging::Logger;

/**
 * We use an intrusive doubly-linked list to keep track of blocked threads.
 * This gives us O(1) insertion and removal, and means we don't need to do any allocation.
 * (The objects themselves are stack-allocated.)
 * Waking potentially-blocked threads when a file descriptor is closed is O(n) in the total number
 * of blocked threads (not the number of threads actually blocked on the file descriptor in
 * question). For now at least, this seems like a good compromise for Android.
 */

static Mutex sBlockedThreadListMutex;
static AsynchronousCloseMonitor* sBlockedThreadList = NULL;

/**
 * The specific signal chosen here is arbitrary, but bionic needs to know so that SIGRTMIN
 * starts at a higher value.
 */
#if defined(__APPLE__)
static const int BLOCKED_THREAD_SIGNAL = SIGUSR2;
#else
static const int BLOCKED_THREAD_SIGNAL = __SIGRTMIN + 2;
#endif

static void BlockedThreadSignalHandler(int /*signal*/)
{
    // Do nothing. We only sent this signal for its side-effect of interrupting syscalls.
}

void AsynchronousCloseMonitor::Init()
{
    // Ensure that the signal we send interrupts system calls but doesn't kill threads.
    // Using sigaction(2) lets us ensure that the SA_RESTART flag is not set.
    // (The whole reason we're sending this signal is to unblock system calls!)
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = BlockedThreadSignalHandler;
    sa.sa_flags = 0;
    int rc = sigaction(BLOCKED_THREAD_SIGNAL, &sa, NULL);
    if (rc == -1) {
        Logger::E(LOG_TAG, "setting blocked thread signal handler failed: %s", strerror(errno));
    }
}

void AsynchronousCloseMonitor::SignalBlockedThreads(
    /* [in] */ Int32 fd)
{
    Mutex::AutoLock lock(&sBlockedThreadListMutex);
    for (AsynchronousCloseMonitor* it = sBlockedThreadList; it != NULL; it = it->mNext) {
        if (it->mFd == fd) {
            it->mSignaled = TRUE;
            pthread_kill(it->mThread, BLOCKED_THREAD_SIGNAL);
            // Keep going, because there may be more than one thread...
        }
    }
}

Boolean AsynchronousCloseMonitor::WasSignaled() const
{
    return mSignaled;
}

AsynchronousCloseMonitor::AsynchronousCloseMonitor(
        /* [in] */ Int32 fd)
{
    Mutex::AutoLock lock(&sBlockedThreadListMutex);
    // Who are we, and what are we waiting for?
    mThread = pthread_self();
    mFd = fd;
    mSignaled = FALSE;
    // Insert ourselves at the head of the intrusive doubly-linked list...
    mPrev = NULL;
    mNext = sBlockedThreadList;
    if (mNext != NULL) {
        mNext->mPrev = this;
    }
    sBlockedThreadList = this;
}

AsynchronousCloseMonitor::~AsynchronousCloseMonitor()
{
    Mutex::AutoLock lock(&sBlockedThreadListMutex);
    // Unlink ourselves from the intrusive doubly-linked list...
    if (mNext != NULL) {
        mNext->mPrev = mPrev;
    }
    if (mPrev == NULL) {
        sBlockedThreadList = mNext;
    }
    else {
        mPrev->mNext = mNext;
    }
}
