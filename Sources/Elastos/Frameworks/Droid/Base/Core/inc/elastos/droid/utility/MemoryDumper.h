
#ifndef __ELASTOS_DROID_UTILITY_MEMORY_DUMPER_H__
#define __ELASTOS_DROID_UTILITY_MEMORY_DUMPER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Math.h>

namespace Elastos {
namespace Droid {
namespace Utility {

/*
 * ref: http://elastos.org/elorg_common/gitbook_elastos/Elastos_Developer_Doc/elastos/memleakcheck.html
 * adb shell
 * chmod 777 /data/debug
 * adb shell setprop libc.debug.malloc 1
 * adb shell stop
 * adb shell start
 */
class ECO_PUBLIC MemoryDumper
{
public:
    static CARAPI Dump();

    static CARAPI Dump(
        /* [in] */ const String& filename);

private:
    ECO_LOCAL MemoryDumper();
    ECO_LOCAL MemoryDumper(const MemoryDumper&);
};


} // namespace Utility
} // namepsace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_UTILITY_MEMORY_DUMPER_H__