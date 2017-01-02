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

#ifndef __ELASTOS_DROID_SETTINGS_SEARCH_CSETTINGSSEARCHINDEXABLESPROVIDER_H__
#define __ELASTOS_DROID_SETTINGS_SEARCH_CSETTINGSSEARCHINDEXABLESPROVIDER_H__

#include "_Elastos_Droid_Settings_Search_CSettingsSearchIndexablesProvider.h"
#include "elastos/droid/provider/SearchIndexablesProvider.h"

using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Provider::SearchIndexablesProvider;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Search {

CarClass(CSettingsSearchIndexablesProvider)
    , public SearchIndexablesProvider
{
public:
    CAR_OBJECT_DECL()

    CSettingsSearchIndexablesProvider();

    ~CSettingsSearchIndexablesProvider();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate(
        /* [out] */ Boolean* result);

    //@Override
    virtual CARAPI QueryXmlResources(
        /* [in] */ ArrayOf<String>* projection,
        /* [out] */ ICursor** result);

    //@Override
    virtual CARAPI QueryRawData(
        /* [in] */ ArrayOf<String>* projection,
        /* [out] */ ICursor** result);

    //@Override
    virtual CARAPI QueryNonIndexableKeys(
        /* [in] */ ArrayOf<String>* projection,
        /* [out] */ ICursor** result);

private:
    static const String TAG;
};

} // namespace Search
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_SEARCH_CSETTINGSSEARCHINDEXABLESPROVIDER_H__
