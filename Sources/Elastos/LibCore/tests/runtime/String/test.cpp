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

#include <elastos/core/Object.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <Elastos.CoreLibrary.Utility.h>

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

//====================================================================================================================
//                  Assert Assistant
//====================================================================================================================
void assertEquals(Int32 aspect, Int32 test)
{
    printf("Equals: %d, test: %d\n", aspect, test);
    assert(aspect == test);
}

void assertNotEquals(Int32 aspect, Int32 test)
{
    printf("Not Equals: %d, test: %d\n", aspect, test);
    assert(aspect != test);
}

void assertEquals(const String& aspect, const String& test)
{
    printf("Equals: [%s], test: [%s]\n", aspect.string(), test.string());
    assert(aspect.Equals(test));
}

void assertEquals(Char32 aspect, Char32 test)
{
    printf("Equals: [%d],[%08x], test: [%d],[%08x]\n", aspect, aspect, test, test);
    assert(aspect == test);
}

void assertNotEquals(const String& aspect, const String& test)
{
    printf("Not Equals: [%s], test: [%s]\n", aspect.string(), test.string());
    assert(!aspect.Equals(test));
}

void assertNotEquals(Char32 aspect, Char32 test)
{
    printf("Not Equals: [%d],[%08x], test: [%d],[%08x]\n", aspect, aspect, test, test);
    assert(aspect != test);
}

void assertContains(const String& aspect, const String& test, Boolean isSensitive = TRUE)
{
    printf("%s, Contains: [%s], test: [%s]\n", (isSensitive ? "Sensitive" : "Insensitive"),
        aspect.string(), test.string());
    assert(isSensitive ? test.Contains(aspect) : test.ContainsIgnoreCase(aspect));
}

void assertContains(const char* aspect, const String& test, Boolean isSensitive = TRUE)
{
    printf("%s, Contains: [%s], test: [%s]\n", (isSensitive ? "Sensitive" : "Insensitive"),
        aspect, test.string());
    assert(isSensitive ? test.Contains(aspect) : test.ContainsIgnoreCase(aspect));
}

void assertNotContains(const String& aspect, const String& test, Boolean isSensitive = TRUE)
{
    printf("%s, Not Contains: [%s], test: [%s]\n", (isSensitive ? "Sensitive" : "Insensitive"),
        aspect.string(), test.string());
    assert(isSensitive ? !test.Contains(aspect) : !test.ContainsIgnoreCase(aspect));
}

void assertNotContains(const char* aspect, const String& test, Boolean isSensitive = TRUE)
{
    printf("%s, Not Contains: [%s], test: [%s]\n", (isSensitive ? "Sensitive" : "Insensitive"),
        aspect, test.string());
    assert(isSensitive ? !test.Contains(aspect) : !test.ContainsIgnoreCase(aspect));
}

void assertStartWith(const String& aspect, const String& test, Boolean isSensitive = TRUE)
{
    printf("%s, StartWith: [%s], test: [%s]\n", (isSensitive ? "Sensitive" : "Insensitive"),
        aspect.string(), test.string());
    assert(isSensitive ? test.StartWith(aspect) : test.StartWithIgnoreCase(aspect));
}

void assertStartWith(const char* aspect, const String& test, Boolean isSensitive = TRUE)
{
    printf("%s, StartWith: [%s], test: [%s]\n", (isSensitive ? "Sensitive" : "Insensitive"),
        aspect, test.string());
    assert(isSensitive ? test.StartWith(aspect) : test.StartWithIgnoreCase(aspect));
}

void assertNotStartWith(const String& aspect, const String& test, Boolean isSensitive = TRUE)
{
    printf("%s, Not StartWith: [%s], test: [%s]\n", (isSensitive ? "Sensitive" : "Insensitive"),
        aspect.string(), test.string());
    assert(isSensitive ? !test.StartWith(aspect) : !test.StartWithIgnoreCase(aspect));
}

void assertNotStartWith(const char* aspect, const String& test, Boolean isSensitive = TRUE)
{
    printf("%s, Not StartWith: [%s], test: [%s]\n", (isSensitive ? "Sensitive" : "Insensitive"),
        aspect, test.string());
    assert(isSensitive ? !test.StartWith(aspect) : !test.StartWithIgnoreCase(aspect));
}

void assertEndWith(const String& aspect, const String& test, Boolean isSensitive = TRUE)
{
    printf("%s, EndWith: [%s], test: [%s]\n", (isSensitive ? "Sensitive" : "Insensitive"),
        aspect.string(), test.string());
    assert(isSensitive ? test.EndWith(aspect) : test.EndWithIgnoreCase(aspect));
}

void assertEndWith(const char* aspect, const String& test, Boolean isSensitive = TRUE)
{
    printf("%s, EndWith: [%s], test: [%s]\n", (isSensitive ? "Sensitive" : "Insensitive"),
        aspect, test.string());
    assert(isSensitive ? test.EndWith(aspect) : test.EndWithIgnoreCase(aspect));
}

void assertNotEndWith(const String& aspect, const String& test, Boolean isSensitive = TRUE)
{
    printf("%s, Not EndWith: [%s], test: [%s]\n", (isSensitive ? "Sensitive" : "Insensitive"),
        aspect.string(), test.string());
    assert(isSensitive ? !test.EndWith(aspect) : !test.EndWithIgnoreCase(aspect));
}

void assertNotEndWith(const char* aspect, const String& test, Boolean isSensitive = TRUE)
{
    printf("%s, Not EndWith: [%s], test: [%s]\n", (isSensitive ? "Sensitive" : "Insensitive"),
        aspect, test.string());
    assert(isSensitive ? !test.EndWith(aspect) : !test.EndWithIgnoreCase(aspect));
}

//====================================================================================================================
//                  Testers
//====================================================================================================================

