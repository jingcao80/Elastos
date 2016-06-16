
#include "elastos/droid/phone/CIccProvider.h"

namespace Elastos {
namespace Droid {
namespace Phone {

CAR_INTERFACE_IMPL(CIccProvider, IccProvider, IIccProvider)

CAR_OBJECT_IMPL(CIccProvider)

ECode CIccProvider::constructor()
{
    return IccProvider::constructor();
}


} // namespace Phone
} // namespace Droid
} // namespace Elastos