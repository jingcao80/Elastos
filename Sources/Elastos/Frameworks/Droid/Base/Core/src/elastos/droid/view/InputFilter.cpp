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

#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/InputFilter.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::View::EIID_IInputFilter;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace View {

const Int32 InputFilter::MSG_INSTALL = 1;
const Int32 InputFilter::MSG_UNINSTALL = 2;
const Int32 InputFilter::MSG_INPUT_EVENT = 3;

//==========================================================================
// InputFilter::H
//==========================================================================
InputFilter::H::H(
    /* [in] */ ILooper* loop,
    /* [in] */ InputFilter* host)
    : Handler(loop)
    , mHost(host)
{
}

ECode InputFilter::H::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    switch (what) {
        case InputFilter::MSG_INSTALL:
            mHost->mInputFilterHost = IIInputFilterHost::Probe(obj);
            // if (mInboundInputEventConsistencyVerifier != null) {
            //     mInboundInputEventConsistencyVerifier.reset();
            // }
            // if (mOutboundInputEventConsistencyVerifier != null) {
            //     mOutboundInputEventConsistencyVerifier.reset();
            // }
            mHost->OnInstalled();
            break;

        case InputFilter::MSG_UNINSTALL:
            mHost->OnUninstalled();
            mHost->mInputFilterHost = NULL;
            break;

        case InputFilter::MSG_INPUT_EVENT: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            AutoPtr<IInputEvent> event = IInputEvent::Probe(obj);
            // if (mInboundInputEventConsistencyVerifier != null) {
            //     mInboundInputEventConsistencyVerifier.onInputEvent(event, 0);
            // }
            mHost->OnInputEvent(event, arg1);
            event->Recycle();
            break;
        }
    }
    return NOERROR;
}

//==========================================================================
// InputFilter
//==========================================================================
CAR_INTERFACE_IMPL_3(InputFilter, Object, IInputFilter, IIInputFilter, IBinder)

InputFilter::InputFilter()
{}

InputFilter::~InputFilter()
{}

ECode InputFilter::constructor(
    /* [in] */ ILooper* loop)
{
    mH = new H(loop, this);
    return NOERROR;
}

ECode InputFilter::Install(
    /* [in] */ IIInputFilterHost* host)
{
    AutoPtr<IMessage> msg;
    mH->ObtainMessage(MSG_INSTALL, host, (IMessage**)&msg);
    return msg->SendToTarget();
}

ECode InputFilter::Uninstall()
{
    Boolean result;
    return mH->SendEmptyMessage(MSG_UNINSTALL, &result);
}

ECode InputFilter::FilterInputEvent(
    /* [in] */ IInputEvent* event,
    /* [in] */ Int32 policyFlags)
{
    AutoPtr<IMessage> msg;
    mH->ObtainMessage(MSG_INPUT_EVENT, policyFlags, 0, event, (IMessage**)&msg);
    return msg->SendToTarget();
}

ECode InputFilter::SendInputEvent(
    /* [in] */ IInputEvent* event,
    /* [in] */ Int32 policyFlags)
{
    if (event == NULL) {
        Logger::E("InputFilter", "event must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mInputFilterHost == NULL) {
        Logger::E("InputFilter", "Cannot send input event because the input filter " \
            "is not installed.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    // if (mOutboundInputEventConsistencyVerifier != null) {
    //     mOutboundInputEventConsistencyVerifier.onInputEvent(event, 0);
    // }
    return mInputFilterHost->SendInputEvent(event, policyFlags);
}

ECode InputFilter::OnInputEvent(
    /* [in] */ IInputEvent* event,
    /* [in] */ Int32 policyFlags)
{
    return SendInputEvent(event, policyFlags);
}

ECode InputFilter::OnInstalled()
{
    return NOERROR;
}

ECode InputFilter::OnUninstalled()
{
    return NOERROR;
}

}   //namespace View
}   //namespace Droid
}   //namespace Elastos
