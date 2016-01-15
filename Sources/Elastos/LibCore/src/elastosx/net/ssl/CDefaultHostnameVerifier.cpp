
#include "Elastos.CoreLibrary.Security.h"
#include "CDefaultHostnameVerifier.h"
#include "CDistinguishedNameParser.h"
#include "InetAddress.h"
#include "CLocale.h"
#include "CArrayList.h"
#include "Collections.h"

using Elastos::Security::Cert::ICertificate;
using Elastos::Net::InetAddress;
using Elastos::Core::IInteger32;
using Elastos::Core::ICharSequence;
using Elastos::Utility::CLocale;
using Elastos::Utility::ICollection;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterable;
using Elastos::Utility::Collections;
using Elastosx::Security::Auth::X500::IX500Principal;
using Elastosx::Net::Ssl::CDistinguishedNameParser;

namespace Elastosx {
namespace Net {
namespace Ssl {

const Int32 CDefaultHostnameVerifier::ALT_DNS_NAME = 2;
const Int32 CDefaultHostnameVerifier::ALT_IPA_NAME = 7;

CAR_INTERFACE_IMPL_2(CDefaultHostnameVerifier, Object, IDefaultHostnameVerifier, IHostnameVerifier)

CAR_OBJECT_IMPL(CDefaultHostnameVerifier)

ECode CDefaultHostnameVerifier::Verify(
    /* [in] */ const String& host,
    /* [in] */ ISSLSession* session,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    //try {
        ECode ec = NOERROR;
        AutoPtr<ArrayOf<AutoPtr<ICertificate> > > certificates;
        ec = session->GetPeerCertificates((ArrayOf<ICertificate*>**)&certificates);
        if(!SUCCEEDED(ec)) {
            *result = FALSE;
            return ec;
        }
        else {
            assert(0 && "TODO");
            // TODO: return Verify(host, (IX509Certificate*)(*certificates)[0]->Probe(EIID_IX509Certificate), result);
        }
    //} catch (SSLException e) {
        //return false;
    //}
    return NOERROR;
}

ECode CDefaultHostnameVerifier::Verify(
    /* [in] */ const String& host,
    /* [in] */ IX509Certificate* certificate,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean _result;
    FAIL_RETURN(InetAddress::IsNumeric(host, &_result))
    if (_result) {
        FAIL_RETURN(VerifyIpAddress(host, certificate, result))
    }
    else {
        FAIL_RETURN(VerifyHostName(host, certificate, result))
    }
    return NOERROR;
}

ECode CDefaultHostnameVerifier::VerifyIpAddress(
    /* [in] */ const String& ipAddress,
    /* [in] */ IX509Certificate* certificate,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IList> listName;
    ECode ec = NOERROR;
    ec = GetSubjectAltNames(certificate, ALT_IPA_NAME, (IList**)&listName);
    if (SUCCEEDED(ec)) {
        AutoPtr<IIterator> it;
        IIterable::Probe(listName)->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            ICharSequence* hc = ICharSequence::Probe(obj);
            String altName;
            hc->ToString(&altName);
            if (ipAddress.EqualsIgnoreCase(altName)) {
                *result = TRUE;
                return ec;
            }
        }
    }
    *result = FALSE;
    return ec;
}

ECode CDefaultHostnameVerifier::VerifyHostName(
    /* [in] */ const String& hostName,
    /* [in] */ IX509Certificate* certificate,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    String _hostName;
    _hostName = hostName.ToLowerCase(/*CLocale::US*/);
    Boolean hasDns = FALSE;
    AutoPtr<IList> listName;
    ECode ec = NOERROR;
    ec = GetSubjectAltNames(certificate, ALT_DNS_NAME, (IList**)&listName);
    if (SUCCEEDED(ec)) {
        AutoPtr<IIterator> it;
        IIterable::Probe(listName)->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            ICharSequence* hc = ICharSequence::Probe(obj);
            String altName;
            hc->ToString(&altName);
            hasDns = TRUE;
            Boolean _result;
            FAIL_RETURN(VerifyHostName(_hostName, altName, &_result))
            if (_result) {
                *result = TRUE;
                return NOERROR;
            }
        }
    }

