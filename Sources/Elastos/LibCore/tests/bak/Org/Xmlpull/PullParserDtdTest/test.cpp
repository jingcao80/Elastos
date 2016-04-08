#include "test.h"

const int CTest::READ_BUFFER_SIZE;

int CTest::testDeclaringParameterEntities(int argc, char* argv[])
{
    String xml = String("<!DOCTYPE foo [")
               + String("  <!ENTITY % a \"android\">")
               + String("]><foo></foo>");
    AutoPtr<IXmlPullParser> parser = newPullParser(xml);
    Int32 value = 0;
    while ((parser->Next(&value), value) != IXmlPullParser::END_DOCUMENT) {
        PFL_EX("value: %d", value);
    }
    return 0;
}

int CTest::testUsingParameterEntitiesInDtds(int argc, char* argv[])
{
    assertParseFailure(String("<!DOCTYPE foo [")
                     + String("  <!ENTITY % a \"android\">")
                     + String("  <!ENTITY b \"%a;\">")
                     + String("]><foo></foo>"));
    return 0;
}

int CTest::testUsingParameterInDocuments(int argc, char* argv[])
{
    assertParseFailure(String("<!DOCTYPE foo [")
                     + String("  <!ENTITY % a \"android\">")
                     + String("]><foo>&a;</foo>"));
    return 0;
}

int CTest::testGeneralAndParameterEntityWithTheSameName(int argc, char* argv[])
{
    String xml = String("<!DOCTYPE foo [")
               + String("  <!ENTITY a \"aaa\">")
               + String("  <!ENTITY % a \"bbb\">")
               + String("]><foo>&a;</foo>");
    AutoPtr<IXmlPullParser> parser = newPullParser(xml);
    Int32 value = 0;
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::TEXT == (parser->Next(&value), value));
    String str("");
    assert(String("aaa") == (parser->GetText(&str), str));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::END_DOCUMENT == (parser->Next(&value), value));
    return 0;
}

int CTest::testInternalEntities(int argc, char* argv[])
{
    String xml = String("<!DOCTYPE foo [")
               + String("  <!ENTITY a \"android\">")
               + String("]><foo>&a;</foo>");
    AutoPtr<IXmlPullParser> parser = newPullParser(xml);
    Int32 value = 0;
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::TEXT == (parser->Next(&value), value));
    String str("");
    assert(String("android") == (parser->GetText(&str), str));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::END_DOCUMENT == (parser->Next(&value), value));
    return 0;
}

int CTest::testExternalDtdIsSilentlyIgnored(int argc, char* argv[])
{
    String xml = String("<!DOCTYPE foo SYSTEM \"http://127.0.0.1:1/no-such-file.dtd\"><foo></foo>");
    AutoPtr<IXmlPullParser> parser = newPullParser(xml);
    Int32 value = 0;
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    String str("");
    assert(String("foo") == (parser->GetName(&str), str));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    assert(String("foo") == (parser->GetName(&str), str));
    assert(IXmlPullParser::END_DOCUMENT == (parser->Next(&value), value));
    return 0;
}

int CTest::testExternalAndInternalDtd(int argc, char* argv[])
{
    String xml = String("<!DOCTYPE foo SYSTEM \"http://127.0.0.1:1/no-such-file.dtd\" [")
               + String("  <!ENTITY a \"android\">")
               + String("]><foo>&a;</foo>");
    AutoPtr<IXmlPullParser> parser = newPullParser(xml);
    Int32 value = 0;
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::TEXT == (parser->Next(&value), value));
    String str("");
    assert(String("android") == (parser->GetText(&str), str));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::END_DOCUMENT == (parser->Next(&value), value));
    return 0;
}

int CTest::testInternalEntitiesAreParsed(int argc, char* argv[])
{
    String xml = String("<!DOCTYPE foo [")
               + String("  <!ENTITY a \"&#38;#65;\">") // &#38; expands to '&', &#65; expands to 'A'
               + String("]><foo>&a;</foo>");
    AutoPtr<IXmlPullParser> parser = newPullParser(xml);
    Int32 value = 0;
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::TEXT == (parser->Next(&value), value));
    String str("");
    assert(String("A") == (parser->GetText(&str), str));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::END_DOCUMENT == (parser->Next(&value), value));
    return 0;
}

