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

#include "security/cert/CertPath.h"
#include "security/cert/CCertificateFactoryHelper.h"
#include "core/StringBuilder.h"
#include "CByteArrayInputStream.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::StringBuilder;
using Elastos::IO::CByteArrayInputStream;
using Elastos::IO::IInputStream;
using Elastos::Security::Cert::CCertificateFactoryHelper;
using Elastos::Security::Cert::ICertificate;
using Elastos::Security::Cert::ICertPath;
using Elastos::Security::Cert::ICertificateFactory;
using Elastos::Security::Cert::ICertificateFactoryHelper;
using Elastos::Utility::IIterator;
using Elastos::Utility::IListIterator;
using Elastos::IO::EIID_ISerializable;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Security {
namespace Cert {

//{1024033A-F65A-26E1-C4C9-6005CBA3600A}
extern "C" const InterfaceID EIID_CertPath =
    { 0x1024033A, 0xF65A, 0x26E1, { 0xC4, 0xC9, 0x60, 0x05, 0xCB, 0xA3, 0x60, 0x0A } };
static const Int64 sSerialVersionUID = 6068470306649138683L;

CAR_INTERFACE_IMPL_2(CertPath, Object, ICertPath, ISerializable)
CertPath::CertPath(
    /* [in] */ const String& type)
    : mType(type)
{}

ECode CertPath::GetType(
    /* [out] */ String* type)
{
    VALIDATE_NOT_NULL(type)
    *type = mType;
    return NOERROR;
}

ECode CertPath::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* isEqual)
{
    VALIDATE_NOT_NULL(isEqual)
    if (this == reinterpret_cast<CertPath*>(other->Probe(EIID_CertPath))) {
        *isEqual = TRUE;
        return NOERROR;
    }

    if (ICertPath::Probe(other)) {
        AutoPtr<ICertPath> o = ICertPath::Probe(other);
        String otherType;
        o->GetType(&otherType);
        if (mType.Equals(otherType)) {
            AutoPtr<IList> certs, oCerts;
            GetCertificates((IList**)&certs);
            o->GetCertificates((IList**)&oCerts);
            if (certs == oCerts) {
                *isEqual = TRUE;
                return NOERROR;
            }
            else {
                certs->Equals(oCerts.Get(), isEqual);
                if (*isEqual == TRUE) {
                    return NOERROR;
                }
            }
        }
    }
    *isEqual = FALSE;
    return NOERROR;
}

ECode CertPath::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    Int32 hash = mType.GetHashCode();
    AutoPtr<IList> certs;
    GetCertificates((IList**)&certs);
    Int32 tmp;
    certs->GetHashCode(&tmp);
    hash = hash * 31 + tmp;
    *hashCode = hash;
    return NOERROR;
}

ECode CertPath::ToString(
    /* [out] */ String* str)
{
    StringBuilder sb(mType);
    sb.Append(" Cert Path, len=");
    AutoPtr<IList> certs;
    GetCertificates((IList**)&certs);
    Int32 size;
    certs->GetSize(&size);
    sb.Append(size);
    sb.Append(": [\n");
    Int32 n = 1;
    AutoPtr<IListIterator> listIterator;
    certs->GetListIterator((IListIterator**)&listIterator);
    Boolean hasNext;
    while((IIterator::Probe(listIterator)->HasNext(&hasNext), hasNext)) {
        sb.Append("---------------certificate ");
        sb.Append(n);
        sb.Append("---------------\n");
        AutoPtr<IInterface> elem;
        IIterator::Probe(listIterator)->GetNext((IInterface**)&elem);
        String tmp = Object::ToString(elem);
        sb.Append(tmp);
        n++;
    }
    sb.Append("\n]");
    sb.ToString(str);
    return NOERROR;
}

ECode CertPath::WriteReplace(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    AutoPtr<CertPathRep> ret;
    AutoPtr<ArrayOf<Byte> > encode;
    ECode ec = GetEncoded((ArrayOf<Byte>**)&encode);
    if (ec == (ECode)E_CERTIFICATE_ENCODING_EXCEPTION) {
        return E_NOT_SERIALIZABLE_EXCEPTION;
    }
    ret = new CertPathRep(mType, encode);
    *object = (ISerializable*)ret.Get();
    REFCOUNT_ADD(*object)
    return NOERROR;
}

const AutoPtr<ArrayOf<IObjectStreamField*> > CertPath::CertPathRep::sSerialPersistentFields = CertPathRep::InitFields();
CAR_INTERFACE_IMPL(CertPath::CertPathRep, Object, ISerializable)

CertPath::CertPathRep::CertPathRep(
    /* [in] */ const String& type,
    /* [in] */ArrayOf<Byte>* data)
    : mType(type)
    , mData(data)
{}

ECode CertPath::CertPathRep::ReadResolve(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    AutoPtr<ICertificateFactoryHelper> helper;
    AutoPtr<ICertificateFactory> cf;
    CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&helper);
    helper->GetInstance(mType, (ICertificateFactory**)&cf);
    AutoPtr<IInputStream> is;
    CByteArrayInputStream::New(mData, (IInputStream**)&is);
    AutoPtr<ICertPath> cp;
    FAIL_RETURN(cf->GenerateCertPath(is, (ICertPath**)&cp))
    *object = cp.Get();
    REFCOUNT_ADD(*object)
    return NOERROR;
}

AutoPtr<ArrayOf<IObjectStreamField*> > CertPath::CertPathRep::InitFields()
{
    AutoPtr<ArrayOf<IObjectStreamField*> > objs = ArrayOf<IObjectStreamField*>::Alloc(2);
    AutoPtr<IObjectStreamField> field;
    Slogger::D("CertPath", "[TODO] Need CObjectStreamField");
    // CObjectStreamField::New(String("type"), /*String.class*/ECLSID_CString, (IObjectStreamField**)&field);
    objs->Set(0, field);
    field = NULL;
    // CObjectStreamField::New(String("data"), /*byte[].class*/ECLSID_CArrayOf, TRUE, (IObjectStreamField**)&field);
    objs->Set(1, field);
    return objs;
}

} // end Cert
} // end Security
} // end Elastos
