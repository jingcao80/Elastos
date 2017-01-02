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

#ifndef __ELASTOS_DROID_INTERNAL_CONTENT_CSELECTIONBUILDER_H__
#define __ELASTOS_DROID_INTERNAL_CONTENT_CSELECTIONBUILDER_H__

#include "_Elastos_Droid_Internal_Content_CSelectionBuilder.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/List.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/Object.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::Etl::List;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::Sqlite::ISQLiteDatabase;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Content {

CarClass(CSelectionBuilder)
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CSelectionBuilder();

    ~CSelectionBuilder();

    /**
     * Reset any internal state, allowing this builder to be recycled.
     */
    CARAPI Reset();

    /**
     * Append the given selection clause to the internal state. Each clause is
     * surrounded with parenthesis and combined using {@code AND}.
     */
    CARAPI Append(
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<IInterface*>* selectionArgs);

    /**
     * Return selection string for current internal state.
     *
     * @see #getSelectionArgs()
     */
    CARAPI GetSelection(
        /* [out] */ String* str);

    /**
     * Return selection arguments for current internal state.
     *
     * @see #getSelection()
     */
    CARAPI GetSelectionArgs(
        /* [out, callee] */ ArrayOf<String>** selectionArgs);

    /**
     * Execute query using the current internal state as {@code WHERE} clause.
     * Missing arguments as treated as {@code null}.
     */
    CARAPI Query(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& table,
        /* [in] */ ArrayOf<String>* columns,
        /* [in] */ const String& orderBy,
        /* [out] */ ICursor** cursor);

    /**
     * Execute query using the current internal state as {@code WHERE} clause.
     */
    CARAPI Query(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& table,
        /* [in] */ ArrayOf<String>* columns,
        /* [in] */ const String& groupBy,
        /* [in] */ const String& having,
        /* [in] */ const String& orderBy,
        /* [in] */ const String& limit,
        /* [out] */ ICursor** cursor);

    /**
     * Execute update using the current internal state as {@code WHERE} clause.
     */
    CARAPI Update(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& table,
        /* [in] */ IContentValues* values,
        /* [out] */ Int32* affectedRow);

    /**
     * Execute delete using the current internal state as {@code WHERE} clause.
     */
    CARAPI Delete(
        /* [in] */ ISQLiteDatabase* db,
        /* [in] */ const String& table,
        /* [out] */ Int32* affectedRow);

private:
    AutoPtr<StringBuilder> mSelection;
    List<String> mSelectionArgs;

};

}
}
}
}

#endif // __ELASTOS_DROID_INTERNAL_CONTENT_CSELECTIONBUILDER_H__
