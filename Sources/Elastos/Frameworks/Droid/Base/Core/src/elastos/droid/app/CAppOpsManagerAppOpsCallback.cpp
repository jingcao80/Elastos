#include "elastos/droid/app/CAppOpsManagerAppOpsCallback.h"
// #include "elastos/droid/app/AppOpsManager.h"

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Internal::App::EIID_IIAppOpsCallback;

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL_2(CAppOpsManagerAppOpsCallback, Object, IIAppOpsCallback, IBinder)

CAR_OBJECT_IMPL(CAppOpsManagerAppOpsCallback)

ECode CAppOpsManagerAppOpsCallback::constructor(
        /* [in] */ IAppOpsManagerOnOpChangedListener* callback)
{
    mCallback = callback;
    return NOERROR;
}

ECode CAppOpsManagerAppOpsCallback::OpChanged(
    /* [in] */ Int32 op,
    /* [in] */ const String& packageName)
{
    IAppOpsManagerOnOpChangedInternalListener* internal = IAppOpsManagerOnOpChangedInternalListener::Probe(mCallback);
    if (internal) {
        internal->OnOpChanged(op, packageName);
    }

    assert(0 && "TODO");
    // if (!AppOpsManager::sOpToString[op].IsNull()) {
    //     mCallback->OnOpChanged(AppOpsManager::sOpToString[op], packageName);
    // }
    return NOERROR;
}

ECode CAppOpsManagerAppOpsCallback::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

} // namespace App
} // namespace Droid
} // namespace Elastos

