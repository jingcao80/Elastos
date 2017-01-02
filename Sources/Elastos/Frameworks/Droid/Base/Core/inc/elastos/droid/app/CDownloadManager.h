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

#ifndef __ELASTOS_DROID_APP_CDOWNLOADMANAGER_H__
#define __ELASTOS_DROID_APP_CDOWNLOADMANAGER_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "_Elastos_Droid_App_CDownloadManager.h"
#include "elastos/droid/database/CursorWrapper.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::CursorWrapper;
using Elastos::Droid::Database::ICursorWrapper;
using Elastos::Droid::Database::ICharArrayBuffer;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Database::IDataSetObserver;
using Elastos::Droid::Os::IParcelFileDescriptor;

namespace Elastos {
namespace Droid {
namespace App {

/**
 * The download manager is a system service that handles long-running HTTP downloads. Clients may
 * request that a URI be downloaded to a particular destination file. The download manager will
 * conduct the download in the background, taking care of HTTP interactions and retrying downloads
 * after failures or across connectivity changes and system reboots.
 *
 * Instances of this class should be obtained through
 * {@link android.content.Context#getSystemService(String)} by passing
 * {@link android.content.Context#DOWNLOAD_SERVICE}.
 *
 * Apps that request downloads through this API should register a broadcast receiver for
 * {@link #ACTION_NOTIFICATION_CLICKED} to appropriately handle when the user clicks on a running
 * download in a notification or from the downloads UI.
 *
 * Note that the application must have the {@link android.Manifest.permission#INTERNET}
 * permission to use this class.
 */
CarClass(CDownloadManager)
    , public Object
    , public IDownloadManager
{
private:
    class CursorTranslator
        : public CursorWrapper
    {
    public:
        CursorTranslator();

        CARAPI constructor(
            /* [in] */ ICursor* cursor,
            /* [in] */ IUri* baseUri);

        CARAPI GetInt32(
             /* [in] */ Int32 columnIndex,
             /* [out] */ Int32* value);

        CARAPI GetInt64(
            /* [in] */ Int32 columnIndex,
            /* [out] */ Int64* value);

        CARAPI GetString(
            /* [in] */ Int32 columnIndex,
            /* [out] */ String* value);

    private:
        CARAPI_(String) GetLocalUri();

        CARAPI_(Int64) GetReason(
            /* [in] */ Int32 status);

        CARAPI_(Int64) GetPausedReason(
            /* [in] */ Int32 status);

        CARAPI_(Int64) GetErrorCode(
            /* [in] */ Int32 status);

        CARAPI_(Int32) TranslateStatus(
            /* [in] */ Int32 status);

    private:
        AutoPtr<IUri> mBaseUri;
    };

public:

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ const String& packageName);

    /**
     * Makes this object access the download provider through /all_downloads URIs rather than
     * /my_downloads URIs, for clients that have permission to do so.
     * @hide
     */
    CARAPI SetAccessAllDownloads(
        /* [in] */ Boolean accessAllDownloads);

    /**
     * Enqueue a new download.  The download will start automatically once the download manager is
     * ready to execute it and connectivity is available.
     *
     * @param request the parameters specifying this download
     * @return an ID for the download, unique across the system.  This ID is used to make future
     * calls related to this download.
     */
    CARAPI Enqueue(
        /* [in] */ IDownloadManagerRequest* request,
        /* [out] */ Int64* id);

    /**
     * Marks the specified download as 'to be deleted'. This is done when a completed download
     * is to be removed but the row was stored without enough info to delete the corresponding
     * metadata from Mediaprovider database. Actual cleanup of this row is done in DownloadService.
     *
     * @param ids the IDs of the downloads to be marked 'deleted'
     * @return the number of downloads actually updated
     * @hide
     */
    CARAPI MarkRowDeleted(
        /* [in] */ ArrayOf<Int64>* ids,
        /* [out] */ Int32* number);

    /**
     * Cancel downloads and remove them from the download manager.  Each download will be stopped if
     * it was running, and it will no longer be accessible through the download manager.
     * If there is a downloaded file, partial or complete, it is deleted.
     *
     * @param ids the IDs of the downloads to remove
     * @return the number of downloads actually removed
     */
    CARAPI Remove(
        /* [in] */ ArrayOf<Int64>* ids,
        /* [out] */ Int32* number);

    /**
     * Query the download manager about downloads that have been requested.
     * @param query parameters specifying filters for this query
     * @return a Cursor over the result set of downloads, with columns consisting of all the
     * COLUMN_* constants.
     */
    CARAPI Query(
        /* [in] */ IDownloadManagerQuery* query,
        /* [out] */ ICursor** cursor);

    /**
     * Open a downloaded file for reading.  The download must have completed.
     * @param id the ID of the download
     * @return a read-only {@link ParcelFileDescriptor}
     * @throws FileNotFoundException if the destination file does not already exist
     */
    CARAPI OpenDownloadedFile(
        /* [in] */ Int64 id,
        /* [out] */ IParcelFileDescriptor** descriptor);

    /**
     * Returns {@link Uri} for the given downloaded file id, if the file is
     * downloaded successfully. otherwise, null is returned.
     *<p>
     * If the specified downloaded file is in external storage (for example, /sdcard dir),
     * then it is assumed to be safe for anyone to read and the returned {@link Uri} corresponds
     * to the filepath on sdcard.
     *
     * @param id the id of the downloaded file.
     * @return the {@link Uri} for the given downloaded file id, if download was successful. null
     * otherwise.
     */
    CARAPI GetUriForDownloadedFile(
        /* [in] */ Int64 id,
        /* [out] */ IUri** uri);

