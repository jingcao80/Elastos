
#include "elastos/droid/server/pm/CLocalObserver.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Content::Pm::IPackageManagerHelper;
using Elastos::Droid::Content::Pm::CPackageManagerHelper;
using Elastos::Droid::Content::Pm::EIID_IIPackageInstallObserver2;
using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

CAR_INTERFACE_IMPL_2(CLocalObserver, Object, IIPackageInstallObserver2, IBinder)

CAR_OBJECT_IMPL(CLocalObserver)

ECode CLocalObserver::constructor(
    /* [in] */ IIPackageInstallerSession* host)
{
    mHost = (CPackageInstallerSession*)host;
    return NOERROR;
}

ECode CLocalObserver::OnUserActionRequired(
    /* [in] */ IIntent* intent)
{
    return E_ILLEGAL_STATE_EXCEPTION;
}

ECode CLocalObserver::OnPackageInstalled(
    /* [in] */ const String& basePackageName,
    /* [in] */ Int32 returnCode,
    /* [in] */ const String& msg,
    /* [in] */ IBundle* extras)
{
    mHost->DestroyInternal();
    mHost->DispatchSessionFinished(returnCode, msg, extras);
    return NOERROR;
}

ECode CLocalObserver::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    return Object::ToString(str);
}

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos
