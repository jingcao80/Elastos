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

#ifndef __ELASTOS_SECURITY_KEYSTORESPI_H__
#define __ELASTOS_SECURITY_KEYSTORESPI_H__

#include "Elastos.CoreLibrary.Security.h"
#include "elastos/core/Object.h"

using Elastos::Core::Object;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::Utility::IEnumeration;
using Elastos::Utility::IDate;

namespace Elastos {
namespace Security {

class ECO_PUBLIC KeyStoreSpi
    : public Object
    , public IKeyStoreSpi
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Returns the key with the given alias, using the password to recover the
     * key from the store.
     *
     * @param alias
     *            the alias for the entry.
     * @param password
     *            the password used to recover the key.
     * @return the key with the specified alias, or {@code null} if the
     *         specified alias is not bound to an entry.
     * @throws NoSuchAlgorithmException
     *             if the algorithm for recovering the key is not available.
     * @throws UnrecoverableKeyException
     *             if the key can not be recovered.
     */
    virtual CARAPI EngineGetKey(
        /* [in] */ const String& alias,
        /* [in] */ ArrayOf<Char32> *password,
        /*[out] */ IKey **key) = 0;

    /**
     * Returns the certificate chain for the entry with the given alias.
     *
     * @param alias
     *            the alias for the entry
     * @return the certificate chain for the entry with the given alias, or
     *         {@code null} if the specified alias is not bound to an entry.
     */
    virtual CARAPI EngineGetCertificateChain(
            /* [in] */ const String& alias,
            /* [out, callee] */ ArrayOf<Elastos::Security::Cert::ICertificate*> **certChain) = 0;

    /**
     * Returns the trusted certificate for the entry with the given alias.
     *
     * @param alias
     *            the alias for the entry.
     * @return the trusted certificate for the entry with the given alias, or
     *         {@code null} if the specified alias is not bound to an entry.
     */
    virtual CARAPI EngineGetCertificate(
        /* [in] */ const String& alias,
        /* [out] */ Elastos::Security::Cert::ICertificate **cert) = 0;

    /**
     * Returns the creation date of the entry with the given alias.
     *
     * @param alias
     *            the alias for the entry.
     * @return the creation date, or {@code null} if the specified alias is not
     *         bound to an entry.
     */
    virtual CARAPI EngineGetCreationDate(
        /* [in] */ const String& alias,
        /* [out] */ IDate **creationDate) = 0;

    /**
     * Associates the given alias with the key, password and certificate chain.
     * <p>
     * If the specified alias already exists, it will be reassigned.
     *
     * @param alias
     *            the alias for the key.
     * @param key
     *            the key.
     * @param password
     *            the password.
     * @param chain
     *            the certificate chain.
     * @throws KeyStoreException
     *             if the specified key can not be protected, or if this
     *             operation fails for another reason.
     * @throws IllegalArgumentException
     *             if {@code key} is a {@code PrivateKey} and {@code chain} does
     *             not contain any certificates.
     */
    virtual CARAPI EngineSetKeyEntry(
        /* [in] */ const String& alias,
        /* [in] */ IKey *key,
        /* [in] */ ArrayOf<Char32> *password,
        /* [in] */ ArrayOf<Elastos::Security::Cert::ICertificate*> *chain) = 0;

    /**
     * Associates the given alias with a key and a certificate chain.
     * <p>
     * If the specified alias already exists, it will be reassigned.
     *
     * @param alias
     *            the alias for the key.
     * @param key
     *            the key in an encoded format.
     * @param chain
     *            the certificate chain.
     * @throws KeyStoreException
     *             if this operation fails.
     * @throws IllegalArgumentException
     *             if {@code key} is a {@code PrivateKey} and {@code chain}
     *             does.
     */
    virtual CARAPI EngineSetKeyEntry(
        /* [in] */ const String& alias,
        /* [in] */ ArrayOf<Byte> *key,
        /* [in] */ ArrayOf<Elastos::Security::Cert::ICertificate*> *chain) = 0;

