
#include "test.h"

const String FEATURE_PROCESS_NAMESPACES = String("http://xmlpull.org/v1/doc/features.html#process-namespaces");

int CTest::testAttributeNoValueWithRelaxed(int argc, char* argv[])
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    parser->SetFeature(String("http://xmlpull.org/v1/doc/features.html#relaxed"), true);
    AutoPtr<IStringReader> sr;
    CStringReader::New(String("<input checked></input>"), (IStringReader**)&sr);
    parser->SetInput(sr);
    Int32 value = 0;
    String str("");
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(String("input") == (parser->GetName(&str), str));
    assert(String("checked") == (parser->GetAttributeName(0, &str), str));
    assert(String("checked") == (parser->GetAttributeValue(0, &str), str));
    return 0;
}

int CTest::testAttributeUnquotedValueWithRelaxed(int argc, char* argv[])
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    parser->SetFeature(String("http://xmlpull.org/v1/doc/features.html#relaxed"), true);
    AutoPtr<IStringReader> sr;
    CStringReader::New(String("<input checked=true></input>"), (IStringReader**)&sr);
    parser->SetInput(sr);
    Int32 value = 0;
    String str("");
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(String("input") == (parser->GetName(&str), str));
    assert(String("checked") == (parser->GetAttributeName(0, &str), str));
    assert(String("true") == (parser->GetAttributeValue(0, &str), str));
    return 0;
}

int CTest::testUnterminatedEntityWithRelaxed(int argc, char* argv[])
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    parser->SetFeature(String("http://xmlpull.org/v1/doc/features.html#relaxed"), true);
    AutoPtr<IStringReader> sr;
    CStringReader::New(String("<foo bar='A&W'>mac&cheese</foo>"), (IStringReader**)&sr);
    parser->SetInput(sr);
    Int32 value = 0;
    String str("");
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(String("foo") == (parser->GetName(&str), str));
    assert(String("bar") == (parser->GetAttributeName(0, &str), str));
    assert(String("A&W") == (parser->GetAttributeValue(0, &str), str));
    assert(IXmlPullParser::TEXT == (parser->Next(&value), value));
    assert(String("mac&cheese") == (parser->GetText(&str), str));
    return 0;
}

int CTest::testEntitiesAndNamespaces(int argc, char* argv[])
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    parser->SetFeature(String("http://xmlpull.org/v1/doc/features.html#process-namespaces"), true);
    AutoPtr<IStringReader> sr;
    CStringReader::New(String("<foo:a xmlns:foo='http://foo' xmlns:bar='http://bar'><bar:b/></foo:a>"), (IStringReader**)&sr);
    parser->SetInput(sr);
    testNamespace(parser);
    return 0;
}

int CTest::testEntitiesAndNamespacesWithRelaxed(int argc, char* argv[])
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    parser->SetFeature(String("http://xmlpull.org/v1/doc/features.html#process-namespaces"), true);
    parser->SetFeature(String("http://xmlpull.org/v1/doc/features.html#relaxed"), true);
    AutoPtr<IStringReader> sr;
    CStringReader::New(String("<foo:a xmlns:foo='http://foo' xmlns:bar='http://bar'><bar:b/></foo:a>"), (IStringReader**)&sr);
    parser->SetInput(sr);
    testNamespace(parser);
    return 0;
}

int CTest::testRegularNumericEntities(int argc, char* argv[])
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    AutoPtr<IStringReader> sr;
    CStringReader::New(String("<foo>&#65;</foo>"), (IStringReader**)&sr);
    parser->SetInput(sr);
    Int32 value = 0;
    String str("");
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::ENTITY_REF == (parser->NextToken(&value), value));
    assert(String("#65") == (parser->GetName(&str), str));
    assert(String("A") == (parser->GetText(&str), str));
    return 0;
}

int CTest::testNumericEntitiesLargerThanChar(int argc, char* argv[])
{
    assertParseFailure(String("<foo>&#2147483647; &#-2147483648;</foo>"));
    return 0;
}

int CTest::testNumericEntitiesLargerThanInt(int argc, char* argv[])
{
    assertParseFailure(String("<foo>&#2147483648;</foo>"));
    return 0;
}

int CTest::testCharacterReferenceOfHexUtf16Surrogates(int argc, char* argv[])
{
    testCharacterReferenceOfUtf16Surrogates(String("<foo>&#x10000; &#x10381; &#x10FFF0;</foo>"));
    return 0;
}

int CTest::testCharacterReferenceOfDecimalUtf16Surrogates(int argc, char* argv[])
{
    testCharacterReferenceOfUtf16Surrogates(String("<foo>&#65536; &#66433; &#1114096;</foo>"));
    return 0;
}

int CTest::testCharacterReferenceOfLastUtf16Surrogate(int argc, char* argv[])
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    AutoPtr<IStringReader> sr;
    CStringReader::New(String("<foo>&#x10FFFF;</foo>"), (IStringReader**)&sr);
    parser->SetInput(sr);
    Int32 value = 0;
    String str("");
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::TEXT == (parser->Next(&value), value));
    AutoPtr< ArrayOf<Char32> > array = ArrayOf<Char32>::Alloc(1);
    (*array)[0] = 0x10FFFF;
    String substr(*array, 0, 1);
    assert(substr == (parser->GetText(&str), str));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    return 0;
}

int CTest::testOmittedNumericEntities(int argc, char* argv[])
{
    assertParseFailure(String("<foo>&#;</foo>"));
    return 0;
}

int CTest::testCarriageReturnLineFeed(int argc, char* argv[])
{
    testLineEndings(String("\r\n<foo\r\na='b\r\nc'\r\n>d\r\ne</foo\r\n>\r\n"));
    return 0;
}

int CTest::testLoneCarriageReturn(int argc, char* argv[])
{
    testLineEndings(String("\r<foo\ra='b\rc'\r>d\re</foo\r>\r"));
    return 0;
}

int CTest::testLoneNewLine(int argc, char* argv[])
{
    testLineEndings(String("\n<foo\na='b\nc'\n>d\ne</foo\n>\n"));
    return 0;
}

int CTest::testXmlDeclaration(int argc, char* argv[])
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    AutoPtr<IStringReader> sr;
    CStringReader::New(String("<?xml version='1.0' encoding='UTF-8' standalone='no'?><foo/>"), (IStringReader**)&sr);
    parser->SetInput(sr);
    Int32 value = 0;
    String str("");
    assert(IXmlPullParser::START_TAG == (parser->NextToken(&value), value));
    AutoPtr<ICharSequence> csq;
    parser->GetProperty(String("http://xmlpull.org/v1/doc/properties.html#xmldecl-version"), (IInterface**)&csq);
    assert(csq != NULL);
    csq->ToString(&str);
    assert(String("1.0") == str);
    AutoPtr<IBoolean> blean;
    parser->GetProperty(String("http://xmlpull.org/v1/doc/properties.html#xmldecl-standalone"), (IInterface**)&blean);
    Boolean outbool = FALSE;
    blean->GetValue(&outbool);
    assert(FALSE == outbool);
    assert(String("UTF-8") == (parser->GetInputEncoding(&str), str));
    return 0;
}

int CTest::testXmlDeclarationExtraAttributes(int argc, char* argv[])
{
    assertParseFailure(String("<?xml version='1.0' encoding='UTF-8' standalone='no' a='b'?><foo/>"));
    return 0;
}

int CTest::testCustomEntitiesUsingNext(int argc, char* argv[])
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    AutoPtr<IStringReader> sr;
    CStringReader::New(String("<foo a='cd&aaaaaaaaaa;ef'>wx&aaaaaaaaaa;yz</foo>"), (IStringReader**)&sr);
    parser->SetInput(sr);
    Int32 value = 0;
    String str("");
    parser->DefineEntityReplacementText(String("aaaaaaaaaa"), String("b"));
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(String("cdbef") == (parser->GetAttributeValue(0, &str), str));
    assert(IXmlPullParser::TEXT == (parser->Next(&value), value));
    assert(String("wxbyz") == (parser->GetText(&str), str));
    return 0;
}

int CTest::testCustomEntitiesUsingNextToken(int argc, char* argv[])
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    AutoPtr<IStringReader> sr;
    CStringReader::New(String("<foo a='cd&aaaaaaaaaa;ef'>wx&aaaaaaaaaa;yz</foo>"), (IStringReader**)&sr);
    parser->SetInput(sr);
    Int32 value = 0;
    String str("");
    parser->DefineEntityReplacementText(String("aaaaaaaaaa"), String("b"));
    assert(IXmlPullParser::START_TAG == (parser->NextToken(&value), value));
    assert(String("cdbef") == (parser->GetAttributeValue(0, &str), str));
    assert(IXmlPullParser::TEXT == (parser->NextToken(&value), value));
    assert(String("wx") == (parser->GetText(&str), str));
    assert(IXmlPullParser::ENTITY_REF == (parser->NextToken(&value), value));
    assert(String("aaaaaaaaaa") == (parser->GetName(&str), str));
    assert(String("b") == (parser->GetText(&str), str));
    assert(IXmlPullParser::TEXT == (parser->NextToken(&value), value));
    assert(String("yz") == (parser->GetText(&str), str));
    return 0;
}

int CTest::testCustomEntitiesAreNotEvaluated(int argc, char* argv[])
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    AutoPtr<IStringReader> sr;
    CStringReader::New(String("<foo a='&a;'>&a;</foo>"), (IStringReader**)&sr);
    parser->SetInput(sr);
    Int32 value = 0;
    String str("");
    parser->DefineEntityReplacementText(String("a"), String("&amp; &a;"));
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(String("&amp; &a;") == (parser->GetAttributeValue(0, &str), str));
    assert(IXmlPullParser::TEXT == (parser->Next(&value), value));
    assert(String("&amp; &a;") == (parser->GetText(&str), str));
    return 0;
}

int CTest::testMissingEntities(int argc, char* argv[])
{
    assertParseFailure(String("<foo>&aaa;</foo>"));
    return 0;
}

int CTest::testMissingEntitiesWithRelaxed(int argc, char* argv[])
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    parser->SetFeature(String("http://xmlpull.org/v1/doc/features.html#relaxed"), true);
    AutoPtr<IStringReader> sr;
    CStringReader::New(String("<foo>&aaa;</foo>"), (IStringReader**)&sr);
    parser->SetInput(sr);
    Int32 value = 0;
    String str("");
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::TEXT == (parser->Next(&value), value));
    assert((parser->GetName(&str), str).IsNull());
    // Expected unresolved entities to be left in-place. The old parser
    // would resolve these to the empty string.
    assert(String("&aaa;") == (parser->GetText(&str), str));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    return 0;
}

int CTest::testMissingEntitiesUsingNextToken(int argc, char* argv[])
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    testMissingEntitiesUsingNextToken(parser);
    return 0;
}

int CTest::testMissingEntitiesUsingNextTokenWithRelaxed(int argc, char* argv[])
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    parser->SetFeature(String("http://xmlpull.org/v1/doc/features.html#relaxed"), true);
    testMissingEntitiesUsingNextToken(parser);
    return 0;
}

int CTest::testEntityInAttributeUsingNextToken(int argc, char* argv[])
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    AutoPtr<IStringReader> sr;
    CStringReader::New(String("<foo bar=\"&amp;\"></foo>"), (IStringReader**)&sr);
    parser->SetInput(sr);
    Int32 value = 0;
    String str("");
    assert(IXmlPullParser::START_TAG == (parser->NextToken(&value), value));
    assert(String("foo") == (parser->GetName(&str), str));
    assert(String("&") == (parser->GetAttributeValueEx(String(NULL), String("bar"), &str), str));
    return 0;
}

int CTest::testMissingEntitiesInAttributesUsingNext(int argc, char* argv[])
{
    assertParseFailure(String("<foo b='&aaa;'></foo>"));
    return 0;
}

int CTest::testMissingEntitiesInAttributesUsingNextWithRelaxed(int argc, char* argv[])
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    AutoPtr<IStringReader> sr;
    CStringReader::New(String("<foo b='&aaa;'></foo>"), (IStringReader**)&sr);
    parser->SetInput(sr);
    Int32 value = 0;
    String str("");
    parser->SetFeature(String("http://xmlpull.org/v1/doc/features.html#relaxed"), true);
    assert(IXmlPullParser::START_TAG == (parser->NextToken(&value), value));
    assert(1 == (parser->GetAttributeCount(&value), value));
    assert(String("b") == (parser->GetAttributeName(0, &str), str));
    // Expected unresolved entities to be left in-place. The old parser
    // would resolve these to the empty string.
    assert(String("&aaa;") == (parser->GetAttributeValue(0, &str), str));
    return 0;
}

int CTest::testMissingEntitiesInAttributesUsingNextToken(int argc, char* argv[])
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    AutoPtr<IStringReader> sr;
    CStringReader::New(String("<foo b='&aaa;'></foo>"), (IStringReader**)&sr);
    parser->SetInput(sr);
    testMissingEntitiesInAttributesUsingNextToken(parser);
    return 0;
}

int CTest::testMissingEntitiesInAttributesUsingNextTokenWithRelaxed(int argc, char* argv[])
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    AutoPtr<IStringReader> sr;
    CStringReader::New(String("<foo b='&aaa;'></foo>"), (IStringReader**)&sr);
    parser->SetInput(sr);
    parser->SetFeature(String("http://xmlpull.org/v1/doc/features.html#relaxed"), true);
    testMissingEntitiesInAttributesUsingNextToken(parser);
    return 0;
}

int CTest::testGreaterThanInText(int argc, char* argv[])
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    AutoPtr<IStringReader> sr;
    CStringReader::New(String("<foo>></foo>"), (IStringReader**)&sr);
    parser->SetInput(sr);
    Int32 value = 0;
    String str("");
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::TEXT == (parser->Next(&value), value));
    assert(String(">") == (parser->GetText(&str), str));
    return 0;
}

int CTest::testGreaterThanInAttribute(int argc, char* argv[])
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    AutoPtr<IStringReader> sr;
    CStringReader::New(String("<foo a='>'></foo>"), (IStringReader**)&sr);
    parser->SetInput(sr);
    Int32 value = 0;
    String str("");
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(String(">") == (parser->GetAttributeValue(0, &str), str));
    return 0;
}

int CTest::testLessThanInText(int argc, char* argv[])
{
    assertParseFailure(String("<foo><</foo>"));
    return 0;
}

int CTest::testLessThanInAttribute(int argc, char* argv[])
{
    assertParseFailure(String("<foo a='<'></foo>"));
    return 0;
}

int CTest::testQuotesInAttribute(int argc, char* argv[])
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    AutoPtr<IStringReader> sr;
    CStringReader::New(String("<foo a='\"' b=\"'\"></foo>"), (IStringReader**)&sr);
    parser->SetInput(sr);
    Int32 value = 0;
    String str("");
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(String("\"") == (parser->GetAttributeValue(0, &str), str));
    assert(String("'") == (parser->GetAttributeValue(1, &str), str));
    return 0;
}

int CTest::testQuotesInText(int argc, char* argv[])
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    AutoPtr<IStringReader> sr;
    CStringReader::New(String("<foo>\" '</foo>"), (IStringReader**)&sr);
    parser->SetInput(sr);
    Int32 value = 0;
    String str("");
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::TEXT == (parser->Next(&value), value));
    assert(String("\" '") == (parser->GetText(&str), str));
    return 0;
}

int CTest::testCdataDelimiterInAttribute(int argc, char* argv[])
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    AutoPtr<IStringReader> sr;
    CStringReader::New(String("<foo a=']]>'></foo>"), (IStringReader**)&sr);
    parser->SetInput(sr);
    Int32 value = 0;
    String str("");
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(String("]]>") == (parser->GetAttributeValue(0, &str), str));
    return 0;
}

int CTest::testCdataDelimiterInText(int argc, char* argv[])
{
    assertParseFailure(String("<foo>]]></foo>"));
    return 0;
}

int CTest::testUnexpectedEof(int argc, char* argv[])
{
    assertParseFailure(String("<foo><![C"));
    return 0;
}

int CTest::testUnexpectedSequence(int argc, char* argv[])
{
    assertParseFailure(String("<foo><![Cdata[bar]]></foo>"));
    return 0;
}

int CTest::testThreeDashCommentDelimiter(int argc, char* argv[])
{
    assertParseFailure(String("<foo><!--a---></foo>"));
    return 0;
}

int CTest::testTwoDashesInComment(int argc, char* argv[])
{
    assertParseFailure(String("<foo><!-- -- --></foo>"));
    return 0;
}

int CTest::testEmptyComment(int argc, char* argv[])
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    AutoPtr<IStringReader> sr;
    CStringReader::New(String("<foo><!----></foo>"), (IStringReader**)&sr);
    parser->SetInput(sr);
    Int32 value = 0;
    String str("");
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::COMMENT == (parser->NextToken(&value), value));
    assert(String("") == (parser->GetText(&str), str));
    return 0;
}

int CTest::testManyCloseBraces(int argc, char* argv[])
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    AutoPtr<IStringReader> sr;
    CStringReader::New(String("<foo>]]]]]]]]]]]]]]]]]]]]]]]</foo>"), (IStringReader**)&sr);
    parser->SetInput(sr);
    Int32 value = 0;
    String str("");
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::TEXT == (parser->Next(&value), value));
    assert(String("]]]]]]]]]]]]]]]]]]]]]]]") == (parser->GetText(&str), str));
    return 0;
}

int CTest::testCommentUsingNext(int argc, char* argv[])
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    AutoPtr<IStringReader> sr;
    CStringReader::New(String("<foo>ab<!-- comment! -->cd</foo>"), (IStringReader**)&sr);
    parser->SetInput(sr);
    Int32 value = 0;
    String str("");
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::TEXT == (parser->Next(&value), value));
    assert(String("abcd") == (parser->GetText(&str), str));
    return 0;
}

int CTest::testCommentUsingNextToken(int argc, char* argv[])
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    AutoPtr<IStringReader> sr;
    CStringReader::New(String("<foo>ab<!-- comment! -->cd</foo>"), (IStringReader**)&sr);
    parser->SetInput(sr);
    Int32 value = 0;
    String str("");
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::TEXT == (parser->NextToken(&value), value));
    assert(String("ab") == (parser->GetText(&str), str));
    assert(IXmlPullParser::COMMENT == (parser->NextToken(&value), value));
    assert(String(" comment! ") == (parser->GetText(&str), str));
    assert(IXmlPullParser::TEXT == (parser->NextToken(&value), value));
    assert(String("cd") == (parser->GetText(&str), str));
    return 0;
}

int CTest::testCdataUsingNext(int argc, char* argv[])
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    AutoPtr<IStringReader> sr;
    CStringReader::New(String("<foo>ab<![CDATA[cdef]]gh&amp;i]]>jk</foo>"), (IStringReader**)&sr);
    parser->SetInput(sr);
    Int32 value = 0;
    String str("");
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::TEXT == (parser->Next(&value), value));
    assert(String("abcdef]]gh&amp;ijk") == (parser->GetText(&str), str));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    return 0;
}

int CTest::testCdataUsingNextToken(int argc, char* argv[])
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    AutoPtr<IStringReader> sr;
    CStringReader::New(String("<foo>ab<![CDATA[cdef]]gh&amp;i]]>jk</foo>"), (IStringReader**)&sr);
    parser->SetInput(sr);
    Int32 value = 0;
    String str("");
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::TEXT == (parser->NextToken(&value), value));
    assert(String("ab") == (parser->GetText(&str), str));
    assert(IXmlPullParser::CDSECT == (parser->NextToken(&value), value));
    assert(String("cdef]]gh&amp;i") == (parser->GetText(&str), str));
    assert(IXmlPullParser::TEXT == (parser->NextToken(&value), value));
    assert(String("jk") == (parser->GetText(&str), str));
    assert(IXmlPullParser::END_TAG == (parser->NextToken(&value), value));
    return 0;
}

int CTest::testEntityLooksLikeCdataClose(int argc, char* argv[])
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    AutoPtr<IStringReader> sr;
    CStringReader::New(String("<foo>&#93;&#93;></foo>"), (IStringReader**)&sr);
    parser->SetInput(sr);
    Int32 value = 0;
    String str("");
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::TEXT == (parser->Next(&value), value));
    assert(String("]]>") == (parser->GetText(&str), str));
    return 0;
}

int CTest::testProcessingInstructionUsingNext(int argc, char* argv[])
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    AutoPtr<IStringReader> sr;
    CStringReader::New(String("<foo>ab<?cd efg hij?>kl</foo>"), (IStringReader**)&sr);
    parser->SetInput(sr);
    Int32 value = 0;
    String str("");
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::TEXT == (parser->Next(&value), value));
    assert(String("abkl") == (parser->GetText(&str), str));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    return 0;
}

int CTest::testProcessingInstructionUsingNextToken(int argc, char* argv[])
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    AutoPtr<IStringReader> sr;
    CStringReader::New(String("<foo>ab<?cd efg hij?>kl</foo>"), (IStringReader**)&sr);
    parser->SetInput(sr);
    Int32 value = 0;
    String str("");
    assert(IXmlPullParser::START_TAG == (parser->NextToken(&value), value));
    assert(IXmlPullParser::TEXT == (parser->NextToken(&value), value));
    assert(String("ab") == (parser->GetText(&str), str));
    assert(IXmlPullParser::PROCESSING_INSTRUCTION == (parser->NextToken(&value), value));
    assert(String("cd efg hij") == (parser->GetText(&str), str));
    assert(IXmlPullParser::TEXT == (parser->NextToken(&value), value));
    assert(String("kl") == (parser->GetText(&str), str));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    return 0;
}

int CTest::testWhitespaceUsingNextToken(int argc, char* argv[])
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    AutoPtr<IStringReader> sr;
    CStringReader::New(String("  \n  <foo> \n </foo>   \n   "), (IStringReader**)&sr);
    parser->SetInput(sr);
    Int32 value = 0;
    String str("");
    assert(IXmlPullParser::IGNORABLE_WHITESPACE == (parser->NextToken(&value), value));
    Boolean outbool = FALSE;
    assert(true == (parser->IsWhitespace(&outbool), outbool));
    assert(String("  \n  ") == (parser->GetText(&str), str));
    assert(IXmlPullParser::START_TAG == (parser->NextToken(&value), value));
    assert(IXmlPullParser::TEXT == (parser->NextToken(&value), value));
    assert(true == (parser->IsWhitespace(&outbool), outbool));
    assert(String(" \n ") == (parser->GetText(&str), str));
    assert(IXmlPullParser::END_TAG == (parser->NextToken(&value), value));
    assert(IXmlPullParser::IGNORABLE_WHITESPACE == (parser->NextToken(&value), value));
    assert(true == (parser->IsWhitespace(&outbool), outbool));
    assert(String("   \n   ") == (parser->GetText(&str), str));
    assert(IXmlPullParser::END_DOCUMENT == (parser->NextToken(&value), value));
    return 0;
}

int CTest::testLinesAndColumns(int argc, char* argv[])
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    AutoPtr<IStringReader> sr;
    CStringReader::New(String("\n")
                     + String("  <foo><bar a='\n")
                     + String("' b='cde'></bar\n")
                     + String("><!--\n")
                     + String("\n")
                     + String("--><baz/>fg\n")
                     + String("</foo>"), (IStringReader**)&sr);
    parser->SetInput(sr);
    Int32 value = 0;
    String str("");
    assert(String("1,1") == (parser->GetLineNumber(&value), StringUtils::Int32ToString(value))
                 + String(",")
                 + (parser->GetColumnNumber(&value), StringUtils::Int32ToString(value)));
    assert(IXmlPullParser::IGNORABLE_WHITESPACE == (parser->NextToken(&value), value));
    assert(String("2,3") == (parser->GetLineNumber(&value), StringUtils::Int32ToString(value))
                 + String(",")
                 + (parser->GetColumnNumber(&value), StringUtils::Int32ToString(value)));
    assert(IXmlPullParser::START_TAG == (parser->NextToken(&value), value));
    assert(String("2,8") == (parser->GetLineNumber(&value), StringUtils::Int32ToString(value))
                 + String(",")
                 + (parser->GetColumnNumber(&value), StringUtils::Int32ToString(value)));
    assert(IXmlPullParser::START_TAG == (parser->NextToken(&value), value));
    assert(String("3,11") == (parser->GetLineNumber(&value), StringUtils::Int32ToString(value))
                 + String(",")
                 + (parser->GetColumnNumber(&value), StringUtils::Int32ToString(value)));
    assert(IXmlPullParser::END_TAG == (parser->NextToken(&value), value));
    assert(String("4,2") == (parser->GetLineNumber(&value), StringUtils::Int32ToString(value))
                 + String(",")
                 + (parser->GetColumnNumber(&value), StringUtils::Int32ToString(value)));
    assert(IXmlPullParser::COMMENT == (parser->NextToken(&value), value));
    assert(String("6,4") == (parser->GetLineNumber(&value), StringUtils::Int32ToString(value))
                 + String(",")
                 + (parser->GetColumnNumber(&value), StringUtils::Int32ToString(value)));
    assert(IXmlPullParser::START_TAG == (parser->NextToken(&value), value));
    assert(String("6,10") == (parser->GetLineNumber(&value), StringUtils::Int32ToString(value))
                 + String(",")
                 + (parser->GetColumnNumber(&value), StringUtils::Int32ToString(value)));
    assert(IXmlPullParser::END_TAG == (parser->NextToken(&value), value));
    assert(String("6,10") == (parser->GetLineNumber(&value), StringUtils::Int32ToString(value))
                 + String(",")
                 + (parser->GetColumnNumber(&value), StringUtils::Int32ToString(value)));
    assert(IXmlPullParser::TEXT == (parser->NextToken(&value), value));
    assert(String("7,1") == (parser->GetLineNumber(&value), StringUtils::Int32ToString(value))
                 + String(",")
                 + (parser->GetColumnNumber(&value), StringUtils::Int32ToString(value)));
    assert(IXmlPullParser::END_TAG == (parser->NextToken(&value), value));
    assert(String("7,7") == (parser->GetLineNumber(&value), StringUtils::Int32ToString(value))
                 + String(",")
                 + (parser->GetColumnNumber(&value), StringUtils::Int32ToString(value)));
    assert(IXmlPullParser::END_DOCUMENT == (parser->NextToken(&value), value));
    assert(String("7,7") == (parser->GetLineNumber(&value), StringUtils::Int32ToString(value))
                 + String(",")
                 + (parser->GetColumnNumber(&value), StringUtils::Int32ToString(value)));
    return 0;
}

int CTest::testEmptyEntityReferenceUsingNext(int argc, char* argv[])
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    AutoPtr<IStringReader> sr;
    CStringReader::New(String("<foo>&empty;</foo>"), (IStringReader**)&sr);
    parser->SetInput(sr);
    Int32 value = 0;
    String str("");
    parser->DefineEntityReplacementText(String("empty"), String(""));
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    return 0;
}

int CTest::testEmptyEntityReferenceUsingNextToken(int argc, char* argv[])
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    AutoPtr<IStringReader> sr;
    CStringReader::New(String("<foo>&empty;</foo>"), (IStringReader**)&sr);
    parser->SetInput(sr);
    Int32 value = 0;
    String str("");
    parser->DefineEntityReplacementText(String("empty"), String(""));
    assert(IXmlPullParser::START_TAG == (parser->NextToken(&value), value));
    assert(IXmlPullParser::ENTITY_REF == (parser->NextToken(&value), value));
    assert(String("empty") == (parser->GetName(&str), str));
    assert(String("") == (parser->GetText(&str), str));
    assert(IXmlPullParser::END_TAG == (parser->NextToken(&value), value));
    return 0;
}

int CTest::testEmptyCdataUsingNext(int argc, char* argv[])
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    AutoPtr<IStringReader> sr;
    CStringReader::New(String("<foo><![CDATA[]]></foo>"), (IStringReader**)&sr);
    parser->SetInput(sr);
    Int32 value = 0;
    String str("");
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    return 0;
}

int CTest::testEmptyCdataUsingNextToken(int argc, char* argv[])
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    AutoPtr<IStringReader> sr;
    CStringReader::New(String("<foo><![CDATA[]]></foo>"), (IStringReader**)&sr);
    parser->SetInput(sr);
    Int32 value = 0;
    String str("");
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::CDSECT == (parser->NextToken(&value), value));
    assert(String("") == (parser->GetText(&str), str));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    return 0;
}

int CTest::testParseReader(int argc, char* argv[])
{
    String snippet = String("<dagny dad=\"bob\">hello</dagny>");
    AutoPtr<IXmlPullParser> parser = newPullParser();
    parser->SetFeature(FEATURE_PROCESS_NAMESPACES, false);
    AutoPtr<IStringReader> sr;
    CStringReader::New(snippet, (IStringReader**)&sr);
    parser->SetInput(sr);
    validate(parser);
    return 0;
}

int CTest::testParseInputStream(int argc, char* argv[])
{
    String snippet = String("<dagny dad=\"bob\">hello</dagny>");
    AutoPtr<IXmlPullParser> parser = newPullParser();
    parser->SetFeature(FEATURE_PROCESS_NAMESPACES, false);
    AutoPtr<ArrayOf<Char32> > gchars = snippet.GetChars();
    AutoPtr< ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(gchars->GetLength());
    for (int i = 0; i < snippet.GetLength(); ++i) {
        (*buffer)[i] = (*gchars)[i] & 0xFF;
    }
    AutoPtr<IByteArrayInputStream> binput;
    CByteArrayInputStream::New(buffer, (IByteArrayInputStream**)&binput);
    parser->SetInputEx(binput, String("UTF-8"));
    validate(parser);
    return 0;
}

int CTest::testNextAfterEndDocument(int argc, char* argv[])
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    AutoPtr<IStringReader> sr;
    CStringReader::New(String("<foo></foo>"), (IStringReader**)&sr);
    parser->SetInput(sr);
    Int32 value = 0;
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::END_DOCUMENT == (parser->Next(&value), value));
    assert(IXmlPullParser::END_DOCUMENT == (parser->Next(&value), value));
    return 0;
}

int CTest::testNamespaces(int argc, char* argv[])
{
    String xml = String("<one xmlns='ns:default' xmlns:n1='ns:1' a='b'>\n")
               + String("  <n1:two c='d' n1:e='f' xmlns:n2='ns:2'>text</n1:two>\n")
               + String("</one>");

    AutoPtr<IXmlPullParser> parser = newPullParser();
    parser->SetFeature(FEATURE_PROCESS_NAMESPACES, true);
    AutoPtr<IStringReader> sr;
    CStringReader::New(xml, (IStringReader**)&sr);
    parser->SetInput(sr);
    Int32 value = 0;
    String str("");

    assert(0 == (parser->GetDepth(&value), value));
    assert(0 == (parser->GetNamespaceCount(0, &value), value));

    // try {
    ECode ec = parser->GetNamespaceCount(1, &value);
    if (ec != NOERROR) {
        printf("fail is expected ~~~~\n");
    }
        // fail();
    // } catch (IndexOutOfBoundsException e) { /* expected */ }

    // one
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(1 == (parser->GetDepth(&value), value));

    checkNamespacesInOne(parser);

    // n1:two
    assert(IXmlPullParser::START_TAG == (parser->NextTag(&value), value));

    assert(2 == (parser->GetDepth(&value), value));
    checkNamespacesInTwo(parser);

    // Body of two.
    assert(IXmlPullParser::TEXT == (parser->Next(&value), value));

    // End of two.
    assert(IXmlPullParser::END_TAG == (parser->NextTag(&value), value));

    // Depth should still be 2.
    assert(2 == (parser->GetDepth(&value), value));

    // We should still be able to see the namespaces from two.
    checkNamespacesInTwo(parser);

    // End of one.
    assert(IXmlPullParser::END_TAG == (parser->NextTag(&value), value));

    // Depth should be back to 1.
    assert(1 == (parser->GetDepth(&value), value));

    // We can still see the namespaces in one.
    checkNamespacesInOne(parser);

    // We shouldn't be able to see the namespaces in two anymore.
    // try {
        ec = parser->GetNamespaceCount(2, &value);
        if (ec != NOERROR) {
            printf("fail is expected ~~~~\n");
        }
        // fail();
    // } catch (IndexOutOfBoundsException e) { /* expected */ }

    assert(IXmlPullParser::END_DOCUMENT == (parser->Next(&value), value));

    // We shouldn't be able to see the namespaces in one anymore.
    // try {
        parser->GetNamespaceCount(1, &value);
        if (ec != NOERROR) {
            printf("fail is expected ~~~~\n");
        }
        // fail();
    // } catch (IndexOutOfBoundsException e) { /* expected */ }

    assert(0 == (parser->GetNamespaceCount(0, &value), value));
    return 0;
}

