#ifndef  __ELASTOS_DROID_SERVICES_TELEPHONY_COMMON_AUDIOMODE_H__
#define  __ELASTOS_DROID_SERVICES_TELEPHONY_COMMON_AUDIOMODE_H__

#include "_Elastos.Droid.Server.Telephony.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include <elastos/core/StringBuffer.h>

using Elastos::Core::Object;
using Elastos::Core::StringBuffer;

namespace Elastos {
namespace Droid {
namespace Services {
namespace Telephony {
namespace Common {

/**
 * Container class for audio modes.
 */
class AudioMode
    : public Object
{
public:
    TO_STRING_IMPL("AudioMode")

    static CARAPI_(String) ToString(
        /* [in] */ Int32 mode);

private:
    static CARAPI_(void) ListAppend(
        /* [in] */ StringBuffer* buffer,
        /* [in] */ const String& str);

public:
    // These can be used as a bit mask
    static Int32 EARPIECE;
    static Int32 BLUETOOTH;
    static Int32 WIRED_HEADSET;
    static Int32 SPEAKER;

    static Int32 WIRED_OR_EARPIECE;
    static Int32 ALL_MODES;
};

} // namespace Common
} // namespace Telephony
} // namespace Services
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICES_TELEPHONY_COMMON_AUDIOMODE_H__