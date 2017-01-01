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

#include "org/alljoyn/bus/OnPingListener.h"
#include "org/alljoyn/bus/NativeOnPingListener.h"

namespace Org {
namespace Alljoyn {
namespace Bus {

CAR_INTERFACE_IMPL(OnPingListener, Object, IOnPingListener)

OnPingListener::OnPingListener()
    : mHandle(0)
{}

OnPingListener::~OnPingListener()
{
    Destroy();
}

ECode OnPingListener::constructor()
{
    Create();
    return NOERROR;
}

ECode OnPingListener::OnPing(
    /* [in] */ Int32 status,
    /* [in] */ IInterface* context)
{
    return NOERROR;
}

void OnPingListener::Create()
{
    mHandle = reinterpret_cast<Int64>(new NativeOnPingListener((IOnPingListener*)this));
}

void OnPingListener::Destroy()
{
    if (mHandle != 0) {
        NativeOnPingListener* t = reinterpret_cast<NativeOnPingListener*>(mHandle);
        delete t;
        mHandle = 0;
    }
}
} // namespace Bus
} // namespace Alljoyn
} // namespace Org

