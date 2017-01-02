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

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/CWindowIdFocusObserver.h"
#include "elastos/droid/view/WindowIdFocusObserver.h"
#include <elastos/core/AutoLock.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;

namespace Elastos {
namespace Droid {
namespace View {

CAR_OBJECT_IMPL(CWindowIdFocusObserver);

CAR_INTERFACE_IMPL(CWindowIdFocusObserver, Object, IIWindowFocusObserver);

ECode CWindowIdFocusObserver::constructor(
    /* [in] */ IWindowIdFocusObserver* observer)
{
    mObserver = observer;
    return NOERROR;
}

ECode CWindowIdFocusObserver::FocusGained(
    /* [in] */ IBinder* inputToken)
{
    WindowIdFocusObserver* observer = (WindowIdFocusObserver*)mObserver.Get();
    AutoPtr<IWindowId> token;
    AutoPtr<IHashMap> map = observer->mRegistrations;
    {    AutoLock syncLock(map);
        AutoPtr<IInterface> obj;
        map->Get(inputToken, (IInterface**)&obj);
        token = IWindowId::Probe(obj);
    }
    if (observer->mHandler != NULL) {
        AutoPtr<IMessage> msg;
        Boolean reslult = FALSE;
        observer->mHandler->ObtainMessage(1, token, (IMessage**)&msg);
        observer->mHandler->SendMessage(msg, &reslult);
    } else {
        observer->OnFocusGained(token);
    }
    return NOERROR;
}

ECode CWindowIdFocusObserver::FocusLost(
    /* [in] */ IBinder* inputToken)
{
    WindowIdFocusObserver* observer = (WindowIdFocusObserver*)mObserver.Get();
    AutoPtr<IWindowId> token;
    AutoPtr<IHashMap> map = observer->mRegistrations;
    {    AutoLock syncLock(map);
        AutoPtr<IInterface> obj;
        map->Get(inputToken, (IInterface**)&obj);
        token = IWindowId::Probe(obj);
    }
    if (observer->mHandler != NULL) {
        AutoPtr<IMessage> msg;
        Boolean reslult = FALSE;
        observer->mHandler->ObtainMessage(2, token, (IMessage**)&msg);
        observer->mHandler->SendMessage(msg, &reslult);
    } else {
        observer->OnFocusLost(token);
    }
    return NOERROR;
}

ECode CWindowIdFocusObserver::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

} // namespace View
} // namepsace Droid
} // namespace Elastos
