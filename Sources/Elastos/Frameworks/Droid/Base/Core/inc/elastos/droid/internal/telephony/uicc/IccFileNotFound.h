#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCFILENOTFOUND_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCFILENOTFOUND_H__

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
class IccFileNotFound
    : public IccException
    , public IIccFileNotFound
{
public:
    CAR_INTERFACE_DECL();

    IccFileNotFound();

    IccFileNotFound(
        /* [in] */ const String& s);

    IccFileNotFound(
        /* [in] */ Int32 ef);
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCFILENOTFOUND_H__

