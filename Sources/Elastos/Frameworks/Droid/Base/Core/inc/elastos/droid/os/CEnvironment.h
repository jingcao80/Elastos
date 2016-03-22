
#ifndef __ELASTOS_DROID_OS_CENVIRONMENT_H__
#define __ELASTOS_DROID_OS_CENVIRONMENT_H__

#include "_Elastos_Droid_Os_CEnvironment.h"
#include <elastos/core/Singleton.h>

using Elastos::IO::IFile;

namespace Elastos {
namespace Droid {
namespace Os {

/**
 * Provides access to environment variables.
 */
CarClass(CEnvironment)
    , public Singleton
    , public IEnvironment
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Return root of the "system" partition holding the core Android OS.
     * Always present and mounted read-only.
     */
    CARAPI GetRootDirectory(
        /* [out] */ IFile** dir);

    /**
     * Return root directory of the "oem" partition holding OEM customizations,
     * if any. If present, the partition is mounted read-only.
     *
     * @hide
     */
    CARAPI GetOemDirectory(
        /* [out] */ IFile** dir);

    /**
     * Return root directory of the "vendor" partition that holds vendor-provided
     * software that should persist across simple reflashing of the "system" partition.
     * @hide
     */
    CARAPI GetVendorDirectory(
        /* [out] */ IFile** dir);

    /**
     * Return the root directory for "prebundled" apps.  These apps will be installed directly
     * from this partition but will not be marked as system apps and will hence be uninstallable.
     * @hide
     */
     CARAPI GetPrebundledDirectory(
        /* [out] */ IFile** dir);

    /**
     * Gets the system directory available for secure storage.
     * If Encrypted File system is enabled, it returns an encrypted directory (/data/secure/system).
     * Otherwise, it returns the unencrypted /data/system directory.
     * @return File object representing the secure storage system directory.
     * @hide
     */
    CARAPI GetSystemSecureDirectory(
        /* [out] */ IFile** dir);

    /**
     * Gets the data directory for secure storage.
     * If Encrypted File system is enabled, it returns an encrypted directory (/data/secure).
     * Otherwise, it returns the unencrypted /data directory.
     * @return File object representing the data directory for secure storage.
     * @hide
     */
    CARAPI GetSecureDataDirectory(
        /* [out] */ IFile** dir);

    /**
     * Return directory used for internal media storage, which is protected by
     * {@link android.Manifest.permission#WRITE_MEDIA_STORAGE}.
     *
     * @hide
     */
    CARAPI GetMediaStorageDirectory(
        /* [out] */ IFile** dir);

    /**
     * Return the system directory for a user. This is for use by system services to store
     * files relating to the user. This directory will be automatically deleted when the user
     * is removed.
     *
     * @hide
     */
    CARAPI GetUserSystemDirectory(
        /* [in] */ Int32 userId,
        /* [out] */ IFile** dir);

    /**
     * Returns the config directory for a user. This is for use by system services to store files
     * relating to the user which should be readable by any app running as that user.
     *
     * @hide
     */
    CARAPI GetUserConfigDirectory(
        /* [in] */ Int32 userId,
        /* [out] */ IFile** dir);

    /**
     * Returns whether the Encrypted File System feature is enabled on the device or not.
     * @return <code>true</code> if Encrypted File System feature is enabled, <code>false</code>
     * if disabled.
     * @hide
     */
    CARAPI IsEncryptedFilesystemEnabled(
        /* [out] */ Boolean* result);

    /**
     * Return the user data directory.
     */
    CARAPI GetDataDirectory(
        /* [out] */ IFile** dir);