    /**
     * Associates the given alias with a certificate.
     * <p>
     * If the specified alias already exists, it will be reassigned.
     *
     * @param alias
     *            the alias for the certificate.
     * @param cert
     *            the certificate.
     * @throws KeyStoreException
     *             if an existing alias is not associated to an entry containing
     *             a trusted certificate, or this method fails for any other
     *             reason.
     */
    virtual CARAPI EngineSetCertificateEntry(
        /* [in] */ const String& alias,
        /* [in] */ Elastos::Security::Cert::ICertificate *cert) = 0;

    /**
     * Deletes the entry identified with the given alias from this {@code
     * KeyStoreSpi}.
     *
     * @param alias
     *            the alias for the entry.
     * @throws KeyStoreException
     *             if the entry can not be deleted.
     */
    virtual CARAPI EngineDeleteEntry(
        /* [in] */ const String& alias) = 0;

    /**
     * Returns an {@code Enumeration} over all alias names stored in this
     * {@code KeyStoreSpi}.
     *
     * @return an {@code Enumeration} over all alias names stored in this
     *         {@code KeyStoreSpi}.
     */
    virtual CARAPI EngineAliases(
        /* [out] */ IEnumeration **aliases) = 0;

    /**
     * Indicates whether the given alias is present in this {@code KeyStoreSpi}.
     *
     * @param alias
     *            the alias of an entry.
     * @return {@code true} if the alias exists, {@code false} otherwise.
     */
    virtual CARAPI EngineContainsAlias(
        /* [in] */ const String& alias,
        /* [out] */ Boolean *result) = 0;

    /**
     * Returns the number of entries stored in this {@code KeyStoreSpi}.
     *
     * @return the number of entries stored in this {@code KeyStoreSpi}.
     */
    virtual CARAPI EngineSize(
        /* [out] */ Int32 *size) = 0;

    /**
     * Indicates whether the specified alias is associated with either a
     * {@link KeyStore.PrivateKeyEntry} or a {@link KeyStore.SecretKeyEntry}.
     *
     * @param alias
     *            the alias of an entry.
     * @return {@code true} if the given alias is associated with a key entry.
     */
    virtual CARAPI EngineIsKeyEntry(
        /* [in] */ const String& alias,
        /* [out] */ Boolean *result) = 0;

    /**
     * Indicates whether the specified alias is associated with a
     * {@link KeyStore.TrustedCertificateEntry}.
     *
     * @param alias
     *            the alias of an entry.
     * @return {@code true} if the given alias is associated with a certificate
     *         entry.
     */
    virtual CARAPI EngineIsCertificateEntry(
        /* [in] */ const String& alias,
        /* [out] */ Boolean *result) = 0;

    /**
     * Returns the alias associated with the first entry whose certificate
     * matches the specified certificate.
     *
     * @param cert
     *            the certificate to find the associated entry's alias for.
     * @return the alias or {@code null} if no entry with the specified
     *         certificate can be found.
     */
    virtual CARAPI EngineGetCertificateAlias(
        /* [in] */ Elastos::Security::Cert::ICertificate *cert,
        /* [out] */ String* alias) = 0;

    /**
     * Writes this {@code KeyStoreSpi} to the specified {@code OutputStream}.
     * The data written to the {@code OutputStream} is protected by the
     * specified password.
     *
     * @param stream
     *            the {@code OutputStream} to write the store's data to.
     * @param password
     *            the password to protect the data.
     * @throws IOException
     *             if a problem occurred while writing to the stream.
     * @throws NoSuchAlgorithmException
     *             if the required algorithm is not available.
     * @throws CertificateException
     *             if the an exception occurred while storing the certificates
     *             of this code {@code KeyStoreSpi}.
     */
    virtual CARAPI EngineStore(
        /* [in] */ IOutputStream *stream,
        /* [in] */ ArrayOf<Char32> *password) = 0;

    /**
     * Stores this {@code KeyStoreSpi} using the specified {@code
     * LoadStoreParameter}.
     *
     * @param param
     *            the {@code LoadStoreParameter} that specifies how to store
     *            this {@code KeyStoreSpi}, maybe {@code null}.
     * @throws IOException
     *             if a problem occurred while writing to the stream.
     * @throws NoSuchAlgorithmException
     *             if the required algorithm is not available.
     * @throws CertificateException
     *             if the an exception occurred while storing the certificates
     *             of this code {@code KeyStoreSpi}.
     * @throws IllegalArgumentException
     *             if the given {@link KeyStore.LoadStoreParameter} is not
     *             recognized.
     */
    virtual CARAPI EngineStore(
        /* [in] */ IKeyStoreLoadStoreParameter *param);

