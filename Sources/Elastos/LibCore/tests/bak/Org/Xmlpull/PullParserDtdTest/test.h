#ifndef __PULLPARSERDTD_TEST_H
#define __PULLPARSERDTD_TEST_H

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
    int testDeclaringParameterEntities(int argc, char* argv[]);
    int testUsingParameterEntitiesInDtds(int argc, char* argv[]);
    int testUsingParameterInDocuments(int argc, char* argv[]);
    int testGeneralAndParameterEntityWithTheSameName(int argc, char* argv[]);
    int testInternalEntities(int argc, char* argv[]);
    int testExternalDtdIsSilentlyIgnored(int argc, char* argv[]);
    int testExternalAndInternalDtd(int argc, char* argv[]);
    int testInternalEntitiesAreParsed(int argc, char* argv[]);
    int testFoolishlyRecursiveInternalEntities(int argc, char* argv[]);
    int testExpansionOfEntityAndCharacterReferences(int argc, char* argv[]);
    int testGeneralEntitiesAreStoredUnresolved(int argc, char* argv[]);
    int testStructuredEntityAndNextToken(int argc, char* argv[]);
    int testUsingExternalEntities(int argc, char* argv[]);
    int testExternalDtdAndMissingEntity(int argc, char* argv[]);
    int testExternalIdIsCaseSensitive(int argc, char* argv[]);
    int testDtdDoesNotInformIgnorableWhitespace(int argc, char* argv[]);
    int testEmptyDoesNotInformIgnorableWhitespace(int argc, char* argv[]);
    int testAttributeOfTypeEntity(int argc, char* argv[]);
    int testTagStructureNotValidated(int argc, char* argv[]);
    int testAttributeDefaultValues(int argc, char* argv[]);
    int testAttributeDefaultValueEntitiesExpanded(int argc, char* argv[]);
    int testAttributeDefaultValuesAndNamespaces(int argc, char* argv[]);
    int testAttributeEntitiesExpandedEagerly(int argc, char* argv[]);
    int testRequiredAttributesOmitted(int argc, char* argv[]);
    int testFixedAttributesWithConflictingValues(int argc, char* argv[]);
    int testParsingNotations(int argc, char* argv[]);
    int testCommentsInDoctype(int argc, char* argv[]);
    int testDoctypeNameOnly(int argc, char* argv[]);
    int testVeryLongEntities(int argc, char* argv[]);
    int testManuallyRegisteredEntitiesWithDoctypeParsing(int argc, char* argv[]);
    int testDoctypeWithNextToken(int argc, char* argv[]);
    int testDoctypeSpansBuffers(int argc, char* argv[]);
    int testDoctypeInDocumentElement(int argc, char* argv[]);
    int testDoctypeAfterDocumentElement(int argc, char* argv[]);

public:
    virtual AutoPtr<IXmlPullParser> newPullParser() = 0;

private:
    int indexOfAttributeWithName(AutoPtr<IXmlPullParser> parser, String name);
    void assertParseFailure(String xml);
    String repeat(char c, int length);
    AutoPtr<IXmlPullParser> newPullParser(String xml);


private:
    static const int READ_BUFFER_SIZE = 8192;
};

class  KmlCtest : public CTest
{
public:
     KmlCtest(){}
    ~ KmlCtest(){}

    AutoPtr<IXmlPullParser> newPullParser();
};

#endif //__PULLPARSERDTD_TEST_H
