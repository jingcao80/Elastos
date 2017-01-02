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

#ifndef __ELASTOS_DROID_CONTENT_RES_CCONFIGURATIONHELPER_H__
#define __ELASTOS_DROID_CONTENT_RES_CCONFIGURATIONHELPER_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "_Elastos_Droid_Content_Res_CConfigurationHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;
using Elastos::Utility::ILocale;
using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlSerializer;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

CarClass(CConfigurationHelper)
    , public Singleton
    , public IConfigurationHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI ResetScreenLayout(
        /* [in] */ Int32 curLayout,
        /* [out] */ Int32* layout);

    CARAPI ReduceScreenLayout(
        /* [in] */ Int32 curLayout,
        /* [in] */ Int32 longSizeDp,
        /* [in] */ Int32 shortSizeDp,
        /* [out] */ Int32* layout);

    /**
     * Determine if a new resource needs to be loaded from the bit set of
     * configuration changes returned by {@link #updateFrom(Configuration)}.
     *
     * @param configChanges The mask of changes configurations as returned by
     * {@link #updateFrom(Configuration)}.
     * @param interestingChanges The configuration changes that the resource
     * can handled, as given in {@link android.util.TypedValue#changingConfigurations}.
     *
     * @return Return true if the resource needs to be loaded, else false.
     */
    CARAPI NeedNewResources(
        /* [in] */ Int32 configChanges,
        /* [in] */ Int32 interestingChanges,
        /* [out] */ Boolean * value);

    CARAPI GetEmpty(
        /* [out] */ IConfiguration** empty);

    CARAPI LocaleToResourceQualifier(
        /* [in] */ ILocale* locale,
        /* [out] */ String* resourceQualifier);

    CARAPI ResourceQualifierString(
        /* [in] */ IConfiguration* config,
        /* [out] */ String* resourceQualifier);

    CARAPI GenerateDelta(
        /* [in] */ IConfiguration* base,
        /* [in] */ IConfiguration* change,
        /* [out] */ IConfiguration** configuration);

    CARAPI ReadXmlAttrs(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IConfiguration* configOut);

    CARAPI WriteXmlAttrs(
        /* [in] */ IXmlSerializer* xml,
        /* [in] */ IConfiguration* config);
};

}
}
}
}

#endif // __ELASTOS_DROID_CONTENT_RES_CCONFIGURATIONHELPER_H__