    /**
     * Loads this {@code KeyStoreSpi} from the given {@code InputStream}.
     * Utilizes the given password to verify the stored data.
     *
     * @param stream
     *            the {@code InputStream} to load this {@code KeyStoreSpi}'s
     *            data from.
     * @param password
     *            the password to verify the stored data, maybe {@code null}.
     * @throws IOException
     *             if a problem occurred while reading from the stream.
     * @throws NoSuchAlgorithmException
     *             if the required algorithm is not available.
     * @throws CertificateException
     *             if the an exception occurred while loading the certificates
     *             of this code {@code KeyStoreSpi}.
     */
    virtual CARAPI EngineLoad(
        /* [in] */ IInputStream *stream,
        /* [in] */ ArrayOf<Char32> *password) = 0;

    /**
     * Loads this {@code KeyStoreSpi} using the specified {@code
     * LoadStoreParameter}.
     *
     * @param param
     *            the {@code LoadStoreParameter} that specifies how to load this
     *            {@code KeyStoreSpi}, maybe {@code null}.
     * @throws IOException
     *             if a problem occurred while reading from the stream.
     * @throws NoSuchAlgorithmException
     *             if the required algorithm is not available.
     * @throws CertificateException
     *             if the an exception occurred while loading the certificates
     *             of this code {@code KeyStoreSpi}.
     * @throws IllegalArgumentException
     *             if the given {@link KeyStore.LoadStoreParameter} is not
     *             recognized.
     */
    virtual CARAPI EngineLoad(
        /* [in] */ IKeyStoreLoadStoreParameter *param);

    /**
     * Returns the {@code Entry} with the given alias, using the specified
     * {@code ProtectionParameter}.
     *
     * @param alias
     *            the alias of the requested entry.
     * @param protParam
     *            the {@code ProtectionParameter}, used to protect the requested
     *            entry, maybe {@code null}.
     * @return he {@code Entry} with the given alias, using the specified
     *         {@code ProtectionParameter}.
     * @throws NoSuchAlgorithmException
     *             if the required algorithm is not available.
     * @throws UnrecoverableEntryException
     *             if the entry can not be recovered.
     * @throws KeyStoreException
     *             if this operation fails
     */
    virtual CARAPI EngineGetEntry(
        /* [in] */ const String& alias,
        /* [in] */ IKeyStoreProtectionParameter *protParam,
        /* [out] */ IKeyStoreEntry **entry);

    /**
     * Stores the given {@code Entry} in this {@code KeyStoreSpi} and associates
     * the entry with the given {@code alias}. The entry is protected by the
     * specified {@code ProtectionParameter}.
     * <p>
     * If the specified alias already exists, it will be reassigned.
     *
     * @param alias
     *            the alias for the entry.
     * @param entry
     *            the entry to store.
     * @param protParam
     *            the {@code ProtectionParameter} to protect the entry.
     * @throws KeyStoreException
     *             if this operation fails.
     */
    virtual CARAPI EngineSetEntry(
        /* [in] */ const String& alias,
        /* [in] */ IKeyStoreEntry *entry,
        /* [in] */ IKeyStoreProtectionParameter *protParam);

    /**
     * Indicates whether the entry for the given alias is assignable to the
     * provided {@code Class}.
     *
     * @param alias
     *            the alias for the entry.
     * @param entryClass
     *            the type of the entry.
     * @return {@code true} if the {@code Entry} for the alias is assignable to
     *         the specified {@code entryClass}.
     */
    virtual CARAPI EngineEntryInstanceOf(
        /* [in] */ const String& alias,
        /* [in] */ const ClassID& entryClass,
        /* [out] */ Boolean *result);

    /*
     * This method returns password which is encapsulated in
     * CallbackHandlerProtection object If there is no implementation of
     * CallbackHandler then this method returns null
     */
    static CARAPI GetPasswordFromCallBack(
        /* [in] */ IKeyStoreProtectionParameter *protParam,
        /* [out, callee] */ ArrayOf<Char32> **pwd);
};

} // namespace Security
} // namespace Elastos

#endif // __ELASTOS_SECURITY_KEYSTORESPI_H__
