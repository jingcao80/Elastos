
#include "CMonkeySourceNetworkViewsHelper.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CAR_OBJECT_IMPL(CMonkeySourceNetworkViewsHelper)

ECode CMonkeySourceNetworkViewsHelper::Setup()
{
    // TODO: Add your code here
    MonkeySourceNetworkViewsHelper::Setup();
    return NOERROR;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos
