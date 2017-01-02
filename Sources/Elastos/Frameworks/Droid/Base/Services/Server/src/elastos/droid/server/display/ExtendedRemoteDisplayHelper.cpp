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

#include <Elastos.CoreLibrary.Core.h>
#include "elastos/droid/server/display/ExtendedRemoteDisplayHelper.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::CPathClassLoader;
using Elastos::Core::IClassLoader;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Display {

const String ExtendedRemoteDisplayHelper::TAG("ExtendedRemoteDisplayHelper");

AutoPtr<IClassInfo> ExtendedRemoteDisplayHelper::sExtRemoteDisplayClass;
AutoPtr<IMethodInfo> ExtendedRemoteDisplayHelper::sExtRemoteDisplayListen;
AutoPtr<IMethodInfo> ExtendedRemoteDisplayHelper::sExtRemoteDisplayDispose;

ExtendedRemoteDisplayHelper::StaticInitializer::StaticInitializer()
{
    // todo
    //Check availability of ExtendedRemoteDisplay runtime
    // try {
    //     sExtRemoteDisplayClass = Class.forName("com.qualcomm.wfd.ExtendedRemoteDisplay");
    // } catch (Throwable t) {
    //     Slog.i(TAG, "ExtendedRemoteDisplay Not available.");
    // }

    // if(sExtRemoteDisplayClass != null) {
    //     // If ExtendedRemoteDisplay is available find the methods
    //     Slog.i(TAG, "ExtendedRemoteDisplay Is available. Find Methods");
    //     try {
    //         Class args[] = {
    //                            String.class,
    //                            RemoteDisplay.Listener.class,
    //                            Handler.class, Context.class
    //                        };
    //         sExtRemoteDisplayListen = sExtRemoteDisplayClass.getDeclaredMethod("listen", args);
    //     } catch (Throwable t) {
    //         Slog.i(TAG, "ExtendedRemoteDisplay.listen Not available.");
    //     }

    //     try {
    //         Class args[] = {};
    //         sExtRemoteDisplayDispose = sExtRemoteDisplayClass.getDeclaredMethod("dispose", args);
    //     } catch (Throwable t) {
    //         Slog.i(TAG, "ExtendedRemoteDisplay.dispose Not available.");
    //     }
    // }

    // //Check availability of ExtendedRemoteDisplay runtime
    // // try {
    // AutoPtr<IClassLoader> cl;
    // ECode ec = CPathClassLoader::New(String(""), (IClassLoader**)&cl);
    // if (FAILED(ec)) {
    //     Slogger::I(TAG, "ExtendedRemoteDisplay Not available.");
    // }
    // cl->LoadClass(String("com.qualcomm.wfd.ExtendedRemoteDisplay"), (IClassInfo**)&sExtRemoteDisplayClass);
    // // } catch (Throwable t) {
    // //     Slog.i(TAG, "ExtendedRemoteDisplay Not available.");
    // // }

    // if(sExtRemoteDisplayClass != NULL) {
    //     // If ExtendedRemoteDisplay is available find the methods
    //     Slogger::I(TAG, "ExtendedRemoteDisplay Is available. Find Methods");
    //     // try {
    //     // Class args[] = {
    //     //                    String.class,
    //     //                    RemoteDisplay.Listener.class,
    //     //                    Handler.class, Context.class
    //     //                };
    //     ec = sExtRemoteDisplayClass->GetMethodInfo(String("listen"), String(NULL),
    //             (IMethodInfo**)&sExtRemoteDisplayListen);
    //     if (FAILED(ec)) {
    //         Slogger::I(TAG, "ExtendedRemoteDisplay.listen Not available.");
    //     }
    //     // } catch (Throwable t) {
    //     //     Slog.i(TAG, "ExtendedRemoteDisplay.listen Not available.");
    //     // }

    //     // try {
    //     // Class args[] = {};
    //     ec = sExtRemoteDisplayClass->GetMethodInfo(String("dispose"), String(NULL),
    //             (IMethodInfo**)&sExtRemoteDisplayDispose);
    //     if (FAILED(ec)) {
    //         Slogger::I(TAG, "ExtendedRemoteDisplay.dispose Not available.");
    //     }
    //     // } catch (Throwable t) {
    //     //     Slog.i(TAG, "ExtendedRemoteDisplay.dispose Not available.");
    //     // }
    // }
}
INIT_PROI_4 const ExtendedRemoteDisplayHelper::StaticInitializer ExtendedRemoteDisplayHelper::sInitializer;

ECode ExtendedRemoteDisplayHelper::Listen(
    /* [in] */ const String& iface,
    /* [in] */ IRemoteDisplayListener* listener,
    /* [in] */ IHandler* handler,
    /* [in] */ IContext* context,
    /* [out] */ IInterface** result)
{
    // AutoPtr<IInterface> extRemoteDisplay;
    // Slogger::I(TAG, "ExtendedRemoteDisplay.listen");

    // if(sExtRemoteDisplayListen != NULL && sExtRemoteDisplayDispose != NULL){
    //     // try {
    //     AutoPtr<IArgumentList> argumentList;
    //     methodInfo->CreateArgumentList((IArgumentList**)&argumentList);
    //     argumentList->SetInputArgumentOfString(0, iface);
    //     argumentList->SetInputArgumentOfObjectPtr(1, listener);
    //     argumentList->SetInputArgumentOfObjectPtr(2, handler);
    //     argumentList->SetInputArgumentOfObjectPtr(3, context);
    //     ECode ec = sExtRemoteDisplayListen->Invoke(NULL, argumentList);
    //     // } catch (InvocationTargetException e) {
    //     //     Slog.i(TAG, "ExtendedRemoteDisplay.listen - InvocationTargetException");
    //     //     Throwable cause = e.getCause();
    //     //     if (cause instanceof RuntimeException) {
    //     //         throw (RuntimeException) cause;
    //     //     } else if (cause instanceof Error) {
    //     //         throw (Error) cause;
    //     //     } else {
    //     //         throw new RuntimeException(e);
    //     //     }
    //     // } catch (IllegalAccessException e) {
    //     //     Slog.i(TAG, "ExtendedRemoteDisplay.listen -IllegalAccessException");
    //     //     e.printStackTrace();
    //     // }
    // }
    // return extRemoteDisplay;

    return NOERROR;
}

ECode ExtendedRemoteDisplayHelper::Dispose(
    /* [in] */ IInterface* extRemoteDisplay)
{
    return NOERROR;
}

Boolean ExtendedRemoteDisplayHelper::IsAvailable()
{
    return FALSE;
}

} // namespace Display
} // namespace Server
} // namespace Droid
} // namespace Elastos