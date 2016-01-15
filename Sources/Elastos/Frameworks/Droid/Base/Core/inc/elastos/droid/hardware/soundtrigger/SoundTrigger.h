
#ifndef __ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_SOUNDTRIGGER_H__
#define __ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_SOUNDTRIGGER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Hardware.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IHandler;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Soundtrigger {

class SoundTrigger
    : public Object
    , public ISoundTrigger
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Returns a list of descriptors for all harware modules loaded.
     * @param modules A ModuleProperties array where the list will be returned.
     * @return - {@link #STATUS_OK} in case of success
     *         - {@link #STATUS_ERROR} in case of unspecified error
     *         - {@link #STATUS_PERMISSION_DENIED} if the caller does not have system permission
     *         - {@link #STATUS_NO_INIT} if the native service cannot be reached
     *         - {@link #STATUS_BAD_VALUE} if modules is null
     *         - {@link #STATUS_DEAD_OBJECT} if the binder transaction to the native service fails
     */
    CARAPI NativeListModules(
        /* [in] */ IList* modules,
        /* [out] */ Int32* result);

    /**
     * Get an interface on a hardware module to control sound models and recognition on
     * this module.
     * @param moduleId Sound module system identifier {@link ModuleProperties#id}. mandatory.
     * @param listener {@link StatusListener} interface. Mandatory.
     * @param handler the Handler that will receive the callabcks. Can be null if default handler
     *                is OK.
     * @return a valid sound module in case of success or null in case of error.
     */
    CARAPI AttachModule(
        /* [in] */ Int32 moduleId,
        /* [in] */ ISoundTriggerStatusListener* listener,
        /* [in] */ IHandler* handler,
        /* [out] */ ISoundTriggerModule** _module);
};

} // namespace Soundtrigger
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_SOUNDTRIGGER_H__
