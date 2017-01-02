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

#include "elastos/droid/webkit/CWebViewHelper.h"
#include "elastos/droid/webkit/WebView.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

CAR_INTERFACE_IMPL(CWebViewHelper, Object, IWebViewHelper);

CAR_SINGLETON_IMPL(CWebViewHelper);

ECode CWebViewHelper::EnablePlatformNotifications()
{
    WebView::EnablePlatformNotifications();
    return NOERROR;
}

ECode CWebViewHelper::DisablePlatformNotifications()
{
    WebView::DisablePlatformNotifications();
    return NOERROR;
}

ECode CWebViewHelper::FindAddress(
    /* [in] */ const String& addr,
    /* [out] */ String * outAddr)
{
    VALIDATE_NOT_NULL(outAddr);
    *outAddr = WebView::FindAddress(addr);

    return NOERROR;
}

ECode CWebViewHelper::GetPluginList(
    /* [out] */ IPluginList ** pluginList)
{
    VALIDATE_NOT_NULL(pluginList);
    AutoPtr<IPluginList> _pluginList = WebView::GetPluginList();
    *pluginList = _pluginList.Get();
    REFCOUNT_ADD(*pluginList);

    return NOERROR;
}

ECode CWebViewHelper::ClearClientCertPreferences(
    /* [in] */ IRunnable* onCleared)
{
    return WebView::ClearClientCertPreferences(onCleared);
}

ECode CWebViewHelper::EnableSlowWholeDocumentDraw()
{
    return WebView::EnableSlowWholeDocumentDraw();
}

ECode CWebViewHelper::SetWebContentsDebuggingEnabled(
    /* [in] */ Boolean enabled)
{
    return WebView::SetWebContentsDebuggingEnabled(enabled);
}

} // namespace Webkit
} // namepsace Droid
} // namespace Elastos