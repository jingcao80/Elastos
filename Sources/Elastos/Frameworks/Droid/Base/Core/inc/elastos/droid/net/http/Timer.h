
#ifndef __ELASTOS_DROID_NET_HTTP_TIMER_H__
#define __ELASTOS_DROID_NET_HTTP_TIMER_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

/**
 * {@hide}
 * Debugging tool
 */
class Timer
    : public Object
{
public:
    Timer();

    ~Timer();

    CARAPI constructor();

    CARAPI Mark(
        /* [in] */ const String& message);

private:
    Int64 mStart;

    Int64 mLast;
};

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_HTTP_TIMER_H__
