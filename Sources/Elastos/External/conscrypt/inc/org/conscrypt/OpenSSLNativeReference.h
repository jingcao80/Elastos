
#ifndef __ORG_CONSCRYPT_OPENSSLNATIVEREFERENCE_H__
#define __ORG_CONSCRYPT_OPENSSLNATIVEREFERENCE_H__

#include "Org.Conscrypt.h"
#include <elastos/core/Object.h>

namespace Org {
namespace Conscrypt {

/**
 * Used to hold onto native OpenSSL references and run finalization on those
 * objects. Individual types must subclass this and implement finalizer.
 */
class OpenSSLNativeReference
    : public Object
{
public:
    OpenSSLNativeReference(
        /* [in] */ Int64 ctx);

public:
    Int64 mContext;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLNATIVEREFERENCE_H__