    if (!hasDns) {
        AutoPtr<IX500Principal> principal;
        FAIL_RETURN(certificate->GetSubjectX500Principal((IX500Principal**)&principal))
        // RFC 2818 advises using the most specific name for matching.
        String cn;
        AutoPtr<IDistinguishedNameParser> parser;
        CDistinguishedNameParser::New(principal, (IDistinguishedNameParser**)&parser);
        FAIL_RETURN(parser->FindMostSpecific(String("cn"), &cn))
        if (!cn.IsNull()) {
            return VerifyHostName(_hostName, cn, result);
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode CDefaultHostnameVerifier::GetSubjectAltNames(
    /* [in] */ IX509Certificate* certificate,
    /* [in] */ Int32 type,
    /* [out] */ IList** names)
{
    VALIDATE_NOT_NULL(names)

    AutoPtr<IList> result;
    CArrayList::New((IArrayList**)&result);
    //try {
        AutoPtr<ICollection> subjectAltNames;
        AutoPtr< ArrayOf<IInterface*> > outarr;
        FAIL_GOTO(certificate->GetSubjectAlternativeNames((ICollection**)&subjectAltNames), EXIT)
        if (subjectAltNames == NULL) {
            return Collections::GetEmptyList(names);
        }

        subjectAltNames->ToArray((ArrayOf<IInterface*>**)&outarr);
        for(Int32 i = 0; i < outarr->GetLength(); ++i) {
            AutoPtr<IList> entry = IList::Probe((*outarr)[i]);
            Int32 size;
            if (entry == NULL || (entry->GetSize(&size), size) < 2) {
                continue;
            }
            AutoPtr<IInterface> object;
            entry->Get(0, (IInterface**)&object);
            AutoPtr<IInteger32> altNameType = (IInteger32*)IInteger32::Probe(object);
            if (altNameType == NULL) {
                continue;
            }
            Int32 value;
            altNameType->GetValue(&value);
            if (value == type) {
                String altName;
                AutoPtr<IInterface> object;
                entry->Get(1, (IInterface**)&object);
                AutoPtr<ICharSequence> sequence = (ICharSequence*)ICharSequence::Probe(object);
                sequence->ToString(&altName);
                if (!altName.IsNull()) {
                    Boolean _result;
                    result->Add(sequence, &_result);
                }
            }
        }

        *names = result;
        REFCOUNT_ADD(*names);
        return NOERROR;
    //} catch (CertificateParsingException e) {
EXIT:
        FAIL_RETURN(Collections::GetEmptyList(names))
        return NOERROR;
    //}
}

ECode CDefaultHostnameVerifier::VerifyHostName(
    /* [in] */ const String& hostName,
    /* [in] */ const String& cn,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (hostName.IsNull() || hostName.IsEmpty() || cn.IsNull() || cn.IsEmpty()) {
        *result = FALSE;
        return NOERROR;
    }

    String _cn;
    _cn = cn.ToLowerCase(/*CLocale::US*/);

    if (!_cn.Contains("*")) {
        *result = hostName.Equals(_cn);
        return NOERROR;
    }

    if (_cn.StartWith("*.") && hostName.RegionMatches(0, _cn, 2, _cn.GetLength() - 2)) {
        *result = TRUE; // "*.foo.com" matches "foo.com"
        return NOERROR;
    }

    Int32 asterisk = _cn.IndexOf('*');
    Int32 dot = _cn.IndexOf('.');
    if (asterisk > dot) {
        *result = FALSE; // malformed; wildcard must be in the first part of the cn
        return NOERROR;
    }

    if (!hostName.RegionMatches(0, _cn, 0, asterisk)) {
        *result = FALSE; // prefix before '*' doesn't match
        return NOERROR;
    }

    Int32 suffixLength = _cn.GetLength() - (asterisk + 1);
    Int32 suffixStart = hostName.GetLength() - suffixLength;
    if (hostName.IndexOf('.', asterisk) < suffixStart) {
        // TODO: remove workaround for *.clients.google.com http://b/5426333
        if (!hostName.EndWith(".clients.google.com")) {
            *result = FALSE; // wildcard '*' can't match a '.'
            return NOERROR;
        }
    }

    if (!hostName.RegionMatches(suffixStart, _cn, asterisk + 1, suffixLength)) {
        *result = FALSE; // suffix after '*' doesn't match
        return NOERROR;
    }

    *result = TRUE;
    return NOERROR;
}

} // namespace Ssl
} // namespace Net
} // namespace Elastosx
