#include "org/conscrypt/COpenSSLBIOSourceHelper.h"
#include "org/conscrypt/OpenSSLBIOSource.h"

namespace Org {
namespace Conscrypt {

CAR_SINGLETON_IMPL(COpenSSLBIOSourceHelper)

CAR_INTERFACE_IMPL(COpenSSLBIOSourceHelper, Singleton, IOpenSSLBIOSource)

ECode COpenSSLBIOSourceHelper::Wrap(
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ IOpenSSLBIOSource** result)
{
    return OpenSSLBIOSource::Wrap(buffer, result);
}

} // namespace Conscrypt
} // namespace Org
