
#include "elastos/droid/location/GpsNavigationMessageEvent.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Location {

const String GpsNavigationMessageEvent::TAG("GpsNavigationMessageEvent");

CAR_INTERFACE_IMPL_2(GpsNavigationMessageEvent, Object, IGpsNavigationMessageEvent, IParcelable)

GpsNavigationMessageEvent::GpsNavigationMessageEvent()
{
}

ECode GpsNavigationMessageEvent::constructor(
    /* [in] */ IGpsNavigationMessage* message)
{
    if (message == NULL) {
        Logger::E(TAG, "Parameter 'message' must not be null.");
        return E_INVALID_PARAMETER_EXCEPTION;
    }
    mNavigationMessage = message;
    return NOERROR;
}

ECode GpsNavigationMessageEvent::GetNavigationMessage(
    /* [out] */ IGpsNavigationMessage** message)
{
    VALIDATE_NOT_NULL(message)
    *message = mNavigationMessage;
    REFCOUNT_ADD(*message)
    return NOERROR;
}

ECode GpsNavigationMessageEvent::ReadFromParcel(
    /* [in] */ IParcel* parcel)
{
    // ClassLoader classLoader = getClass().getClassLoader();
    // GpsNavigationMessage navigationMessage = in.readParcelable(classLoader);
    // return new GpsNavigationMessageEvent(navigationMessage);
    return E_NOT_IMPLEMENTED;
}

ECode GpsNavigationMessageEvent::WriteToParcel(
    /* [in] */ IParcel* parcel)
{
    // parcel.writeParcelable(mNavigationMessage, flags);
    return E_NOT_IMPLEMENTED;
}

ECode GpsNavigationMessageEvent::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    StringBuilder builder("[ GpsNavigationMessageEvent:\n\n");
    AutoPtr<IObject> o = IObject::Probe(mNavigationMessage);
    String str;
    o->ToString(&str);
    builder += str;
    builder += "\n]";
    *result = builder.ToString();
    return NOERROR;
}

}//namespace Location
}//namespace Droid
}//namespace Elastos
