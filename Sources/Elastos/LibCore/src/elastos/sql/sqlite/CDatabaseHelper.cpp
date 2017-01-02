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

#include "CDatabaseHelper.h"
#include "Database.h"

namespace Elastos {
namespace Sql {
namespace SQLite {

CAR_INTERFACE_IMPL(CDatabaseHelper, Object, IDatabaseHelper)

CAR_SINGLETON_IMPL(CDatabaseHelper);

ECode CDatabaseHelper::Complete(
    /* [in] */ const String& sql,
    /* [out] */ Boolean * value)
{
    VALIDATE_NOT_NULL(value)
    *value = Database::Complete(sql);
    return NOERROR;
}

ECode CDatabaseHelper::Version(
    /* [out] */ String * str)
{
    VALIDATE_NOT_NULL(str)
    *str = Database::Version();
    return NOERROR;
}

ECode CDatabaseHelper::ErrorString(
    /* [in] */ Int32 error_code,
    /* [out] */ String * str)
{
    VALIDATE_NOT_NULL(str)
    *str = Database::ErrorString(error_code);
    return NOERROR;
}

ECode CDatabaseHelper::Status(
    /* [in] */ Int32 op,
    /* [in] */ ArrayOf<Int32>* info,
    /* [in] */ Boolean flag,
    /* [out] */ Int32 * value)
{
    VALIDATE_NOT_NULL(value)
    *value = Database::Status(op, info,flag);
    return NOERROR;
}

ECode CDatabaseHelper::LongFromJulian(
    /* [in] */ Double d,
    /* [out] */ Int64 * value)
{
    VALIDATE_NOT_NULL(value)
    *value = Database::LongFromJulian(d);
    return NOERROR;
}

ECode CDatabaseHelper::LongFromJulian(
    /* [in] */ const String& s ,
    /* [out] */ Int64 * value)
{
    VALIDATE_NOT_NULL(value)
    *value = Database::LongFromJulian(s);
    return NOERROR;
}

ECode CDatabaseHelper::JulianFromLong(
    /* [in] */ Int64 ms,
    /* [out] */ Double * value)
{
    VALIDATE_NOT_NULL(value)
    *value = Database::JulianFromLong(ms);
    return NOERROR;
}

} // namespace SQLite
} // namespace Sql
} // namespace Elastos
