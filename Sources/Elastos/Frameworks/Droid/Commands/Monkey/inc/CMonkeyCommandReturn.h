
#ifndef __CMONKEYCOMMANDRETURN_H__
#define __CMONKEYCOMMANDRETURN_H__

#include "_CMonkeyCommandReturn.h"
#include "MonkeyCommandReturn.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CarClass(CMonkeyCommandReturn), public MonkeyCommandReturn
{
public:
    CAR_OBJECT_DECL()

    CARAPI HasMessage(
        /* [out] */ Boolean *result);

    CARAPI GetMessage(
        /* [out] */ String *message);

    CARAPI WasSuccessful(
        /* [out] */ Boolean *result);

    CARAPI constructor(
        /* [in] */ Boolean success);

    CARAPI constructor(
        /* [in] */ Boolean success,
        /* [in] */ const String& message);
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __CMONKEYCOMMANDRETURN_H__
