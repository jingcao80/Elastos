//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_OS_CRECOVERYSYSTEM_H__
#define __ELASTOS_DROID_OS_CRECOVERYSYSTEM_H__

#include "_Elastos_Droid_Os_CRecoverySystem.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::IO::IFile;

namespace Elastos {
namespace Droid {
namespace Os {

/**
 * RecoverySystem contains methods for interacting with the Android
 * recovery system (the separate partition that can be used to install
 * system updates, wipe user data, etc.)
 */
CarClass(CRecoverySystem)
    , public Singleton
    , public IRecoverySystem
{
public:

    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

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
    CARAPI VerifyPackage(
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
    CARAPI InstallPackage(
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
    CARAPI RebootWipeUserData(
        /* [in] */ IContext* context);

    /** {@hide} */
    CARAPI RebootWipeUserData(
        /* [in] */ IContext* context,
        /* [in] */ const String& reason);

    /** {@hide} */
    CARAPI RebootWipeUserData(
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
    CARAPI RebootWipeUserData(
        /* [in] */ IContext* context,
        /* [in] */ Boolean shutdown,
        /* [in] */ const String& reason,
        /* [in] */ Boolean wipeMedia);

    /**
     * Reboot into the recovery system to wipe the /cache partition.
     * @throws IOException if something goes wrong.
     */
    CARAPI RebootWipeCache(
        /* [in] */ IContext* context);

    /** {@hide} */
    CARAPI RebootWipeCache(
        /* [in] */ IContext* context,
        /* [in] */ const String& reason);

    /**
     * Called after booting to process and remove recovery-related files.
     * @return the log file from recovery, or null if none was found.
     *
     * @hide
     */
    CARAPI HandleAftermath(
        /* [out] */ String* result);
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OS_CRECOVERYSYSTEM_H__
