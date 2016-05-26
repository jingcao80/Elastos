
#ifndef __CMONKEYSOURCENETWORKVIEWSHELPER_H__
#define __CMONKEYSOURCENETWORKVIEWSHELPER_H__

#include "_CMonkeySourceNetworkViewsHelper.h"
#include "MonkeySourceNetworkViewsHelper.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CarClass(CMonkeySourceNetworkViewsHelper), public MonkeySourceNetworkViewsHelper
{
public:

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI Setup();
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __CMONKEYSOURCENETWORKVIEWSHELPER_H__
