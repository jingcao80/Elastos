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

#ifndef  __ELASTOS_DROID_PHONE_CPHONESEARCHINDEXABLESPROVIDER_H__
#define  __ELASTOS_DROID_PHONE_CPHONESEARCHINDEXABLESPROVIDER_H__

#include "_Elastos_Droid_TeleService_Phone_CPhoneSearchIndexablesProvider.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/provider/SearchIndexablesProvider.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Provider.h"

using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Provider::ISearchIndexableResource;
using Elastos::Droid::Provider::SearchIndexablesProvider;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

CarClass(CPhoneSearchIndexablesProvider)
    , public SearchIndexablesProvider
{
public:
    CAR_OBJECT_DECL();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI QueryXmlResources(
        /* [in] */ ArrayOf<String>* projection,
        /* [out] */ ICursor** cursor);

    //@Override
    CARAPI QueryRawData(
       /* [in] */ ArrayOf<String>* projection,
        /* [out] */ ICursor** cursor);

    //@Override
    CARAPI QueryNonIndexableKeys(
        /* [in] */ ArrayOf<String>* projection,
        /* [out] */ ICursor** cursor);

private:
    static const String TAG;

    static AutoPtr<ArrayOf<ISearchIndexableResource*> > INDEXABLE_RES;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CPHONESEARCHINDEXABLESPROVIDER_H__