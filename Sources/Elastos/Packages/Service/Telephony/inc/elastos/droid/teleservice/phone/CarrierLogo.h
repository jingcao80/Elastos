#ifndef  __ELASTOS_DROID_PHONE_CARRIERLOGO_H__
#define  __ELASTOS_DROID_PHONE_CARRIERLOGO_H__

#include "_Elastos.Droid.TeleService.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

/**
 * Utility class to look up carrier logo resource IDs.
 */
class CarrierLogo
    : public Object
{
public:
    TO_STRING_IMPL("CarrierLogo")

    static CARAPI_(Int32) GetLogo(
        /* [in] */ const String& name);

private:
    /** This class is never instantiated. */
    CarrierLogo();

    static CARAPI_(AutoPtr<IMap>) GetLogoMap();

private:
    static AutoPtr<IMap> sLogoMap;

};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CARRIERLOGO_H__