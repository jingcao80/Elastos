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

#ifndef __ELASTOS_UTILITY_JARVERIFIER_H__
#define __ELASTOS_UTILITY_JARVERIFIER_H__

#include "OutputStream.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::Etl::HashMap;
using Elastos::IO::IOutputStream;
using Elastos::IO::OutputStream;
using Elastos::Security::Cert::ICertificate;
using Elastos::Security::IMessageDigest;

namespace Elastos {
namespace Utility {
namespace Jar {

/**
 * Non-public class used by {@link JarFile} and {@link JarInputStream} to manage
 * the verification of signed JARs. {@code JarFile} and {@code JarInputStream}
 * objects are expected to have a {@code JarVerifier} instance member which
 * can be used to carry out the tasks associated with verifying a signed JAR.
 * These tasks would typically include:
 * <ul>
 * <li>verification of all signed signature files
 * <li>confirmation that all signed data was signed only by the party or parties
 * specified in the signature block data
 * <li>verification that the contents of all signature files (i.e. {@code .SF}
 * files) agree with the JAR entries information found in the JAR manifest.
 * </ul>
 */
class JarVerifier
    : public Object
{
public:
    typedef ArrayOf<ICertificate*> CertificateArray;
    typedef HashMap<String, AutoPtr<CertificateArray> > StringCertificateMap;
    typedef typename StringCertificateMap::Iterator StringCertificateMapIterator;

    /**
    * Stores and a hash and a message digest and verifies that massage digest
    * matches the hash.
    */
    class VerifierEntry
        : public OutputStream
    {
    public:
        VerifierEntry(
            /* [in] */ const String& name,
            /* [in] */ IMessageDigest* digest,
            /* [in] */ ArrayOf<Byte>* hash,
            /* [in] */ ArrayOf<AutoPtr<CertificateArray> > * certificates,
            /* [in] */ HashMap<String, AutoPtr<ArrayOf<AutoPtr<CertificateArray> > > >* map,
            /* [in] */ JarVerifier* host);

        /**
         * Updates a digest with one byte.
         */
        CARAPI Write(
            /* [in] */ Int32 value);

        CARAPI Write(
            /* [in] */ ArrayOf<Byte>* buffer);

        /**
         * Updates a digest with byte array.
         */
        CARAPI Write(
            /* [in] */ ArrayOf<Byte>* buffer,
            /* [in] */ Int32 offset,
            /* [in] */ Int32 count);

        /**
         * Verifies that the digests stored in the manifest match the decrypted
         * digests from the .SF file. This indicates the validity of the
         * signing, not the integrity of the file, as it's digest must be
         * calculated and verified when its contents are read.
         *
         * @throws SecurityException
         *             if the digest value stored in the manifest does <i>not</i>
         *             agree with the decrypted digest as recovered from the
         *             <code>.SF</code> file.
         */
        CARAPI Verify();

    private:
        String mName;

        AutoPtr<IMessageDigest> mDigest;

        AutoPtr<ArrayOf<Byte> > mHash;

        AutoPtr<ArrayOf<AutoPtr<CertificateArray > > > mCertificates;

        AutoPtr<HashMap<String, AutoPtr<ArrayOf<AutoPtr<CertificateArray> > > > > mVerifiedEntries;

        JarVerifier* mHost;
    };

public:
    /**
     * Constructs and returns a new instance of {@code JarVerifier}.
     *
     * @param name
     *            the name of the JAR file being verified.
     */
    JarVerifier(
        /* [in] */ const String& name,
        /* [in] */ IManifest* manifest,
        /* [in] */ HashMap<String, AutoPtr<ArrayOf<Byte> > >* metaEntries);

    ~JarVerifier();

    CARAPI InitEntry(
        /* [in] */ const String& name,
        /* [out] */ VerifierEntry** entry);

    CARAPI AddMetaEntry(
        /* [in] */ const String& name,
        /* [in] */ ArrayOf<Byte>* buf);

    CARAPI ReadCertificates(
        /* [out] */ Boolean* result);

    /**
     * Returns a <code>boolean</code> indication of whether or not the
     * associated jar file is signed.
     *
     * @return {@code true} if the JAR is signed, {@code false}
     *         otherwise.
     */
    CARAPI IsSignedJar(
        /* [out] */ Boolean* isSigned);

    /**
     * Returns all of the {@link java.security.cert.Certificate} chains that
     * were used to verify the signature on the JAR entry called
     * {@code name}. Callers must not modify the returned arrays.
     *
     * @param name
     *            the name of a JAR entry.
     * @return an array of {@link java.security.cert.Certificate} chains.
     */
    AutoPtr<ArrayOf<AutoPtr<CertificateArray> > > GetCertificateChains(
        /* [in] */ const String& name);

    CARAPI RemoveMetaEntries();

private:
    CARAPI VerifyCertificate(
        /* [in] */ const String& certFile);

    CARAPI Verify(
        /* [in] */ IAttributes* attributes,
        /* [in] */ const String& entry,
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ Boolean ignoreSecondEndline,
        /* [in] */ Boolean ignorable,
        /* [out] */ Boolean* result);

public:
    Int32 mMainAttributesEnd;

private:
    static const AutoPtr<ArrayOf<String> > DIGEST_ALGORITHMS;

    String mJarName;
    AutoPtr<IManifest> mManifest;
    AutoPtr<HashMap<String, AutoPtr<ArrayOf<Byte> > > > mMetaEntries;

    typedef HashMap<String, AutoPtr<IAttributes> > StringAttributesMap;
    typedef typename StringAttributesMap::Iterator StringAttributesMapIterator;

    AutoPtr<HashMap<String, AutoPtr<StringAttributesMap> > > mSignatures;

    AutoPtr<StringCertificateMap > mCertificates;
    AutoPtr<HashMap<String, AutoPtr<ArrayOf<AutoPtr<CertificateArray> > > > > mVerifiedEntries;
};

} // namespace Jar
} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY_JARVERIFIER_H__