void testConstructor()
{
    printf("\n============== BEGIN testConstructor ==============\n");
    String str1;
    String str2("str2");
    String str3("000str3", 3);
    String str4(str3);

    printf("str1: [%s]\n", str1.string());
    printf("str2: [%s]\n", str2.string());
    printf("str3: [%s]\n", str3.string());
    printf("str4: [%s]\n", str4.string());

    String strList[8] = {
        String("A", 3),
        String("中"),
        String("中文字符"),
        String("A中"),
        String("中A"),
        String("A中符"),
        String("中A符"),
        String("中符A"),
    };

    for (Int32 i = 0; i < 8; i++) {
        printf("[%s], length:%d, byteCount:%d\n",
            strList[i].string(), strList[i].GetLength(), strList[i].GetByteLength());
    }

    printf("\n============== Constructor with array ==============\n");
    str1 = String("中文A0字符");
    AutoPtr<ArrayOf<Char32> > chars = str1.GetChars();
    str2 = String(*chars);
    assertEquals(str1, str2);

    str2 = String(*chars, 0);
    assertEquals(str1, str2);

    str2 = String(*chars, 0, chars->GetLength());
    assertEquals(str1, str2);

    str2 = String(*chars, 1, chars->GetLength());
    assertEquals(String("文A0字符"), str2);

    str2 = String(*chars, 0, chars->GetLength() - 1);
    assertEquals(String("中文A0字"), str2);

    str2 = String(*chars, 1, chars->GetLength() - 2);
    assertEquals(String("文A0字"), str2);

    str2 = String(*chars, chars->GetLength() - 1, 1);
    assertEquals(String("符"), str2);

    str2 = String(*chars, 0, 0);
    assert(str2.IsNullOrEmpty());

    str2 = String(*chars, chars->GetLength(), 0);
    assert(str2.IsNullOrEmpty());

    str2 = String(*chars, chars->GetLength(), 1);
    assert(str2.IsNullOrEmpty());

    printf("============== END testConstructor ==============\n");
}

void testSetTo()
{
    printf("\n============== BEGIN testSetTo ==============\n");

    String str1, str2;

    str1 = String(NULL);
    str2 = str1;
    assertEquals(str1, str2);

    str1 = String("");
    str2 = str1;
    assertEquals(str1, str2);

    str1 = String("A");
    str2 = str1;
    assertEquals(str1, str2);

    str1 = String("中文A0字符");
    str2 = str1;
    assertEquals(str1, str2);

    printf("\n============== END testSetTo ==============\n");
}

void testAppend()
{
    printf("\n============== BEGIN testAppend ==============\n");

    String str1, str2;

    str1 += "";
    assertNotEquals(String(""), str1);

    str1 += "中";
    assertEquals(String("中"), str1);

    str1 += "文A";
    assertEquals(String("中文A"), str1);

    str1 += "0字符";
    assertEquals(String("中文A0字符"), str1);

    str2 += String(NULL);
    assertEquals(String(NULL), str2);

    str2 += String("");
    assertEquals(String(""), str2);

    str2 += String("中");
    assertEquals(String("中"), str2);

    str2 += String("文A");
    assertEquals(String("中文A"), str2);

    str2 += String("0字符");
    assertEquals(String("中文A0字符"), str2);

    str1 = String("中文A0字符");
    Char32 ch = str1.GetChar(0);
    printf(" ========== %c, %d\n", ch, ch);
    str2.Append(ch);
    assertEquals(String("中文A0字符中"), str2);

    ch = str1.GetChar(2);
    printf(" ========== %c\n", ch);
    str2.Append(ch);
    assertEquals(String("中文A0字符中A"), str2);

    str2.Append('C');
    assertEquals(String("中文A0字符中AC"), str2);

    printf("\n============== Append with numOfChars ==============\n");

    str2 = String("");
    str2.Append(str1, 0);
    assertEquals(String("中文A0字符"), str2);

    str2 = String("");
    str2.Append(str1, 1);
    assertEquals(String("文A0字符"), str2);

    str2 = String("");
    str2.Append(str1, 5);
    assertEquals(String("符"), str2);

    str2 = String("");
    str2.Append(str1, 6);
    assert(str2.IsNullOrEmpty());

    str2 = String("");
    str2.Append(str1, 0, 6);
    assertEquals(String("中文A0字符"), str2);

    str2 = String("");
    str2.Append(str1, 1, 5);
    assertEquals(String("文A0字符"), str2);

    str2 = String("");
    str2.Append(str1, 1, 6);
    assertEquals(String("文A0字符"), str2);

    str2 = String("");
    str2.Append(str1, 0, 5);
    assertEquals(String("中文A0字"), str2);

    str2 = String("");
    str2.Append(str1, 0, 1);
    assertEquals(String("中"), str2);

    str2 = String("");
    str2.Append(str1, 5, 1);
    assertEquals(String("符"), str2);

    str2 = String("");
    str2.Append(str1, 1, 4);
    assertEquals(String("文A0字"), str2);

    printf("\n============== Constructor with array ==============\n");

    str1 = String("中文A0字符");
    AutoPtr<ArrayOf<Char32> > chars = str1.GetChars();
    str2 = String("");

    str2.Append(*chars, 0, chars->GetLength() - 1);
    assertEquals(String("中文A0字"), str2);

    printf("\n============== AppendFormat ==============\n");
    str1 = String("中文A0字符");
    str1.AppendFormat(", github: %s", "http://github.com/");
    str2 = String("中文A0字符, github: http://github.com/");
    assertEquals(str2, str1);

    printf("\n============== END testAppend ==============\n");
}

void testSubstring()
{
    printf("\n============== BEGIN testSubstring ==============\n");

    String str1("中文A0字符");
    String str2 = str1.Substring(0);
    assertEquals(String("中文A0字符"), str2);

    str2 = str1.Substring(1);
    assertEquals(String("文A0字符"), str2);
    str2 = str1.Substring(2);
    assertEquals(String("A0字符"), str2);
    str2 = str1.Substring(3);
    assertEquals(String("0字符"), str2);
    str2 = str1.Substring(5);
    assertEquals(String("符"), str2);
    str2 = str1.Substring(6);
    assertEquals(String(""), str2);

    str2 = str1.Substring(0, 6);
    assertEquals(String("中文A0字符"), str2);
    str2 = str1.Substring(1, 6);
    assertEquals(String("文A0字符"), str2);
    str2 = str1.Substring(2, 6);
    assertEquals(String("A0字符"), str2);
    str2 = str1.Substring(3, 6);
    assertEquals(String("0字符"), str2);
    str2 = str1.Substring(5, 6);
    assertEquals(String("符"), str2);
    str2 = str1.Substring(6, 6);
    assertEquals(String(""), str2);

    str2 = str1.Substring(0, 1);
    assertEquals(String("中"), str2);
    str2 = str1.Substring(1, 2);
    assertEquals(String("文"), str2);
    str2 = str1.Substring(0, 5);
    assertEquals(String("中文A0字"), str2);
    str2 = str1.Substring(2, 5);
    assertEquals(String("A0字"), str2);
    str2 = str1.Substring(5, 6);
    assertEquals(String("符"), str2);
    str2 = str1.Substring(5, 5);
    assertEquals(String(""), str2);

    str1 = String("");
    str2 = str1.Substring(0);
    assertEquals(String(""), str2);

    str2 = str1.Substring(1);
    assertEquals(String(""), str2);

    printf("\n============== END testSubstring ==============\n");
}

