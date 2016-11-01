
#ifndef __Elastos_DevSamples_SecurityDemo_ElastosKeyPairGeneratorTest_H__
#define __Elastos_DevSamples_SecurityDemo_ElastosKeyPairGeneratorTest_H__

#include "Elastos.DevSamples.SecurityDemo.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Core::Object;
using Elastos::Security::IKeyPairGenerator;
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

private:
    CARAPI SetupPassword();

    static CARAPI_(AutoPtr<IDate>) initNOW();

    static CARAPI_(AutoPtr<IDate>) initNOW_PLUS_10_YEARS();

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
