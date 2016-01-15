#include "MonkeySourceNetworkViewsHelper.h"
#include "CMonkeySourceNetworkViews.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

ECode MonkeySourceNetworkViewsHelper::Setup()
{
    CMonkeySourceNetworkViews::Setup();
    return NOERROR;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos