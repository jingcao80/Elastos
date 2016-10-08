
#ifndef __ORG_CONSCRYPT_OPENSSLECPOINTCONTEXT_H__
#define __ORG_CONSCRYPT_OPENSSLECPOINTCONTEXT_H__

#include <elastos/coredef.h>
#include <Elastos.CoreLibrary.Security.h>
#include <elastos/core/Object.h>
#include "_Org.Conscrypt.h"

using Elastos::Security::Spec::IECPoint;

namespace Org {
namespace Conscrypt {

/**
 * Byte array wrapper for hashtable use. Implements equals() and hashCode().
 */
class OpenSSLECPointContext
    : public IOpenSSLECPointContext
    , public Object
{
public:
    CAR_INTERFACE_DECL()

    OpenSSLECPointContext();

    ~OpenSSLECPointContext();

    CARAPI constructor (
        /* [in] */ IOpenSSLECGroupContext* group,
        /* [in] */ Int64 pointCtx);

    CARAPI GetECPoint(
        /* [out] */ IECPoint** result);

    CARAPI GetContext(
        /* [out] */ Int64* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    static CARAPI GetInstance(
        /* [in] */ Int32 curveType,
        /* [in] */ IOpenSSLECGroupContext* group,
        /* [in] */ IECPoint* javaPoint,
        /* [out] */ IOpenSSLECPointContext** result);

private:
    AutoPtr<IOpenSSLECGroupContext> mGroup;
    Int64 mPointCtx;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLECPOINTCONTEXT_H__
