
#include "CASN1AnyHelper.h"
#include "CASN1Any.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {


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

