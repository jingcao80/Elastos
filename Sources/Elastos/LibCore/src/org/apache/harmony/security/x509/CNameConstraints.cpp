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

#include "org/apache/harmony/security/x509/CNameConstraints.h"
#include "org/apache/harmony/security/x509/CGeneralSubtrees.h"
#include "org/apache/harmony/security/x509/CGeneralNames.h"
#include "org/apache/harmony/security/x509/CGeneralName.h"
#include "org/apache/harmony/security/asn1/ASN1Type.h"
#include "org/apache/harmony/security/asn1/CASN1Implicit.h"
#include "org/apache/harmony/security/asn1/CASN1OctetString.h"
#include "Elastos.CoreLibrary.Security.h"
#include "Elastos.CoreLibrary.Extensions.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/CArrayList.h>
#include "core/CArrayOf.h"
#include "core/CByte.h"

using Org::Apache::Harmony::Security::Asn1::ASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1Implicit;
using Org::Apache::Harmony::Security::Asn1::CASN1Implicit;
using Org::Apache::Harmony::Security::Asn1::IASN1OctetString;
using Org::Apache::Harmony::Security::Asn1::CASN1OctetString;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Core::IByte;
using Elastos::Core::CByte;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Security::Cert::IX509Extension;
using Elastos::Utility::CArrayList;
using Elastosx::Security::Auth::X500::IX500Principal;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

ECode CNameConstraints::MyASN1Sequence::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IInterface> con;
    bis->GetContent((IInterface**)&con);
    AutoPtr<IArrayOf> values = IArrayOf::Probe(con);

    AutoPtr<IInterface> obj0;
    values->Get(0, (IInterface**)&obj0);

    AutoPtr<IInterface> obj1;
    values->Get(1, (IInterface**)&obj1);

    AutoPtr<ArrayOf<Byte> > array;
    bis->GetEncoded((ArrayOf<Byte>**)&array);

    AutoPtr<INameConstraints> tmp;
    CNameConstraints::New(
            IGeneralSubtrees::Probe(obj0),
            IGeneralSubtrees::Probe(obj1),
            array, (INameConstraints**)&tmp);
    *object = TO_IINTERFACE(tmp);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CNameConstraints::MyASN1Sequence::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    CNameConstraints* nc = (CNameConstraints*)INameConstraints::Probe(object);
    values->Set(0, TO_IINTERFACE(nc->mPermittedSubtrees));
    values->Set(1, TO_IINTERFACE(nc->mExcludedSubtrees));
    return NOERROR;
}

AutoPtr<IASN1Sequence> CNameConstraints::initASN1()
{
    AutoPtr<IASN1Implicit> implicit1;
    CASN1Implicit::New(0, CGeneralSubtrees::ASN1, (IASN1Implicit**)&implicit1);

    AutoPtr<IASN1Implicit> implicit2;
    CASN1Implicit::New(1, CGeneralSubtrees::ASN1, (IASN1Implicit**)&implicit2);

    AutoPtr<ArrayOf<IASN1Type*> > array = ArrayOf<IASN1Type*>::Alloc(2);
    array->Set(0, IASN1Type::Probe(implicit1));
    array->Set(1, IASN1Type::Probe(implicit2));

    AutoPtr<ASN1Sequence> tmp = new MyASN1Sequence();
    tmp->constructor(array);
    tmp->SetOptional(0);
    tmp->SetOptional(1);
    return IASN1Sequence::Probe(tmp);
}

AutoPtr<IASN1Sequence> CNameConstraints::ASN1 = initASN1();

CAR_OBJECT_IMPL(CNameConstraints)

CAR_INTERFACE_IMPL(CNameConstraints, ExtensionValue, INameConstraints)

ECode CNameConstraints::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncode)
{
    VALIDATE_NOT_NULL(ppEncode);

    if (mEncoding == NULL) {
        IASN1Type::Probe(ASN1)->Encode(TO_IINTERFACE(this), (ArrayOf<Byte>**)&mEncoding);
    }
    *ppEncode = mEncoding;
    REFCOUNT_ADD(*ppEncode);
    return NOERROR;
}

