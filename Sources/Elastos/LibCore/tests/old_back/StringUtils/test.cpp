
#include <elastos.h>
#include <stdio.h>
#include <new>
#include <Elastos.CoreLibrary.h>
#include <elastos/StringUtils.h>
#include <elastos/StringBuilder.h>

using namespace Elastos;
using Elastos::Core::StringUtils;
using Elastos::Core::CString;
using Elastos::Core::StringBuilder;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Regex::CSplitter;
using Elastos::Utility::Regex::ISplitter;
using Elastos::Utility::Regex::IPattern;
using Elastos::Utility::Regex::IPatternHelper;
using Elastos::Utility::Regex::CPatternHelper;
using Elastos::Utility::Regex::IMatcher;
using Elastos::Utility::Regex::CMatcher;

void assertEquals(const String& aspect, const String& test)
{
    printf("aspect: [%s], test: [%s]\n", aspect.string(), test.string());
    assert(aspect.Equals(test));
}

void testString()
{
    String n1, n2;
    assert(n1.Equals(n2));
    assert(n1 == n2);
    assert(n2 == NULL);
    assert(NULL == n2);
    n1 = String("Not NULL");
    assert(n1 != NULL);
    assert(NULL != n1);
    assert(n1 != n2);
    assert(!n1.Equals(n2));
}

void printStringArray(ArrayOf<String>* result)
{
    printf("\n==============================\n");
    for (Int32 i = 0; i < result->GetLength(); ++i) {
       printf(" >> %d : [%s]\n", i, (*result)[i].string());
    }
}

void testSplit()
{
    {
        printf("\n========StringUtils======\n");
        const String testString("j,  ,ja, aj, aja, jj, jja, j");

        AutoPtr<ArrayOf<String> > result;
        ECode ec = StringUtils::Split(testString, String("j"),
            (ArrayOf<String>**)&result);
        printStringArray(result);
    }

    {
        printf("\n========IPattern======\n");
        const String testString("7");

        const String patternStr("\\s*,\\s*");
        AutoPtr<IPatternHelper> helper;
        CPatternHelper::AcquireSingleton((IPatternHelper**)&helper);
        AutoPtr<IPattern> pattern;
        helper->Compile(patternStr, (IPattern**)&pattern);
        AutoPtr<ArrayOf<String> > result;
        pattern->Split(testString, (ArrayOf<String>**)&result);
        printStringArray(result);
    }

    {
        printf("\n========IPattern======\n");
        const String testString("2,4");

        const String patternStr("\\s*,\\s*");
        AutoPtr<IPatternHelper> helper;
        CPatternHelper::AcquireSingleton((IPatternHelper**)&helper);
        AutoPtr<IPattern> pattern;
        helper->Compile(patternStr, (IPattern**)&pattern);
        AutoPtr<ArrayOf<String> > result;
        pattern->Split(testString, (ArrayOf<String>**)&result);
        printStringArray(result);
    }

    {
        printf("\n========IMatcher======\n");
        AutoPtr<IPatternHelper> helper;
        AutoPtr<IPattern> pattern;
        AutoPtr<IPattern> pattern_new;
        AutoPtr<IMatcher> matcher;
        AutoPtr<ICharSequence> input;
        String regex;
        ECode ec;

        CPatternHelper::AcquireSingleton((IPatternHelper**)&helper);
        helper->CompileEx(String("abc"), 0, (IPattern**)&pattern);

        CString::New(String("abc ABC  123  !@#"), (ICharSequence**)&input);
        CMatcher::New(pattern, input, (IMatcher**)&matcher);

        matcher->Pattern((IPattern**)&pattern_new);
        assert(pattern_new != NULL);

        pattern_new->GetPattern(&regex);
        printf("regex = %s\n", (const char *)regex);
    }
}

void testReplaceFirst()
{
    printf("\n====String::testReplaceFirst========\n");

    {
        String str("!'123123.123HelloWorld!123123helloworld#");
        String patterns[4] = {
            String("[hw\\p{Upper}]"),
            String("(o|l){2,}"),
            String("([\'\"]?)(\\d+)"),
            String("^!.*#$")
        };

        String results[4] = {
            String("!'123123.123?elloWorld!123123helloworld#"),
            String("!'123123.123He?World!123123helloworld#"),
            String("!?.123HelloWorld!123123helloworld#"),
            String("?")
        };

        ECode ec;
        String result;
        for(int i = 0; i < 4; i++) {
            ec = StringUtils::ReplaceFirst(str, patterns[i], String("?"), &result);
            assertEquals(results[i], result);
        }
    }
}

