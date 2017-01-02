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

#ifndef __ELASTOS_DROID_GOOGLE_MMS_UTILITY_SQLITEWRAPPER_H__
#define __ELASTOS_DROID_GOOGLE_MMS_UTILITY_SQLITEWRAPPER_H__

#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

using Elastos::Droid::Content::IContext;
//using Elastos::Droid::Database::Sqlite::ISQLiteException;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Content::IContentValues;

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Utility {

class SqliteWrapper
    : public Object
{
public:
    static CARAPI CheckSQLiteException(
        /* [in] */ IContext* context);
        ///* [in] */ ISQLiteException* e);

    static CARAPI_(AutoPtr<ICursor>) Query(
        /* [in] */ IContext* context,
        /* [in] */ IContentResolver* resolver,
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& sortOrder);

    static CARAPI_(Boolean) Requery(
        /* [in] */ IContext* context,
        /* [in] */ ICursor* cursor);

    static CARAPI_(Int32) Update(
        /* [in] */ IContext* context,
        /* [in] */ IContentResolver* resolver,
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* values,
        /* [in] */ const String& where,
        /* [in] */ ArrayOf<String>* selectionArgs);

    static CARAPI_(Int32) Delete(
        /* [in] */ IContext* context,
        /* [in] */ IContentResolver* resolver,
        /* [in] */ IUri* uri,
        /* [in] */ const String& where,
        /* [in] */ ArrayOf<String>* selectionArgs);

    static CARAPI_(AutoPtr<IUri>) Insert(
        /* [in] */ IContext* context,
        /* [in] */ IContentResolver* resolver,
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* values);

private:
    SqliteWrapper();

    // FIXME: It looks like outInfo.lowMemory does not work well as we expected.
    // after run command: adb shell fillup -p 100, outInfo.lowMemory is still false.
    static CARAPI_(Boolean) IsLowMemory(
        /* [in] */ IContext* context);

    // FIXME: need to optimize this method.
    static CARAPI_(Boolean) IsLowMemory();
        ///* [in] */ ISQLiteException* e);

private:
    static const String TAG;
    static const String SQLITE_EXCEPTION_DETAIL_MESSAGE;
};

} // namespace Utility
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_UTILITY_SQLITEWRAPPER_H__
