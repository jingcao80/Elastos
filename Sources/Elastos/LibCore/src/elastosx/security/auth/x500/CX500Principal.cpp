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

#include "Elastos.CoreLibrary.Utility.h"
#include "org/apache/harmony/security/x501/CName.h"
#include "CX500Principal.h"
#include "CString.h"
#include "StringBuilder.h"
#include "AutoLock.h"
#include "org/apache/harmony/security/x501/CName.h"

using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::X501::CName;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Security::EIID_IPrincipal;
using Elastos::Security::IPrincipal;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::X501::CName;

namespace Elastosx {
namespace Security {
namespace Auth {
namespace X500 {

CAR_OBJECT_IMPL(CX500Principal)
CAR_INTERFACE_IMPL_2(CX500Principal, Object, IX500Principal, IPrincipal)

const Int64 CX500Principal::mSerialVersionUID = -500463348111345721LL;

ECode CX500Principal::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** encodedName)
{
    VALIDATE_NOT_NULL(encodedName)
    *encodedName = NULL;

    AutoPtr<ArrayOf<Byte> > src;
    FAIL_RETURN(mDn->GetEncoded((ArrayOf<Byte>**)&src));
    AutoPtr<ArrayOf<Byte> > dst = ArrayOf<Byte>::Alloc(src->GetLength());
    dst->Copy(0, src, 0, dst->GetLength());
    *encodedName = dst;
    return NOERROR;
}

ECode CX500Principal::GetName(
    /* [in] */ const String& format,
    /* [out] */ String *name)
{
    VALIDATE_NOT_NULL(name)
    *name = String(NULL);

    if (CANONICAL.Equals(format)) {
        return GetCanonicalName(name);
    }
    return mDn->GetName(format, name);
}

ECode CX500Principal::GetName(
    /* [in] */ const String& format,
    /* [in] */ IMap *oidMap,
    /* [out] */ String * pName)
{
    VALIDATE_NOT_NULL(pName);
    *pName = String(NULL);
    VALIDATE_NOT_NULL(oidMap)

    String rfc1779Name;
    mDn->GetName(RFC1779, &rfc1779Name);
    String rfc2253Name;
    mDn->GetName(RFC2253, &rfc2253Name);

    if (format.EqualsIgnoreCase("RFC1779")) {
        AutoPtr<StringBuilder> resultName = new StringBuilder(rfc1779Name);
        Int32 fromIndex = resultName->GetLength();
        Int32 equalIndex = -1;
        while (-1 != (resultName->LastIndexOf(String("="), fromIndex, &equalIndex), equalIndex)) {
            Int32 commaIndex;
            resultName->LastIndexOf(String(","), equalIndex, &commaIndex);
            String subName = resultName->Substring(commaIndex + 1,
                    equalIndex).Trim();
            if (subName.GetLength() > 4
                    && subName.Substring(0, 4).Equals("OID.")) {
                String subSubName = subName.Substring(4);
                AutoPtr<ICharSequence> csq;
                CString::New(subSubName, (ICharSequence**)&csq);
                Boolean b;
                if (oidMap->ContainsKey(csq, &b), b) {
                    String replaceName;
                    AutoPtr<ICharSequence> pReplaceName;
                    AutoPtr<IInterface> tmp;
                    oidMap->Get(csq, (IInterface**)&tmp);
                    pReplaceName = ICharSequence::Probe(tmp);
                    pReplaceName->ToString(&replaceName);
                    if(commaIndex > 0) replaceName = String(" ") + replaceName;
                    resultName->Replace(commaIndex + 1, equalIndex, replaceName);
                }
            }
            fromIndex = commaIndex;
        }
        resultName->ToString(pName);
        return NOERROR;
    } else if (format.EqualsIgnoreCase("RFC2253")) {
        AutoPtr<StringBuilder> resultName = new StringBuilder(rfc2253Name);
        AutoPtr<StringBuilder> subsidyName = new StringBuilder(rfc1779Name);

        Int32 fromIndex = resultName->GetLength();
        Int32 subsidyFromIndex = subsidyName->GetLength();
        Int32 equalIndex = -1;
        Int32 subsidyEqualIndex = -1;
        while (-1 != (resultName->LastIndexOf(String("="), fromIndex, &equalIndex), equalIndex)) {
            subsidyName->LastIndexOf(String("="), subsidyFromIndex, &subsidyEqualIndex);
            Int32 commaIndex;
            resultName->LastIndexOf(String(","), equalIndex, &commaIndex);
            String subName = resultName->Substring(commaIndex + 1, equalIndex).Trim();
            AutoPtr<ICharSequence> csq;
            CString::New(subName, (ICharSequence**)&csq);
            Boolean b;
            if (oidMap->ContainsKey(csq, &b), b) {
                Int32 subOrignalEndIndex;
                resultName->IndexOf(String(","), equalIndex, &subOrignalEndIndex);
                if (-1 == subOrignalEndIndex)
                    subOrignalEndIndex = resultName->GetLength();
                Int32 subGoalEndIndex;
                subsidyName->IndexOf(String(","), subsidyEqualIndex, &subGoalEndIndex);
                if (-1 == subGoalEndIndex)
                    subGoalEndIndex = subsidyName->GetLength();
                resultName->Replace(equalIndex + 1, subOrignalEndIndex,
                        subsidyName->Substring(subsidyEqualIndex + 1,
                                subGoalEndIndex));
                AutoPtr<ICharSequence> pReplaceName;
                AutoPtr<IInterface> tmp;
                oidMap->Get(csq, (IInterface**)&tmp);
                pReplaceName = ICharSequence::Probe(tmp);
                String s;
                pReplaceName->ToString(&s);
                resultName->Replace(commaIndex + 1, equalIndex, s);
            }
            fromIndex = commaIndex;
            subsidyFromIndex = subsidyEqualIndex - 1;
        }
        resultName->ToString(pName);
        return NOERROR;
    } else {
        // throw new IllegalArgumentException("invalid format specified: " + format);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
}

ECode CX500Principal::Equals(
    /* [in] */ IInterface *o,
    /* [out] */ Boolean *result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    VALIDATE_NOT_NULL(o)

    if ((CX500Principal*)(IX500Principal*)o == this) {
        *result = TRUE;
        return NOERROR;
    }
    InterfaceID id1, id2;
    this->GetInterfaceID(TO_IINTERFACE(this), &id1);
    o->GetInterfaceID(o, &id2);
    if (o == NULL || id1 == id2) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IX500Principal> principal = IX500Principal::Probe(o);
    String name1, name2;
    this->GetCanonicalName(&name1);
    ((CX500Principal*)principal.Get())->GetCanonicalName(&name2);
    *result = name1.Equals(name2);
    return NOERROR;
}

ECode CX500Principal::GetName(
    /* [out] */ String * pName)
{
    VALIDATE_NOT_NULL(pName);
    *pName = String(NULL);

    return mDn->GetName(RFC2253, pName);
}

ECode CX500Principal::GetHashCode(
    /* [out] */ Int32 *hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    *hashCode = 0;

    String name;
    GetCanonicalName(&name);
    *hashCode = name.GetHashCode();
    return NOERROR;
}

ECode CX500Principal::ToString(
    /* [out] */ String * pStr)
{
    VALIDATE_NOT_NULL(pStr)
    *pStr = String(NULL);

    mDn->GetName(RFC1779, pStr);
    return NOERROR;
}

ECode CX500Principal::constructor(
    /* [in] */ ArrayOf<Byte> *name)
{
    VALIDATE_NOT_NULL(name)

    if (name == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // try {
        // FIXME dn = new Name(name);
    AutoPtr<IInterface> o;
    IASN1Type::Probe(CName::ASN1)->Decode(name, (IInterface**)&o);
    mDn = IName::Probe(o);
    return NOERROR;
    // } catch (IOException e) {
        // throw incorrectInputEncoding(e);
    // }
}

ECode CX500Principal::constructor(
    /* [in] */ IInputStream *in)
{
    VALIDATE_NOT_NULL(in)

    if (in == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    // try {
        // FIXME dn = new Name(is);
    AutoPtr<IInterface> o;
    return IASN1Type::Probe(CName::ASN1)->Decode(in, (IInterface**)&o);
    mDn = IName::Probe(o);
    return NOERROR;
    // } catch (IOException e) {
        // throw incorrectInputEncoding(e);
    // }
}

ECode CX500Principal::constructor(
    /* [in] */ const String& name)
{
    if (name.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }
    // try {
    return CName::New(name, (IName**)&mDn);
    // } catch (IOException e) {
        // throw incorrectInputName(e, name);
    // }
}

ECode CX500Principal::constructor(
    /* [in] */ const String& name,
    /* [in] */ IMap *keywordMap)
{
    VALIDATE_NOT_NULL(keywordMap)

    if (name.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }
    // try {
    String s;
    FAIL_RETURN(SubstituteNameFromMap(name, keywordMap, &s));
    return CName::New(s, (IName**)&mDn);
    // } catch (IOException e) {
        // throw incorrectInputName(e, name);
    // }
}

ECode CX500Principal::GetCanonicalName(
        /* [out] */ String *name)
{
    VALIDATE_NOT_NULL(name)
    *name = String(NULL);

    AutoLock lock(mLock);
    if (mCanonicalName.IsNull()) {
        mDn->GetName(CANONICAL, &mCanonicalName);
    }
    *name = mCanonicalName;
    return NOERROR;
}

ECode CX500Principal::SubstituteNameFromMap(
    /* [in] */ const String& name,
    /* [in] */ IMap *keywordMap,
    /* [out] */ String *nm)
{
    VALIDATE_NOT_NULL(nm)
    *nm = String(NULL);
    VALIDATE_NOT_NULL(keywordMap)

    AutoPtr<StringBuilder> sbName = new StringBuilder(name);
    Int32 fromIndex;
    sbName->GetLength(&fromIndex);
    Int32 equalIndex;
    String tmpName;
    Int32 tmpIndex;
    while (-1 != (equalIndex = (sbName->LastIndexOf(String("="), fromIndex, &tmpIndex), tmpIndex))) {
        Int32 commaIndex;
        sbName->LastIndexOf(String(","), equalIndex, &commaIndex);
        String subName;
        subName = (sbName->Substring(commaIndex + 1, equalIndex, &subName), subName).Trim();
        AutoPtr<ICharSequence> key;
        CString::New(subName, (ICharSequence**)&key);
        Boolean isContained;
        keywordMap->ContainsKey(key, &isContained);
        if (isContained) {
            AutoPtr<IInterface> elem;
            keywordMap->Get(key, (IInterface**)&elem);
            String elemString;
            ICharSequence::Probe(elem)->ToString(&elemString);
            sbName->Replace(commaIndex + 1, equalIndex, elemString);
        }
        fromIndex = commaIndex;
    }
    return sbName->ToString(nm);
}

} // namespace Callback
} // namespace Auth
} // namespace Security
} // namespace Elastosx

