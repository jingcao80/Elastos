
#ifndef __ELASTOS_DROID_NET_UIDRANGE_H__
#define __ELASTOS_DROID_NET_UIDRANGE_H__

#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * An inclusive range of UIDs.
 *
 * @hide
 */
class UidRange
    : public Object
    , public IParcelable
    , public IUidRange
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 startUid,
        /* [in] */ Int32 stopUid);

    static CARAPI CreateForUser(
        /* [in] */ Int32 userId,
        /* [out] */ IUidRange** result);

    CARAPI GetStartUser(
        /* [out] */ Int32* result);

    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetStart(
        /* [out] */ Int32* result);

    CARAPI GetStop(
        /* [out] */ Int32* result);

private:
    Int32 mStart;

    Int32 mStop;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_UIDRANGE_H__
