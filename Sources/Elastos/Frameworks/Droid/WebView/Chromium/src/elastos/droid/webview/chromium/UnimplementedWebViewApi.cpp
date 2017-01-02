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

#include "elastos/droid/webkit/webview/chromium/UnimplementedWebViewApi.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::CThrowable;
using Elastos::Core::IStackTraceElement;
using Elastos::Core::IThrowable;
using Elastos::Utility::Logging::Logger;


namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {

//=====================================================================
//      UnimplementedWebViewApi::UnimplementedWebViewApiException
//=====================================================================
UnimplementedWebViewApi::UnimplementedWebViewApiException::UnimplementedWebViewApiException()
{
    // ==================before translated======================
    // super();
}

//=====================================================================
//                       UnimplementedWebViewApi
//=====================================================================
String UnimplementedWebViewApi::TAG("UnimplementedWebViewApi");
Boolean UnimplementedWebViewApi::THROW = FALSE;
Boolean UnimplementedWebViewApi::FULL_TRACE = FALSE;

ECode UnimplementedWebViewApi::Invoke()
{
    // ==================before translated======================
    // if (THROW) {
    //     throw new UnimplementedWebViewApiException();
    // } else {
    //     if (FULL_TRACE) {
    //         Log.w(TAG, "Unimplemented WebView method called in: " +
    //               Log.getStackTraceString(new Throwable()));
    //     } else {
    //         StackTraceElement[] trace = new Throwable().getStackTrace();
    //         // The stack trace [0] index is this method (invoke()).
    //         StackTraceElement unimplementedMethod = trace[1];
    //         StackTraceElement caller = trace[2];
    //         Log.w(TAG, "Unimplemented WebView method " + unimplementedMethod.getMethodName() +
    //                 " called from: " + caller.toString());
    //     }
    // }

    if (THROW) {
        //throw new UnimplementedWebViewApiException();
        assert(0);
    }
    else {
        if (FULL_TRACE) {
            assert(0);
            Logger::W(TAG, String("Unimplemented WebView method called in: "));// +
                // Log.getStackTraceString(new Throwable()));
        }
        else {
            AutoPtr<IThrowable> throwable;
            CThrowable::New((IThrowable**)&throwable);
            AutoPtr< ArrayOf<IStackTraceElement*> > trace;
            throwable->GetStackTrace((ArrayOf<IStackTraceElement*>**)&trace);

            // The stack trace [0] index is this method (invoke()).
            AutoPtr<IStackTraceElement> unimplementedMethod = (*trace)[1];
            AutoPtr<IStackTraceElement> caller = (*trace)[2];

            String methodName;
            //unimplementedMethod->GetMethodName(&methodName);
            IObject* objTmp = IObject::Probe(caller);
            String callerStr;
            objTmp->ToString(&callerStr);
            Logger::W(TAG, String("Unimplemented WebView method ") + methodName +
                String(" called from: ") + callerStr);
        }
    }

    return NOERROR;
}

} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos


