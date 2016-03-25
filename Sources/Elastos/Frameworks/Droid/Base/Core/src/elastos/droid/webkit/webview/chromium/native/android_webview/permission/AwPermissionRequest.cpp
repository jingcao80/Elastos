
#include <Elastos.Droid.Net.h>
#include "elastos/droid/webkit/webview/chromium/native/android_webview/permission/AwPermissionRequest.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/api/AwPermissionRequest_dec.h"
#include "elastos/droid/webkit/webview/chromium/native/base/ThreadUtils.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Webkit::Webview::Chromium::Base::ThreadUtils;

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {
namespace Permission {

String AwPermissionRequest::TAG("AwPermissionRequest");

AwPermissionRequest::AwPermissionRequest(
    /* [in] */ Handle64 nativeAwPermissionRequest,
    /* [in] */ IUri* origin,
    /* [in] */ Int64 resources)
    : mOrigin(origin)
    , mResources(resources)
    , mProcessed(FALSE)
    , mNativeAwPermissionRequest(nativeAwPermissionRequest)
{
}

//@Override
AwPermissionRequest::~AwPermissionRequest()
{
    if (mNativeAwPermissionRequest == 0) return;
    Logger::E(TAG, "Neither grant() nor deny() has been called,\
                    the permission request is denied by default.");
    Deny();
}

//@CalledByNative return AwPermissionRequest
AutoPtr<IInterface> AwPermissionRequest::Create(
    /* [in] */ Handle64 nativeAwPermissionRequest,
    /* [in] */ const String& url,
    /* [in] */ Int64 resources)
{
    if (nativeAwPermissionRequest == 0) return NULL;

    AutoPtr<IUriHelper> helper;
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    AutoPtr<IUri> origin;
    helper->Parse(url, (IUri**)&origin);
    AutoPtr<AwPermissionRequest> request = new AwPermissionRequest(nativeAwPermissionRequest, origin, resources);
    AutoPtr<IInterface> result = TO_IINTERFACE(request);
    return result;
}



AutoPtr<IUri> AwPermissionRequest::GetOrigin()
{
    return mOrigin;
}

Int64 AwPermissionRequest::GetResources()
{
    return mResources;
}

ECode AwPermissionRequest::Grant()
{
    ECode validate = Validate();
    if (FAILED(validate))
        return validate;
    if (mNativeAwPermissionRequest != 0)
        NativeOnAccept(mNativeAwPermissionRequest, TRUE);
    mProcessed = TRUE;
    return NOERROR;
}

ECode AwPermissionRequest::Deny()
{
    ECode validate = Validate();
    if (FAILED(validate))
        return validate;
    if (mNativeAwPermissionRequest != 0)
        NativeOnAccept(mNativeAwPermissionRequest, FALSE);
    mProcessed = TRUE;
    return NOERROR;
}

//@CalledByNative
void AwPermissionRequest::DetachNativeInstance()
{
    mNativeAwPermissionRequest = 0;
}

ECode AwPermissionRequest::Validate()
{
    if (!ThreadUtils::RunningOnUiThread()) {
        // throw new IllegalStateException(
        //         "Either grant() or deny() should be called on UI thread");
        //assert(0);
        Logger::E(TAG, "Either grant() or deny() should be called on UI thread");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    if (mProcessed) {
        // throw new IllegalStateException("Either grant() or deny() has been already called.");
        //assert(0);
        Logger::E(TAG, "Either grant() or deny() has been already called.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

void AwPermissionRequest::NativeOnAccept(
    /* [in] */ Handle64 nativeAwPermissionRequest,
    /* [in] */ Boolean allowed)
{
    Elastos_AwPermissionRequest_nativeOnAccept(TO_IINTERFACE(this), nativeAwPermissionRequest, allowed);
}
//callback function definition

void AwPermissionRequest::DetachNativeInstance(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwPermissionRequest> mObj = (AwPermissionRequest*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwPermissionRequest::DetachNativeInstance, mObj is NULL");
        return;
    }
    mObj->DetachNativeInstance();
}

} // namespace Permission
} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
