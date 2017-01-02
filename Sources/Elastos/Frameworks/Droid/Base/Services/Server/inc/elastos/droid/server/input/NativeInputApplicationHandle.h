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

#ifndef __NATIVEINPUTAPPLICATIONHANDLE_H__
#define __NATIVEINPUTAPPLICATIONHANDLE_H__

#include "elastos/droid/server/input/InputApplicationHandle.h"
#include <inputflinger/InputApplication.h>

namespace Elastos {
namespace Droid {
namespace Server {
namespace Input {

class NativeInputApplicationHandle
    : public android::InputApplicationHandle
{
public:
    NativeInputApplicationHandle(
        /* [in] */ IWeakReference* obj);

    virtual ~NativeInputApplicationHandle();

    CARAPI_(AutoPtr<Elastos::Droid::Server::Input::InputApplicationHandle>) getInputApplicationHandleObj();

    virtual bool updateInfo();

private:
    AutoPtr<IWeakReference> mObjWeak;
};

extern android::sp<android::InputApplicationHandle> GetNativeInputApplicationHandle(
    /* [in] */ Elastos::Droid::Server::Input::InputApplicationHandle* inputApplicationHandleObj);

} // namespace Input
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__NATIVEINPUTAPPLICATIONHANDLE_H__
