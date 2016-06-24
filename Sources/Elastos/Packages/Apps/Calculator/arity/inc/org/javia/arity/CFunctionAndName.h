
#ifndef _ORG_JAVIA_ARITY_CFUNCTIONANDNAME_H__
#define _ORG_JAVIA_ARITY_CFUNCTIONANDNAME_H__

#include "_Org_Javia_Arity_CFunctionAndName.h"
#include <elastos/core/Object.h>

namespace Org {
namespace Javia {
namespace Arity {

/**
    Encapsulates together a function and its name.
    Is used to return both the function and its name
    from Compiler.compileWithName().
 */
CarClass(CFunctionAndName)
    , public Object
    , public IFunctionAndName
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IFunction* fun,
        /* [in] */ const String& name);

public:
    AutoPtr<IFunction> mFunction;
    String mName;
};

} // namespace Arity
} // namespace Javia
} // namespace Org

#endif // _ORG_JAVIA_ARITY_CFUNCTIONANDNAME_H__