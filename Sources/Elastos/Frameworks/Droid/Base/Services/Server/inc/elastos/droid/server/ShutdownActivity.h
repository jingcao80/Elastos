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

#ifndef __ELASTOS_DROID_SERVER_SHUTDOWNACTIVITY_H__
#define __ELASTOS_DROID_SERVER_SHUTDOWNACTIVITY_H__

#include "elastos/droid/app/Activity.h"
#include <elastos/core/Thread.h>

using Elastos::Core::Thread;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::App::Activity;

namespace Elastos {
namespace Droid {
namespace Server {

class ShutdownActivity
    : public Activity
{
private:
    class LocalThread
        : public Thread
    {
    public:
        LocalThread(
            /* [in] */ ShutdownActivity* host)
            : mHost(host)
        {
        }

        CARAPI constructor(
            /* [in] */ const String& name)
        {
            return Thread::constructor(name);
        }

        CARAPI Run();

    private:
        ShutdownActivity* mHost;
    };

public:
    ShutdownActivity();

    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

private:
    static const String TAG;
    Boolean mReboot;
    Boolean mConfirm;
};

} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_SHUTDOWNACTIVITY_H__
