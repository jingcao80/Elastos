
#include "Elastos.CoreLibrary.IO.h"
#include "CStructPollfd.h"

namespace Elastos {
namespace Droid {
namespace System {

CAR_OBJECT_IMPL(CStructPollfd)

CAR_INTERFACE_IMPL(CStructPollfd, Object, IStructPollfd)

ECode CStructPollfd::GetFd(
    /* [out] */ IFileDescriptor** fd)
{
    VALIDATE_NOT_NULL(fd);

    *fd = mFd;
    REFCOUNT_ADD(*fd)
    return NOERROR;
}

ECode CStructPollfd::SetFd(
    /* [in] */ IFileDescriptor* fd)
{
    mFd = fd;
    return NOERROR;
}

ECode CStructPollfd::GetEvents(
    /* [out] */ Int16* events)
{
    VALIDATE_NOT_NULL(events);

    *events = mEvents;
    return NOERROR;
}

ECode CStructPollfd::SetEvents(
    /* [in] */ Int16 events)
{
    mEvents = events;
    return NOERROR;
}

ECode CStructPollfd::GetRevents(
    /* [out] */ Int16* revents)
{
    VALIDATE_NOT_NULL(revents);

    *revents = mRevents;
    return NOERROR;
}

ECode CStructPollfd::SetRevents(
    /* [in] */ Int16 revents)
{
    mRevents = revents;
    return NOERROR;
}

ECode CStructPollfd::GetUserData(
    /* [out] */ IInterface** userData)
{
    VALIDATE_NOT_NULL(userData);

    *userData = mUserData;
    REFCOUNT_ADD(*userData);
    return NOERROR;
}

ECode CStructPollfd::SetUserData(
    /* [in] */ IInterface* userData)
{
    mUserData = userData;
    return NOERROR;
}

} // namespace System
} // namespace Droid
} // namespace Elastos
