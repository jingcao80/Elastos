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

#include "elastos/droid/internal/net/CVpnConfigHelper.h"
#include "elastos/droid/internal/net/CVpnConfig.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Net {

CAR_INTERFACE_IMPL(CVpnConfigHelper, Singleton, IVpnConfigHelper)

CAR_SINGLETON_IMPL(CVpnConfigHelper)

ECode CVpnConfigHelper::GetIntentForConfirmation(
    /* [out] */ IIntent** intent)
{
    return CVpnConfig::GetIntentForConfirmation(intent);
}

ECode CVpnConfigHelper::GetIntentForStatusPanel(
    /* [in] */ IContext* context,
    /* [out] */ IPendingIntent** intent)
{
    return CVpnConfig::GetIntentForStatusPanel(context, intent);
}

ECode CVpnConfigHelper::GetVpnLabel(
    /* [in] */ IContext* context,
    /* [in] */ const String& packageName,
    /* [out] */ ICharSequence** label)
{
    return CVpnConfig::GetVpnLabel(context, packageName, label);
}

} // namespace Net
} // namespace Internal
} // namespace Droid
} // namespace Elastos
