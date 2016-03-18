#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_PHONEACCOUNTHANDLE_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_PHONEACCOUNTHANDLE_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Telecomm.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

/**
 * The unique identifier for a {@link PhoneAccount}.
 * @hide
 */
class PhoneAccountHandle
    : public Object
    , public IPhoneAccountHandle
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IComponentName* componentName,
        /* [in] */const String& id);

    /**
     * The {@code ComponentName} of the {@link android.telecom.ConnectionService} which is
     * responsible for making phone calls using this {@code PhoneAccountHandle}.
     *
     * @return A suitable {@code ComponentName}.
     */
    CARAPI GetComponentName(
        /* [out] */ IComponentName** result);

    /**
     * A string that uniquely distinguishes this particular {@code PhoneAccountHandle} from all the
     * others supported by the {@link ConnectionService} that created it.
     * <p>
     * A {@code ConnectionService} must select identifiers that are stable for the lifetime of
     * their users' relationship with their service, across many Android devices. For example, a
     * good set of identifiers might be the email addresses with which with users registered for
     * their accounts with a particular service. Depending on how a service chooses to operate,
     * a bad set of identifiers might be an increasing series of integers
     * ({@code 0}, {@code 1}, {@code 2}, ...) that are generated locally on each phone and could
     * collide with values generated on other phones or after a data wipe of a given phone.
     *
     * @return A service-specific unique identifier for this {@code PhoneAccountHandle}.
     */
    CARAPI GetId(
        /* [out] */ String* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* out);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* in);

private:
    AutoPtr<IComponentName> mComponentName;
    String mId;
};

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOMM_TELECOM_PHONEACCOUNTHANDLE_H__