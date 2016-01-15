
#ifndef __ELASTOS_DROID_NET_CMAILTO_H__
#define __ELASTOS_DROID_NET_CMAILTO_H__

#include "_Elastos_Droid_Net_CMailTo.h"
#include "elastos/droid/net/MailTo.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 *
 * MailTo URL parser
 *
 * This class parses a mailto scheme URL and then can be queried for
 * the parsed parameters. This implements RFC 2368.
 *
 */
CarClass(CMailTo)
    , public MailTo
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_CMAILTO_H__
