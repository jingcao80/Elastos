#include "org/conscrypt/COpenSSLBIOSinkHelper.h"
#include "org/conscrypt/OpenSSLBIOSink.h"

namespace Org {
namespace Conscrypt {

CAR_SINGLETON_IMPL(COpenSSLBIOSinkHelper)

CAR_INTERFACE_IMPL(COpenSSLBIOSinkHelper, Singleton, IOpenSSLBIOSink)

ECode COpenSSLBIOSinkHelper::Create(
    /* [out] */ IOpenSSLBIOSink** result)
{
    return OpenSSLBIOSink::Create(result);
}

} // namespace Conscrypt
} // namespace Org
