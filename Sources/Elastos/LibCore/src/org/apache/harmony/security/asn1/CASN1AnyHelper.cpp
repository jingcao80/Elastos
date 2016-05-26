
#include "CASN1AnyHelper.h"
#include "CASN1Any.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CAR_SINGLETON_IMPL(CASN1AnyHelper)

CAR_INTERFACE_IMPL(CASN1AnyHelper, Singleton, IASN1AnyHelper)

ECode CASN1AnyHelper::GetInstance(
    /* [out] */ IASN1Any** instance)
{
    return CASN1Any::GetInstance(instance);
}

}
}
}
}
}