void testReplaceAll()
{
    printf("\n====String::testReplaceAll========\n");

    {
        String str("!'123123.123HelloWorld!123123helloworld#");
        String patterns[4] = {
            String("[hw\\p{Upper}]"),
            String("(o|l){2,}"),
            String("([\'\"]?)(\\d+)"),
            String("^!.*#$")
        };

        String results[4] = {
            String("!\'123123.123?ello?orld!123123?ello?orld#"),
            String("!\'123123.123He?World!123123he?world#"),
            String("!?.?HelloWorld!?helloworld#"),
            String("?")
        };

        ECode ec;
        String result;
        for(int i = 0; i < 4; i++) {
            ec = StringUtils::ReplaceAll(str, patterns[i], String("?"), &result);
            assertEquals(results[i], result);
        }
    }

    String aspect("project_Id");
    String test;
    ECode ec = StringUtils::ReplaceAll(aspect, String("(?!^)(\\p{Upper})(?!$)"), String("$1"), &test);

    assertEquals(aspect, test);

    aspect = String("1&amp;2");
    StringUtils::ReplaceAll(String("1&2"), "&", "&amp;", &test);
    assertEquals(aspect, test);

    aspect = String("1&lt;2");
    StringUtils::ReplaceAll(String("1<2"), "<", "&lt;", &test);
    assertEquals(aspect, test);

    aspect = String("1&gt;2");
    StringUtils::ReplaceAll(String("1>2"), ">", "&gt;", &test);
    assertEquals(aspect, test);

    aspect = String("1&apos;2");
    StringUtils::ReplaceAll(String("1'2"), "'", "&apos;", &test);
    assertEquals(aspect, test);

    aspect = String("1&quot;2");
    StringUtils::ReplaceAll(String("1\\2"), "\\\\", "&quot;", &test);
    assertEquals(aspect, test);
}

void testMatches()
{
    printf("begin String::testMatches\n");

    Boolean* result;
    printf("call IStringUtils::Matches\n");
    ECode ec = StringUtils::Matches(String("jiazhenjiang"), String("i"), result);
    if(*result){
        printf("test failed\n");
    }else{
        printf("test true\n");
    }
    printf("testMatches is after\n");
}

void testReplace()
{
    String str("#,##0.###");
    printf(" ### Original: [%s]\n", str.string());
    String temp;
    StringUtils::ReplaceAll(str, String("\\."), String(""), &temp);
    printf(" ##1 temp: [%s]\n", temp.string());
}

void testTrim()
{
    {
        printf("\n=========TrimStart()==========");
        AutoPtr<ArrayOf<String> > strs = ArrayOf<String>::Alloc(10);
        strs->Set(0, String(" A"));
        strs->Set(1, String(" AB"));
        strs->Set(2, String("A "));
        strs->Set(3, String("AB "));
        strs->Set(4, String(" A "));
        strs->Set(5, String("  AB "));
        strs->Set(6, String("A"));
        strs->Set(7, String("AB"));
        strs->Set(8, String("A B"));
        strs->Set(9, String("  "));

        printStringArray(strs);

        for (Int32 i = 0; i < strs->GetLength(); ++i) {
            String str = (*strs)[i];
            (*strs)[i] = str.TrimStart();
        }

        printStringArray(strs);
    }

    {
        printf("\n=========TrimEnd()==========");
        AutoPtr<ArrayOf<String> > strs = ArrayOf<String>::Alloc(10);
        strs->Set(0, String(" A"));
        strs->Set(1, String(" AB"));
        strs->Set(2, String("A "));
        strs->Set(3, String("AB "));
        strs->Set(4, String(" A "));
        strs->Set(5, String("  AB "));
        strs->Set(6, String("A"));
        strs->Set(7, String("AB"));
        strs->Set(8, String("A B"));
        strs->Set(9, String("  "));

        printStringArray(strs);

        for (Int32 i = 0; i < strs->GetLength(); ++i) {
            String str = (*strs)[i];
            (*strs)[i] = str.TrimEnd();
        }

        printStringArray(strs);
    }

    {
        printf("\n=========Trim()==========");
        AutoPtr<ArrayOf<String> > strs = ArrayOf<String>::Alloc(10);
        strs->Set(0, String(" A"));
        strs->Set(1, String(" AB"));
        strs->Set(2, String("A "));
        strs->Set(3, String("AB "));
        strs->Set(4, String(" A "));
        strs->Set(5, String("  AB "));
        strs->Set(6, String("A"));
        strs->Set(7, String("AB"));
        strs->Set(8, String("A B"));
        strs->Set(9, String("  "));

        printStringArray(strs);

        for (Int32 i = 0; i < strs->GetLength(); ++i) {
            String str = (*strs)[i];
            (*strs)[i] = str.Trim();
        }

        printStringArray(strs);
    }
}

