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

#ifndef __Elastos_DevSamples_SecurityDemo_ElastosKeyStoreTest_H__
#define __Elastos_DevSamples_SecurityDemo_ElastosKeyStoreTest_H__

#include "Elastos.DevSamples.SecurityDemo.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Core::Object;
using Elastos::Security::IPrivateKey;
using Elastos::Security::IKeyStorePrivateKeyEntry;
using Elastos::Security::IKeyPairGenerator;
using Elastos::Security::Cert::ICertificate;
using Elastos::Security::Cert::IX509Certificate;
using Elastos::Math::IBigInteger;
using Elastos::Utility::IDate;
using Elastos::Utility::ICollection;
using Elastosx::Security::Auth::X500::IX500Principal;

namespace Elastos {
namespace DevSamples {
namespace SecurityDemo {

class ElastosKeyStoreTest
    : public Object
{
public:
    ElastosKeyStoreTest(
        /* [in] */ IContext* c);

    CARAPI TestKeyStore_Aliases_Encrypted_Success();

    CARAPI TestKeyStore_Aliases_NotInitialized_Encrypted_Failure();

    CARAPI TestKeyStore_ContainsAliases_PrivateAndCA_Encrypted_Success();

    CARAPI TestKeyStore_ContainsAliases_CAOnly_Encrypted_Success();

    CARAPI TestKeyStore_ContainsAliases_NonExistent_Encrypted_Failure();

    CARAPI TestKeyStore_DeleteEntry_Encrypted_Success();

    CARAPI TestKeyStore_DeleteEntry_EmptyStore_Encrypted_Success();

    CARAPI TestKeyStore_DeleteEntry_NonExistent_Encrypted_Success();

    CARAPI TestKeyStore_GetCertificate_Single_Encrypted_Success();

    CARAPI TestKeyStore_GetCertificate_NonExist_Encrypted_Failure();

    CARAPI TestKeyStore_GetCertificateAlias_CAEntry_Encrypted_Success();

    CARAPI TestKeyStore_GetCertificateAlias_PrivateKeyEntry_Encrypted_Success();

    CARAPI TestKeyStore_GetCertificateAlias_CAEntry_WithPrivateKeyUsingCA_Encrypted_Success();

    CARAPI TestKeyStore_GetCertificateAlias_NonExist_Empty_Encrypted_Failure();

    CARAPI TestKeyStore_GetCertificateAlias_NonExist_Encrypted_Failure();

    CARAPI TestKeyStore_GetCertificateChain_SingleLength_Encrypted_Success();

    CARAPI TestKeyStore_GetCertificateChain_NonExist_Encrypted_Failure();

    CARAPI TestKeyStore_GetCreationDate_PrivateKeyEntry_Encrypted_Success();

    CARAPI TestKeyStore_GetCreationDate_PrivateKeyEntry_Unencrypted_Success();

    CARAPI TestKeyStore_GetCreationDate_CAEntry_Encrypted_Success();

    CARAPI TestKeyStore_GetEntry_NullParams_Encrypted_Success();

    CARAPI TestKeyStore_GetEntry_DSA_NullParams_Unencrypted_Success();

    CARAPI TestKeyStore_GetEntry_EC_NullParams_Unencrypted_Success();

    CARAPI TestKeyStore_GetEntry_RSA_NullParams_Unencrypted_Success();

    CARAPI TestKeyStore_GetEntry_Nonexistent_NullParams_Encrypted_Failure();

    CARAPI TestKeyStore_GetEntry_Nonexistent_NullParams_Unencrypted_Failure();

    CARAPI TestKeyStore_GetKey_NoPassword_Encrypted_Success();

    CARAPI TestKeyStore_GetKey_NoPassword_Unencrypted_Success();

    CARAPI TestKeyStore_GetKey_Certificate_Encrypted_Failure();

    CARAPI TestKeyStore_GetKey_NonExistent_Encrypted_Failure();

    CARAPI TestKeyStore_GetProvider_Encrypted_Success();

    CARAPI TestKeyStore_GetType_Encrypted_Success();

    CARAPI TestKeyStore_IsCertificateEntry_CA_Encrypted_Success();

    CARAPI TestKeyStore_IsCertificateEntry_PrivateKey_Encrypted_Failure();

    CARAPI TestKeyStore_IsCertificateEntry_NonExist_Encrypted_Failure();

    CARAPI TestKeyStore_IsCertificateEntry_NonExist_Unencrypted_Failure();

    CARAPI TestKeyStore_IsKeyEntry_PrivateKey_Encrypted_Success();

    CARAPI TestKeyStore_IsKeyEntry_CA_Encrypted_Failure();

    CARAPI TestKeyStore_IsKeyEntry_NonExist_Encrypted_Failure();

    CARAPI TestKeyStore_SetCertificate_CA_Encrypted_Success();

    CARAPI TestKeyStore_SetCertificate_CAExists_Overwrite_Encrypted_Success();

    CARAPI TestKeyStore_SetCertificate_PrivateKeyExists_Encrypted_Failure();

    CARAPI TestKeyStore_SetEntry_PrivateKeyEntry_Encrypted_Success();

    CARAPI TestKeyStore_SetEntry_PrivateKeyEntry_DSA_Unencrypted_Success();

    CARAPI TestKeyStore_SetEntry_PrivateKeyEntry_EC_Unencrypted_Success();

    CARAPI TestKeyStore_SetEntry_PrivateKeyEntry_RSA_Unencrypted_Success();

    CARAPI TestKeyStore_SetEntry_PrivateKeyEntry_Params_Unencrypted_Failure();

    CARAPI TestKeyStore_SetEntry_PrivateKeyEntry_Overwrites_PrivateKeyEntry_Encrypted_Success();

    CARAPI TestKeyStore_SetEntry_CAEntry_Overwrites_PrivateKeyEntry_Encrypted_Success();

    CARAPI TestKeyStore_SetEntry_PrivateKeyEntry_Overwrites_CAEntry_Encrypted_Success();

    CARAPI TestKeyStore_SetEntry_PrivateKeyEntry_Overwrites_ShortPrivateKeyEntry_Encrypted_Success();

    CARAPI TestKeyStore_SetEntry_CAEntry_Overwrites_CAEntry_Encrypted_Success();

    CARAPI TestKeyStore_SetKeyEntry_ProtectedKey_Encrypted_Failure();

    CARAPI TestKeyStore_SetKeyEntry_Encrypted_Success();

    CARAPI TestKeyStore_SetKeyEntry_Replaced_Encrypted_Success();

    CARAPI TestKeyStore_SetKeyEntry_ReplacedChain_Encrypted_Success();

    CARAPI TestKeyStore_SetKeyEntry_ReplacedChain_DifferentPrivateKey_Encrypted_Failure();

    CARAPI TestKeyStore_SetKeyEntry_ReplacedChain_UnencryptedToEncrypted_Failure();

    CARAPI TestKeyStore_Size_Encrypted_Success();

    CARAPI TestKeyStore_Store_LoadStoreParam_Encrypted_Failure();

    CARAPI TestKeyStore_Load_InputStreamSupplied_Encrypted_Failure();

    CARAPI TestKeyStore_Load_PasswordSupplied_Encrypted_Failure();

    CARAPI TestKeyStore_Store_OutputStream_Encrypted_Failure();

    CARAPI TestKeyStore_KeyOperations_Wrap_Encrypted_Success();

private:
    CARAPI SetupPassword();

    static CARAPI_(AutoPtr<IDate>) initNOW();

    static CARAPI_(AutoPtr<IDate>) initNOW_PLUS_10_YEARS();

    CARAPI AssertAliases(
        /* [in] */ ArrayOf<String>* expectedAliases);

    CARAPI AssertPrivateKeyEntryEquals(
        /* [in] */ IKeyStorePrivateKeyEntry* keyEntry,
        /* [in] */ const String& keyType,
        /* [in] */ ArrayOf<Byte>* key,
        /* [in] */ ArrayOf<Byte>* cert,
        /* [in] */ ArrayOf<Byte>* ca);

    CARAPI AssertPrivateKeyEntryEquals(
        /* [in] */ IKeyStorePrivateKeyEntry* keyEntry,
        /* [in] */ IPrivateKey* expectedKey,
        /* [in] */ ICertificate* expectedCert,
        /* [in] */ ICollection* expectedChain);

    //@SuppressWarnings("deprecation")
    CARAPI static GenerateCertificate(
        /* [in] */ Elastos::Droid::KeyStore::Security::IKeyStore* keyStore,
        /* [in] */ const String& alias,
        /* [in] */ IBigInteger* serialNumber,
        /* [in] */ IX500Principal* subjectDN,
        /* [in] */ IDate* notBefore,
        /* [in] */ IDate* notAfter,
        /* [out] */ IX509Certificate** certificate);

    CARAPI SetupKey();

    CARAPI_(Boolean) IsEquals(
        /* [in] */ IInterface* expected,
        /* [in] */ IInterface* actual);

    CARAPI AssertEquals(
        /* [in] */ IInterface* expected,
        /* [in] */ IInterface* actual);

private:
    AutoPtr<IContext> mContext;

    AutoPtr<Elastos::Droid::KeyStore::Security::IKeyStore> mAndroidKeyStore;

    AutoPtr<Elastos::Security::IKeyStore> mKeyStore;

    static const String TEST_ALIAS_1;

    static const String TEST_ALIAS_2;

    static const String TEST_ALIAS_3;

    static AutoPtr<IX500Principal> TEST_DN_1;

    static AutoPtr<IX500Principal> TEST_DN_2;

    static AutoPtr<IBigInteger> TEST_SERIAL_1;

    static AutoPtr<IBigInteger> TEST_SERIAL_2;

    static Int64 NOW_MILLIS;

    /* We have to round this off because X509v3 doesn't store milliseconds. */
    static AutoPtr<IDate> NOW;

    //@SuppressWarnings("deprecation")
    static AutoPtr<IDate> NOW_PLUS_10_YEARS;

    /**
     * The amount of time to allow before and after expected time for variance
     * in timing tests.
     */
    static const Int32 SLOP_TIME_MILLIS;
};

} // namespace SecurityDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_SecurityDemo_ElastosKeyStoreTest_H__
