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

#ifndef __ELASTOS_DROID_DIALER_SETTINGS_DEFAULTRINGTONEPREFERENCE_H__
#define __ELASTOS_DROID_DIALER_SETTINGS_DEFAULTRINGTONEPREFERENCE_H__

#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/preference/RingtonePreference.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Utility.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Preference::RingtonePreference;
using Elastos::Droid::Utility::IAttributeSet;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Settings {

/**
 * RingtonePreference which doesn't show default ringtone setting.
 */
class DefaultRingtonePreference
    : public RingtonePreference
    , public IDefaultRingtonePreference
{
public:
    CAR_INTERFACE_DECL();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

protected:
    // @Override
    CARAPI OnPrepareRingtonePickerIntent(
        /* [in] */ IIntent* ringtonePickerIntent);

    // @Override
    CARAPI OnSaveRingtone(
        /* [in] */ IUri* ringtoneUri);

    // @Override
    CARAPI OnRestoreRingtone(
        /* [out] */ IUri** uri);
};

} // Settings
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_SETTINGS_DEFAULTRINGTONEPREFERENCE_H__