void testSubstring()
{
    printf("\n- - - testSubstring()- - - \n");

    String str("0123456789");
    String subStr = str.Substring(0, 10);
    printf(" > str.Substring(0, 10): [%s]\n", subStr.string());
    subStr = str.Substring(1, 10);
    printf(" > str.Substring(1, 10): [%s]\n", subStr.string());
    subStr = str.Substring(8, 10);
    printf(" > str.Substring(8, 10): [%s]\n", subStr.string());
    subStr = str.Substring(9, 10);
    printf(" > str.Substring(9, 10): [%s]\n", subStr.string());
    subStr = str.Substring(10, 10);
    printf(" > str.Substring(10, 10): [%s]\n", subStr.string());

    subStr = str.Substring(0, 9);
    printf(" > str.Substring(0, 9): [%s]\n", subStr.string());
    subStr = str.Substring(1, 9);
    printf(" > str.Substring(1, 9): [%s]\n", subStr.string());
    subStr = str.Substring(8, 9);
    printf(" > str.Substring(8, 9): [%s]\n", subStr.string());
    subStr = str.Substring(9, 9);
    printf(" > str.Substring(9, 9): [%s]\n", subStr.string());

    String zhStr("和路由器一样，Binder驱动虽然默默无闻，却是通信的核心。");
    Int32 charCount = zhStr.GetLength();
    Int32 end = charCount;
    Int32 start = 0;
    subStr = zhStr.Substring(start, end);
    printf(" > str.Substring(%d, %d): [%s]\n", start, end, subStr.string());
    end = charCount - 1;
    subStr = zhStr.Substring(start, end);
    printf(" > str.Substring(%d, %d): [%s]\n", start, end, subStr.string());
    end = charCount - 2;
    subStr = zhStr.Substring(start, end);
    printf(" > str.Substring(%d, %d): [%s]\n", start, end, subStr.string());
    end = charCount;
    start = 6;
    subStr = zhStr.Substring(start, end);
    printf(" > str.Substring(%d, %d): [%s]\n", start, end, subStr.string());
    start = 7;
    subStr = zhStr.Substring(start, end);
    printf(" > str.Substring(%d, %d): [%s]\n", start, end, subStr.string());
    start = 8;
    subStr = zhStr.Substring(start, end);
    printf(" > str.Substring(%d, %d): [%s]\n", start, end, subStr.string());
    start = 9;
    subStr = zhStr.Substring(start, end);
    printf(" > str.Substring(%d, %d): [%s]\n", start, end, subStr.string());
    start = 12;
    subStr = zhStr.Substring(start, end);
    printf(" > str.Substring(%d, %d): [%s]\n", start, end, subStr.string());
    start = 13;
    subStr = zhStr.Substring(start, end);
    printf(" > str.Substring(%d, %d): [%s]\n", start, end, subStr.string());
    start = 14;
    subStr = zhStr.Substring(start, end);
    printf(" > str.Substring(%d, %d): [%s]\n", start, end, subStr.string());
    start = 14;
    end = charCount - 1;
    subStr = zhStr.Substring(start, end);
    printf(" > str.Substring(%d, %d): [%s]\n", start, end, subStr.string());
    start = 14;
    end = charCount - 2;
    subStr = zhStr.Substring(start, end);
    printf(" > str.Substring(%d, %d): [%s]\n", start, end, subStr.string());
    end = charCount - 15;
    subStr = zhStr.Substring(start, end);
    printf(" > str.Substring(%d, %d): [%s]\n", start, end, subStr.string());
    end = charCount - 16;
    subStr = zhStr.Substring(start, end);
    printf(" > str.Substring(%d, %d): [%s]\n", start, end, subStr.string());
}

