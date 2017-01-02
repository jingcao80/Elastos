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

#include "elastos/droid/media/tv/CTvInputInfoHelper.h"
#include "elastos/droid/media/tv/CTvInputInfo.h"

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

CAR_INTERFACE_IMPL(CTvInputInfoHelper, Singleton, ITvInputInfoHelper)

CAR_SINGLETON_IMPL(CTvInputInfoHelper)

ECode CTvInputInfoHelper::CreateTvInputInfo(
    /* [in] */ IContext* context,
    /* [in] */ IResolveInfo* service,
    /* [out] */ ITvInputInfo** result)
{
    return CTvInputInfo::CreateTvInputInfo(context, service, result);
}

ECode CTvInputInfoHelper::CreateTvInputInfo(
    /* [in] */ IContext* context,
    /* [in] */ IResolveInfo* service,
    /* [in] */ IHdmiDeviceInfo* hdmiDeviceInfo,
    /* [in] */ const String& parentId,
    /* [in] */ const String& label,
    /* [in] */ IUri* iconUri,
    /* [out] */ ITvInputInfo** result)
{
    return CTvInputInfo::CreateTvInputInfo(context, service,
            hdmiDeviceInfo, parentId, label, iconUri,result);
}

ECode CTvInputInfoHelper::CreateTvInputInfo(
    /* [in] */ IContext* context,
    /* [in] */ IResolveInfo* service,
    /* [in] */ ITvInputHardwareInfo* hardwareInfo,
    /* [in] */ const String& label,
    /* [in] */ IUri* iconUri,
    /* [out] */ ITvInputInfo** result)
{
    return CTvInputInfo::CreateTvInputInfo(context, service,
            hardwareInfo, label, iconUri,result);
}

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos
