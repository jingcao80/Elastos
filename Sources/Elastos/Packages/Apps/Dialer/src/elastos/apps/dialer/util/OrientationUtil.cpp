
#include "elastos/apps/dialer/util/OrientationUtil.h"

using Elastos::Droid::Content::IConfiguration;
using Elastos::Droid::Content::Res::IResources;

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace Util {

Boolean OrientationUtil::IsLandscape(
    /* [in] */ IContext* context)
{
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<IConfiguration> configuration;
    res->GetConfiguration((IConfiguration**)&configuration);
    Int32 orientation;
    configuration->GetOrientation(&orientation);
    return orientation == IConfiguration::ORIENTATION_LANDSCAPE;
}

} // Util
} // Dialer
} // Apps
} // Elastos