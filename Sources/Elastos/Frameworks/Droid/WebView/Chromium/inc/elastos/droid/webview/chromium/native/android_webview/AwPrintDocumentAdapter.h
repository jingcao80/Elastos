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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWPRINTDOCUMENTADAPTER_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWPRINTDOCUMENTADAPTER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.Droid.Webkit.h>
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwPdfExporter.h"

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::ICancellationSignal;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Webkit::IValueCallback;
//TODO using Elastos::Droid::Print::IPageRange;
//TODO using Elastos::Droid::Print::IPrintAttributes;
//TODO using Elastos::Droid::Print::IPrintDocumentAdapter;
//TODO using Elastos::Droid::Print::IWriteResultCallback;//defined in PrintDocumentAdapter.java
//TODO using Elastos::Droid::Print::ILayoutResultCallback;//defined in PrintDocumentAdapter.java
//TODO using Elastos::Droid::Print::IPrintDocumentInfo;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

/**
 * Adapter for printing Webview. This class implements the abstract
 * system class PrintDocumentAdapter and hides all printing details from
 * the developer.
 */
class AwPrintDocumentAdapter
//TODO : public PrintDocumentAdapter
{
public:
    class InnerValueCallback
        : public Object
        , public IValueCallback
    {
    public:
        CAR_INTERFACE_DECL()
        InnerValueCallback(
            /* [in] */ AwPrintDocumentAdapter* owner,
            /* [in] */ /*TODO IWriteResultCallback*/IInterface* callback);

        CARAPI OnReceiveValue(
            /* [in] */ IInterface* value);

    private:
        AwPrintDocumentAdapter* mOwner;
        AutoPtr</*TODO IWriteResultCallback*/IInterface> mCallback;
    };

public:
    /**
     * Constructor.
     * TODO(sgurun) remove in favor of constructor below once the AOSP changes are in.
     *
     * @param pdfExporter The PDF exporter to export the webview contents to a PDF file.
     */
    AwPrintDocumentAdapter(
        /* [in] */ AwPdfExporter* pdfExporter);

    /**
     * Constructor.
     *
     * @param pdfExporter The PDF exporter to export the webview contents to a PDF file.
     * @param documentName  The name of the pdf document.
     */
    AwPrintDocumentAdapter(
        /* [in] */ AwPdfExporter* pdfExporter,
        /* [in] */ const String& documentName);

    //@Override
    CARAPI_(void) OnLayout(
        /* [in] */ /*TODO IPrintAttributes*/IInterface* oldAttributes,
        /* [in] */ /*TODO IPrintAttributes*/IInterface* newAttributes,
        /* [in] */ ICancellationSignal* cancellationSignal,
        /* [in] */ /*TODO ILayoutResultCallback*/IInterface* callback,
        /* [in] */ IBundle* metadata);

    //@Override
    CARAPI_(void) OnWrite(
        /* [in] */ ArrayOf</*TODO IPageRange*/IInterface*>* pages,
        /* [in] */ IParcelFileDescriptor* destination,
        /* [in] */ ICancellationSignal* cancellationSignal,
        /* [in] */ /*TODO IWriteResultCallback*/IInterface* callback);

private:
    AutoPtr<AwPdfExporter> mPdfExporter;
    AutoPtr</*TODO IPrintAttributes*/IInterface> mAttributes;
    String mDocumentName;
};

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWPRINTDOCUMENTADAPTER_H__
