#ifndef __LIBCORE_UTILITY_CZONEINFOHELPER_H__
#define __LIBCORE_UTILITY_CZONEINFOHELPER_H__

#include "Singleton.h"
#include "_Libcore_Utility_CZoneInfoHelper.h"

using Libcore::IO::IBufferIterator;

namespace Libcore {
namespace Utility {

CarClass(CZoneInfoHelper)
    , public Singleton
    , public IZoneInfoHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI MakeTimeZone(
        /* [in] */ const String& id,
        /* [in] */ IBufferIterator* it,
        /* [out] */ IZoneInfo** zoneInfo);
};

} // namespace Utility
} // namespace Libcore
#endif