void testToLowerCase()
{
    printf("\n============== BEGIN testToLowerCase ==============\n");

    String str1, str2;

    str1 = String("");
    str2 = str1.ToLowerCase();
    assertEquals(String(""), str2);
    str2 = str1.ToLowerCase(0, 1);
    assertEquals(String(""), str2);
    str2 = str1.ToLowerCase(0, 0);
    assertEquals(String(""), str2);
    str2 = str1.ToLowerCase(1, 1);
    assertEquals(String(""), str2);

    str1 = String("1");
    str2 = str1.ToLowerCase();
    assertEquals(String("1"), str2);
    str2 = str1.ToLowerCase(0, 1);
    assertEquals(String("1"), str2);
    str2 = str1.ToLowerCase(0, 0);
    assertEquals(String("1"), str2);
    str2 = str1.ToLowerCase(1, 1);
    assertEquals(String("1"), str2);

    str1 = String("A");
    str2 = str1.ToLowerCase();
    assertEquals(String("a"), str2);
    str2 = str1.ToLowerCase(0, 1);
    assertEquals(String("a"), str2);
    str2 = str1.ToLowerCase(0, 0);
    assertEquals(String("A"), str2);
    str2 = str1.ToLowerCase(1, 1);
    assertEquals(String("A"), str2);
    str2 = str1.ToLowerCase(1, 1);
    assertEquals(String("A"), str2);

    str1 = String("Ab");
    str2 = str1.ToLowerCase();
    assertEquals(String("ab"), str2);
    str2 = str1.ToLowerCase(0, 1);
    assertEquals(String("ab"), str2);
    str2 = str1.ToLowerCase(0, 0);
    assertEquals(String("Ab"), str2);
    str2 = str1.ToLowerCase(1, 0);
    assertEquals(String("Ab"), str2);
    str2 = str1.ToLowerCase(1, 1);
    assertEquals(String("Ab"), str2);
    str2 = str1.ToLowerCase(1, 2);
    assertEquals(String("Ab"), str2);
    str2 = str1.ToLowerCase(2, 0);
    assertEquals(String("Ab"), str2);
    str2 = str1.ToLowerCase(2, 1);
    assertEquals(String("Ab"), str2);
    str2 = str1.ToLowerCase(0, 2);
    assertEquals(String("ab"), str2);
    str2 = str1.ToLowerCase(0, 3);
    assertEquals(String("ab"), str2);

    str1 = String("AB");
    str2 = str1.ToLowerCase();
    assertEquals(String("ab"), str2);
    str2 = str1.ToLowerCase(0, 1);
    assertEquals(String("aB"), str2);
    str2 = str1.ToLowerCase(0, 0);
    assertEquals(String("AB"), str2);
    str2 = str1.ToLowerCase(1, 1);
    assertEquals(String("Ab"), str2);
    str2 = str1.ToLowerCase(1, 2);
    assertEquals(String("Ab"), str2);
    str2 = str1.ToLowerCase(0, 2);
    assertEquals(String("ab"), str2);

    str1 = String("中");
    str2 = str1.ToLowerCase();
    assertEquals(String("中"), str2);
    str2 = str1.ToLowerCase(0, 1);
    assertEquals(String("中"), str2);
    str2 = str1.ToLowerCase(0, 0);
    assertEquals(String("中"), str2);
    str2 = str1.ToLowerCase(1, 1);
    assertEquals(String("中"), str2);

    str1 = String("中B");
    str2 = str1.ToLowerCase();
    assertEquals(String("中b"), str2);
    str2 = str1.ToLowerCase(0, 1);
    assertEquals(String("中B"), str2);
    str2 = str1.ToLowerCase(0, 0);
    assertEquals(String("中B"), str2);
    str2 = str1.ToLowerCase(1, 0);
    assertEquals(String("中B"), str2);
    str2 = str1.ToLowerCase(1, 1);
    assertEquals(String("中b"), str2);
    str2 = str1.ToLowerCase(1, 2);
    assertEquals(String("中b"), str2);
    str2 = str1.ToLowerCase(0, 2);
    assertEquals(String("中b"), str2);
    str2 = str1.ToLowerCase(0, 3);
    assertEquals(String("中b"), str2);

    printf("\n============== END testToLowerCase ==============\n");
}

void testGetChar()
{
    printf("\n============== BEGIN testGetChar ==============\n");
    String str16("中文A0字符");
    String str("中文A0字符");

    Char32 ch;
    for (UInt32 i = 0; i < str16.GetLength(); ++i) {
        ch = str16.GetChar(i);
        printf(" > %d: %08x\n", i, ch);
    }

    assertEquals(str.GetChar(0), str.GetChar(0));
    assertEquals(str.GetChar(1), str.GetChar(1));
    assertEquals(str.GetChar(2), str.GetChar(2));
    assertEquals(str.GetChar(3), str.GetChar(3));
    assertEquals(str.GetChar(4), str.GetChar(4));
    assertEquals(str.GetChar(5), str.GetChar(5));
    assertEquals(str.GetChar(6), str.GetChar(6));
    assertEquals(str.GetChar(7), str.GetChar(7));

    str16 = String("A中文字符0");
    str = String("A中文字符0");
    assertEquals(str.GetChar(0), str.GetChar(0));
    assertEquals(str.GetChar(1), str.GetChar(1));
    assertEquals(str.GetChar(2), str.GetChar(2));
    assertEquals(str.GetChar(3), str.GetChar(3));
    assertEquals(str.GetChar(4), str.GetChar(4));
    assertEquals(str.GetChar(5), str.GetChar(5));
    assertEquals(str.GetChar(6), str.GetChar(6));
    assertEquals(str.GetChar(7), str.GetChar(7));

    printf("\n============== END testGetChar ==============\n");
}

void testGetChars()
{
    printf("\n============== BEGIN testGetChars ==============\n");

    String str16("中文A0字符");
    String str("中文A0字符");
    AutoPtr<ArrayOf<Char32> > array = str16.GetChars();
    assert(array != NULL && array->GetLength() == str16.GetLength());
    printf(" > array size: %d\n", array->GetLength());
    for (UInt32 i = 0; i < array->GetLength(); i++) {
        assertEquals((*array)[i], str.GetChar(i));
    }

    array = str16.GetChars(1);
    assert(array != NULL && array->GetLength() == str16.GetLength() - 1);
    printf(" > array size: %d\n", array->GetLength());
    for (UInt32 i = 0; i < array->GetLength(); i++) {
        assertEquals((*array)[i], str.GetChar(i + 1));
    }

    array = str16.GetChars(2, 4);
    assert(array != NULL && array->GetLength() == 2);
    printf(" > array size: %d\n", array->GetLength());
    for (UInt32 i = 0; i < array->GetLength(); i++) {
        assertEquals((*array)[i], str.GetChar(i + 2));
    }

    array = str16.GetChars(2, 6);
    assert(array != NULL && array->GetLength() == str16.GetLength() - 2);
    printf(" > array size: %d\n", array->GetLength());
    for (UInt32 i = 0; i < array->GetLength(); i++) {
        assertEquals((*array)[i], str.GetChar(i + 2));
    }

    String nullStr;
    String emptyStr("");
    array = nullStr.GetChars();
    assert(array != NULL && array->GetLength() == 0);

    array = emptyStr.GetChars();
    assert(array != NULL && array->GetLength() == 0);

    String oneCharStr("A");
    array = oneCharStr.GetChars();
    assert(array->GetLength() == 1 && (*array)[0] == 'A');
    array = oneCharStr.GetChars(0, 1);
    assert(array->GetLength() == 1 && (*array)[0] == 'A');
    array = oneCharStr.GetChars(0, 0);
    assert(array->GetLength() == 0);
    array = oneCharStr.GetChars(1, 1);
    assert(array->GetLength() == 0);

    printf("\n============== END testGetChars ==============\n");
}

void testTrim()
{
    printf("\n============== BEGIN testTrim ==============\n");

    String str1("中 文 A 0 字 符");
    String str2("中 文 A 0 字 符");

    str1 = String("中 文 A 0 字 符");
    assertEquals(str1.Trim(), str2);
    assertEquals(str1.TrimStart(), str2);
    assertEquals(str1.TrimEnd(), str2);

    str1 = String(" 中 文 A 0 字 符");
    assertEquals(str1.Trim(), str2);
    assertEquals(str1.TrimStart(), str2);
    assertEquals(str1.TrimEnd(), String(" 中 文 A 0 字 符"));

    str1 = String("  中 文 A 0 字 符");
    assertEquals(str1.Trim(), str2);
    assertEquals(str1.TrimStart(), str2);
    assertEquals(str1.TrimEnd(), String("  中 文 A 0 字 符"));

    str1 = String("  中 文 A 0 字 符  ");
    assertEquals(str1.Trim(), str2);
    assertEquals(str1.TrimStart(), String("中 文 A 0 字 符  "));
    assertEquals(str1.TrimEnd(), String("  中 文 A 0 字 符"));

    str1 = String("");
    str2 = String("");;
    assertEquals(str1.Trim(), str2);
    assertEquals(str1.TrimStart(), str2);
    assertEquals(str1.TrimEnd(), str2);

    str1 = String(" 0 ");
    str2 = String("0");;
    assertEquals(str1.Trim(), str2);
    assertEquals(str1.TrimStart(), String("0 "));
    assertEquals(str1.TrimEnd(), String(" 0"));

    printf("\n============== END testTrim ==============\n");
}

