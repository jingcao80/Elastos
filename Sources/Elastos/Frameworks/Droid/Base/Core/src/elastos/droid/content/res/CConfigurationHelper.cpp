
#include "elastos/droid/content/res/CConfigurationHelper.h"
#include "elastos/droid/content/res/CConfiguration.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

CAR_INTERFACE_IMPL(CConfigurationHelper, Object, IConfigurationHelper)

CAR_SINGLETON_IMPL(CConfigurationHelper)

ECode CConfigurationHelper::ResetScreenLayout(
    /* [in] */ Int32 curLayout,
    /* [out] */ Int32* layout)
{
    VALIDATE_NOT_NULL(layout);
    *layout = CConfiguration::ResetScreenLayout(curLayout);
    return NOERROR;
}

ECode CConfigurationHelper::ReduceScreenLayout(
    /* [in] */ Int32 curLayout,
    /* [in] */ Int32 longSizeDp,
    /* [in] */ Int32 shortSizeDp,
    /* [out] */ Int32* layout)
{
    VALIDATE_NOT_NULL(layout);
    *layout = CConfiguration::ReduceScreenLayout(curLayout, longSizeDp, shortSizeDp);
    return NOERROR;
}

ECode CConfigurationHelper::NeedNewResources(
    /* [in] */ Int32 configChanges,
    /* [in] */ Int32 interestingChanges,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = CConfiguration::NeedNewResources(configChanges, interestingChanges);
    return NOERROR;
}

ECode CConfigurationHelper::GetEmpty(
    /* [out] */ IConfiguration** empty)
{
    VALIDATE_NOT_NULL(empty);
    *empty = CConfiguration::EMPTY;
    REFCOUNT_ADD(*empty);
    return NOERROR;
}

}
}
}
}

