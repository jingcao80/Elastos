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

#ifndef __LIBCORE_IO_ASYNCHRONOUS_CLOSE_MONITOR_H_INCLUDED__
#define __LIBCORE_IO_ASYNCHRONOUS_CLOSE_MONITOR_H_INCLUDED__

#include <elastos/core/Object.h>

/**
 * AsynchronousCloseMonitor helps implement Java's asynchronous close semantics.
 *
 * AsynchronousCloseMonitor::init must be called before anything else.
 *
 * Every blocking I/O operation must be surrounded by an AsynchronousCloseMonitor
 * instance. For example:
 *
 *   {
 *     AsynchronousCloseMonitor monitor(fd);
 *     byteCount = ::read(fd, buf, sizeof(buf));
 *   }
 *
 * To interrupt all threads currently blocked on file descriptor 'fd', call signalBlockedThreads:
 *
 *   AsynchronousCloseMonitor::signalBlockedThreads(fd);
 *
 * To test to see if the interruption was due to the signalBlockedThreads call:
 *
 *   monitor.wasSignaled();
 */

class ECO_PUBLIC AsynchronousCloseMonitor
{
public:
    AsynchronousCloseMonitor(
        /* [in] */ Int32 fd);

    ~AsynchronousCloseMonitor();

    CARAPI_(Boolean) WasSignaled() const;

    static CARAPI_(void) Init();

    static CARAPI_(void) SignalBlockedThreads(
        /* [in] */ Int32 fd);

private:
    AsynchronousCloseMonitor* mPrev;
    AsynchronousCloseMonitor* mNext;
    pthread_t mThread;
    Int32 mFd;
    Boolean mSignaled;

    // Disallow copy and assignment.
    ECO_LOCAL AsynchronousCloseMonitor(const AsynchronousCloseMonitor&);
    ECO_LOCAL void operator=(const AsynchronousCloseMonitor&);
};

#endif  // __LIBCORE_IO_ASYNCHRONOUS_CLOSE_MONITOR_H_INCLUDED__
