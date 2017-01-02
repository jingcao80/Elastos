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

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_FILECHOOSERPARAMSADAPTER_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_FILECHOOSERPARAMSADAPTER_H_

#include "Elastos.Droid.Webkit.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwContentsClient.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Webkit::Webview::Chromium::AndroidWebview::AwContentsClient;
using Elastos::Droid::Webkit::IWebChromeClientFileChooserParams;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {

class FileChooserParamsAdapter
    : public Object
    , public IWebChromeClientFileChooserParams
{
public:
    CAR_INTERFACE_DECL()

    FileChooserParamsAdapter(
        /* [in] */ AwContentsClient::FileChooserParams* params,
        /* [in] */ IContext* context);

    static CARAPI_(AutoPtr< ArrayOf<IUri*> >) ParseFileChooserResult(
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* intent);

    //@Override
    CARAPI GetMode(
        /* [out] */ Int32* mode);

    //@Override
    CARAPI GetAcceptTypes(
        /* [out] */ ArrayOf<String>** acceptTypes);

    //@Override
    CARAPI IsCaptureEnabled(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI GetTitle(
        /* [out] */ ICharSequence** title);

    //@Override
    CARAPI GetFilenameHint(
        /* [out] */ String* fileName);

    //@Override
    CARAPI CreateIntent(
        /* [out] */ IIntent** intent);

private:
    AutoPtr<AwContentsClient::FileChooserParams> mParams;
};

} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_FILECHOOSERPARAMSADAPTER_H_

