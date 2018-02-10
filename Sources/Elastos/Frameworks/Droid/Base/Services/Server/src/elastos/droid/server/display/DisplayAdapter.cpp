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

#include <Elastos.Droid.Content.h>
#include "elastos/droid/server/display/DisplayAdapter.h"
#include "elastos/droid/server/display/DisplayDevice.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Display {

const Int32 DisplayAdapter::DISPLAY_DEVICE_EVENT_ADDED;
const Int32 DisplayAdapter::DISPLAY_DEVICE_EVENT_CHANGED;
const Int32 DisplayAdapter::DISPLAY_DEVICE_EVENT_REMOVED;

// Called with SyncRoot lock held.
DisplayAdapter::DisplayAdapter(
    /* [in] */ Object* syncRoot,
    /* [in] */ IContext* context,
    /* [in] */ IHandler* handler,
    /* [in] */ IDisplayAdapterListener* listener,
    /* [in] */ const String& name)
    : mSyncRoot(syncRoot)
    , mContext(context)
    , mHandler(handler)
    , mListener(listener)
    , mName(name)
{}

DisplayAdapter::~DisplayAdapter()
{}

Object* DisplayAdapter::GetSyncRoot()
{
    return mSyncRoot;
}

AutoPtr<IContext> DisplayAdapter::GetContext()
{
    return mContext;
}

AutoPtr<IHandler> DisplayAdapter::GetHandler()
{
    return mHandler;
}

String DisplayAdapter::GetName()
{
    return mName;
}

void DisplayAdapter::RegisterLocked()
{}

void DisplayAdapter::DumpLocked(
    /* [in] */ IPrintWriter* pw)
{}

void DisplayAdapter::SendDisplayDeviceEventLocked(
    /* [in] */ DisplayDevice* device,
    /* [in] */ Int32 event)
{
    AutoPtr<IRunnable> runnable = new SendDisplayDeviceEventRunnable(device, event, this);
    Boolean result;
    mHandler->Post(runnable, &result);
}

void DisplayAdapter::SendTraversalRequestLocked()
{
    AutoPtr<IRunnable> runnable = new SendTraversalRequestRunnable(this);
    Boolean result;
    mHandler->Post(runnable, &result);
}

DisplayAdapter::SendDisplayDeviceEventRunnable::SendDisplayDeviceEventRunnable(
    /* [in] */ DisplayDevice* device,
    /* [in] */ Int32 event,
    /* [in] */ DisplayAdapter* host)
    : mDevice(device)
    , mEvent(event)
    , mHost(host)
{}

ECode DisplayAdapter::SendDisplayDeviceEventRunnable::Run()
{
    if (mHost->mListener)
        mHost->mListener->OnDisplayDeviceEvent((HANDLE)mDevice.Get(), mEvent);
    return NOERROR;
}

DisplayAdapter::SendTraversalRequestRunnable::SendTraversalRequestRunnable(
    /* [in] */ DisplayAdapter* host)
    : mHost(host)
{}

ECode DisplayAdapter::SendTraversalRequestRunnable::Run()
{
    if (mHost->mListener)
        mHost->mListener->OnTraversalRequested();
    return NOERROR;
}

} // namespace Display
} // namespace Server
} // namespace Droid
} // namespace Elastos
