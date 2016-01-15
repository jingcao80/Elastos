#ifndef __ELASTOS_DROID_CONTENT_PM_KEYSET_H__
#define __ELASTOS_DROID_CONTENT_PM_KEYSET_H__

#include "Elastos.Droid.Content.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

/**
 * Represents a {@code KeySet} that has been declared in the AndroidManifest.xml
 * file for the application.  A {@code KeySet} can be used explicitly to
 * represent a trust relationship with other applications on the device.
 * @hide
 */
class KeySet
    : public Object
    , public IKeySet
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    KeySet(
        /* [in] */ IBinder* token);

    virtual ~KeySet();

    CARAPI GetToken(
        /* [out] */ IBinder** token);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hash);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    AutoPtr<IBinder> mToken;
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_KEYSET_H__