int CTest::testFoolishlyRecursiveInternalEntities(int argc, char* argv[])
{
    String xml = String("<!DOCTYPE foo [")
               + String("  <!ENTITY a \"&#38;#38;#38;#38;\">") // expand &#38; to '&' only twice
               + String("]><foo>&a;</foo>");
    AutoPtr<IXmlPullParser> parser = newPullParser(xml);
    Int32 value = 0;
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::TEXT == (parser->Next(&value), value));
    String str("");
    assert(String("&#38;#38;") == (parser->GetText(&str), str));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::END_DOCUMENT == (parser->Next(&value), value));
    return 0;
}

int CTest::testExpansionOfEntityAndCharacterReferences(int argc, char* argv[])
{
    String xml = String("<!DOCTYPE foo [")
               + String("<!ENTITY example \"<p>An ampersand (&#38;#38;) may be escaped\n")
               + String("numerically (&#38;#38;#38;) or with a general entity\n")
               + String("(&amp;amp;).</p>\" >")
               + String("]><foo>&example;</foo>");
    AutoPtr<IXmlPullParser> parser = newPullParser(xml);
    Int32 value = 0;
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    String str("");
    assert(String("p") == (parser->GetName(&str), str));
    assert(IXmlPullParser::TEXT == (parser->Next(&value), value));
    assert(String("An ampersand (&) may be escaped\n")
         + String("numerically (&#38;) or with a general entity\n")
         + String("(&amp;).") == (parser->GetText(&str), str));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    assert(String("p") == (parser->GetName(&str), str));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::END_DOCUMENT == (parser->Next(&value), value));
    return 0;
}

int CTest::testGeneralEntitiesAreStoredUnresolved(int argc, char* argv[])
{
    String xml = String("<!DOCTYPE foo [")
               + String("<!ENTITY b \"&a;\" >")
               + String("<!ENTITY a \"android\" >")
               + String("]><foo>&a;</foo>");
    AutoPtr<IXmlPullParser> parser = newPullParser(xml);
    Int32 value = 0;
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::TEXT == (parser->Next(&value), value));
    String str("");
    assert(String("android") == (parser->GetText(&str), str));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::END_DOCUMENT == (parser->Next(&value), value));
    return 0;
}

int CTest::testStructuredEntityAndNextToken(int argc, char* argv[])
{
    String xml = String("<!DOCTYPE foo [<!ENTITY bb \"<bar>baz<!--quux--></bar>\">]><foo>a&bb;c</foo>");
    AutoPtr<IXmlPullParser> parser = newPullParser(xml);
    Int32 value = 0;
    assert(IXmlPullParser::DOCDECL == (parser->NextToken(&value), value));
    assert(IXmlPullParser::START_TAG == (parser->NextToken(&value), value));
    String str("");
    assert(String("foo") == (parser->GetName(&str), str));
    assert(IXmlPullParser::TEXT == (parser->NextToken(&value), value));
    assert(String("a") == (parser->GetText(&str), str));
    assert(IXmlPullParser::ENTITY_REF == (parser->NextToken(&value), value));
    assert(String("bb") == (parser->GetName(&str), str));
    assert(String("") == (parser->GetText(&str), str));
    assert(IXmlPullParser::START_TAG == (parser->NextToken(&value), value));
    assert(String("bar") == (parser->GetName(&str), str));
    assert(IXmlPullParser::TEXT == (parser->NextToken(&value), value));
    assert(String("baz") == (parser->GetText(&str), str));
    assert(IXmlPullParser::COMMENT == (parser->NextToken(&value), value));
    assert(String("quux") == (parser->GetText(&str), str));
    assert(IXmlPullParser::END_TAG == (parser->NextToken(&value), value));
    assert(String("bar") == (parser->GetName(&str), str));
    assert(IXmlPullParser::TEXT == (parser->NextToken(&value), value));
    assert(String("c") == (parser->GetText(&str), str));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::END_DOCUMENT == (parser->Next(&value), value));
    return 0;
}

