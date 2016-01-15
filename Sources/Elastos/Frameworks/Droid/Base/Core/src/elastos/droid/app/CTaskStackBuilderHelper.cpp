#include "elastos/droid/app/CTaskStackBuilderHelper.h"
#include "elastos/droid/app/CTaskStackBuilder.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(CTaskStackBuilderHelper, Singleton, ITaskStackBuilderHelper)

CAR_SINGLETON_IMPL(CTaskStackBuilderHelper)

ECode CTaskStackBuilderHelper::Create(
    /* [in] */ IContext* context,
    /* [out] */ ITaskStackBuilder** builder)
{
    VALIDATE_NOT_NULL(builder);
    AutoPtr<ITaskStackBuilder> tsb = CTaskStackBuilder::Create(context);
    *builder = tsb;
    REFCOUNT_ADD(*builder);
    return NOERROR;
}

}
}
}
