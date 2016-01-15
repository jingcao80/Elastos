
#include "elastos/droid/content/CIntentFilterHelper.h"
#include "elastos/droid/content/IntentFilter.h"

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL(CIntentFilterHelper, Object, IIntentFilterHelper)

CAR_SINGLETON_IMPL(CIntentFilterHelper)

ECode CIntentFilterHelper::Create(
    /* [in] */ const String& action,
    /* [in] */ const String& dataType,
    /* [out] */ IIntentFilter** intentFilter)
{
    return IntentFilter::Create(action, dataType, intentFilter);
}

}
}
}

