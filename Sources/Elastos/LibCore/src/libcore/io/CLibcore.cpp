
#include "CLibcore.h"
#include "CPosix.h"

namespace Libcore {
namespace IO {

static AutoPtr<IOs> InitsOs()
{
    AutoPtr<IOs> os;
    CPosix::New((IOs**)&os);
    return os;
}

INIT_PROI_1 AutoPtr<IOs> CLibcore::sOs = InitsOs();

CAR_SINGLETON_IMPL(CLibcore)

CAR_INTERFACE_IMPL(CLibcore, Singleton, ILibcore)

ECode CLibcore::GetOs(
    /* [out] */ IOs ** os)
{
    VALIDATE_NOT_NULL(os);

    *os = sOs;
    REFCOUNT_ADD(*os);

    return NOERROR;
}

} // namespace IO
} // namespace Libcore