    /**
     * Return the primary external storage directory. This directory may not
     * currently be accessible if it has been mounted by the user on their
     * computer, has been removed from the device, or some other problem has
     * happened. You can determine its current state with
     * {@link #getExternalStorageState()}.
     * <p>
     * <em>Note: don't be confused by the word "external" here. This directory
     * can better be thought as media/shared storage. It is a filesystem that
     * can hold a relatively large amount of data and that is shared across all
     * applications (does not enforce permissions). Traditionally this is an SD
     * card, but it may also be implemented as built-in storage in a device that
     * is distinct from the protected internal storage and can be mounted as a
     * filesystem on a computer.</em>
     * <p>
     * On devices with multiple users (as described by {@link UserManager}),
     * each user has their own isolated external storage. Applications only have
     * access to the external storage for the user they're running as.
     * <p>
     * In devices with multiple "external" storage directories, this directory
     * represents the "primary" external storage that the user will interact
     * with. Access to secondary storage is available through
     * <p>
     * Applications should not directly use this top-level directory, in order
     * to avoid polluting the user's root namespace. Any files that are private
     * to the application should be placed in a directory returned by
     * {@link android.content.Context#getExternalFilesDir
     * Context.getExternalFilesDir}, which the system will take care of deleting
     * if the application is uninstalled. Other shared files should be placed in
     * one of the directories returned by
     * {@link #getExternalStoragePublicDirectory}.
     * <p>
     * Writing to this path requires the
     * {@link android.Manifest.permission#WRITE_EXTERNAL_STORAGE} permission,
     * and starting in read access requires the
     * {@link android.Manifest.permission#READ_EXTERNAL_STORAGE} permission,
     * which is automatically granted if you hold the write permission.
     * <p>
     * Starting in {@link android.os.Build.VERSION_CODES#KITKAT}, if your
     * application only needs to store internal data, consider using
     * {@link Context#getExternalFilesDir(String)} or
     * {@link Context#getExternalCacheDir()}, which require no permissions to
     * read or write.
     * <p>
     * This path may change between platform versions, so applications should
     * only persist relative paths.
     * <p>
     * Here is an example of typical code to monitor the state of external
     * storage:
     * <p>
     * {@sample
     * development/samples/ApiDemos/src/com/example/android/apis/content/ExternalStorage.java
     * monitor_storage}
     *
     * @see #getExternalStorageState()
     * @see #isExternalStorageRemovable()
     */
    CARAPI GetExternalStorageDirectory(
        /* [out] */ IFile** dir);

    /** {@hide} */
    CARAPI GetSecondaryStorageDirectory(
        /* [out] */ IFile** dir);

    /** {@hide} */
    CARAPI GetLegacyExternalStorageDirectory(
        /* [out] */ IFile** dir);

    /** {@hide} */
    CARAPI GetLegacyExternalStorageObbDirectory(
        /* [out] */ IFile** dir);

    /** {@hide} */
    CARAPI GetEmulatedStorageSource(
        /* [in] */ Int32 userId,
        /* [out] */ IFile** dir);

    /** {@hide} */
    CARAPI GetEmulatedStorageObbSource(
        /* [out] */ IFile** dir);

    /**
     * Get a top-level public external storage directory for placing files of
     * a particular type.  This is where the user will typically place and
     * manage their own files, so you should be careful about what you put here
     * to ensure you don't erase their files or get in the way of their own
     * organization.
     *
     * <p>Here is an example of typical code to manipulate a picture on
     * the public external storage:</p>
     *
     * {@sample development/samples/ApiDemos/src/com/example/android/apis/content/ExternalStorage.java
     * public_picture}
     *
     * @param type The type of storage directory to return.  Should be one of
     * {@link #DIRECTORY_MUSIC}, {@link #DIRECTORY_PODCASTS},
     * {@link #DIRECTORY_RINGTONES}, {@link #DIRECTORY_ALARMS},
     * {@link #DIRECTORY_NOTIFICATIONS}, {@link #DIRECTORY_PICTURES},
     * {@link #DIRECTORY_MOVIES}, {@link #DIRECTORY_DOWNLOADS}, or
     * {@link #DIRECTORY_DCIM}.  May not be null.
     *
     * @return Returns the File path for the directory.  Note that this
     * directory may not yet exist, so you must make sure it exists before
     * using it such as with {@link File#mkdirs File.mkdirs()}.
     */
    CARAPI GetExternalStoragePublicDirectory(
        /* [in] */ const String& type,
        /* [out] */ IFile** dir);

    /**
     * Returns the path for android-specific data on the SD card.
     * @hide
     */
    CARAPI BuildExternalStorageAndroidDataDirs(
        /* [out, callee] */ ArrayOf<IFile*>** files);

    /**
     * Generates the raw path to an application's data
     * @hide
     */
    CARAPI BuildExternalStorageAppDataDirs(
        /* [in] */ const String& packageName,
        /* [out, callee] */ ArrayOf<IFile*>** files);

    /**
     * Generates the raw path to an application's media
     * @hide
     */
    CARAPI BuildExternalStorageAppMediaDirs(
        /* [in] */ const String& packageName,
        /* [out, callee] */ ArrayOf<IFile*>** files);

    /**
     * Generates the raw path to an application's OBB files
     * @hide
     */
    CARAPI BuildExternalStorageAppObbDirs(
        /* [in] */ const String& packageName,
        /* [out, callee] */ ArrayOf<IFile*>** files);

    /**
     * Generates the path to an application's files.
     * @hide
     */
    CARAPI BuildExternalStorageAppFilesDirs(
        /* [in] */ const String& packageName,
        /* [out, callee] */ ArrayOf<IFile*>** files);

    /**
     * Generates the path to an application's cache.
     * @hide
     */
    CARAPI BuildExternalStorageAppCacheDirs(
        /* [in] */ const String& packageName,
        /* [out, callee] */ ArrayOf<IFile*>** files);

