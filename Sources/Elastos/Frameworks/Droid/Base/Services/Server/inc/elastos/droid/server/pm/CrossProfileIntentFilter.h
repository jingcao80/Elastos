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

#ifndef __ELASTOS_DROID_SERVER_PM_CROSSPROFILEINTENTFILTER_H__
#define __ELASTOS_DROID_SERVER_PM_CROSSPROFILEINTENTFILTER_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/IntentFilter.h"

using Elastos::Droid::Content::IntentFilter;
using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlSerializer;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

class CrossProfileIntentFilter : public IntentFilter
{
public:
    CARAPI constructor(
        /* [in] */ IIntentFilter* filter);

    CrossProfileIntentFilter(
        /* [in] */ const String& ownerPackage,
        /* [in] */ Int32 ownerUserId,
        /* [in] */ Int32 targetUserId,
        /* [in] */ Int32 flags);

    CrossProfileIntentFilter(
        /* [in] */ IXmlPullParser* parser);

    CARAPI_(Int32) GetTargetUserId();

    CARAPI_(Int32) GetFlags();

    CARAPI_(Int32) GetOwnerUserId();

    CARAPI_(String) GetOwnerPackage();

    CARAPI_(String) GetStringFromXml(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ const String& attribute,
        /* [in] */ const String& defaultValue);

    CARAPI_(Int32) GetInt32FromXml(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ const String& attribute,
        /* [in] */ Int32 defaultValue);

    CARAPI WriteToXml(
        /* [in] */ IXmlSerializer* serializer);

    CARAPI ToString(
        /* [out] */ String* str);

public:
    // If the intent matches the IntentFilter, then it can be forwarded to this userId.
    Int32 mTargetUserId;
    Int32 mOwnerUserId; // userId of the app which has set this CrossProfileIntentFilter.
    String mOwnerPackage; // packageName of the app.
    Int32 mFlags;

private:
    static const String ATTR_TARGET_USER_ID;
    static const String ATTR_FLAGS;
    static const String ATTR_OWNER_USER_ID;
    static const String ATTR_OWNER_PACKAGE;
    static const String ATTR_FILTER;

    static const String TAG;
};

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Server::Pm::CrossProfileIntentFilter, IInterface)
DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Droid::Server::Pm::CrossProfileIntentFilter)

#endif //__ELASTOS_DROID_SERVER_PM_CROSSPROFILEINTENTFILTER_H__

