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

#include "elastos/droid/app/CPendingIntentHelper.h"
#include "elastos/droid/app/CPendingIntent.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(CPendingIntentHelper, Singleton, IPendingIntentHelper)

CAR_SINGLETON_IMPL(CPendingIntentHelper)

ECode CPendingIntentHelper::GetActivity(
    /* [in] */ IContext *context,
    /* [in] */ Int32 requestCode,
    /* [in] */ IIntent *intent,
    /* [in] */ Int32 flags,
    /* [out] */ IPendingIntent **pendingIntent)
{
    return CPendingIntent::GetActivity(context, requestCode, intent, flags, pendingIntent);
}

ECode CPendingIntentHelper::GetActivity(
    /* [in] */ IContext *context,
    /* [in] */ Int32 requestCode,
    /* [in] */ IIntent *intent,
    /* [in] */ Int32 flags,
    /* [in] */ IBundle *options,
    /* [out] */ IPendingIntent **pendingIntent)
{
    return CPendingIntent::GetActivity(context, requestCode, intent, flags, options, pendingIntent);
}

ECode CPendingIntentHelper::GetActivityAsUser(
    /* [in] */ IContext *context,
    /* [in] */ Int32 requestCode,
    /* [in] */ IIntent *intent,
    /* [in] */ Int32 flags,
    /* [in] */ IBundle *options,
    /* [in] */ IUserHandle *user,
    /* [out] */ IPendingIntent **pendingIntent)
{
    return CPendingIntent::GetActivityAsUser(context, requestCode, intent, flags, options, user, pendingIntent);
}

ECode CPendingIntentHelper::GetActivities(
    /* [in] */ IContext *context,
    /* [in] */ Int32 requestCode,
    /* [in] */ ArrayOf<IIntent *> *intents,
    /* [in] */ Int32 flags,
    /* [out] */ IPendingIntent **pendingIntent)
{
    return CPendingIntent::GetActivities(context, requestCode, intents, flags, pendingIntent);
}

ECode CPendingIntentHelper::GetActivities(
    /* [in] */ IContext *context,
    /* [in] */ Int32 requestCode,
    /* [in] */ ArrayOf<IIntent *> *intents,
    /* [in] */ Int32 flags,
    /* [in] */ IBundle *options,
    /* [out] */ IPendingIntent **pendingIntent)
{
    return CPendingIntent::GetActivities(context, requestCode, intents, flags, options, pendingIntent);
}

ECode CPendingIntentHelper::GetActivitiesAsUser(
    /* [in] */ IContext *context,
    /* [in] */ Int32 requestCode,
    /* [in] */ ArrayOf<IIntent *> *intents,
    /* [in] */ Int32 flags,
    /* [in] */ IBundle *options,
    /* [in] */ IUserHandle *user,
    /* [out] */ IPendingIntent **pendingIntent)
{
    return CPendingIntent::GetActivitiesAsUser(context, requestCode, intents, flags, options, user, pendingIntent);
}

ECode CPendingIntentHelper::GetBroadcast(
    /* [in] */ IContext *context,
    /* [in] */ Int32 requestCode,
    /* [in] */ IIntent *intent,
    /* [in] */ Int32 flags,
    /* [out] */ IPendingIntent **pendingIntent)
{
    return CPendingIntent::GetBroadcast(context, requestCode, intent, flags, pendingIntent);
}

ECode CPendingIntentHelper::GetBroadcastAsUser(
    /* [in] */ IContext *context,
    /* [in] */ Int32 requestCode,
    /* [in] */ IIntent *intent,
    /* [in] */ Int32 flags,
    /* [in] */ IUserHandle *userHandle,
    /* [out] */ IPendingIntent **pendingIntent)
{
    return CPendingIntent::GetBroadcastAsUser(context, requestCode, intent, flags, userHandle, pendingIntent);
}

ECode CPendingIntentHelper::GetService(
    /* [in] */ IContext *context,
    /* [in] */ Int32 requestCode,
    /* [in] */ IIntent *intent,
    /* [in] */ Int32 flags,
    /* [out] */ IPendingIntent **pendingIntent)
{
    return CPendingIntent::GetService(context, requestCode, intent, flags, pendingIntent);
}

ECode CPendingIntentHelper::WritePendingIntentOrNullToParcel(
    /* [in] */ IPendingIntent *sender,
    /* [in] */ IParcel *out)
{
    return CPendingIntent::WritePendingIntentOrNullToParcel(sender, out);
}

ECode CPendingIntentHelper::ReadPendingIntentOrNullFromParcel(
    /* [in] */ IParcel *in,
    /* [out] */ IPendingIntent **messenger)
{
    return CPendingIntent::ReadPendingIntentOrNullFromParcel(in, messenger);
}

}
}
}