    /**
     * Gets the Android Download/Cache content directory.
     */
    CARAPI GetDownloadCacheDirectory(
        /* [out] */ IFile** dir);

     /**
     * Gets the current state of the primary "external" storage device.
     *
     * <p>See {@link #getExternalStorageDirectory()} for more information.
     */
    CARAPI GetExternalStorageState(
        /* [out] */ String* state);

    /**
     * @deprecated use {@link #getExternalStorageState(File)}
     */
    //@Deprecated
    CARAPI GetStorageState(
        /* [in] */ IFile* path,
        /* [out] */ String* state);

    /**
     * @hide
     */
    CARAPI GetSecondaryStorageState(
        /* [out] */ String* state);

    /**
     * Returns the current state of the storage device that provides the given
     * path.
     *
     * @return one of {@link #MEDIA_UNKNOWN}, {@link #MEDIA_REMOVED},
     *         {@link #MEDIA_UNMOUNTED}, {@link #MEDIA_CHECKING},
     *         {@link #MEDIA_NOFS}, {@link #MEDIA_MOUNTED},
     *         {@link #MEDIA_MOUNTED_READ_ONLY}, {@link #MEDIA_SHARED},
     *         {@link #MEDIA_BAD_REMOVAL}, or {@link #MEDIA_UNMOUNTABLE}.
     */
    CARAPI GetExternalStorageState(
        /* [in] */ IFile* path,
        /* [out] */ String* state);

    /**
     * Returns whether the primary "external" storage device is removable.
     * If true is returned, this device is for example an SD card that the
     * user can remove.  If false is returned, the storage is built into
     * the device and can not be physically removed.
     *
     * <p>See {@link #getExternalStorageDirectory()} for more information.
     */
    CARAPI IsExternalStorageRemovable(
        /* [in] */ IFile* path,
        /* [out] */ Boolean* isRemovable);

    /** {@hide} */
    CARAPI IsNoEmulatedStorageExist(
        /* [out] */ Boolean* isExist);

    /**
     * Returns whether the storage device that provides the given path is
     * removable.
     *
     * @return true if the storage device can be removed (such as an SD card),
     *         or false if the storage device is built in and cannot be
     *         physically removed.
     * @throws IllegalArgumentException if the path is not a valid storage
     *             device.
     */
    CARAPI IsExternalStorageRemovable(
        /* [out] */ Boolean* isRemovable);

    /**
     * Returns whether the device has an external storage device which is
     * emulated. If true, the device does not have real external storage, and the directory
     * returned by {@link #getExternalStorageDirectory()} will be allocated using a portion of
     * the internal storage system.
     *
     * <p>Certain system services, such as the package manager, use this
     * to determine where to install an application.
     *
     * <p>Emulated external storage may also be encrypted - see
     * {@link android.app.admin.DevicePolicyManager#setStorageEncryption(
     * android.content.ComponentName, boolean)} for additional details.
     */
    CARAPI IsExternalStorageEmulated(
        /* [out] */ Boolean* isEmulated);

    /**
     * Returns whether the storage device that provides the given path is
     * emulated. If true, data stored on this device will be stored on a portion
     * of the internal storage system.
     *
     * @throws IllegalArgumentException if the path is not a valid storage
     *             device.
     */
    CARAPI IsExternalStorageEmulated(
        /* [in] */ IFile* path,
        /* [out] */ Boolean* isEmulated);

    /** {@hide} */
    CARAPI SetUserRequired(
        /* [in] */ Boolean userRequired);

    /**
     * Append path segments to each given base path, returning result.
     *
     * @hide
     */
    CARAPI BuildPaths(
        /* [in] */ ArrayOf<IFile*>* base,
        /* [in] */ ArrayOf<String>* segments,
        /* [out, callee] */ ArrayOf<IFile*>** file);

    /**
     * Append path segments to given base path, returning result.
     *
     * @hide
     */
    CARAPI BuildPath(
        /* [in] */ IFile* base,
        /* [in] */ ArrayOf<String>* segments,
        /* [out] */ IFile** file);

    /**
     * If the given path exists on emulated external storage, return the
     * translated backing path hosted on internal storage. This bypasses any
     * emulation later, improving performance. This is <em>only</em> suitable
     * for read-only access.
     * <p>
     * Returns original path if given path doesn't meet these criteria. Callers
     * must hold {@link android.Manifest.permission#WRITE_MEDIA_STORAGE}
     * permission.
     *
     * @hide
     */
    CARAPI MaybeTranslateEmulatedPathToInternal(
        /* [in] */ IFile* path,
        /* [out] */ IFile** file);
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CENVIRONMENT_H__
