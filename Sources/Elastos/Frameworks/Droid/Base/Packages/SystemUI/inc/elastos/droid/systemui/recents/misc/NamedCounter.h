
#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_MISC_NAMEDCOUNTER_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_MISC_NAMEDCOUNTER_H__

#include "_Elastos.Droid.SystemUI.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Misc {

/**
 * Used to generate successive incremented names.
 */
class NamedCounter
    : public Object
{
public:
    NamedCounter(
        /* [in] */ const String& prefix,
        /* [in] */ const String& suffix);

    /** Returns the next name. */
    CARAPI_(String) NextName();

public:
    Int32 mCount;
    String mPrefix;
    String mSuffix;
};

} // namespace Misc
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_MISC_NAMEDCOUNTER_H__