ECode CNameConstraints::PrepareNames()
{
    // array of lists with permitted General Names divided by type
    CArrayList::New(9, (IArrayList**)&mPermitted_names);
    if (mPermittedSubtrees != NULL) {
        AutoPtr<IList> subtrees;
        mPermittedSubtrees->GetSubtrees((IList**)&subtrees);
        Int32 size;
        subtrees->GetSize(&size);
        for (Int32 i = 0; i < size; i ++) {
            AutoPtr<IInterface> obj;
            subtrees->Get(i, (IInterface**)&obj);
            AutoPtr<IGeneralSubtree> generalSubtree = IGeneralSubtree::Probe(obj);

            AutoPtr<IGeneralName> name;
            generalSubtree->GetBase((IGeneralName**)&name);
            Int32 tag;
            name->GetTag(&tag);

            AutoPtr<IInterface> namesObj;
            mPermitted_names->Get(tag, (IInterface**)&namesObj);
            if (namesObj == NULL) {
                AutoPtr<IArrayList> list;
                CArrayList::New((IArrayList**)&list);
                mPermitted_names->Set(tag, TO_IINTERFACE(list));
            }
            mPermitted_names->Add(tag, TO_IINTERFACE(name));
        }
    }
    // array of lists with excluded General Names divided by type
    CArrayList::New(9, (IArrayList**)&mExcluded_names);
    if (mExcludedSubtrees != NULL) {
        AutoPtr<IList> subtrees;
        mExcludedSubtrees->GetSubtrees((IList**)&subtrees);
        Int32 size;
        subtrees->GetSize(&size);

        for (Int32 i = 0; i < size; i ++) {
            AutoPtr<IInterface> obj;
            subtrees->Get(i, (IInterface**)&obj);
            AutoPtr<IGeneralSubtree> generalSubtree = IGeneralSubtree::Probe(obj);

            AutoPtr<IGeneralName> name;
            generalSubtree->GetBase((IGeneralName**)&name);
            Int32 tag;
            name->GetTag(&tag);

            AutoPtr<IInterface> namesObj;
            mExcluded_names->Get(tag, (IInterface**)&namesObj);
            if (namesObj == NULL) {
                AutoPtr<IArrayList> list;
                CArrayList::New((IArrayList**)&list);
                mExcluded_names->Set(tag, TO_IINTERFACE(list));
            }
            mExcluded_names->Add(tag, TO_IINTERFACE(name));
        }
    }
    return NOERROR;
}

ECode CNameConstraints::GetExtensionValue(
    /* [in] */ IX509Certificate* cert,
    /* [in] */ const String& OID,
    /* [out] */ ArrayOf<Byte>** values)
{
    VALIDATE_NOT_NULL(values);
    *values = NULL;

    ECode ec = NOERROR;
    //try
    {
        AutoPtr<ArrayOf<Byte> > bytes;
        FAIL_GOTO(Elastos::Security::Cert::IX509Extension::Probe(cert)->GetExtensionValue(OID,
                (ArrayOf<Byte>**)&bytes), ERROR)
        if (bytes == NULL) {
            *values = NULL;
            return NOERROR;
        }
        AutoPtr<IASN1OctetString> str;
        FAIL_GOTO(CASN1OctetString::GetInstance((IASN1OctetString**)&str), ERROR)
        AutoPtr<IInterface> obj;
        FAIL_GOTO(IASN1Type::Probe(str)->Decode(bytes, (IInterface**)&obj), ERROR)
        AutoPtr<IArrayOf> arrayof = IArrayOf::Probe(obj);

        Int32 length;
        arrayof->GetLength(&length);
        AutoPtr<ArrayOf<Byte> > array = ArrayOf<Byte>::Alloc(length);
        for (Int32 i = 0; i < length; i++) {
            AutoPtr<IInterface> value;
            arrayof->Get(i, (IInterface**)&value);
            AutoPtr<IByte> b = IByte::Probe(value);
            Byte num;
            b->GetValue(&num);
            (*array)[i] = num;
        }
        *values = array;
        REFCOUNT_ADD(*values);
        return NOERROR;
    } //catch (IOException e) {
ERROR:
    if (ec == (ECode)E_IO_EXCEPTION) {
        *values = NULL;
        return NOERROR;
    }

    return NOERROR;
}

ECode CNameConstraints::DumpValue(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ const String& prefix)
{
    sb->Append(prefix);
    sb->Append(String("Name Constraints: [\n"));
    if (mPermittedSubtrees != NULL) {
        sb->Append(prefix);
        sb->Append(String("  Permitted: [\n"));

        AutoPtr<IList> subtrees;
        mPermittedSubtrees->GetSubtrees((IList**)&subtrees);
        Int32 size;
        subtrees->GetSize(&size);
        for (Int32 i = 0; i < size; i ++) {
            AutoPtr<IInterface> obj;
            subtrees->Get(i, (IInterface**)&obj);
            AutoPtr<IGeneralSubtree> generalSubtree = IGeneralSubtree::Probe(obj);

            generalSubtree->DumpValue(sb, prefix + String("    "));
        }
        sb->Append(prefix);
        sb->Append(String("  ]\n"));
    }
    if (mExcludedSubtrees != NULL) {
        sb->Append(prefix);
        sb->Append(String("  Excluded: [\n"));

        AutoPtr<IList> subtrees;
        mExcludedSubtrees->GetSubtrees((IList**)&subtrees);
        Int32 size;
        subtrees->GetSize(&size);
        for (Int32 i = 0; i < size; i ++) {
            AutoPtr<IInterface> obj;
            subtrees->Get(i, (IInterface**)&obj);
            AutoPtr<IGeneralSubtree> generalSubtree = IGeneralSubtree::Probe(obj);

            generalSubtree->DumpValue(sb, prefix + String("    "));
        }
        sb->Append(prefix);
        sb->Append(String("  ]\n"));
    }
    sb->Append('\n');
    sb->Append(prefix);
    sb->Append(String("]\n"));
    return NOERROR;
}

ECode CNameConstraints::IsAcceptable(
    /* [in] */ IX509Certificate* cert,
    /* [out] */ Boolean* pIsAcceptable)
{
    VALIDATE_NOT_NULL(pIsAcceptable);

    if (mPermitted_names == NULL) {
        PrepareNames();
    }

    AutoPtr<ArrayOf<Byte> > bytes;
    GetExtensionValue(cert, String("2.5.29.17"), (ArrayOf<Byte>**)&bytes);
    AutoPtr<IList> names;

    ECode ec = NOERROR;
    //try
    {
        if (bytes == NULL) {
            CArrayList::New(1, (IList**)&names); // will check the subject field only
        }
        else {
            AutoPtr<IInterface> obj;
            FAIL_GOTO(ec = IASN1Type::Probe(CGeneralNames::ASN1)->Decode(bytes, (IInterface**)&obj), ERROR);
            FAIL_GOTO(ec = IGeneralNames::Probe(obj)->GetNames((IList**)&names), ERROR);
        }
    }// catch (IOException e) {
ERROR:
    if (ec == (ECode)E_IO_EXCEPTION) {
        // the certificate is broken;
        //e.printStackTrace();
        *pIsAcceptable = FALSE;
        return NOERROR;
    }

    AutoPtr<IInterface> obj1;
    mExcluded_names->Get(4, (IInterface**)&obj1);
    AutoPtr<IInterface> obj2;
    mPermitted_names->Get(4, (IInterface**)&obj2);
    if ((obj1 != NULL) || (obj2 != NULL)) {
        //try {
        AutoPtr<IX500Principal> subject;
        cert->GetSubjectX500Principal((IX500Principal**)&subject);
        String name;
        assert(0);
        //subject->GetName(&name);
        AutoPtr<IGeneralName> gname;
        CGeneralName::New(4, name, (IGeneralName**)&gname);
        names->Add(TO_IINTERFACE(gname));
        //} catch (IOException e) {
            // should never be happened
        //}
    }
    return IsAcceptable(names, pIsAcceptable);
}

