#ifndef __ELASTOS_DROID_INTERNAL_OS_ANDROIDPRINTSTREAM_H__
#define __ELASTOS_DROID_INTERNAL_OS_ANDROIDPRINTSTREAM_H__

#include "elastos/droid/internal/os/LoggingPrintStream.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

class AndroidPrintStream
    : public LoggingPrintStream
{
public:
    /**
     * Constructs a new logging print stream.
     *
     * @param priority from {@link android.util.Log}
     * @param tag to log
     */
    AndroidPrintStream(
        /* [in] */ Int32 priority,
        /* [in] */ const String& tag);

protected:
    CARAPI_(void) Log(
        /* [in] */ const String& line);

private:
    const Int32 mPriority;
    const String mTag;
};

} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_OS_ANDROIDPRINTSTREAM_H__
