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

#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwPrintDocumentAdapter.h"

//TODO using Elastos::Droid::Print::IPrintDocumentInfo;
//TODO using Elastos::Droid::Print::IPrintDocumentInfoBuilder;
//TODO using Elastos::Droid::Print::CPrintDocumentInfoBuilder;
using Elastos::Droid::Webkit::EIID_IValueCallback;
using Elastos::Core::IBoolean;
using Elastos::Core::CBoolean;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

//===============================================================
//          AwPrintDocumentAdapter::InnerValueCallback
//===============================================================
CAR_INTERFACE_IMPL(AwPrintDocumentAdapter::InnerValueCallback, Object, IValueCallback);

AwPrintDocumentAdapter::InnerValueCallback::InnerValueCallback(
    /* [in] */ AwPrintDocumentAdapter* owner,
    /* [in] */ /*TODO IWriteResultCallback*/IInterface* callback)
    : mOwner(owner)
    , mCallback(callback)
{
}

ECode AwPrintDocumentAdapter::InnerValueCallback::OnReceiveValue(
    /* [in] */ IInterface* iValue)
{
    AutoPtr<IBoolean> bValue = IBoolean::Probe(iValue);
    Boolean value = FALSE;
    if (bValue)
        bValue->GetValue(&value);
    if (value) {
        //TODO AutoPtr<ArrayOf<IPageRange*> > pageRange = ArrayOf<IPageRange*>::Alloc(1);
        //TODO (*pageRange)[0] = IPageRange::ALL_PAGES;
        //TODO callback->OnWriteFinished(pageRange);
    }
    else {
        // TODO(sgurun) provide a localized error message
        //TODO callback->OnWriteFailed(NULL);
    }

    return NOERROR;
}

//===============================================================
//                   AwPrintDocumentAdapter
//===============================================================

/**
 * Constructor.
 * TODO(sgurun) remove in favor of constructor below once the AOSP changes are in.
 *
 * @param pdfExporter The PDF exporter to export the webview contents to a PDF file.
 */
AwPrintDocumentAdapter::AwPrintDocumentAdapter(
    /* [in] */ AwPdfExporter* pdfExporter)
    : mPdfExporter(pdfExporter)
    , mDocumentName("default")
{
}

/**
 * Constructor.
 *
 * @param pdfExporter The PDF exporter to export the webview contents to a PDF file.
 * @param documentName  The name of the pdf document.
 */
AwPrintDocumentAdapter::AwPrintDocumentAdapter(
    /* [in] */ AwPdfExporter* pdfExporter,
    /* [in] */ const String& documentName)
    : mPdfExporter(pdfExporter)
    , mDocumentName(documentName)
{
}


//@Override
void AwPrintDocumentAdapter::OnLayout(
    /* [in] */ /*TODO IPrintAttributes*/IInterface* oldAttributes,
    /* [in] */ /*TODO IPrintAttributes*/IInterface* newAttributes,
    /* [in] */ ICancellationSignal* cancellationSignal,
    /* [in] */ /*TODO ILayoutResultCallback*/IInterface* callback,
    /* [in] */ IBundle* metadata)
{
    mAttributes = newAttributes;
    //TODO AutoPtr<IPrintDocumentInfoBuilder> builder;
    //TODO CPrintDocumentInfoBuilder::New(mDocumentName, (IPrintDocumentInfoBuilder**)&builder);
    //TODO AutoPtr<IPrintDocumentInfo> documentInfo = builder->Build();
    // TODO(sgurun) once componentization is done, do layout changes and
    // generate PDF here, set the page range information to documentinfo
    // and call onLayoutFinished with true/false depending on whether
    // layout actually changed.

    //TODO callback->OnLayoutFinished(documentInfo, TRUE);
}

//@Override
void AwPrintDocumentAdapter::OnWrite(
    /* [in] */ ArrayOf</*TODO IPageRange*/IInterface*>* pages,
    /* [in] */ IParcelFileDescriptor* destination,
    /* [in] */ ICancellationSignal* cancellationSignal,
    /* [in] */ /*TODO IWriteResultCallback*/IInterface* callback)
{
    AutoPtr<IValueCallback> valueCallback = new InnerValueCallback(this, callback);
    mPdfExporter->ExportToPdf(destination, mAttributes, valueCallback, cancellationSignal);
}

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
