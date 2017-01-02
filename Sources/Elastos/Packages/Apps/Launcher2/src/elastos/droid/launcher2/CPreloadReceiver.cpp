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

#include "elastos/droid/launcher2/CPreloadReceiver.h"
#include "elastos/droid/text/TextUtils.h"
#include "Elastos.Droid.Service.h"
#include "Elastos.CoreLibrary.Core.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::CThread;
using Elastos::Core::IThread;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CPreloadReceiver::MyRunnable::MyRunnable(
    /* [in] */ ILauncherProvider* provider,
    /* [in] */ Int32 workspaceResId,
    /* [in] */ Boolean overridePrevious)
    : mProvider(provider)
    , mWorkspaceResId(workspaceResId)
    , mOverridePrevious(overridePrevious)
{
}

ECode CPreloadReceiver::MyRunnable::Run()
{
    return mProvider->LoadDefaultFavoritesIfNecessary(mWorkspaceResId, mOverridePrevious);
}

const String CPreloadReceiver::TAG("Launcher.PreloadReceiver");
const Boolean CPreloadReceiver::LOGD = FALSE;

CAR_INTERFACE_IMPL(CPreloadReceiver, BroadcastReceiver, IPreloadReceiver);

CAR_OBJECT_IMPL(CPreloadReceiver);

CPreloadReceiver::CPreloadReceiver()
{
    Slogger::E("CPreloadReceiver", "============================CPreloadReceiver::CPreloadReceiver()");
}

ECode CPreloadReceiver::constructor()
{
    return BroadcastReceiver::constructor();
}

ECode CPreloadReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    AutoPtr<IContext> ctx;
    context->GetApplicationContext((IContext**)&ctx);
    AutoPtr<ILauncherApplication> app = ILauncherApplication::Probe(ctx);
    AutoPtr<ILauncherProvider> provider;
    app->GetLauncherProvider((ILauncherProvider**)&provider);
    if (provider != NULL) {
        String name;
        intent->GetStringExtra(IPreloadReceiver::EXTRA_WORKSPACE_NAME, &name);
        Int32 workspaceResId;
        if (!TextUtils::IsEmpty(name)) {
            AutoPtr<IResources> res;
            context->GetResources((IResources**)&res);
            res->GetIdentifier(name, String("xml"), String("com.android.launcher"), &workspaceResId);
        }
        else {
            workspaceResId = 0;
        }

        Boolean overridePrevious;
        intent->GetBooleanExtra(IPreloadReceiver::EXTRA_OVERRIDE_PREVIOUS, FALSE, &overridePrevious);
        if (LOGD) {
            StringBuilder sb;
            sb += "workspace name: ";
            sb += name;
            sb += " id: ";
            sb += workspaceResId;
            Slogger::D(TAG, sb.ToString());
        }
        AutoPtr<MyRunnable> run = new MyRunnable(provider, workspaceResId, overridePrevious);
        AutoPtr<IThread> t;
        CThread::New(IRunnable::Probe(run), (IThread**)&t);
        t->Start();
    }
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos