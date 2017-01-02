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

#include "security/cert/CLDAPCertStoreParameters.h"
#include "core/StringBuilder.h"

using Elastos::Core::EIID_ICloneable;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Security {
namespace Cert {

const String CLDAPCertStoreParameters::DEFAULT_LDAP_SERVER_NAME("localhost");

CAR_OBJECT_IMPL(CLDAPCertStoreParameters)
CAR_INTERFACE_IMPL_3(CLDAPCertStoreParameters, Object, ILDAPCertStoreParameters, ICertStoreParameters, ICloneable)

ECode CLDAPCertStoreParameters::Clone(
    /* [out] */ IInterface **object)
{
    VALIDATE_NOT_NULL(object)
    AutoPtr<ILDAPCertStoreParameters> clonedObj;
    FAIL_RETURN(CLDAPCertStoreParameters::New((ILDAPCertStoreParameters**)&clonedObj))
    *object = clonedObj;
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode CLDAPCertStoreParameters::GetPort(
    /* [out] */ Int32 *port)
{
    VALIDATE_NOT_NULL(port)
    *port = mPort;
    return NOERROR;
}

ECode CLDAPCertStoreParameters::GetServerName(
    /* [out] */ String *name)
{
    VALIDATE_NOT_NULL(name)
    *name = mServerName;
    return NOERROR;
}

ECode CLDAPCertStoreParameters::ToString(
    /* [out] */ String *str)
{
    StringBuilder sb("LDAPCertStoreParameters: [\n serverName: ");
    String sn;
    GetServerName(&sn);
    sb.Append(sn);
    sb.Append("\n port: ");
    Int32 port;
    GetPort(&port);
    sb.Append(port);
    sb.Append("\n]");
    return sb.ToString(str);
}

ECode CLDAPCertStoreParameters::constructor(
    /* [in] */ const String& serverName,
    /* [in] */ Int32 port)
{
    if (serverName.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }
    mPort = port;
    mServerName = serverName;
    return NOERROR;
}

ECode CLDAPCertStoreParameters::constructor()
{
    mPort = DEFAULT_LDAP_PORT;
    mServerName = DEFAULT_LDAP_SERVER_NAME;
    return NOERROR;
}

ECode CLDAPCertStoreParameters::constructor(
    /* [in] */ const String& serverName)
{
    if (serverName.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }
    mPort = DEFAULT_LDAP_PORT;
    mServerName = serverName;
    return NOERROR;
}

}
}
}