int CTest::testUsingExternalEntities(int argc, char* argv[])
{
    String xml = String("<!DOCTYPE foo [")
               + String("  <!ENTITY a SYSTEM \"http://localhost:1/no-such-file.xml\">")
               + String("]><foo>&a;</foo>");
    AutoPtr<IXmlPullParser> parser = newPullParser(xml);
    Int32 value = 0;
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    // &a; is dropped!
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::END_DOCUMENT == (parser->Next(&value), value));
    return 0;
}

int CTest::testExternalDtdAndMissingEntity(int argc, char* argv[])
{
    String xml = String("<!DOCTYPE foo SYSTEM \"http://127.0.0.1:1/no-such-file.dtd\">")
               + String("<foo>&a;</foo>");
    AutoPtr<IXmlPullParser> parser = newPullParser(xml);
    Int32 value = 0;
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::END_DOCUMENT == (parser->Next(&value), value));
    return 0;
}

int CTest::testExternalIdIsCaseSensitive(int argc, char* argv[])
{
    // The spec requires 'SYSTEM' in upper case
    assertParseFailure(String("<!DOCTYPE foo [")
                     + String("  <!ENTITY a system \"http://localhost:1/no-such-file.xml\">")
                     + String("]><foo/>"));
    return 0;
}

int CTest::testDtdDoesNotInformIgnorableWhitespace(int argc, char* argv[])
{
    String xml = String("<!DOCTYPE foo [\n")
               + String("  <!ELEMENT foo (bar)*>\n")
               + String("  <!ELEMENT bar ANY>\n")
               + String("]>")
               + String("<foo>  \n  <bar></bar>  \t  </foo>");
    AutoPtr<IXmlPullParser> parser = newPullParser(xml);
    Int32 value = 0;
    String str("");
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(String("foo") == (parser->GetName(&str), str));
    assert(IXmlPullParser::TEXT == (parser->Next(&value), value));
    assert(String("  \n  ") == (parser->GetText(&str), str));
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(String("bar") == (parser->GetName(&str), str));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    assert(String("bar") == (parser->GetName(&str), str));
    assert(IXmlPullParser::TEXT == (parser->Next(&value), value));
    assert(String("  \t  ") == (parser->GetText(&str), str));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::END_DOCUMENT == (parser->Next(&value), value));
    return 0;
}

int CTest::testEmptyDoesNotInformIgnorableWhitespace(int argc, char* argv[])
{
    String xml = String("<!DOCTYPE foo [\n")
               + String("  <!ELEMENT foo EMPTY>\n")
               + String("]>")
               + String("<foo>  \n  </foo>");
    Int32 value = 0;
    String str("");
    AutoPtr<IXmlPullParser> parser = newPullParser(xml);
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(String("foo") == (parser->GetName(&str), str));
    assert(IXmlPullParser::TEXT == (parser->Next(&value), value));
    assert(String("  \n  ") == (parser->GetText(&str), str));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::END_DOCUMENT == (parser->Next(&value), value));
    return 0;
}

int CTest::testAttributeOfTypeEntity(int argc, char* argv[])
{
    String xml = String("<!DOCTYPE foo [\n")
               + String("  <!ENTITY a \"android\">")
               + String("  <!ELEMENT foo ANY>\n")
               + String("  <!ATTLIST foo\n")
               + String("    bar ENTITY #IMPLIED>")
               + String("]>")
               + String("<foo bar=\"a\"></foo>");
    Int32 value = 0;
    String str("");
    AutoPtr<IXmlPullParser> parser = newPullParser(xml);
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(String("foo") == (parser->GetName(&str), str));
    assert(String("a") == (parser->GetAttributeValueEx(String(NULL), String("bar"), &str), str));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::END_DOCUMENT == (parser->Next(&value), value));
    return 0;
}

int CTest::testTagStructureNotValidated(int argc, char* argv[])
{
    String xml = String("<!DOCTYPE foo [\n")
               + String("  <!ELEMENT foo (bar)*>\n")
               + String("  <!ELEMENT bar ANY>\n")
               + String("  <!ELEMENT baz ANY>\n")
               + String("]>")
               + String("<foo><bar/><bar/><baz/></foo>");
    Int32 value = 0;
    String str("");
    AutoPtr<IXmlPullParser> parser = newPullParser(xml);
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(String("foo") == (parser->GetName(&str), str));
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(String("bar") == (parser->GetName(&str), str));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(String("bar") == (parser->GetName(&str), str));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(String("baz") == (parser->GetName(&str), str));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::END_DOCUMENT == (parser->Next(&value), value));
    return 0;
}

