
#ifndef _ORG_JAVIA_ARITY_CCOMPILEDFUNCTION_H__
#define _ORG_JAVIA_ARITY_CCOMPILEDFUNCTION_H__

#include "_Org_Javia_Arity_CCompiledFunction.h"
#include "org/javia/arity/CompiledFunction.h"

namespace Org {
namespace Javia {
namespace Arity {

CarClass(CCompiledFunction), public CompiledFunction
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Arity
} // namespace Javia
} // namespace Org

#endif // _ORG_JAVIA_ARITY_CCOMPILEDFUNCTION_H__