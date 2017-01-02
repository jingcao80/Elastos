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

#include "elastos/droid/launcher2/CInstallShortcutReceiverHelper.h"
#include "elastos/droid/launcher2/InstallShortcutReceiver.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_SINGLETON_IMPL(CInstallShortcutReceiverHelper)

CAR_INTERFACE_IMPL(CInstallShortcutReceiverHelper, Singleton, IInstallShortcutReceiverHelper)

ECode CInstallShortcutReceiverHelper::EnableInstallQueue()
{
    return InstallShortcutReceiver::EnableInstallQueue();
}

ECode CInstallShortcutReceiverHelper::DisableAndFlushInstallQueue(
    /* [in] */ IContext* context)
{
    return InstallShortcutReceiver::DisableAndFlushInstallQueue(context);
}

ECode CInstallShortcutReceiverHelper::FlushInstallQueue(
    /* [in] */ IContext* context)
{
    return InstallShortcutReceiver::FlushInstallQueue(context);
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos