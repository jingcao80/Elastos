
#ifndef __ELASTOSX_NET_SSL_CSSLPERMISSION_H__
#define __ELASTOSX_NET_SSL_CSSLPERMISSION_H__

#include "_Elastosx_Net_Ssl_CSSLPermission.h"
//TODO #include "BasicPermission.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::Security::IPermission;
//TODO using Elastos::Security::BasicPermission;

namespace Elastosx {
namespace Net {
namespace Ssl {

/**
 * Legacy security code; do not use.
 */
CarClass(CSSLPermission)
//TOD0    , public BasicPermission
    , public Object
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ const String& name);

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ const String& actions);

    CARAPI GetActions(
        /* [out] */ String* action);

    CARAPI Implies(
        /* [in] */ IPermission* permission,
        /* [out] */ Boolean* result);
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif //__ELASTOSX_NET_SSL_CSSLPERMISSION_H__
