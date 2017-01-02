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

#ifndef __ELASTOS_UTILITY_JAR_ELASTOS_UTILITY_CJARENTRY_H__
#define __ELASTOS_UTILITY_JAR_ELASTOS_UTILITY_CJARENTRY_H__

#include "Elastos.CoreLibrary.Security.h"
#include "_Elastos_Utility_Jar_CJarEntry.h"
#include "ZipEntry.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::Utility::Zip::IZipEntry;
using Elastos::Utility::Zip::ZipEntry;
using Elastos::Security::ICodeSigner;
using Elastos::Security::Cert::ICertificate;
using Elastos::Security::Cert::ICertificateFactory;

namespace Elastos {
namespace Utility {
namespace Jar {

CarClass(CJarEntry)
    , public ZipEntry
    , public IJarEntry
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CJarEntry();

    CARAPI constructor(
        /* [in] */ const String& name);

    CARAPI constructor(
        /* [in] */ IZipEntry* zipEntry);

    CARAPI constructor(
        /* [in] */ IJarEntry* jarEntry);

    CARAPI constructor(
        /* [in] */ IZipEntry* jarEntry,
        /* [in] */ IJarFile* parentJar);

    /**
     * Returns the {@code Attributes} object associated with this entry or
     * {@code null} if none exists.
     *
     * @return the {@code Attributes} for this entry.
     * @throws IOException
     *                If an error occurs obtaining the {@code Attributes}.
     * @see Attributes
     */
    CARAPI GetAttributes(
        /* [out] */ IAttributes** attrib);

    /**
     * Returns an array of {@code Certificate} Objects associated with this
     * entry or {@code null} if none exists. Make sure that the everything is
     * read from the input stream before calling this method, or else the method
     * returns {@code null}.
     * <p>
     * This method returns all the signers' unverified chains concatenated
     * together in one array. To know which certificates were tied to the
     * private keys that made the signatures on this entry, see
     * {@link #getCodeSigners()} instead.
     *
     * @see java.security.cert.Certificate
     */
    CARAPI GetCertificates(
        /* [out, callee] */ ArrayOf<ICertificate*>** certificates);

    CARAPI SetAttributes(
        /* [in] */ IAttributes* attrib);

    /**
     * Returns the code signers for the digital signatures associated with the
     * JAR file. If there is no such code signer, it returns {@code null}. Make
     * sure that the everything is read from the input stream before calling
     * this method, or else the method returns {@code null}.
     * <p>
     * Only the digital signature on the entry is cryptographically verified.
     * None of the certificates in the the {@link CertPath} returned from
     * {@link CodeSigner#getSignerCertPath()} are verified and must be verified
     * by the caller if needed. See {@link CertPathValidator} for more
     * information.
     *
     * @return an array of CodeSigner for this JAR entry.
     * @see CodeSigner
     */
    CARAPI GetCodeSigners(
        /* [out, callee] */ ArrayOf<ICodeSigner*>** codeSigner);

private:
    CARAPI GetCodeSigners(
        /* [in] */ ArrayOf<AutoPtr<ArrayOf<ICertificate*> > > * certChains,
        /* [out, callee] */ ArrayOf<ICodeSigner*>** codeSigners);

    CARAPI AddCodeSigner(
    /* [in] */ List<AutoPtr<ICodeSigner> >* asigners,
    /* [in] */ ArrayOf<ICertificate*>* certs);

public:
    AutoPtr<IJarFile> mParentJar;
    AutoPtr<ArrayOf<ICodeSigner*> > mSigners;

private:
    AutoPtr<IAttributes> mAttributes;
    // Cached factory used to build CertPath-s in <code>getCodeSigners()</code>.
    AutoPtr<ICertificateFactory> mFactory;
    Boolean mIsFactoryChecked;
};

} // namespace Jar
} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY_JAR_ELASTOS_UTILITY_CJARENTRY_H__
