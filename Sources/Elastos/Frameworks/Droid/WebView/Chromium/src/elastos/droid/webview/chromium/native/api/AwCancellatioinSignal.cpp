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
