//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_SECURITY_CERT_CLDAPCERTSTOREPARAMETERS_H__
#define __ELASTOS_SECURITY_CERT_CLDAPCERTSTOREPARAMETERS_H__

#include "_Elastos_Security_Cert_CLDAPCertStoreParameters.h"
#include "core/Object.h"

using Elastos::Core::ICloneable;

namespace Elastos {
namespace Security {
namespace Cert {

CarClass(CLDAPCertStoreParameters)
    , public Object
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
