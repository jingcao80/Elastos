
#ifndef __ORG_CONSCRYPT_COPENSSLBIOSINK_H__
#define __ORG_CONSCRYPT_COPENSSLBIOSINK_H__

#include "_Org_Conscrypt_COpenSSLBIOSink.h"
#include "OpenSSLBIOSink.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLBIOSink)
    , public OpenSSLBIOSink
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLBIOSINK_H__
