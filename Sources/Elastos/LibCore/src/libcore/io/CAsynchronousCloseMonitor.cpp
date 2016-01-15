
#include "Elastos.CoreLibrary.IO.h"
#include "CAsynchronousCloseMonitor.h"
#include "AsynchronousCloseMonitor.h"

namespace Libcore {
namespace IO {

CAR_SINGLETON_IMPL(CAsynchronousCloseMonitor)

CAR_INTERFACE_IMPL(CAsynchronousCloseMonitor, Singleton, IAsynchronousCloseMonitor)

ECode CAsynchronousCloseMonitor::SignalBlockedThreads(
    /* [in] */ IFileDescriptor* fd)
{
    Int32 iFd;
    fd->GetDescriptor(&iFd);
    AsynchronousCloseMonitor::SignalBlockedThreads(iFd);
    return NOERROR;
}

} // namespace IO
} // namespace Libcore