// wuweizuo automatic build .cpp file from .java file.

#include "elastos/droid/webkit/webview/chromium/native/content/browser/DownloadInfo.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

//=====================================================================
//                        DownloadInfo::Builder
//=====================================================================
AutoPtr<DownloadInfo::Builder> DownloadInfo::Builder::SetUrl(
    /* [in] */ const String& url)
{
    mUrl = url;
    return this;
}

AutoPtr<DownloadInfo::Builder> DownloadInfo::Builder::SetUserAgent(
    /* [in] */ const String& userAgent)
{
    mUserAgent = userAgent;
    return this;
}

AutoPtr<DownloadInfo::Builder> DownloadInfo::Builder::SetMimeType(
    /* [in] */ const String& mimeType)
{
    mMimeType = mimeType;
    return this;
}

AutoPtr<DownloadInfo::Builder> DownloadInfo::Builder::SetCookie(
    /* [in] */ const String& cookie)
{
    mCookie = cookie;
    return this;
}

AutoPtr<DownloadInfo::Builder> DownloadInfo::Builder::SetFileName(
    /* [in] */ const String& fileName)
{
    mFileName = fileName;
    return this;
}

AutoPtr<DownloadInfo::Builder> DownloadInfo::Builder::SetDescription(
    /* [in] */ const String& description)
{
    mDescription = description;
    return this;
}

AutoPtr<DownloadInfo::Builder> DownloadInfo::Builder::SetFilePath(
    /* [in] */ const String& filePath)
{
    mFilePath = filePath;
    return this;
}

AutoPtr<DownloadInfo::Builder> DownloadInfo::Builder::SetReferer(
    /* [in] */ const String& referer)
{
    mReferer = referer;
    return this;
}

AutoPtr<DownloadInfo::Builder> DownloadInfo::Builder::SetContentLength(
    /* [in] */ Int64 contentLength)
{
    mContentLength = contentLength;
    return this;
}

AutoPtr<DownloadInfo::Builder> DownloadInfo::Builder::SetIsGETRequest(
    /* [in] */ Boolean isGETRequest)
{
    mIsGETRequest = isGETRequest;
    return this;
}

AutoPtr<DownloadInfo::Builder> DownloadInfo::Builder::SetHasDownloadId(
    /* [in] */ Boolean hasDownloadId)
{
    mHasDownloadId = hasDownloadId;
    return this;
}

AutoPtr<DownloadInfo::Builder> DownloadInfo::Builder::SetDownloadId(
    /* [in] */ Int32 downloadId)
{
    mDownloadId = downloadId;
    return this;
}

AutoPtr<DownloadInfo::Builder> DownloadInfo::Builder::SetIsSuccessful(
    /* [in] */ Boolean isSuccessful)
{
    mIsSuccessful = isSuccessful;
    return this;
}

AutoPtr<DownloadInfo::Builder> DownloadInfo::Builder::SetContentDisposition(
    /* [in] */ const String& contentDisposition)
{
    mContentDisposition = contentDisposition;
    return this;
}

AutoPtr<DownloadInfo::Builder> DownloadInfo::Builder::SetPercentCompleted(
    /* [in] */ Int32 percentCompleted)
{
    assert(percentCompleted <= 100);
    mPercentCompleted = percentCompleted;
    return this;
}

AutoPtr<DownloadInfo::Builder> DownloadInfo::Builder::SetTimeRemainingInMillis(
    /* [in] */ Int64 timeRemainingInMillis)
{
    mTimeRemainingInMillis = timeRemainingInMillis;
    return this;
}

AutoPtr<DownloadInfo> DownloadInfo::Builder::Build()
{
    return new DownloadInfo(this);
}

AutoPtr<DownloadInfo::Builder> DownloadInfo::Builder::FromDownloadInfo(
    /* [in] */ /*const*/ DownloadInfo* downloadInfo)
{
    AutoPtr<Builder> builder = new Builder();
    builder->SetUrl(downloadInfo->GetUrl())
           ->SetUserAgent(downloadInfo->GetUserAgent())
           ->SetMimeType(downloadInfo->GetMimeType())
           ->SetCookie(downloadInfo->GetCookie())
           ->SetFileName(downloadInfo->GetFileName())
           ->SetDescription(downloadInfo->GetDescription())
           ->SetFilePath(downloadInfo->GetFilePath())
           ->SetReferer(downloadInfo->GetReferer())
           ->SetContentLength(downloadInfo->GetContentLength())
           ->SetHasDownloadId(downloadInfo->HasDownloadId())
           ->SetDownloadId(downloadInfo->GetDownloadId())
           ->SetContentDisposition(downloadInfo->GetContentDisposition())
           ->SetIsGETRequest(downloadInfo->IsGETRequest())
           ->SetIsSuccessful(downloadInfo->IsSuccessful())
           ->SetPercentCompleted(downloadInfo->GetPercentCompleted())
           ->SetTimeRemainingInMillis(downloadInfo->GetTimeRemainingInMillis());

    return builder;
}

//=====================================================================
//                             DownloadInfo
//=====================================================================

DownloadInfo::DownloadInfo(
    /* [in] */ DownloadInfo::Builder* builder)
    : mUrl(builder->mUrl)
    , mUserAgent(builder->mUserAgent)
    , mMimeType(builder->mMimeType)
    , mCookie(builder->mCookie)
    , mFileName(builder->mFileName)
    , mDescription(builder->mDescription)
    , mFilePath(builder->mFilePath)
    , mReferer(builder->mReferer)
    , mContentLength(builder->mContentLength)
    , mHasDownloadId(builder->mHasDownloadId)
    , mDownloadId(builder->mDownloadId)
    , mContentDisposition(builder->mContentDisposition)
    , mIsGETRequest(builder->mIsGETRequest)
    , mIsSuccessful(builder->mIsSuccessful)
    , mPercentCompleted(builder->mPercentCompleted)
    , mTimeRemainingInMillis(builder->mTimeRemainingInMillis)
{
}

String DownloadInfo::GetUrl()
{
    return mUrl;
}

String DownloadInfo::GetUserAgent()
{
    return mUserAgent;
}

String DownloadInfo::GetMimeType()
{
    return mMimeType;
}

String DownloadInfo::GetCookie()
{
    return mCookie;
}

String DownloadInfo::GetFileName()
{
    return mFileName;
}

String DownloadInfo::GetDescription()
{
    return mDescription;
}

String DownloadInfo::GetFilePath()
{
    return mFilePath;
}

String DownloadInfo::GetReferer()
{
    return mReferer;
}

Int64 DownloadInfo::GetContentLength()
{
    return mContentLength;
}

Boolean DownloadInfo::IsGETRequest()
{
    return mIsGETRequest;
}

Boolean DownloadInfo::HasDownloadId()
{
    return mHasDownloadId;
}

Int32 DownloadInfo::GetDownloadId()
{
    return mDownloadId;
}

Boolean DownloadInfo::IsSuccessful()
{
    return mIsSuccessful;
}

String DownloadInfo::GetContentDisposition()
{
    return mContentDisposition;
}

Int32 DownloadInfo::GetPercentCompleted()
{
    return mPercentCompleted;
}

Int64 DownloadInfo::GetTimeRemainingInMillis()
{
    return mTimeRemainingInMillis;
}

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
