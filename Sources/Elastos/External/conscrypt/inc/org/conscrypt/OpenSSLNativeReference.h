
#ifndef __ORG_CONSCRYPT_OPENSSLNATIVEREFERENCE_H__
#define __ORG_CONSCRYPT_OPENSSLNATIVEREFERENCE_H__

#include "_Org.Conscrypt.h"
#include <elastos/core/Object.h>

namespace Org {
namespace Conscrypt {

/**
 * Used to hold onto native OpenSSL references and run finalization on those
 * objects. Individual types must subclass this and implement finalizer.
 */
class OpenSSLNativeReference
    : public Object
    , public IOpenSSLNativeReference
{
public:
    CAR_INTERFACE_DECL();

    OpenSSLNativeReference();

    CARAPI constructor(
        /* [in] */ Int64 ctx);

    CARAPI GetNativeContext(
        /* [out] */ Int64* ctxRef);

public:
    Int64 mContext;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLNATIVEREFERENCE_H__
