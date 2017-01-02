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

#ifndef  __ELASTOS_DROID_LAUNCHER2_CPRELOADRECEIVER_H__
#define  __ELASTOS_DROID_LAUNCHER2_CPRELOADRECEIVER_H__

#include "_Elastos_Droid_Launcher2_CPreloadReceiver.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include <elastos/core/Runnable.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Core::Runnable;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CarClass(CPreloadReceiver)
    , public BroadcastReceiver
    , public IPreloadReceiver
{
private:
    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ ILauncherProvider* provider,
            /* [in] */ Int32 workspaceResId,
            /* [in] */ Boolean overridePrevious);

        CARAPI Run();

    private:
        AutoPtr<ILauncherProvider> mProvider;
        Int32 mWorkspaceResId;
        Boolean mOverridePrevious;
    };

public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CPreloadReceiver();

    CARAPI constructor();

    //@Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

private:
    static const String TAG;
    static const Boolean LOGD;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CPRELOADRECEIVER_H__