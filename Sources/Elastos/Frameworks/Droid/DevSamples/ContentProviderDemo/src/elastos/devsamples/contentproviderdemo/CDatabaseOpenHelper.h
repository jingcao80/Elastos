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

#ifndef __Elastos_DevSamples_ContentProviderDemo_CDatabaseOpenHelper_H__
#define __Elastos_DevSamples_ContentProviderDemo_CDatabaseOpenHelper_H__

#include "_Elastos_DevSamples_ContentProviderDemo_CDatabaseOpenHelper.h"
#include <Elastos.Droid.Content.h>
#include <elastos/droid/database/sqlite/SQLiteOpenHelper.h>

using Elastos::Droid::Database::Sqlite::SQLiteOpenHelper;
using Elastos::Droid::Database::Sqlite::ISQLiteDatabase;

namespace Elastos {
namespace DevSamples {
namespace ContentProviderDemo {

CarClass(CDatabaseOpenHelper)
    , public SQLiteOpenHelper
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI OnCreate(
        /* [in] */ ISQLiteDatabase* db);

    CARAPI OnUpgrade(
            /* [in] */ ISQLiteDatabase* db,
            /* [in] */ Int32 oldVersion,
            /* [in] */ Int32 newVersion);

    CARAPI Add(
        /* [in] */ const String& username,
        /* [in] */ const String& sex,
        /* [in] */ const String& email);
};

} // namespace ContentProviderDemo
} // namespace DevSamples
} // namespace Elastos

#endif //__Elastos_DevSamples_ContentProviderDemo_CDatabaseOpenHelper_H__
