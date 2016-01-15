
#include "Elastos.CoreLibrary.IO.h"
#include "DatagramSocketImpl.h"

namespace Elastos {
namespace Net {

CAR_INTERFACE_IMPL_2(DatagramSocketImpl, Object, IDatagramSocketImpl, ISocketOptions)

DatagramSocketImpl::DatagramSocketImpl()
    : mLocalPort(-1)
{}

ECode DatagramSocketImpl::GetFileDescriptor(
    /* [out] */ IFileDescriptor** fileDescriptor)
{
    VALIDATE_NOT_NULL(fileDescriptor);
    *fileDescriptor = mFd;
    REFCOUNT_ADD(*fileDescriptor);
    return NOERROR;
}

ECode DatagramSocketImpl::GetLocalPort(
    /* [out] */ Int32* port)
{
    VALIDATE_NOT_NULL(port);
    *port = mLocalPort;
    return NOERROR;
}

ECode DatagramSocketImpl::Connect(
    /* [in] */ IInetAddress* inetAddr,
    /* [in] */ Int32 port)
{
    // do nothing
    return NOERROR;
}

ECode DatagramSocketImpl::Disconnect()
{
    // do nothing
    return NOERROR;
}

} // namespace Net
} // namespace Elastos
