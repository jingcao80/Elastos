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

#ifndef __ELASTOS_DROID_SERVER_MASTERCLEARRECEIVER_H__
#define __ELASTOS_DROID_SERVER_MASTERCLEARRECEIVER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include <elastos/core/Thread.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Core::Thread;
using Elastos::Core::IThread;

namespace Elastos {
namespace Droid {
namespace Server {

class MasterClearReceiver
    : public BroadcastReceiver
{
private:
    class RebootThread
        : public Thread
    {
        friend class FileObserver;
    public:
        RebootThread(
            /* [in] */ IIntent* intent,
            /* [in] */ Boolean shutdown,
            /* [in] */ const String& reason);

        CARAPI Run();

    private:
        AutoPtr<IIntent> mIntent;
        Boolean mShutdown;
        String mReason;
    };

public:
    TO_STRING_IMPL("MasterClearReceiver")

    CARAPI constructor();

    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

public:
    static const String EXTRA_WIPE_MEDIA;

private:
    AutoPtr<RebootThread> mRebootThread;
};

} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_MASTERCLEARRECEIVER_H__
