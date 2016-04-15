// #include <elastos.h>
// #include <cmdef.h>
// #include <elautoptr.h>
// #include <eltypes.h>
// #include <Elastos.Sql.h>
// #include <stdio.h>
// #include <elastos/Mutex.h>

// using namespace Elastos;
// using namespace Elastos::Sql::SQLite;
// using namespace Elastos::Sql;
#include"OldDatabaseTest.h"

void testSqlDatabase()
{
    // printf("error===>%d,%d,%d,%d,%d\n", EINVAL,EBUSY,EAGAIN,EDEADLK,EPERM);
    AutoPtr<IDatabase> db;
    Int32 mode = 0x00000004;
    String basename("test.db");
    ECode ec = CDatabase::New((IDatabase**)&db);
    // printf("==== ec: 0x%x ====\n", ec);
    String sql(NULL);
    if (db != NULL) {
        // test create or open database
        ec = db->Open(basename,mode);
        // ec = db->OpenAuxFile(String("dsfds"));
        if (ec != NOERROR)
        {
            printf("ems==> 0x%x\n", ec);
            return;
        }

        AutoPtr<IStmt> stmt;
        CStmt::New((IStmt**)&stmt);

        // test create table
        // sql = String("create table student (_id integer PRIMARY KEY , name text);");
        // db->Exec(sql,(ICallback *)stmt.Get());

        // test insert
        // Int32 count(8);
        // for (Int32 i = 0; i < count; ++i)
        // {
        //     sql = String("insert into student values(")+StringUtils::Int32ToString(i)+String(",")
        //           +String("'")+StringUtils::Int32ToString(i)+String("aa');");
        //     db->Exec(sql,(ICallback *)stmt.Get());
        //     printf("flag====>%d\n", i);
        // }

        //test delete
        // sql = String("delete from student ;");
        // db->Exec(sql,(ICallback *)stmt.Get());

        //test update
        // sql = String("update student set name = '%q-%q' where _id = 2 ;");
        // AutoPtr<ArrayOf<String> > argsstr = ArrayOf<String>::Alloc(2);
        // (*argsstr)[0] = String("00");
        // (*argsstr)[1] = String("11");
        // db->ExecEx(sql,(ICallback *)stmt.Get(),*argsstr);

        // test Interrupt
        // db->Interrupt();

        // test BusyHandler
        // AutoPtr<IBusyHandler> bh;
        // CBusyHandler::New();
        // db->BusyHandler();

        // test timeout
        // db->BusyTimeout(1000);

        // test LastInsertRowid
        // Int64 out = (Int64)0;
        // db->LastInsertRowid(&out);
        // printf("LastInsertRowid===>%lld\n", out);

        // test Changes
        // db->Changes(&out);
        // printf("Changes===>%lld\n", out);

        // test GetTable
        // String sql = String("select * from student where _id = %q;");
        // AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(1);
        // (*args)[0] = String("3");
        // Int32 rows = 5;
        // AutoPtr<ITableResult> otable = NULL;
        // CTableResult::New((ITableResult**)&otable);
        // printf("otable first===>%lld\n", (Int64)otable.Get());
        // // db->GetTableEx3(sql,*args,(ITableResult**)&otable);
        // db->GetTableEx4(sql,*args,otable);
        // printf("otable second===>%lld\n", (Int64)otable.Get());
        // otable->ToString(&sql);
        // printf("sql==>%s\n", sql.string());

        //test Dbversion
        // String ver;
        // db->Dbversion(&ver);
        // printf("ver==>%s\n", ver.string());

        // test CreateFunction
        // AutoPtr<IFunction> f;
        // String name("2aa");
        // Int32 nargs(2);
        // db->CreateFunction(name,nargs,f);


        // test close database
        ec = db->Close();
        printf("ec==>%d\n", ec);
        if (ec != NOERROR) {
            printf("ems==> 0x%x\n", ec);
            return;
        }
        // free(otable);
        // otable = NULL;
        printf("testSqlDatabase\n");


    }

    printf("outdb \n");

}

int main(int argc, char *argv[])
{
    // testSqlDatabase();
    OldDatabaseTest testdb;
    //test open
    testdb.setUp();

    // testdb.testDatabase();

    // testdb.testOpen();

    // testdb.testOpenAuxFile();

    // testdb.testClose();

    // testdb.testExecStringCallback();

    // testdb.testExecStringCallbackStringArray();

    // testdb.testLast_insert_rowid();

    // testdb.testChanges();

    // testdb.testGetTableString();

    // testdb.testGetTableStringStringArray();

    // testdb.testGetTableStringStringArrayTableResult();

    // testdb.testComplete();

    // testdb.testVersion();

    // testdb.testDbversion();

    // testdb.testCreate_function();

    // testdb.testCreate_aggregate();

    // testdb.testFunction_type();

    // testdb.testLast_error();

    // testdb.testSet_last_error();

    // testdb.testError_message();

    // testdb.testError_string();

    // testdb.testSet_encoding();

    // testdb.testSet_authorizer();

    // testdb.testTrace();

    // testdb.testCompileString();

    // testdb.testCompileStringStringArray();

    // testdb.testPrepare();

    // testdb.testOpen_blob();

    // testdb.testIs3();

    testdb.testProgress_handler();



    //test close
    testdb.tearDown();

    printf("finish~~~\n");
    return 0;
}
