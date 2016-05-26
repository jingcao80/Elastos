
#include "CMonkey.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CAR_OBJECT_IMPL(CMonkey)

ECode CMonkey::Main(
    /* [in] */ const ArrayOf<String>& args)
{
    Monkey::Main(args);
    return NOERROR;
}


} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos
