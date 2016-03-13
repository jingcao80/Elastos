
#include "elastos/droid/internal/os/CRuntimeInit.h"
#include "elastos/droid/internal/os/RuntimeInit.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

CAR_INTERFACE_IMPL(CRuntimeInit, Singleton, IRuntimeInit)

CAR_SINGLETON_IMPL(CRuntimeInit)

ECode CRuntimeInit::Main(
    /* [in] */ const ArrayOf<String>& args)
{
    return RuntimeInit::Main(args);
}

ECode CRuntimeInit::SetApplicationObject(
    /* [in] */ IBinder* app)
{
    return RuntimeInit::SetApplicationObject(app);
}

ECode CRuntimeInit::GetApplicationObject(
    /* [out] */ IBinder** app)
{
    return RuntimeInit::GetApplicationObject(app);
}

} // namespace Os
} // namespace Droid
} // namespace Internal
} // namespace Elastos
