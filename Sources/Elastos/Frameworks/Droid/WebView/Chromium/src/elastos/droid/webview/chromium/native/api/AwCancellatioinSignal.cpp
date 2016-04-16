
#include "Elastos.Droid.Os.h"
#include "elastos/droid/webkit/webview/chromium/native/api/AwCancellatioinSignal.h"
#include "elastos/droid/webkit/webview/chromium/native/api/CancellationSignal_dec.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::CCancellationSignal;
using Elastos::Droid::Os::ICancellationSignal;
using Elastos::Droid::Os::ICancellationSignalOnCancelListener;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Api {

Boolean AwCancellationSignal::IsCanceled(
    /* [in] */ IInterface* obj)
{
    AutoPtr<ICancellationSignal> mObj = ICancellationSignal::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwCancellationSignal", "AwCancellationSignal::IsCanceled, mObj is NULL");
        return FALSE;
    }
    Boolean isCanceled;
    mObj->IsCanceled(&isCanceled);
    return isCanceled;
}

void AwCancellationSignal::ThrowIfCanceled(
    /* [in] */ IInterface* obj)
{
    AutoPtr<ICancellationSignal> mObj = ICancellationSignal::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwCancellationSignal", "AwCancellationSignal::ThrowIfCanceled, mObj is NULL");
        return;
    }
    mObj->ThrowIfCanceled();
}

void AwCancellationSignal::Cancel(
    /* [in] */ IInterface* obj)
{
    AutoPtr<ICancellationSignal> mObj = ICancellationSignal::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwCancellationSignal", "AwCancellationSignal::Cancel, mObj is NULL");
        return;
    }
    mObj->Cancel();
}

void AwCancellationSignal::SetOnCancelListener(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* p0)
{
    AutoPtr<ICancellationSignal> mObj = ICancellationSignal::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwCancellationSignal", "AwCancellationSignal::SetOnCancelListener, mObj is NULL");
        return;
    }
    AutoPtr<ICancellationSignalOnCancelListener> listener = ICancellationSignalOnCancelListener::Probe(p0);
    mObj->SetOnCancelListener(listener);
}

AutoPtr<IInterface> AwCancellationSignal::Constructor()
{
    AutoPtr<ICancellationSignal> cancellationSignal;
    CCancellationSignal::New((ICancellationSignal**)&cancellationSignal);
    return cancellationSignal;
}

} // namespace Api
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
