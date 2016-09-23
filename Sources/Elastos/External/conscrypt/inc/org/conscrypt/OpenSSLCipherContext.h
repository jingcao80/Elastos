
#ifndef __ORG_CONSCRYPT_OPENSSLCIPHERCONTEXT_H__
#define __ORG_CONSCRYPT_OPENSSLCIPHERCONTEXT_H__

#include <elastos/coredef.h>
#include <elastos/core/Object.h>
#include "_Org.Conscrypt.h"

namespace Org {
namespace Conscrypt {

class OpenSSLCipherContext
    : public IOpenSSLCipherContext
    , public Object
{
public:
    CAR_INTERFACE_DECL()

    OpenSSLCipherContext();

    ~OpenSSLCipherContext();

    CARAPI constructor(
        /* [in] */ Int64 ctx);

    CARAPI GetContext(
        /* [out] */ Int64* result);

private:
    Int64 mContext;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLCIPHERCONTEXT_H__
