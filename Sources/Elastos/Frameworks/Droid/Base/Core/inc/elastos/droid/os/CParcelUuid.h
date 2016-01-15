#ifndef __ELASTOS_DROID_OS_CPARCELUUID_H__
#define __ELASTOS_DROID_OS_CPARCELUUID_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "_Elastos_Droid_Os_CParcelUuid.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IUUID;

namespace Elastos {
namespace Droid {
namespace Os {

/**
 * This class is a Parcelable wrapper around {@link UUID} which is an
 * immutable representation of a 128-bit universally unique
 * identifier.
 */
CarClass(CParcelUuid)
    , public Object
    , public IParcelUuid
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CParcelUuid();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IUUID* pfd);

    /**
     * Creates a new ParcelUuid from a string representation of {@link UUID}.
     *
     * @param uuid
     *            the UUID string to parse.
     * @return a ParcelUuid instance.
     * @throws NullPointerException
     *             if {@code uuid} is {@code null}.
     * @throws IllegalArgumentException
     *             if {@code uuid} is not formatted correctly.
     */
    static CARAPI FromString(
        /* [in] */ const String& uuid,
        /* [out] */ IParcelUuid** id);

    CARAPI GetUuid(
        /* [out] */ IUUID** uuid);

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hash);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    AutoPtr<IUUID> mUuid;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OS_CPARCELUUID_H__
