
#include "CFileDescriptor.h"
#include "droid/system/OsConstants.h"
#include "CLibcore.h"
#include "unistd.h"
#include <sys/socket.h>
#include <sys/types.h>

using Elastos::Droid::System::OsConstants;
using Libcore::IO::ILibcore;
using Libcore::IO::CLibcore;
using Libcore::IO::IOs;

namespace Elastos {
namespace IO {

static AutoPtr<IFileDescriptor> CreateSTDIN()
{
    AutoPtr<CFileDescriptor> fd;
    CFileDescriptor::NewByFriend((CFileDescriptor**)&fd);
    fd->SetDescriptor(OsConstants::_STDIN_FILENO);
    return (IFileDescriptor*)fd.Get();
}

static AutoPtr<IFileDescriptor> CreateSTDOUT()
{
    AutoPtr<CFileDescriptor> fd;
    CFileDescriptor::NewByFriend((CFileDescriptor**)&fd);
    fd->SetDescriptor(OsConstants::_STDOUT_FILENO);
    return (IFileDescriptor*)fd.Get();
}

static AutoPtr<IFileDescriptor> CreateSTDERR()
{
    AutoPtr<CFileDescriptor> fd;
    CFileDescriptor::NewByFriend((CFileDescriptor**)&fd);
    fd->SetDescriptor(OsConstants::_STDERR_FILENO);
    return (IFileDescriptor*)fd.Get();
}

INIT_PROI_1 const AutoPtr<IFileDescriptor> CFileDescriptor::IN = CreateSTDIN();
INIT_PROI_1 const AutoPtr<IFileDescriptor> CFileDescriptor::OUT = CreateSTDOUT();
INIT_PROI_1 const AutoPtr<IFileDescriptor> CFileDescriptor::ERR = CreateSTDERR();

CFileDescriptor::CFileDescriptor()
    : mDescriptor(-1)
{}

CAR_OBJECT_IMPL(CFileDescriptor)

CAR_INTERFACE_IMPL(CFileDescriptor, Object, IFileDescriptor)

ECode CFileDescriptor::constructor()
{
    return NOERROR;
}

ECode CFileDescriptor::Sync()
{
    // try {
    AutoPtr<ILibcore> libcore;
    CLibcore::AcquireSingleton((ILibcore**)&libcore);
    AutoPtr<IOs> os;
    libcore->GetOs((IOs**)&os);
    Boolean isAtty;
    if (os->Isatty(this, &isAtty), isAtty) {
        return os->Tcdrain(this);
    }
    else {
        return os->Fsync(this);
    }
    // } catch (ErrnoException errnoException) {
    //     SyncFailedException sfe = new SyncFailedException(errnoException.getMessage());
    //     sfe.initCause(errnoException);
    //     throw sfe;
    // }
}

ECode CFileDescriptor::Valid(
    /* [out] */ Boolean* isValid)
{
    VALIDATE_NOT_NULL(isValid);

    *isValid = mDescriptor != -1;
    return NOERROR;
}

ECode CFileDescriptor::GetInt(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mDescriptor;
    return NOERROR;
}

ECode CFileDescriptor::GetDescriptor(
    /* [out] */ Int32* fd)
{
    VALIDATE_NOT_NULL(fd);
    *fd = mDescriptor;
    return NOERROR;
}

ECode CFileDescriptor::SetDescriptor(
    /* [in] */ Int32 fd)
{
    mDescriptor = fd;
    return NOERROR;
}

ECode CFileDescriptor::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = String("FileDescriptor[");
    str->AppendFormat("%d]", mDescriptor);

    return NOERROR;
}

ECode CFileDescriptor::IsSocket(
    /* [out] */ Boolean* isSocket)
{
    VALIDATE_NOT_NULL(isSocket)

    int error;
    socklen_t error_length = sizeof(error);
    *isSocket = TEMP_FAILURE_RETRY(getsockopt(mDescriptor, SOL_SOCKET, SO_ERROR, &error, &error_length));
    return NOERROR;
}

} // namespace IO
} // namespace Elastos
