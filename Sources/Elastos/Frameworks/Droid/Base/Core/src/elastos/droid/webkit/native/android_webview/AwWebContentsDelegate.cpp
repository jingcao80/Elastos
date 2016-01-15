#include "elastos/droid/webkit/native/android_webview/AwWebContentsDelegate.h"
#include "elastos/droid/webkit/native/android_webview/api/AwWebContentsDelegate_dec.h"
#include <elastos/utility/logging/Logger.h>
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace AndroidWebview {

// Call in response to a prior runFileChooser call.
void AwWebContentsDelegate::NativeFilesSelectedInChooser(
    /* [in] */ Int32 processId,
    /* [in] */ Int32 renderId,
    /* [in] */ Int32 mode_flags,
    /* [in] */ ArrayOf<String>* filePath,
    /* [in] */ ArrayOf<String>* displayName)
{
    Elastos_AwWebContentsDelegate_nativeFilesSelectedInChooser(processId, renderId, mode_flags, filePath, displayName);
}

//callback function definition
void AwWebContentsDelegate::RunFileChooser(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 processId,
    /* [in] */ Int32 renderId,
    /* [in] */ Int32 mode_flags,
    /* [in] */ const String& acceptTypes,
    /* [in] */ const String& title,
    /* [in] */ const String& defaultFilename,
    /* [in] */ Boolean capture)
{
    AutoPtr<AwWebContentsDelegate> mObj = (AwWebContentsDelegate*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("AwWebContentsDelegate", "AwWebContentsDelegate::RunFileChooser, mObj is NULL");
        return;
    }
    mObj->RunFileChooser(processId, renderId, mode_flags, acceptTypes, title, defaultFilename, capture);
}

Boolean AwWebContentsDelegate::AddNewContents(
    /* [in] */ IInterface* obj,
    /* [in] */ Boolean isDialog,
    /* [in] */ Boolean isUserGesture)
{
    AutoPtr<AwWebContentsDelegate> mObj = (AwWebContentsDelegate*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("AwWebContentsDelegate", "AwWebContentsDelegate::AddNewContents, mObj is NULL");
        return FALSE;
    }
    return mObj->AddNewContents(isDialog, isUserGesture);
}

void AwWebContentsDelegate::CloseContents(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwWebContentsDelegate> mObj = (AwWebContentsDelegate*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("AwWebContentsDelegate", "AwWebContentsDelegate::CloseContents, mObj is NULL");
        return;
    }
    mObj->CloseContents();
}

void AwWebContentsDelegate::ActivateContents(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwWebContentsDelegate> mObj = (AwWebContentsDelegate*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("AwWebContentsDelegate", "AwWebContentsDelegate::ActivateContents, mObj is NULL");
        return;
    }
    mObj->ActivateContents();
}

void AwWebContentsDelegate::ToggleFullscreenModeForTab(
    /* [in] */ IInterface* obj,
    /* [in] */ Boolean enterFullscreen)
{
    AutoPtr<AwWebContentsDelegate> mObj = (AwWebContentsDelegate*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("AwWebContentsDelegate", "AwWebContentsDelegate::ToggleFullscreenModeForTab, mObj is NULL");
        return;
    }
    mObj->ToggleFullscreenModeForTab(enterFullscreen);
}
} // namespace AndroidWebview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