int CTest::testAttributeDefaultValues(int argc, char* argv[])
{
    String xml = String("<!DOCTYPE foo [\n")
               + String("  <!ATTLIST bar\n")
               + String("    baz (a|b|c)  \"c\">")
               + String("]>")
               + String("<foo>")
               + String("<bar/>")
               + String("<bar baz=\"a\"/>")
               + String("</foo>");
    Int32 value = 0;
    String str("");
    AutoPtr<IXmlPullParser> parser = newPullParser(xml);
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(String("bar") == (parser->GetName(&str), str));
    assert(String("c") == (parser->GetAttributeValueEx(String(NULL), String("baz"), &str), str));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(String("bar") == (parser->GetName(&str), str));
    assert(String("a") == (parser->GetAttributeValueEx(String(NULL), String("baz"), &str), str));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::END_DOCUMENT == (parser->Next(&value), value));
    return 0;
}

int CTest::testAttributeDefaultValueEntitiesExpanded(int argc, char* argv[])
{
    String xml = String("<!DOCTYPE foo [\n")
               + String("  <!ENTITY g \"ghi\">")
               + String("  <!ELEMENT foo ANY>\n")
               + String("  <!ATTLIST foo\n")
               + String("    bar CDATA \"abc &amp; def &g; jk\">")
               + String("]>")
               + String("<foo></foo>");
    Int32 value = 0;
    String str("");
    AutoPtr<IXmlPullParser> parser = newPullParser(xml);
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(String("foo") == (parser->GetName(&str), str));
    assert(String("abc & def ghi jk") == (parser->GetAttributeValueEx(String(NULL), String("bar"), &str), str));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::END_DOCUMENT == (parser->Next(&value), value));
    return 0;
}

int CTest::testAttributeDefaultValuesAndNamespaces(int argc, char* argv[])
{
    String xml = String("<!DOCTYPE foo [\n")
               + String("  <!ATTLIST foo\n")
               + String("    bar:a CDATA \"android\">")
               + String("]>")
               + String("<foo xmlns:bar='http://bar'></foo>");
    Int32 value = 0;
    String str("");
    AutoPtr<IXmlPullParser> parser = newPullParser(xml);
    String xmlstr = String("http://xmlpull.org/v1/doc/features.html#process-namespaces");// IXmlPullParser::FEATURE_PROCESS_NAMESPACES;
    parser->SetFeature(xmlstr, true);
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(String("foo") == (parser->GetName(&str), str));
    // In Expat, namespaces don't apply to default attributes
    int index = indexOfAttributeWithName(parser, String("bar:a"));
    assert(String("") == (parser->GetAttributeNamespace(index, &str), str));
    assert(String("bar:a") == (parser->GetAttributeName(index, &str), str));
    assert(String("android") == (parser->GetAttributeValue(index, &str), str));
    assert(String("CDATA") == (parser->GetAttributeType(index, &str), str));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::END_DOCUMENT == (parser->Next(&value), value));
    return 0;
}

int CTest::testAttributeEntitiesExpandedEagerly(int argc, char* argv[])
{
    assertParseFailure(String("<!DOCTYPE foo [\n")
                     + String("  <!ELEMENT foo ANY>\n")
                     + String("  <!ATTLIST foo\n")
                     + String("    bar CDATA \"abc &amp; def &g; jk\">")
                     + String("  <!ENTITY g \"ghi\">")
                     + String("]>")
                     + String("<foo></foo>"));
    return 0;
}

int CTest::testRequiredAttributesOmitted(int argc, char* argv[])
{
    String xml = String("<!DOCTYPE foo [\n")
               + String("  <!ELEMENT foo ANY>\n")
               + String("  <!ATTLIST foo\n")
               + String("    bar (a|b|c) #REQUIRED>")
               + String("]>")
               + String("<foo></foo>");
    Int32 value = 0;
    String str("");
    AutoPtr<IXmlPullParser> parser = newPullParser(xml);
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(String("foo") == (parser->GetName(&str), str));
    assert((parser->GetAttributeValueEx(String(NULL), String("bar"), &str), str.IsNull()));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    return 0;
}

