#ifndef __ELASTOS_DROID_SERVER_PM_DEXFILE_H__
#define __ELASTOS_DROID_SERVER_PM_DEXFILE_H__

#include "_Elastos.Droid.Server.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

class DexFile
{
public:
    /**
     * See {@link #isDexOptNeededInternal(String, String, String, boolean)}.
     *
     * @hide
     */
    static const Byte UP_TO_DATE = 0;

    /**
     * See {@link #isDexOptNeededInternal(String, String, String, boolean)}.
     *
     * @hide
     */
    static const Byte PATCHOAT_NEEDED = 1;

    /**
     * See {@link #isDexOptNeededInternal(String, String, String, boolean)}.
     *
     * @hide
     */
    static const Byte DEXOPT_NEEDED = 2;

    /**
     * Returns UP_TO_DATE if the VM believes that the apk/jar file
     * is up to date, PATCHOAT_NEEDED if it believes that the file is up
     * to date but it must be relocated to match the base address offset,
     * and DEXOPT_NEEDED if it believes that it is out of date and should
     * be passed through "dexopt" again.
     *
     * @param fileName the absolute path to the apk/jar file to examine.
     * @return DEXOPT_NEEDED if dexopt should be called on the file,
     *         PATCHOAT_NEEDED if we need to run "patchoat" on it and
     *         UP_TO_DATE otherwise.
     * @throws java.io.FileNotFoundException if fileName is not readable,
     *         not a file, or not present.
     * @throws java.io.IOException if fileName is not a valid apk/jar file or
     *         if problems occur while parsing it.
     * @throws java.lang.NullPointerException if fileName is null.
     * @throws dalvik.system.StaleDexCacheError if the optimized dex file
     *         is stale but exists on a read-only partition.
     *
     * @hide
     */
    static Byte IsDexOptNeededInternal(
        /* [in] */ const String& fileName,
        /* [in] */ const String& pkgname,
        /* [in] */ const String& instructionSet,
        /* [in] */ Boolean defer);
};

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_PM_DEXFILE_H__
