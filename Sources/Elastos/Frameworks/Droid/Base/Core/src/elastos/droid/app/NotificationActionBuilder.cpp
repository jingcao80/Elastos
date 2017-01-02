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

#include "elastos/droid/app/NotificationActionBuilder.h"
#include "elastos/droid/app/CNotificationAction.h"
#include "elastos/droid/os/CBundle.h"
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Droid::Os::CBundle;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollection;
using Elastos::Utility::ICollections;

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(NotificationActionBuilder, Object, INotificationActionBuilder)

NotificationActionBuilder::NotificationActionBuilder()
    : mIcon(0)
{}

ECode NotificationActionBuilder::constructor(
    /* [in] */ Int32 icon,
    /* [in] */ ICharSequence* title,
    /* [in] */ IPendingIntent* intent)
{
    AutoPtr<IBundle> bundle;
    CBundle::New((IBundle**)&bundle);
    return constructor(icon, title, intent, bundle, NULL);
}

ECode NotificationActionBuilder::constructor(
    /* [in] */ INotificationAction* action)
{
    AutoPtr<IBundle> extras;
    action->GetExtras((IBundle**)&extras);
    AutoPtr<IBundle> bundle;
    CBundle::New(extras, (IBundle**)&bundle);
    AutoPtr<ArrayOf<IRemoteInput*> > remoteInputs;
    action->GetRemoteInputs((ArrayOf<IRemoteInput*>**)&remoteInputs);
    return constructor(((CNotificationAction*)action)->mIcon, ((CNotificationAction*)action)->mTitle, ((CNotificationAction*)action)->mActionIntent, bundle,
            remoteInputs);
}

ECode NotificationActionBuilder::constructor(
    /* [in] */ Int32 icon,
    /* [in] */ ICharSequence* title,
    /* [in] */ IPendingIntent* intent,
    /* [in] */ IBundle* extras,
    /* [in] */ ArrayOf<IRemoteInput*>* remoteInputs)
{
    mIcon = icon;
    mTitle = title;
    mIntent = intent;
    mExtras = extras;
    if (remoteInputs != NULL) {
        CArrayList::New(remoteInputs->GetLength(), (IArrayList**)&mRemoteInputs);
        Boolean bNoUse;
        AutoPtr<ICollections> helper;
        CCollections::AcquireSingleton((ICollections**)&helper);
        FAIL_RETURN(helper->AddAll(ICollection::Probe(mRemoteInputs), (ArrayOf<IInterface*>*) remoteInputs, &bNoUse))
    }
    return NOERROR;
}

ECode NotificationActionBuilder::AddExtras(
    /* [in] */ IBundle* extras)
{
    if (extras != NULL) {
        mExtras->PutAll(extras);
    }
    return NOERROR;
}

ECode NotificationActionBuilder::GetExtras(
    /* [out] */ IBundle** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mExtras;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode NotificationActionBuilder::AddRemoteInput(
    /* [in] */ IRemoteInput* remoteInput)
{
    if (mRemoteInputs == NULL) {
        CArrayList::New((IArrayList**)&mRemoteInputs);
    }
    mRemoteInputs->Add(remoteInput);
    return NOERROR;
}

ECode NotificationActionBuilder::Extend(
    /* [in] */ INotificationActionExtender* extender)
{
    return extender->Extend(this);
}

ECode NotificationActionBuilder::Build(
    /* [out] */ INotificationAction** result)
{
    AutoPtr<ArrayOf<IRemoteInput*> > remoteInputs;
    if (mRemoteInputs != NULL) {
        Int32 size;
        mRemoteInputs->GetSize(&size);
        AutoPtr<ArrayOf<IInterface*> > tmpArray;
        mRemoteInputs->ToArray((ArrayOf<IInterface*>**)&tmpArray);
        remoteInputs = ArrayOf<IRemoteInput*>::Alloc(size);
        for (Int32 i = 0; i < size; ++i) {
            remoteInputs->Set(i, IRemoteInput::Probe((*tmpArray)[i]));
        }
    }
    return CNotificationAction::New(mIcon, mTitle, mIntent, mExtras, remoteInputs, result);
}

} // namespace App
} // namespace Droid
} // namespace Elastos
