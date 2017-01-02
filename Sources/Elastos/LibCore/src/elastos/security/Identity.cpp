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

#include "Identity.h"
#include "CVector.h"
#include "Objects.h"
#include "Arrays.h"
#include "CoreUtils.h"

using Elastos::Core::CoreUtils;
using Elastos::IO::EIID_ISerializable;
using Elastos::Utility::Arrays;
using Elastos::Utility::CVector;
using Elastos::Utility::Objects;

namespace Elastos {
namespace Security {

CAR_INTERFACE_IMPL_3(Identity, Object, IIdentity, IPrincipal, ISerializable)
Identity::~Identity()
{}

Identity::Identity()
    : mInfo("no additional info")
{}

ECode Identity::constructor(
    /* [in] */ const String& name)
{
    mName = name;
    return NOERROR;
}

ECode Identity::constructor(
    /* [in] */ const String& name,
    /* [in] */ IIdentityScope* scope) /*throws KeyManagementException*/
{
    constructor(name);
    if (scope != NULL) {
        FAIL_RETURN(scope->AddIdentity(this));
        mScope = scope;
    }
    return NOERROR;
}

ECode Identity::AddCertificate(
    /* [in] */ ICertificate* certificate) /*throws KeyManagementException*/
{
    AutoPtr<IPublicKey> certPK;
    certificate->GetPublicKey((IPublicKey**)&certPK);
    if (mPublicKey != NULL) {
        if (!CheckKeysEqual(mPublicKey, certPK)) {
            // throw new KeyManagementException("Cert's public key does not match Identity's public key");
            return E_KEY_MANAGEMENT_EXCEPTION;
        }
    }
    else {
        mPublicKey = certPK;
    }
    if (mCertificates == NULL) {
        CVector::New((IVector**)&mCertificates);
    }
    mCertificates->Add(certificate);
    return NOERROR;
}

Boolean Identity::CheckKeysEqual(
    /* [in] */ IPublicKey* pk1,
    /* [in] */ IPublicKey* pk2)
{
    // first, they should have the same format
    // second, their encoded form must be the same

    // assert(pk1 != NULL);
    // assert(pk2 != NULL);

    String format1;
    IKey::Probe(pk1)->GetFormat(&format1);
    String format2;
    if ((pk2 == NULL)
            || (((IKey::Probe(pk2)->GetFormat(&format2), format2) != NULL) ^ (format1 != NULL))
            || ((format1 != NULL) && !format1.Equals(format2))) {
        return FALSE;
    }

    AutoPtr<ArrayOf<Byte> > ks1;
    IKey::Probe(pk1)->GetEncoded((ArrayOf<Byte>**)&ks1);
    AutoPtr<ArrayOf<Byte> > ks2;
    IKey::Probe(pk2)->GetEncoded((ArrayOf<Byte>**)&ks2);
    return Arrays::Equals(ks1, ks2);
}

ECode Identity::RemoveCertificate(
    /* [in] */ ICertificate* certificate) /*throws KeyManagementException*/
{
    if (mCertificates != NULL) {
        Boolean flag = FALSE;
        if (mCertificates->Contains(certificate, &flag), !flag) {
            // throw new KeyManagementException("Certificate not found");
            return E_KEY_MANAGEMENT_EXCEPTION;
        }
        mCertificates->RemoveElement(certificate);
    }
    return NOERROR;
}

ECode Identity::Certificates(
    /* [out] */ ArrayOf<ICertificate*>** result)
{
    VALIDATE_NOT_NULL(result);
    if (mCertificates == NULL) {
        *result = ArrayOf<ICertificate*>::Alloc(0);
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    Int32 size = 0;
    mCertificates->GetSize(&size);
    AutoPtr<ArrayOf<IInterface*> > _ret = ArrayOf<IInterface*>::Alloc(size);
    mCertificates->CopyInto(_ret);
    AutoPtr<ArrayOf<ICertificate*> > ret = ArrayOf<ICertificate*>::Alloc(size);
    for (Int32 i = 0; i < size; i++) {
        ret->Set(i, ICertificate::Probe((*_ret)[i]));
    }
    *result = ret;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

Boolean Identity::IdentityEquals(
    /* [in] */ Identity* identity)
{
    if (!mName.Equals(identity->mName)) {
        return FALSE;
    }

    if (mPublicKey == NULL) {
        return (identity->mPublicKey == NULL);
    }

    return CheckKeysEqual(mPublicKey, identity->mPublicKey);
}

ECode Identity::ToString(
    /* [in] */ Boolean detailed,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    String s;
    ToString(&s);
    if (detailed) {
        *result = s + " " + mInfo;
        return NOERROR;
    }
    *result = s;
    return NOERROR;
}

ECode Identity::GetScope(
    /* [out] */ IIdentityScope** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mScope;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode Identity::SetPublicKey(
    /* [in] */ IPublicKey* key) /*throws KeyManagementException*/
{
    // this check does not always work
    if ((mScope != NULL) && (key != NULL)) {
        AutoPtr<IIdentity> i;
        mScope->GetIdentity(key, (IIdentity**)&i);
        //System.out.println("###DEBUG## Identity: "+i);
        if ((i != NULL) && (i != this)) {
            // throw new KeyManagementException("key already used in scope");
            return E_KEY_MANAGEMENT_EXCEPTION;
        }
    }
    mPublicKey = key;
    mCertificates = NULL;
    return NOERROR;
}

ECode Identity::GetPublicKey(
    /* [out] */ IPublicKey** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPublicKey;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode Identity::SetInfo(
    /* [in] */ const String& info)
{
    mInfo = info;
    return NOERROR;
}

ECode Identity::GetInfo(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mInfo;
    return NOERROR;
}

ECode Identity::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (this->Probe(EIID_IInterface) == obj) {
        *result = TRUE;
        return NOERROR;
    }
    if (IIdentity::Probe(obj) == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    Identity* i = (Identity*)IIdentity::Probe(obj);
    if (Objects::Equals(CoreUtils::Convert(mName), CoreUtils::Convert(i->mName))
            && (Objects::Equals(mScope, i->mScope))) {
        *result = TRUE;
        return NOERROR;
    }
    *result = IdentityEquals(i);
    return NOERROR;
}

ECode Identity::GetName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mName;
    return NOERROR;
}

ECode Identity::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 hash = 0;
    if (mName != NULL) {
        hash += mName.GetHashCode();
    }
    if (mScope != NULL) {
        Int32 hc = 0;
        IObject::Probe(mScope)->GetHashCode(&hc);
        hash += hc;
    }
    *result = hash;
    return NOERROR;
}

ECode Identity::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    String s = (mName == NULL ? String("") : mName);
    if (mScope != NULL) {
        String n;
        IPrincipal::Probe(mScope)->GetName(&n);
        *result = s + " [" + n + "]";
        return NOERROR;
    }
    *result = s;
    return NOERROR;
}

} // namespace Security
} // namespace Elastos
