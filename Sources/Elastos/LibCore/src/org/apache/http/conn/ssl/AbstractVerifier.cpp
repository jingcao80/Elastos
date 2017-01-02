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

#include "Elastos.CoreLibrary.Security.h"
#include "org/apache/http/conn/ssl/AbstractVerifier.h"
#include "org/apache/http/conn/util/InetAddressUtils.h"
#include "elastos/core/CString.h"
#include "elastos/utility/logging/Logger.h"
#include "elastos/utility/Arrays.h"
#include "elastos/utility/CLinkedList.h"
#include "elastosx/net/ssl/CDistinguishedNameParser.h"

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::IInteger32;
using Elastos::Security::Cert::ICertificate;
using Elastos::Utility::Arrays;
using Elastos::Utility::ICollection;
using Elastos::Utility::IIterable;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;
using Elastos::Utility::ILinkedList;
using Elastos::Utility::CLinkedList;
using Elastos::Utility::ILocale;
using Elastos::Utility::Logging::Logger;
using Elastosx::Net::Ssl::EIID_IHostnameVerifier;
using Elastosx::Net::Ssl::CDistinguishedNameParser;
using Elastosx::Net::Ssl::IDistinguishedNameParser;
using Elastosx::Security::Auth::X500::IX500Principal;
using Org::Apache::Http::Conn::Util::InetAddressUtils;

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace SSL {

static AutoPtr< ArrayOf<ICharSequence*> > InitBadCountry2lds()
{
    AutoPtr< ArrayOf<ICharSequence*> > stringArray = ArrayOf<ICharSequence*>::Alloc(14);
    CString::New(String("ac"), (ICharSequence**)&(*stringArray)[0]);
    CString::New(String("co"), (ICharSequence**)&(*stringArray)[1]);
    CString::New(String("com"), (ICharSequence**)&(*stringArray)[2]);
    CString::New(String("ed"), (ICharSequence**)&(*stringArray)[3]);
    CString::New(String("edu"), (ICharSequence**)&(*stringArray)[4]);
    CString::New(String("go"), (ICharSequence**)&(*stringArray)[5]);
    CString::New(String("gouv"), (ICharSequence**)&(*stringArray)[6]);
    CString::New(String("gov"), (ICharSequence**)&(*stringArray)[7]);
    CString::New(String("info"), (ICharSequence**)&(*stringArray)[8]);
    CString::New(String("lg"), (ICharSequence**)&(*stringArray)[9]);
    CString::New(String("ne"), (ICharSequence**)&(*stringArray)[10]);
    CString::New(String("net"), (ICharSequence**)&(*stringArray)[11]);
    CString::New(String("or"), (ICharSequence**)&(*stringArray)[12]);
    CString::New(String("org"), (ICharSequence**)&(*stringArray)[13]);
    Arrays::Sort(stringArray.Get());
    return stringArray;

}
const AutoPtr< ArrayOf<ICharSequence*> > AbstractVerifier::BAD_COUNTRY_2LDS = InitBadCountry2lds();

CAR_INTERFACE_IMPL_3(AbstractVerifier, Object, IAbstractVerifier, IX509HostnameVerifier, IHostnameVerifier)

ECode AbstractVerifier::Verify(
    /* [in] */ const String& host,
    /* [in] */ ISSLSocket* ssl)
{
    if(host == NULL) {
        Logger::E("AbstractVerifier", "host to verify is null");
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<ISSLSession> session;
    ssl->GetSession((ISSLSession**)&session);
    AutoPtr< ArrayOf<ICertificate*> > certs;
    session->GetPeerCertificates((ArrayOf<ICertificate*>**)&certs);
    AutoPtr<IX509Certificate> x509 = IX509Certificate::Probe((*certs)[0]);
    return Verify(host, x509);
}

ECode AbstractVerifier::Verify(
    /* [in] */ const String& host,
    /* [in] */ ISSLSession* session,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    // try {
    AutoPtr< ArrayOf<ICertificate*> > certs;
    session->GetPeerCertificates((ArrayOf<ICertificate*>**)&certs);
    AutoPtr<IX509Certificate> x509 = IX509Certificate::Probe((*certs)[0]);
    FAIL_RETURN(Verify(host, x509))
    *result = TRUE;
    return NOERROR;
    // }
    // catch(SSLException e) {
    //     return false;
    // }
}

ECode AbstractVerifier::Verify(
    /* [in] */ const String& host,
    /* [in] */ IX509Certificate* cert)
{
    AutoPtr< ArrayOf<String> > cns = GetCNs(cert);
    AutoPtr< ArrayOf<String> > subjectAlts = GetDNSSubjectAlts(cert);
    return Verify(host, cns, subjectAlts);
}

ECode AbstractVerifier::Verify(
    /* [in] */ const String& host,
    /* [in] */ ArrayOf<String>* cns,
    /* [in] */ ArrayOf<String>* subjectAlts,
    /* [in] */ Boolean strictWithSubDomains)
{
    // Build the list of names we're going to check.  Our DEFAULT and
    // STRICT implementations of the HostnameVerifier only use the
    // first CN provided.  All other CNs are ignored.
    // (Firefox, wget, curl, Sun Java 1.4, 5, 6 all work this way).
    AutoPtr<ILinkedList> names;
    CLinkedList::New((ILinkedList**)&names);
    AutoPtr<ICollection> col = ICollection::Probe(names);
    if(cns != NULL && cns->GetLength() > 0 && !(*cns)[0].IsNull()) {
        AutoPtr<ICharSequence> cs;
        CString::New((*cns)[0], (ICharSequence**)&cs);
        col->Add(cs);
    }
    if(subjectAlts != NULL) {
        for (Int32 i = 0; i < subjectAlts->GetLength(); ++i) {
            String subjectAlt = (*subjectAlts)[i];
            if (!subjectAlt.IsNull()) {
                AutoPtr<ICharSequence> cs;
                CString::New(subjectAlt, (ICharSequence**)&cs);
                col->Add(cs);
            }
        }
    }

    Boolean isEmpty;
    if(names->IsEmpty(&isEmpty), isEmpty) {
        Logger::E("AbstractVerifier", "Certificate for <%s> doesn't contain CN or DNS subjectAlt", host.string());
        return E_SSL_EXCEPTION;
    }

    // StringBuffer for building the error message.
    StringBuilder buf;

    // We're can be case-insensitive when comparing the host we used to
    // establish the socket to the hostname in the certificate.
    String hostName = host.Trim().ToLowerCase(/*ILocale::ENGLISH*/);
    Boolean match = FALSE;
    AutoPtr<IIterator> it;
    names->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> cs;
        it->GetNext((IInterface**)&cs);
        String cn;
        ICharSequence::Probe(cs)->ToString(&cn);
        cn = cn.ToLowerCase(/*ILocale::ENGLISH*/);
        // Store CN in StringBuffer in case we need to report an error.
        buf.Append(" <");
        buf.Append(cn);
        buf.AppendChar('>');
        Boolean result;
        if(it->HasNext(&result), result) {
            buf.Append(" OR");
        }

        // The CN better have at least two dots if it wants wildcard
        // action.  It also can't be [*.co.uk] or [*.co.jp] or
        // [*.org.uk], etc...
        Boolean isipv4;
        Boolean doWildcard = cn.StartWith("*.") &&
                cn.IndexOf('.', 2) != -1 &&
                AcceptableCountryWildcard(cn) &&
                (InetAddressUtils::IsIPv4Address(host, &isipv4), !isipv4);

        if(doWildcard) {
            match = hostName.EndWith(cn.Substring(1));
            if(match && strictWithSubDomains) {
                // If we're in strict mode, then [*.foo.com] is not
                // allowed to match [a.b.foo.com]
                match = CountDots(hostName) == CountDots(cn);
            }
        }
        else {
            match = hostName.Equals(cn);
        }
        if(match) {
            break;
        }
    }
    if(!match) {
        Logger::E("AbstractVerifier", "hostname in certificate didn't match: <%s> !=%s", host.string(), buf.ToString().string());
        return E_SSL_EXCEPTION;
    }
    return NOERROR;
}

Boolean AbstractVerifier::AcceptableCountryWildcard(
    /* [in] */ const String& cn)
{
    Int32 cnLen = cn.GetLength();
    if(cnLen >= 7 && cnLen <= 9) {
        // Look for the '.' in the 3rd-last position:
        if(cn.GetChar(cnLen - 3) == '.') {
            // Trim off the [*.] and the [.XX].
            String s = cn.Substring(2, cnLen - 3);
            AutoPtr<ICharSequence> cs;
            CString::New(s, (ICharSequence**)&cs);
            // And test against the sorted array of bad 2lds:
            Int32 x;
            Arrays::BinarySearch(BAD_COUNTRY_2LDS.Get(), cs.Get(), &x);
            return x < 0;
        }
    }
    return TRUE;
}

AutoPtr< ArrayOf<String> > AbstractVerifier::GetCNs(
    /* [in] */ IX509Certificate* cert)
{
    AutoPtr<IX500Principal> principal;
    cert->GetSubjectX500Principal((IX500Principal**)&principal);
    AutoPtr<IDistinguishedNameParser> dnParser;
    CDistinguishedNameParser::New(principal, (IDistinguishedNameParser**)&dnParser);
    AutoPtr<IList> cnList;
    dnParser->GetAllMostSpecificFirst(String("cn"), (IList**)&cnList);

    Boolean isEmpty;
    if(cnList->IsEmpty(&isEmpty), !isEmpty) {
        AutoPtr<ICollection> col = ICollection::Probe(cnList);
        Int32 size;
        col->GetSize(&size);
        AutoPtr< ArrayOf<String> > cns = ArrayOf<String>::Alloc(size);
        AutoPtr< ArrayOf<IInterface*> > array;
        col->ToArray((ArrayOf<IInterface*>**)&array);
        for (Int32 i = 0; i < array->GetLength(); ++i) {
            AutoPtr<ICharSequence> cs = ICharSequence::Probe((*array)[i]);
            String str;
            cs->ToString(&str);
            (*cns)[i] = str;
        }
        return cns;
    }
    else {
        return NULL;
    }
}

AutoPtr< ArrayOf<String> > AbstractVerifier::GetDNSSubjectAlts(
    /* [in] */ IX509Certificate* cert)
{
    AutoPtr<ILinkedList> subjectAltList;
    CLinkedList::New((ILinkedList**)&subjectAltList);
    AutoPtr<ICollection> subjectAltCol = ICollection::Probe(subjectAltList);
    AutoPtr<ICollection> c;
    // try {
    if (FAILED(cert->GetSubjectAlternativeNames((ICollection**)&c))) {
        Logger::E("AbstractVerifier", "Error parsing certificate.");
    }
    // }
    // catch(CertificateParsingException cpe) {
    //     Logger.getLogger(AbstractVerifier.class.getName())
    //             .log(Level.FINE, "Error parsing certificate.", cpe);
    // }
    if(c != NULL) {
        AutoPtr<IIterable> iterable = IIterable::Probe(c);
        AutoPtr<IIterator> it;
        iterable->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IList> aC = IList::Probe(obj);
            AutoPtr<IInterface> value;
            aC->Get(0, (IInterface**)&value);
            Int32 type;
            IInteger32::Probe(value)->GetValue(&type);
            // If type is 2, then we've got a dNSName
            if (type == 2) {
                AutoPtr<IInterface> cs;
                aC->Get(1, (IInterface**)&cs);
                subjectAltCol->Add(cs);
            }
        }
    }
    Boolean isEmpty;
    if(subjectAltList->IsEmpty(&isEmpty), !isEmpty) {
        Int32 size;
        subjectAltCol->GetSize(&size);
        AutoPtr< ArrayOf<String> > subjectAlts = ArrayOf<String>::Alloc(size);
        AutoPtr< ArrayOf<IInterface*> > array;
        subjectAltCol->ToArray((ArrayOf<IInterface*>**)&array);
        for (Int32 i = 0; i < array->GetLength(); ++i) {
            AutoPtr<ICharSequence> cs = ICharSequence::Probe((*array)[i]);
            String str;
            cs->ToString(&str);
            (*subjectAlts)[i] = str;
        }
        return subjectAlts;
    }
    else {
        return NULL;
    }
}

Int32 AbstractVerifier::CountDots(
    /* [in] */ const String& s)
{
    Int32 count = 0;
    for (Int32 i = 0; i < s.GetLength(); i++) {
        if(s.GetChar(i) == '.') {
            count++;
        }
    }
    return count;
}

} // namespace SSL
} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org