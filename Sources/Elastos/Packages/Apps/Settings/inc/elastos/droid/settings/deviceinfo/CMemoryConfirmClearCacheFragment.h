#ifndef __ELASTOS_DROID_SETTINGS_DEVICEINFO_CMEMORYCONFIRMCLEARCACHEFRAGMENT_H__
#define __ELASTOS_DROID_SETTINGS_DEVICEINFO_CMEMORYCONFIRMCLEARCACHEFRAGMENT_H__

#include "_Elastos_Droid_Settings_Deviceinfo_CMemoryConfirmClearCacheFragment.h"
#include "elastos/droid/settings/deviceinfo/CMemory.h"

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Deviceinfo {

CarClass(CMemoryConfirmClearCacheFragment)
    , public CMemory::ConfirmClearCacheFragment
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Deviceinfo
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_DEVICEINFO_CMEMORYCONFIRMCLEARCACHEFRAGMENT_H__