
#ifndef __Elastos_DevSamples_SecurityDemo_ElastosKeyPairGeneratorTest_H__
#define __Elastos_DevSamples_SecurityDemo_ElastosKeyPairGeneratorTest_H__

#include "Elastos.DevSamples.SecurityDemo.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Core::Object;
using Elastos::Security::IKeyPair;
using Elastos::Security::IKeyPairGenerator;
using Elastos::Security::Spec::IAlgorithmParameterSpec;
using Elastos::Math::IBigInteger;
using Elastos::Utility::IDate;
using Elastosx::Security::Auth::X500::IX500Principal;

namespace Elastos {
namespace DevSamples {
namespace SecurityDemo {

class ElastosKeyPairGeneratorTest
    : public Object
{
public:
    ElastosKeyPairGeneratorTest(
        /* [in] */ IContext* c);

    CARAPI TestKeyPairGenerator_Initialize_Params_Encrypted_Success();

    CARAPI TestKeyPairGenerator_Initialize_KeySize_Encrypted_Failure();

    CARAPI TestKeyPairGenerator_Initialize_KeySizeAndSecureRandom_Encrypted_Failure();

    CARAPI TestKeyPairGenerator_Initialize_ParamsAndSecureRandom_Encrypted_Failure();

    CARAPI TestKeyPairGenerator_GenerateKeyPair_Encrypted_Success();

    CARAPI TestKeyPairGenerator_GenerateKeyPair_DSA_Unencrypted_Success();

    CARAPI TestKeyPairGenerator_GenerateKeyPair_DSA_2048_Unencrypted_Success();

    CARAPI TestKeyPairGenerator_GenerateKeyPair_DSA_SpecifiedParams_Unencrypted_Success();

    CARAPI TestKeyPairGenerator_GenerateKeyPair_EC_Unencrypted_Success();

    CARAPI TestKeyPairGenerator_GenerateKeyPair_EC_P521_Unencrypted_Success();

    CARAPI TestKeyPairGenerator_GenerateKeyPair_RSA_Unencrypted_Success();

    CARAPI TestKeyPairGenerator_GenerateKeyPair_RSA_WithParams_Unencrypted_Success();

    CARAPI TestKeyPairGenerator_GenerateKeyPair_Replaced_Success();

    CARAPI TestKeyPairGenerator_GenerateKeyPair_Replaced_UnencryptedToEncrypted_Success();

private:
    CARAPI SetupPassword();

    static CARAPI_(AutoPtr<IDate>) initNOW();

    static CARAPI_(AutoPtr<IDate>) initNOW_PLUS_10_YEARS();

    CARAPI_(Boolean) IsEquals(
        /* [in] */ IInterface* expected,
        /* [in] */ IInterface* actual);

    CARAPI AssertEquals(
        /* [in] */ IInterface* expected,
        /* [in] */ IInterface* actual);

    static AssertDateEquals(
        /* [in] */ IDate* date1,
        /* [in] */ IDate* date2);

    CARAPI AssertKeyPairCorrect(
        /* [in] */ IKeyPair* pair,
        /* [in] */ const String& alias,
        /* [in] */ const String& keyType,
        /* [in] */ Int32 keySize,
        /* [in] */ IAlgorithmParameterSpec* spec,
        /* [in] */ IX500Principal* dn,
        /* [in] */ IBigInteger* serial,
        /* [in] */ IDate* start,
        /* [in] */ IDate* end);

private:
    AutoPtr<IContext> mContext;

    AutoPtr<Elastos::Droid::KeyStore::Security::IKeyStore> mAndroidKeyStore;

    AutoPtr<IKeyPairGenerator> mGenerator;

    static const String TEST_ALIAS_1;

    static const String TEST_ALIAS_2;

    static AutoPtr<IX500Principal> TEST_DN_1;

    static AutoPtr<IX500Principal> TEST_DN_2;

    static AutoPtr<IBigInteger> TEST_SERIAL_1 ;

    static AutoPtr<IBigInteger> TEST_SERIAL_2;

    static Int64 NOW_MILLIS;

    /* We have to round this off because X509v3 doesn't store milliseconds. */
    static AutoPtr<IDate> NOW;

    //@SuppressWarnings("deprecation")
    static AutoPtr<IDate> NOW_PLUS_10_YEARS;
};

} // namespace SecurityDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_SecurityDemo_ElastosKeyPairGeneratorTest_H__
