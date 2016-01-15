#ifndef __ELASTOS_DROID_WEBKIT_ANDROIDWEBVIEW_PERMISSION_AWPERMISSIONREQUEST_H__
#define __ELASTOS_DROID_WEBKIT_ANDROIDWEBVIEW_PERMISSION_AWPERMISSIONREQUEST_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Net.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Net::IUri;
using Elastos::Core::Object;
// import org.chromium.base.CalledByNative;
// import org.chromium.base.JNINamespace;
// import org.chromium.base.ThreadUtils;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace AndroidWebview {
namespace Permission {

/**
 * This class wraps permission request in Chromium side, and can only be created
 * by native side.
 */
//@JNINamespace("android_webview")
class AwPermissionRequest
    : public Object
{
public:
    CARAPI_(AutoPtr<IUri>) GetOrigin();

    CARAPI_(Int64) GetResources();

    CARAPI Grant();

    CARAPI Deny();

protected:
    //@Override
    //protected void finalize();
    ~AwPermissionRequest();

private:
    AwPermissionRequest(
        /* [in] */ Int64 nativeAwPermissionRequest,
        /* [in] */ IUri* origin,
        /* [in] */ Int64 resources);

    //@CalledByNative return AwPermissionRequest
    static CARAPI_(AutoPtr<IInterface>) Create(
        /* [in] */ Int64 nativeAwPermissionRequest,
        /* [in] */ const String& url,
        /* [in] */ Int64 resources);

    //@CalledByNative
    CARAPI_(void) DetachNativeInstance();

    CARAPI Validate();

    CARAPI_(void) NativeOnAccept(
        /* [in] */ Int64 nativeAwPermissionRequest,
        /* [in] */ Boolean allowed);

//callback function declaration
public:
    static CARAPI_(void*) ElaAwPermissionRequestCallback_Init();

private:
    static CARAPI_(void) DetachNativeInstance(
        /* [in] */ IInterface* obj);

private:
    static String TAG;

    AutoPtr<IUri> mOrigin;
    Int64 mResources;
    Boolean mProcessed;

    // AwPermissionRequest native instance.
    Int64 mNativeAwPermissionRequest;
};

} // namespace Permission
} // namespace AndroidWebview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_ANDROIDWEBVIEW_PERMISSION_AWPERMISSIONREQUEST_H__
