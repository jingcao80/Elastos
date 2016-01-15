
#ifndef __ELASTOS_DROID_NET_CSNTPCLIENT_H__
#define __ELASTOS_DROID_NET_CSNTPCLIENT_H__

#include "_Elastos_Droid_Net_CSntpClient.h"
#include "elastos/droid/net/SntpClient.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * {@hide}
 *
 * Simple SNTP client class for retrieving network time.
 *
 * Sample usage:
 * <pre>SntpClient client = new SntpClient();
 * if (client.requestTime("time.foo.com")) {
 *     long now = client.getNtpTime() + SystemClock.elapsedRealtime() - client.getNtpTimeReference();
 * }
 * </pre>
 */
CarClass(CSntpClient)
    , public SntpClient
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_CSNTPCLIENT_H__
