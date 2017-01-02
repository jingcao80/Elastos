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

#ifndef __ELASTOS_DROID_CONTACTS_COMMON_DATABASE_EMPTYCURSOR_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_DATABASE_EMPTYCURSOR_H__

#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/database/AbstractCursor.h"
#include <Elastos.Droid.Database.h>

using Elastos::Droid::Database::AbstractCursor;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Database {

/**
 * A cursor that is empty.
 * <p>
 * If you want an empty cursor, this class is better than a MatrixCursor because it has less
 * overhead.
 */
class EmptyCursor : public AbstractCursor
{
public:
    CARAPI constructor(
        /* [in] */ ArrayOf<String>* columns);

    // @Override
    CARAPI GetCount(
        /* [out] */ Int32* count);

    // @Override
    CARAPI GetColumnNames(
        /* [out, callee] */ ArrayOf<String>** names);

    // @Override
    CARAPI GetString(
        /* [in] */ Int32 column,
        /* [out] */ String* str);

    // @Override
    CARAPI GetInt16(
        /* [in] */ Int32 column,
        /* [out] */ Int16* value);

    // @Override
    CARAPI GetInt32(
        /* [in] */ Int32 column,
        /* [out] */ Int32* value);

    // @Override
    CARAPI GetInt64(
        /* [in] */ Int32 column,
        /* [out] */ Int64* value);

    // @Override
    CARAPI GetFloat(
        /* [in] */ Int32 column,
        /* [out] */ Float* value);

    // @Override
    CARAPI GetDouble(
        /* [in] */ Int32 column,
        /* [out] */ Double* value);

    // @Override
    CARAPI IsNull(
        /* [in] */ Int32 column,
        /* [out] */ Boolean* isNull);

private:
    AutoPtr<ArrayOf<String> > mColumns;
};

} // Database
} // Common
} // Contacts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_DATABASE_EMPTYCURSOR_H__
