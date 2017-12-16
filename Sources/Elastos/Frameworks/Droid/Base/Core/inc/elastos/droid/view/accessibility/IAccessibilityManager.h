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

#ifndef __ELASTOS_DROID_VIEW_ACCESSIBILITY_IACCESSIBILITYMANAGER_H__
#define __ELASTOS_DROID_VIEW_ACCESSIBILITY_IACCESSIBILITYMANAGER_H__

#include "Elastos.Droid.AccessibilityService.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>
#include <binder/Binder.h>

using Elastos::Droid::AccessibilityService::IIAccessibilityServiceClient;
using Elastos::Droid::AccessibilityService::IAccessibilityServiceInfo;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Os::IBinder;
using Elastos::Core::Object;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

class IAccessibilityManagerProxy
    : public Object
    , public IIAccessibilityManager
{
public:
    IAccessibilityManagerProxy(
        /* [in] */ android::sp<android::IBinder>& am);

    CAR_INTERFACE_DECL();

    CARAPI AddClient(
        /* [in] */ IIAccessibilityManagerClient* client,
        /* [in] */ Int32 userId,
        /* [out] */ Int32* result);

    CARAPI SendAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* uiEvent,
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* result);

    CARAPI GetInstalledAccessibilityServiceList(
        /* [in] */ Int32 userId,
        /* [out] */ IList** services);

    CARAPI GetEnabledAccessibilityServiceList(
        /* [in] */ Int32 feedbackType,
        /* [in] */ Int32 userId,
        /* [out] */ IList** services);

    CARAPI Interrupt(
        /* [in] */ Int32 userId);

    CARAPI AddAccessibilityInteractionConnection(
        /* [in] */ IIWindow* windowToken,
        /* [in] */ IIAccessibilityInteractionConnection* connection,
        /* [in] */ Int32 userId,
        /* [out] */ Int32* result);

    CARAPI RemoveAccessibilityInteractionConnection(
        /* [in] */ IIWindow* windowToken);

    CARAPI RegisterUiTestAutomationService(
        /* [in] */ IBinder* owner,
        /* [in] */ IIAccessibilityServiceClient* client,
        /* [in] */ IAccessibilityServiceInfo* info);

    CARAPI UnregisterUiTestAutomationService(
        /* [in] */ IIAccessibilityServiceClient* client);

    CARAPI TemporaryEnableAccessibilityStateUntilKeyguardRemoved(
        /* [in] */ IComponentName* service,
        /* [in] */ Boolean touchExplorationEnabled);

    CARAPI GetWindowToken(
        /* [in] */ Int32 windowId,
        /* [out] */ IBinder** binder);

private:
    static const String DESCRIPTOR;
    static const Int32 TRANSACTION_sendAccessibilityEvent;
    static const Int32 TRANSACTION_addClient;
    static const Int32 TRANSACTION_addAccessibilityInteractionConnection;
    static const Int32 TRANSACTION_removeAccessibilityInteractionConnection;

    android::sp<android::IBinder> mAm;
};

} // namespace Accessibility
} // namespace View
} // namespace Droid
} // namespace Accessibility

#endif // __ELASTOS_DROID_VIEW_ACCESSIBILITY_IACCESSIBILITYMANAGER_H__
