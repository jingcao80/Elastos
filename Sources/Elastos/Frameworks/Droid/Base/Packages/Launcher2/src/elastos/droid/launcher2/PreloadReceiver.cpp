
#include "elastos/droid/launcher2/PreloadReceiver.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

MyRunnable::MyRunnable(
    /* [in] */ ILauncherProvider* provider,
    /* [in] */ Int32 workspaceResId,
    /* [in] */ Boolean overridePrevious)
    : mProvider(provider)
    , mWorkspaceResId(workspaceResId)
    , mOverridePrevious(overridePrevious)
{
}

ECode MyRunnable::Run()
{
    return mProvider->LoadDefaultFavoritesIfNecessary(mWorkspaceResId, mOverridePrevious);
}

const String PreloadReceiver::TAG("Launcher.PreloadReceiver");
const Boolean PreloadReceiver::LOGD = FALSE;

CAR_INTERFACE_IMPL(PreloadReceiver, BroadcastReceiver, IPreloadReceiver);

ECode PreloadReceiver::OnReceive(
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
        intent->GetStringExtra(EXTRA_WORKSPACE_NAME, &name);
        Int32 workspaceResId;
        if (!TextUtils::IsEmpty(name)) {
            AutoPtr<IResources> res;
            context->GetResources(&res);
            res->GetIdentifier(name, String("xml"), String("com.android.launcher"), &workspaceResId);
        }
        else {
            workspaceResId = 0;
        }

        Boolean overridePrevious;
        intent->GetBooleanExtra(EXTRA_OVERRIDE_PREVIOUS, FALSE);
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