int CTest::testFixedAttributesWithConflictingValues(int argc, char* argv[])
{
    String xml = String("<!DOCTYPE foo [\n")
               + String("  <!ELEMENT foo ANY>\n")
               + String("  <!ATTLIST foo\n")
               + String("    bar (a|b|c) #FIXED \"c\">")
               + String("]>")
               + String("<foo bar=\"a\"></foo>");
    Int32 value = 0;
    String str("");
    AutoPtr<IXmlPullParser> parser = newPullParser(xml);
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(String("foo") == (parser->GetName(&str), str));
    assert(String("a") == (parser->GetAttributeValueEx(String(NULL), String("bar"), &str), str));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    return 0;
}

int CTest::testParsingNotations(int argc, char* argv[])
{
    String xml = String("<!DOCTYPE foo [\n")
               + String("  <!NOTATION type-a PUBLIC \"application/a\"> \n")
               + String("  <!NOTATION type-b PUBLIC \"image/b\">\n")
               + String("  <!NOTATION type-c PUBLIC \"-//W3C//DTD SVG 1.1//EN\"\n")
               + String("     \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\"> \n")
               + String("  <!ENTITY file          SYSTEM \"d.xml\">\n")
               + String("  <!ENTITY fileWithNdata SYSTEM \"e.bin\" NDATA type-b>\n")
               + String("]>")
               + String("<foo type=\"type-a\"/>");
    Int32 value = 0;
    String str("");
    AutoPtr<IXmlPullParser> parser = newPullParser(xml);
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(String("foo") == (parser->GetName(&str), str));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    return 0;
}

int CTest::testCommentsInDoctype(int argc, char* argv[])
{
    String xml = String("<!DOCTYPE foo [")
               + String("  <!-- ' -->")
               + String("]><foo>android</foo>");
    Int32 value = 0;
    String str("");
    AutoPtr<IXmlPullParser> parser = newPullParser(xml);
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::TEXT == (parser->Next(&value), value));
    assert(String("android") == (parser->GetText(&str), str));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::END_DOCUMENT == (parser->Next(&value), value));
    return 0;
}

int CTest::testDoctypeNameOnly(int argc, char* argv[])
{
    String xml = String("<!DOCTYPE foo><foo></foo>");
    Int32 value = 0;
    String str("");
    AutoPtr<IXmlPullParser> parser = newPullParser(xml);
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(String("foo") == (parser->GetName(&str), str));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    assert(String("foo") == (parser->GetName(&str), str));
    assert(IXmlPullParser::END_DOCUMENT == (parser->Next(&value), value));
    return 0;
}

int CTest::testVeryLongEntities(int argc, char* argv[])
{
    String a = repeat('a', READ_BUFFER_SIZE + 1);
    String b = repeat('b', READ_BUFFER_SIZE + 1);
    String c = repeat('c', READ_BUFFER_SIZE + 1);

    String xml = String("<!DOCTYPE foo [\n")
               + String("  <!ENTITY ")
               + a
               + String("  \"d &")
               + b
               + String("; e\">")
               + String("  <!ENTITY ")
               + b
               + String("  \"f ")
               + c
               + String(" g\">")
               + String("]>")
               + String("<foo>h &")
               + a
               + String("; i</foo>");
    Int32 value = 0;
    String str("");
    AutoPtr<IXmlPullParser> parser = newPullParser(xml);
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::TEXT == (parser->Next(&value), value));
    assert(String("h d f ") + c + String(" g e i") == (parser->GetText(&str), str));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::END_DOCUMENT == (parser->Next(&value), value));
    return 0;
}

int CTest::testManuallyRegisteredEntitiesWithDoctypeParsing(int argc, char* argv[])
{
    String xml = String("<foo>&a;</foo>");
    AutoPtr<IXmlPullParser> parser = newPullParser(xml);
    // try {
        ECode ec = parser->DefineEntityReplacementText(String("a"), String("android"));
        if (ec != NOERROR) {
            printf("fail expected ~~~~\n");
        }
        // fail();
    // } catch (UnsupportedOperationException expected) {
    // } catch (IllegalStateException expected) {
    // }
    return 0;
}

