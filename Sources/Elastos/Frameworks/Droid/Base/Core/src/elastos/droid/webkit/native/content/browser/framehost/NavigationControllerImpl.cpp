
#include "elastos/droid/webkit/native/content/browser/framehost/NavigationControllerImpl.h"
#include "elastos/droid/webkit/native/content/api/NavigationControllerImpl_dec.h"
#include <elastos/utility/logging/Logger.h>
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {
namespace Framehost {

NavigationControllerImpl::NavigationControllerImpl(
    /* [in] */ Int64 nativeNavigationControllerAndroid)
{
    mNativeNavigationControllerAndroid = nativeNavigationControllerAndroid;
}

//@CalledByNative
AutoPtr<IInterface> NavigationControllerImpl::Create(
    /* [in] */ Int64 nativeNavigationControllerAndroid)
{
    AutoPtr<NavigationControllerImpl> ret = new NavigationControllerImpl(nativeNavigationControllerAndroid);
    return TO_IINTERFACE(ret);
}

//@CalledByNative
void NavigationControllerImpl::Destroy()
{
    mNativeNavigationControllerAndroid = 0;
}

//@Override
Boolean NavigationControllerImpl::CanGoBack()
{
    return mNativeNavigationControllerAndroid != 0
            && NativeCanGoBack(mNativeNavigationControllerAndroid);
}

//@Override
Boolean NavigationControllerImpl::CanGoForward()
{
    return mNativeNavigationControllerAndroid != 0
            && NativeCanGoForward(mNativeNavigationControllerAndroid);
}

//@Override
Boolean NavigationControllerImpl::CanGoToOffset(
    /* [in] */ Int32 offset)
{
    return mNativeNavigationControllerAndroid != 0
            && NativeCanGoToOffset(mNativeNavigationControllerAndroid, offset);
}

//@Override
void NavigationControllerImpl::GoToOffset(
    /* [in] */ Int32 offset)
{
    if (mNativeNavigationControllerAndroid != 0) {
        NativeGoToOffset(mNativeNavigationControllerAndroid, offset);
    }
}

//@Override
void NavigationControllerImpl::GoToNavigationIndex(
    /* [in] */ Int32 index)
{
    if (mNativeNavigationControllerAndroid != 0) {
        NativeGoToNavigationIndex(mNativeNavigationControllerAndroid, index);
    }
}

//@Override
void NavigationControllerImpl::GoBack()
{
    if (mNativeNavigationControllerAndroid != 0) {
        NativeGoBack(mNativeNavigationControllerAndroid);
    }
}

//@Override
void NavigationControllerImpl::GoForward()
{
    if (mNativeNavigationControllerAndroid != 0) {
        NativeGoForward(mNativeNavigationControllerAndroid);
    }
}

Boolean NavigationControllerImpl::NativeCanGoBack(
    /* [in] */ Int64 nativeNavigationControllerAndroid)
{
    return Elastos_NavigationControllerImpl_nativeCanGoBack(THIS_PROBE(IInterface), (Handle32)nativeNavigationControllerAndroid);
}

Boolean NavigationControllerImpl::NativeCanGoForward(
    /* [in] */ Int64 nativeNavigationControllerAndroid)
{
    return Elastos_NavigationControllerImpl_nativeCanGoForward(THIS_PROBE(IInterface), (Handle32)nativeNavigationControllerAndroid);
}

Boolean NavigationControllerImpl::NativeCanGoToOffset(
    /* [in] */ Int64 nativeNavigationControllerAndroid,
    /* [in] */ Int32 offset)
{
    return Elastos_NavigationControllerImpl_nativeCanGoToOffset(THIS_PROBE(IInterface), (Handle32)nativeNavigationControllerAndroid, offset);
}

void NavigationControllerImpl::NativeGoBack(
    /* [in] */ Int64 nativeNavigationControllerAndroid)
{
    Elastos_NavigationControllerImpl_nativeGoBack(THIS_PROBE(IInterface), (Handle32)nativeNavigationControllerAndroid);
}

void NavigationControllerImpl::NativeGoForward(
    /* [in] */ Int64 nativeNavigationControllerAndroid)
{
    Elastos_NavigationControllerImpl_nativeGoForward(THIS_PROBE(IInterface), (Handle32)nativeNavigationControllerAndroid);
}

void NavigationControllerImpl::NativeGoToOffset(
    /* [in] */ Int64 nativeNavigationControllerAndroid,
    /* [in] */ Int32 offset)
{
    Elastos_NavigationControllerImpl_nativeGoToOffset(THIS_PROBE(IInterface), (Handle32)nativeNavigationControllerAndroid, offset);
}

void NavigationControllerImpl::NativeGoToNavigationIndex(
    /* [in] */ Int64 nativeNavigationControllerAndroid,
    /* [in] */ Int32 index)
{
    Elastos_NavigationControllerImpl_nativeGoToNavigationIndex(THIS_PROBE(IInterface), (Handle32)nativeNavigationControllerAndroid, index);
}

void NavigationControllerImpl::Destroy(
    /* [in] */ IInterface* obj)
{
    AutoPtr<NavigationControllerImpl> mObj = (NavigationControllerImpl*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("NavigationControllerImpl", "NavigationControllerImpl::Destroy, mObj is NULL");
        return;
    }
    mObj->Destroy();
}

} // namespace Framehost
} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
