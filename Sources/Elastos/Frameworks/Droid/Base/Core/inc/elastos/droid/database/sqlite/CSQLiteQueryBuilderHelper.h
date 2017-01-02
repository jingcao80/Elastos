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

#ifndef __ELASTOS_DROID_DATABASE_SQLITE_CSQLITEQUERYBUILDERHELPER_H__
#define __ELASTOS_DROID_DATABASE_SQLITE_CSQLITEQUERYBUILDERHELPER_H__

#include "_Elastos_Droid_Database_Sqlite_CSQLiteQueryBuilderHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::IStringBuilder;

namespace Elastos {
namespace Droid {
namespace Database {
namespace Sqlite {

CarClass(CSQLiteQueryBuilderHelper)
    , public Singleton
    , public ISQLiteQueryBuilderHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI BuildQueryString(
        /* [in] */ Boolean distinct,
        /* [in] */ const String& tables,
        /* [in] */ ArrayOf<String>* columns,
        /* [in] */ const String& where,
        /* [in] */ const String& groupBy,
        /* [in] */ const String& having,
        /* [in] */ const String& orderBy,
        /* [in] */ const String& limit,
        /* [out] */ String* queryStr);

    CARAPI AppendColumns(
        /* [in] */ IStringBuilder* s,
        /* [in] */ const ArrayOf<String>& columns);
};

} //Sqlite
} //Database
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_DATABASE_SQLITE_CSQLITEQUERYBUILDERHELPER_H__
