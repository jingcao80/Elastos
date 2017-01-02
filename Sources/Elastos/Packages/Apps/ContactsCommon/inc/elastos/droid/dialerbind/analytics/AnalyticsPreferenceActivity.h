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

#ifndef __ELASTOS_APPS_DIALERBIND_ANALYTICS_ANALYTICSPREFERENCEACTIVITY_H__
#define __ELASTOS_APPS_DIALERBIND_ANALYTICS_ANALYTICSPREFERENCEACTIVITY_H__

namespace Elastos{
namespace Apps{
namespace DialerBind {
namespace Analytics {

class AnalyticsPreferenceActivity
    : public PreferenceActivity
    , public IAnalyticsPreferenceActivity
    , public IAnalyticsInterface
{
public:
    CAR_INTERFACE_DECL()

    // @Override
    CARAPI SendHitEvent(
        /* [in] */ const String& categoryId,
        /* [in] */ const String& actionId,
        /* [in] */ const String& labelId);

    // @Override
    CARAPI SendScreenView();
};

} // Analytics
} // DialerBind
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALERBIND_ANALYTICS_ANALYTICSPREFERENCEACTIVITY_H__
