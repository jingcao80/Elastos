#include "test.h"

String CTest::NAMESPACE = String(NULL);

int CTest::testWhitespaceInAttributeValue(int argc, char* argv[])
{
    AutoPtr<IStringWriter> stringWriter;
    CStringWriter::New((IStringWriter**)&stringWriter);
    AutoPtr<IXmlSerializer> serializer;
    CKXmlSerializer::New((IXmlSerializer**)&serializer);
    serializer->SetOutputEx(stringWriter);
    serializer->StartDocument(String("UTF-8"), NULL);
    serializer->WriteStartTag(NAMESPACE, String("a"));
    ECode ec = serializer->WriteAttribute(NAMESPACE, String("cr"), String("\r"));
    serializer->WriteAttribute(NAMESPACE, String("lf"), String("\n"));
    serializer->WriteAttribute(NAMESPACE, String("tab"), String("\t"));
    serializer->WriteAttribute(NAMESPACE, String("space"), String(" "));
    serializer->WriteEndTag(NAMESPACE, String("a"));
    serializer->EndDocument();
    AutoPtr<IStringBuffer> sbuf;
    stringWriter->GetBuffer((IStringBuffer**)&sbuf);
    String str;
    sbuf->Substring(0, &str);
    assertXmlEquals(String("<a cr=\"&#13;\" lf=\"&#10;\" tab=\"&#9;\" space=\" \" />"), str);
    return 0;
}

int CTest::testWriteDocument(int argc, char* argv[])
{
    AutoPtr<IStringWriter> stringWriter;
    CStringWriter::New((IStringWriter**)&stringWriter);
    AutoPtr<IXmlSerializer> serializer;
    CKXmlSerializer::New((IXmlSerializer**)&serializer);
    serializer->SetOutputEx(stringWriter);
    serializer->StartDocument(String("UTF-8"), NULL);
    serializer->WriteStartTag(NAMESPACE, String("foo"));
    serializer->WriteAttribute(NAMESPACE, String("quux"), String("abc"));
    serializer->WriteStartTag(NAMESPACE, String("bar"));
    serializer->WriteEndTag(NAMESPACE, String("bar"));
    serializer->WriteStartTag(NAMESPACE, String("baz"));
    serializer->WriteEndTag(NAMESPACE, String("baz"));
    serializer->WriteEndTag(NAMESPACE, String("foo"));
    serializer->EndDocument();

    AutoPtr<IStringBuffer> sbuf;
    stringWriter->GetBuffer((IStringBuffer**)&sbuf);
    String str;
    sbuf->Substring(0, &str);
    assertXmlEquals(String("<foo quux=\"abc\"><bar /><baz /></foo>"), str);
    return 0;
}

int CTest::testWriteSpecialCharactersInText(int argc, char* argv[])
{
    AutoPtr<IStringWriter> stringWriter;
    CStringWriter::New((IStringWriter**)&stringWriter);
    AutoPtr<IXmlSerializer> serializer;
    CKXmlSerializer::New((IXmlSerializer**)&serializer);
    serializer->SetOutputEx(stringWriter);
    serializer->StartDocument(String("UTF-8"), NULL);
    serializer->WriteStartTag(NAMESPACE, String("foo"));
    serializer->WriteText(String("5'8\", 5 < 6 & 7 > 3!"));
    serializer->WriteEndTag(NAMESPACE, String("foo"));
    serializer->EndDocument();

    AutoPtr<IStringBuffer> sbuf;
    stringWriter->GetBuffer((IStringBuffer**)&sbuf);
    String str;
    sbuf->Substring(0, &str);
    assertXmlEquals(String("<foo>5'8\", 5 &lt; 6 &amp; 7 &gt; 3!</foo>"), str);
    return 0;
}

int CTest::testInvalidCharactersInText(int argc, char* argv[])
{
    AutoPtr<IXmlSerializer> serializer = newSerializer();
    serializer->WriteStartTag(NAMESPACE, String("root"));
    for (int ch = 0; ch <= 0xffff; ++ch) {
        String s("");
        s += (Char32)ch;
        ECode ec;
        if (isValidXmlCodePoint(ch)) {
            ec = serializer->WriteText(String("a") + s + String("b"));
            PFL_EX("ch: %d, ec : %p", ch, ec)
        }
        else {
            // try {
                ec = serializer->WriteText(String("a") + s + String("b"));
                if (ec != NOERROR) {
                    printf("fail expected ~~~~~\n");
                }
                // fail(s);
            // } catch (IllegalArgumentException expected) {
            // }
        }
    }
    serializer->WriteEndTag(NAMESPACE, String("root"));
    return 0;
}

