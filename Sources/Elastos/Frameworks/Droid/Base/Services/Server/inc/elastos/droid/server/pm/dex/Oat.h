
#ifndef __ELASTOS_DROID_SERVER_PM_DEX_OAT_H__
#define __ELASTOS_DROID_SERVER_PM_DEX_OAT_H__

#include <elastos/coredef.h>

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {
namespace Dex {

class __attribute__ ((__aligned__(4), __packed__)) OatHeader
{
public:
    CARAPI_(Boolean) IsValid() const;

    CARAPI_(uint32_t) GetDexFileCount() const;

    CARAPI_(uint32_t) GetKeyValueStoreSize() const;
};

} // namespace Dex
} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_PM_DEX_OAT_H__
