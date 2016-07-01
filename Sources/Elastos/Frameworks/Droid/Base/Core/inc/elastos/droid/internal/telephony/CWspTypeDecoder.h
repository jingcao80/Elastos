
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CWSPTYPEDECODER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CWSPTYPEDECODER_H__

#include "_Elastos_Droid_Internal_Telephony_CWspTypeDecoder.h"
#include "elastos/droid/internal/telephony/WspTypeDecoder.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CWspTypeDecoder)
    , public WspTypeDecoder
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CWSPTYPEDECODER_H__
