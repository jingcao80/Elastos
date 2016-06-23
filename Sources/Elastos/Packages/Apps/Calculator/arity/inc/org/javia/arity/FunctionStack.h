
#ifndef _ORG_JAVIA_ARITY_FUNCTIONSTACK_H__
#define _ORG_JAVIA_ARITY_FUNCTIONSTACK_H__

#include "_Org.Javia.Arity.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

namespace Org {
namespace Javia {
namespace Arity {

class FunctionStack : public Object
{
public:
    FunctionStack();

    CARAPI_(void) Clear();

    CARAPI_(void) Push(
        /* [in] */ IFunction* b);

    /*
    void pop(int cnt) {
        size -= cnt;
    }
    */

    CARAPI_(AutoPtr<IFunction>) Pop();

    CARAPI_(AutoPtr<ArrayOf<IFunction*> >) ToArray();

private:
    AutoPtr<ArrayOf<IFunction*> > mData;
    Int32 mSize;
};

} // namespace Arity
} // namespace Javia
} // namespace Org

#endif // _ORG_JAVIA_ARITY_FUNCTIONSTACK_H__