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

#ifndef __ELASTOS_DROID_SETTINGS_TRUSTAGENTUTILS_H__
#define __ELASTOS_DROID_SETTINGS_TRUSTAGENTUTILS_H__

#include "Elastos.Droid.Content.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Settings {

class TrustAgentUtils
{
public:
    class TrustAgentComponentInfo
        : public Object
    {
    public:
        AutoPtr<IComponentName> mComponentName;
        String mTitle;
        String mSummary;
    };

public:
    /**
     * @return TRUE, if the service in resolveInfo has the permission to provide a trust agent.
     */
    static CARAPI_(Boolean) CheckProvidePermission(
        /* [in] */ IResolveInfo* resolveInfo,
        /* [in] */ IPackageManager* pm);

    static CARAPI_(AutoPtr<IComponentName>) GetComponentName(
        /* [in] */ IResolveInfo* resolveInfo);

    static CARAPI_(AutoPtr<TrustAgentComponentInfo>) GetSettingsComponent(
        /* [in] */ IPackageManager* pm,
        /* [in] */ IResolveInfo* resolveInfo);

protected:
    static const String TAG;

private:
    static const String TRUST_AGENT_META_DATA;
    static const String PERMISSION_PROVIDE_AGENT;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_TRUSTAGENTUTILS_H__