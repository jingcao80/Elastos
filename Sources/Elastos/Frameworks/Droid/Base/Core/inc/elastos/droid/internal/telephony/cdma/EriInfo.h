#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_ERIINFO_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_ERIINFO_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

class EriInfo
    : public Object
    , public IEriInfo
{
public:
    CAR_INTERFACE_DECL();

    CARAPI constructor(
        /* [in] */ Int32 roamingIndicator,
        /* [in] */ Int32 iconIndex,
        /* [in] */ Int32 iconMode,
        /* [in] */ const String& eriText,
        /* [in] */ Int32 callPromptId,
        /* [in] */ Int32 alertId);

public:
    Int32 roamingIndicator;
    Int32 iconIndex;
    Int32 iconMode;
    String eriText;
    Int32 callPromptId;
    Int32 alertId;
};

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CDMA_ERIINFO_H__