void testIndexOfString()
{
    printf("\n============== BEGIN testIndexOfString ==============\n");
    Int32 index;
    String str("中文Aa0中文");

    index = str.IndexOf("", 0);
    assertEquals(0, index);

    index = str.IndexOf("中", 0);
    assertEquals(0, index);

    index = str.IndexOf("中文", 0);
    assertEquals(0, index);

    index = str.IndexOf("文", 0);
    assertEquals(1, index);

    index = str.IndexOf("中文", 1);
    assertEquals(5, index);

    index = str.IndexOf("文", 6);
    assertEquals(6, index);

    index = str.IndexOf("文A", 0);
    assertEquals(1, index);

    index = str.IndexOf("A", 0);
    assertEquals(2, index);

    index = str.IndexOf("Aa0", 0);
    assertEquals(2, index);

    index = str.IndexOf("Aa0中文", 0);
    assertEquals(2, index);

    index = str.IndexOf("中文Aa0中文", 0);
    assertEquals(0, index);

    index = str.IndexOf("中字", 0);
    assertEquals(-1, index);

    index = str.IndexOf("中A", 0);
    assertEquals(-1, index);

    index = str.IndexOf("中符", 0);
    assertEquals(-1, index);

    index = str.IndexOf("中文Aa0中文B", 0);
    assertEquals(-1, index);

    printf("============Insensitive==============\n");
    index = str.IndexOfIgnoreCase("", 0);
    assertEquals(0, index);

    index = str.IndexOfIgnoreCase("中", 0);
    assertEquals(0, index);

    index = str.IndexOfIgnoreCase("中文", 0);
    assertEquals(0, index);

    index = str.IndexOfIgnoreCase("文", 0);
    assertEquals(1, index);

    index = str.IndexOfIgnoreCase("中文", 1);
    assertEquals(5, index);

    index = str.IndexOfIgnoreCase("文", 6);
    assertEquals(6, index);

    index = str.IndexOfIgnoreCase("文A", 0);
    assertEquals(1, index);

    index = str.IndexOfIgnoreCase("文a", 0);
    assertEquals(1, index);

    index = str.IndexOfIgnoreCase("A", 0);
    assertEquals(2, index);

    index = str.IndexOfIgnoreCase("a", 0);
    assertEquals(2, index);

    index = str.IndexOfIgnoreCase("Aa0", 0);
    assertEquals(2, index);

    index = str.IndexOfIgnoreCase("aA0", 0);
    assertEquals(2, index);

    index = str.IndexOfIgnoreCase("Aa0中文", 0);
    assertEquals(2, index);

    index = str.IndexOfIgnoreCase("aA0中文", 0);
    assertEquals(2, index);

    index = str.IndexOfIgnoreCase("中文Aa0中文", 0);
    assertEquals(0, index);

    index = str.IndexOfIgnoreCase("中文AA0中文", 0);
    assertEquals(0, index);

    index = str.IndexOfIgnoreCase("中字", 0);
    assertEquals(-1, index);

    index = str.IndexOfIgnoreCase("中A", 0);
    assertEquals(-1, index);

    index = str.IndexOfIgnoreCase("中符", 0);
    assertEquals(-1, index);

    index = str.IndexOfIgnoreCase("中文Aa0中文B", 0);
    assertEquals(-1, index);

    printf("============IndexOf String============\n");
    index = str.IndexOf(String("中"), 0);
    assertEquals(0, index);

    index = str.IndexOf(String("中文"), 0);
    assertEquals(0, index);

    index = str.IndexOf(String("文"), 0);
    assertEquals(1, index);

    index = str.IndexOf(String("中文"), 1);
    assertEquals(5, index);

    index = str.IndexOf(String("文"), 6);
    assertEquals(6, index);

    index = str.IndexOf(String("文A"), 0);
    assertEquals(1, index);

    index = str.IndexOf(String("A"), 0);
    assertEquals(2, index);

    index = str.IndexOf(String("Aa0"), 0);
    assertEquals(2, index);

    index = str.IndexOf(String("Aa0中文"), 0);
    assertEquals(2, index);

    index = str.IndexOf(String("中文Aa0中文"), 0);
    assertEquals(0, index);

    index = str.IndexOf(String("中字"), 0);
    assertEquals(-1, index);

    index = str.IndexOf(String("中A"), 0);
    assertEquals(-1, index);

    index = str.IndexOf(String("中符"), 0);
    assertEquals(-1, index);

    index = str.IndexOf(String("中文Aa0中文B"), 0);
    assertEquals(-1, index);

    printf("\n============== END testIndexOfString ==============\n");
}

void testIndexOfChar()
{
    printf("\n============== BEGIN testIndexOfChar ==============\n");

    Char32 ch;
    Int32 index;
    String str16("中文Aa0字符");

    ch = str16.GetChar(0);
    index = str16.IndexOf(ch, 0);
    assertEquals(0, index);
    index = str16.IndexOfIgnoreCase(ch, 0);
    assertEquals(0, index);
    index = str16.IndexOfIgnoreCase(ch, 1);
    assertEquals(-1, index);

    ch = str16.GetChar(6);
    index = str16.IndexOf(ch, 0);
    assertEquals(6, index);
    index = str16.IndexOfIgnoreCase(ch, 6);
    assertEquals(6, index);
    index = str16.IndexOfIgnoreCase(ch, 7);
    assertEquals(-1, index);

    ch = str16.GetChar(4);
    index = str16.IndexOf(ch, 0);
    assertEquals(4, index);
    index = str16.IndexOfIgnoreCase(ch, 0);
    assertEquals(4, index);
    index = str16.IndexOfIgnoreCase(ch, 4);
    assertEquals(4, index);
    index = str16.IndexOfIgnoreCase(ch, 5);
    assertEquals(-1, index);

    ch = str16.GetChar(2);
    index = str16.IndexOf(ch, 0);
    assertEquals(2, index);
    index = str16.IndexOf(ch, 1);
    assertEquals(2, index);
    index = str16.IndexOf(ch, 2);
    assertEquals(2, index);
    index = str16.IndexOf(ch, 3);
    assertEquals(-1, index);
    index = str16.IndexOfIgnoreCase(ch, 0);
    assertEquals(2, index);
    index = str16.IndexOfIgnoreCase(ch, 1);
    assertEquals(2, index);
    index = str16.IndexOfIgnoreCase(ch, 2);
    assertEquals(2, index);
    index = str16.IndexOfIgnoreCase(ch, 3);
    assertEquals(3, index);
    index = str16.IndexOfIgnoreCase(ch, 4);
    assertEquals(-1, index);

    str16 = String("中文A0字符a");
    ch = str16.GetChar(6);
    index = str16.IndexOf(ch, 0);
    assertEquals(6, index);
    index = str16.IndexOfIgnoreCase(ch, 6);
    assertEquals(6, index);
    index = str16.IndexOfIgnoreCase(ch, 7);
    assertEquals(-1, index);

    printf("\n============== BEGIN testIndexOfChar ==============\n");
}

