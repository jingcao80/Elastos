#ifndef __PULLPARSER_TEST_H
#define __PULLPARSER_TEST_H

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
    int testAttributeNoValueWithRelaxed(int argc, char* argv[]);
    int testAttributeUnquotedValueWithRelaxed(int argc, char* argv[]);
    int testUnterminatedEntityWithRelaxed(int argc, char* argv[]);
    int testEntitiesAndNamespaces(int argc, char* argv[]);
    int testEntitiesAndNamespacesWithRelaxed(int argc, char* argv[]);
    int testRegularNumericEntities(int argc, char* argv[]);
    int testNumericEntitiesLargerThanChar(int argc, char* argv[]);
    int testNumericEntitiesLargerThanInt(int argc, char* argv[]);
    int testCharacterReferenceOfHexUtf16Surrogates(int argc, char* argv[]);
    int testCharacterReferenceOfDecimalUtf16Surrogates(int argc, char* argv[]);
    int testCharacterReferenceOfLastUtf16Surrogate(int argc, char* argv[]);
    int testOmittedNumericEntities(int argc, char* argv[]);
    int testCarriageReturnLineFeed(int argc, char* argv[]);
    int testLoneCarriageReturn(int argc, char* argv[]);
    int testLoneNewLine(int argc, char* argv[]);
    int testXmlDeclaration(int argc, char* argv[]);
    int testXmlDeclarationExtraAttributes(int argc, char* argv[]);
    int testCustomEntitiesUsingNext(int argc, char* argv[]);
    int testCustomEntitiesUsingNextToken(int argc, char* argv[]);
    int testCustomEntitiesAreNotEvaluated(int argc, char* argv[]);
    int testMissingEntities(int argc, char* argv[]);
    int testMissingEntitiesWithRelaxed(int argc, char* argv[]);
    int testMissingEntitiesUsingNextToken(int argc, char* argv[]);
    int testMissingEntitiesUsingNextTokenWithRelaxed(int argc, char* argv[]);
    int testEntityInAttributeUsingNextToken(int argc, char* argv[]);
    int testMissingEntitiesInAttributesUsingNext(int argc, char* argv[]);
    int testMissingEntitiesInAttributesUsingNextWithRelaxed(int argc, char* argv[]);
    int testMissingEntitiesInAttributesUsingNextToken(int argc, char* argv[]);
    int testMissingEntitiesInAttributesUsingNextTokenWithRelaxed(int argc, char* argv[]);
    int testGreaterThanInText(int argc, char* argv[]);
    int testGreaterThanInAttribute(int argc, char* argv[]);
    int testLessThanInText(int argc, char* argv[]);
    int testLessThanInAttribute(int argc, char* argv[]);
    int testQuotesInAttribute(int argc, char* argv[]);
    int testQuotesInText(int argc, char* argv[]);
    int testCdataDelimiterInAttribute(int argc, char* argv[]);
    int testCdataDelimiterInText(int argc, char* argv[]);
    int testUnexpectedEof(int argc, char* argv[]);
    int testUnexpectedSequence(int argc, char* argv[]);
    int testThreeDashCommentDelimiter(int argc, char* argv[]);
    int testTwoDashesInComment(int argc, char* argv[]);
    int testEmptyComment(int argc, char* argv[]);
    int testManyCloseBraces(int argc, char* argv[]);
    int testCommentUsingNext(int argc, char* argv[]);
    int testCommentUsingNextToken(int argc, char* argv[]);
    int testCdataUsingNext(int argc, char* argv[]);
    int testCdataUsingNextToken(int argc, char* argv[]);
    int testEntityLooksLikeCdataClose(int argc, char* argv[]);
    int testProcessingInstructionUsingNext(int argc, char* argv[]);
    int testProcessingInstructionUsingNextToken(int argc, char* argv[]);
    int testWhitespaceUsingNextToken(int argc, char* argv[]);
    int testLinesAndColumns(int argc, char* argv[]);
    int testEmptyEntityReferenceUsingNext(int argc, char* argv[]);
    int testEmptyEntityReferenceUsingNextToken(int argc, char* argv[]);
    int testEmptyCdataUsingNext(int argc, char* argv[]);
    int testEmptyCdataUsingNextToken(int argc, char* argv[]);
    int testParseReader(int argc, char* argv[]);
    int testParseInputStream(int argc, char* argv[]);
    int testNextAfterEndDocument(int argc, char* argv[]);
    int testNamespaces(int argc, char* argv[]);
    int testTextBeforeDocumentElement(int argc, char* argv[]);
    int testTextAfterDocumentElement(int argc, char* argv[]);
    int testTextNoDocumentElement(int argc, char* argv[]);
    int testBomAndByteInput(int argc, char* argv[]);
    int testBomAndByteInputWithExplicitCharset(int argc, char* argv[]);
    int testBomAndCharacterInput(int argc, char* argv[]);
    int testNextTextAdvancesToEndTag(int argc, char* argv[]);
    int testNextTag(int argc, char* argv[]);

public:
    virtual AutoPtr<IXmlPullParser> newPullParser() = 0;

private:

    void testNamespace(AutoPtr<IXmlPullParser> parser);
    void testCharacterReferenceOfUtf16Surrogates(String xml);
    void testLineEndings(String xml);
    void testMissingEntitiesUsingNextToken(AutoPtr<IXmlPullParser> parser);
    void testMissingEntitiesInAttributesUsingNextToken(AutoPtr<IXmlPullParser> parser);
    static void validate(AutoPtr<IXmlPullParser> parser);
    void checkNamespacesInOne(AutoPtr<IXmlPullParser> parser);
    void checkNamespacesInTwo(AutoPtr<IXmlPullParser> parser);
    void assertParseFailure(String xml);
};

class  KmlCtest : public CTest
{
public:
     KmlCtest(){}
    ~ KmlCtest(){}

    AutoPtr<IXmlPullParser> newPullParser();
};

#endif //__PULLPARSER_TEST_H
