
#include "elastos/droid/app/CNotificationBuilderHelper.h"
#include "elastos/droid/app/CNotificationBuilder.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_SINGLETON_IMPL(CNotificationBuilderHelper);
CAR_INTERFACE_IMPL(CNotificationBuilderHelper, Object, INotificationBuilderHelper);
ECode CNotificationBuilderHelper::CalculateTopPadding(
    /* [in] */ IContext* ctx,
    /* [in] */ Boolean hasThreeLines,
    /* [in] */ Float fontScale,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = CNotificationBuilder::CalculateTopPadding(ctx, hasThreeLines, fontScale);
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