void testLastIndexOfChar()
{
    printf("\n============== BEGIN testLastIndexOfChar ==============\n");

    Char32 ch;
    Int32 index;
    String str16("中文A中文Aa字");
    AutoPtr<ArrayOf<Char32> > chars = str16.GetChars();

    ch = str16.GetChar(0);
    index = str16.LastIndexOf(ch, 0);
    assertEquals(0, index);
    index = str16.LastIndexOf(ch, 1);
    assertEquals(0, index);
    index = str16.LastIndexOf(ch, 2);
    assertEquals(0, index);
    index = str16.LastIndexOf(ch, 3);
    assertEquals(3, index);
    index = str16.LastIndexOf(ch, 4);
    assertEquals(3, index);
    index = str16.LastIndexOf(ch, 7);
    assertEquals(3, index);
    index = str16.LastIndexOf(ch, 8);
    assertEquals(3, index);
    index = str16.LastIndexOfIgnoreCase(ch, 0);
    assertEquals(0, index);
    index = str16.LastIndexOfIgnoreCase(ch, 1);
    assertEquals(0, index);
    index = str16.LastIndexOfIgnoreCase(ch, 2);
    assertEquals(0, index);
    index = str16.LastIndexOfIgnoreCase(ch, 3);
    assertEquals(3, index);
    index = str16.LastIndexOfIgnoreCase(ch, 4);
    assertEquals(3, index);
    index = str16.LastIndexOfIgnoreCase(ch, 7);
    assertEquals(3, index);
    index = str16.LastIndexOfIgnoreCase(ch, 8);
    assertEquals(3, index);

    //String str16("中文A中文Aa字");
    ch = 'A';
    index = str16.LastIndexOf(ch, 0);
    assertEquals(-1, index);
    index = str16.LastIndexOf(ch, 3);
    assertEquals(2, index);
    index = str16.LastIndexOf(ch, 6);
    assertEquals(5, index);
    index = str16.LastIndexOf(ch, 7);
    assertEquals(5, index);
    index = str16.LastIndexOfIgnoreCase(ch, 0);
    assertEquals(-1, index);
    index = str16.LastIndexOfIgnoreCase(ch, 3);
    assertEquals(2, index);
    index = str16.LastIndexOfIgnoreCase(ch, 6);
    assertEquals(6, index);
    index = str16.LastIndexOfIgnoreCase(ch, 7);
    assertEquals(6, index);

    //String str16("中文A中文Aa字");
    ch = str16.GetChar(7); // "字"
    index = str16.LastIndexOf(ch, 0);
    assertEquals(-1, index);
    index = str16.LastIndexOf(ch, 6);
    assertEquals(-1, index);
    index = str16.LastIndexOf(ch, 7);
    assertEquals(7, index);
    index = str16.LastIndexOf(ch, 8);
    assertEquals(7, index);
    index = str16.LastIndexOfIgnoreCase(ch, 0);
    assertEquals(-1, index);
    index = str16.LastIndexOfIgnoreCase(ch, 6);
    assertEquals(-1, index);
    index = str16.LastIndexOfIgnoreCase(ch, 7);
    assertEquals(7, index);
    index = str16.LastIndexOfIgnoreCase(ch, 8);
    assertEquals(7, index);

    str16 = String("中文A中文A字a");
    ch = 'a';
    index = str16.LastIndexOf(ch, 0);
    assertEquals(-1, index);
    index = str16.LastIndexOf(ch, 7);
    assertEquals(7, index);
    index = str16.LastIndexOf(ch, 8);
    assertEquals(7, index);
    index = str16.LastIndexOfIgnoreCase(ch, 0);
    assertEquals(-1, index);
    index = str16.LastIndexOfIgnoreCase(ch, 1);
    assertEquals(-1, index);
    index = str16.LastIndexOfIgnoreCase(ch, 2);
    assertEquals(2, index);
    index = str16.LastIndexOfIgnoreCase(ch, 5);
    assertEquals(5, index);
    index = str16.LastIndexOfIgnoreCase(ch, 7);
    assertEquals(7, index);
    index = str16.LastIndexOfIgnoreCase(ch, 8);
    assertEquals(7, index);

    printf("\n============== END testLastIndexOfChar ==============\n");
}

void testLastIndexOfString()
{
    printf("\n============== BEGIN testLastIndexOfString ==============\n");
    Int32 index;
    String str("中文AaA中文");

    index = str.LastIndexOf("");
    assertEquals(7, index);

    index = str.LastIndexOf("中", 0);
    assertEquals(0, index);

    index = str.LastIndexOf("中文", 0);
    assertEquals(0, index);

    index = str.LastIndexOf("文", 0);
    assertEquals(-1, index);

    index = str.LastIndexOf("文", 1);
    assertEquals(1, index);

    index = str.LastIndexOf("文", 2);
    assertEquals(1, index);

    index = str.LastIndexOf("中文", 7);
    assertEquals(5, index);

    index = str.LastIndexOf("文", 5);
    assertEquals(1, index);

    index = str.LastIndexOf("文", 6);
    assertEquals(6, index);

    index = str.LastIndexOf("文A", 3);
    assertEquals(1, index);

    index = str.LastIndexOf("A", 5);
    assertEquals(4, index);

    index = str.LastIndexOf("AaA", 5);
    assertEquals(2, index);

    index = str.LastIndexOf("AaA中", 7);
    assertEquals(2, index);

    index = str.LastIndexOf("中文AaA中文", 7);
    assertEquals(0, index);

    index = str.LastIndexOf("中字", 7);
    assertEquals(-1, index);

    index = str.LastIndexOf("中A", 7);
    assertEquals(-1, index);

    index = str.LastIndexOf("中符", 7);
    assertEquals(-1, index);

    index = str.LastIndexOf("中文AaA中文B", 7);
    assertEquals(-1, index);

    printf("============Insensitive==============\n");
    //String str("中文AaA中文");
    index = str.LastIndexOfIgnoreCase("");
    assertEquals(7, index);

    index = str.LastIndexOfIgnoreCase("中", 7);
    assertEquals(5, index);

    index = str.LastIndexOfIgnoreCase("中文", 7);
    assertEquals(5, index);

    index = str.LastIndexOfIgnoreCase("文", 7);
    assertEquals(6, index);

    index = str.LastIndexOfIgnoreCase("中文", 3);
    assertEquals(0, index);

    index = str.LastIndexOfIgnoreCase("文", 7);
    assertEquals(6, index);

    index = str.LastIndexOfIgnoreCase("文A", 3);
    assertEquals(1, index);

    index = str.LastIndexOfIgnoreCase("A", 7);
    assertEquals(4, index);

    index = str.LastIndexOfIgnoreCase("AaA", 4);
    assertEquals(2, index);

    index = str.LastIndexOfIgnoreCase("AaA中", 4);
    assertEquals(2, index);

    index = str.LastIndexOfIgnoreCase("中文AaA中文", 7);
    assertEquals(0, index);

    index = str.LastIndexOfIgnoreCase("中字", 7);
    assertEquals(-1, index);

    index = str.LastIndexOfIgnoreCase("中A", 7);
    assertEquals(-1, index);

    index = str.LastIndexOfIgnoreCase("中符", 7);
    assertEquals(-1, index);

    index = str.LastIndexOfIgnoreCase("中文AaA中文B", 7);
    assertEquals(-1, index);

    printf("\n============== END testLastIndexOfString ==============\n");
}

