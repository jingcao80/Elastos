
#include <stdio.h>
#include <elastos.h>
#include <elautoptr.h>
#include <elastos/Math.h>

#ifndef DEBUG
#define DEBUG
#endif

#define private public
#include <elastos/StringBuffer.h>
#undef private

using namespace Elastos;
using namespace Elastos::Core;

void testStringBuffer()
{
    printf("==== Enter testStringBuffer ====\n");

    printf("\n === Construct === \n");

    StringBuffer sbc(String("Construct from String"));
    String str = sbc.ToString();
    printf(" > Construct:\n%s\n", (const char*)str);

    printf("\n === Append === \n");

    Char32 a = 'A';
    StringBuffer sb;
    ECode ec = sb.ToString(&str);
    printf(">> Get string from emtpy StringBuffer %s, %08x - %08x\n", str.string(), NOERROR, ec);

    sb.AppendCStr(">>Start\n");
    sb.AppendNULL();
    sb.AppendChar('_');
    sb.AppendChar(a);
    sb.AppendChar('_');
    sb.AppendBoolean(TRUE);
    sb.AppendChar('_');
    sb.AppendBoolean(FALSE);
    sb.AppendChar('_');
    sb.AppendInt32(32);
    sb.AppendChar('_');
    sb.AppendInt32(-32);
    sb.AppendChar('_');
    sb.AppendInt64(64);
    sb.AppendChar('_');
    sb.AppendInt64(-64);
    sb.AppendChar('_');
    sb.AppendFloat(10.0f);
    sb.AppendChar('_');
    sb.AppendDouble(101010.1010);
    sb.AppendChar('_');
    sb.AppendString(String("String"));
    sb.AppendCStr("\n<<End");

    str = sb.ToString();
    printf(" > AppendTest:\n%s\n", (const char*)str);


    printf("\n === Index ===\n");
    Int32 index = 0;
    String subStr("32");
    sb.IndexOf(subStr, &index);
    printf(" > IndexOf %s is %d\n", (const char*)subStr, index);

    subStr = String("_NOT_");
    sb.IndexOf(subStr, &index);
    printf(" > IndexOf %s is %d\n", (const char*)subStr, index);

    subStr = String("32");
    sb.LastIndexOf(subStr, &index);
    printf(" > LastIndexOf %s is %d\n", (const char*)subStr, index);

    subStr = String("_NOT_");
    sb.LastIndexOf(subStr, &index);
    printf(" > LastIndexOf %s is %d\n", (const char*)subStr, index);


    printf("\n === Substring ===\n");
    Int32 start = 30, end = 32;
    sb.Substring(start, &subStr);
    printf(" > Substring from %d is : %s\n", start, (const char*)subStr);
    sb.SubstringEx(start, end, &subStr);
    printf(" > Substring from %d to %d is : %s\n", start, end, (const char*)subStr);

    printf("\n === Get ===\n");
    Char32 ch = 0;
    sb.GetChar(start, &ch);
    printf(" > GetChar at %d is : %c\n", start, ch);
    sb.GetLength(&end);
    printf(" > GetLength is : %d\n", end);
    sb.GetByteCount(&end);
    printf(" > GetByteCount is : %d\n", end);
    sb.GetCapacity(&end);
    printf(" > GetCapacity is : %d\n", end);

    printf("\n === Set/Replace/Insert ===\n");
    sb.SetChar(13, 'B');
    sb.ToString(&str);
    printf(" > SetCharAt:\n%s\n", (const char*)str);
    sb.Replace(15, 15 + 4, String("Replace"));
    sb.ToString(&str);
    printf(" > Replace:\n%s\n", (const char*)str);
    sb.InsertString(15, String("Insert_"));
    sb.ToString(&str);
    printf(" > InsertString:\n%s\n", (const char*)str);
    sb.InsertString(0, String("HeadInsert_"));
    sb.ToString(&str);
    printf(" > InsertString in head:\n%s\n", (const char*)str);
    sb.InsertChar(19, '_');
    sb.ToString(&str);
    printf(" > InsertChar:\n%s\n", (const char*)str);
    sb.InsertBoolean(19, TRUE);
    sb.ToString(&str);
    printf(" > InsertBoolean:\n%s\n", (const char*)str);
    sb.InsertInt32(19, 32);
    sb.ToString(&str);
    printf(" > InsertInt32:\n%s\n", (const char*)str);
    sb.InsertInt64(19, 64);
    sb.ToString(&str);
    printf(" > InsertInt64:\n%s\n", (const char*)str);
    sb.InsertFloat(19, 128.0);
    sb.ToString(&str);
    printf(" > InsertFloat:\n%s\n", (const char*)str);
    sb.InsertDouble(19, 10000.00001);
    sb.ToString(&str);
    printf(" > InsertDouble:\n%s\n", (const char*)str);

    ArrayOf<Char32>* chars = ArrayOf<Char32>::Alloc(10);
    for (Int32 i = 0; i < chars->GetLength(); ++i) {
        (*chars)[i] = 'A' + i;
    }
    sb.InsertChars(19, *chars);
    sb.ToString(&str);
    printf(" > InsertChars:\n%s\n", (const char*)str);
    sb.InsertCharsEx(19, *chars, 5, 5);
    sb.ToString(&str);
    printf(" > InsertCharsEx:\n%s\n", (const char*)str);

    printf("\n === Delete ===\n");
    sb.Delete(19, 24);
    sb.ToString(&str);
    printf(" > Delete:\n%s\n", (const char*)str);
    sb.DeleteChar(1);
    sb.ToString(&str);
    printf(" > DeleteChar:\n%s\n", (const char*)str);

    printf("\n === Reverse ===\n");
    sb.Reverse();
    sb.ToString(&str);
    printf(" > Reverse:\n%s\n", (const char*)str);

    printf("==== Leave testStringBuffer ====\n");
}

void testAutoPtr()
{
    printf("\n==== Leave testAutoPtr ====\n");

    {
        AutoPtr<StringBuffer> sb = new StringBuffer(10);
        sb = NULL;
    }

    printf("==== Leave testAutoPtr ====\n");
}

void testMisc()
{
    printf("\n==== Leave testAutoPtr ====\n");
    {
        StringBuffer sb(20);
        String str;
        for (Int32 i = 0; i < 20; i++) {
            sb.AppendChar('A' + i);
        }

        str = sb.ToString();
        printf(" >> Capacity %d, length %d, str: %s\n", sb.mCapacity, sb.GetLength(), str.string());

        sb.TrimToSize();
        printf(" >> Capacity %d, length %d, str: %s\n", sb.mCapacity, sb.GetLength(), str.string());
    }

    {
        StringBuffer sb;
        String str;
        for (Int32 i = 0; i < 20; i++) {
            sb.AppendChar('A' + i);
        }

        sb.InsertString(0, String("0123456789"));
        str = sb.ToString();
        printf("\n >> InsertString Capacity %d, length %d, str: %s\n", sb.mCapacity, sb.GetLength(), str.string());
    }

    {
        StringBuffer sb;
        String str;
        sb.AppendNULL();
        str = sb.ToString();
        printf("\n >> AppendNULL Capacity %d, length %d, str: %s\n", sb.mCapacity, sb.GetLength(), str.string());
    }

    {
        StringBuffer sb(5);
        String str;
        for (Int32 i = 0; i < 5; ++i) {
            sb.AppendChar('1' + i);
        }
        str = sb.ToString();
        printf("\n >> BeforeReplace Capacity %d, length %d, str: %s\n", sb.mCapacity, sb.GetLength(), str.string());

        sb.Replace(0, 5, String("ABCDE"));
        str = sb.ToString();
        printf(" >> AfterReplace Capacity %d, length %d, str: %s\n", sb.mCapacity, sb.GetLength(), str.string());

        sb.Replace(0, 4, String("444"));
        str = sb.ToString();
        printf(" >> AfterReplace Capacity %d, length %d, str: %s\n", sb.mCapacity, sb.GetLength(), str.string());

        sb.Replace(0, 3, String("UVWXYZ"));
        str = sb.ToString();
        printf(" >> AfterReplace Capacity %d, length %d, str: %s\n", sb.mCapacity, sb.GetLength(), str.string());

        sb.Replace(0, 7, String("1234567"));
        str = sb.ToString();
        printf(" >> AfterReplace Capacity %d, length %d, str: %s\n", sb.mCapacity, sb.GetLength(), str.string());

        sb.Replace(0, 0, String("ABC"));
        str = sb.ToString();
        printf(" >> AfterReplace Capacity %d, length %d, str: %s\n", sb.mCapacity, sb.GetLength(), str.string());

        sb.Replace(0, 10, String("0123456789"));
        str = sb.ToString();
        printf(" >> AfterReplace Capacity %d, length %d, str: %s\n", sb.mCapacity, sb.GetLength(), str.string());

        sb.Replace(10, 10, String("ABC"));
        str = sb.ToString();
        printf(" >> AfterReplace Capacity %d, length %d, str: %s\n", sb.mCapacity, sb.GetLength(), str.string());

        sb.Replace(14, 18, String("XYZ"));
        str = sb.ToString();
        printf(" >> AfterReplace Capacity %d, length %d, str: %s\n", sb.mCapacity, sb.GetLength(), str.string());
        sb.Replace(12, 13, String("0"));
        str = sb.ToString();
        printf(" >> AfterReplace Capacity %d, length %d, str: %s\n", sb.mCapacity, sb.GetLength(), str.string());
    }

    printf("==== Leave testAutoPtr ====\n");
}

void testAppendDoubleFloat()
{
    printf("\n==== Enter testAppendDoubleFloat ====\n");
    {
        Double d = Math::DOUBLE_MAX_VALUE;
        StringBuffer sb;
        String str;
//        sb.AppendDouble(d);
//        str = sb.ToString();
//        printf(" >> Append Double %e, %s\n", d, str.string());
//
//        d = Math::DOUBLE_MIN_VALUE;
//        sb.Reset();
//        sb.AppendDouble(d);
//        str = sb.ToString();
//        printf(" >> Append Double %e, %s\n", d, str.string());
//
//        d = 9999912345.54321;
//        sb.Reset();
//        sb.AppendDouble(d);
//        str = sb.ToString();
//        printf(" >> Append Double %e, %s\n", d, str.string());

        d = 2013.0806;
        sb.Reset();
        sb.AppendDouble(d);
        str = sb.ToString();
        printf(" >> Append Double %e, %s\n", d, str.string());

        float f = 2013.0806;
        sb.Reset();
        sb.AppendFloat(f);
        str = sb.ToString();
        printf(" >> Append Float %e, %s\n", f, str.string());

//        Float f = Math::FLOAT_MAX_VALUE;
//        sb.Reset();
//        sb.AppendFloat(f);
//        str = sb.ToString();
//        printf(" >> Append Float %e, %s\n", f, str.string());
//
//        f = Math::FLOAT_MIN_VALUE;
//        sb.Reset();
//        sb.AppendFloat(f);
//        str = sb.ToString();
//        printf(" >> Append Float %e, %s\n", f, str.string());
//
//        f = 12345.54321F;
//        sb.Reset();
//        sb.AppendFloat(f);
//        str = sb.ToString();
//        printf(" >> Append Float %f, %s\n", f, str.string());
    }

    printf("==== Leave testAppendDoubleFloat ====\n");
}

void testChineseImpl(StringBuffer& sb)
{
    printf("\n");
    sb.DbgPrint();
    Int32 length = sb.GetLength();
    String str;
    for (Int32 i = -1; i <= length + 2; i++) {
        sb.Substring(i, &str);
        printf(" >> Char from index %d is %s\n", i, str.string());
    }
}

