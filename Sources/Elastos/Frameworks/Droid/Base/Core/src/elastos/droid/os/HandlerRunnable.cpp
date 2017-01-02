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

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/os/HandlerRunnable.h"

using Elastos::Droid::Os::EIID_IHandler;
using Elastos::Core::EIID_IRunnable;

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL(HandlerRunnable, Handler, IRunnable)

HandlerRunnable::HandlerRunnable(
    /* [in] */ Boolean async)
{
    Handler::constructor(async);
}

HandlerRunnable::HandlerRunnable(
    /* [in] */ ILooper* looper,
    /* [in] */ Boolean async)
    : Handler(looper, async)
{
}

HandlerRunnable::HandlerRunnable(
    /* [in] */ IHandlerCallback* callback,
    /* [in] */ Boolean takeStrongRefOfCallback,
    /* [in] */ Boolean async)
    : Handler(callback, takeStrongRefOfCallback, async)
{
}

HandlerRunnable::HandlerRunnable(
    /* [in] */ ILooper* looper,
    /* [in] */ IHandlerCallback* callback,
    /* [in] */ Boolean takeStrongRefOfCallback,
    /* [in] */ Boolean async)
    : Handler(looper, callback, takeStrongRefOfCallback, async)
{
}

HandlerRunnable::~HandlerRunnable()
{
}

ECode HandlerRunnable::HandleMessage(
    /* [in] */ IMessage* msg)
{
    return NOERROR;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
