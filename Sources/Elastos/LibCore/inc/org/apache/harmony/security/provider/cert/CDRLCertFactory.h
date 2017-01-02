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

#ifndef __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CERT_CDRLCERTFACTORY_H__
#define __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CERT_CDRLCERTFACTORY_H__

#include "_Org_Apache_Harmony_Security_Provider_Cert_CDRLCertFactory.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Provider {
namespace Cert {

//Todo later
CarClass(CDRLCertFactory), public Provider
{
public:
    CAR_OBJECT_DECL()

    CARAPI GetProperty(
        /* [in] */ const String& name,
        /* [out] */ String * pStr);

    CARAPI GetPropertyEx(
        /* [in] */ const String& name,
        /* [in] */ const String& defaultValue,
        /* [out] */ String * pStr);

    CARAPI List(
        /* [in] */ Elastos::IO::IPrintStream * pOutstream);

    CARAPI ListEx(
        /* [in] */ Elastos::IO::IPrintWriter * pOutwriter);

    CARAPI Load(
        /* [in] */ Elastos::IO::IInputStream * pInstream);

    CARAPI LoadEx(
        /* [in] */ Elastos::IO::IReader * pInreader);

    CARAPI PropertyNames(
        /* [out] */ IInterface ** ppInter);

    CARAPI StringPropertyNames(
        /* [out] */ Elastos::Utility::IObjectStringMap ** ppStrmap);

    CARAPI Save(
        /* [in] */ Elastos::IO::IOutputStream * pOutstream,
        /* [in] */ const String& comment);

    CARAPI SetProperty(
        /* [in] */ const String& name,
        /* [in] */ const String& value,
        /* [out] */ IObject ** ppObj);

    CARAPI Store(
        /* [in] */ Elastos::IO::IOutputStream * pOutstream,
        /* [in] */ const String& comment);

    CARAPI StoreEx(
        /* [in] */ Elastos::IO::IWriter * pWriter,
        /* [in] */ const String& comment);

    CARAPI LoadFromXML(
        /* [in] */ Elastos::IO::IInputStream * pInstream);

    CARAPI StoreToXML(
        /* [in] */ Elastos::IO::IOutputStream * pOs,
        /* [in] */ const String& comment);

    CARAPI StoreToXMLEx(
        /* [in] */ Elastos::IO::IOutputStream * pOs,
        /* [in] */ const String& comment,
        /* [in] */ const String& encoding);

    CARAPI GetName(
        /* [out] */ String * pName);

    CARAPI GetVersion(
        /* [out] */ Double * pVersionNumber);

    CARAPI GetInfo(
        /* [out] */ String * pInfo);

    CARAPI GetService(
        /* [in] */ const String& type,
        /* [in] */ const String& algorithm,
        /* [out] */ Elastos::Security::IProviderService ** ppService);

    CARAPI GetServices(
        /* [out] */ Elastos::Utility::ISet ** ppServices);

    CARAPI constructor();

private:
    /**
     * @serial
     */
    static Int64 serialVersionUID = -7269650779605195879LL;
};

} // namespace Cert
} // namespace Provider
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CERT_CDRLCERTFACTORY_H__
