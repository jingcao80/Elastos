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

#ifndef __ELASTOS_SQL_SQLITE_JDBC_CJDBCDATABASEMETADATA_H__
#define __ELASTOS_SQL_SQLITE_JDBC_CJDBCDATABASEMETADATA_H__

#include "_Elastos_Sql_SQLite_JDBC_CJDBCDatabaseMetaData.h"
#include "CJDBCConnection.h"
#include "CJDBCDriver.h"
#include "CJDBCStatement.h"
#include "CShell.h"
#include "CTableResult.h"
#include "CJDBCResultSet.h"
#include "CTableResultX.h"

using Elastos::Sql::IResultSet;

namespace Elastos {
namespace Sql {
namespace SQLite {
namespace JDBC{

CarClass(CJDBCDatabaseMetaData)
    , public Object
    , public IJDBCDatabaseMetaData
    , public IDatabaseMetaData
    , public IWrapper
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI AllProceduresAreCallable(
        /* [out] */ Boolean * callable);

    CARAPI AllTablesAreSelectable(
        /* [out] */ Boolean * selectable);

    CARAPI DataDefinitionCausesTransactionCommit(
        /* [out] */ Boolean * commit);

    CARAPI DataDefinitionIgnoredInTransactions(
        /* [out] */ Boolean * ignored);

    CARAPI DeletesAreDetected(
        /* [in] */ Int32 type,
        /* [out] */ Boolean * detected);

    CARAPI DoesMaxRowSizeIncludeBlobs(
        /* [out] */ Boolean * doesmax);

    CARAPI GetAttributes(
        /* [in] */ const String& catalog,
        /* [in] */ const String& schemaPattern,
        /* [in] */ const String& typeNamePattern,
        /* [in] */ const String& attributeNamePattern,
        /* [out] */ IResultSet ** ppResultSet);

    CARAPI GetBestRowIdentifier(
        /* [in] */ const String& catalog,
        /* [in] */ const String& schema,
        /* [in] */ const String& table,
        /* [in] */ Int32 scope,
        /* [in] */ Boolean nullable,
        /* [out] */ IResultSet ** resultSet);

    CARAPI GetCatalogs(
        /* [out] */ IResultSet ** resultSet);

    CARAPI GetCatalogSeparator(
        /* [out] */ String * str);

    CARAPI GetCatalogTerm(
        /* [out] */ String * str);

    CARAPI GetColumnPrivileges(
        /* [in] */ const String& catalog,
        /* [in] */ const String& schema,
        /* [in] */ const String& table,
        /* [in] */ const String& columnNamePattern,
        /* [out] */ IResultSet ** resultSet);

    CARAPI GetColumns(
        /* [in] */ const String& catalog,
        /* [in] */ const String& schemaPattern,
        /* [in] */ const String& tableNamePattern,
        /* [in] */ const String& columnNamePattern,
        /* [out] */ IResultSet ** resultSet);

    CARAPI GetConnection(
        /* [out] */ IConnection ** connection);

    CARAPI GetCrossReference(
        /* [in] */ const String& primaryCatalog,
        /* [in] */ const String& primarySchema,
        /* [in] */ const String& primaryTable,
        /* [in] */ const String& foreignCatalog,
        /* [in] */ const String& foreignSchema,
        /* [in] */ const String& foreignTable,
        /* [out] */ IResultSet ** resultSet);

    CARAPI GetDatabaseMajorVersion(
        /* [out] */ Int32 * ver);

    CARAPI GetDatabaseMinorVersion(
        /* [out] */ Int32 * ver);

    CARAPI GetDatabaseProductName(
        /* [out] */ String * name);

    CARAPI GetDatabaseProductVersion(
        /* [out] */ String * ver);

    CARAPI GetDefaultTransactionIsolation(
        /* [out] */ Int32 * level);

    CARAPI GetDriverMajorVersion(
        /* [out] */ Int32 * ver);

    CARAPI GetDriverMinorVersion(
        /* [out] */ Int32 * ver);

    CARAPI GetDriverName(
        /* [out] */ String * name);

    CARAPI GetDriverVersion(
        /* [out] */ String * ver);

    CARAPI GetExportedKeys(
        /* [in] */ const String& catalog,
        /* [in] */ const String& schema,
        /* [in] */ const String& table,
        /* [out] */ IResultSet ** resultSet);

    CARAPI GetExtraNameCharacters(
        /* [out] */ String * name);

    CARAPI GetIdentifierQuoteString(
        /* [out] */ String * quote);

