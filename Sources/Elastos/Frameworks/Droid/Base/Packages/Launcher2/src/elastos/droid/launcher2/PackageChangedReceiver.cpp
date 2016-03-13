
#include "elastos/droid/launcher2/PackageChangedReceiver.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(PackageChangedReceiver, BroadcastReceiver, IPackageChangedReceiver);

ECode PackageChangedReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    AutoPtr<IUri> data;
    intent->GetData((IUri**)&data);
    String packageName;
    data->GetSchemeSpecificPart(&packageName);

    if (packageName.IsNull() || packageName->GetLength() == 0) {
        // they sent us a bad intent
        return NOERROR;
    }

    AutoPtr<IContext> ctx;
    context->GetApplicationContext((IContext**)&ctx);
    AutoPtr<LauncherApplication> app = (LauncherApplication*)IObject::Probe(ctx);
    AutoPtr<WidgetPreviewLoader::CacheDb> db;
    app->GetWidgetPreviewCacheDb((WidgetPreviewLoader::CacheDb**)&db)
    return WidgetPreviewLoader::RemoveFromDb(db, packageName);
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos