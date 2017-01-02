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

#ifndef __ELASTOS_DROID_INTERNAL_OS_BACKGROUNDTHREAD_H__
#define __ELASTOS_DROID_INTERNAL_OS_BACKGROUNDTHREAD_H__

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/os/HandlerThread.h"

using Elastos::Droid::Os::HandlerThread;
using Elastos::Droid::Os::IHandler;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

/**
 * Shared singleton background thread for each process.
 */
class BackgroundThread
    : public HandlerThread
    , public IBackgroundThread
{
public:
    CAR_INTERFACE_DECL()

    static CARAPI_(AutoPtr<IBackgroundThread>) GetInstance();

    static CARAPI_(AutoPtr<IHandler>) GetHandler();

private:
    BackgroundThread();

    static CARAPI_(void) EnsureThreadLocked();

private:
    static AutoPtr<BackgroundThread> sInstance;
    static AutoPtr<IHandler> sHandler;
    static Object sLock;
};

} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_OS_BACKGROUNDTHREAD_H__
