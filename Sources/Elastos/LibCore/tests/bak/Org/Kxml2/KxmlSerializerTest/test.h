#ifndef __KXMLSERIALIZER_TEST_H
#define __KXMLSERIALIZER_TEST_H

#include <elastos.h>
#include "Elastos.CoreLibrary.h"
#include "elastos/StringBuilder.h"
#include "elastos/StringBuffer.h"
#include "elastos/StringUtils.h"
#include "elastos/HashMap.h"
#include "elastos/List.h"

using namespace Elastos;
using namespace Org::Xml::Sax;
using namespace Org::Xml::Sax::Ext;
using namespace Org::Xml::Sax::Helpers;
using namespace Elastos::Core;
using namespace Elastos::IO;
using namespace Org::Kxml2::IO;
using namespace Org::Xmlpull::V1;

#define TEST(a, x) a.test_##x

class CTest {
public:
    CTest(){};
    ~CTest(){};

public:
    int testWhitespaceInAttributeValue(int argc, char* argv[]);
    int testWriteDocument(int argc, char* argv[]);
    int testWriteSpecialCharactersInText(int argc, char* argv[]);
    int testInvalidCharactersInText(int argc, char* argv[]);
    int testInvalidCharactersInAttributeValues(int argc, char* argv[]);
    int testInvalidCharactersInCdataSections(int argc, char* argv[]);
    int testCdataWithTerminatorInside(int argc, char* argv[]);

private:
    void assertXmlEquals(String expectedXml, String actualXml);
    static AutoPtr<IXmlSerializer> newSerializer();
    static Boolean isValidXmlCodePoint(int c);

private:
    static String NAMESPACE;
};

#endif //__KXMLSERIALIZER_TEST_H
