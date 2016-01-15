
#include "Elastos.Droid.Content.h"
#include "elastos/droid/webkit/native/ui/base/DeviceFormFactor.h"
#include "elastos/droid/webkit/native/ui/api/DeviceFormFactor_dec.h"

using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::IResources;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Ui {
namespace Base {

//=====================================================================
//                           DeviceFormFactor
//=====================================================================
const Int32 DeviceFormFactor::MINIMUM_TABLET_WIDTH_DP;
Boolean DeviceFormFactor::sIsTablet = FALSE;

Boolean DeviceFormFactor::IsTablet(
    /* [in] */ IContext* context)
{
    // ==================before translated======================
    // if (sIsTablet == null) {
    //     int minimumScreenWidthDp = context.getResources().getConfiguration().
    //             smallestScreenWidthDp;
    //     sIsTablet = minimumScreenWidthDp >= MINIMUM_TABLET_WIDTH_DP;
    // }
    // return sIsTablet;

    if (!sIsTablet) {
        AutoPtr<IResources> resources;
        context->GetResources((IResources**)&resources);

        AutoPtr<IConfiguration> configuration;
        resources->GetConfiguration((IConfiguration**)&configuration);

        Int32 minimumScreenWidthDp = configuration->SMALLEST_SCREEN_WIDTH_DP_UNDEFINED;
        sIsTablet = minimumScreenWidthDp >= MINIMUM_TABLET_WIDTH_DP;
    }
    return sIsTablet;
}

Boolean DeviceFormFactor::IsTablet(
    /* [in] */ IInterface* context)
{
    IContext* c = IContext::Probe(context);
    return IsTablet(c);
}

} // namespace Base
} // namespace Ui
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


