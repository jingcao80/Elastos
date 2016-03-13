
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/AwPdfExporter.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/api/AwPdfExporter_dec.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Logger;
//TODO using Elastos::Droid::Print::IMargins;
//TODO using Elastos::Droid::Print::IMediaSize;
//TODO using Elastos::Droid::Print::IResolution;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

const String AwPdfExporter::TAG("AwPdfExporter");

AwPdfExporter::AwPdfExporter(
    /* [in] */ IViewGroup* containerView)
    : mNativeAwPdfExporter(0)
{
    SetContainerView(containerView);
}

void AwPdfExporter::SetContainerView(
    /* [in] */ IViewGroup* containerView)
{
    mContainerView = containerView;
}

ECode AwPdfExporter::ExportToPdf(
    /* [in] */ IParcelFileDescriptor* fd,
    /* [in] */ /*TODO IPrintAttributes*/IInterface* attributes,
    /* [in] */ IValueCallback* resultCallback,
    /* [in] */ ICancellationSignal* cancellationSignal)
{
    if (fd == NULL) {
        //throw new IllegalArgumentException("fd cannot be null");
        Logger::E(TAG, "fd cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (resultCallback == NULL) {
        //throw new IllegalArgumentException("resultCallback cannot be null");
        Logger::E(TAG, "resultCallback cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mResultCallback != NULL) {
        //throw new IllegalStateException("printing is already pending");
        Logger::E(TAG, "printing is already pending");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    //TODO AutoPtr<IMediaSize> mediaSize;
    //TODO if (attributes->GetMediaSize((IMediaSize**)&mediaSize), mediaSize == NULL)
    {
        //throw new  IllegalArgumentException("attributes must specify a media size");
        Logger::E(TAG, "attributes must specify a media size");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    //TODO AutoPtr<IResolution> resolution;
    //TODO if (attributes->GetResolution((IResolution**)&resolution), resolution == NULL)
    {
        //throw new IllegalArgumentException("attributes must specify print resolution");
        Logger::E(TAG, "attributes must specify print resolution");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    //TODO AutoPtr<IMargins> margins;
    //TODO if (attributes->GetMinMargins((IMargins**)&margins), margins == NULL)
    {
        //throw new IllegalArgumentException("attributes must specify margins");
        Logger::E(TAG, "attributes must specify margins");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mNativeAwPdfExporter == 0) {
        AutoPtr<IBoolean> boolTmp = CoreUtils::Convert(FALSE);
        resultCallback->OnReceiveValue(TO_IINTERFACE(boolTmp));
        return NOERROR;
    }

    mResultCallback = resultCallback;
    mAttributes = attributes;
    mFd = fd;

    Int32 _fd;
    mFd->GetFd(&_fd);
    NativeExportToPdf(mNativeAwPdfExporter, _fd, cancellationSignal);
}

//@CalledByNative
void AwPdfExporter::SetNativeAwPdfExporter(
    /* [in] */ Handle64 nativePdfExporter)
{
    mNativeAwPdfExporter = nativePdfExporter;
    // Handle the cornercase that Webview.Destroy is called before the native side
    // has a chance to complete the pdf exporting.
    if (nativePdfExporter == 0 && mResultCallback != NULL) {
        AutoPtr<IBoolean> boolTmp = CoreUtils::Convert(FALSE);
        mResultCallback->OnReceiveValue(TO_IINTERFACE(boolTmp));
        mResultCallback = NULL;
    }
}

Int32 AwPdfExporter::GetPrintDpi(
    /* [in] */ /*TODO IPrintAttributes*/IInterface* attributes)
{
    // TODO(sgurun) android print attributes support horizontal and
    // vertical DPI. Chrome has only one DPI. Revisit this.
    //TODO AutoPtr<IResolution> resolution;
    //TODO attributes->GetResolution((IResolution**)&resolution);
    Int32 horizontalDpi = 0;
    //TODO resolution->GetHorizontalDpi(&horizontalDpi);
    Int32 verticalDpi = 0;
    //TODO resolution->GetVerticalDpi(&verticalDpi);
    if (horizontalDpi != verticalDpi) {
        Logger::W(TAG, "Horizontal and vertical DPIs differ. Using horizontal DPI \
                       hDpi=%d, vDPI=%d.", horizontalDpi, verticalDpi);
    }
    return horizontalDpi;
}

//@CalledByNative
void AwPdfExporter::DidExportPdf(
    /* [in] */ Boolean success)
{
    AutoPtr<IBoolean> boolTmp = CoreUtils::Convert(success);
    mResultCallback->OnReceiveValue(TO_IINTERFACE(boolTmp));
    mResultCallback = NULL;
    mAttributes = NULL;
    // The caller should close the file.
    mFd = NULL;
}

//@CalledByNative
Int32 AwPdfExporter::GetPageWidth()
{
    //TODO AutoPtr<IMediaSize> mediaSize;
    //TODO mAttributes->GetMediaSize((IMediaSize**)&mediaSize);
    Int32 mils = 0;
    //TODO mediaSize->GetWidthMils(&mils);
    return mils;
}

//@CalledByNative
Int32 AwPdfExporter::GetPageHeight()
{
    //TODO AutoPtr<IMediaSize> mediaSize;
    //TODO mAttributes->GetMediaSize((IMediaSize**)&mediaSize);
    Int32 mils = 0;
    //TODO mediaSize->GetHeightMils(&mils);
    return mils;
}

//@CalledByNative
Int32 AwPdfExporter::GetDpi()
{
    return GetPrintDpi(mAttributes);
}

//@CalledByNative
Int32 AwPdfExporter::GetLeftMargin()
{
    //TODO AutoPtr<IMargins> margins;
    //TODO mAttributes->GetMinMargins((IMargins**)&margins);
    Int32 mils = 0;
    //TODO margins->GetLeftMils(&mils);
    return mils;
}

//@CalledByNative
Int32 AwPdfExporter::GetRightMargin()
{
    //TODO AutoPtr<IMargins> margins;
    //TODO mAttributes->GetMinMargins((IMargins**)&margins);
    Int32 mils = 0;
    //TODO margins->GetRightMils(&mils);
    return mils;
}

//@CalledByNative
Int32 AwPdfExporter::GetTopMargin()
{
    //TODO AutoPtr<IMargins> margins;
    //TODO mAttributes->GetMinMargins((IMargins**)&margins);
    Int32 mils = 0;
    //TODO margins->GetTopMils(&mils);
    return mils;
}

//@CalledByNative
Int32 AwPdfExporter::GetBottomMargin()
{
    //TODO AutoPtr<IMargins> margins;
    //TODO mAttributes->GetMinMargins((IMargins**)&margins);
    Int32 mils = 0;
    //TODO margins->GetBottomMils(&mils);
    return mils;
}

void AwPdfExporter::NativeExportToPdf(
    /* [in] */ Handle64 nativeAwPdfExporter,
    /* [in] */ Int32 fd,
    /* [in] */ ICancellationSignal* cancellationSignal)
{
    Elastos_AwPdfExporter_nativeExportToPdf(THIS_PROBE(IInterface), nativeAwPdfExporter, fd, TO_IINTERFACE(cancellationSignal));
}
//callback function definition
void AwPdfExporter::SetNativeAwPdfExporter(
    /* [in] */ IInterface* obj,
    /* [in] */ Handle64 nativePdfExporter)
{
    AutoPtr<AwPdfExporter> mObj = (AwPdfExporter*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwPdfExporter::SetNativeAwPdfExporter, mObj is NULL");
        return;
    }
    mObj->SetNativeAwPdfExporter(nativePdfExporter);
}

void AwPdfExporter::DidExportPdf(
    /* [in] */ IInterface* obj,
    /* [in] */ Boolean success)
{
    AutoPtr<AwPdfExporter> mObj = (AwPdfExporter*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwPdfExporter::DidExportPdf, mObj is NULL");
        return;
    }
    mObj->DidExportPdf(success);
}

Int32 AwPdfExporter::GetPageWidth(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwPdfExporter> mObj = (AwPdfExporter*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwPdfExporter::GetPageWidth, mObj is NULL");
        return 0;
    }
    return mObj->GetPageWidth();
}

Int32 AwPdfExporter::GetPageHeight(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwPdfExporter> mObj = (AwPdfExporter*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwPdfExporter::GetPageHeight, mObj is NULL");
        return 0;
    }
    return mObj->GetPageHeight();
}

Int32 AwPdfExporter::GetDpi(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwPdfExporter> mObj = (AwPdfExporter*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwPdfExporter::GetDpi, mObj is NULL");
        return 0;
    }
    return mObj->GetDpi();
}

Int32 AwPdfExporter::GetLeftMargin(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwPdfExporter> mObj = (AwPdfExporter*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwPdfExporter::GetLeftMargin, mObj is NULL");
        return 0;
    }
    return mObj->GetLeftMargin();
}

Int32 AwPdfExporter::GetRightMargin(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwPdfExporter> mObj = (AwPdfExporter*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwPdfExporter::GetRightMargin, mObj is NULL");
        return 0;
    }
    return mObj->GetRightMargin();
}

Int32 AwPdfExporter::GetTopMargin(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwPdfExporter> mObj = (AwPdfExporter*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwPdfExporter::GetTopMargin, mObj is NULL");
        return 0;
    }
    return mObj->GetTopMargin();
}

Int32 AwPdfExporter::GetBottomMargin(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwPdfExporter> mObj = (AwPdfExporter*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "AwPdfExporter::GetBottomMargin, mObj is NULL");
        return 0;
    }
    return mObj->GetBottomMargin();
}

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
