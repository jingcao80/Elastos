
#ifndef __ELASTOS_DROID_CONTENT_PM_CVERIFIERINFO_H__
#define __ELASTOS_DROID_CONTENT_PM_CVERIFIERINFO_H__

#include "_Elastos_Droid_Content_Pm_CVerifierInfo.h"
#include <elastos/core/Object.h>

using Elastos::Security::IPublicKey;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

/**
 * Contains information about a package verifier as used by
 * {@code PackageManagerService} during package verification.
 *
 * @hide
 */
CarClass(CVerifierInfo)
    , public Object
    , public IVerifierInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CVerifierInfo();

    virtual ~CVerifierInfo();

    CARAPI constructor();

    /**
     * Creates an object that represents a verifier info object.
     *
     * @param packageName the package name in Java-style. Must not be {@code
     *            null} or empty.
     * @param publicKey the public key for the signer encoded in Base64. Must
     *            not be {@code null} or empty.
     * @throws IllegalArgumentException if either argument is null or empty.
     */
    CARAPI constructor(
        /* [in] */ const String& packageName,
        /* [in] */ IPublicKey* publicKey);

    CARAPI GetPackageName(
        /* [out] */ String* packageName);

    CARAPI SetPackageName(
        /* [in] */ const String& packageName);

    CARAPI GetPublicKey(
        /* [out] */ IPublicKey** publicKey);

    CARAPI SetPublicKey(
        /* [in] */ IPublicKey* publicKey);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

public:
    /** Package name of the verifier. */
    String mPackageName;

    /** Signatures used to sign the package verifier's package. */
    AutoPtr<IPublicKey> mPublicKey;

};

}
}
}
}

#endif // __ELASTOS_DROID_CONTENT_PM_CVERIFIERINFO_H__
