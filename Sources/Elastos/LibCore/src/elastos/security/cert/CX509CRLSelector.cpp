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

#include "CX509CRLSelector.h"
#include "CArrayList.h"
#include "CX500Principal.h"
#include "CString.h"
#include "StringBuilder.h"
#include "CDate.h"
#include "X509CRL.h"

using Elastos::Core::EIID_ICloneable;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Core::CString;
using Elastos::Utility::IDate;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CDate;
using Elastosx::Security::Auth::X500::CX500Principal;

namespace Elastos {
namespace Security {
namespace Cert {

CAR_OBJECT_IMPL(CX509CRLSelector)
CAR_INTERFACE_IMPL_3(CX509CRLSelector, Object, IX509CRLSelector, ICRLSelector, ICloneable)
CX509CRLSelector::CX509CRLSelector()
    : mDateAndTime(-1)
{}

ECode CX509CRLSelector::SetIssuers(
    /* [in] */ ICollection *issuers)
{
    if (issuers == NULL) {
        mIssuerNames = NULL;
        mIssuerPrincipals = NULL;
        return NOERROR;
    }
    Int32 size;
    issuers->GetSize(&size);
    AutoPtr<IArrayList> al;
    CArrayList::New(size, (IArrayList**)&al);
    mIssuerNames = al;
    al = NULL;
    CArrayList::New(issuers, (IArrayList**)&al);
    mIssuerPrincipals = al;
    Boolean hasNext;
    AutoPtr<IIterator> it;
    issuers->GetIterator((IIterator**)&it);
    while ((it->HasNext(&hasNext), hasNext)) {
        AutoPtr<IInterface> issuer;
        it->GetNext((IInterface**)&issuer);
        String name;
        IX500Principal::Probe(issuer)->GetName(IX500Principal::CANONICAL, &name);
        AutoPtr<ICharSequence> cs;
        CString::New(name, (ICharSequence**)&cs);
        Boolean isModified;
        mIssuerNames->Add(cs.Get(), &isModified);
    }
    return NOERROR;
}

ECode CX509CRLSelector::SetIssuerNames(
    /* [in] */ ICollection *names)
{
    if (names == NULL) {
        mIssuerNames = NULL;
        mIssuerPrincipals = NULL;
        return NOERROR;
    }
    Int32 size;
    if ((names->GetSize(&size), size) == 0) {
        return NOERROR;
    }
    AutoPtr<IArrayList> al;
    CArrayList::New(size, (IArrayList**)&al);
    mIssuerNames = al;
    //Apache...Todo later
    /*
    for (Object name: names) {
        if (name instanceof String) {
            issuerNames.add(
                    new Name((String) name).getName(
                        X500Principal.CANONICAL));
        } else if (name instanceof byte[]) {
            issuerNames.add(
                    new Name((byte[]) name).getName(
                        X500Principal.CANONICAL));
        } else {
            throw new IOException("name neither a String nor a byte[]");
        }
    }
    */
    return E_NOT_IMPLEMENTED;
}

ECode CX509CRLSelector::AddIssuer(
    /* [in] */ IX500Principal *issuer)
{
    if (issuer == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    if (mIssuerNames == NULL) {
        CArrayList::New((IArrayList**)&mIssuerNames);
    }
    String name;
    issuer->GetName(IX500Principal::CANONICAL, &name);
    Boolean isContained;
    AutoPtr<ICharSequence> cs;
    CString::New(name, (ICharSequence**)&cs);
    if ((mIssuerNames->Contains(cs.Get(), &isContained), !isContained)) {
        mIssuerNames->Add(cs.Get(), &isContained);
    }
    Int32 size;
    mIssuerNames->GetSize(&size);
    if (mIssuerPrincipals == NULL) {
        CArrayList::New(size, (IArrayList**)&mIssuerPrincipals);
    }
    // extend the list of issuer Principals
    size -= 1;
    Int32 ipSize;
    Boolean isModified;
    for (Int32 i = (mIssuerPrincipals->GetSize(&ipSize), ipSize); i < size; i++) {
        AutoPtr<IInterface> elem;
        AutoPtr<IX500Principal> ixp;
        mIssuerNames->Get(i, (IInterface**)&elem);
        String name;
        ICharSequence::Probe(elem)->ToString(&name);
        CX500Principal::New(name, (IX500Principal**)&ixp);
        mIssuerPrincipals->Add(ixp.Get(), &isModified);
    }
    return mIssuerPrincipals->Add(issuer);
}

ECode CX509CRLSelector::AddIssuerName(
    /* [in] */ const String& _iss_name)
{
    String iss_name = _iss_name;
    if (mIssuerNames == NULL) {
        CArrayList::New((IArrayList**)&mIssuerNames);
    }

    if (iss_name.IsNull()) {
        iss_name = String("");
    }

    //Apache...Todo later
    /*
     String name = new Name(iss_name).getName(X500Principal.CANONICAL);
    if (!issuerNames.contains(name)) {
        issuerNames.add(name);
    }
    */
    return E_NOT_IMPLEMENTED;
}

ECode CX509CRLSelector::AddIssuerNameEx(
    /* [in] */ ArrayOf<Byte> *iss_name)
{
    if (iss_name == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    if (mIssuerNames == NULL) {
        CArrayList::New((IArrayList**)&mIssuerNames);
    }

    //Apache...Todo later
    /*
    String name = new Name(iss_name).getName(X500Principal.CANONICAL);
    if (!issuerNames.contains(name)) {
        issuerNames.add(name);
    }
    */
    return E_NOT_IMPLEMENTED;
}

ECode CX509CRLSelector::SetMinCRLNumber(
    /* [in] */ IBigInteger *minCRL)
{
    mMinCRL = minCRL;
    return NOERROR;
}

ECode CX509CRLSelector::SetMaxCRLNumber(
    /* [in] */ IBigInteger *maxCRL)
{
    mMaxCRL = maxCRL;
    return NOERROR;
}

ECode CX509CRLSelector::SetDateAndTime(
    /* [in] */ IDate *dateAndTime)
{
    if (dateAndTime == NULL) {
        mDateAndTime = -1;
        return NOERROR;
    }
    return dateAndTime->GetTime(&mDateAndTime);
}

ECode CX509CRLSelector::SetCertificateChecking(
    /* [in] */ IX509Certificate *cert)
{
    mCertificateChecking = cert;
    return NOERROR;
}

ECode CX509CRLSelector::GetIssuers(
    /* [out] */ ICollection **issuers)
{
    if (mIssuerNames == NULL) {
        return NOERROR;
    }
    Int32 issuerNamesSize;
    mIssuerNames->GetSize(&issuerNamesSize);
    if (mIssuerPrincipals == NULL) {
        CArrayList::New(issuerNamesSize, (IArrayList**)&mIssuerPrincipals);
    }
    Int32 size = issuerNamesSize;
    mIssuerPrincipals->GetSize(&issuerNamesSize);
    // extend the list of issuer Principals
    for (Int32 i = issuerNamesSize; i < size; i++) {
        AutoPtr<IInterface> elem;
        AutoPtr<IX500Principal> ixp;
        mIssuerNames->Get(i, (IInterface**)&elem);
        String name;
        ICharSequence::Probe(elem)->ToString(&name);
        CX500Principal::New(name, (IX500Principal**)&ixp);
        Boolean isModified;
        mIssuerPrincipals->Add(ixp.Get(), &isModified);
    }
    //Apache...Todo later
    //return Collections.unmodifiableCollection(issuerPrincipals);
    return E_NOT_IMPLEMENTED;
}

ECode CX509CRLSelector::GetIssuerNames(
    /* [out] */ ICollection **issuer)
{
    if (mIssuerNames == NULL) {
        return NOERROR;
    }
    //return mIssuerNames->Clone(issuer);
    return E_NOT_IMPLEMENTED;
}

ECode CX509CRLSelector::GetMinCRL(
    /* [out] */ IBigInteger **minCrl)
{
    VALIDATE_NOT_NULL(minCrl)
    *minCrl = mMinCRL;
    REFCOUNT_ADD(*minCrl)
    return NOERROR;
}

ECode CX509CRLSelector::GetMaxCRL(
    /* [out] */ IBigInteger **maxCrl)
{
    VALIDATE_NOT_NULL(maxCrl)
    *maxCrl = mMaxCRL;
    REFCOUNT_ADD(*maxCrl)
    return NOERROR;
}

ECode CX509CRLSelector::GetDateAndTime(
    /* [out] */ IDate **tm)
{
    if (mDateAndTime == -1) {
        return NOERROR;
    }
    return CDate::New(mDateAndTime, tm);
}

ECode CX509CRLSelector::GetCertificateChecking(
    /* [out] */ IX509Certificate **checking)
{
    VALIDATE_NOT_NULL(checking)
    *checking = mCertificateChecking;
    REFCOUNT_ADD(*checking)
    return NOERROR;
}

ECode CX509CRLSelector::ToString(
    /* [out] */ String *str)
{
    StringBuilder result;
    result.Append("X509CRLSelector:\n[");
    if (mIssuerNames) {
        result.Append("\n  IssuerNames:\n  [");
        Int32 size;
        mIssuerNames->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            result.Append("\n    ");
            AutoPtr<IInterface> elem;
            mIssuerNames->Get(i, (IInterface**)&elem);
            String name;
            ICharSequence::Probe(elem)->ToString(&name);
            result.Append(name);
        }
        result.Append("\n  ]");
    }
    if (mMinCRL != NULL) {
        result.Append("\n  minCRL: ");
        result.Append(mMinCRL.Get());
    }
    if (mMaxCRL != NULL) {
        result.Append("\n  mMaxCRL: ");
        result.Append(mMaxCRL.Get());
    }
    if (mDateAndTime != -1) {
        result.Append("\n  dateAndTime: ");
        AutoPtr<IDate> dt;
        CDate::New(mDateAndTime, (IDate**)&dt);
        result.Append(dt.Get());
    }
    if (mCertificateChecking != NULL) {
        result.Append("\n  certificateChecking: ");
        result.Append(mCertificateChecking.Get());
    }
    result.Append("\n]");
    return result.ToString(str);
}

ECode CX509CRLSelector::constructor()
{
    return NOERROR;
}

ECode CX509CRLSelector::Match(
    /* [in] */ ICRL *crl,
    /* [out] */ Boolean *result)
{
    VALIDATE_NOT_NULL(result)
    if (!IX509CRL::Probe(crl)) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IX509CRL> crlist = IX509CRL::Probe(crl);
    if (mIssuerNames != NULL) {
        // the search speed depends on the class of issuerNames
        AutoPtr<IX500Principal> principal;
        crlist->GetIssuerX500Principal((IX500Principal**)&principal);
        String name;
        principal->GetName(IX500Principal::CANONICAL, &name);
        Boolean isContained;
        AutoPtr<ICharSequence> cs;
        CString::New(name, (ICharSequence**)&cs);
        if ((mIssuerNames->Contains(cs.Get(), &isContained), !isContained)) {
            *result = FALSE;
            return NOERROR;
        }
    }
    if (mMinCRL || mMaxCRL) {
        // As specified in rfc 3280 (http://www.ietf.org/rfc/rfc3280.txt)
        // CRL Number Extension's OID is 2.5.29.20 .
        AutoPtr<ArrayOf<Byte> > bytes;
        IX509Extension::Probe(crlist)->GetExtensionValue(String("2.5.29.20"), (ArrayOf<Byte>**)&bytes);
        //Apache...Todo later
        /*bytes = (byte[]) ASN1OctetString.getInstance().decode(bytes);
        BigInteger crlNumber = new BigInteger((byte[])
                ASN1Integer.getInstance().decode(bytes));
        if ((minCRL != null) && (crlNumber.compareTo(minCRL) < 0)) {
            return false;
        }
        if ((maxCRL != null) && (crlNumber.compareTo(maxCRL) > 0)) {
            return false;
        }
        */
    }
    if (mDateAndTime != -1) {
        AutoPtr<IDate> thisUp, nextUp;
        crlist->GetThisUpdate((IDate**)&thisUp);
        crlist->GetNextUpdate((IDate**)&nextUp);
        if (thisUp == NULL || nextUp == NULL) {
            *result = FALSE;
            return NOERROR;
        }
        Int64 thisTime, nextTime;
        thisUp->GetTime(&thisTime);
        nextUp->GetTime(&nextTime);
        if (mDateAndTime < thisTime || mDateAndTime > nextTime) {
            *result = FALSE;
            return NOERROR;
        }
    }
    *result = TRUE;
    return NOERROR;
}

ECode CX509CRLSelector::Clone(
/* [out] */ IInterface **obj)
{
    VALIDATE_NOT_NULL(obj)
    AutoPtr<IX509CRLSelector> result;
    CX509CRLSelector::New((IX509CRLSelector**)&result);
    if (mIssuerNames != NULL) {
        CArrayList::New(ICollection::Probe(mIssuerNames),
            (IArrayList**)&(((CX509CRLSelector*)result.Get())->mIssuerNames));
    }
    *obj = result.Get();
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

}
}
}