    CARAPI GetImportedKeys(
        /* [in] */ const String& catalog,
        /* [in] */ const String& schema,
        /* [in] */ const String& table,
        /* [out] */ IResultSet ** resultSet);

    CARAPI GetIndexInfo(
        /* [in] */ const String& catalog,
        /* [in] */ const String& schema,
        /* [in] */ const String& table,
        /* [in] */ Boolean unique,
        /* [in] */ Boolean approximate,
        /* [out] */ IResultSet ** resultSet);

    CARAPI GetJDBCMajorVersion(
        /* [out] */ Int32 * ver);

    CARAPI GetJDBCMinorVersion(
        /* [out] */ Int32 * ver);

    CARAPI GetMaxBinaryLiteralLength(
        /* [out] */ Int32 * len);

    CARAPI GetMaxCatalogNameLength(
        /* [out] */ Int32 * len);

    CARAPI GetMaxCharLiteralLength(
        /* [out] */ Int32 * len);

    CARAPI GetMaxColumnNameLength(
        /* [out] */ Int32 * len);

    CARAPI GetMaxColumnsInGroupBy(
        /* [out] */ Int32 * number);

    CARAPI GetMaxColumnsInIndex(
        /* [out] */ Int32 * number);

    CARAPI GetMaxColumnsInOrderBy(
        /* [out] */ Int32 * number);

    CARAPI GetMaxColumnsInSelect(
        /* [out] */ Int32 * number);

    CARAPI GetMaxColumnsInTable(
        /* [out] */ Int32 * number);

    CARAPI GetMaxConnections(
        /* [out] */ Int32 * connections);

    CARAPI GetMaxCursorNameLength(
        /* [out] */ Int32 * len);

    CARAPI GetMaxIndexLength(
        /* [out] */ Int32 * len);

    CARAPI GetMaxProcedureNameLength(
        /* [out] */ Int32 * len);

    CARAPI GetMaxRowSize(
        /* [out] */ Int32 * number);

    CARAPI GetMaxSchemaNameLength(
        /* [out] */ Int32 * len);

    CARAPI GetMaxStatementLength(
        /* [out] */ Int32 * len);

    CARAPI GetMaxStatements(
        /* [out] */ Int32 * number);

    CARAPI GetMaxTableNameLength(
        /* [out] */ Int32 * len);

    CARAPI GetMaxTablesInSelect(
        /* [out] */ Int32 * number);

    CARAPI GetMaxUserNameLength(
        /* [out] */ Int32 * len);

    CARAPI GetNumericFunctions(
        /* [out] */ String * len);

    CARAPI GetPrimaryKeys(
        /* [in] */ const String& catalog,
        /* [in] */ const String& schema,
        /* [in] */ const String& table,
        /* [out] */ IResultSet ** resultSet);

    CARAPI GetProcedureColumns(
        /* [in] */ const String& catalog,
        /* [in] */ const String& schemaPattern,
        /* [in] */ const String& procedureNamePattern,
        /* [in] */ const String& columnNamePattern,
        /* [out] */ IResultSet ** resultSet);

    CARAPI GetProcedures(
        /* [in] */ const String& catalog,
        /* [in] */ const String& schemaPattern,
        /* [in] */ const String& procedureNamePattern,
        /* [out] */ IResultSet ** resultSet);

    CARAPI GetProcedureTerm(
        /* [out] */ String * prefer);

    CARAPI GetResultSetHoldability(
        /* [out] */ Int32 * code);

    CARAPI GetSchemas(
        /* [out] */ IResultSet ** resultSet);

    CARAPI GetSchemaTerm(
        /* [out] */ String * term);

    CARAPI GetSearchStringEscape(
        /* [out] */ String * escape);

    CARAPI SetSQLKeywords(
        /* [out] */ String * key);

    CARAPI GetSQLStateType(
        /* [out] */ Int32 * type);

    CARAPI GetStringFunctions(
        /* [out] */ String * str);

    CARAPI GetSuperTables(
        /* [in] */ const String& catalog,
        /* [in] */ const String& schemaPattern,
        /* [in] */ const String& tableNamePattern,
        /* [out] */ IResultSet ** resultSet);

    CARAPI GetSuperTypes(
        /* [in] */ const String& catalog,
        /* [in] */ const String& schemaPattern,
        /* [in] */ const String& typeNamePattern,
        /* [out] */ IResultSet ** resultSet);

    CARAPI GetSystemFunctions(
        /* [out] */ String * str);

    CARAPI GetTablePrivileges(
        /* [in] */ const String& catalog,
        /* [in] */ const String& schemaPattern,
        /* [in] */ const String& tableNamePattern,
        /* [out] */ IResultSet ** resultSet);

