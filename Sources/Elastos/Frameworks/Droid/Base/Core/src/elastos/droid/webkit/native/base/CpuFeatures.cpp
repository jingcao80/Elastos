
#include "elastos/droid/webkit/native/base/CpuFeatures.h"
#include "elastos/droid/webkit/native/base/api/CpuFeatures_dec.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Base {

/**
 * Return the number of CPU Cores on the device.
 */
Int32 CpuFeatures::GetCount()
{
    return NativeGetCoreCount();
}

/**
 * Return the CPU feature mask.
 * This is a 64-bit integer that corresponds to the CPU's features.
 * The value comes directly from android_getCpuFeatures().
 */
Int64 CpuFeatures::GetMask()
{
    return NativeGetCpuFeatures();
}

Int32 CpuFeatures::NativeGetCoreCount()
{
    return Elastos_CpuFeatures_nativeGetCoreCount();
}

Int64 CpuFeatures::NativeGetCpuFeatures()
{
    return Elastos_CpuFeatures_nativeGetCpuFeatures();
}

} // namespace Base
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
