#ifndef __ELASTOS_DROID_OS_ENVIRONMENT_H__
#define __ELASTOS_DROID_OS_ENVIRONMENT_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include "Elastos.Droid.Os.h"

using Elastos::IO::IFile;
using Elastos::Droid::Os::Storage::IStorageVolume;

namespace Elastos {
namespace Droid {
namespace Os {

/**
 * Provides access to environment variables.
 */
class ECO_PUBLIC Environment
{
public:

    /** {@hide} */
    class UserEnvironment
        : public Object
        , public IUserEnvironment
    {
    public:
        CAR_INTERFACE_DECL()

        UserEnvironment();

        virtual ~UserEnvironment();

        // TODO: generalize further to create package-specific environment
        CARAPI constructor(
            /* [in] */ Int32 useid);

        //@Deprecated
        CARAPI GetExternalStorageDirectory(
            /* [out] */ IFile** file);

        //@Deprecated
        CARAPI GetExternalStoragePublicDirectory(
            /* [in] */ const String& type,
            /* [out] */ IFile** file);

        CARAPI GetExternalDirsForVold(
            /* [out, callee] */ ArrayOf<IFile*>** files);

        CARAPI GetExternalDirsForApp(
            /* [out, callee] */ ArrayOf<IFile*>** files);

        CARAPI GetMediaDir(
            /* [out] */ IFile** file);

        CARAPI BuildExternalStoragePublicDirs(
            /* [in] */ const String& type,
            /* [out, callee] */ ArrayOf<IFile*>** files);

        CARAPI BuildExternalStorageAndroidDataDirs(
            /* [out, callee] */ ArrayOf<IFile*>** files);

        CARAPI BuildExternalStorageAndroidObbDirs(
            /* [out, callee] */ ArrayOf<IFile*>** files);

        CARAPI BuildExternalStorageAppDataDirs(
            /* [in] */ const String& packageName,
            /* [out, callee] */ ArrayOf<IFile*>** files);

        CARAPI BuildExternalStorageAppDataDirsForVold(
            /* [in] */ const String& packageName,
            /* [out, callee] */ ArrayOf<IFile*>** files);

        CARAPI BuildExternalStorageAppMediaDirs(
            /* [in] */ const String& packageName,
            /* [out, callee] */ ArrayOf<IFile*>** files);

        CARAPI BuildExternalStorageAppMediaDirsForVold(
            /* [in] */ const String& packageName,
            /* [out, callee] */ ArrayOf<IFile*>** files);

        CARAPI BuildExternalStorageAppObbDirs(
            /* [in] */ const String& packageName,
            /* [out, callee] */ ArrayOf<IFile*>** files);

        CARAPI BuildExternalStorageAppObbDirsForVold(
            /* [in] */ const String& packageName,
            /* [out, callee] */ ArrayOf<IFile*>** files);

        CARAPI BuildExternalStorageAppFilesDirs(
            /* [in] */ const String& packageName,
            /* [out, callee] */ ArrayOf<IFile*>** files);

        CARAPI BuildExternalStorageAppCacheDirs(
            /* [in] */ const String& packageName,
            /* [out, callee] */ ArrayOf<IFile*>** files);

    private:
        /** External storage dirs, as visible to vold */
        AutoPtr<ArrayOf<IFile*> > mExternalDirsForVold;
        /** External storage dirs, as visible to apps */
        AutoPtr<ArrayOf<IFile*> > mExternalDirsForApp;
        /** Primary emulated storage dir for direct access */
        AutoPtr<IFile> mEmulatedDirForDirect;
    };

public:

    /** {@hide} */
    static CARAPI InitForCurrentUser();

    /**
     * Return root of the "system" partition holding the core Android OS.
     * Always present and mounted read-only.
     */
    static AutoPtr<IFile> GetRootDirectory();

    /**
     * Return root directory of the "oem" partition holding OEM customizations,
     * if any. If present, the partition is mounted read-only.
     *
     * @hide
     */
    static AutoPtr<IFile> GetOemDirectory();

    /**
     * Return root directory of the "vendor" partition that holds vendor-provided
     * software that should persist across simple reflashing of the "system" partition.
     * @hide
     */
    static AutoPtr<IFile> GetVendorDirectory();

    /**
     * Gets the system directory available for secure storage.
     * If Encrypted File system is enabled, it returns an encrypted directory (/data/secure/system).
     * Otherwise, it returns the unencrypted /data/system directory.
     * @return File object representing the secure storage system directory.
     * @hide
     */
    static AutoPtr<IFile> GetSystemSecureDirectory();

    /**
     * Gets the data directory for secure storage.
     * If Encrypted File system is enabled, it returns an encrypted directory (/data/secure).
     * Otherwise, it returns the unencrypted /data directory.
     * @return File object representing the data directory for secure storage.
     * @hide
     */
    static AutoPtr<IFile> GetSecureDataDirectory();

    /**
     * Return directory used for internal media storage, which is protected by
     * {@link android.Manifest.permission#WRITE_MEDIA_STORAGE}.
     *
     * @hide
     */
    static AutoPtr<IFile> GetMediaStorageDirectory();

    /**
     * Return the system directory for a user. This is for use by system services to store
     * files relating to the user. This directory will be automatically deleted when the user
     * is removed.
     *
     * @hide
     */
    static AutoPtr<IFile> GetUserSystemDirectory(
        /* [in] */ Int32 userId);

    /**
     * Returns the config directory for a user. This is for use by system services to store files
     * relating to the user which should be readable by any app running as that user.
     *
     * @hide
     */
    static AutoPtr<IFile> GetUserConfigDirectory(
        /* [in] */ Int32 userId);

    /**
     * Returns whether the Encrypted File System feature is enabled on the device or not.
     * @return <code>true</code> if Encrypted File System feature is enabled, <code>false</code>
     * if disabled.
     * @hide
     */
    static Boolean IsEncryptedFilesystemEnabled();

    /**
     * Return the user data directory.
     */
    static AutoPtr<IFile> GetDataDirectory();

    /**
     * Return the primary external storage directory. This directory may not
     * currently be accessible if it has been mounted by the user on their
     * computer, has been removed from the device, or some other problem has
     * happened. You can determine its current state with
     * {@link #GetExternalStorageState()}.
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
     * {@link android.content.Context#GetExternalFilesDir
     * Context.GetExternalFilesDir}, which the system will take care of deleting
     * if the application is uninstalled. Other shared files should be placed in
     * one of the directories returned by
     * {@link #GetExternalStoragePublicDirectory}.
     * <p>
     * Writing to this path requires the
     * {@link android.Manifest.permission#WRITE_EXTERNAL_STORAGE} permission,
     * and starting in read access requires the
     * {@link android.Manifest.permission#READ_EXTERNAL_STORAGE} permission,
     * which is automatically granted if you hold the write permission.
     * <p>
     * Starting in {@link android.os.Build.VERSION_CODES#KITKAT}, if your
     * application only needs to store internal data, consider using
     * {@link Context#GetExternalFilesDir(String)} or
     * {@link Context#GetExternalCacheDir()}, which require no permissions to
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
     * @see #GetExternalStorageState()
     * @see #IsExternalStorageRemovable()
     */
    static AutoPtr<IFile> GetExternalStorageDirectory();

    /** {@hide} */
    static AutoPtr<IFile> GetLegacyExternalStorageDirectory();

    /** {@hide} */
    static AutoPtr<IFile> GetLegacyExternalStorageObbDirectory();

    /** {@hide} */
    static AutoPtr<IFile> GetEmulatedStorageSource(
        /* [in] */ Int32 userId);

    /** {@hide} */
    static AutoPtr<IFile> GetEmulatedStorageObbSource();

    /**
     * Get a top-level external storage directory for placing files of
     * a particular type.  This is where the user will typically place and
     * manage their own files, so you should be careful about what you put here
     * to ensure you don't erase their files or get in the way of their own
     * organization.
     *
     * <p>On devices with multiple users (as described by {@link UserManager}),
     * each user has their own isolated external storage. Applications only
     * have access to the external storage for the user they're running as.</p>
     *
     * <p>Here is an example of typical code to manipulate a picture on
     * the external storage:</p>
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
    static AutoPtr<IFile> GetExternalStoragePublicDirectory(
        /* [in] */ const String& type);

    /**
     * Returns the path for android-specific data on the SD card.
     * @hide
     */
    static AutoPtr<ArrayOf<IFile*> > BuildExternalStorageAndroidDataDirs();

    /**
     * Generates the raw path to an application's data
     * @hide
     */
    static AutoPtr<ArrayOf<IFile*> > BuildExternalStorageAppDataDirs(
        /* [in] */ const String& packageName);

    /**
     * Generates the raw path to an application's media
     * @hide
     */
    static AutoPtr<ArrayOf<IFile*> > BuildExternalStorageAppMediaDirs(
        /* [in] */ const String& packageName);

    /**
     * Generates the raw path to an application's OBB files
     * @hide
     */
    static AutoPtr<ArrayOf<IFile*> > BuildExternalStorageAppObbDirs(
        /* [in] */ const String& packageName);

    /**
     * Generates the path to an application's files.
     * @hide
     */
    static AutoPtr<ArrayOf<IFile*> > BuildExternalStorageAppFilesDirs(
        /* [in] */ const String& packageName);

    /**
     * Generates the path to an application's cache.
     * @hide
     */
    static AutoPtr<ArrayOf<IFile*> > BuildExternalStorageAppCacheDirs(
        /* [in] */ const String& packageName);

    /**
     * Return the download/cache content directory.
     */
    static AutoPtr<IFile> GetDownloadCacheDirectory() ;

    /**
     * Returns the current state of the primary "external" storage device.
     *
     * @see #GetExternalStorageDirectory()
     * @return one of {@link #MEDIA_UNKNOWN}, {@link #MEDIA_REMOVED},
     *         {@link #MEDIA_UNMOUNTED}, {@link #MEDIA_CHECKING},
     *         {@link #MEDIA_NOFS}, {@link #MEDIA_MOUNTED},
     *         {@link #MEDIA_MOUNTED_READ_ONLY}, {@link #MEDIA_SHARED},
     *         {@link #MEDIA_BAD_REMOVAL}, or {@link #MEDIA_UNMOUNTABLE}.
     */
    static String GetExternalStorageState();

    /**
     * @deprecated use {@link #GetExternalStorageState(File)}
     */
    //@Deprecated
    static String GetStorageState(
        /* [in] */ IFile* path);

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
    static String GetExternalStorageState(
        /* [in] */ IFile* path);

    /**
     * Returns whether the primary "external" storage device is removable.
     *
     * @return true if the storage device can be removed (such as an SD card),
     *         or false if the storage device is built in and cannot be
     *         physically removed.
     */
    static Boolean IsExternalStorageRemovable();

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
    static Boolean IsExternalStorageRemovable(
        /* [in] */ IFile* path);

    /**
     * Returns whether the primary "external" storage device is emulated. If
     * true, data stored on this device will be stored on a portion of the
     * internal storage system.
     *
     * @see DevicePolicyManager#setStorageEncryption(android.content.ComponentName,
     *      Boolean)
     */
    static Boolean IsExternalStorageEmulated();

    /**
     * Returns whether the storage device that provides the given path is
     * emulated. If true, data stored on this device will be stored on a portion
     * of the internal storage system.
     *
     * @throws IllegalArgumentException if the path is not a valid storage
     *             device.
     */
    static Boolean IsExternalStorageEmulated(
        /* [in] */ IFile* path);

    static AutoPtr<IFile> GetDirectory(
        /* [in] */ const String& variableName,
        /* [in] */ const String& defaultPath);

    /** {@hide} */
    static CARAPI SetUserRequired(
        /* [in] */ Boolean userRequired);

    /**
     * Append path segments to each given base path, returning result.
     *
     * @hide
     */
    static AutoPtr<ArrayOf<IFile*> > BuildPaths(
        /* [in] */ ArrayOf<IFile*>* base,
        /* [in] */ ArrayOf<String>* segments);

    static CARAPI BuildPaths(
        /* [in] */ ArrayOf<IFile*>* base,
        /* [in] */ ArrayOf<String>* segments,
        /* [out, callee] */ ArrayOf<IFile*>** files);

    /**
     * Append path segments to given base path, returning result.
     *
     * @hide
     */
    static AutoPtr<IFile> BuildPath(
        /* [in] */ IFile* base,
        /* [in] */ ArrayOf<String>* segments);

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
    static AutoPtr<IFile> MaybeTranslateEmulatedPathToInternal(
        /* [in] */ IFile* path);

private:

    ECO_LOCAL static CARAPI ThrowIfUserRequired();

    ECO_LOCAL static Boolean IsStorageDisabled();

    ECO_LOCAL static AutoPtr<IStorageVolume> GetStorageVolume(
        /* [in] */ IFile* path);

public:

    /**
     * Unknown storage state, such as when a path isn't backed by known storage
     * media.
     *
     * @see #GetExternalStorageState(File)
     */
    static const String MEDIA_UNKNOWN; // = "unknown";

    /**
     * Storage state if the media is not present.
     *
     * @see #GetExternalStorageState(File)
     */
    static const String MEDIA_REMOVED; // = "removed";

    /**
     * Storage state if the media is present but not mounted.
     *
     * @see #GetExternalStorageState(File)
     */
    static const String MEDIA_UNMOUNTED; // = "unmounted";

    /**
     * Storage state if the media is present and being disk-checked.
     *
     * @see #GetExternalStorageState(File)
     */
    static const String MEDIA_CHECKING; // = "checking";

    /**
     * Storage state if the media is present but is blank or is using an
     * unsupported filesystem.
     *
     * @see #GetExternalStorageState(File)
     */
    static const String MEDIA_NOFS; // = "nofs";

    /**
     * Storage state if the media is present and mounted at its mount point with
     * read/write access.
     *
     * @see #GetExternalStorageState(File)
     */
    static const String MEDIA_MOUNTED; // = "mounted";

    /**
     * Storage state if the media is present and mounted at its mount point with
     * read-only access.
     *
     * @see #GetExternalStorageState(File)
     */
    static const String MEDIA_MOUNTED_READ_ONLY; // = "mounted_ro";

    /**
     * Storage state if the media is present not mounted, and shared via USB
     * mass storage.
     *
     * @see #GetExternalStorageState(File)
     */
    static const String MEDIA_SHARED; // = "shared";

    /**
     * Storage state if the media was removed before it was unmounted.
     *
     * @see #GetExternalStorageState(File)
     */
    static const String MEDIA_BAD_REMOVAL; // = "bad_removal";

    /**
     * Storage state if the media is present but cannot be mounted. Typically
     * this happens if the file system on the media is corrupted.
     *
     * @see #GetExternalStorageState(File)
     */
    static const String MEDIA_UNMOUNTABLE; // = "unmountable";


    /**
     * Standard directory in which to place any audio files that should be
     * in the regular list of music for the user.
     * This may be combined with
     * {@link #DIRECTORY_PODCASTS}, {@link #DIRECTORY_NOTIFICATIONS},
     * {@link #DIRECTORY_ALARMS}, and {@link #DIRECTORY_RINGTONES} as a series
     * of directories to categories a particular audio file as more than one
     * type.
     */
    static String DIRECTORY_MUSIC; // = "Music";

    /**
     * Standard directory in which to place any audio files that should be
     * in the list of podcasts that the user can select (not as regular
     * music).
     * This may be combined with {@link #DIRECTORY_MUSIC},
     * {@link #DIRECTORY_NOTIFICATIONS},
     * {@link #DIRECTORY_ALARMS}, and {@link #DIRECTORY_RINGTONES} as a series
     * of directories to categories a particular audio file as more than one
     * type.
     */
    static String DIRECTORY_PODCASTS; // = "Podcasts";

    /**
     * Standard directory in which to place any audio files that should be
     * in the list of ringtones that the user can select (not as regular
     * music).
     * This may be combined with {@link #DIRECTORY_MUSIC},
     * {@link #DIRECTORY_PODCASTS}, {@link #DIRECTORY_NOTIFICATIONS}, and
     * {@link #DIRECTORY_ALARMS} as a series
     * of directories to categories a particular audio file as more than one
     * type.
     */
    static String DIRECTORY_RINGTONES; // = "Ringtones";

    /**
     * Standard directory in which to place any audio files that should be
     * in the list of alarms that the user can select (not as regular
     * music).
     * This may be combined with {@link #DIRECTORY_MUSIC},
     * {@link #DIRECTORY_PODCASTS}, {@link #DIRECTORY_NOTIFICATIONS},
     * and {@link #DIRECTORY_RINGTONES} as a series
     * of directories to categories a particular audio file as more than one
     * type.
     */
    static String DIRECTORY_ALARMS; // = "Alarms";

    /**
     * Standard directory in which to place any audio files that should be
     * in the list of notifications that the user can select (not as regular
     * music).
     * This may be combined with {@link #DIRECTORY_MUSIC},
     * {@link #DIRECTORY_PODCASTS},
     * {@link #DIRECTORY_ALARMS}, and {@link #DIRECTORY_RINGTONES} as a series
     * of directories to categories a particular audio file as more than one
     * type.
     */
    static String DIRECTORY_NOTIFICATIONS; // = "Notifications";

    /**
     * Standard directory in which to place pictures that are available to
     * the user.  Note that this is primarily a convention for the top-level
     * directory, as the media scanner will find and collect pictures
     * in any directory.
     */
    static String DIRECTORY_PICTURES; // = "Pictures";

    /**
     * Standard directory in which to place movies that are available to
     * the user.  Note that this is primarily a convention for the top-level
     * directory, as the media scanner will find and collect movies
     * in any directory.
     */
    static String DIRECTORY_MOVIES; // = "Movies";

    /**
     * Standard directory in which to place files that have been downloaded by
     * the user.  Note that this is primarily a convention for the top-level
     * directory, you are free to download files anywhere in your own
     * directories.  Also note that though the constant here is
     * named DIRECTORY_DOWNLOADS (plural), the actual file name is non-plural for
     * backwards compatibility reasons.
     */
    static String DIRECTORY_DOWNLOADS; // = "Download";

    /**
     * The traditional location for pictures and videos when mounting the
     * device as a camera.  Note that this is primarily a convention for the
     * top-level directory, as this convention makes no sense elsewhere.
     */
    static String DIRECTORY_DCIM; // = "DCIM";

    /**
     * Standard directory in which to place documents that have been created by
     * the user.
     */
    static String DIRECTORY_DOCUMENTS; // = "Documents";

    /** {@hide} */
    static const String DIR_ANDROID; // = "Android";

    /** {@hide} */
    //@Deprecated
    static const String DIRECTORY_ANDROID; // = DIR_ANDROID;

private:
    friend class UserEnvironment;

    ECO_LOCAL static const String ENV_EXTERNAL_STORAGE; // = "EXTERNAL_STORAGE";
    ECO_LOCAL static const String ENV_EMULATED_STORAGE_SOURCE; // = "EMULATED_STORAGE_SOURCE";
    ECO_LOCAL static const String ENV_EMULATED_STORAGE_TARGET; // = "EMULATED_STORAGE_TARGET";
    ECO_LOCAL static const String ENV_MEDIA_STORAGE; // = "MEDIA_STORAGE";
    ECO_LOCAL static const String ENV_SECONDARY_STORAGE; // = "SECONDARY_STORAGE";
    ECO_LOCAL static const String ENV_ANDROID_ROOT; // = "ANDROID_ROOT";
    ECO_LOCAL static const String ENV_OEM_ROOT; // = "OEM_ROOT";
    ECO_LOCAL static const String ENV_VENDOR_ROOT; // = "VENDOR_ROOT";

    ECO_LOCAL static const String DIR_DATA; // = "data";
    ECO_LOCAL static const String DIR_MEDIA; // = "media";
    ECO_LOCAL static const String DIR_OBB; // = "obb";
    ECO_LOCAL static const String DIR_FILES; // = "files";
    ECO_LOCAL static const String DIR_CACHE; // = "cache";

    ECO_LOCAL static const AutoPtr<IFile> DIR_ANDROID_ROOT; // = GetDirectory(ENV_ANDROID_ROOT, "/system");
    ECO_LOCAL static const AutoPtr<IFile> DIR_OEM_ROOT; // = GetDirectory(ENV_OEM_ROOT, "/oem");
    ECO_LOCAL static const AutoPtr<IFile> DIR_VENDOR_ROOT; // = GetDirectory(ENV_VENDOR_ROOT, "/vendor");
    ECO_LOCAL static const AutoPtr<IFile> DIR_MEDIA_STORAGE; // = GetDirectory(ENV_MEDIA_STORAGE, "/data/media");

    ECO_LOCAL static const AutoPtr<IFile> DATA_DIRECTORY; //= GetDirectory("ANDROID_DATA", "/data");

    /**
     * @hide
     */
    ECO_LOCAL static const AutoPtr<IFile> SECURE_DATA_DIRECTORY;// = GetDirectory("ANDROID_SECURE_DATA", "/data/secure");

    ECO_LOCAL static const AutoPtr<IFile> DOWNLOAD_CACHE_DIRECTORY;// = GetDirectory("DOWNLOAD_CACHE", "/cache");


    ECO_LOCAL static const String CANONCIAL_EMULATED_STORAGE_TARGET;
        // = GetCanonicalPathOrNull(ENV_EMULATED_STORAGE_TARGET);

    ECO_LOCAL static const String SYSTEM_PROPERTY_EFS_ENABLED; // = "persist.security.efs.enabled";

    ECO_LOCAL static AutoPtr<IUserEnvironment> sCurrentUser;
    ECO_LOCAL static Boolean sUserRequired;

private:
    ECO_LOCAL Environment();
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_ENVIRONMENT_H__
