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

#ifndef __NATIVEINPUTWINDOWHANDLE_H__
#define __NATIVEINPUTWINDOWHANDLE_H__

#include "elastos/droid/server/input/InputWindowHandle.h"
#include <inputflinger/InputWindow.h>

namespace Elastos {
namespace Droid {
namespace Server {
namespace Input {

class NativeInputWindowHandle
    : public android::InputWindowHandle
{
public:
    NativeInputWindowHandle(
        /* [in] */ const android::sp<android::InputApplicationHandle>& inputApplicationHandle,
        /* [in] */ IWeakReference* obj);

    virtual ~NativeInputWindowHandle();

    CARAPI_(AutoPtr<Elastos::Droid::Server::Input::InputWindowHandle>) getInputWindowHandleObj();

    virtual bool updateInfo();

private:
    AutoPtr<IWeakReference> mObjWeak;
};

extern android::sp<NativeInputWindowHandle> GetNativeInputWindowHandle(
    /* [in] */ Elastos::Droid::Server::Input::InputWindowHandle* inputWindowHandleObj);

} // namespace Input
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__NATIVEINPUTWINDOWHANDLE_H__
