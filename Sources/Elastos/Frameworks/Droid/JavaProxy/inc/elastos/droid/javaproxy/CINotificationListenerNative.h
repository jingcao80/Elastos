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

#ifndef __ELASTOS_DROID_JAVAPROXY_CINOTIFICATIONLISTENERNATIVE_H__
#define __ELASTOS_DROID_JAVAPROXY_CINOTIFICATIONLISTENERNATIVE_H__

#include "_Elastos_Droid_JavaProxy_CINotificationListenerNative.h"
#include <elastos/core/Object.h>
#include <jni.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Service::Notification::IINotificationListener;
using Elastos::Droid::Service::Notification::INotificationRankingUpdate;
using Elastos::Droid::Service::Notification::IIStatusBarNotificationHolder;

namespace Elastos {
namespace Droid {
namespace JavaProxy {

CarClass(CINotificationListenerNative)
    , public Object
    , public IINotificationListener
    , public IBinder
{
public:
    ~CINotificationListenerNative();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ HANDLE jVM,
        /* [in] */ HANDLE jInstance);

    CARAPI OnListenerConnected(
        /* [in] */ INotificationRankingUpdate* update);

    CARAPI OnNotificationPosted(
        /* [in] */ IIStatusBarNotificationHolder* notificationHolder,
        /* [in] */ INotificationRankingUpdate* update);

    CARAPI OnNotificationRemoved(
        /* [in] */ IIStatusBarNotificationHolder* notificationHolder,
        /* [in] */ INotificationRankingUpdate* update);

    CARAPI OnNotificationRankingUpdate(
        /* [in] */ INotificationRankingUpdate* update);

    CARAPI OnListenerHintsChanged(
        /* [in] */ Int32 hints);

    CARAPI OnInterruptionFilterChanged(
        /* [in] */ Int32 interruptionFilter);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    static const String TAG;

    JavaVM* mJVM;
    jobject mJInstance;
};

} // namespace JavaProxy
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_JAVAPROXY_CINOTIFICATIONLISTENERNATIVE_H__
