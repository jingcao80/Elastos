
#ifndef __ELASTOS_DROID_PROVIDERS_DOWNLOADS_CONSTANTS_H__
#define __ELASTOS_DROID_PROVIDERS_DOWNLOADS_CONSTANTS_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Downloads {

/**
 * Contains the internal constants that are used in the download manager.
 * As a general rule, modifying these constants should be done with care.
 */
class Constants
    : public Object
{
public:
    /** Tag used for debugging/logging */
    static const String TAG;

    /** The column that used to be used for the HTTP method of the request */
    static const String RETRY_AFTER_X_REDIRECT_COUNT;

    /** The column that used to be used for the magic OTA update filename */
    static const String OTA_UPDATE;

    /** The column that used to be used to reject system filetypes */
    static const String NO_SYSTEM_FILES;

    /** The column that is used for the downloads's ETag */
    static const String ETAG;

    /** The column that is used for the initiating app's UID */
    static const String UID;

    /** The intent that gets sent when the service must wake up for a retry */
    static const String ACTION_RETRY;

    /** the intent that gets sent when clicking a successful download */
    static const String ACTION_OPEN;

    /** the intent that gets sent when clicking an incomplete/failed download  */
    static const String ACTION_LIST;

    /** the intent that gets sent when deleting the notification of a completed download */
    static const String ACTION_HIDE;

    /** The default base name for downloaded files if we can't get one at the HTTP level */
    static const String DEFAULT_DL_FILENAME;

    /** The default extension for html files if we can't get one at the HTTP level */
    static const String DEFAULT_DL_HTML_EXTENSION;

    /** The default extension for text files if we can't get one at the HTTP level */
    static const String DEFAULT_DL_TEXT_EXTENSION;

    /** The default extension for binary files if we can't get one at the HTTP level */
    static const String DEFAULT_DL_BINARY_EXTENSION;

    static const String PROVIDER_PACKAGE_NAME;

    /**
     * When a number has to be appended to the filename, this string is used to separate the
     * base filename from the sequence number
     */
    static const String FILENAME_SEQUENCE_SEPARATOR;

    /** A magic filename that is allowed to exist within the system cache */
    static const String RECOVERY_DIRECTORY;

    /** The default user agent used for downloads */
    static const String DEFAULT_USER_AGENT;

    /** The MIME type of APKs */
    static const String MIMETYPE_APK;

    /** The buffer size used to stream the data */
    static const Int32 BUFFER_SIZE;

    /** The minimum amount of progress that has to be done before the progress bar gets updated */
    static const Int32 MIN_PROGRESS_STEP;

    /** The minimum amount of time that has to elapse before the progress bar gets updated, in ms */
    static const Int64 MIN_PROGRESS_TIME;

    /**
     * The number of times that the download manager will retry its network
     * operations when no progress is happening before it gives up.
     */
    static const Int32 MAX_RETRIES;

    /**
     * The minimum amount of time that the download manager accepts for
     * a Retry-After response header with a parameter in delta-seconds.
     */
    static const Int32 MIN_RETRY_AFTER; // 30s

    /**
     * The maximum amount of time that the download manager accepts for
     * a Retry-After response header with a parameter in delta-seconds.
     */
    static const Int32 MAX_RETRY_AFTER; // 24h

    /**
     * The maximum number of redirects.
     */
    static const Int32 MAX_REDIRECTS; // can't be more than 7.

    /**
     * The time between a failure and the first retry after an IOException.
     * Each subsequent retry grows exponentially, doubling each time.
     * The time is in seconds.
     */
    static const Int32 RETRY_FIRST_DELAY;

    /** Enable separate connectivity logging */
    static const Boolean LOGX;

    static const Boolean LOGV;

    static const Boolean LOGVV;

    static const String STORAGE_AUTHORITY;
    static const String STORAGE_ROOT_ID;

    /**
     * Name of directory on cache partition containing in-progress downloads.
     */
    static const String DIRECTORY_CACHE_RUNNING;

private:
    /** Enable verbose logging - use with "setprop log.tag.DownloadManager VERBOSE" */
    static const Boolean LOCAL_LOGV;

    /** Enable super-verbose logging */
    static const Boolean LOCAL_LOGVV;
};

} // namespace Downloads
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_PROVIDERS_DOWNLOADS_CONSTANTS_H__
