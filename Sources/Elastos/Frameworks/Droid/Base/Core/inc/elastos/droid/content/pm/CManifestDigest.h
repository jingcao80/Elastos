
#ifndef __ELASTOS_DROID_CONTENT_PM_CMANIFESTDIGEST_H__
#define __ELASTOS_DROID_CONTENT_PM_CMANIFESTDIGEST_H__

#include "_Elastos_Droid_Content_Pm_CManifestDigest.h"
#include <elastos/core/Object.h>

using Elastos::IO::IInputStream;
using Elastos::Utility::Jar::IAttributes;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CarClass(CManifestDigest)
    , public Object
    , public IManifestDigest
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    static CARAPI FromInputStream(
        /* [in] */ IInputStream* attributes,
        /* [out] */ IManifestDigest** digest);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* digest);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* code);

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
    static const String TAG;// = "ManifestDigest";

    /** The digest of the manifest in our preferred order. */
    AutoPtr<ArrayOf<Byte> > mDigest;

    /** What we print out first when toString() is called. */
    static const String TO_STRING_PREFIX;

    /** Digest algorithm to use. */
    static const String DIGEST_ALGORITHM;// = "SHA-256";
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_CMANIFESTDIGEST_H__