    /**
     * Returns {@link Uri} for the given downloaded file id, if the file is
     * downloaded successfully. otherwise, null is returned.
     *<p>
     * If the specified downloaded file is in external storage (for example, /sdcard dir),
     * then it is assumed to be safe for anyone to read and the returned {@link Uri} corresponds
     * to the filepath on sdcard.
     *
     * @param id the id of the downloaded file.
     * @return the {@link Uri} for the given downloaded file id, if download was successful. null
     * otherwise.
     */
    CARAPI GetMimeTypeForDownloadedFile(
        /* [in] */ Int64 id,
        /* [out] */ String* uri);

    /**
     * Restart the given downloads, which must have already completed (successfully or not).  This
     * method will only work when called from within the download manager's process.
     * @param ids the IDs of the downloads
     * @hide
     */
    CARAPI RestartDownload(
        /* [in] */ ArrayOf<Int64>* ids);

    /**
     * Pause the given running download by user.
     *
     * @param id the ID of the download to be paused
     * @hide
     */
    CARAPI PauseDownload(
        /* [in] */ Int64 id);

    /**
     * Resume the given paused download by user.
     *
     * @param id the ID of the download to be resumed
     * @hide
     */
    CARAPI ResumeDownload(
        /* [in] */ Int64 id);

    static CARAPI GetMaxBytesOverMobile(
        /* [in] */ IContext* context,
        /* [out] */ Int64* size);

    static CARAPI GetRecommendedMaxBytesOverMobile(
        /* [in] */ IContext* context,
        /* [out] */ Int64* size);

    static CARAPI IsActiveNetworkExpensive(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* actived);

    static CARAPI GetActiveNetworkWarningBytes(
        /* [in] */ IContext* context,
        /* [out] */ Int64* bytes);

    /**
     * Adds a file to the downloads database system, so it could appear in Downloads App
     * (and thus become eligible for management by the Downloads App).
     * <p>
     * It is helpful to make the file scannable by MediaScanner by setting the param
     * isMediaScannerScannable to true. It makes the file visible in media managing
     * applications such as Gallery App, which could be a useful purpose of using this API.
     *
     * @param title the title that would appear for this file in Downloads App.
     * @param description the description that would appear for this file in Downloads App.
     * @param isMediaScannerScannable true if the file is to be scanned by MediaScanner. Files
     * scanned by MediaScanner appear in the applications used to view media (for example,
     * Gallery app).
     * @param mimeType mimetype of the file.
     * @param path absolute pathname to the file. The file should be world-readable, so that it can
     * be managed by the Downloads App and any other app that is used to read it (for example,
     * Gallery app to display the file, if the file contents represent a video/image).
     * @param length length of the downloaded file
     * @param showNotification true if a notification is to be sent, false otherwise
     * @return  an ID for the download entry added to the downloads app, unique across the system
     * This ID is used to make future calls related to this download.
     */
    CARAPI AddCompletedDownload(
        /* [in] */ const String& title,
        /* [in] */ const String& description,
        /* [in] */ Boolean isMediaScannerScannable,
        /* [in] */ const String& mimeType,
        /* [in] */ const String& path,
        /* [in] */ Int64 length,
        /* [in] */ Boolean showNotification,
        /* [out] */ Int64* id);

    /** {@hide} */
    CARAPI AddCompletedDownload(
        /* [in] */ const String& title,
        /* [in] */ const String& description,
        /* [in] */ Boolean isMediaScannerScannable,
        /* [in] */ const String& mimeType,
        /* [in] */ const String& path,
        /* [in] */ Int64 length,
        /* [in] */ Boolean showNotification,
        /* [in] */ Boolean allowWrite,
        /* [out] */ Int64* id);

    /**
     * Get the DownloadProvider URI for the download with the given ID.
     */
    CARAPI_(AutoPtr<IUri>) GetDownloadUri(
        /* [in] */ Int64 id);

    CARAPI GetDownloadUri(
        /* [in] */ Int64 id,
        /* [in] */ IUri** uri);

    /**
     * Get a parameterized SQL WHERE clause to select a bunch of IDs.
     */
    static CARAPI_(String) GetWhereClauseForIds(
        /* [in] */ ArrayOf<Int64>* ids);

    /**
     * Get the selection args for a clause returned by {@link #getWhereClauseForIds(long[])}.
     */
    static CARAPI_(AutoPtr< ArrayOf<String> >) GetWhereArgsForIds(
        /* [in] */ ArrayOf<Int64>* ids);

private:
    static CARAPI ValidateArgumentIsNonEmpty(
        /* [in] */ const String& paramName,
        /* [in] */ const String& val);

public:
    /**
     * columns to request from DownloadProvider.
     * @hide
     */
    static const AutoPtr<ArrayOf<String> > UNDERLYING_COLUMNS;

private:
    AutoPtr<IContentResolver> mResolver;
    String mPackageName;
    AutoPtr<IUri> mBaseUri;

    static const String NON_DOWNLOADMANAGER_DOWNLOAD;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CDOWNLOADMANAGER_H__
