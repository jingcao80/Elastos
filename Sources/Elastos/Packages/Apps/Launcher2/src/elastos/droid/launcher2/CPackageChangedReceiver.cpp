
#include "elastos/droid/launcher2/CPackageChangedReceiver.h"
#include "elastos/droid/launcher2/LauncherApplication.h"
#include "elastos/droid/launcher2/WidgetPreviewLoader.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Service.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Net::IUri;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_OBJECT_IMPL(CPackageChangedReceiver);

CPackageChangedReceiver::CPackageChangedReceiver()
{
    Slogger::E("CPackageChangedReceiver", "=========CPackageChangedReceiver::CPackageChangedReceiver()");
}

ECode CPackageChangedReceiver::constructor()
{
    return BroadcastReceiver::constructor();
}

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
    AutoPtr<LauncherApplication> app = (LauncherApplication*)IObject::Probe(ctx);
    AutoPtr<IWidgetPreviewLoaderCacheDb> db;
    app->GetWidgetPreviewCacheDb((IWidgetPreviewLoaderCacheDb**)&db);
    return WidgetPreviewLoader::RemoveFromDb(
            (WidgetPreviewLoader::CacheDb*)db.Get(), packageName);
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos