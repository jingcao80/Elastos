#include "test.h"
#include <stdio.h>
#include <stdlib.h>
#include <elautoptr.h>

typedef int (KmlCtest::*PTestEntry)(int argc, char *argv[]);

PTestEntry TestEntry[] =
{
    &KmlCtest::testDeclaringParameterEntities,
    &KmlCtest::testUsingParameterEntitiesInDtds,
    &KmlCtest::testUsingParameterInDocuments,
    &KmlCtest::testGeneralAndParameterEntityWithTheSameName,
    &KmlCtest::testInternalEntities,
    &KmlCtest::testExternalDtdIsSilentlyIgnored,
    &KmlCtest::testExternalAndInternalDtd,
    &KmlCtest::testInternalEntitiesAreParsed,
    &KmlCtest::testFoolishlyRecursiveInternalEntities,
    &KmlCtest::testExpansionOfEntityAndCharacterReferences,
    &KmlCtest::testGeneralEntitiesAreStoredUnresolved,
    &KmlCtest::testStructuredEntityAndNextToken,
    &KmlCtest::testUsingExternalEntities,
    &KmlCtest::testExternalDtdAndMissingEntity,
    &KmlCtest::testExternalIdIsCaseSensitive,
    &KmlCtest::testDtdDoesNotInformIgnorableWhitespace,
    &KmlCtest::testEmptyDoesNotInformIgnorableWhitespace,
    &KmlCtest::testAttributeOfTypeEntity,
    &KmlCtest::testTagStructureNotValidated,
    &KmlCtest::testAttributeDefaultValues,
    &KmlCtest::testAttributeDefaultValueEntitiesExpanded,
    &KmlCtest::testDeclaringParameterEntities,
    &KmlCtest::testUsingParameterEntitiesInDtds,
    &KmlCtest::testUsingParameterInDocuments,
    &KmlCtest::testGeneralAndParameterEntityWithTheSameName,
    &KmlCtest::testInternalEntities,
    &KmlCtest::testExternalDtdIsSilentlyIgnored,
    &KmlCtest::testExternalAndInternalDtd,
    &KmlCtest::testAttributeDefaultValuesAndNamespaces,
    &KmlCtest::testAttributeEntitiesExpandedEagerly,
    &KmlCtest::testRequiredAttributesOmitted,
    &KmlCtest::testFixedAttributesWithConflictingValues,
    &KmlCtest::testParsingNotations,
    &KmlCtest::testCommentsInDoctype,
    &KmlCtest::testDoctypeNameOnly,
    &KmlCtest::testVeryLongEntities,
    &KmlCtest::testManuallyRegisteredEntitiesWithDoctypeParsing,
    &KmlCtest::testDoctypeWithNextToken,
    &KmlCtest::testDoctypeSpansBuffers,
    &KmlCtest::testDoctypeInDocumentElement,
    &KmlCtest::testDoctypeAfterDocumentElement,
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
