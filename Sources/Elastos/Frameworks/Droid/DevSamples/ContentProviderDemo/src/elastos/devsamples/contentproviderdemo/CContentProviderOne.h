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

#ifndef __Elastos_DevSamples_ContentProviderDemo_CContentProviderOne_H__
#define __Elastos_DevSamples_ContentProviderDemo_CContentProviderOne_H__

#include "_Elastos_DevSamples_ContentProviderDemo_CContentProviderOne.h"
#include <Elastos.Droid.Database.h>
#include <elastos/droid/content/ContentProvider.h>

using Elastos::Droid::Content::IUriMatcher;
using Elastos::Droid::Content::ContentProvider;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Database::Sqlite::ISQLiteOpenHelper;

namespace Elastos {
namespace DevSamples {
namespace ContentProviderDemo {

CarClass(CContentProviderOne)
    , public ContentProvider
{
public:
    CAR_OBJECT_DECL()

    CContentProviderOne();

    ~CContentProviderOne();

    TO_STRING_IMPL("CContentProviderOne")

    CARAPI constructor();

    CARAPI OnCreate(
        /* [out] */ Boolean* succeeded);

    CARAPI Delete(
        /* [in] */ IUri* uri,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [out] */ Int32* rowsAffected);

    CARAPI GetType(
        /* [in] */ IUri* uri,
        /* [out] */ String* type);

    CARAPI Insert(
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* values,
        /* [out] */ IUri** insertedItemUri);

    CARAPI Query(
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& sortOrder,
        /* [out] */ ICursor** cursor);

    CARAPI Update(
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* values,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [out] */ Int32* rowsAffected);

private:
    static AutoPtr<IUriMatcher> GetUriMatcher();

private:
    static AutoPtr<IUriMatcher> sUriMatcher;

    AutoPtr<ISQLiteOpenHelper> mDBOpenHelper;
};

} // namespace ContentProviderDemo
} // namespace DevSamples
} // namespace Elastos

#endif //__Elastos_DevSamples_ContentProviderDemo_CContentProviderOne_H__
