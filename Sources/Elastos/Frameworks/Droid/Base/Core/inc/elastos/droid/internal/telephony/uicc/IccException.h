#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCEXCEPTION_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCEXCEPTION_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

/**
  * {@hide}
  */
class IccException
    : public Object//Exception
    , public IIccException
{
public:
    CAR_INTERFACE_DECL();

    IccException();

    IccException(
        /* [in] */ const String& s);
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCEXCEPTION_H__

