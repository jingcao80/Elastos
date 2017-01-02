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

#include "elastos/droid/contacts/common/database/EmptyCursor.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Database {

ECode EmptyCursor::constructor(
    /* [in] */ ArrayOf<String>* columns)
{
    mColumns = columns;
    return NOERROR;
}

ECode EmptyCursor::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    *count = 0;
    return NOERROR;
}

ECode EmptyCursor::GetColumnNames(
    /* [out, callee] */ ArrayOf<String>** names)
{
    VALIDATE_NOT_NULL(names)
    *names = mColumns;
    REFCOUNT_ADD(*names)
    return NOERROR;
}

ECode EmptyCursor::GetString(
    /* [in] */ Int32 column,
    /* [out] */ String* str)
{
    Logger::E("EmptyCursor", "Operation not permitted on an empty cursor.");
    return E_CURSOR_INDEX_OUT_OF_BOUNDS_EXCEPTION;
}

ECode EmptyCursor::GetInt16(
    /* [in] */ Int32 column,
    /* [out] */ Int16* value)
{
    Logger::E("EmptyCursor", "Operation not permitted on an empty cursor.");
    return E_CURSOR_INDEX_OUT_OF_BOUNDS_EXCEPTION;
}

ECode EmptyCursor::GetInt32(
    /* [in] */ Int32 column,
    /* [out] */ Int32* value)
{
    Logger::E("EmptyCursor", "Operation not permitted on an empty cursor.");
    return E_CURSOR_INDEX_OUT_OF_BOUNDS_EXCEPTION;
}

ECode EmptyCursor::GetInt64(
    /* [in] */ Int32 column,
    /* [out] */ Int64* value)
{
    Logger::E("EmptyCursor", "Operation not permitted on an empty cursor.");
    return E_CURSOR_INDEX_OUT_OF_BOUNDS_EXCEPTION;
}

ECode EmptyCursor::GetFloat(
    /* [in] */ Int32 column,
    /* [out] */ Float* value)
{
    Logger::E("EmptyCursor", "Operation not permitted on an empty cursor.");
    return E_CURSOR_INDEX_OUT_OF_BOUNDS_EXCEPTION;
}

ECode EmptyCursor::GetDouble(
    /* [in] */ Int32 column,
    /* [out] */ Double* value)
{
    Logger::E("EmptyCursor", "Operation not permitted on an empty cursor.");
    return E_CURSOR_INDEX_OUT_OF_BOUNDS_EXCEPTION;
}

ECode EmptyCursor::IsNull(
    /* [in] */ Int32 column,
    /* [out] */ Boolean* isNull)
{
    Logger::E("EmptyCursor", "Operation not permitted on an empty cursor.");
    return E_CURSOR_INDEX_OUT_OF_BOUNDS_EXCEPTION;
}

} // namespace Database
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos
