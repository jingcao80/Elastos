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

#ifndef __ELASTOS_DROID_SERVER_IOTHREAD_H__
#define __ELASTOS_DROID_SERVER_IOTHREAD_H__

#include "elastos/droid/server/ServiceThread.h"
#include <Elastos.Droid.Os.h>

using Elastos::Droid::Os::IHandler;

namespace Elastos {
namespace Droid {
namespace Server {

/**
 * Shared singleton I/O thread for the system.  This is a thread for non-background
 * service operations that can potential block briefly on network IO operations
 * (not waiting for data itself, but communicating with network daemons).
 */
class IoThread
    : public ServiceThread
{
public:
    static AutoPtr<IoThread> Get();

    static AutoPtr<IHandler> GetHandler();

private:
    static void EnsureThreadLocked();

    IoThread();

public:
    static Object sLock;

private:
    static AutoPtr<IoThread> sInstance;
    static AutoPtr<IHandler> sHandler;

};

} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_IOTHREAD_H__