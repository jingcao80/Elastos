#include "elastos/droid/utility/CSizeHelper.h"
#include "elastos/droid/utility/CSize.h"

namespace Elastos {
namespace Droid {
namespace Utility {

CAR_INTERFACE_IMPL(CSizeHelper, Singleton, ISizeHelper)

CAR_SINGLETON_IMPL(CSizeHelper)

ECode CSizeHelper::ParseSize(
    /* [in] */ const String& string,
    /* [out] */ ISize** outration)
{
    return CSize::ParseSize(string, outration);;
}


}
}
}
