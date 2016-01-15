
#include "CMonkey.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

ECode CMonkey::Main(
    /* [in] */ const ArrayOf<String>& args)
{
    Monkey::Main(args);
    return NOERROR;
}

UInt32 CMonkey::AddRef()
{
    return _CMonkey::AddRef();
}

UInt32 CMonkey::Release()
{
    return _CMonkey::Release();
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos
