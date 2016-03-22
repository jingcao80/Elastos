#ifndef __ELASTOS_DROID_OS_RECOVERYSYSTEM_H__
#define __ELASTOS_DROID_OS_RECOVERYSYSTEM_H__

// #include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::IO::IFile;
using Elastos::Utility::IHashSet;

namespace Elastos {
namespace Droid {
namespace Os {

class ECO_PUBLIC RecoverySystem
{
private:
    class OpenConditionBroadcastReceiver : public BroadcastReceiver
    {
    public:
        OpenConditionBroadcastReceiver(
            /* [in] */ IConditionVariable* condition)
        {
            mCondition = condition;
        }

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);
    private:
        AutoPtr<IConditionVariable> mCondition;
    };
public:
    /**
     * Verify the cryptographic signature of a system update package
     * before installing it.  Note that the package is also verified
     * separately by the installer once the device is rebooted into
     * the recovery system.  This function will return only if the
     * package was successfully verified; otherwise it will throw an
     * exception.
     *
     * Verification of a package can take significant time, so this
     * function should not be called from a UI thread.  Interrupting
     * the thread while this function is in progress will result in a
     * SecurityException being thrown (and the thread's interrupt flag
     * will be cleared).
     *
     * @param packageFile  the package to be verified
     * @param listener     an object to receive periodic progress
     * updates as verification proceeds.  May be null.
     * @param deviceCertsZipFile  the zip file of certificates whose
     * public keys we will accept.  Verification succeeds if the
     * package is signed by the private key corresponding to any
     * public key in this file.  May be null to use the system default
     * file (currently "/system/etc/security/otacerts.zip").
     *
     * @throws IOException if there were any errors reading the
     * package or certs files.
     * @throws GeneralSecurityException if verification failed
     */
    static CARAPI VerifyPackage(
        /* [in] */ IFile* packageFile,
        /* [in] */ IRecoverySystemProgressListener* listener,
        /* [in] */ IFile* deviceCertsZipFile);

    /**
     * Reboots the device in order to install the given update
     * package.
     * Requires the {@link android.Manifest.permission#REBOOT} permission.
     *
     * @param context      the Context to use
     * @param packageFile  the update package to install.  Must be on
     * a partition mountable by recovery.  (The set of partitions
     * known to recovery may vary from device to device.  Generally,
     * /cache and /data are safe.)
     *
     * @throws IOException  if writing the recovery command file
     * fails, or if the reboot itself fails.
     */
    static CARAPI InstallPackage(
        /* [in] */ IContext* ctx,
        /* [in] */ IFile* packageFile);

    /**
     * Reboots the device and wipes the user data and cache
     * partitions.  This is sometimes called a "factory reset", which
     * is something of a misnomer because the system partition is not
     * restored to its factory state.  Requires the
     * {@link android.Manifest.permission#REBOOT} permission.
     *
     * @param context  the Context to use
     *
     * @throws IOException  if writing the recovery command file
     * fails, or if the reboot itself fails.
     * @throws SecurityException if the current user is not allowed to wipe data.
     */
    static CARAPI RebootWipeUserData(
        /* [in] */ IContext* context);

    /** {@hide} */
    static CARAPI RebootWipeUserData(
        /* [in] */ IContext* context,
        /* [in] */ String reason);

    /** {@hide} */
    static CARAPI RebootWipeUserData(
        /* [in] */ IContext* context,
        /* [in] */ Boolean shutdown);

    /**
     * Reboots the device and wipes the user data and cache
     * partitions.  This is sometimes called a "factory reset", which
     * is something of a misnomer because the system partition is not
     * restored to its factory state.  Requires the
     * {@link android.Manifest.permission#REBOOT} permission.
     *
     * @param context   the Context to use
     * @param shutdown  if true, the device will be powered down after
     *                  the wipe completes, rather than being rebooted
     *                  back to the regular system.
     *
     * @throws IOException  if writing the recovery command file
     * fails, or if the reboot itself fails.
     * @throws SecurityException if the current user is not allowed to wipe data.
     *
     * @hide
     */
    static CARAPI RebootWipeUserData(
        /* [in] */ IContext* context,
        /* [in] */ Boolean shutdown,
        /* [in] */ String reason,
        /* [in] */ Boolean wipeMedia);

    /**
     * Reboot into the recovery system to wipe the /cache partition.
     * @throws IOException if something goes wrong.
     */
    static CARAPI RebootWipeCache(
        /* [in] */ IContext* context);

    /** {@hide} */
    static CARAPI RebootWipeCache(
        /* [in] */ IContext* context,
        /* [in] */ String reason);

    /**
     * Called after booting to process and remove recovery-related files.
     * @return the log file from recovery, or null if none was found.
     *
     * @hide
     */
    static String HandleAftermath();

private:
    ECO_LOCAL RecoverySystem();

    /** @return the set of certs that can be used to sign an OTA package. */
    ECO_LOCAL static AutoPtr<IHashSet> GetTrustedCerts(
        /* [in] */ IFile* keystore);

    /**
     * Reboot into the recovery system with the supplied argument.
     * @param args to pass to the recovery utility.
     * @throws IOException if something goes wrong.
     */
    ECO_LOCAL static ECode BootCommand(
        /* [in] */ IContext* context, Int32 count, ...);

    /**
     * Internally, recovery treats each line of the command file as a separate
     * argv, so we only need to protect against newlines and nulls.
     */
    ECO_LOCAL static String SanitizeArg(
        /* [in] */ String arg);

public:
    static const String TAG; //= "RecoverySystem";

    /**
     * Default location of zip file containing public keys (X509
     * certs) authorized to sign OTA updates.
     */
    static const AutoPtr<IFile> DEFAULT_KEYSTORE; // = new File("/system/etc/security/otacerts.zip");

    /** Send progress to listeners no more often than this (in ms). */
    static const Int64 PUBLISH_PROGRESS_INTERVAL_MS; // = 500;

    /** Used to communicate with recovery.  See bootable/recovery/recovery.c. */
    static AutoPtr<IFile> RECOVERY_DIR; // = new File("/cache/recovery");
    static AutoPtr<IFile> COMMAND_FILE; // = new File(RECOVERY_DIR, "command");
    static AutoPtr<IFile> LOG_FILE; // = new File(RECOVERY_DIR, "log");
    static String LAST_PREFIX; // = "last_";

    // Length limits for reading files.
    static Int32 LOG_FILE_MAX_LENGTH; // = 64 * 1024;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OS_RECOVERYSYSTEM_H__
