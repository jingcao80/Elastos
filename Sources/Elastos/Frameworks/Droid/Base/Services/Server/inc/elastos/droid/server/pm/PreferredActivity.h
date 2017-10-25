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

#ifndef __ELASTOS_DROID_SERVER_PM_PREFERREDACTIVITY_H__
#define __ELASTOS_DROID_SERVER_PM_PREFERREDACTIVITY_H__

#include "elastos/droid/content/IntentFilter.h"
#include "elastos/droid/server/pm/PreferredComponent.h"

using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlSerializer;
using Elastos::Droid::Content::IntentFilter;
using Elastos::Droid::Content::IComponentName;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

class PreferredActivity
    : public IntentFilter
    , public PreferredComponent::ICallbacks
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IIntentFilter* filter);

    PreferredActivity(
        /* [in] */ Int32 match,
        /* [in] */ ArrayOf<IComponentName*>* set,
        /* [in] */ IComponentName* activity,
        /* [in] */ Boolean always);

    PreferredActivity(
        /* [in] */ IXmlPullParser* parser);

    using IntentFilter::WriteToXml;

    CARAPI WriteToXml(
        /* [in] */ IXmlSerializer* serializer,
        /* [in] */ Boolean full);

    CARAPI OnReadTag(
        /* [in] */ const String& tagName,
        /* [in] */ IXmlPullParser* parser,
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* str);

public:
    AutoPtr<PreferredComponent> mPref;

private:
    static const String TAG;

    static const Boolean DEBUG_FILTERS = FALSE;
};

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Server::Pm::PreferredActivity, IInterface)
DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Droid::Server::Pm::PreferredActivity)

#endif //__ELASTOS_DROID_SERVER_PM_PREFERREDACTIVITY_H__