void testContains()
{
    printf("\n============== BEGIN testContains ==============\n");

    String str1;

    str1 = String(NULL);
    assertNotContains("", str1);
    assertNotContains("1", str1, FALSE);

    str1 = String("");
    assertContains("", str1);
    assertContains("", str1, FALSE);

    str1 = String("1");
    assertContains("", str1);
    assertContains("", str1, FALSE);
    assertContains("1", str1);
    assertContains("1", str1, FALSE);
    assertNotContains("0", str1, FALSE);

    str1 = String("A");
    assertContains("A", str1);
    assertContains("A", str1, FALSE);
    assertNotContains("B", str1, FALSE);
    assertNotContains("AB", str1, FALSE);

    str1 = String("中");
    assertContains("中", str1);
    assertContains("中", str1, FALSE);
    assertNotContains("豆瓣", str1, FALSE);

    str1 = String("中文A0字符");
    assertContains("中", str1);
    assertContains("中", str1, FALSE);
    assertContains("中文", str1);
    assertContains("中文", str1, FALSE);
    assertContains("文A", str1);
    assertContains("文A", str1, FALSE);
    assertContains("A", str1);
    assertContains("A", str1, FALSE);
    assertContains("字", str1);
    assertContains("字", str1, FALSE);
    assertContains("符", str1);
    assertContains("符", str1, FALSE);
    assertNotContains("豆瓣", str1);
    assertNotContains("豆瓣", str1, FALSE);
    assertNotContains("中文A0字符B", str1);
    assertNotContains("中文A0字符B", str1, FALSE);

    str1 = String(NULL);
    assertNotContains(String(""), str1);
    assertNotContains(String("1"), str1, FALSE);

    str1 = String("");
    assertContains(String(""), str1);
    assertContains(String(""), str1, FALSE);

    str1 = String("1");
    assertContains(String(""), str1);
    assertContains(String(""), str1, FALSE);
    assertContains(String("1"), str1);
    assertContains(String("1"), str1, FALSE);
    assertNotContains(String("0"), str1, FALSE);

    str1 = String("A");
    assertContains(String("A"), str1);
    assertContains(String("A"), str1, FALSE);
    assertNotContains(String("B"), str1, FALSE);
    assertNotContains(String("AB"), str1, FALSE);

    str1 = String("中");
    assertContains(String("中"), str1);
    assertContains(String("中"), str1, FALSE);
    assertNotContains(String("豆瓣"), str1, FALSE);

    str1 = String("中文A0字符");
    assertContains(String("中"), str1);
    assertContains(String("中"), str1, FALSE);
    assertContains(String("中文"), str1);
    assertContains(String("中文"), str1, FALSE);
    assertContains(String("文A"), str1);
    assertContains(String("文A"), str1, FALSE);
    assertContains(String("A"), str1);
    assertContains(String("A"), str1, FALSE);
    assertContains(String("字"), str1);
    assertContains(String("字"), str1, FALSE);
    assertContains(String("符"), str1);
    assertContains(String("符"), str1, FALSE);
    assertNotContains(String("豆瓣"), str1);
    assertNotContains(String("豆瓣"), str1, FALSE);
    assertNotContains(String("中文A0字符B"), str1);
    assertNotContains(String("中文A0字符B"), str1, FALSE);

    printf("\n============== END testContains ==============\n");
}

void testStartWith()
{
    printf("\n============== BEGIN testStartWith ==============\n");

    String str1;

    str1 = String(NULL);
    assertNotStartWith("", str1);
    assertNotStartWith("1", str1, FALSE);

    str1 = String("");
    assertStartWith("", str1);
    assertStartWith("", str1, FALSE);

    str1 = String("1");
    assertStartWith("1", str1);
    assertStartWith("1", str1, FALSE);
    assertNotStartWith("0", str1);
    assertNotStartWith("0", str1, FALSE);

    str1 = String("A");
    assertStartWith("A", str1);
    assertStartWith("A", str1, FALSE);
    assertNotStartWith("B", str1);
    assertNotStartWith("B", str1, FALSE);
    assertNotStartWith("AB", str1, FALSE);

    str1 = String("中");
    assertStartWith("中", str1);
    assertStartWith("中", str1, FALSE);
    assertNotStartWith("豆", str1);
    assertNotStartWith("豆", str1, FALSE);

    str1 = String("中文A0字符");
    assertStartWith("中", str1);
    assertStartWith("中", str1, FALSE);
    assertStartWith("中文", str1);
    assertStartWith("中文", str1, FALSE);
    assertStartWith("中文A", str1);
    assertStartWith("中文A", str1, FALSE);
    assertStartWith("中文A0字符", str1);
    assertStartWith("中文A0字符", str1, FALSE);
    assertNotStartWith("字", str1);
    assertNotStartWith("字", str1, FALSE);
    assertNotStartWith("中字", str1);
    assertNotStartWith("中字", str1, FALSE);
    assertNotStartWith("中文B", str1);
    assertNotStartWith("中文B", str1, FALSE);
    assertNotStartWith("中文A0字符B", str1);
    assertNotStartWith("中文A0字符B", str1, FALSE);

    str1 = String(NULL);
    assertNotStartWith(String(""), str1);
    assertNotStartWith(String("1"), str1, FALSE);

    str1 = String(String(""));
    assertStartWith(String(""), str1);
    assertStartWith(String(""), str1, FALSE);

    str1 = String(String("1"));
    assertStartWith(String("1"), str1);
    assertStartWith(String("1"), str1, FALSE);
    assertNotStartWith(String("0"), str1);
    assertNotStartWith(String("0"), str1, FALSE);

    str1 = String(String("A"));
    assertStartWith(String("A"), str1);
    assertStartWith(String("A"), str1, FALSE);
    assertNotStartWith(String("B"), str1);
    assertNotStartWith(String("B"), str1, FALSE);
    assertNotStartWith(String("AB"), str1, FALSE);

    str1 = String(String("中"));
    assertStartWith(String("中"), str1);
    assertStartWith(String("中"), str1, FALSE);
    assertNotStartWith(String("豆"), str1);
    assertNotStartWith(String("豆"), str1, FALSE);

    str1 = String(String("中文A0字符"));
    assertStartWith(String("中"), str1);
    assertStartWith(String("中"), str1, FALSE);
    assertStartWith(String("中文"), str1);
    assertStartWith(String("中文"), str1, FALSE);
    assertStartWith(String("中文A"), str1);
    assertStartWith(String("中文A"), str1, FALSE);
    assertStartWith(String("中文A0字符"), str1);
    assertStartWith(String("中文A0字符"), str1, FALSE);
    assertNotStartWith(String("字"), str1);
    assertNotStartWith(String("字"), str1, FALSE);
    assertNotStartWith(String("中字"), str1);
    assertNotStartWith(String("中字"), str1, FALSE);
    assertNotStartWith(String("中文B"), str1);
    assertNotStartWith(String("中文B"), str1, FALSE);
    assertNotStartWith(String("中文A0字符B"), str1);
    assertNotStartWith(String("中文A0字符B"), str1, FALSE);

    printf("\n============== END testStartWith ==============\n");
}

void testEndWith()
{
    printf("\n============== BEGIN testEndWith ==============\n");

    String str1;

    str1 = String(NULL);
    assertNotEndWith("", str1);
    assertNotEndWith("1", str1, FALSE);

    str1 = String("");
    assertEndWith("", str1);
    assertEndWith("", str1, FALSE);

    str1 = String("1");
    assertEndWith("1", str1);
    assertEndWith("1", str1, FALSE);
    assertNotEndWith("0", str1);
    assertNotEndWith("0", str1, FALSE);

    str1 = String("A");
    assertEndWith("A", str1);
    assertEndWith("A", str1, FALSE);
    assertNotEndWith("B", str1);
    assertNotEndWith("B", str1, FALSE);
    assertNotEndWith("AB", str1, FALSE);

    str1 = String("中");
    assertEndWith("中", str1);
    assertEndWith("中", str1, FALSE);
    assertNotEndWith("豆", str1);
    assertNotEndWith("豆", str1, FALSE);

    str1 = String("中文A0字符");
    assertEndWith("", str1);
    assertEndWith("", str1, FALSE);
    assertEndWith("符", str1);
    assertEndWith("符", str1, FALSE);
    assertEndWith("字符", str1);
    assertEndWith("字符", str1, FALSE);
    assertEndWith("0字符", str1);
    assertEndWith("0字符", str1, FALSE);
    assertEndWith("中文A0字符", str1);
    assertEndWith("中文A0字符", str1, FALSE);
    assertNotEndWith("字", str1);
    assertNotEndWith("字", str1, FALSE);
    assertNotEndWith("字中", str1);
    assertNotEndWith("字中", str1, FALSE);
    assertNotEndWith("中文A0字符B", str1);
    assertNotEndWith("中文A0字符B", str1, FALSE);

    str1 = String(NULL);
    assertNotEndWith(String(""), str1);
    assertNotEndWith(String("1"), str1, FALSE);

    str1 = String(String(""));
    assertEndWith(String(""), str1);
    assertEndWith(String(""), str1, FALSE);

    str1 = String(String("1"));
    assertEndWith(String("1"), str1);
    assertEndWith(String("1"), str1, FALSE);
    assertNotEndWith(String("0"), str1);
    assertNotEndWith(String("0"), str1, FALSE);

    str1 = String(String("A"));
    assertEndWith(String("A"), str1);
    assertEndWith(String("A"), str1, FALSE);
    assertNotEndWith(String("B"), str1);
    assertNotEndWith(String("B"), str1, FALSE);
    assertNotEndWith(String("AB"), str1, FALSE);

    str1 = String(String("中"));
    assertEndWith(String("中"), str1);
    assertEndWith(String("中"), str1, FALSE);
    assertNotEndWith(String("豆"), str1);
    assertNotEndWith(String("豆"), str1, FALSE);

    str1 = String("中文A0字符");
    assertEndWith(String("符"), str1);
    assertEndWith(String("符"), str1, FALSE);
    assertEndWith(String("字符"), str1);
    assertEndWith(String("字符"), str1, FALSE);
    assertEndWith(String("0字符"), str1);
    assertEndWith(String("0字符"), str1, FALSE);
    assertEndWith(String("中文A0字符"), str1);
    assertEndWith(String("中文A0字符"), str1, FALSE);
    assertNotEndWith(String("字"), str1);
    assertNotEndWith(String("字"), str1, FALSE);
    assertNotEndWith(String("字中"), str1);
    assertNotEndWith(String("字中"), str1, FALSE);
    assertNotEndWith(String("中文A0字符B"), str1);
    assertNotEndWith(String("中文A0字符B"), str1, FALSE);

    printf("\n============== END testEndWith ==============\n");
}

void testCompare()
{
    printf("\n============== BEGIN testCompare ==============\n");
    String str1("A");
    assert(str1.Compare("A") == 0);
    assert(str1.CompareIgnoreCase("a") == 0);

    str1 = String("AD");
    assert(str1.Compare("AC") > 0);
    assert(str1.Compare("Ae") < 0);

    str1 = String("1AB");
    assert(str1.Compare("1AB") == 0);
    assert(str1.CompareIgnoreCase("1aB") == 0);

    str1 = String("中文AB");
    assert(str1.Compare("中文AB") == 0);
    assert(str1.CompareIgnoreCase("中文aB") == 0);

    str1 = String("中文AD");
    assert(str1.Compare("中文AC") > 0);
    assert(str1.Compare("中文AE") < 0);
    assert(str1.CompareIgnoreCase("中文Ad1") < 0);

    printf("\n============== END testCompare ==============\n");
}

//==================================================================
//                      MAIN
//==================================================================
int main(int argc, char *argv[])
{
    printf("============== BEGIN TEST ==============\n");

    testConstructor();
    testSetTo();
    testAppend();
    testSubstring();
    testGetChar();
    testGetChars();
    testToLowerCase();
    testTrim();
    testIndexOfString();
    testIndexOfChar();
    testLastIndexOfChar();
    testLastIndexOfString();
    testContains();
    testStartWith();
    testEndWith();
    testCompare();

    printf("\n============== END TEST ==============\n");
    return 0;
}
