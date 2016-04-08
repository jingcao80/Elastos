#include "test.h"
#include <stdio.h>
#include <stdlib.h>
#include <elautoptr.h>

typedef int (KmlCtest::*PTestEntry)(int argc, char *argv[]);

PTestEntry TestEntry[] =
{
    &KmlCtest::testAttributeNoValueWithRelaxed,
    &KmlCtest::testAttributeUnquotedValueWithRelaxed,
    &KmlCtest::testUnterminatedEntityWithRelaxed,
    &KmlCtest::testEntitiesAndNamespaces,
    &KmlCtest::testEntitiesAndNamespacesWithRelaxed,
    &KmlCtest::testRegularNumericEntities,
    &KmlCtest::testNumericEntitiesLargerThanChar,
    &KmlCtest::testNumericEntitiesLargerThanInt,
    &KmlCtest::testCharacterReferenceOfHexUtf16Surrogates,
    &KmlCtest::testCharacterReferenceOfDecimalUtf16Surrogates,
    &KmlCtest::testCharacterReferenceOfLastUtf16Surrogate,
    &KmlCtest::testOmittedNumericEntities,
    &KmlCtest::testCarriageReturnLineFeed,
    &KmlCtest::testLoneCarriageReturn,
    &KmlCtest::testLoneNewLine,
    &KmlCtest::testXmlDeclaration,
    &KmlCtest::testXmlDeclarationExtraAttributes,
    &KmlCtest::testCustomEntitiesUsingNext,
    &KmlCtest::testCustomEntitiesUsingNextToken,
    &KmlCtest::testCustomEntitiesAreNotEvaluated,
    &KmlCtest::testMissingEntities,
    &KmlCtest::testMissingEntitiesWithRelaxed,
    &KmlCtest::testMissingEntitiesUsingNextToken,
    &KmlCtest::testMissingEntitiesUsingNextTokenWithRelaxed,
    &KmlCtest::testEntityInAttributeUsingNextToken,
    &KmlCtest::testMissingEntitiesInAttributesUsingNext,
    &KmlCtest::testMissingEntitiesInAttributesUsingNextWithRelaxed,
    &KmlCtest::testMissingEntitiesInAttributesUsingNextToken,
    &KmlCtest::testMissingEntitiesInAttributesUsingNextTokenWithRelaxed,
    &KmlCtest::testGreaterThanInText,
    &KmlCtest::testGreaterThanInAttribute,
    &KmlCtest::testLessThanInText,
    &KmlCtest::testLessThanInAttribute,
    &KmlCtest::testQuotesInAttribute,
    &KmlCtest::testQuotesInText,
    &KmlCtest::testCdataDelimiterInAttribute,
    &KmlCtest::testCdataDelimiterInText,
    &KmlCtest::testUnexpectedEof,
    &KmlCtest::testUnexpectedSequence,
    &KmlCtest::testThreeDashCommentDelimiter,
    &KmlCtest::testTwoDashesInComment,
    &KmlCtest::testEmptyComment,
    &KmlCtest::testManyCloseBraces,
    &KmlCtest::testCommentUsingNext,
    &KmlCtest::testCommentUsingNextToken,
    &KmlCtest::testCdataUsingNext,
    &KmlCtest::testCdataUsingNextToken,
    &KmlCtest::testEntityLooksLikeCdataClose,
    &KmlCtest::testProcessingInstructionUsingNext,
    &KmlCtest::testProcessingInstructionUsingNextToken,
    &KmlCtest::testWhitespaceUsingNextToken,
    &KmlCtest::testLinesAndColumns,
    &KmlCtest::testEmptyEntityReferenceUsingNext,
    &KmlCtest::testEmptyEntityReferenceUsingNextToken,
    &KmlCtest::testEmptyCdataUsingNext,
    &KmlCtest::testEmptyCdataUsingNextToken,
    &KmlCtest::testParseReader,
    &KmlCtest::testParseInputStream,
    &KmlCtest::testNextAfterEndDocument,
    &KmlCtest::testNamespaces,
    &KmlCtest::testTextBeforeDocumentElement,
    &KmlCtest::testTextAfterDocumentElement,
    &KmlCtest::testTextNoDocumentElement,
    &KmlCtest::testBomAndByteInput,
    &KmlCtest::testBomAndByteInputWithExplicitCharset,
    &KmlCtest::testBomAndCharacterInput,
    &KmlCtest::testNextTextAdvancesToEndTag,
    &KmlCtest::testNextTag,
};

int main(int argc, char* argv[])
{
    printf("%s %d\n", __FILE__, __LINE__);
    int nIndex = atoi(argv[1]);
    printf("%s %d\n", __FILE__, __LINE__);
    KmlCtest testObj;
    (testObj.*TestEntry[nIndex-1])(argc, argv);

    return 0;
}
