
#ifndef __ELASTOS_DROID_LOCATION_GPSNAVIGATIONMESSAGEEVENT_H__
#define __ELASTOS_DROID_LOCATION_GPSNAVIGATIONMESSAGEEVENT_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Location.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Location {

/**
 * A class implementing a container for data associated with a navigation message event.
 * Events are delivered to registered instances of {@link Listener}.
 *
 * @hide
 */
class GpsNavigationMessageEvent
    : public Object
    , public IGpsNavigationMessageEvent
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    GpsNavigationMessageEvent();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IGpsNavigationMessage* message);

    // @NonNull
    CARAPI GetNavigationMessage(
        /* [out] */ IGpsNavigationMessage** coll);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* parcel);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

private:
    AutoPtr<IGpsNavigationMessage> mNavigationMessage;
    const static String TAG;
};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif // __ELASTOS_DROID_LOCATION_GPSNAVIGATIONMESSAGEEVENT_H__
