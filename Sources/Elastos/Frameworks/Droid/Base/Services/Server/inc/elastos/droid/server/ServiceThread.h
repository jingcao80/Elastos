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

#ifndef __ELASTOS_DROID_SERVER_SERVICE_THREAD_H__
#define __ELASTOS_DROID_SERVER_SERVICE_THREAD_H__

#include <elastos/droid/os/HandlerThread.h>

using Elastos::Droid::Os::HandlerThread;

namespace Elastos {
namespace Droid {
namespace Server {

/**
 * Special handler thread that we create for system services that require their own loopers.
 */
class ServiceThread
    : public HandlerThread
{
public:
    ServiceThread(
        /* [in] */ const String& name,
        /* [in] */ Int32 priority,
        /* [in] */ Boolean allowIo);

    // @Override
    CARAPI Run();

private:
    static const String TAG;

    Boolean mAllowIo;
};

} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_SERVICE_THREAD_H__