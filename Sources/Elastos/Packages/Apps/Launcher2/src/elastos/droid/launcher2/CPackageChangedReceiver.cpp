
#include "elastos/droid/launcher2/CPackageChangedReceiver.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

ECode CPackageChangedReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    AutoPtr<IUri> data;
    intent->GetData((IUri**)&data);
    String packageName;
    data->GetSchemeSpecificPart(&packageName);

    if (packageName.IsNull() || packageName.GetLength() == 0) {
        // they sent us a bad intent
        return NOERROR;
    }

    AutoPtr<IContext> ctx;
    context->GetApplicationContext((IContext**)&ctx);
    assert(0 && "need class LauncherApplication");
    // AutoPtr<LauncherApplication> app = (LauncherApplication*)IObject::Probe(ctx);
    // AutoPtr<WidgetPreviewLoader::CacheDb> db;
    // app->GetWidgetPreviewCacheDb((WidgetPreviewLoader::CacheDb**)&db)
    // return WidgetPreviewLoader::RemoveFromDb(db, packageName);
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos