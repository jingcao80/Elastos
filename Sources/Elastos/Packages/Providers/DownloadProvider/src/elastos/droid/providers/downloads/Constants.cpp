
#include "elastos/droid/providers/downloads/Constants.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/os/Build.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Os::Build;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Downloads {

const String Constants::TAG("DownloadManager");

const String Constants::RETRY_AFTER_X_REDIRECT_COUNT("method");

const String Constants::OTA_UPDATE("otaupdate");

const String Constants::NO_SYSTEM_FILES("no_system");

const String Constants::ETAG("etag");

const String Constants::UID("uid");

const String Constants::ACTION_RETRY("android.intent.action.DOWNLOAD_WAKEUP");

const String Constants::ACTION_OPEN("android.intent.action.DOWNLOAD_OPEN");

const String Constants::ACTION_LIST("android.intent.action.DOWNLOAD_LIST");

const String Constants::ACTION_HIDE("android.intent.action.DOWNLOAD_HIDE");

const String Constants::DEFAULT_DL_FILENAME("downloadfile");

const String Constants::DEFAULT_DL_HTML_EXTENSION(".html");

const String Constants::DEFAULT_DL_TEXT_EXTENSION(".txt");

const String Constants::DEFAULT_DL_BINARY_EXTENSION(".bin");

const String Constants::PROVIDER_PACKAGE_NAME("com.android.providers.downloads");

const String Constants::FILENAME_SEQUENCE_SEPARATOR("-");

const String Constants::RECOVERY_DIRECTORY("recovery");

static String GetUserAgent()
{
    StringBuilder builder;

    Boolean validRelease = !TextUtils::IsEmpty(Build::VERSION::RELEASE);
    Boolean validId = !TextUtils::IsEmpty(Build::ID);
    Boolean includeModel = Build::VERSION::CODENAME.Equals("REL")
            && !TextUtils::IsEmpty(Build::MODEL);

    builder.Append("AndroidDownloadManager");
    if (validRelease) {
        builder.Append("/");
        builder.Append(Build::VERSION::RELEASE);
    }
    builder.Append(" (Linux; U; Android");
    if (validRelease) {
        builder.Append(" ");
        builder.Append(Build::VERSION::RELEASE);
    }
    if (includeModel || validId) {
        builder.Append(";");
        if (includeModel) {
            builder.Append(" ");
            builder.Append(Build::MODEL);
        }
        if (validId) {
            builder.Append(" Build/");
            builder.Append(Build::ID);
        }
    }
    builder.Append(")");

    String str = builder.ToString();
    return str;
}

const String Constants::DEFAULT_USER_AGENT = GetUserAgent();

const String Constants::MIMETYPE_APK("application/vnd.android.package");

const Int32 Constants::BUFFER_SIZE = 8192;

const Int32 Constants::MIN_PROGRESS_STEP = 65536;

const Int64 Constants::MIN_PROGRESS_TIME = 2000;

const Int32 Constants::MAX_RETRIES = 5;

const Int32 Constants::MIN_RETRY_AFTER = 30; // 30s

const Int32 Constants::MAX_RETRY_AFTER = 24 * 60 * 60; // 24h

const Int32 Constants::MAX_REDIRECTS = 5; // can't be more than 7.

const Int32 Constants::RETRY_FIRST_DELAY = 30;

const Boolean Constants::LOGX = FALSE;

const Boolean Constants::LOCAL_LOGV = FALSE;
const Boolean Constants::LOGV = LOCAL_LOGV;// && Log.isLoggable(TAG, Log.VERBOSE);

const Boolean Constants::LOCAL_LOGVV = FALSE;
const Boolean Constants::LOGVV = LOCAL_LOGVV && LOGV;

const String Constants::STORAGE_AUTHORITY("com.android.providers.downloads.documents");
const String Constants::STORAGE_ROOT_ID("downloads");

const String Constants::DIRECTORY_CACHE_RUNNING("partial_downloads");

} // namespace Downloads
} // namespace Providers
} // namespace Droid
} // namespace Elastos
