#ifndef  __ELASTOS_DROID_PHONE_CADNLIST_H__
#define  __ELASTOS_DROID_PHONE_CADNLIST_H__

#include "_Elastos_Droid_TeleService_Phone_CADNList.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/teleservice/phone/ADNList.h"

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

/**
 * Abbreviated Dial Numbers (ADN) list activity for the Phone app. By default, this class will show
 * you all Service Dialing Numbers (SDN) that are supported by a service provider.  SDNs are a form
 * of speed dial for accessing service provider contacts like "#MIN" for getting user minutes.
 * To see this class in use, trigger the radio info screen by dialing *#*#INFO#*#* and open the
 * menu.
 * This class can also be used as a base class for simple contact lists that can be represented with
 * only labels and numbers.
 */
CarClass(CADNList)
    , public ADNList
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CADNLIST_H__