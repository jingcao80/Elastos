#include "elastos/droid/systemui/recents/misc/NamedCounter.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Misc {

NamedCounter::NamedCounter(
    /* [in] */ const String& prefix,
    /* [in] */ const String& suffix)
    : mCount(0)
    , mPrefix(prefix)
    , mSuffix(suffix)
{
}

String NamedCounter::NextName()
{
    StringBuilder name(mPrefix);
    name += mCount;
    name += mSuffix;
    mCount++;
    return name.ToString();
}

} // namespace Misc
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos