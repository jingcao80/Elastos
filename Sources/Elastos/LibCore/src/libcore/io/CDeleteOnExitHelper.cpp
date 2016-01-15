
#include "CDeleteOnExitHelper.h"
#include "DeleteOnExit.h"

namespace Libcore {
namespace IO {

CAR_SINGLETON_IMPL(CDeleteOnExitHelper)

CAR_INTERFACE_IMPL(CDeleteOnExitHelper, Singleton, IDeleteOnExitHelper)

ECode CDeleteOnExitHelper::GetInstance(
    /* [out] */ IDeleteOnExit** instance)
{
    return DeleteOnExit::GetInstance(instance);
}

} // namespace IO
} // namespace Libcore

