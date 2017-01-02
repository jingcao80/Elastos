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

#include "elastos/droid/webkit/CWebChromeClientFileChooserParamsHelper.h"
#include "elastos/droid/webkit/WebViewFactory.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

CAR_INTERFACE_IMPL(CWebChromeClientFileChooserParamsHelper, Object, IWebChromeClientFileChooserParamsHelper);

CAR_SINGLETON_IMPL(CWebChromeClientFileChooserParamsHelper);

ECode CWebChromeClientFileChooserParamsHelper::ParseResult(
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* data,
    /* [out, callee] */ ArrayOf<IUri*>** uriArrayOut)
{
    AutoPtr<IWebViewFactoryProviderStatics> statics;
    WebViewFactory::GetProvider()->GetStatics((IWebViewFactoryProviderStatics**)&statics);
    return statics->ParseFileChooserResult(resultCode, data, uriArrayOut);
}

} // namespace Webkit
} // namespace Droid
} // namespace Elastos
