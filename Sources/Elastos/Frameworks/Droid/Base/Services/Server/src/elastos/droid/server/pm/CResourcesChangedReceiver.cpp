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

#include "elastos/droid/server/pm/CResourcesChangedReceiver.h"

using Elastos::Droid::Content::EIID_IIntentReceiver;
using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

CAR_INTERFACE_IMPL_2(CResourcesChangedReceiver, Object, IIntentReceiver, IBinder)

CAR_OBJECT_IMPL(CResourcesChangedReceiver)

ECode CResourcesChangedReceiver::constructor(
    /* [in] */ IIPackageManager* owner,
    /* [in] */ ISet* keys,
    /* [in] */ Boolean reportStatus)
{
    mOwner = (CPackageManagerService*)owner;
    mKeys = keys;
    mReportStatus = reportStatus;
    return NOERROR;
}

ECode CResourcesChangedReceiver::PerformReceive(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& data,
    /* [in] */ IBundle* extras,
    /* [in] */ Boolean ordered,
    /* [in] */ Boolean sticky,
    /* [in] */ Int32 sendingUser)
{
    AutoPtr<IMessage> msg;
    mOwner->mHandler->ObtainMessage(CPackageManagerService::UPDATED_MEDIA_STATUS, (IMessage**)&msg);
    msg->SetObj(mKeys);
    msg->SetArg1(mReportStatus ? 1 : 0);
    msg->SetArg2(1);
    Boolean result;
    return mOwner->mHandler->SendMessage(msg, &result);
}

ECode CResourcesChangedReceiver::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    return Object::ToString(str);
}

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos
