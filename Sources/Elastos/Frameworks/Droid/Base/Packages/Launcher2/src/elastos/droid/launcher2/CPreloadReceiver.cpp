
#include "elastos/droid/launcher2/CPreloadReceiver.h"
#include "Elastos.Droid.Service.h"
#include "R.h"
#include <elastos/utility/logging/Slogger.h>
#include "elastos/droid/text/TextUtils.h"
#include "Elastos.CoreLibrary.Core.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::CThread;
using Elastos::Core::IThread;
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
    assert(0 && "need class LauncherProvider");
    //return mProvider->LoadDefaultFavoritesIfNecessary(mWorkspaceResId, mOverridePrevious);
    return NOERROR;
}

const String CPreloadReceiver::TAG("Launcher.PreloadReceiver");
const Boolean CPreloadReceiver::LOGD = FALSE;

CAR_INTERFACE_IMPL(CPreloadReceiver, BroadcastReceiver, IPreloadReceiver);

CAR_OBJECT_IMPL(CPreloadReceiver);

ECode CPreloadReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    AutoPtr<IContext> ctx;
    context->GetApplicationContext((IContext**)&ctx);
    AutoPtr<ILauncherApplication> app = ILauncherApplication::Probe(ctx);
    AutoPtr<ILauncherProvider> provider;
    assert(0 && "need class LauncherApplication");
    //app->GetLauncherProvider((ILauncherProvider**)&provider);
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