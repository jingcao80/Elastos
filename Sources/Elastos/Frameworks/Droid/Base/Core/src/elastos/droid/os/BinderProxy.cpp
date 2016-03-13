
#include "Elastos.Droid.Content.h"
#include "elastos/droid/os/BinderProxy.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <binder/IBinder.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL_2(BinderProxy, Object, IBinderProxy, IBinder)

BinderProxy::BinderProxy()
    : mObject(0)
{}

BinderProxy::~BinderProxy()
{
    Destroy();
}

ECode BinderProxy::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash);
    *hash = (Int32)this;
    return NOERROR;
}

ECode BinderProxy::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("BinderProxy:{");
    sb += StringUtils::ToHexString((Int32)this);
    sb += "}";
    *str = sb.ToString();
    return NOERROR;
}

void BinderProxy::Destroy()
{
    android::IBinder* b = (android::IBinder*)mObject;

    b->decStrong(this);
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
