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

#ifndef _ELASTOS_DROID_PROVIDERS_MEDIA_MEDIAUPGRADERECEIVER_H__
#define _ELASTOS_DROID_PROVIDERS_MEDIA_MEDIAUPGRADERECEIVER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Providers.Media.h"
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Media {

class MediaUpgradeReceiver
    : public BroadcastReceiver
    , public IMediaUpgradeReceiver
{
public:
    TO_STRING_IMPL("MediaUpgradeReceiver")

    CAR_INTERFACE_DECL()

    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

public:
    static const String TAG;
    static const String PREF_DB_VERSION;
};

} // namespace Media
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_PROVIDERS_MEDIA_MEDIAUPGRADERECEIVER_H__
