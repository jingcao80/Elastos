
#ifndef __ELASTOS_DROID_SYSTEM_CSTRUCTPOLLFD_H__
#define __ELASTOS_DROID_SYSTEM_CSTRUCTPOLLFD_H__

#include "Elastos.CoreLibrary.IO.h"
#include "core/Object.h"
#include "_Elastos_Droid_System_CStructPollfd.h"

using Elastos::IO::IFileDescriptor;

namespace Elastos {
namespace Droid {
namespace System {

/**
 * Corresponds to C's {@code struct pollfd} from
 * <a href="http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/poll.h.html">&lt;poll.h&gt;</a>
 */
CarClass(CStructPollfd)
    , public Object
    , public IStructPollfd
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    /** The file descriptor to poll. */
    CARAPI GetFd(
        /* [out] */ IFileDescriptor** fd);

    CARAPI SetFd(
        /* [in] */ IFileDescriptor* fd);

    /**
     * The events we're interested in. POLLIN corresponds to being in select(2)'s read fd set,
     * POLLOUT to the write fd set.
     */
    CARAPI GetEvents(
        /* [out] */ Int16* events);

    CARAPI SetEvents(
        /* [in] */ Int16 events);

    /** The events that actually happened. */
    CARAPI GetRevents(
        /* [out] */ Int16* revents);

    CARAPI SetRevents(
        /* [in] */ Int16 revents);

    /**
     * A non-standard extension that lets callers conveniently map back to the object
     * their fd belongs to. This is used by Selector, for example, to associate each
     * FileDescriptor with the corresponding SelectionKey.
     */
    CARAPI GetUserData(
        /* [out] */ IInterface** userData);

    CARAPI SetUserData(
        /* [in] */ IInterface* userData);

private:
    /** The file descriptor to poll. */
    AutoPtr<IFileDescriptor> mFd;

    /**
     * The events we're interested in. POLLIN corresponds to being in select(2)'s read fd set,
     * POLLOUT to the write fd set.
     */
    Int16 mEvents;

    /** The events that actually happened. */
    Int16 mRevents;

    /**
     * A non-standard extension that lets callers conveniently map back to the object
     * their fd belongs to. This is used by Selector, for example, to associate each
     * FileDescriptor with the corresponding SelectionKey.
     */
    AutoPtr<IInterface> mUserData;
};

} // namespace System
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEM_CSTRUCTPOLLFD_H__
