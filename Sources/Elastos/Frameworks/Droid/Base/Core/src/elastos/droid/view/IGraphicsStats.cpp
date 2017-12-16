
#include "elastos/droid/view/IGraphicsStats.h"
#include "elastos/droid/os/AndroidParcelUtils.h"
#include <binder/Parcel.h>

using Elastos::Droid::Os::AndroidParcelUtils;

namespace Elastos {
namespace Droid {
namespace View {

const String IGrphicsStatsProxy::TAG("IGrphicsStatsProxy");
const String IGrphicsStatsProxy::DESCRIPTOR("android.view.IGraphicsStats");
const Int32 IGrphicsStatsProxy::TRANSACTION_requestBufferForProcess = android::IBinder::FIRST_CALL_TRANSACTION + 0;

CAR_INTERFACE_IMPL(IGrphicsStatsProxy, Object, IIGraphicsStats)

IGrphicsStatsProxy::IGrphicsStatsProxy(
    /* [in] */ android::sp<android::IBinder> remote)
    : mRemote(remote)
{}

ECode IGrphicsStatsProxy::RequestBufferForProcess(
    /* [in] */ const String& packageName,
    /* [in] */ IIGraphicsStatsCallback* cb,
    /* [out] */ IParcelFileDescriptor** fd)
{
    VALIDATE_NOT_NULL(fd);

    android::Parcel data, reply;

    data.writeInterfaceToken(android::String16(DESCRIPTOR));
    AndroidParcelUtils::WriteString(data, packageName);
    AndroidParcelUtils::WriteIIGraphicsStatsCallback(data, cb);
    mRemote->transact(TRANSACTION_requestBufferForProcess, data, &reply, 0);
    ECode ec = reply.readExceptionCode() == 0 ? NOERROR : E_REMOTE_EXCEPTION;
    if (AndroidParcelUtils::ReadInt32(reply) != 0) {
        AutoPtr<IParcelFileDescriptor> result = AndroidParcelUtils::ReadParcelFileDescriptor(reply);
        *fd = result;
        REFCOUNT_ADD(*fd);
    }
    return ec;
}

} // namespace View
} // namespace Droid
} // namespace Elastos
