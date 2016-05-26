
#ifndef __CMONKEYSOURCENETWORK_H__
#define __CMONKEYSOURCENETWORK_H__

#include "_CMonkeySourceNetwork.h"
#include "MonkeySourceNetwork.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CarClass(CMonkeySourceNetwork) , public MonkeySourceNetwork
{
public:
    CAR_OBJECT_DECL()

    CARAPI GetNextEvent(
        /* [out] */ IMonkeyEvent **event);

    CARAPI SetVerbose(
        /* [in] */ Int32 verbose);

    CARAPI Validate(
        /* [out] */ Boolean *result);

    CARAPI constructor(
        /* [in] */ Int32 port);
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __CMONKEYSOURCENETWORK_H__
