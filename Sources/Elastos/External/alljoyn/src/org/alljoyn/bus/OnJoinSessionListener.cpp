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

#include "org/alljoyn/bus/OnJoinSessionListener.h"
#include "org/alljoyn/bus/NativeOnJoinSessionListener.h"

namespace Org {
namespace Alljoyn {
namespace Bus {

CAR_INTERFACE_IMPL(OnJoinSessionListener, Object, IOnJoinSessionListener)

OnJoinSessionListener::OnJoinSessionListener()
    : mHandle(0)
{}

OnJoinSessionListener::~OnJoinSessionListener()
{
    Destroy();
}

ECode OnJoinSessionListener::constructor()
{
    Create();
    return NOERROR;
}

void OnJoinSessionListener::Create()
{
    mHandle = reinterpret_cast<Int64>(new NativeOnJoinSessionListener((IOnJoinSessionListener*)this));
}

void OnJoinSessionListener::Destroy()
{
    if (mHandle != 0) {
        NativeOnJoinSessionListener* t = reinterpret_cast<NativeOnJoinSessionListener*>(mHandle);
        delete t;
        mHandle = 0;
    }
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org