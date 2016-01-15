
#ifndef __ELASTOS_DROID_NET_CURLQUERYSANITIZERPARAMETERVALUEPAIR_H__
#define __ELASTOS_DROID_NET_CURLQUERYSANITIZERPARAMETERVALUEPAIR_H__

#include "_Elastos_Droid_Net_CUrlQuerySanitizerParameterValuePair.h"
#include "elastos/droid/net/UrlQuerySanitizer.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * A simple tuple that holds parameter-value pairs.
 *
 */
CarClass(CUrlQuerySanitizerParameterValuePair)
    , public UrlQuerySanitizerParameterValuePair
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_CURLQUERYSANITIZERPARAMETERVALUEPAIR_H__
