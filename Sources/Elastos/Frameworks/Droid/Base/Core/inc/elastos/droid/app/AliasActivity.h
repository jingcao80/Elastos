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

#ifndef __ELASTOS_DROID_APP_ALIAS_ACTIVITY_H__
#define __ELASTOS_DROID_APP_ALIAS_ACTIVITY_H__

#include "elastos/droid/app/Activity.h"

using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Utility::IAttributeSet;

using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace App {

/**
 * Stub activity that launches another activity (and then finishes itself)
 * based on information in its component's manifest meta-data.  This is a
 * simple way to implement an alias-like mechanism.
 *
 * To use this activity, you should include in the manifest for the associated
 * component an entry named "android.app.alias".  It is a reference to an XML
 * resource describing an intent that launches the real application.
 */
class AliasActivity
    : public Activity
    , public IAliasActivity
{
public:
    CAR_INTERFACE_DECL()

    AliasActivity();

    virtual ~AliasActivity();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

private:
    CARAPI ParseAlias(
        /* [in] */ IXmlPullParser* parser,
        /* [out] */ IIntent** result);
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_APP_ALIAS_ACTIVITY_H__