int CTest::testTextBeforeDocumentElement(int argc, char* argv[])
{
    assertParseFailure(String("not xml<foo/>"));
    return 0;
}

int CTest::testTextAfterDocumentElement(int argc, char* argv[])
{
    assertParseFailure(String("<foo/>not xml"));
    return 0;
}

int CTest::testTextNoDocumentElement(int argc, char* argv[])
{
    assertParseFailure(String("not xml"));
    return 0;
}

int CTest::testBomAndByteInput(int argc, char* argv[])
{
    String xml = String("\ufeff<?xml version='1.0'?><input/>");
    const char * aaaaa = xml.string();
    AutoPtr< ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(xml.GetByteLength());
    for (int i = 0; i < xml.GetByteLength(); ++i) {
        (*buffer)[i] = aaaaa[i];
    }
    AutoPtr<IByteArrayInputStream> binput;
    CByteArrayInputStream::New(buffer, (IByteArrayInputStream**)&binput);
    AutoPtr<IXmlPullParser> parser = newPullParser();
    parser->SetInputEx(binput, String(NULL));
    Int32 value = 0;
    String str("");
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(String("input") == (parser->GetName(&str), str));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    assert(String("input") == (parser->GetName(&str), str));
    assert(IXmlPullParser::END_DOCUMENT == (parser->Next(&value), value));
    return 0;
}

int CTest::testBomAndByteInputWithExplicitCharset(int argc, char* argv[])
{
    String xml = String("\ufeff<?xml version='1.0'?><input/>");
    const char * aaaaa = xml.string();
    AutoPtr< ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(xml.GetByteLength());
    for (int i = 0; i < xml.GetByteLength(); ++i) {
        (*buffer)[i] = aaaaa[i];
    }
    AutoPtr<IByteArrayInputStream> binput;
    CByteArrayInputStream::New(buffer, (IByteArrayInputStream**)&binput);
    AutoPtr<IXmlPullParser> parser = newPullParser();
    parser->SetInputEx(binput, String("UTF-8"));
    Int32 value = 0;
    String str("");
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(String("input") == (parser->GetName(&str), str));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    assert(String("input") == (parser->GetName(&str), str));
    assert(IXmlPullParser::END_DOCUMENT == (parser->Next(&value), value));
    return 0;
}

int CTest::testBomAndCharacterInput(int argc, char* argv[])
{
    assertParseFailure(String("\ufeff<?xml version='1.0'?><input/>"));
    return 0;
}

int CTest::testNextTextAdvancesToEndTag(int argc, char* argv[])
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    AutoPtr<IStringReader> sr;
    CStringReader::New(String("<foo>bar</foo>"), (IStringReader**)&sr);
    parser->SetInput(sr);
    Int32 value = 0;
    String str("");
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(String("bar") == (parser->NextText(&str), str));
    assert(IXmlPullParser::END_TAG == (parser->GetEventType(&value), value));
    return 0;
}

int CTest::testNextTag(int argc, char* argv[])
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    AutoPtr<IStringReader> sr;
    CStringReader::New(String("<foo> <bar></bar> </foo>"), (IStringReader**)&sr);
    parser->SetInput(sr);
    Int32 value = 0;
    String str("");
    assert(IXmlPullParser::START_TAG == (parser->NextTag(&value), value));
    assert(String("foo") == (parser->GetName(&str), str));
    assert(IXmlPullParser::START_TAG == (parser->NextTag(&value), value));
    assert(String("bar") == (parser->GetName(&str), str));
    assert(IXmlPullParser::END_TAG == (parser->NextTag(&value), value));
    assert(String("bar") == (parser->GetName(&str), str));
    assert(IXmlPullParser::END_TAG == (parser->NextTag(&value), value));
    assert(String("foo") == (parser->GetName(&str), str));
    return 0;
}


void CTest::testNamespace(AutoPtr<IXmlPullParser>  parser)
{
    Int32 value = 0;
    String str("");
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(String("http://foo") == (parser->GetNamespaceEx(&str), str));
    assert(String("a") == (parser->GetName(&str), str));
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(String("http://bar") == (parser->GetNamespaceEx(&str), str));
    assert(String("b") == (parser->GetName(&str), str));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    assert(String("http://bar") == (parser->GetNamespaceEx(&str), str));
    assert(String("b") == (parser->GetName(&str), str));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    assert(String("http://foo") == (parser->GetNamespaceEx(&str), str));
    assert(String("a") == (parser->GetName(&str), str));
}