void testStringBuilder()
{
    printf("\n- - - - testStringBuilder- - - \n");
    StringBuilder str("和路由器一样，Binder驱动虽然默默无闻，却是通信的核心。");
    Int32 charCount = str.GetLength();
    Int32 end = charCount;
    Int32 start = 0;
    String subStr = str.Substring(start, end);
    printf(" > str.Substring(%d, %d): [%s]\n", start, end, subStr.string());
    end = charCount - 1;
    subStr = str.Substring(start, end);
    printf(" > str.Substring(%d, %d): [%s]\n", start, end, subStr.string());
    end = charCount - 2;
    subStr = str.Substring(start, end);
    printf(" > str.Substring(%d, %d): [%s]\n", start, end, subStr.string());
    end = charCount;
    start = 6;
    subStr = str.Substring(start, end);
    printf(" > str.Substring(%d, %d): [%s]\n", start, end, subStr.string());
    start = 7;
    subStr = str.Substring(start, end);
    printf(" > str.Substring(%d, %d): [%s]\n", start, end, subStr.string());
    start = 8;
    subStr = str.Substring(start, end);
    printf(" > str.Substring(%d, %d): [%s]\n", start, end, subStr.string());
    start = 9;
    subStr = str.Substring(start, end);
    printf(" > str.Substring(%d, %d): [%s]\n", start, end, subStr.string());
    start = 12;
    subStr = str.Substring(start, end);
    printf(" > str.Substring(%d, %d): [%s]\n", start, end, subStr.string());
    start = 13;
    subStr = str.Substring(start, end);
    printf(" > str.Substring(%d, %d): [%s]\n", start, end, subStr.string());
    start = 14;
    subStr = str.Substring(start, end);
    printf(" > str.Substring(%d, %d): [%s]\n", start, end, subStr.string());
    start = 14;
    end = charCount - 1;
    subStr = str.Substring(start, end);
    printf(" > str.Substring(%d, %d): [%s]\n", start, end, subStr.string());
    start = 14;
    end = charCount - 2;
    subStr = str.Substring(start, end);
    printf(" > str.Substring(%d, %d): [%s]\n", start, end, subStr.string());
    end = charCount - 15;
    subStr = str.Substring(start, end);
    printf(" > str.Substring(%d, %d): [%s]\n", start, end, subStr.string());
    end = charCount - 16;
    subStr = str.Substring(start, end);
    printf(" > str.Substring(%d, %d): [%s]\n", start, end, subStr.string());
}

void testAppend()
{
    printf("\n===============testAppend===============\n");
    String className("SoftKeyboard.CSoftKeyboard");
    Int32 length = className.GetLength();
    String tmp(".CSoftKeyboard");
    String str1("SoftKeyboard");

    String str3 = String(className.string() + 12, length - 12);

    printf("[.CSoftKeyboard] is %d\n", strlen(".CSoftKeyboard"));
    printf("tmp: [%s], tmp length: %d\n", tmp.string(), tmp.GetLength());
    printf("str3: [%s], length - 12: %d, str3 length: %d\n", str3.string(), length - 12, str3.GetLength());

    printf("\n");
    String str2 = className.Substring(12, length);
    printf("str2: [%s], str2 length: %d\n", str2.string(), str2.GetLength());

    StringBuilder sb;
    sb += str1;
    sb += "/";

    // printf(" result is %s\n", sb.ToString().string());

    sb += str2;

    printf(" result is %s\n", sb.ToString().string());
}

void testParse()
{
    AutoPtr<ArrayOf<String> > hexInts = ArrayOf<String>::Alloc(3);
    hexInts->Set(0, String("188"));
    hexInts->Set(1, String("24"));
    hexInts->Set(2, String("0"));

    Int32 num = 0;
    for (Int32 i = 0; i< hexInts->GetLength(); ++i) {
        String hexString = (*hexInts)[i];
        ECode ec = StringUtils::ParseInt32(hexString, 16, &num);
        if (FAILED(ec)) {
            printf("Failed to parse hex string %s\n", hexString.string());
        }
        else {
            printf(" hexString: %s, value: %d\n", hexString.string(), num);
        }
    }

}


int main(int argc, char *argv[])
{
    printf("- - - - - - - \n");

    // testString();
    // testSplit();
    // testReplaceFirst();
    // testReplaceAll();
    // testReplace();
    // testTrim();
    // testSubstring();
    // testStringBuilder();
    // testAppend();

    testParse();

    printf("- - - - - - - \n");
    return 0;
}
