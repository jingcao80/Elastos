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

#include "elastos/droid/app/CTaskStackBuilder.h"
#include "elastos/droid/app/CPendingIntentHelper.h"
#include "elastos/droid/os/CUserHandle.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/CIntentHelper.h"
#include "elastos/droid/content/CComponentName.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntentHelper;
using Elastos::Droid::Content::CIntentHelper;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;

namespace Elastos {
namespace Droid {
namespace App {

const String CTaskStackBuilder::TAG("TaskStackBuilder");

CAR_INTERFACE_IMPL(CTaskStackBuilder, Object, ITaskStackBuilder)

CAR_OBJECT_IMPL(CTaskStackBuilder)

CTaskStackBuilder::CTaskStackBuilder()
{
}

CTaskStackBuilder::~CTaskStackBuilder()
{
}

ECode CTaskStackBuilder::constructor(
    /* [in] */ IContext *context)
{
    mSourceContext = context;
    return NOERROR;
}

AutoPtr<ITaskStackBuilder> CTaskStackBuilder::Create(
    /* [in] */ IContext* context)
{
    AutoPtr<ITaskStackBuilder> cb;
    CTaskStackBuilder::New(context, (ITaskStackBuilder**)&cb);
    return cb;
}

ECode CTaskStackBuilder::AddNextIntent(
    /* [in] */ IIntent *nextIntent)
{
    mIntents.PushBack(nextIntent);
    return NOERROR;
}

ECode CTaskStackBuilder::AddNextIntentWithParentStack(
    /* [in] */ IIntent *nextIntent)
{
    VALIDATE_NOT_NULL(nextIntent);

    AutoPtr<IComponentName> target;
    nextIntent->GetComponent((IComponentName**)&target);
    if (target == NULL) {
        AutoPtr<IPackageManager> pm;
        mSourceContext->GetPackageManager((IPackageManager**)&pm);
        nextIntent->ResolveActivity(pm, (IComponentName**)&target);
    }
    if (target != NULL) {
        AddParentStack(target);
    }
    AddNextIntent(nextIntent);
    return NOERROR;
}

ECode CTaskStackBuilder::AddParentStack(
    /* [in] */ IActivity *sourceActivity)
{
    VALIDATE_NOT_NULL(sourceActivity);
    AutoPtr<IIntent> parent;
    sourceActivity->GetParentActivityIntent((IIntent**)&parent);
    if (parent != NULL) {
        // We have the actual parent intent, build the rest from static metadata
        // then add the direct parent intent to the end.
        AutoPtr<IComponentName> target;
        parent->GetComponent((IComponentName**)&target);
        if (target == NULL) {
            AutoPtr<IPackageManager> pm;
            mSourceContext->GetPackageManager((IPackageManager**)&pm);
            parent->ResolveActivity(pm ,(IComponentName**)&target);
        }
        AddParentStack(target);
        AddNextIntent(parent);
    }
    return NOERROR;
}

ECode CTaskStackBuilder::AddParentStack(
    /* [in] */ IComponentName *sourceActivityName)
{
    Int32 insertAt = mIntents.GetSize();
    AutoPtr<IPackageManager> pm;
    mSourceContext->GetPackageManager((IPackageManager**)&pm);
    //try {
        AutoPtr<IActivityInfo> info;
        pm->GetActivityInfo(sourceActivityName, 0, (IActivityInfo**)&info);
        if (info != NULL) {
            String parentActivity, packagename;
            info->GetParentActivityName(&parentActivity);
            AutoPtr<IIntentHelper> helper;
            CIntentHelper::AcquireSingleton((IIntentHelper**)&helper);
            IPackageItemInfo* pii = IPackageItemInfo::Probe(info);
            while (!parentActivity.IsNull()) {
                AutoPtr<IComponentName> target;
                pii->GetPackageName(&packagename);
                CComponentName::New(packagename, parentActivity, (IComponentName**)&target);

                info = NULL;
                pm->GetActivityInfo(target, 0, (IActivityInfo**)&info);
                info->GetParentActivityName(&parentActivity);
                AutoPtr<IIntent> parent;
                if (parentActivity.IsNull() && insertAt == 0) {
                    helper->MakeMainActivity(target, (IIntent**)&parent);
                }
                else {
                    CIntent::New((IIntent**)&parent);
                    parent->SetComponent(target);
                }
                mIntents.Insert(insertAt, parent);
            }
        }
        else {
            Logger::E(TAG, "Bad ComponentName while traversing activity parent metadata");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    //} catch (NameNotFoundException e) {
    //    Log.e(TAG, "Bad ComponentName while traversing activity parent metadata");
    //    throw new IllegalArgumentException(e);
    //}
    return NOERROR;
}

ECode CTaskStackBuilder::GetIntentCount(
    /* [out] */ Int32 *num)
{
    VALIDATE_NOT_NULL(num);
    *num = mIntents.GetSize();
    return NOERROR;
}

ECode CTaskStackBuilder::EditIntentAt(
    /* [in] */ Int32 index,
    /* [out] */ IIntent **intent)
{
    VALIDATE_NOT_NULL(intent);
    if(index < mIntents.GetSize()-1) {
        *intent = mIntents[index];
    }
    else {
        *intent = NULL;
    }
    REFCOUNT_ADD(*intent);
    return NOERROR;
}

ECode CTaskStackBuilder::StartActivities()
{
    return StartActivities(NULL);
}

ECode CTaskStackBuilder::StartActivities(
    /* [in] */ IBundle *options,
    /* [in] */ IUserHandle *userHandle)
{
    if (mIntents.IsEmpty()) {
        return E_ILLEGAL_STATE_EXCEPTION;
        //throw new IllegalStateException(
        //        "No intents added to TaskStackBuilder; cannot startActivities");
    }
    AutoPtr<ArrayOf<IIntent *> > intents;
    GetIntents((ArrayOf<IIntent *>**)&intents);
    return mSourceContext->StartActivitiesAsUser(intents, options, userHandle);
}

ECode CTaskStackBuilder::StartActivities(
    /* [in] */ IBundle *options)
{
    AutoPtr<IUserHandle> userHandle;
    CUserHandle::New(UserHandle::GetMyUserId(), (IUserHandle**)&userHandle);
    return StartActivities(options, userHandle);
}

ECode CTaskStackBuilder::GetPendingIntent(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 flags,
    /* [out] */ IPendingIntent **pendingIntent)
{
    return GetPendingIntent(requestCode, flags, NULL, pendingIntent);
}

ECode CTaskStackBuilder::GetPendingIntent(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 flags,
    /* [in] */ IBundle *options,
    /* [out] */ IPendingIntent **pendingIntent)
{
    VALIDATE_NOT_NULL(pendingIntent);
    *pendingIntent = NULL;

    if (mIntents.IsEmpty()) {
        return E_ILLEGAL_STATE_EXCEPTION;
        //throw new IllegalStateException(
        //        "No intents added to TaskStackBuilder; cannot getPendingIntent");
    }

    AutoPtr<ArrayOf<IIntent *> > intents;
    GetIntents((ArrayOf<IIntent *>**)&intents);
    AutoPtr<IPendingIntentHelper> helper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper);
    return helper->GetActivities(mSourceContext, requestCode, intents,
            flags, options, pendingIntent);
}

ECode CTaskStackBuilder::GetPendingIntent(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 flags,
    /* [in] */ IBundle *options,
    /* [in] */ IUserHandle *user,
    /* [out] */ IPendingIntent **pendingIntent)
{
    VALIDATE_NOT_NULL(pendingIntent);
    *pendingIntent = NULL;

    if (mIntents.IsEmpty()) {
        return E_ILLEGAL_STATE_EXCEPTION;
        //throw new IllegalStateException(
        //        "No intents added to TaskStackBuilder; cannot getPendingIntent");
    }

    AutoPtr<ArrayOf<IIntent *> > intents;
    GetIntents((ArrayOf<IIntent *>**)&intents);
    AutoPtr<IPendingIntentHelper> helper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper);
    return helper->GetActivitiesAsUser(mSourceContext, requestCode, intents, flags,
            options, user, pendingIntent);
}

ECode CTaskStackBuilder::GetIntents(
    /* [out, callee] */ ArrayOf<IIntent *> **intents)
{
    VALIDATE_NOT_NULL(intents);
    *intents = NULL;

    Int32 size = mIntents.GetSize();
    if (size != 0) {
        AutoPtr<ArrayOf<IIntent *> > array = ArrayOf<IIntent *>::Alloc(size);
        AutoPtr<IIntent> intent0;
        CIntent::New(mIntents[0], (IIntent**)&intent0);
        intent0->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK
            | IIntent::FLAG_ACTIVITY_CLEAR_TASK
            | IIntent::FLAG_ACTIVITY_TASK_ON_HOME);
        array->Set(0, intent0);

        for (Int32 i = 1; i < size; ++i) {
            AutoPtr<IIntent> intent;
            CIntent::New(mIntents[i], (IIntent**)&intent);
            array->Set(i, intent);
        }

        *intents = array;
        REFCOUNT_ADD(*intents);
    }

    return NOERROR;
}


}
}
}
