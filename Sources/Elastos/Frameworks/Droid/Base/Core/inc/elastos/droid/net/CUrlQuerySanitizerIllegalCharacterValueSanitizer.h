
#ifndef __ELASTOS_DROID_NET_CURLQUERYSANITIZERILLEGALCHARACTERVALUESANITIZER_H__
#define __ELASTOS_DROID_NET_CURLQUERYSANITIZERILLEGALCHARACTERVALUESANITIZER_H__

#include "_Elastos_Droid_Net_CUrlQuerySanitizerIllegalCharacterValueSanitizer.h"
#include "elastos/droid/net/UrlQuerySanitizer.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Sanitize values based on which characters they contain. Illegal
 * characters are replaced with either space or '_', depending upon
 * whether space is a legal character or not.
 */
CarClass(CUrlQuerySanitizerIllegalCharacterValueSanitizer)
    , public UrlQuerySanitizerIllegalCharacterValueSanitizer
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_CURLQUERYSANITIZERILLEGALCHARACTERVALUESANITIZER_H__
