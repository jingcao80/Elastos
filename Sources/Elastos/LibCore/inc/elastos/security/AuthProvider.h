
#ifndef __ELASTOS_SECURITY_AUTHPROVIDER_H__
#define __ELASTOS_SECURITY_AUTHPROVIDER_H__

#include "Provider.h"

namespace Elastos {
namespace Security {

class AuthProvider
    : public Provider
    , public IAuthProvider
{
public:
    CAR_INTERFACE_DECL()

    virtual ~AuthProvider() = 0;

protected:
    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ Double version,
        /* [in] */ const String& info);
};

} // namespace Security
} // namespace Elastos

#endif // __ELASTOS_SECURITY_AUTHPROVIDER_H__
