
#ifndef __ELASTOS_SECURITY_CERT_CLDAPCERTSTOREPARAMETERS_H__
#define __ELASTOS_SECURITY_CERT_CLDAPCERTSTOREPARAMETERS_H__

#include "_Elastos_Security_Cert_CLDAPCertStoreParameters.h"

namespace Elastos {
namespace Security {
namespace Cert {

CarClass(CLDAPCertStoreParameters)
    : public Object
    , public ILDAPCertStoreParameters
    , public ICertStoreParameters
    , public ICloneable
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI Clone(
        /* [out] */ IInterface **object);

    CARAPI GetPort(
        /* [out] */ Int32 *port);

    CARAPI GetServerName(
        /* [out] */ String *name);

    CARAPI ToString(
        /* [out] */ String *str);

    CARAPI constructor(
        /* [in] */ const String& serverName,
        /* [in] */ Int32 port);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& serverName);

private:
    // Default LDAP server name
    static const String DEFAULT_LDAP_SERVER_NAME;
    // Default LDAP server port number
    static const Int32 DEFAULT_LDAP_PORT = 389;

    // LDAP server name for this cert store
    String mServerName;
    // LDAP server port number for this cert store
    Int32 mPort;
};

}
}
}

#endif // __ELASTOS_SECURITY_CERT_CLDAPCERTSTOREPARAMETERS_H__
