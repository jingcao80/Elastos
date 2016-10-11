
#ifndef __ORG_CONSCRYPT_CSSLPARAMETERSIMPL_H__
#define __ORG_CONSCRYPT_CSSLPARAMETERSIMPL_H__

#include "_Org_Conscrypt_CSSLParametersImpl.h"
#include "SSLParametersImpl.h"

namespace Org {
namespace Conscrypt {

CarClass(CSSLParametersImpl)
    , public SSLParametersImpl
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_CSSLPARAMETERSIMPL_H__