    CARAPI GetTables(
        /* [in] */ const String& catalog,
        /* [in] */ const String& schemaPattern,
        /* [in] */ const String& tableNamePattern,
        /* [in] */ const ArrayOf<String> & types,
        /* [out] */ IResultSet ** resultSet);

    CARAPI GetTableTypes(
        /* [out] */ IResultSet ** resultSet);

    CARAPI GetTimeDateFunctions(
        /* [out] */ String * timedate);

    CARAPI GetTypeInfo(
        /* [out] */ IResultSet ** resultSet);

    CARAPI GetUDTs(
        /* [in] */ const String& catalog,
        /* [in] */ const String& schemaPattern,
        /* [in] */ const String& typeNamePattern,
        /* [in] */ const ArrayOf<Int32> & types,
        /* [out] */ IResultSet ** resultSet);

    CARAPI GetURL(
        /* [out] */ String * url);

    CARAPI GetUserName(
        /* [out] */ String * name);

    CARAPI GetVersionColumns(
        /* [in] */ const String& catalog,
        /* [in] */ const String& schema,
        /* [in] */ const String& table,
        /* [out] */ IResultSet ** resultSet);

    CARAPI InsertsAreDetected(
        /* [in] */ Int32 type,
        /* [out] */ Boolean * detected);

    CARAPI IsCatalogAtStart(
        /* [out] */ Boolean * isCatalog);

    CARAPI IsReadOnly(
        /* [out] */ Boolean * isReadOnly);

    CARAPI LocatorsUpdateCopy(
        /* [out] */ Boolean * isCopy);

    CARAPI NullPlusNonNullIsNull(
        /* [out] */ Boolean * isNull);

    CARAPI NullsAreSortedAtEnd(
        /* [out] */ Boolean * isNull);

    CARAPI NullsAreSortedAtStart(
        /* [out] */ Boolean * isNull);

    CARAPI NullsAreSortedHigh(
        /* [out] */ Boolean * isNull);

    CARAPI NullsAreSortedLow(
        /* [out] */ Boolean * isNull);

    CARAPI OthersDeletesAreVisible(
        /* [in] */ Int32 type,
        /* [out] */ Boolean * isVisible);

    CARAPI OthersInsertsAreVisible(
        /* [in] */ Int32 type,
        /* [out] */ Boolean * isVisible);

    CARAPI OthersUpdatesAreVisible(
        /* [in] */ Int32 type,
        /* [out] */ Boolean * isVisible);

    CARAPI OwnDeletesAreVisible(
        /* [in] */ Int32 type,
        /* [out] */ Boolean * isVisible);

    CARAPI OwnInsertsAreVisible(
        /* [in] */ Int32 type,
        /* [out] */ Boolean * isVisible);

    CARAPI OwnUpdatesAreVisible(
        /* [in] */ Int32 type,
        /* [out] */ Boolean * isVisible);

    CARAPI StoresLowerCaseIdentifiers(
        /* [out] */ Boolean * isStored);

    CARAPI StoresLowerCaseQuotedIdentifiers(
        /* [out] */ Boolean * isStored);

    CARAPI StoresMixedCaseIdentifiers(
        /* [out] */ Boolean * isStored);

    CARAPI StoresMixedCaseQuotedIdentifiers(
        /* [out] */ Boolean * isStored);

    CARAPI StoresUpperCaseIdentifiers(
        /* [out] */ Boolean * isStored);

    CARAPI StoresUpperCaseQuotedIdentifiers(
        /* [out] */ Boolean * isStored);