void CTest::testCharacterReferenceOfUtf16Surrogates(String xml)
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    AutoPtr<IStringReader> sr;
    CStringReader::New(xml, (IStringReader**)&sr);
    parser->SetInput(sr);
    Int32 value = 0;
    String str("");
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::TEXT == (parser->Next(&value), value));
    // assert(new String(new int[]{65536, ' ', 66433, ' ', 1114096}, 0, 5),
    //         parser.getText());
    AutoPtr< ArrayOf<Char32> > array = ArrayOf<Char32>::Alloc(5);
    (*array)[0] = 65536;
    (*array)[1] = ' ';
    (*array)[2] = 66433;
    (*array)[3] = ' ';
    (*array)[4] = 1114096;
    String substr(*array, 0, 5);
    assert(substr == (parser->GetText(&str), str));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
}

void CTest::testLineEndings(String xml)
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    AutoPtr<IStringReader> sr;
    CStringReader::New(xml, (IStringReader**)&sr);
    parser->SetInput(sr);
    Int32 value = 0;
    String str("");
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(String("foo") == (parser->GetName(&str), str));
    assert(String("b c") == (parser->GetAttributeValue(0, &str), str));
    assert(IXmlPullParser::TEXT == (parser->Next(&value), value));
    assert(String("d\ne") == (parser->GetText(&str), str));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    assert(String("foo") == (parser->GetName(&str), str));
    assert(IXmlPullParser::END_DOCUMENT == (parser->Next(&value), value));
}

void CTest::testMissingEntitiesUsingNextToken(AutoPtr<IXmlPullParser> parser)
{
    AutoPtr<IStringReader> sr;
    CStringReader::New(String("<foo>&aaa;</foo>"), (IStringReader**)&sr);
    parser->SetInput(sr);
    Int32 value = 0;
    String str("");
    assert(IXmlPullParser::START_TAG == (parser->NextToken(&value), value));
    assert(IXmlPullParser::ENTITY_REF == (parser->NextToken(&value), value));
    assert(String("aaa") == (parser->GetName(&str), str));
    assert((parser->GetText(&str), str).IsNull());
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
}

void CTest::testMissingEntitiesInAttributesUsingNextToken(AutoPtr<IXmlPullParser> parser)
{
    Int32 value = 0;
    String str("");
    assert(IXmlPullParser::START_TAG == (parser->NextToken(&value), value));
    assert(1 == (parser->GetAttributeCount(&value), value));
    assert(String("b") == (parser->GetAttributeName(0, &str), str));
    // Expected unresolved entities to be left in-place. The old parser
    // would resolve these to the empty string.
    assert(String("&aaa;") == (parser->GetAttributeValue(0, &str), str));
}

void CTest::validate(AutoPtr<IXmlPullParser> parser)
{
    Int32 value = 0;
    String str("");
    assert(IXmlPullParser::START_DOCUMENT == (parser->GetEventType(&value), value));
    assert(0 == (parser->GetDepth(&value), value));
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(1 == (parser->GetDepth(&value), value));
    assert(String("dagny") == (parser->GetName(&str), str));
    assert(1 == (parser->GetAttributeCount(&value), value));
    assert(String("dad") == (parser->GetAttributeName(0, &str), str));
    assert(String("bob") == (parser->GetAttributeValue(0, &str), str));
    assert(String("bob") == (parser->GetAttributeValueEx(String(NULL), String("dad"), &str), str));
    assert(IXmlPullParser::TEXT == (parser->Next(&value), value));
    assert(1 == (parser->GetDepth(&value), value));
    assert(String("hello") == (parser->GetText(&str), str));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    assert(1 == (parser->GetDepth(&value), value));
    assert(String("dagny") == (parser->GetName(&str), str));
    assert(IXmlPullParser::END_DOCUMENT == (parser->Next(&value), value));
    assert(0 == (parser->GetDepth(&value), value));
}

void CTest::checkNamespacesInOne(AutoPtr<IXmlPullParser> parser)
{
    Int32 value = 0;
    String str("");
    assert(2 == (parser->GetNamespaceCount(1, &value), value));

    // Prefix for default namespace is null.
    assert((parser->GetNamespacePrefix(0, &str), str).IsNull());
    assert(String("ns:default") == (parser->GetNamespaceUri(0, &str), str));

    assert(String("n1") == (parser->GetNamespacePrefix(1, &str), str));
    assert(String("ns:1") == (parser->GetNamespaceUri(1, &str), str));

    assert(String("ns:default") == (parser->GetNamespace(String(NULL), &str), str));

    // KXML returns null.
    // assert("ns:default", parser.getNamespace(""));
}

void CTest::checkNamespacesInTwo(AutoPtr<IXmlPullParser> parser)
{
    Int32 value = 0;
    String str("");

    // These should still be valid.
    checkNamespacesInOne(parser);

    assert(3 == (parser->GetNamespaceCount(2, &value), value));

    // Default ns should still be in the stack
    assert((parser->GetNamespacePrefix(0, &str), str).IsNull());
    assert(String("ns:default") == (parser->GetNamespaceUri(0, &str), str));
}

void CTest::assertParseFailure(String xml)
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    AutoPtr<IStringReader> sr;
    CStringReader::New(xml, (IStringReader**)&sr);
    parser->SetInput(sr);
    Int32 value = 0;
    ECode ec = NOERROR;
    // try {
        while ((ec = parser->Next(&value), value) != IXmlPullParser::END_DOCUMENT && ec == NOERROR) {
        }
        if (ec != NOERROR) {
            printf("fail expected ~~~~~~~~\n");
        }
        // fail();
    // } catch (XmlPullParserException expected) {
    // }
}

AutoPtr<IXmlPullParser> KmlCtest::newPullParser()
{
    AutoPtr<IKXmlParser> xpp;
    CKXmlParser::New((IKXmlParser**)&xpp);
    return xpp;
}
