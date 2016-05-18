
#ifndef __ELASTOS_DROID_COMMANDS_AM_CAM_H__
#define __ELASTOS_DROID_COMMANDS_AM_CAM_H__

#include "_Elastos_Droid_Commands_Am_CAm.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Am {

CarClass(CAm)
    , public Singleton
    , public IAm
{
public:
    CAR_SINGLETON_DECL();

    CAR_INTERFACE_DECL()

    /**
     * Command-line entry point.
     *
     * @param args The command-line arguments
     */
    CARAPI Main(
        /* [in] */ const ArrayOf<String>& args);
};

}
}
}
}

#endif // __ELASTOS_DROID_COMMANDS_AM_CAM_H__
