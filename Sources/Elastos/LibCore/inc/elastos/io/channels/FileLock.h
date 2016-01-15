#ifndef __ELASTOS_IO_CHANNELS_FILELOCK_H__
#define __ELASTOS_IO_CHANNELS_FILELOCK_H__

#include "Elastos.CoreLibrary.IO.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::IO::ICloseable;

namespace Elastos {
namespace IO {
namespace Channels {

/**
 * A {@code FileLock} represents a locked region of a file.
 * <p>
 * Locks have certain properties that enable collaborating processes to avoid
 * the lost update problem or reading inconsistent data. Logically, a file lock
 * can be <em>exclusive</em> or <em>shared</em>. Multiple processes can hold
 * shared locks on the same region of a file, but only a single process can hold
 * an exclusive lock on a given region of a file and no other process can
 * simultaneously hold a shared lock overlapping the exclusive lock. An
 * application can determine whether a {@code FileLock} is shared or exclusive
 * via the {@code isShared()} method.
 * <p>
 * Locks held by a particular process cannot overlap one another. Applications
 * can determine whether a proposed lock will overlap by using the {@code
 * overlaps(long, long)}) method. Locks held in other processes may overlap
 * locks held in this process. Locks are shared amongst all threads in the
 * acquiring process, and are therefore unsuitable for intra-process
 * synchronization.
 * <p>
 * Once a lock is acquired, it is immutable in all its state except {@code
 * isValid()}. The lock will initially be valid, but may be rendered invalid by
 * explicit removal of the lock, using {@code release()}, or implicitly by
 * closing the channel or exiting the process (terminating the VM).
 * <h3>Platform dependencies</h3>
 * <p>
 * Locks are intended to be true platform operating system file locks, and
 * therefore locks held by the VM will be visible to other
 * operating system processes.
 * <p>
 * The characteristics of the underlying operating system locks will show
 * through in the Java implementation. For example, some platforms' locks are
 * 'mandatory' -- meaning the operating system enforces the locks on processes
 * that attempt to access locked regions of files; whereas other platforms'
 * locks are only 'advisory' -- meaning that processes are required to
 * collaborate to ensure locks are acquired and there is a potential for
 * processes to not play well. To be on the safe side, it is best to assume that
 * the platform is adopting advisory locks and always acquire shared locks when
 * reading a region of a file.
 * <p>
 * On some platforms, the presence of a lock will prevent the file from being
 * memory-mapped. On some platforms, closing a channel on a given file handle
 * will release all the locks held on that file -- even if there are other
 * channels open on the same file; their locks will also be released. The safe
 * option here is to ensure that you only acquire locks on a single channel for
 * a particular file and that becomes the synchronization point.
 * <p>
 * Further care should be exercised when locking files maintained on network
 * file systems, since they often have further limitations.
 */
class FileLock
    : public Object
    , public ICloseable
    , public IFileLock
{
public:
    CAR_INTERFACE_DECL()

    CARAPI Channel(
        /* [out] */ IFileChannel** channel);

    CARAPI GetPosition(
        /* [out] */ Int64* position);

    CARAPI GetSize(
        /* [out] */ Int64* size);

    CARAPI IsShared(
        /* [out] */ Boolean* shared);

    CARAPI Overlaps(
        /* [in] */ Int64 start,
        /* [in] */ Int64 length,
        /* [out] */ Boolean* result);

    virtual CARAPI IsValid(
        /* [out] */ Boolean* valid) = 0;

    virtual CARAPI ReleaseLock() = 0;

    CARAPI Close();

    CARAPI ToString(
        /* [out] */ String* string);

protected:
    FileLock(
        /* [in] */ IFileChannel* channel,
        /* [in] */ Int64 position,
        /* [in] */ Int64 size,
        /* [in] */ Boolean shared);

private:
    AutoPtr<IFileChannel> mChannel;
    Int64 mPosition;
    Int64 mSize;
    Boolean mShared;
};

} // namespace Channelss
} // namespace IO
} // namespace Elastos

#endif // #ifndef __ELASTOS_IO_CHANNELS_FILELOCK_H__