int CTest::testDoctypeWithNextToken(int argc, char* argv[])
{
    String xml = String("<!DOCTYPE foo [<!ENTITY bb \"bar baz\">]><foo>a&bb;c</foo>");
    Int32 value = 0;
    String str("");
    AutoPtr<IXmlPullParser> parser = newPullParser(xml);
    assert(IXmlPullParser::DOCDECL == (parser->NextToken(&value), value));
    assert(String(" foo [<!ENTITY bb \"bar baz\">]") == (parser->GetText(&str), str));
    assert((parser->GetName(&str), str.IsNull()));
    assert(IXmlPullParser::START_TAG == (parser->NextToken(&value), value));
    assert(IXmlPullParser::TEXT == (parser->Next(&value), value));
    assert(String("abar bazc") == (parser->GetText(&str), str));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::END_DOCUMENT == (parser->Next(&value), value));
    return 0;
}

int CTest::testDoctypeSpansBuffers(int argc, char* argv[])
{
    AutoPtr< ArrayOf<Char32> > doctypeChars = ArrayOf<Char32>::Alloc(READ_BUFFER_SIZE + 1);
    for (int i = 0; i < READ_BUFFER_SIZE + 1; ++i) {
        (*doctypeChars)[i] = 'x';
    }
    String doctypeBody = String(" foo [<!--") + String(*doctypeChars) + String("-->]");
    String xml = String("<!DOCTYPE") + doctypeBody + String("><foo/>");
    Int32 value = 0;
    String str("");
    AutoPtr<IXmlPullParser> parser = newPullParser(xml);
    assert(IXmlPullParser::DOCDECL == (parser->NextToken(&value), value));
    assert(doctypeBody == (parser->GetText(&str), str));
    assert(IXmlPullParser::START_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::END_TAG == (parser->Next(&value), value));
    assert(IXmlPullParser::END_DOCUMENT == (parser->Next(&value), value));
    return 0;
}

int CTest::testDoctypeInDocumentElement(int argc, char* argv[])
{
    assertParseFailure(String("<foo><!DOCTYPE foo></foo>"));
    return 0;
}

int CTest::testDoctypeAfterDocumentElement(int argc, char* argv[])
{
    assertParseFailure(String("<foo/><!DOCTYPE foo>"));
    return 0;
}

int CTest::indexOfAttributeWithName(AutoPtr<IXmlPullParser> parser, String name)
{
    Int32 value = 0;
    parser->GetAttributeCount(&value);
    for (int i = 0; i < value; i++) {
        String str("");
        parser->GetAttributeName(i, &str);
        if (str.Equals(name)) {
            return i;
        }
    }
    return -1;
}

void CTest::assertParseFailure(String xml)
{
    AutoPtr<IXmlPullParser> parser = newPullParser();
    AutoPtr<IStringReader> reader;
    CStringReader::New(xml, (IStringReader**)&reader);
    parser->SetInput(reader);
    Int32 value = 0;
    ECode ec = NOERROR;
    // try {
        while ((ec = parser->Next(&value), value) != IXmlPullParser::END_DOCUMENT && ec == NOERROR) {
            // PFL_EX("value: %d", value)
        }
        if (ec != NOERROR) {
            printf("fail expected ~~~\n");
        }
        // fail();
    // } catch (XmlPullParserException expected) {
    // }
}

String CTest::repeat(char c, int length)
{
    AutoPtr< ArrayOf<Char32> > chars = ArrayOf<Char32>::Alloc(length);
    for (int i = 0; i < length; ++i) {
        (*chars)[i] = c;
    }
    return String(*chars);
}

AutoPtr<IXmlPullParser> CTest::newPullParser(String xml)
{
    AutoPtr<IXmlPullParser> result = newPullParser();
    AutoPtr<IStringReader> reader;
    CStringReader::New(xml, (IStringReader**)&reader);
    result->SetInput(reader);
    return result;
}

AutoPtr<IXmlPullParser> KmlCtest::newPullParser()
{
    AutoPtr<IKXmlParser> result;
    CKXmlParser::New((IKXmlParser**)&result);
    String FEATURE_PROCESS_DOCDECL = String("http://xmlpull.org/v1/doc/features.html#process-docdecl");
    result->SetFeature(FEATURE_PROCESS_DOCDECL, true);
    return result;
}