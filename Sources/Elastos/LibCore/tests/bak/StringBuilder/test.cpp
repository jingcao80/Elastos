#include <elastos.h>
#include <eltypes.h>
#include <stdio.h>
#include <elastos/StringBuilder.h>
//#include <elastos/IntegralToString.h>

using namespace Elastos;
using namespace Elastos::Core;

void testStringBuilder()
{
    printf("==== Enter testStringBuilder ====\n");

    printf("\n === Construct === \n");

    StringBuilder sbc(String("Construct from String"));
    String str = sbc.ToString();
    printf(" > Construct:\n%s\n", (const char*)str);

    printf("\n === Append === \n");

    Char32 a = 'A';
    StringBuilder sb;
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

    sb.ToString(&str);
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
    Int32 start = 30, end = 32, length = 0;
    sb.GetLength(&length);
    printf(" === total length is %d\n", length);
    sb.Substring(start, &subStr);
    printf(" > Substring from %d is : %s\n", start, (const char*)subStr);
    sb.SubstringEx(start, end, &subStr);
    printf(" > Substring from %d to %d is : %s\n", start, end, (const char*)subStr);
    start = 0;
    sb.Substring(start, &subStr);
    printf(" > Substring from %d is : %s\n", start, (const char*)subStr);
    start = 0, end = length;
    sb.SubstringEx(start, end, &subStr);
    printf(" > Substring from %d to %d is : %s\n", start, end, (const char*)subStr);
    start = 0, end = length + 2;
    sb.SubstringEx(start, end, &subStr);
    printf(" > Substring from %d to %d is : %s\n", start, end, (const char*)subStr);
    start = 2, end = length - 2;
    sb.SubstringEx(start, end, &subStr);
    printf(" > Substring from %d to %d is : %s\n", start, end, (const char*)subStr);
    start = 2, end = length + 2;
    sb.SubstringEx(start, end, &subStr);
    printf(" > Substring from %d to %d is : %s\n", start, end, (const char*)subStr);

//    printf("\n === Get ===\n");
//    Char32 ch = 0;
//    sb.GetChar(start, &ch);
//    printf(" > GetChar at %d is : %c\n", start, ch);
//    sb.GetLength(&end);
//    printf(" > GetLength is : %d\n", end);
//    sb.GetByteCount(&end);
//    printf(" > GetByteCount is : %d\n", end);
//    sb.GetCapacity(&end);
//    printf(" > GetCapacity is : %d\n", end);
//
//    printf("\n === Set/Replace/Insert ===\n");
//    sb.SetChar(13, 'B');
//    sb.ToString(&str);
//    printf(" > SetCharAt:\n%s\n", (const char*)str);
//    sb.Replace(15, 15 + 4, String("Replace"));
//    sb.ToString(&str);
//    printf(" > Replace:\n%s\n", (const char*)str);
//    sb.InsertString(15, String("Insert_"));
//    sb.ToString(&str);
//    printf(" > InsertString:\n%s\n", (const char*)str);
//    sb.InsertString(0, String("HeadInsert_"));
//    sb.ToString(&str);
//    printf(" > InsertString in head:\n%s\n", (const char*)str);
//    sb.InsertChar(19, '_');
//    sb.ToString(&str);
//    printf(" > InsertChar:\n%s\n", (const char*)str);
//    sb.InsertBoolean(19, TRUE);
//    sb.ToString(&str);
//    printf(" > InsertBoolean:\n%s\n", (const char*)str);
//    sb.InsertInt32(19, 32);
//    sb.ToString(&str);
//    printf(" > InsertInt32:\n%s\n", (const char*)str);
//    sb.InsertInt64(19, 64);
//    sb.ToString(&str);
//    printf(" > InsertInt64:\n%s\n", (const char*)str);
//    sb.InsertFloat(19, 128.0);
//    sb.ToString(&str);
//    printf(" > InsertFloat:\n%s\n", (const char*)str);
//    sb.InsertDouble(19, 10000.00001);
//    sb.ToString(&str);
//    printf(" > InsertDouble:\n%s\n", (const char*)str);
//
//    ArrayOf<Char32>* chars = ArrayOf<Char32>::Alloc(10);
//    for (Int32 i = 0; i < chars->GetLength(); ++i) {
//        (*chars)[i] = 'A' + i;
//    }
//    sb.InsertChars(19, *chars);
//    sb.ToString(&str);
//    printf(" > InsertChars:\n%s\n", (const char*)str);
//    sb.InsertCharsEx(19, *chars, 5, 5);
//    sb.ToString(&str);
//    printf(" > InsertCharsEx:\n%s\n", (const char*)str);
//
//    printf("\n === Delete ===\n");
//    sb.Delete(19, 24);
//    sb.ToString(&str);
//    printf(" > Delete:\n%s\n", (const char*)str);
//    sb.DeleteChar(1);
//    sb.ToString(&str);
//    printf(" > DeleteChar:\n%s\n", (const char*)str);
//
//    printf("\n === Reverse ===\n");
//    sb.Reverse();
//    sb.ToString(&str);
//    printf(" > Reverse:\n%s\n", (const char*)str);


    printf("==== Leave testStringBuilder ====\n");
}

void doTest()
{
    testStringBuilder();
}

int main(int argc, char *argv[]) {
    printf("==== call doTest ====\n");
    doTest();
    printf("\n==== call doTest end ====\n");

    return 0;
}
