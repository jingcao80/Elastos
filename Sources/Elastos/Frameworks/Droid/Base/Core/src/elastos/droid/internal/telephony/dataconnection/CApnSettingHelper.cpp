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

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/internal/telephony/dataconnection/CApnSetting.h"
#include "elastos/droid/internal/telephony/dataconnection/CApnSettingHelper.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

CAR_INTERFACE_IMPL(CApnSettingHelper, Singleton, IApnSettingHelper)

CAR_SINGLETON_IMPL(CApnSettingHelper)

ECode CApnSettingHelper::FromString(
    /* [in] */ const String& data,
    /* [out] */ IApnSetting** result)
{
    return CApnSetting::FromString(data, result);
}

ECode CApnSettingHelper::ArrayFromString(
    /* [in] */ const String& data,
    /* [out] */ IList** result)
{
    return CApnSetting::ArrayFromString(data, result);
}

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
