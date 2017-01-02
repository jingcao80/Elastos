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
#include "CJarEntry.h"
#include "JarVerifier.h"
#include "CJarFile.h"
#include "CLinkedList.h"
// #include "CCodeSigner.h"
// #include "CCertificateFactory.h"
// #include "CCertificateFactoryHelper.h"
// #include <cutils/log.h>


using Elastos::Utility::CLinkedList;
using Elastos::Utility::IList;
using Elastos::Security::IPrincipal;
//using Elastos::Security::CCodeSigner
using Elastos::Security::Cert::ICertificate;
using Elastos::Security::Cert::ICertPath;
using Elastos::Security::Cert::IX509Certificate;
using Elastos::Security::Cert::ICertificateFactoryHelper;;
//using Elastos::Security::Cert::CCertificateFactory;
//using Elastos::Security::Cert::CCertificateFactoryHelper;
using Elastosx::Security::Auth::X500::IX500Principal;

namespace Elastos {
namespace Utility {
namespace Jar {

CAR_INTERFACE_IMPL(CJarEntry, ZipEntry, IJarEntry)

CAR_OBJECT_IMPL(CJarEntry)

CJarEntry::CJarEntry()
    : mIsFactoryChecked(FALSE)
{}

ECode CJarEntry::constructor(
    /* [in] */ const String& name)
{
    return ZipEntry::constructor(name);
}

ECode CJarEntry::constructor(
    /* [in] */ IZipEntry* zipEntry)
{
    return constructor(zipEntry, NULL);
}

ECode CJarEntry::constructor(
    /* [in] */ IZipEntry* zipEntry,
    /* [in] */ IJarFile* parentJar)
{
    FAIL_RETURN(ZipEntry::constructor(zipEntry))
    mParentJar = parentJar;
    return NOERROR;
}

ECode CJarEntry::constructor(
    /* [in] */ IJarEntry* jarEntry)
{
    FAIL_RETURN(ZipEntry::constructor(IZipEntry::Probe(jarEntry)))

    mParentJar = ((CJarEntry*)jarEntry)->mParentJar;
    mAttributes = ((CJarEntry*)jarEntry)->mAttributes;
    mSigners = ((CJarEntry*)jarEntry)->mSigners;
    return NOERROR;
}

ECode CJarEntry::GetAttributes(
    /* [out] */ IAttributes** attrib)
{
    VALIDATE_NOT_NULL(attrib)
    if (mAttributes != NULL || mParentJar == NULL) {
        *attrib = mAttributes;
        REFCOUNT_ADD(*attrib)
        return NOERROR;
    }

    AutoPtr<IManifest> manifest;
    FAIL_RETURN(mParentJar->GetManifest((IManifest**)&manifest));
    if (!manifest) {
        *attrib = NULL;
        return NOERROR;
    }

    String name;
    GetName(&name);
    mAttributes = NULL;
    FAIL_RETURN(manifest->GetAttributes(name, (IAttributes**)&mAttributes));
    *attrib = mAttributes;
    REFCOUNT_ADD(*attrib)
    return NOERROR;
}

ECode CJarEntry::GetCertificates(
    /* [out, callee] */ ArrayOf<ICertificate*>** certificates)
{
    VALIDATE_NOT_NULL(certificates)
    *certificates = NULL;

    if (mParentJar == NULL) {
        return NOERROR;
    }
    AutoPtr<JarVerifier> jarVerifier = ((CJarFile*)mParentJar.Get())->mVerifier;
    if (jarVerifier == NULL) {
        return NOERROR;
    }

    String name;
    GetName(&name);
    AutoPtr<ArrayOf<AutoPtr<ArrayOf<ICertificate*> > > > certChains;
    certChains = jarVerifier->GetCertificateChains(name);
    if (certChains == NULL) {
        return NOERROR;
    }

    // Measure number of certs.
    Int32 count = 0;
    for (Int32 i = 0; i < certChains->GetLength(); ++i) {
        count += (*certChains)[i]->GetLength();
    }

    // Create new array and copy all the certs into it.
    AutoPtr<ArrayOf<ICertificate*> > certs = ArrayOf<ICertificate*>::Alloc(count);
    count = 0;
    for (Int32 i = 0; i < certChains->GetLength(); ++i) {
        certs->Copy(count, (*certChains)[i], 0, (*certChains)[i]->GetLength());
        count += (*certChains)[i]->GetLength();
    }

    *certificates = certs;
    REFCOUNT_ADD(*certificates)
    return NOERROR;
}

ECode CJarEntry::SetAttributes(
        /* [in] */ IAttributes* attrib)
{
    mAttributes = attrib;
    return NOERROR;
}

ECode CJarEntry::GetCodeSigners(
    /* [out, callee] */ ArrayOf<ICodeSigner*>** codeSigner)
{
    VALIDATE_NOT_NULL(codeSigner)
    *codeSigner = NULL;

    if (mParentJar == NULL) {
        return NOERROR;
    }

    AutoPtr<JarVerifier> jarVerifier = ((CJarFile*)mParentJar.Get())->mVerifier;
    if (jarVerifier == NULL) {
        return NOERROR;
    }

    String name;
    GetName(&name);
    AutoPtr<ArrayOf<AutoPtr<ArrayOf<ICertificate*> > > > certChains;
    certChains = jarVerifier->GetCertificateChains(name);
    if (certChains == NULL) {
        return NOERROR;
    }

    if (mSigners == NULL) {
        FAIL_RETURN(GetCodeSigners(certChains, (ArrayOf<ICodeSigner*>**)&mSigners))
    }

    if (mSigners == NULL) {
        return NOERROR;
    }

    AutoPtr<ArrayOf<ICodeSigner*> > cp = mSigners->Clone();
    *codeSigner = mSigners;
    REFCOUNT_ADD(*codeSigner)
    return NOERROR;
}

ECode CJarEntry::GetCodeSigners(
    /* [in] */ ArrayOf<AutoPtr<ArrayOf<ICertificate*> > > * certChains,
    /* [out, callee] */ ArrayOf<ICodeSigner*>** codeSigners)
{
    VALIDATE_NOT_NULL(codeSigners)
    *codeSigners = NULL;

    if (certChains == NULL) {
        return NOERROR;
    }

    AutoPtr<List<AutoPtr<ICodeSigner> > > asigners;
    asigners = new List<AutoPtr<ICodeSigner> >();

    for (Int32 i = 0; i < certChains->GetLength(); ++i) {
        AddCodeSigner(asigners, (*certChains)[i]);
    }

    AutoPtr<ArrayOf<ICodeSigner*> > tmp = ArrayOf<ICodeSigner*>::Alloc(asigners->GetSize());
    List<AutoPtr<ICodeSigner> >::Iterator it = asigners->Begin();
    Int32 i = 0;
    for (; it != asigners->End(); ++it) {
        tmp->Set(i++, *it);
    }
    *codeSigners = tmp;
    REFCOUNT_ADD(*codeSigners)
    return NOERROR;
}

ECode CJarEntry::AddCodeSigner(
    /* [in] */ List<AutoPtr<ICodeSigner> > * asigners,
    /* [in] */ ArrayOf<ICertificate*> * certs)
{
    for (Int32 i = 0; i < certs->GetLength(); ++i) {
        // Only X509Certificate instances are counted. See API spec.
        if (IX509Certificate::Probe((*certs)[i]) == NULL) {
            return NOERROR;
        }
    }

    AutoPtr<ICertPath> certPath;
    if (!mIsFactoryChecked) {
        AutoPtr<ICertificateFactoryHelper> helper;
        // CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&helper);
        mFactory = NULL;
        ECode ec = helper->GetInstance(String("X.509"), (ICertificateFactory**)&mFactory);
        if (ec == (ECode)E_CERTIFICATE_EXCEPTION) {
            // do nothing
        }
        else if (FAILED(ec)) {
            return ec;
        }
        mIsFactoryChecked = TRUE;
    }

    if (mFactory == NULL) {
        return NOERROR;
    }

    AutoPtr<IList> lst;
    FAIL_RETURN(CLinkedList::New((IList**)&lst))
    List<AutoPtr<ICodeSigner> >::Iterator it = asigners->Begin();
    for (Int32 i = 0; it != asigners->End(); ++it, ++i)
        lst->Add(i, (*it).Get());
    // ECode ec = mFactory->GenerateCertPath(lst, (ICertPath**)&certPath);
    // if (ec == (ECode)E_CERTIFICATE_EXCEPTION) {
    //     // do nothing
    // }
    // else if( FAILED(ec)) {
    //     return ec;
    // }

    if (certPath != NULL) {
        AutoPtr<ICodeSigner> codeSigner;
        // CCodeSigner::New(certPath, NULL, (ICodeSigner**)&codeSigner);
        asigners->PushBack(codeSigner);
    }
    return NOERROR;
}

} // namespace Jar
} // namespace Utility
} // namespace Elastos