void testChinese()
{
    printf("\n==== Enter testChinese ====\n");
    String testStr1("012ABCABC");
    String testStr2("\u2D4E\u8765\u8765ABC");//“中文helloworld”

    StringBuffer sb1(testStr1);
    StringBuffer sb2(testStr2);

    testChineseImpl(sb1);

    // test sb1
    //
    Int32 index = 0;
    Int32 start = 0;
    String subStr("ABC");

    sb1.LastIndexOf(subStr, &index);
    printf("\n >> Last Index of %s is %d\n", subStr.string(), index);

    subStr = String("012ABC");
    sb1.LastIndexOf(subStr, &index);
    printf(" >> Last Index of %s is %d\n", subStr.string(), index);

    subStr = String("0");
    sb1.LastIndexOf(subStr, &index);
    printf(" >> Last Index of %s is %d\n", subStr.string(), index);

    subStr = String("C");
    sb1.LastIndexOf(subStr, &index);
    printf(" >> Last Index of %s is %d\n", subStr.string(), index);

    subStr = String("AB");
    sb1.LastIndexOf(subStr, &index);
    printf(" >> Last Index of %s is %d\n", subStr.string(), index);

    subStr = String("A");
    start = 10;
    sb1.LastIndexOfEx(subStr, start, &index);
    printf(" >> Last Index of %s, start %d is %d\n", subStr.string(), start, index);

    subStr = String("A");
    start = 9;
    sb1.LastIndexOfEx(subStr, start, &index);
    printf(" >> Last Index of %s, start %d is %d\n", subStr.string(), start, index);

    subStr = String("A");
    start = 6;
    sb1.LastIndexOfEx(subStr, start, &index);
    printf(" >> Last Index of %s, start %d is %d\n", subStr.string(), start, index);

    subStr = String("A");
    start = 4;
    sb1.LastIndexOfEx(subStr, start, &index);
    printf(" >> Last Index of %s, start %d is %d\n", subStr.string(), start, index);

    subStr = String("A");
    start = 2;
    sb1.LastIndexOfEx(subStr, start, &index);
    printf(" >> Last Index of %s, start %d is %d\n", subStr.string(), start, index);

    subStr = String("A");
    start = -1;
    sb1.LastIndexOfEx(subStr, start, &index);
    printf(" >> Last Index of %s, start %d is %d\n\n", subStr.string(), start, index);

    sb1.IndexOf(subStr, &index);
    printf(" >> Index of %s is %d\n", subStr.string(), index);

    subStr = String("012ABC");
    sb1.IndexOf(subStr, &index);
    printf(" >> Index of %s is %d\n", subStr.string(), index);

    subStr = String("0");
    sb1.IndexOf(subStr, &index);
    printf(" >> Index of %s is %d\n", subStr.string(), index);

    subStr = String("C");
    sb1.IndexOf(subStr, &index);
    printf(" >> Index of %s is %d\n", subStr.string(), index);

    subStr = String("A");
    sb1.IndexOf(subStr, &index);
    printf(" >> Index of %s is %d\n", subStr.string(), index);

    subStr = String("A");
    start = 0;
    sb1.IndexOfEx(subStr, start, &index);
    printf(" >> Index of %s, start %d is %d\n", subStr.string(), start, index);

    subStr = String("A");
    start = 1;
    sb1.IndexOfEx(subStr, start, &index);
    printf(" >> Index of %s, start %d is %d\n", subStr.string(), start, index);

    subStr = String("A");
    start = 3;
    sb1.IndexOfEx(subStr, start, &index);
    printf(" >> Index of %s, start %d is %d\n", subStr.string(), start, index);

    subStr = String("A");
    start = 4;
    sb1.IndexOfEx(subStr, start, &index);
    printf(" >> Index of %s, start %d is %d\n\n", subStr.string(), start, index);


    // test sb2
    //

    testChineseImpl(sb2);

    subStr = String("ABC");
    sb2.LastIndexOf(subStr, &index);
    printf("\n >> Last Index of %s is %d\n", subStr.string(), index);

    subStr = String("\u2D4E");
    sb2.LastIndexOf(subStr, &index);
    printf(" >> Last Index of %s is %d\n", subStr.string(), index);

    subStr = String("\u8765");
    sb2.LastIndexOf(subStr, &index);
    printf(" >> Last Index of %s is %d\n", subStr.string(), index);

    subStr = String("B");
    sb2.LastIndexOf(subStr, &index);
    printf(" >> Last Index of %s is %d\n", subStr.string(), index);

    subStr = String("C");
    sb2.LastIndexOf(subStr, &index);
    printf(" >> Last Index of %s is %d\n", subStr.string(), index);

    subStr = String("\u2D4E\u8765");
    sb2.LastIndexOf(subStr, &index);
    printf(" >> Last Index of %s is %d\n", subStr.string(), index);

    subStr = String("\u2D4E\u8765\u8765ABC");
    sb2.LastIndexOf(subStr, &index);
    printf(" >> Last Index of %s is %d\n", subStr.string(), index);

    subStr = String("D");
    sb2.LastIndexOf(subStr, &index);
    printf(" >> Last Index of %s is %d\n", subStr.string(), index);

    subStr = String("\u8765A");
    start = 4;
    sb2.LastIndexOfEx(subStr, start, &index);
    printf(" >> Last Index of %s, start %d is %d\n", subStr.string(), start, index);

    subStr = String("\u8765A");
    start = 3;
    sb2.LastIndexOfEx(subStr, start, &index);
    printf(" >> Last Index of %s, start %d is %d\n", subStr.string(), start, index);

    subStr = String("\u8765A");
    start = 2;
    sb2.LastIndexOfEx(subStr, start, &index);
    printf(" >> Last Index of %s, start %d is %d\n", subStr.string(), start, index);

    subStr = String("\u8765B");
    start = 3;
    sb2.LastIndexOfEx(subStr, start, &index);
    printf(" >> Last Index of %s, start %d is %d\n", subStr.string(), start, index);

    subStr = String("\u8765");
    start = 3;
    sb2.LastIndexOfEx(subStr, start, &index);
    printf(" >> Last Index of %s, start %d is %d\n", subStr.string(), start, index);

    subStr = String("\u8765");
    start = 2;
    sb2.LastIndexOfEx(subStr, start, &index);
    printf(" >> Last Index of %s, start %d is %d\n", subStr.string(), start, index);

    subStr = String("\u8765");
    start = 1;
    sb2.LastIndexOfEx(subStr, start, &index);
    printf(" >> Last Index of %s, start %d is %d\n", subStr.string(), start, index);

    subStr = String("\u2D4E");
    start = 0;
    sb2.LastIndexOfEx(subStr, start, &index);
    printf(" >> Last Index of %s, start %d is %d\n", subStr.string(), start, index);

    subStr = String("\u2D4E");
    start = -1;
    sb2.LastIndexOfEx(subStr, start, &index);
    printf(" >> Last Index of %s, start %d is %d\n\n", subStr.string(), start, index);

    subStr = String("ABC");
    sb2.IndexOf(subStr, &index);
    printf(" >> Index of %s is %d\n", subStr.string(), index);

    subStr = String("\u2D4E");
    sb2.IndexOf(subStr, &index);
    printf(" >> Index of %s is %d\n", subStr.string(), index);

    subStr = String("\u8765");
    sb2.IndexOf(subStr, &index);
    printf(" >> Index of %s is %d\n", subStr.string(), index);

    subStr = String("B");
    sb2.IndexOf(subStr, &index);
    printf(" >> Index of %s is %d\n", subStr.string(), index);

    subStr = String("C");
    sb2.IndexOf(subStr, &index);
    printf(" >> Index of %s is %d\n", subStr.string(), index);

    subStr = String("\u2D4E\u8765");
    sb2.IndexOf(subStr, &index);
    printf(" >> Index of %s is %d\n", subStr.string(), index);

    subStr = String("\u2D4E\u8765\u8765ABC");
    sb2.IndexOf(subStr, &index);
    printf(" >> Index of %s is %d\n", subStr.string(), index);

    subStr = String("D");
    sb2.IndexOf(subStr, &index);
    printf(" >> Index of %s is %d\n", subStr.string(), index);

    subStr = String("\u8765A");
    start = 0;
    sb2.IndexOfEx(subStr, start, &index);
    printf(" >> Index of %s, start %d is %d\n", subStr.string(), start, index);

    subStr = String("\u8765A");
    start = 2;
    sb2.IndexOfEx(subStr, start, &index);
    printf(" >> Index of %s, start %d is %d\n", subStr.string(), start, index);

    subStr = String("\u8765A");
    start = 3;
    sb2.IndexOfEx(subStr, start, &index);
    printf(" >> Index of %s, start %d is %d\n", subStr.string(), start, index);

    subStr = String("\u8765B");
    start = 1;
    sb2.IndexOfEx(subStr, start, &index);
    printf(" >> Index of %s, start %d is %d\n", subStr.string(), start, index);

    printf("==== Enter testChinese ====\n");
}

void doTest()
{
    testStringBuffer();
    testAutoPtr();
    testMisc();
    testAppendDoubleFloat();
    testChinese();
}

int main()
{
    printf("==== call doTest ====\n");
    doTest();
    printf("\n==== call doTest end ====\n");

	return 0;
}

