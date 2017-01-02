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

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/database/sqlite/SQLiteQueryBuilder.h"
#include "elastos/droid/database/sqlite/SQLiteSession.h"
#include "elastos/droid/database/DatabaseUtils.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Database::DatabaseUtils;
using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Core::CString;
using Elastos::Utility::ISet;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::Regex::IPatternHelper;
using Elastos::Utility::Regex::CPatternHelper;
using Elastos::Utility::Regex::IMatcher;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Database {
namespace Sqlite {

const String SQLiteQueryBuilder::TAG("SQLiteQueryBuilder");

static AutoPtr<IPattern> InitLimitPattern()
{
    AutoPtr<IPatternHelper> helper;
    CPatternHelper::AcquireSingleton((IPatternHelper**)&helper);
    AutoPtr<IPattern> pattern;
    helper->Compile(String("\\s*\\d+\\s*(,\\s*\\d+\\s*)?"), (IPattern**)&pattern);
    return pattern;
}
const AutoPtr<IPattern> SQLiteQueryBuilder::sLimitPattern = InitLimitPattern();

CAR_INTERFACE_IMPL(SQLiteQueryBuilder, Object, ISQLiteQueryBuilder);

SQLiteQueryBuilder::SQLiteQueryBuilder()
    : mDistinct(FALSE)
    , mStrict(FALSE)
{}

ECode SQLiteQueryBuilder::constructor()
{
    return NOERROR;
}

SQLiteQueryBuilder::~SQLiteQueryBuilder()
{
    if (mWhereClause) {
        delete mWhereClause;
    }
}

ECode SQLiteQueryBuilder::SetDistinct(
    /* [in] */ Boolean distinct)
{
    mDistinct = distinct;
    return NOERROR;
}

ECode SQLiteQueryBuilder::GetTables(
    /* [out] */ String* tables)
{
    VALIDATE_NOT_NULL(tables);

    *tables = mTables;
    return NOERROR;
}

ECode SQLiteQueryBuilder::SetTables(
    /* [in]*/ const String& inTables)
{
    mTables = inTables;
    return NOERROR;
}

ECode SQLiteQueryBuilder::AppendWhere(
    /* [in] */ ICharSequence* inWhere)
{
    if (mWhereClause == NULL) {
        Int32 length;
        inWhere->GetLength(&length);
        mWhereClause = new StringBuilder(length + 16);
    }
    if (mWhereClause->GetLength() == 0) {
        *mWhereClause += '(';
    }
    *mWhereClause += inWhere;
    return NOERROR;
}

ECode SQLiteQueryBuilder::AppendWhereEscapeString(
    /* [in] */ const String& inWhere)
{
    if (mWhereClause == NULL) {
        mWhereClause = new StringBuilder(inWhere.GetByteLength() + 16);
    }
    if (mWhereClause->GetLength() == 0) {
        *mWhereClause += '(';
    }
    DatabaseUtils::AppendEscapedSQLString(mWhereClause, inWhere);
    return NOERROR;
}

ECode SQLiteQueryBuilder::SetProjectionMap(
    /* [in] */ IMap* columnMap)
{
    mProjectionMap = columnMap;
    return NOERROR;
}

ECode SQLiteQueryBuilder::SetCursorFactory(
    /* [in] */ ISQLiteDatabaseCursorFactory* factory)
{
    mFactory = factory;
    return NOERROR;
}

ECode SQLiteQueryBuilder::SetStrict(
    /* [in] */ Boolean flag)
{
    mStrict = flag;
    return NOERROR;
}

ECode SQLiteQueryBuilder::BuildQueryString(
    /* [in] */ Boolean distinct,
    /* [in] */ const String& tables,
    /* [in] */ ArrayOf<String>* columns,
    /* [in] */ const String& where,
    /* [in] */ const String& groupBy,
    /* [in] */ const String& having,
    /* [in] */ const String& orderBy,
    /* [in] */ const String& limit,
    /* [out] */ String* queryStr)
{
    VALIDATE_NOT_NULL(queryStr)

    if (groupBy.IsNullOrEmpty() && !having.IsNullOrEmpty()) {
        // throw new IllegalArgumentException(
        //         "HAVING clauses are only permitted when using a groupBy clause");
        Slogger::E(TAG, "HAVING clauses are only permitted when using a groupBy clause");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (!limit.IsNullOrEmpty()) {
        AutoPtr<IMatcher> matcher;
        sLimitPattern->Matcher(limit, (IMatcher**)&matcher);
        Boolean result;
        if (matcher->Matches(&result), !result) {
            // throw new IllegalArgumentException("invalid LIMIT clauses:" + limit);
            Slogger::E(TAG, "invalid LIMIT clauses:%s", limit.string());
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    StringBuilder query("SELECT ");
    if (distinct) {
        query += "DISTINCT ";
    }
    if (columns != NULL && columns->GetLength() != 0) {
        AppendColumns((IStringBuilder*)&query, *columns);
    }
    else {
        query += "* ";
    }
    query += "FROM ";
    query += tables;
    AppendClause(query, String(" WHERE "), where);
    AppendClause(query, String(" GROUP BY "), groupBy);
    AppendClause(query, String(" HAVING "), having);
    AppendClause(query, String(" ORDER BY "), orderBy);
    AppendClause(query, String(" LIMIT "), limit);

    *queryStr = query.ToString();
    return NOERROR;
}

void SQLiteQueryBuilder::AppendClause(
    /* [in] */ StringBuilder& s,
    /* [in] */ const String& name,
    /* [in] */ const String& clause)
{
    if (!TextUtils::IsEmpty(clause)) {
        s += name;
        s += clause;
    }
}

ECode SQLiteQueryBuilder::AppendColumns(
    /* [in] */ IStringBuilder* s,
    /* [in] */ const ArrayOf<String>& columns)
{
    Int32 n = columns.GetLength();

    for (Int32 i = 0; i < n; i++) {
        String column = columns[i];

        if (!column.IsNull()) {
            if (i > 0) {
                s->Append(String(", "));
            }
            s->Append(column);
        }
    }
    s->AppendChar(' ');
    return NOERROR;
}

ECode SQLiteQueryBuilder::Query(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ ArrayOf<String>* projectionIn,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& groupBy,
    /* [in] */ const String& having,
    /* [in] */ const String& sortOrder,
    /* [out] */ ICursor** cursor)
{
    return Query(db, projectionIn, selection, selectionArgs, groupBy, having, sortOrder,
                String(NULL) /* limit */, NULL, cursor);
}

ECode SQLiteQueryBuilder::Query(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ ArrayOf<String>* projectionIn,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& groupBy,
    /* [in] */ const String& having,
    /* [in] */ const String& sortOrder,
    /* [in] */ const String& limit,
    /* [out] */ ICursor** cursor)
{
    return Query(db, projectionIn, selection, selectionArgs,
            groupBy, having, sortOrder, limit, NULL, cursor);
}

ECode SQLiteQueryBuilder::Query(
    /* [in] */ ISQLiteDatabase* db,
    /* [in] */ ArrayOf<String>* projectionIn,
    /* [in] */ const String& selection,
    /* [in] */ ArrayOf<String>* selectionArgs,
    /* [in] */ const String& groupBy,
    /* [in] */ const String& having,
    /* [in] */ const String& sortOrder,
    /* [in] */ const String& limit,
    /* [in] */ ICancellationSignal* cancellationSignal,
    /* [out] */ ICursor** cursor)
{
    VALIDATE_NOT_NULL(cursor)
    *cursor = NULL;

    if (mTables.IsNull()) {
        return NOERROR;
    }

    if (mStrict && !selection.IsNullOrEmpty()) {
        // Validate the user-supplied selection to detect syntactic anomalies
        // in the selection string that could indicate a SQL injection attempt.
        // The idea is to ensure that the selection clause is a valid SQL expression
        // by compiling it twice: once wrapped in parentheses and once as
        // originally specified. An attacker cannot create an expression that
        // would escape the SQL expression while maintaining balanced parentheses
        // in both the wrapped and original forms.
        String sqlForValidation;
        FAIL_RETURN(BuildQuery(projectionIn, String("(") + selection + String(")"), groupBy,
                having, sortOrder, limit, &sqlForValidation))
        FAIL_RETURN(ValidateQuerySql((SQLiteDatabase*)db, sqlForValidation, cancellationSignal)) // will throw if query is invalid
    }

    String sql;
    FAIL_RETURN(BuildQuery(projectionIn, selection, groupBy, having, sortOrder, limit, &sql))

    // if (Log.isLoggable(TAG, Log.DEBUG)) {
    //     Log.d(TAG, "Performing query: " + sql);
    // }
    String str;
    FAIL_RETURN(SQLiteDatabase::FindEditTable(mTables, &str))
    return db->RawQueryWithFactory(mFactory, sql, selectionArgs, str, cancellationSignal, cursor); // will throw if query is invalid
}

ECode SQLiteQueryBuilder::ValidateQuerySql(
    /* [in] */ SQLiteDatabase* db,
    /* [in] */ const String& sql,
    /* [in] */ ICancellationSignal* cancellationSignal)
{
    AutoPtr<SQLiteSession> session;
    FAIL_RETURN(db->GetThreadSession((SQLiteSession**)&session))
    return session->Prepare(sql, db->GetThreadDefaultConnectionFlags(TRUE /*readOnly*/), cancellationSignal, NULL);
}

ECode SQLiteQueryBuilder::BuildQuery(
    /* [in] */ ArrayOf<String>* projectionIn,
    /* [in] */ const String& selection,
    /* [in] */ const String& groupBy,
    /* [in] */ const String& having,
    /* [in] */ const String& sortOrder,
    /* [in] */ const String& limit,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    AutoPtr< ArrayOf<String> > projection;
    FAIL_RETURN(ComputeProjection(projectionIn, (ArrayOf<String>**)&projection));

    StringBuilder where;
    Boolean hasBaseWhereClause = mWhereClause != NULL && mWhereClause->GetLength() > 0;

    if (hasBaseWhereClause) {
        where += *mWhereClause;
        where += ')';
    }

    // Tack on the user's selection, if present.
    if (!selection.IsNullOrEmpty()) {
        if (hasBaseWhereClause) {
            where += " AND ";
        }

        where += '(';
        where += selection;
        where += ')';
    }

    return BuildQueryString(
            mDistinct, mTables, projection, where.ToString(),
            groupBy, having, sortOrder, limit, str);
}

ECode SQLiteQueryBuilder::BuildQuery(
        /* [in] */ ArrayOf<String>* projectionIn,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& groupBy,
        /* [in] */ const String& having,
        /* [in] */ const String& sortOrder,
        /* [in] */ const String& limit,
        /* [out] */ String* str)
{
    return BuildQuery(projectionIn, selection, groupBy, having, sortOrder, limit, str);
}

ECode SQLiteQueryBuilder::BuildUnionSubQuery(
    /* [in] */ const String& typeDiscriminatorColumn,
    /* [in] */ const ArrayOf<String>& unionColumns,
    /* [in] */ ISet* columnsPresentInTable,
    /* [in] */ Int32 computedColumnsOffset,
    /* [in] */ const String& typeDiscriminatorValue,
    /* [in] */ const String& selection,
    /* [in] */ const String& groupBy,
    /* [in] */ const String& having,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    Int32 unionColumnsCount = unionColumns.GetLength();
    AutoPtr<ArrayOf<String> > projectionIn = ArrayOf<String>::Alloc(unionColumnsCount);

    for (Int32 i = 0; i < unionColumnsCount; i++) {
        String unionColumn = unionColumns[i];
        AutoPtr<ICharSequence> charObj;
        CString::New(unionColumn, (ICharSequence**)&charObj);

        Boolean isContain;
        if (unionColumn.Equals(typeDiscriminatorColumn)) {
            (*projectionIn)[i] = String("'") + typeDiscriminatorValue + String("' AS ")
                    + typeDiscriminatorColumn;
        }
        else if (i <= computedColumnsOffset
                || (columnsPresentInTable->Contains(charObj, &isContain), isContain)) {
            (*projectionIn)[i] = unionColumn;
        }
        else {
            (*projectionIn)[i] = String("NULL AS ") + unionColumn;
        }
    }
    return BuildQuery(projectionIn, selection, groupBy, having, String(NULL) /* sortOrder */,
            String(NULL) /* limit */, str);
}

ECode SQLiteQueryBuilder::BuildUnionSubQuery(
    /* [in] */ const String& typeDiscriminatorColumn,
    /* [in] */ const ArrayOf<String>& unionColumns,
    /* [in] */ ISet* columnsPresentInTable,
    /* [in] */ Int32 computedColumnsOffset,
    /* [in] */ const String& typeDiscriminatorValue,
    /* [in] */ const String& selection,
    /* [in] */ const ArrayOf<String>& selectionArgs,
    /* [in] */ const String& groupBy,
    /* [in] */ const String& having,
    /* [out] */ String* str)
{
    return BuildUnionSubQuery(
            typeDiscriminatorColumn, unionColumns, columnsPresentInTable,
            computedColumnsOffset, typeDiscriminatorValue, selection,
            groupBy, having, str);
}

ECode SQLiteQueryBuilder::BuildUnionQuery(
    /* [in] */ const ArrayOf<String>& subQueries,
    /* [in] */ const String& sortOrder,
    /* [in] */ const String& limit,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    StringBuilder query;
    Int32 subQueryCount = subQueries.GetLength();
    String unionOperator = mDistinct ? String(" UNION ") : String(" UNION ALL ");

    for (Int32 i = 0; i < subQueryCount; i++) {
        if (i > 0) {
            query += unionOperator;
        }
        query += subQueries[i];
    }
    AppendClause(query, String(" ORDER BY "), sortOrder);
    AppendClause(query, String(" LIMIT "), limit);
    *str = query.ToString();
    return NOERROR;
}

ECode SQLiteQueryBuilder::ComputeProjection(
    /* [in] */ ArrayOf<String>* projectionIn,
    /* [out] */ ArrayOf<String>** projectionOut)
{
    VALIDATE_NOT_NULL(projectionOut)
    *projectionOut = NULL;

    if (projectionIn != NULL && projectionIn->GetLength() > 0) {
        if (mProjectionMap != NULL) {
            Int32 length = projectionIn->GetLength();
            AutoPtr< ArrayOf<String> > projection = ArrayOf<String>::Alloc(length);

            for (Int32 i = 0; i < length; i++) {
                String userColumn = (*projectionIn)[i];
                AutoPtr<ICharSequence> columnObj;
                CString::New(userColumn, (ICharSequence**)&columnObj);
                AutoPtr<IInterface> value;
                mProjectionMap->Get(columnObj, (IInterface**)&value);
                String column;
                ICharSequence::Probe(value)->ToString(&column);

                if (!column.IsNull()) {
                    (*projection)[i] = column;
                    continue;
                }

                if (!mStrict &&
                        (userColumn.Contains(" AS ") || userColumn.Contains(" as "))) {
                    /* A column alias already exist */
                    (*projection)[i] = userColumn;
                    continue;
                }

                //throw new IllegalArgumentException("Invalid column "
                //        + projectionIn[i]);
                Slogger::E(TAG, "Invalid column %s", (*projectionIn)[i].string());
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            *projectionOut = projection;
            REFCOUNT_ADD(*projectionOut)
            return NOERROR;
        }
        else {
            *projectionOut = projectionIn;
            REFCOUNT_ADD(*projectionOut)
            return NOERROR;
        }
    }
    else if (mProjectionMap != NULL) {
        // Return all columns in projection map.
        AutoPtr<ISet> entrySet;
        mProjectionMap->GetEntrySet((ISet**)&entrySet);
        Int32 size;
        entrySet->GetSize(&size);
        AutoPtr< ArrayOf<String> > projection = ArrayOf<String>::Alloc(size);
        AutoPtr<IIterator> entryIter;
        entrySet->GetIterator((IIterator**)&entryIter);
        Int32 i = 0;

        Boolean hasNext = FALSE;
        while ((entryIter->HasNext(&hasNext), hasNext)) {
            AutoPtr<IInterface> outface;
            entryIter->GetNext((IInterface**)&outface);
            AutoPtr<IMapEntry> entry = IMapEntry::Probe(outface);

            // Don't include the _count column when people ask for no projection.
            AutoPtr<IInterface> obj;
            entry->GetKey((IInterface**)&obj);
            assert(ICharSequence::Probe(obj) != NULL);
            String key;
            ICharSequence::Probe(obj)->ToString(&key);
            if (key.Equals(IBaseColumns::COUNT)) {
                continue;
            }
            obj = NULL;
            entry->GetValue((IInterface**)&obj);
            String value;
            ICharSequence::Probe(obj)->ToString(&value);
            (*projection)[i++] = value;
        }

        *projectionOut = projection;
        REFCOUNT_ADD(*projectionOut)
        return NOERROR;
    }
    return NOERROR;
}

} //Sqlite
} //Database
} //Droid
} //Elastos
