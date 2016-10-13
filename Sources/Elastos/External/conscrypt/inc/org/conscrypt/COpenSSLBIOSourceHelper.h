
#ifndef __ORG_CONSCRYPT_COPENSSLBIOSOURCEHELPER_H__
#define __ORG_CONSCRYPT_COPENSSLBIOSOURCEHELPER_H__

#include "_Org_Conscrypt_COpenSSLBIOSourceHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::IO::IByteBuffer;

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLBIOSourceHelper)
    , public Singleton
    , public IOpenSSLBIOSourceHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI Wrap(
        /* [in] */ IByteBuffer* buffer,
        /* [out] */ IOpenSSLBIOSource** result);
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLBIOSOURCEHELPER_H__
