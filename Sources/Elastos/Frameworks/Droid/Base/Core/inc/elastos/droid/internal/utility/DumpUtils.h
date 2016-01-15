#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_DUMPUTILS_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_DUMPUTILS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Runnable.h"

using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Runnable;
using Elastos::IO::IPrintWriter;
using Elastos::IO::IStringWriter;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

class DumpUtils
{
private:
    class DumpRunnable : public Runnable
    {
    public:
        DumpRunnable(
            /* [in] */ IDump* dump,
            /* [in] */ IStringWriter* sw);

        CARAPI Run();

    private:
        AutoPtr<IDump> mDump;
        AutoPtr<IStringWriter> mSw;
    };

public:
    /**
     * Helper for dumping state owned by a handler thread.
     *
     * Because the caller might be holding an important lock that the handler is
     * trying to acquire, we use a short timeout to avoid deadlocks.  The process
     * is inelegant but this function is only used for debugging purposes.
     */
    static CARAPI DumpAsync(
        /* [in] */ IHandler* handler,
        /* [in] */ IDump* dump,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ Int64 timeout);

private:
    DumpUtils();
};

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_DUMPUTILS_H__
