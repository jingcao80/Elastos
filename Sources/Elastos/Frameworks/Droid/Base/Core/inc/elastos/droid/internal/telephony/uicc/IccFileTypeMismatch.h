#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCFILETYPEMISMATCH_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCFILETYPEMISMATCH_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/internal/telephony/uicc/IccException.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

/**
  * {@hide}
  */
class IccFileTypeMismatch
    : public IccException
    , public IIccFileTypeMismatch
{
public:
    CAR_INTERFACE_DECL();

    IccFileTypeMismatch();

    IccFileTypeMismatch(
        /* [in] */ const String& s);
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCFILETYPEMISMATCH_H__

