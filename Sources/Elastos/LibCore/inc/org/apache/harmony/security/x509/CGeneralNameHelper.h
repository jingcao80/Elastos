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

#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CGENERALNAMEHELPER_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CGENERALNAMEHELPER_H__

#include "_Org_Apache_Harmony_Security_X509_CGeneralNameHelper.h"
#include <elastos/core/Singleton.h>

using Org::Apache::Harmony::Security::Asn1::IASN1Choice;
using Elastos::Core::Singleton;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CGeneralNameHelper)
    , public Singleton
    , public IGeneralNameHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI CheckDNS(
        /* [in] */ const String& dns);

    CARAPI CheckURI(
        /* [in] */ const String& uri);

    CARAPI OidStrToInts(
        /* [in] */ const String& oid,
        /* [out, callee] */ ArrayOf<Int32>** ppInts);

    CARAPI IpStrToBytes(
        /* [in] */ const String& ip,
        /* [out, callee] */ ArrayOf<Byte>** ppBytes);

    CARAPI IpBytesToStr(
        /* [in] */ ArrayOf<Byte>* pIp,
        /* [out] */ String* pStr);

    CARAPI GetASN1(
        /* [out] */ IASN1Choice** ppAsn1);

    CARAPI SetASN1(
        /* [in] */ IASN1Choice* ppAsn1);
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CGENERALNAMEHELPER_H__
