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

#ifndef __ELASTOS_DROID_APP_CDOWNLOADMANAGERREQUEST_H__
#define __ELASTOS_DROID_APP_CDOWNLOADMANAGERREQUEST_H__

#include "Elastos.Droid.Net.h"
#include "_Elastos_Droid_App_CDownloadManagerRequest.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/Pair.h>

using Elastos::Core::ICharSequence;
using Elastos::IO::IFile;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::Pair;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CDownloadManagerRequest)
    , public Object
    , public IDownloadManagerRequest
{
friend class CDownloadManager;

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CDownloadManagerRequest();

    CARAPI constructor(
        /* [in] */ IUri* uri);

    CARAPI constructor(
        /* [in] */ const String& uriString);

    /**
     * Set the local destination for the downloaded file. Must be a file URI to a path on
     * external storage, and the calling application must have the WRITE_EXTERNAL_STORAGE
     * permission.
     * <p>
     * The downloaded file is not scanned by MediaScanner.
     * But it can be made scannable by calling {@link #allowScanningByMediaScanner()}.
     * <p>
     * By default, downloads are saved to a generated filename in the shared download cache and
     * may be deleted by the system at any time to reclaim space.
     *
     * @return this object
     */
    CARAPI SetDestinationUri(
        /* [in] */ IUri* uri);

    /**
     * Set the local destination for the downloaded file to the system cache dir (/cache).
     * This is only available to System apps with the permission
     * {@link android.Manifest.permission#ACCESS_CACHE_FILESYSTEM}.
     * <p>
     * The downloaded file is not scanned by MediaScanner.
     * But it can be made scannable by calling {@link #allowScanningByMediaScanner()}.
     * <p>
     * Files downloaded to /cache may be deleted by the system at any time to reclaim space.
     *
     * @return this object
     * @hide
     */
    CARAPI SetDestinationToSystemCache();

    /**
     * Set the local destination for the downloaded file to a path within the application's
     * external files directory (as returned by {@link Context#getExternalFilesDir(String)}.
     * <p>
     * The downloaded file is not scanned by MediaScanner.
     * But it can be made scannable by calling {@link #allowScanningByMediaScanner()}.
     *
     * @param context the {@link Context} to use in determining the external files directory
     * @param dirType the directory type to pass to {@link Context#getExternalFilesDir(String)}
     * @param subPath the path within the external directory, including the destination filename
     * @return this object
     */
    CARAPI SetDestinationInExternalFilesDir(
        /* [in] */ IContext* context,
        /* [in] */ const String& dirType,
        /* [in] */ const String& subPath);

    /**
     * Set the local destination for the downloaded file to a path within the public external
     * storage directory (as returned by
     * {@link Environment#getExternalStoragePublicDirectory(String)}.
     *<p>
     * The downloaded file is not scanned by MediaScanner.
     * But it can be made scannable by calling {@link #allowScanningByMediaScanner()}.
     *
     * @param dirType the directory type to pass to
     *        {@link Environment#getExternalStoragePublicDirectory(String)}
     * @param subPath the path within the external directory, including the destination filename
     * @return this object
     */
    CARAPI SetDestinationInExternalPublicDir(
        /* [in] */ const String& dirType,
        /* [in] */ const String& subPath);

    /**
     * If the file to be downloaded is to be scanned by MediaScanner, this method
     * should be called before {@link DownloadManager#enqueue(Request)} is called.
     */
    CARAPI AllowScanningByMediaScanner();

    /**
     * Add an HTTP header to be included with the download request.  The header will be added to
     * the end of the list.
     * @param header HTTP header name
     * @param value header value
     * @return this object
     * @see <a href="http://www.w3.org/Protocols/rfc2616/rfc2616-sec4.html#sec4.2">HTTP/1.1
     *      Message Headers</a>
     */
    CARAPI AddRequestHeader(
        /* [in] */ const String& header,
        /* [in] */ const String& value);

    /**
     * Set the title of this download, to be displayed in notifications (if enabled).  If no
     * title is given, a default one will be assigned based on the download filename, once the
     * download starts.
     * @return this object
     */
    CARAPI SetTitle(
        /* [in] */ ICharSequence* title);

    /**
     * Set a description of this download, to be displayed in notifications (if enabled)
     * @return this object
     */
    CARAPI SetDescription(
        /* [in] */ ICharSequence* description);

    /**
     * Set the MIME content type of this download.  This will override the content type declared
     * in the server's response.
     * @see <a href="http://www.w3.org/Protocols/rfc2616/rfc2616-sec3.html#sec3.7">HTTP/1.1
     *      Media Types</a>
     * @return this object
     */
    CARAPI SetMimeType(
        /* [in] */ const String& mimeType);

    /**
     * Control whether a system notification is posted by the download manager while this
     * download is running. If enabled, the download manager posts notifications about downloads
     * through the system {@link android.app.NotificationManager}. By default, a notification is
     * shown.
     *
     * If set to false, this requires the permission
     * android.permission.DOWNLOAD_WITHOUT_NOTIFICATION.
     *
     * @param show whether the download manager should show a notification for this download.
     * @return this object
     * @deprecated use {@link #setNotificationVisibility(int)}
     */
    //@Deprecated
    CARAPI SetShowRunningNotification(
        /* [in] */ Boolean show);

    /**
     * Control whether a system notification is posted by the download manager while this
     * download is running or when it is completed.
     * If enabled, the download manager posts notifications about downloads
     * through the system {@link android.app.NotificationManager}.
     * By default, a notification is shown only when the download is in progress.
     *<p>
     * It can take the following values: {@link #VISIBILITY_HIDDEN},
     * {@link #VISIBILITY_VISIBLE},
     * {@link #VISIBILITY_VISIBLE_NOTIFY_COMPLETED}.
     *<p>
     * If set to {@link #VISIBILITY_HIDDEN}, this requires the permission
     * android.permission.DOWNLOAD_WITHOUT_NOTIFICATION.
     *
     * @param visibility the visibility setting value
     * @return this object
     */
    CARAPI SetNotificationVisibility(
        /* [in] */ Int32 visibility);

    /**
     * Restrict the types of networks over which this download may proceed.
     * By default, all network types are allowed. Consider using
     * {@link #setAllowedOverMetered(boolean)} instead, since it's more
     * flexible.
     *
     * @param flags any combination of the NETWORK_* bit flags.
     * @return this object
     */
    CARAPI SetAllowedNetworkTypes(
        /* [in] */ Int32 flags);

    /**
     * Set whether this download may proceed over a roaming connection.  By default, roaming is
     * allowed.
     * @param allowed whether to allow a roaming connection to be used
     * @return this object
     */
    CARAPI SetAllowedOverRoaming(
        /* [in] */ Boolean allowed);

    /**
     * Set whether this download may proceed over a metered network
     * connection. By default, metered networks are allowed.
     *
     * @see ConnectivityManager#isActiveNetworkMetered()
     */
    CARAPI SetAllowedOverMetered(
        /* [in] */ Boolean allow);

    /**
     * Set whether this download should be displayed in the system's Downloads UI. True by
     * default.
     * @param isVisible whether to display this download in the Downloads UI
     * @return this object
     */
    CARAPI SetVisibleInDownloadsUi(
        /* [in] */ Boolean isVisible);

    /**
     * @return ContentValues to be passed to DownloadProvider.insert()
     */
    CARAPI_(AutoPtr<IContentValues>) ToContentValues(
        /* [in] */ const String& packageName);

private:
    CARAPI SetDestinationFromBase(
        /* [in] */ IFile* base,
        /* [in] */ const String& subPath);

    CARAPI_(void) EncodeHttpHeaders(
        /* [in] */ IContentValues* values);

    CARAPI_(void) PutIfNonNull(
        /* [in] */ IContentValues* contentValues,
        /* [in] */ const String& key,
        /* [in] */ IInterface* value);

private:
    AutoPtr<IUri> mUri;
    AutoPtr<IUri> mDestinationUri;
    List< Pair<String, String> > mRequestHeaders;
    AutoPtr<ICharSequence> mTitle;
    AutoPtr<ICharSequence> mDescription;
    String mMimeType;
    Int32 mAllowedNetworkTypes; // default to all network types allowed
    Boolean mRoamingAllowed;
    Boolean mMeteredAllowed;
    Boolean mIsVisibleInDownloadsUi;
    Boolean mScannable;
    Boolean mUseSystemCache;

    /** if a file is designated as a MediaScanner scannable file, the following value is
     * stored in the database column {@link Downloads.Impl#COLUMN_MEDIA_SCANNED}.
     */
    static const Int32 SCANNABLE_VALUE_YES = 0;
    // value of 1 is stored in the above column by DownloadProvider after it is scanned by
    // MediaScanner
    /** if a file is designated as a file that should not be scanned by MediaScanner,
     * the following value is stored in the database column
     * {@link Downloads.Impl#COLUMN_MEDIA_SCANNED}.
     */
    static const Int32 SCANNABLE_VALUE_NO = 2;

    /** can take any of the following values: {@link #VISIBILITY_HIDDEN}
     * {@link #VISIBILITY_VISIBLE_NOTIFY_COMPLETED}, {@link #VISIBILITY_VISIBLE},
     * {@link #VISIBILITY_VISIBLE_NOTIFY_ONLY_COMPLETION}
     */
    Int32 mNotificationVisibility;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CDOWNLOADMANAGERREQUEST_H__
