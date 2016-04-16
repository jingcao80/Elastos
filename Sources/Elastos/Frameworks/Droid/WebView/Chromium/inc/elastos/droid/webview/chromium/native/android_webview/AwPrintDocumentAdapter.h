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
        CAR_INTERFACE_DECL();
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
