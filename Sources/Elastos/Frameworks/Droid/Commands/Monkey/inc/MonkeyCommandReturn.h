#ifndef __MONKEYCOMMANDRETURN_H_
#define __MONKEYCOMMANDRETURN_H_

#include "elastos/droid/ext/frameworkext.h"
#include <elastos.h>

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

class MonkeyCommandReturn
{
public:
    MonkeyCommandReturn();

    virtual CARAPI HasMessage(
        /* [out] */ Boolean *result);

    virtual CARAPI GetMessage(
        /* [out] */ String *message);

    virtual CARAPI WasSuccessful(
        /* [out] */ Boolean *result);

protected:
    CARAPI Init(
        /* [in] */ Boolean success,
        /* [in] */ const String& message = String());
private:
    volatile const Boolean mSuccess;
    volatile const String mMessage;
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif //__MONKEYCOMMANDRETURN_H_
