
#include "org/javia/arity/CFunctionAndName.h"

using Org::Javia::Arity::EIID_IFunctionAndName;

namespace Org {
namespace Javia {
namespace Arity {

CAR_OBJECT_IMPL(CFunctionAndName)

CAR_INTERFACE_IMPL(CFunctionAndName, Object, IFunctionAndName)

ECode CFunctionAndName::constructor(
    /* [in] */ IFunction* fun,
    /* [in] */ const String& name)
{
    mFunction = fun;
    mName = name;
    return NOERROR;
}

} // namespace Arity
} // namespace Javia
} // namespace Org