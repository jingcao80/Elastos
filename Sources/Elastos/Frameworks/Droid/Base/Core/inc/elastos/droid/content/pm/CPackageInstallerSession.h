
#ifndef __ELASTOS_DROID_CONTENT_PM_CPACKAGE_INSTALLER_SESSION_H__
#define __ELASTOS_DROID_CONTENT_PM_CPACKAGE_INSTALLER_SESSION_H__

#include "_Elastos_Droid_Content_Pm_CPackageInstallerSession.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IIntentSender;

using Elastos::IO::ICloseable;
using Elastos::IO::IOutputStream;
using Elastos::IO::IInputStream;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

/**
 * An installation that is being actively staged. For an install to succeed,
 * all existing and new packages must have identical package names, version
 * codes, and signing certificates.
 * <p>
 * A session may contain any number of split packages. If the application
 * does not yet exist, this session must include a base package.
 * <p>
 * If an APK included in this session is already defined by the existing
 * installation (for example, the same split name), the APK in this session
 * will replace the existing APK.
 */
CarClass(CPackageInstallerSession)
    , public Object
    , public IPackageInstallerSession
    , public ICloseable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CPackageInstallerSession();

    virtual ~CPackageInstallerSession();

    CARAPI constructor();

    /** {@hide} */
    CARAPI constructor(
        /* [in] */ IIPackageInstallerSession* session);

    /** {@hide} */
    CARAPI SetProgress(
        /* [in] */ Float progress);

    /**
     * Set current progress of staging this session. Valid values are
     * anywhere between 0 and 1.
     * <p>
     * Note that this progress may not directly correspond to the value
     * reported by {@link SessionCallback#onProgressChanged(int, float)}, as
     * the system may carve out a portion of the overall progress to
     * represent its own internal installation work.
     */
    CARAPI SetStagingProgress(
        /* [in] */ Float progress);

    /** {@hide} */
    CARAPI AddProgress(
        /* [in] */ Float progress);

    /**
     * Open a stream to write an APK file into the session.
     * <p>
     * The returned stream will start writing data at the requested offset
     * in the underlying file, which can be used to resume a partially
     * written file. If a valid file length is specified, the system will
     * preallocate the underlying disk space to optimize placement on disk.
     * It's strongly recommended to provide a valid file length when known.
     * <p>
     * You can write data into the returned stream, optionally call
     * {@link #fsync(OutputStream)} as needed to ensure bytes have been
     * persisted to disk, and then close when finished. All streams must be
     * closed before calling {@link #commit(IntentSender)}.
     *
     * @param name arbitrary, unique name of your choosing to identify the
     *            APK being written. You can open a file again for
     *            additional writes (such as after a reboot) by using the
     *            same name. This name is only meaningful within the context
     *            of a single install session.
     * @param offsetBytes offset into the file to begin writing at, or 0 to
     *            start at the beginning of the file.
     * @param lengthBytes total size of the file being written, used to
     *            preallocate the underlying disk space, or -1 if unknown.
     *            The system may clear various caches as needed to allocate
     *            this space.
     * @throws IOException if trouble opening the file for writing, such as
     *             lack of disk space or unavailable media.
     * @throws SecurityException if called after the session has been
     *             committed or abandoned.
     */
    CARAPI OpenWrite(
        /* [in] */ const String& name,
        /* [in] */ Int64 offsetBytes,
        /* [in] */ Int64 lengthBytes,
        /* [out] */ IOutputStream** stream);

    /**
     * Ensure that any outstanding data for given stream has been committed
     * to disk. This is only valid for streams returned from
     * {@link #openWrite(String, long, long)}.
     */
    CARAPI Fsync(
        /* [in] */ IOutputStream* stream);

    /**
     * Return all APK names contained in this session.
     * <p>
     * This returns all names which have been previously written through
     * {@link #openWrite(String, long, long)} as part of this session.
     *
     * @throws SecurityException if called after the session has been
     *             committed or abandoned.
     */
    CARAPI GetNames(
        /* [out, callee] */ ArrayOf<String>** names);

    /**
     * Open a stream to read an APK file from the session.
     * <p>
     * This is only valid for names which have been previously written
     * through {@link #openWrite(String, long, long)} as part of this
     * session. For example, this stream may be used to calculate a
     * {@link MessageDigest} of a written APK before committing.
     *
     * @throws SecurityException if called after the session has been
     *             committed or abandoned.
     */
    CARAPI OpenRead(
        /* [in] */ const String& name,
        /* [out] */ IInputStream** stream);

    /**
     * Attempt to commit everything staged in this session. This may require
     * user intervention, and so it may not happen immediately. The final
     * result of the commit will be reported through the given callback.
     * <p>
     * Once this method is called, no additional mutations may be performed
     * on the session. If the device reboots before the session has been
     * finalized, you may commit the session again.
     *
     * @throws SecurityException if streams opened through
     *             {@link #openWrite(String, long, long)} are still open.
     */
    CARAPI Commit(
        /* [in] */ IIntentSender* statusReceiver);

    /**
     * Release this session object. You can open the session again if it
     * hasn't been finalized.
     */
    CARAPI Close();

    /**
     * Completely abandon this session, destroying all staged data and
     * rendering it invalid. Abandoned sessions will be reported to
     * {@link SessionCallback} listeners as failures. This is equivalent to
     * opening the session and calling {@link Session#abandon()}.
     */
    CARAPI Abandon();

private:
    AutoPtr<IIPackageInstallerSession> mSession;
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_CPACKAGE_INSTALLER_SESSION_H__
