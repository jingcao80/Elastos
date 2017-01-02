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

#include "Elastos.Droid.Content.h"
#include "elastos/droid/webkit/CWebViewFactory.h"
#include "elastos/droid/webkit/WebViewFactory.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

CAR_INTERFACE_IMPL(CWebViewFactory, Object, IWebViewFactory);

CAR_SINGLETON_IMPL(CWebViewFactory);

ECode CWebViewFactory::GetWebViewPackageName(
        /* [out] */ String* packageName)
{
    VALIDATE_NOT_NULL(packageName);
    *packageName = WebViewFactory::GetWebViewPackageName();
    return NOERROR;
}

ECode CWebViewFactory::GetLoadedPackageInfo(
    /* [out] */ IPackageInfo** packageInfo)
{
    VALIDATE_NOT_NULL(packageInfo);
    AutoPtr<IPackageInfo> _packageInfo = WebViewFactory::GetLoadedPackageInfo();
    *packageInfo = _packageInfo;
    REFCOUNT_ADD(*packageInfo);
    return NOERROR;
}

ECode CWebViewFactory::PrepareWebViewInZygote()
{
    return WebViewFactory::PrepareWebViewInZygote();
}

ECode CWebViewFactory::PrepareWebViewInSystemServer()
{
    return WebViewFactory::PrepareWebViewInSystemServer();
}

ECode CWebViewFactory::OnWebViewUpdateInstalled()
{
    return WebViewFactory::OnWebViewUpdateInstalled();
}

} // namespace Webkit
} // namepsace Droid
} // namespace Elastos
