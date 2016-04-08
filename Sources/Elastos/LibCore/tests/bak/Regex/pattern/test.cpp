#include "test.h"
#include <stdio.h>
#include <Elastos.CoreLibrary.h>
#include <elautoptr.h>

using namespace Elastos;

int CTest::test1(int argc, char* argv[])
{
    if (argc < 4) {
        printf("the para is error\n");
        printf("please input:\n");
        printf("             pattern 1 regex matchstr\n");
        return -1;
    }

    ECode ec = NOERROR;
    String regex = String(argv[2]);
    printf("the regex is %s\n", (const char *)regex);

    String matchStr = String(argv[3]);
    printf("the match str is %s\n", (const char *) matchStr);

    AutoPtr<ICharSequence> charSeq;
    CString::New(matchStr, (ICharSequence **)&charSeq);

    AutoPtr<IPattern> pattern;

    ec = CPattern::New(regex, 0, (IPattern **) &pattern);
    if (FAILED(ec)) {
        printf("Cann't Get Compiler!\n");
        return ec;
    } else {
        printf("Create the IPattern is Successful!\n");
    }

    Int32 start;

    String outRegex;
    pattern->Pattern(&outRegex);

    printf("the out regex is %s\n", (const char *)outRegex);

    Int32 flags = 1;
    pattern->Flags(&flags);
    printf("the flags is %d\n", flags);


    AutoPtr<IMatcher> matcher;
    pattern->Matcher(charSeq, (IMatcher **)&matcher);

    Boolean isMatched;
    matcher->Matches(&isMatched);
    printf("the isMatched is %d\n", isMatched);

    matcher->RegionStart(&start);
    printf("the start is %d\n",  start);

    matcher->Reset();

    Boolean isLookAt;
    matcher->LookingAt(&isLookAt);
    printf("Is LookAt is %d\n", isLookAt);

    matcher->RegionStart(&start);
    printf("the start is %d\n",  start);

    Boolean isFound;
    matcher->Find(&isFound);
    printf("Found is %d\n", isFound);

    matcher->RegionStart(&start);
    printf("the start is %d\n",  start);

    printf("** OK ** pattern base function!\n");
}


void testcase(Int64 num)
{
    printf("the number is %lld\n", num);
    printf("the number is %llx\n", num);
    return;
}

int CTest::test2(int argc, char* argv[])
{
    Int64 num = 0x4030303030;
    testcase(num);
    Int32 num1 = 0x80;
    testcase(num1);
    return 0;
}



int CTest::test3(int argc, char* argv[])
{
    String input("abc  edf,  xml,  ghf, 1234");
    String regex("\\s+");
    ECode ec = NOERROR;
    AutoPtr<IPattern> pattern;
    ArrayOf<String> *str;

    ec = CPattern::New(regex, 0, (IPattern **)&pattern);

    AutoPtr<ICharSequence> cs;
    CString::New(input, (ICharSequence **)&cs);
    pattern->Split(cs, &str);
    Int32 count;
    count = str->GetLength();
    for(Int32 i = 0; i < count; i++) {
        printf("the str is %s\n", (const char *) (*str)[i]);
    }
    ArrayOf<String>::Free(str);
    return 0;
}

int CTest::test4(int argc, char* argv[])
{

   if (argc < 4) {
        printf("the para is error\n");
        printf("please input:\n");
        printf("             pattern 1 regex matchstr\n");
        return -1;
    }

    ECode ec = NOERROR;
    String regex = String(argv[2]);

    String input = String(argv[3]);AutoPtr<ICharSequence> cs;
    AutoPtr<IPattern> pattern;

    ec = CPattern::New(regex, 0, (IPattern**) &pattern);
    AutoPtr<IMatcher> matcher;


    CString::New(input, (ICharSequence **)&cs);
    ec = pattern->Matcher(cs, (IMatcher **) &matcher);

    String replace("####");
    String result("");
    matcher->ReplaceFirst(replace, &result);
    printf("After Replace Text is %s\n", (const char *) result);

    String resultAll("");
    matcher->ReplaceAll(replace, &resultAll);
    printf("After Replace All Text is %s\n", (const char *) resultAll);

    printf("** OK ** Replace First!\n");
    return 0;
}


int CTest::test5(int argc, char* argv[])
{
    if (argc < 4) {
        printf("the para is error\n");
        printf("please input:\n");
        printf("             pattern 1 regex matchstr\n");
        return -1;
    }

    ECode ec = NOERROR;
    String regex = String(argv[2]);

    String input = String(argv[3]);

    AutoPtr<ICharSequence> cs;
    AutoPtr<IPattern> pattern;

    ec = CPattern::New(regex, 0, (IPattern**) &pattern);
    AutoPtr<IMatcher> matcher;


    CString::New(input, (ICharSequence **)&cs);
    ec = pattern->Matcher(cs, (IMatcher **) &matcher);

    Boolean find;
    matcher->Find(&find);

    AutoPtr<IMatchResult> mr;
    matcher->ToMatchResult((IMatchResult **) &mr);

    Int32 si,ei;
    mr->End(&ei);

    mr->Start(&si);

    String str("");
    mr->Group(&str);
    printf("the str is %s\n", (const char *) str);

    Int32 gc;
    mr->GroupCount(&gc);

    printf("** OK ** Match Result!\n");
    return 0;
}
