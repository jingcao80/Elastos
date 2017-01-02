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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_CONTENTVIEWDOWNLOADDELEGATE_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_CONTENTVIEWDOWNLOADDELEGATE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/DownloadInfo.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

/**
 * Interface to be implemented by the embedder to handle file downloads.
 */
class ContentViewDownloadDelegate : public Object
{
public:
    /**
    * Notify the host application that a file should be downloaded. Replaces
    * onDownloadStart from DownloadListener.
    * @param downloadInfo Information about the requested download.
    */
    virtual CARAPI_(void) RequestHttpGetDownload(
        /* [in] */ DownloadInfo* downloadInfo) = 0;

    /**
     * Notify the host application that a download is started.
     * @param filename File name of the downloaded file.
     * @param mimeType Mime of the downloaded item.
     */
    virtual CARAPI_(void) OnDownloadStarted(
        /* [in] */ const String& filename,
        /* [in] */ const String& mimeType) = 0;

    /**
     * Notify the host application that a download has an extension indicating
     * a dangerous file type.
     * @param filename File name of the downloaded file.
     * @param downloadId The download id.
     */
    virtual CARAPI_(void) OnDangerousDownload(
        /* [in] */ const String& filename,
        /* [in] */ Int32 downloadId) = 0;
};

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_CONTENTVIEWDOWNLOADDELEGATE_H__