ECode CNameConstraints::IsAcceptable(
    /* [in] */ IList* names,
    /* [out] */ Boolean* pIsAcceptable)
{
    VALIDATE_NOT_NULL(pIsAcceptable);

    if (mPermitted_names == NULL) {
        PrepareNames();
    }

    // check map: shows which types of permitted alternative names are
    // presented in the certificate
    AutoPtr<ArrayOf<Byte> > types_presented = ArrayOf<Byte>::Alloc(9);
    // check map: shows if permitted name of presented type is found
    // among the certificate's alternative names
    AutoPtr<ArrayOf<Byte> > permitted_found = ArrayOf<Byte>::Alloc(9);

    Int32 size;
    names->GetSize(&size);
    for (Int32 j = 0; j < size; j++) {
        AutoPtr<IInterface> obj;
        names->Get(j, (IInterface**)&obj);
        AutoPtr<IGeneralName> name = IGeneralName::Probe(obj);

        Int32 type;
        name->GetTag(&type);

        // search the name in excluded names
        AutoPtr<IInterface> ennames;
        mExcluded_names->Get(type, (IInterface**)&ennames);
        if (ennames != NULL) {
            Int32 _size;
            IList::Probe(ennames)->GetSize(&_size);
            for (Int32 i = 0; i < _size; i++) {
                AutoPtr<IInterface> _namesObj;
                IList::Probe(ennames)->Get(i, (IInterface**)&_namesObj);
                Boolean res;
                if (IGeneralName::Probe(_namesObj)->IsAcceptable(name, &res), res) {
                    *pIsAcceptable = FALSE;
                    return NOERROR;
                }
            }
        }

        // Search the name in permitted names
        // (if we already found the name of such type between the alt
        // names - we do not need to check others)
        AutoPtr<IInterface> pnnames;
        mPermitted_names->Get(type, (IInterface**)&pnnames);
        if ((pnnames != NULL) && (!(*permitted_found)[type])) {
            (*types_presented)[type] = TRUE;
            Int32 _size;
            IList::Probe(pnnames)->GetSize(&_size);
            for (Int32 i = 0; i < _size; i++) {
                AutoPtr<IInterface> _namesObj;
                IList::Probe(pnnames)->Get(i, (IInterface**)&_namesObj);
                Boolean res;
                if (IGeneralName::Probe(_namesObj)->IsAcceptable(name, &res), res) {
                    // found one permitted name of such type
                    (*permitted_found)[type] = TRUE;
                }
            }
        }
    }

    for (Int32 type = 0; type < 9; type++) {
        if ((*types_presented)[type] && !(*permitted_found)[type]) {
            *pIsAcceptable = FALSE;
            return NOERROR;
        }
    }
    *pIsAcceptable = TRUE;
    return NOERROR;
}

ECode CNameConstraints::constructor(
    /* [in] */ IGeneralSubtrees* permittedSubtrees,
    /* [in] */ IGeneralSubtrees* excludedSubtrees)
{
    if (permittedSubtrees != NULL) {
        AutoPtr<IList> ps;
        permittedSubtrees->GetSubtrees((IList**)&ps);
        Boolean res;
        if (ps == NULL || (ps->IsEmpty(&res), res)) {
            //throw new IllegalArgumentException("permittedSubtrees are empty");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    if (excludedSubtrees != NULL) {
        AutoPtr<IList> es;
        excludedSubtrees->GetSubtrees((IList**)&es);
        Boolean res;
        if (es == NULL || (es->IsEmpty(&res), res)) {
            //throw new IllegalArgumentException("excludedSubtrees are empty");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    mPermittedSubtrees = permittedSubtrees;
    mExcludedSubtrees = excludedSubtrees;
    return NOERROR;
}

ECode CNameConstraints::constructor(
    /* [in] */ IGeneralSubtrees* permittedSubtrees,
    /* [in] */ IGeneralSubtrees* excludedSubtrees,
    /* [in] */ ArrayOf<Byte>* encoding)
{
    constructor(permittedSubtrees, excludedSubtrees);
    mEncoding = encoding;
    return NOERROR;
}

ECode CNameConstraints::Decode(
    /* [in] */ ArrayOf<Byte>* encoding,
    /* [out] */ INameConstraints** ppObject)
{
    VALIDATE_NOT_NULL(ppObject);

    AutoPtr<IInterface> obj;
    IASN1Type::Probe(ASN1)->Decode(encoding, (IInterface**)&obj);
    *ppObject = INameConstraints::Probe(obj);
    REFCOUNT_ADD(*ppObject);
    return NOERROR;
}

ECode CNameConstraints::GetASN1(
    /* [out] */ IASN1Sequence** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    *ppAsn1 = ASN1;
    REFCOUNT_ADD(*ppAsn1);
    return NOERROR;
}

ECode CNameConstraints::SetASN1(
    /* [in] */ IASN1Sequence* pAsn1)
{
    ASN1 = pAsn1;
    return NOERROR;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org