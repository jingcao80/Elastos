
#ifndef __CMONKEY_H__
#define __CMONKEY_H__

#include "_CMonkey.h"
#include "Monkey.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CarClass(CMonkey), public Monkey
{
public:
    CAR_OBJECT_DECL()

    CARAPI Main(
        /* [in] */ const ArrayOf<String>& args);
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __CMONKEY_H__