    CARAPI SupportsAlterTableWithAddColumn(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsAlterTableWithDropColumn(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsANSI92EntryLevelSQL(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsANSI92FullSQL(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsANSI92IntermediateSQL(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsBatchUpdates(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsCatalogsInDataManipulation(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsCatalogsInIndexDefinitions(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsCatalogsInPrivilegeDefinitions(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsCatalogsInProcedureCalls(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsCatalogsInTableDefinitions(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsColumnAliasing(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsConvert(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsConvert(
        /* [in] */ Int32 fromType,
        /* [in] */ Int32 toType,
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsCoreSQLGrammar(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsCorrelatedSubqueries(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsDataDefinitionAndDataManipulationTransactions(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsDataManipulationTransactionsOnly(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsDifferentTableCorrelationNames(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsExpressionsInOrderBy(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsExtendedSQLGrammar(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsFullOuterJoins(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsGetGeneratedKeys(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsGroupBy(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsGroupByBeyondSelect(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsGroupByUnrelated(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsIntegrityEnhancementFacility(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsLikeEscapeClause(
        /* [out] */ Boolean * value);

    CARAPI SupportsLimitedOuterJoins(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsMinimumSQLGrammar(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsMixedCaseIdentifiers(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsMixedCaseQuotedIdentifiers(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsMultipleOpenResults(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsMultipleResultSets(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsMultipleTransactions(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsNamedParameters(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsNonNullableColumns(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsOpenCursorsAcrossCommit(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsOpenCursorsAcrossRollback(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsOpenStatementsAcrossCommit(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsOpenStatementsAcrossRollback(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsOrderByUnrelated(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsOuterJoins(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsPositionedDelete(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsPositionedUpdate(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsResultSetConcurrency(
        /* [in] */ Int32 type,
        /* [in] */ Int32 concurrency,
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsResultSetHoldability(
        /* [in] */ Int32 holdability,
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsResultSetType(
        /* [in] */ Int32 type,
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsSavepoints(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsSchemasInDataManipulation(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsSchemasInIndexDefinitions(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsSchemasInPrivilegeDefinitions(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsSchemasInProcedureCalls(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsSchemasInTableDefinitions(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsSelectForUpdate(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsStatementPooling(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsStoredProcedures(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsSubqueriesInComparisons(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsSubqueriesInExists(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsSubqueriesInIns(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsSubqueriesInQuantifieds(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsTableCorrelationNames(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsTransactionIsolationLevel(
        /* [in] */ Int32 level,
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsTransactions(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsUnion(
        /* [out] */ Boolean * isSupported);

    CARAPI SupportsUnionAll(
        /* [out] */ Boolean * isSupported);

    CARAPI UpdatesAreDetected(
        /* [in] */ Int32 type,
        /* [out] */ Boolean * isUpdate);

    CARAPI UsesLocalFilePerTable(
        /* [out] */ Boolean * isUsed);

    CARAPI UsesLocalFiles(
        /* [out] */ Boolean * isStored);

    CARAPI AutoCommitFailureClosesAllResultSets(
        /* [out] */ Boolean * isClosed);

    CARAPI GetClientInfoProperties(
        /* [out] */ IResultSet ** resultSet);

    CARAPI GetFunctionColumns(
        /* [in] */ const String& catalog,
        /* [in] */ const String& schemaPattern,
        /* [in] */ const String& functionNamePattern,
        /* [in] */ const String& columnNamePattern,
        /* [out] */ IResultSet ** resultSet);

    CARAPI GetFunctions(
        /* [in] */ const String& catalog,
        /* [in] */ const String& schemaPattern,
        /* [in] */ const String& functionNamePattern,
        /* [out] */ IResultSet ** resultSet);

    CARAPI GetRowIdLifetime(
        /* [out] */ RowIdLifeTime * rowIdLifeTime);

    CARAPI SetSchemas(
        /* [in] */ const String& catalog,
        /* [in] */ const String& schemaPattern,
        /* [out] */ IResultSet ** resultSet);

    CARAPI SupportsStoredFunctionsUsingCallSyntax(
        /* [out] */ Boolean * isSupported);

    CARAPI constructor(
        /* [in] */ IJDBCConnection * pConn);

    CARAPI GetSQLKeywords(
        /* [out] */ String* str);

    CARAPI IsWrapperFor(
        /* [in] */ IInterface * iface,
        /* [out] */ Boolean * value);
    CARAPI Unwrap(
        /* [in] */ PInterface iface,
        /* [out] */ IInterface ** oface);

    static CARAPI_(String) MapTypeName(Int32 type);

    static CARAPI_(Int32) MapSqlType(const String& type);

    static CARAPI_(Int32) GetM(const String& typeStr, Int32 type);

    static CARAPI_(Int32) GetD(const String& typeStr, Int32 type);

    CARAPI_(AutoPtr<IResultSet>) GetSchemas(const String& cat, const String& schema);

    CARAPI_(Boolean) IsWrapperFor(IInterface* iface);

    // <T> T unwrap(java.lang.Class<T> iface);

private:
    CARAPI_(void) InternalImportedKeys(const String& table, const String& pktable,
                                        CJDBCResultSet * in, CTableResultX * out);

private:
    AutoPtr<CJDBCConnection> conn;
};

} // namespace JDBC
} // namespace SQLite
} // namespace Sql
} // namespace Elastos

#endif // __ELASTOS_SQL_SQLITE_JDBC_CJDBCDATABASEMETADATA_H__
