
#ifndef _ORG_JAVIA_ARITY_BYTESTACK_H__
#define _ORG_JAVIA_ARITY_BYTESTACK_H__

#include "_Org.Javia.Arity.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

namespace Org {
namespace Javia {
namespace Arity {

class ByteStack : public Object
{
public:
    ByteStack();

    CARAPI_(void) Clear();

    CARAPI_(void) Push(
        /* [in] */ Byte b);

    /*
    void pop(int cnt) {
        size -= cnt;
    }
    */

    CARAPI_(Byte) Pop();

    CARAPI_(AutoPtr<ArrayOf<Byte> >) ToArray();

private:
    AutoPtr<ArrayOf<Byte> > mData;
    Int32 mSize;
};

} // namespace Arity
} // namespace Javia
} // namespace Org

#endif // _ORG_JAVIA_ARITY_BYTESTACK_H__