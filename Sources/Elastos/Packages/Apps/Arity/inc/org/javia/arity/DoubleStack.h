
#ifndef _ORG_JAVIA_ARITY_DOUBLESTACK_H__
#define _ORG_JAVIA_ARITY_DOUBLESTACK_H__

#include "_Org.Javia.Arity.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

namespace Org {
namespace Javia {
namespace Arity {

class DoubleStack : public Object
{
public:
    DoubleStack();

    CARAPI_(void) Clear();

    CARAPI_(void) Push(
        /* [in] */ Double a,
        /* [in] */ Double b);

    CARAPI Pop(
        /* [in] */ Int32 cnt);

    CARAPI_(void) Pop();

    CARAPI_(AutoPtr<ArrayOf<Double> >) GetRe();

    CARAPI_(AutoPtr<ArrayOf<Double> >) GetIm();

private:
    AutoPtr<ArrayOf<Double> > mRe;
    AutoPtr<ArrayOf<Double> > mIm;
    Int32 mSize;
};

} // namespace Arity
} // namespace Javia
} // namespace Org

#endif // _ORG_JAVIA_ARITY_DOUBLESTACK_H__