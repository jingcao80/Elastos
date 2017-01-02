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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWZOOMCONTROLS_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWZOOMCONTROLS_H__

#include "Elastos.Droid.Widget.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/ContentViewCore.h"

using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IFrameLayout;
using Elastos::Droid::Widget::IZoomButtonsController;
using Elastos::Droid::Widget::IOnZoomListener;
using Elastos::Droid::Webkit::Webview::Chromium::Content::Browser::ContentViewCore;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {
class AwContents;

class AwZoomControls
: public ContentViewCore::ZoomControlsDelegate
{
private:
    class ZoomListener
        : public Object
        , public IOnZoomListener
    {
    public:
        CAR_INTERFACE_DECL()
        ZoomListener(
            /* [in] */ AwZoomControls* owner);

        //@Override
        CARAPI OnVisibilityChanged(
            /* [in] */ Boolean visible);

        //@Override
        CARAPI OnZoom(
            /* [in] */ Boolean zoomIn);

    private:
        AwZoomControls* mOwner;
    };

public:
    AwZoomControls(
        /* [in] */ AwContents* awContents);

    //@Override
    CARAPI_(void) InvokeZoomPicker();

    //@Override
    CARAPI_(void) DismissZoomPicker();

    //@Override
    CARAPI_(void) UpdateZoomControls();

    // This method is used in tests. It doesn't modify the state of zoom controls.
    CARAPI_(AutoPtr<IView>) GetZoomControlsViewForTest();

private:
    CARAPI_(AutoPtr<IZoomButtonsController>) GetZoomController();

    AwContents* mAwContents;
    // It is advised to use getZoomController() where possible.
    AutoPtr<IZoomButtonsController> mZoomButtonsController;
};

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWZOOMCONTROLS_H__
