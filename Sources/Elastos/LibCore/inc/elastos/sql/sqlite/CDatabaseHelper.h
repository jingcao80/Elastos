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

#ifndef __ELASTOS_SQL_SQLITE_CDATABASEHELPER_H__
#define __ELASTOS_SQL_SQLITE_CDATABASEHELPER_H__

#include "_Elastos_Sql_SQLite_CDatabaseHelper.h"
#include "Singleton.h"

using Elastos::Core::Singleton;

namespace Elastos {
namespace Sql {
namespace SQLite {

CarClass(CDatabaseHelper)
    , public Singleton
    , public IDatabaseHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL();

    CARAPI Complete(
        /* [in] */ const String& sql,
        /* [out] */ Boolean * value);

    CARAPI Version(
        /* [out] */ String * str);

    CARAPI ErrorString(
        /* [in] */ Int32 error_code,
        /* [out] */ String * str);

    CARAPI Status(
        /* [in] */ Int32 op,
        /* [in] */ ArrayOf<Int32>* info,
        /* [in] */ Boolean flag,
        /* [out] */ Int32 * value);

    CARAPI LongFromJulian(
        /* [in] */ Double d,
        /* [out] */ Int64 * value);

    CARAPI LongFromJulian(
        /* [in] */ const String& s ,
        /* [out] */ Int64 * value);

    CARAPI JulianFromLong(
        /* [in] */ Int64 ms,
        /* [out] */ Double * value);
};

} // namespace SQLite
} // namespace Sql
} // namespace Elastos

#endif // __ELASTOS_SQL_SQLITE_CDATABASEHELPER_H__
