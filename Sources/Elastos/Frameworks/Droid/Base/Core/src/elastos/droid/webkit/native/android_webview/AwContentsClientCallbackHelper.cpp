
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/webkit/native/android_webview/AwContentsClientCallbackHelper.h"
#include "elastos/droid/webkit/native/android_webview/AwContentsClient.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Graphics::IPicture;
using Elastos::Droid::Os::SystemClock;
using Elastos::Utility::Concurrent::EIID_ICallable;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace AndroidWebview {

//==============================================================================
//               AwContentsClientCallbackHelper::DownloadInfo
//==============================================================================

AwContentsClientCallbackHelper::DownloadInfo::DownloadInfo(
    /* [in] */ const String& url,
    /* [in] */ const String& userAgent,
    /* [in] */ const String& contentDisposition,
    /* [in] */ const String& mimeType,
    /* [in] */ Int64 contentLength)
    : mUrl(url)
    , mUserAgent(userAgent)
    , mContentDisposition(contentDisposition)
    , mMimeType(mimeType)
    , mContentLength(contentLength)
{
}

//==============================================================================
//              AwContentsClientCallbackHelper::LoginRequestInfo
//==============================================================================

AwContentsClientCallbackHelper::LoginRequestInfo::LoginRequestInfo(
    /* [in] */ const String& realm,
    /* [in] */ const String& account,
    /* [in] */ const String& args)
    : mRealm(realm)
    , mAccount(account)
    , mArgs(args)
{
}

//==============================================================================
//             AwContentsClientCallbackHelper::OnReceivedErrorInfo
//==============================================================================

AwContentsClientCallbackHelper::OnReceivedErrorInfo::OnReceivedErrorInfo(
    /* [in] */ Int32 errorCode,
    /* [in] */ const String& description,
    /* [in] */ const String& failingUrl)
    : mErrorCode(errorCode)
    , mDescription(description)
    , mFailingUrl(failingUrl)
{
}

//==============================================================================
//                  AwContentsClientCallbackHelper::MyHandler
//==============================================================================

AwContentsClientCallbackHelper::MyHandler::MyHandler(
    /* [in] */ AwContentsClientCallbackHelper* owner,
    /* [in] */ ILooper* looper)
    : Handler(looper)
    , mOwner(owner)
{
}

ECode AwContentsClientCallbackHelper::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch(what) {
        case MSG_ON_LOAD_RESOURCE: {
            AutoPtr<ICharSequence> cs;
            msg->GetObj((IInterface**)&cs);
            String url;
            cs->ToString(&url);
            mOwner->mContentsClient->OnLoadResource(url);
            break;
        }
        case MSG_ON_PAGE_STARTED: {
            AutoPtr<ICharSequence> cs;
            msg->GetObj((IInterface**)&cs);
            String url;
            cs->ToString(&url);
            mOwner->mContentsClient->OnPageStarted(url);
            break;
        }
        case MSG_ON_DOWNLOAD_START: {
            AutoPtr<DownloadInfo> info;
            msg->GetObj((IInterface**)&info);
            mOwner->mContentsClient->OnDownloadStart(info->mUrl, info->mUserAgent,
                    info->mContentDisposition, info->mMimeType, info->mContentLength);
            break;
        }
        case MSG_ON_RECEIVED_LOGIN_REQUEST: {
            AutoPtr<LoginRequestInfo> info;
            msg->GetObj((IInterface**)&info);
            mOwner->mContentsClient->OnReceivedLoginRequest(info->mRealm, info->mAccount, info->mArgs);
            break;
        }
        case MSG_ON_RECEIVED_ERROR: {
            AutoPtr<OnReceivedErrorInfo> info;
            msg->GetObj((IInterface**)&info);
            mOwner->mContentsClient->OnReceivedError(info->mErrorCode, info->mDescription,
                    info->mFailingUrl);
            break;
        }
        case MSG_ON_NEW_PICTURE: {
            AutoPtr<IPicture> picture;
            AutoPtr<IInterface> iPicture;
            //try {
                AutoPtr<IInterface> obj;
                msg->GetObj((IInterface**)&obj);
                if (obj != NULL) {
                    AutoPtr<ICallable> callable = ICallable::Probe(obj);
                    callable->Call((IInterface**)&iPicture);
                }
            //} catch (Exception e) {
            //    throw new RuntimeException("Error getting picture", e);
            //}
            picture = IPicture::Probe(iPicture);
            mOwner->mContentsClient->OnNewPicture(picture);
            mOwner->mLastPictureTime = SystemClock::GetUptimeMillis();
            mOwner->mHasPendingOnNewPicture = FALSE;
            break;
        }
        case MSG_ON_SCALE_CHANGED_SCALED: {
            Int32 arg1, arg2;
            msg->GetArg1(&arg1);
            msg->GetArg2(&arg2);
            //float oldScale = Float.intBitsToFloat(msg.arg1);
            //float newScale = Float.intBitsToFloat(msg.arg2);
            Float oldScale = *(Float*)&arg1;
            Float newScale = *(Float*)&arg2;
            mOwner->mContentsClient->OnScaleChangedScaled(oldScale, newScale);
            break;
        }
        default:
            //throw new IllegalStateException("AwContentsClientCallbackHelper: unhandled message " + msg.what);
          Logger::E("AwContentsClientCallbackHelper", "unhandled message, %d", what);
          return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

//==============================================================================
//                     AwContentsClientCallbackHelper
//==============================================================================

const Int32 AwContentsClientCallbackHelper::MSG_ON_LOAD_RESOURCE;
const Int32 AwContentsClientCallbackHelper::MSG_ON_PAGE_STARTED;
const Int32 AwContentsClientCallbackHelper::MSG_ON_DOWNLOAD_START;
const Int32 AwContentsClientCallbackHelper::MSG_ON_RECEIVED_LOGIN_REQUEST;
const Int32 AwContentsClientCallbackHelper::MSG_ON_RECEIVED_ERROR;
const Int32 AwContentsClientCallbackHelper::MSG_ON_NEW_PICTURE;
const Int32 AwContentsClientCallbackHelper::MSG_ON_SCALE_CHANGED_SCALED;

// Minimum period allowed between consecutive onNewPicture calls, to rate-limit the callbacks.
const Int64 AwContentsClientCallbackHelper::ON_NEW_PICTURE_MIN_PERIOD_MILLIS;

AwContentsClientCallbackHelper::AwContentsClientCallbackHelper(
    /* [in] */ ILooper* looper,
    /* [in] */ AwContentsClient* contentsClient)
    : mLastPictureTime(0)
    , mHasPendingOnNewPicture(FALSE)
    , mContentsClient(contentsClient)
{
    mHandler = new MyHandler(this, looper);
}

void AwContentsClientCallbackHelper::PostOnLoadResource(
    /* [in] */ const String& url)
{
    AutoPtr<IMessage> msg;
    AutoPtr<ICharSequence> iUrl;
    CString::New(url, (ICharSequence**)&iUrl);
    mHandler->ObtainMessage(MSG_ON_LOAD_RESOURCE, iUrl, (IMessage**)&msg);
    Boolean result;
    mHandler->SendMessage(msg, &result);
}

void AwContentsClientCallbackHelper::PostOnPageStarted(
    /* [in] */ const String& url)
{
    AutoPtr<IMessage> msg;
    AutoPtr<ICharSequence> iUrl;
    CString::New(url, (ICharSequence**)&iUrl);
    mHandler->ObtainMessage(MSG_ON_PAGE_STARTED, iUrl, (IMessage**)&msg);
    Boolean result;
    mHandler->SendMessage(msg, &result);
}

void AwContentsClientCallbackHelper::PostOnDownloadStart(
    /* [in] */ const String& url,
    /* [in] */ const String& userAgent,
    /* [in] */ const String& contentDisposition,
    /* [in] */ const String& mimeType,
    /* [in] */ Int64 contentLength)
{
    AutoPtr<DownloadInfo> info = new DownloadInfo(url, userAgent, contentDisposition, mimeType, contentLength);
    AutoPtr<IMessage> msg;
    AutoPtr<IInterface> obj = TO_IINTERFACE(info);
    mHandler->ObtainMessage(MSG_ON_DOWNLOAD_START, obj, (IMessage**)&msg);
    Boolean result;
    mHandler->SendMessage(msg, &result);
}

void AwContentsClientCallbackHelper::PostOnReceivedLoginRequest(
    /* [in] */ const String& realm,
    /* [in] */ const String& account,
    /* [in] */ const String& args)
{
    AutoPtr<LoginRequestInfo> info = new LoginRequestInfo(realm, account, args);
    AutoPtr<IMessage> msg;
    AutoPtr<IInterface> obj = TO_IINTERFACE(info);
    mHandler->ObtainMessage(MSG_ON_RECEIVED_LOGIN_REQUEST, obj, (IMessage**)&msg);
    Boolean result;
    mHandler->SendMessage(msg, &result);
}

void AwContentsClientCallbackHelper::PostOnReceivedError(
    /* [in] */ Int32 errorCode,
    /* [in] */ const String& description,
    /* [in] */ const String& failingUrl)
{
    AutoPtr<OnReceivedErrorInfo> info = new OnReceivedErrorInfo(errorCode, description, failingUrl);
    AutoPtr<IMessage> msg;
    AutoPtr<IInterface> obj = TO_IINTERFACE(info);
    mHandler->ObtainMessage(MSG_ON_RECEIVED_ERROR, obj, (IMessage**)&msg);
    Boolean result;
    mHandler->SendMessage(msg, &result);
}

void AwContentsClientCallbackHelper::PostOnNewPicture(
    /* [in] */ ICallable* pictureProvider)
{
    if (mHasPendingOnNewPicture) return;
    mHasPendingOnNewPicture = TRUE;
    Int64 pictureTime = Elastos::Core::Math::Max(mLastPictureTime + ON_NEW_PICTURE_MIN_PERIOD_MILLIS,
            SystemClock::GetUptimeMillis());
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_ON_NEW_PICTURE, pictureProvider, (IMessage**)&msg);
    Boolean result;
    mHandler->SendMessageAtTime(msg, pictureTime, &result);
}

void AwContentsClientCallbackHelper::PostOnScaleChangedScaled(
    /* [in] */ Float oldScale,
    /* [in] */ Float newScale)
{
    // The float->int->float conversion here is to avoid unnecessary allocations. The
    // documentation states that intBitsToFloat(floatToIntBits(a)) == a for all values of a
    // (except for NaNs which are collapsed to a single canonical NaN, but we don't care for
    // that case).
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_ON_SCALE_CHANGED_SCALED,
                FloatToIntBits(oldScale), FloatToIntBits(newScale), (IMessage**)&msg);
    Boolean result;
    mHandler->SendMessage(msg, &result);
}

Int32 AwContentsClientCallbackHelper::FloatToIntBits(
    /* [in] */ Float value)
{
    Int32 intBit;
    Boolean isNan = Elastos::Core::Math::IsNaN(value);
    if(isNan) {
        intBit = 0x7fc00000;
    }
    else {
        intBit = *((Int32*)(&value));
    }
    return intBit;
}

} // namespace AndroidWebview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
