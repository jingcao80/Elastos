#include "elastos/droid/utility/CSizeFHelper.h"
#include "elastos/droid/utility/CSizeF.h"

namespace Elastos {
namespace Droid {
namespace Utility {

CAR_INTERFACE_IMPL(CSizeFHelper, Singleton, ISizeFHelper)

CAR_SINGLETON_IMPL(CSizeFHelper)

ECode CSizeFHelper::ParseSizeF(
    /* [in] */ const String& string,
    /* [out] */ ISizeF** outration)
{
    return CSizeF::ParseSizeF(string, outration);
}


}
}
}
