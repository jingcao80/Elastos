
#ifndef __ORG_CONSCRYPT_COPENSSLECKEYPAIRGENERATOR_H__
#define __ORG_CONSCRYPT_COPENSSLECKEYPAIRGENERATOR_H__

#include "_Org_Conscrypt_COpenSSLECKeyPairGenerator.h"
#include <Elastos.CoreLibrary.Core.h>
#include <Elastos.CoreLibrary.Security.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/Object.h>
#include "elastos/security/KeyPairGenerator.h"

using Elastos::Security::IKeyPair;
using Elastos::Security::ISecureRandom;
using Elastos::Security::KeyPairGenerator;
using Elastos::Security::Spec::IAlgorithmParameterSpec;
using Elastos::Utility::IMap;

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLECKeyPairGenerator)
    , public IOpenSSLECKeyPairGenerator
    , public KeyPairGenerator
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    // @Override
    CARAPI GenerateKeyPair(
        /* [out] */ IKeyPair** result);

    // @Override
    CARAPI Initialize(
        /* [in] */ Int32 keysize,
        /* [in] */ ISecureRandom* random);

    // @Override
    CARAPI Initialize(
        /* [in] */ IAlgorithmParameterSpec* param,
        /* [in] */ ISecureRandom* random);

private:
    static Boolean Init();

private:
    static const String ALGORITHM; // = "EC";

    static const Int32 DEFAULT_KEY_SIZE = 192;

    static AutoPtr<IMap/*<Integer, String>*/> SIZE_TO_CURVE_NAME;

    AutoPtr<IOpenSSLECGroupContext> mGroup;

    static Boolean sIsInited;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLECKEYPAIRGENERATOR_H__
