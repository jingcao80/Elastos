#ifndef __PATTTERN_TEST_H
#define __PATTTERN_TEST_H


#include <elastos.h>
#include <stdio.h>
#include "Elastos.CoreLibrary.h"
#include <elautoptr.h>

using namespace Elastos;
using namespace Libcore::ICU;
using namespace Elastos::Text;

#define TEST(a, x) a.test_##x

class CTest {
public:
    CTest();
    ~CTest(){};

public:
    int test_applyPattern(int argc, char* argv[]);
    int test_formatToCharacterIterator(int argc, char* argv[]);
    int test_getLocale(int argc, char* argv[]);
    int test_setFormat(int argc, char* argv[]);
    int test_formatLjava_lang_ObjectLjava_lang_StringBufferLjava_text_FieldPosition(int argc, char* argv[]);
    int test_formatLjava_lang_StringLjava_lang_Object(int argc, char* argv[]);
    int test_formatLjava_lang_ObjectLjava_lang_StringBufferLjava_text_FieldPosition2(int argc, char* argv[]);
    int test_setFormatsLjava_text_Format(int argc, char* argv[]);
    int test_parseLjava_lang_String(int argc, char* argv[]);
    int test_parseObjectLjava_lang_StringLjavajava_text_ParsePosition(int argc, char* argv[]);
    int test_format_Object(int argc, char* argv[]);
    int test_parseLjava_lang_StringLjava_text_ParsePosition(int argc, char* argv[]);

private:
    AutoPtr<IMessageFormat> format1;
};


#endif //__PATTTERN_TEST_H
