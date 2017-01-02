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

#ifndef __ORG_XMLPULL_V1_CXMLPULLPARSERFACTORYHELPER_H__
#define __ORG_XMLPULL_V1_CXMLPULLPARSERFACTORYHELPER_H__

#include "_Org_Xmlpull_V1_CXmlPullParserFactoryHelper.h"
#include <elastos/core/Singleton.h>

namespace Org {
namespace Xmlpull {
namespace V1 {

CarClass(CXmlPullParserFactoryHelper)
    , public Singleton
    , public IXmlPullParserFactoryHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI NewInstance(
        /* [out] */ IXmlPullParserFactory** xmlPullParserFac);

    CARAPI NewInstance(
        /* [in] */ const String& classNames,
     // /* [in] */  Class context,
        /* [out] */ IXmlPullParserFactory** xmlPullParserFac);
};

} // namespace V1
} // namespace Xmlpull
} // namespace Org

#endif // __ORG_XMLPULL_V1_CXMLPULLPARSERFACTORYHELPER_H__
