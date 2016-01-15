
#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include "Org.Conscrypt.h"

namespace Org {
namespace Conscrypt {

class Platform
{
public:
    /**
     * Runs all the setup for the platform that only needs to run once.
     */
    static CARAPI_(void) Setup();
};

} // namespace Conscrypt
} // namespace Org

#endif //__PLATFORM_H__
