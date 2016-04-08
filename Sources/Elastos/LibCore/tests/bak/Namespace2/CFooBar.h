
#ifndef __CFOOBAR_H__
#define __CFOOBAR_H__

#include "_CFooBar.h"

namespace Elastos {
namespace Namespace2 {

CarClass(CFooBar)
{
public:
    CARAPI Foo();

    CARAPI Bar(
        /* [in] */ IFoo * pFoo);

private:
    // TODO: Add your private member variables here.
};

}
}

#endif // __CFOOBAR_H__
