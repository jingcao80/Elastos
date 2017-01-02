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

#ifndef __ELASTOS_DROID_PROVIDER_TELEPHONY_CHBPCDLOOKUPPROVIDER_H__
#define __ELASTOS_DROID_PROVIDER_TELEPHONY_CHBPCDLOOKUPPROVIDER_H__

#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Net.h"
#include "_Elastos_Droid_Providers_Telephony_CHbpcdLookupProvider.h"
#include "elastos/droid/content/ContentProvider.h"
#include "CHbpcdLookupDatabaseHelper.h"

using Elastos::Droid::Content::ContentProvider;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IUriMatcher;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Telephony {

CarClass(CHbpcdLookupProvider)
    , public ContentProvider
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor();

    // @Override
    CARAPI OnCreate(
        /* [out] */ Boolean* succeeded);

    // @Override
    CARAPI Query(
        /* [in] */ IUri* url,
        /* [in] */ ArrayOf<String>* projectionIn,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& sort,
        /* [out] */ ICursor** result);

    // @Override
    CARAPI GetType(
        /* [in] */ IUri* url,
        /* [out] */ String* result);

    // @Override
    CARAPI Insert(
        /* [in] */ IUri* url,
        /* [in] */ IContentValues* initialValues,
        /* [out] */ IUri** uri);

    // @Override
    CARAPI Delete(
        /* [in] */ IUri* url,
        /* [in] */ const String& where,
        /* [in] */ ArrayOf<String>* whereArgs,
        /* [out] */ Int32* result);

    // @Override
    CARAPI Update(
        /* [in] */ IUri* url,
        /* [in] */ IContentValues* values,
        /* [in] */ const String& where,
        /* [in] */ ArrayOf<String>* whereArgs,
        /* [out] */ Int32* result);

private:
    static CARAPI_(Boolean) InitMember();

private:
    static const String TAG; // = "HbpcdLookupProvider";
    static const Boolean DBG; // = TRUE;

    const static Int32 MCC_IDD = 1;
    const static Int32 MCC_LOOKUP_TABLE = 2;
    const static Int32 MCC_SID_CONFLICT = 3;
    const static Int32 MCC_SID_RANGE = 4;
    const static Int32 NANP_AREA_CODE = 5;
    const static Int32 ARBITRARY_MCC_SID_MATCH = 6;
    const static Int32 MCC_IDD_ID = 8;
    const static Int32 MCC_LOOKUP_TABLE_ID = 9;
    const static Int32 MCC_SID_CONFLICT_ID = 10;
    const static Int32 MCC_SID_RANGE_ID = 11;
    const static Int32 NANP_AREA_CODE_ID = 12;
    const static Int32 ARBITRARY_MCC_SID_MATCH_ID = 13;

    static AutoPtr<IUriMatcher> sURIMatcher; // = new UriMatcher(UriMatcher.NO_MATCH);

    static AutoPtr<IHashMap> sIddProjectionMap;
    static AutoPtr<IHashMap> sLookupProjectionMap;
    static AutoPtr<IHashMap> sConflictProjectionMap;
    static AutoPtr<IHashMap> sRangeProjectionMap;
    static AutoPtr<IHashMap> sNanpProjectionMap;
    static AutoPtr<IHashMap> sArbitraryProjectionMap;

    AutoPtr<CHbpcdLookupDatabaseHelper> mDbHelper;

    static Boolean s_Init;
};

} // namespace Telephony
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_TELEPHONY_CHBPCDLOOKUPPROVIDER_H__
