
#ifndef __ELASTOS_DROID_UTILITY_CONFIG_H__
#define __ELASTOS_DROID_UTILITY_CONFIG_H__

#include <elastos.h>

using namespace Elastos;

namespace Elastos {
namespace Droid {
namespace Utility {

class Config
{
public:
    /**
     * If this is a debug build, this field will be true.
     */
    static const Boolean DEBUG = FALSE;

    /*
     * Deprecated fields
     * TODO: Remove platform references to these and @hide them.
     */

    /**
     * @deprecated Use {@link #DEBUG} instead.
     */
    static const Boolean RELEASE = TRUE;

    /**
     * @deprecated Always false.
     */
    static const Boolean PROFILE = FALSE;

    /**
     * @deprecated Always false.
     */
    static const Boolean LOGV = FALSE;

    /**
     * @deprecated Always true.
     */
    static const Boolean LOGD = TRUE;

private:
    Config();
};

}
}
}

#endif // __ELASTOS_DROID_UTILITY_CONFIG_H__
