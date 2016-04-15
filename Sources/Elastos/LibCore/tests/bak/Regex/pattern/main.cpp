#include "test.h"
#include <stdio.h>
#include <stdlib.h>
#include <Elastos.CoreLibrary.h>
#include <elautoptr.h>

typedef int (CTest::*PTestEntry)(int argc, char *argv[]);

PTestEntry TestEntry[] =
{
    &CTest::test1,
    &CTest::test2,
    &CTest::test3,
    &CTest::test4,
    &CTest::test5,
};

int main(int argc, char* argv[])
{
    if (argc < 4) {
        printf("the para is error\n");
        printf("please input:\n");
        printf("             pattern 1 regex matchstr\n");
        return -1;
    }

    int nIndex = atoi(argv[1]);

    CTest testObj;
    (testObj.*TestEntry[nIndex-1])(argc, argv);
   /*
    AutoPtr<IPattern> pattern;
    ECode ec;
    ec = CPattern::New((IPattern**)&pattern);
    if (FAILED(ec)) {
        printf("Cann't Create the pattern!\n");
        return -1;
    }

    ec = CPattern::New(String("pattern"), 0, (IPattern **) &pattern);
    if (FAILED(ec)) {
        printf("Cann't Get Compiler!\n");
        return ec;
    } else {
        printf("Create the IPattern is Successful!\n");
    }

    String outRegex;
    pattern->Pattern(&outRegex);

    printf("the out regex is %s\n", (const char *)outRegex);

    Int32 flags = 1;
    pattern->Flags(&flags);
    printf("the flags is %d\n", flags);
    */
    return 0;
}
