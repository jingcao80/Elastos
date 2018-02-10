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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWPDFEXPORTER_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWPDFEXPORTER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.Droid.Webkit.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Os::ICancellationSignal;
using Elastos::Droid::Os::IParcelFileDescriptor;
//TODO using Elastos::Droid::Print::IPrintAttributes;
// import android.print.PrintAttributes;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Webkit::IValueCallback;
using Elastos::Core::Object;

// import org.chromium.base.CalledByNative;
// import org.chromium.base.JNINamespace;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

/**
 * Export the android webview as a PDF.
 * @TODO(sgurun) explain the ownership of this class and its native counterpart
 */
//@JNINamespace("android_webview")
class AwPdfExporter
    : public Object
{
public:
    AwPdfExporter(
        /* [in] */ IViewGroup* containerView);

    virtual CARAPI_(void) SetContainerView(
        /* [in] */ IViewGroup* containerView);

    virtual CARAPI ExportToPdf(
        /* [in] */ IParcelFileDescriptor* fd,
        /* [in] */ /*TODO IPrintAttributes*/IInterface* attributes,
        /* [in] */ IValueCallback* resultCallback,
        /* [in] */ ICancellationSignal* cancellationSignal);

private:
    //@CalledByNative
    CARAPI_(void) SetNativeAwPdfExporter(
        /* [in] */ HANDLE nativePdfExporter);

    static CARAPI_(Int32) GetPrintDpi(
        /* [in] */ /*TODO IPrintAttributes*/IInterface* attributes);

    //@CalledByNative
    CARAPI_(void) DidExportPdf(
        /* [in] */ Boolean success);

    //@CalledByNative
    CARAPI_(Int32) GetPageWidth();

    //@CalledByNative
    CARAPI_(Int32) GetPageHeight();

    //@CalledByNative
    CARAPI_(Int32) GetDpi();

    //@CalledByNative
    CARAPI_(Int32) GetLeftMargin();

    //@CalledByNative
    CARAPI_(Int32) GetRightMargin();

    //@CalledByNative
    CARAPI_(Int32) GetTopMargin();

    //@CalledByNative
    CARAPI_(Int32) GetBottomMargin();

    CARAPI_(void) NativeExportToPdf(
        /* [in] */ HANDLE nativeAwPdfExporter,
        /* [in] */ Int32 fd,
        /* [in] */ ICancellationSignal* cancellationSignal);

//callback function declaration
public:
    static CARAPI_(void*) ElaAwPdfExporterCallback_Init();

private:
    static CARAPI_(void) SetNativeAwPdfExporter(
        /* [in] */ IInterface* obj,
        /* [in] */ HANDLE nativePdfExporter);

    static CARAPI_(void) DidExportPdf(
        /* [in] */ IInterface* obj,
        /* [in] */ Boolean success);

    static CARAPI_(Int32) GetPageWidth(
        /* [in] */ IInterface* obj);

    static CARAPI_(Int32) GetPageHeight(
        /* [in] */ IInterface* obj);

    static CARAPI_(Int32) GetDpi(
        /* [in] */ IInterface* obj);

    static CARAPI_(Int32) GetLeftMargin(
        /* [in] */ IInterface* obj);

    static CARAPI_(Int32) GetRightMargin(
        /* [in] */ IInterface* obj);

    static CARAPI_(Int32) GetTopMargin(
        /* [in] */ IInterface* obj);

    static CARAPI_(Int32) GetBottomMargin(
        /* [in] */ IInterface* obj);

private:
    static const String TAG;
    HANDLE mNativeAwPdfExporter;
    // TODO(sgurun) result callback should return an int/object indicating errors.
    // potential errors: invalid print parameters, already pending, IO error
    AutoPtr<IValueCallback> mResultCallback;
    //TODO AutoPtr<IPrintAttributes> mAttributes;
    AutoPtr<IInterface> mAttributes;//TODO removed if above line is ok
    AutoPtr<IParcelFileDescriptor> mFd;
    // Maintain a reference to the top level object (i.e. WebView) since in a common
    // use case (offscreen webview) application may expect the framework's print manager
    // to own the Webview (via PrintDocumentAdapter).
    // NOTE: it looks unused, but please do not remove this reference.
    AutoPtr<IViewGroup> mContainerView;
};

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWPDFEXPORTER_H__
