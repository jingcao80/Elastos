
#ifndef __ORG_ALLJOYN_BUS_VERSION_H__
#define __ORG_ALLJOYN_BUS_VERSION_H__

#include <elastos/coredef.h>

namespace Org {
namespace Alljoyn {
namespace Bus {

class ECO_PUBLIC Version
{
public:
    /**
     * Gives the version of AllJoyn Library
     *
     * @return the version of the AllJoyn Library
     */
    static CARAPI_(String) Get();

    /**
     * Gives build information of AllJoyn Library
     *
     * @return build information of the AllJoyn Library
     */
    static CARAPI_(String) GetBuildInfo();

    /**
     * Gives the version of AllJoyn Library as a single number
     *
     * @return the version of the AllJoyn Library as a single number
     */
    static CARAPI_(Int32) GetNumeric();

private:
    Version();
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_VERSION_H__
