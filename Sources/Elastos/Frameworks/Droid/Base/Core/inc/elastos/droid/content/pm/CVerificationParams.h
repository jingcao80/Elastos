
#ifndef __ELASTOS_DROID_CONTENT_PM_CVERIFICATIONPARAMS_H__
#define __ELASTOS_DROID_CONTENT_PM_CVERIFICATIONPARAMS_H__

#include "_Elastos_Droid_Content_Pm_CVerificationParams.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

/**
 * Represents verification parameters used to verify packages to be installed.
 *
 * @hide
 */
CarClass(CVerificationParams)
    , public Object
    , public IVerificationParams
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CVerificationParams();

    ~CVerificationParams();

    CARAPI constructor();

    /**
     * Creates verification specifications for installing with application verification.
     *
     * @param verificationURI The location of the supplementary verification
     *            file. This can be a 'file:' or a 'content:' URI. May be {@code null}.
     * @param originatingURI URI referencing where the package was downloaded
     *            from. May be {@code null}.
     * @param referrer HTTP referrer URI associated with the originatingURI.
     *            May be {@code null}.
     * @param originatingUid UID of the application that the install request originated
     *            from, or NO_UID if not present
     * @param manifestDigest an object that holds the digest of the package
     *            which can be used to verify ownership. May be {@code null}.
     */
    CARAPI constructor(
        /* [in] */ Elastos::Droid::Net::IUri* verificationURI,
        /* [in] */ Elastos::Droid::Net::IUri* originatingURI,
        /* [in] */ Elastos::Droid::Net::IUri* referrer,
        /* [in] */ Int32 originatingUid,
        /* [in] */ Elastos::Droid::Content::Pm::IManifestDigest* manifestDigest);

    CARAPI GetVerificationURI(
        /* [out] */ Elastos::Droid::Net::IUri** uri);

    CARAPI GetOriginatingURI(
        /* [out] */ Elastos::Droid::Net::IUri** uri);

    CARAPI GetReferrer(
        /* [out] */ Elastos::Droid::Net::IUri** uri);

    CARAPI GetOriginatingUid(
        /* [out] */ Int32* uid);

    CARAPI GetManifestDigest(
        /* [out] */ Elastos::Droid::Content::Pm::IManifestDigest** digest);

    CARAPI GetInstallerUid(
        /* [out] */ Int32* uid);

    CARAPI SetInstallerUid(
        /* [in] */ Int32 uid);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* isEquals);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    CARAPI constructor(
        /* [in] */ IParcel* source);

private:
    /** The location of the supplementary verification file. */
    AutoPtr<Elastos::Droid::Net::IUri> mVerificationURI;

    /** URI referencing where the package was downloaded from. */
    AutoPtr<Elastos::Droid::Net::IUri> mOriginatingURI;

    /** HTTP referrer URI associated with the originatingURI. */
    AutoPtr<Elastos::Droid::Net::IUri> mReferrer;

    /** UID of the application that the install request originated from. */
    Int32 mOriginatingUid;

    /** UID of application requesting the install */
    Int32 mInstallerUid;

    /**
     * An object that holds the digest of the package which can be used to
     * verify ownership.
     */
    AutoPtr<IManifestDigest> mManifestDigest;

};

}
}
}
}

#endif // __ELASTOS_DROID_CONTENT_PM_CVERIFICATIONPARAMS_H__