int CTest::testInvalidCharactersInAttributeValues(int argc, char* argv[])
{
    AutoPtr<IXmlSerializer> serializer = newSerializer();
    serializer->WriteStartTag(NAMESPACE, String("root"));
    for (int ch = 0; ch <= 0xffff; ++ch) {
        String s("");
        s += (Char32)ch;
        ECode ec;
        if (isValidXmlCodePoint(ch)) {
            ec = serializer->WriteAttribute(NAMESPACE, String("a"), String("a") + s + String("b"));
            PFL_EX("ch: %d, ec : %p", ch, ec)
        }
        else {
            // try {
            ec = serializer->WriteAttribute(NAMESPACE, String("a"), String("a") + s + String("b"));
            if (ec != NOERROR) {
                printf("fail expected ~~~~~\n");
            }
            //     fail(s);
            // } catch (IllegalArgumentException expected) {
            // }
        }
    }
    serializer->WriteEndTag(NAMESPACE, String("root"));
    return 0;
}

int CTest::testInvalidCharactersInCdataSections(int argc, char* argv[])
{
    AutoPtr<IXmlSerializer> serializer = newSerializer();
    serializer->WriteStartTag(NAMESPACE, String("root"));
    for (int ch = 0; ch <= 0xffff; ++ch) {
        String s("");
        s += (Char32)ch;
        ECode ec;
        if (isValidXmlCodePoint(ch)) {
            ec = serializer->WriteCdSect(String("a") + s + String("b"));
            PFL_EX("ch: %d, ec : %p", ch, ec)
        }
        else {
            // try {
            ec = serializer->WriteCdSect(String("a") + s + String("b"));
            if (ec != NOERROR) {
                printf("fail expected ~~~~~\n");
            }
            //     fail(s);
            // } catch (IllegalArgumentException expected) {
            // }
        }
    }
    serializer->WriteEndTag(NAMESPACE, String("root"));
    return 0;
}

int CTest::testCdataWithTerminatorInside(int argc, char* argv[])
{
    AutoPtr<IStringWriter> writer;
    CStringWriter::New((IStringWriter**)&writer);
    AutoPtr<IXmlSerializer> serializer;
    CKXmlSerializer::New((IXmlSerializer**)&serializer);
    serializer->SetOutputEx(writer);
    serializer->StartDocument(String("UTF-8"), NULL);
    serializer->WriteStartTag(NAMESPACE, String("p"));
    serializer->WriteCdSect(String("a]]>b"));
    serializer->WriteEndTag(NAMESPACE, String("p"));
    serializer->EndDocument();
    // Adjacent CDATA sections aren't merged, so let's stick them together ourselves...
    // TODO
    assert(0 && "TODO");
    // Document doc = domOf(writer.toString());
    // NodeList children = doc.getFirstChild().getChildNodes();
    // String text = "";
    // for (int i = 0; i < children.getLength(); ++i) {
    //     text += children.item(i).getNodeValue();
    // }
    // assertEquals("a]]>b", text);
    return 0;
}


void CTest::assertXmlEquals(String expectedXml, String actualXml)
{
    String declaration = String("<?xml version='1.0' encoding='UTF-8' ?>");

    PFL_EX("first~~~~~~~~~~~ : %s",(declaration + expectedXml).string())

    PFL_EX("second~~~~~~~~~~ : %s", actualXml.string())

    assert(declaration + expectedXml == actualXml);
}

AutoPtr<IXmlSerializer> CTest::newSerializer()
{
    AutoPtr<IByteArrayOutputStream> bytesOut;
    CByteArrayOutputStream::New((IByteArrayOutputStream**)&bytesOut);
    AutoPtr<IXmlSerializer> serializer;
    CKXmlSerializer::New((IXmlSerializer**)&serializer);
    serializer->SetOutput(bytesOut, String("UTF-8"));
    serializer->StartDocument(String("UTF-8"), NULL);
    return serializer;
}

Boolean CTest::isValidXmlCodePoint(int c)
{
    // http://www.w3.org/TR/REC-xml/#charsets
    return (c >= 0x20 && c <= 0xd7ff) || (c == 0x9) || (c == 0xa) || (c == 0xd) ||
            (c >= 0xe000 && c <= 0xfffd) || (c >= 0x10000 && c <= 0x10ffff);
}
