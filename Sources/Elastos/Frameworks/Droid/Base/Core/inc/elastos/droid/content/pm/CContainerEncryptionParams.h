
#ifndef __ELASTOS_DROID_CONTENT_PM_CCONTAINERENCRYPTIONPARAMS_H__
#define __ELASTOS_DROID_CONTENT_PM_CCONTAINERENCRYPTIONPARAMS_H__

#include <Elastos.CoreLibrary.Extensions.h>
#include <Elastos.CoreLibrary.Security.h>
#include "_Elastos_Droid_Content_Pm_CContainerEncryptionParams.h"
#include <elastos/core/Object.h>

using Elastos::Security::Spec::IAlgorithmParameterSpec;
using Elastosx::Crypto::ISecretKey;
using Elastosx::Crypto::Spec::IIvParameterSpec;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CarClass(CContainerEncryptionParams)
    , public Object
    , public IContainerEncryptionParams
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& encryptionAlgorithm,
        /* [in] */ IAlgorithmParameterSpec* encryptionSpec,
        /* [in] */ ISecretKey* encryptionKey);

    CARAPI constructor(
        /* [in] */ const String& encryptionAlgorithm,
        /* [in] */ IAlgorithmParameterSpec* encryptionSpec,
        /* [in] */ ISecretKey* encryptionKey,
        /* [in] */ const String& macAlgorithm,
        /* [in] */ IAlgorithmParameterSpec* macSpec,
        /* [in] */ ISecretKey* macKey,
        /* [in] */ ArrayOf<Byte>* macTag,
        /* [in] */ Int64 authenticatedDataStart,
        /* [in] */ Int64 encryptedDataStart,
        /* [in] */ Int64 dataEnd);

    CARAPI GetEncryptionAlgorithm(
        /* [out] */ String* ealgo);

    CARAPI GetEncryptionSpec(
        /* [out] */ IAlgorithmParameterSpec** spec);

    CARAPI GetEncryptionKey(
        /* [out] */ ISecretKey** key);

    CARAPI GetMacAlgorithm(
        /* [out] */ String* malgo);

    CARAPI GetMacSpec(
        /* [out] */ IAlgorithmParameterSpec** spec);

    CARAPI GetMacKey(
        /* [out] */ ISecretKey** key);

    CARAPI GetMacTag(
        /* [out, callee] */ ArrayOf<Byte>** tag);

    CARAPI GetAuthenticatedDataStart(
        /* [out] */ Int64* aDataStart);

    CARAPI GetEncryptedDataStart(
        /* [out] */ Int64* eDataStart);

    CARAPI GetDataEnd(
        /* [out] */ Int64* dataEnd);

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* isEquals);

    CARAPI GetHashCode(
        /* [out] */ Int32* code);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    static Boolean IsSecretKeyEqual(
        /* [in] */ ISecretKey* key1,
        /* [in] */ ISecretKey* key2);

private:
    /** What we print out first when toString() is called. */
    static const String TO_STRING_PREFIX;

    /**
     * Parameter type for parceling that indicates the next parameters are
     * IvParameters.
     */
    static const Int32 ENC_PARAMS_IV_PARAMETERS;

    /** Parameter type for paceling that indicates there are no MAC parameters. */
    static const Int32 MAC_PARAMS_NONE;

    /** The encryption algorithm used. */
    String mEncryptionAlgorithm;

    /** The parameter spec to be used for encryption. */
    AutoPtr<IIvParameterSpec> mEncryptionSpec;

    /** Secret key to be used for decryption. */
    AutoPtr<ISecretKey> mEncryptionKey;

    /** Algorithm name for the MAC to be used. */
    String mMacAlgorithm;

    /** The parameter spec to be used for the MAC tag authentication. */
    AutoPtr<IAlgorithmParameterSpec> mMacSpec;

    /** Secret key to be used for MAC tag authentication. */
    AutoPtr<ISecretKey> mMacKey;

    /** MAC tag authenticating the data in the container. */
    AutoPtr<ArrayOf<Byte> > mMacTag;

    /** Offset into file where authenticated (e.g., MAC protected) data begins. */
    Int64 mAuthenticatedDataStart;

    /** Offset into file where encrypted data begins. */
    Int64 mEncryptedDataStart;

    /**
     * Offset into file for the end of encrypted data (and, by extension,
     * authenticated data) in file.
     */
    Int64 mDataEnd;
};

}
}
}
}

#endif // __ELASTOS_DROID_CONTENT_PM_CCONTAINERENCRYPTIONPARAMS_H__
