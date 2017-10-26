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

#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_MODEL_RECENTSPACKAGEMONITOR_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_MODEL_RECENTSPACKAGEMONITOR_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/internal/content/PackageMonitor.h"
#include "elastos/droid/systemui/recents/misc/SystemServicesProxy.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Content::PackageMonitor;
using Elastos::Droid::SystemUI::Recents::Misc::SystemServicesProxy;
using Elastos::Droid::SystemUI::Recents::Model::IPackageCallbacks;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Model {

/**
 * The package monitor listens for changes from PackageManager to update the contents of the Recents
 * list.
 */
class RecentsPackageMonitor
    : public PackageMonitor
    , public IRecentsPackageMonitor
{
public:
    CAR_INTERFACE_DECL()

    RecentsPackageMonitor();

    using PackageMonitor::Register;

    /** Registers the broadcast receivers with the specified callbacks. */
    CARAPI Register(
        /* [in] */ IContext* context,
        /* [in] */ IPackageCallbacks* cb);

    /** Unregisters the broadcast receivers. */
    // @Override
    CARAPI Unregister();

    /** Sets the list of tasks to match against package broadcast changes. */
    CARAPI SetTasks(
        /* [in] */ IList* tasks);

    // @Override
    CARAPI OnPackageRemoved(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 uid);

    // @Override
    CARAPI OnPackageChanged(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 uid,
        /* [in] */ ArrayOf<String>* components,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnPackageModified(
        /* [in] */ const String& packageName);

public:
    AutoPtr<IPackageCallbacks> mCb;
    AutoPtr<IList> mTasks;
    AutoPtr<SystemServicesProxy> mSystemServicesProxy;
};

} // namespace Model
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_MODEL_RECENTSPACKAGEMONITOR_H__
