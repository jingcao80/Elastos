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

#include "EventListenerProxy.h"

namespace Elastos {
namespace Utility {

CAR_INTERFACE_IMPL_2(EventListenerProxy, Object, IEventListenerProxy, IEventListener)

EventListenerProxy::EventListenerProxy(
    /* [in] */ IEventListener* listener)
    : mListener(listener)
{}

ECode EventListenerProxy::GetListener(
    /* [out] */ IEventListener** evl)
{
    VALIDATE_NOT_NULL(evl)
    *evl = mListener;
    REFCOUNT_ADD(*evl)
    return NOERROR;
}

} // namespace Utility
} // namespace Elastos