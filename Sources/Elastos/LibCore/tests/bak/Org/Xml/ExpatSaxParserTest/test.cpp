
#include "test.h"

const String CTest::SNIPPET = String("<dagny dad=\"bob\">hello</dagny>");
const String CTest::XML = String("<one xmlns='ns:default' xmlns:n1='ns:1' a='b'>\n")
                         + String("  <n1:two c='d' n1:e='f' xmlns:n2='ns:2'>text</n1:two>\n")
                         + String("</one>");

int CTest::testGlobalReferenceTableOverflow(int argc, char* argv[])
{
    // We used to use a JNI global reference per interned string.
    // Framework apps have a limit of 2000 JNI global references per VM.
    StringBuilder xml;
    xml.AppendCStr("<root>");
    for (int i = 0; i < 4000; ++i) {
        xml.AppendString(String("<tag") + StringUtils::Int32ToString(i) + String(">"));
        xml.AppendString(String("</tag") + StringUtils::Int32ToString(i) + String(">"));
    }
    xml.AppendCStr("</root>");
    AutoPtr<IEntityResolver> er;
    CDefaultHandler::New((IEntityResolver**)&er);
    AutoPtr<IContentHandler> ch = (IContentHandler*)er->Probe(EIID_IContentHandler);
    parse(xml.ToString(), ch);
    return 0;
}

int CTest::testExceptions(int argc, char* argv[])
{
    // From startElement().
    // ContentHandler contentHandler = new DefaultHandler() {
    //     @Override
    //     public void startElement(String uri, String localName,
    //             String qName, Attributes attributes)
    //             throws SAXException {
    //         throw new SAXException();
    //     }
    // };
    AutoPtr<IEntityResolver> er;
    CDefaultHandler::New((IEntityResolver**)&er);
    AutoPtr<IContentHandler> ch = (IContentHandler*)er->Probe(EIID_IContentHandler);
    // try {
        parse(SNIPPET, ch);
    //     fail();
    // } catch (SAXException checked) { /* expected */ }

    // From endElement().
    // contentHandler = new DefaultHandler() {
    //     @Override
    //     public void endElement(String uri, String localName,
    //             String qName)
    //             throws SAXException {
    //         throw new SAXException();
    //     }
    // };
    // try {
        parse(SNIPPET, ch);
    //     fail();
    // } catch (SAXException checked) { /* expected */ }

    // From characters().
    // contentHandler = new DefaultHandler() {
    //     @Override
    //     public void characters(char ch[], int start, int length)
    //             throws SAXException {
    //         throw new SAXException();
    //     }
    // };
    // try {
        parse(SNIPPET, ch);
    //     fail();
    // } catch (SAXException checked) { /* expected */ }
    return 0;
}

int CTest::testSax(int argc, char* argv[])
{
    // try {
        // Parse String.
        TestHandler* handler = new TestHandler();
        parse(SNIPPET, (IContentHandler*)((IEntityResolver*)handler)->Probe(EIID_IContentHandler));
        validate(handler);

        // Parse Reader.
        handler = new TestHandler();
        AutoPtr<IStringReader> reader;
        CStringReader::New(SNIPPET, (IStringReader**)&reader);
        parse(reader, (IContentHandler*)((IEntityResolver*)handler)->Probe(EIID_IContentHandler));
        validate(handler);

        // Parse InputStream.
        handler = new TestHandler();
        AutoPtr<ArrayOf<Char32> > outchars = SNIPPET.GetChars();
        AutoPtr<ArrayOf<Byte> > outbyte = ArrayOf<Byte>::Alloc(outchars->GetLength());
        for (int i = 0; i < outchars->GetLength(); ++i)
        {
            (*outbyte)[i] = (*outchars)[i] & 0xFF;
        }
        AutoPtr<IInputStream> in;
        CByteArrayInputStream::New(outbyte, (IByteArrayInputStream**)&in);
        parse(in,String("UTF_8"), (IContentHandler*)((IEntityResolver*)handler)->Probe(EIID_IContentHandler));
        validate(handler);
    // } catch (SAXException e) {
    //     throw new RuntimeException(e);
    // } catch (IOException e) {
    //     throw new RuntimeException(e);
    // }
    return 0;
}

class NamespaceHandler
    : public IContentHandler
    , public ElRefBase {
public:
    AutoPtr<ILocator> locator;
    Boolean documentStarted;
    Boolean documentEnded;
    HashMap<String, String> prefixMappings;

    Boolean oneStarted;
    Boolean twoStarted;
    Boolean oneEnded;
    Boolean twoEnded;

public:
    CAR_INTERFACE_IMPL();

    void Validate() {
        assert(documentEnded);
    }

    ECode SetDocumentLocator(ILocator* locator) {
        this->locator = locator;
        return NOERROR;
    }

    ECode StartDocument() {
        documentStarted = TRUE;
        assert(locator != NULL);
        assert(0 == prefixMappings.GetSize());
        assert(!documentEnded);
        return NOERROR;
    }

    ECode EndDocument() {
        assert(documentStarted);
        assert(oneEnded);
        assert(twoEnded);
        assert(0 == prefixMappings.GetSize());
        documentEnded = true;
        return NOERROR;
    }

    ECode StartPrefixMapping(const String& prefix, const String& uri) {
        prefixMappings[prefix] = uri;
        return NOERROR;
    }

    ECode EndPrefixMapping(const String& prefix) {
        assert(prefixMappings.Erase(prefix));
        return NOERROR;
    }

    ECode StartElement(const String& uri, const String& localName, const String& qName, IAttributes* atts) {

        if (localName == "one") {
            assert(2 == prefixMappings.GetSize());
            Int32 value = 0;
            locator->GetLineNumber(&value);
            assert(1 == value);

            assert(!oneStarted);
            assert(!twoStarted);
            assert(!oneEnded);
            assert(!twoEnded);

            oneStarted = true;

            assert("ns:default" == uri);
            assert("one" == qName);

            // Check atts.
            Int32 len = 0;
            atts->GetLength(&len);
            assert(1 ==len);

            String str;
            atts->GetURI(0, &str);
            assert("" == str);
            atts->GetLocalName(0, &str);
            assert("a"== str);
            atts->GetValue(0, &str);
            assert("b" == str);
            atts->GetIndex(String(""), String("a"), &len);
            assert(0 == len);
            atts->GetValueEx(String(""), String("a"), &str);
            assert("b" == str);
            return NOERROR;
        }

        if (localName == "two") {
            assert(3 == prefixMappings.GetSize());

            assert(oneStarted);
            assert(!twoStarted);
            assert(!oneEnded);
            assert(!twoEnded);

            twoStarted = true;

            assert("ns:1" == uri);
            assert("n1:two" == qName);

            // Check atts.
            Int32 len = 0;
            atts->GetLength(&len);
            assert(2 == len);

            String str;
            atts->GetURI(0, &str);
            assert("" == str);
            atts->GetLocalName(0, &str);
            assert("c" == str);
            atts->GetValue(0, &str);
            assert("d" == str);
            atts->GetIndex(String(""), String("c"), &len);
            assert(0 == len);
            atts->GetValueEx(String(""), String("c"), &str);
            assert("d" == str);

            atts->GetURI(1, &str);
            assert("ns:1" == str);
            atts->GetLocalName(1, &str);
            assert("e" == str);
            atts->GetValue(1, &str);
            assert("f" == str);
            atts->GetIndex(String("ns:1"), String("e"), &len);
            assert(1 == len);
            atts->GetValueEx(String("ns:1"), String("e"), &str);
            assert("f" == str);

            // We shouldn't find these.
            atts->GetIndex(String("ns:default"), String("e"), &len);
            assert(-1 == len);
            atts->GetValueEx(String("ns:default"), String("e"), &str);
            assert(str.IsNull());

            return NOERROR;
        }

        // fail();
        printf("StartElement=======fail~~~~~\n");
     }

    ECode EndElement(const String& uri, const String& localName, const String& qName) {
        if (localName == "one") {
            Int32 value = 0;
            locator->GetLineNumber(&value);
            assert(3 == value);

            assert(oneStarted);
            assert(twoStarted);
            assert(twoEnded);
            assert(!oneEnded);

            oneEnded = true;

            assert("ns:default" == uri);
            assert("one" == qName);

            return NOERROR;
        }

        if (localName == "two") {
            assert(oneStarted);
            assert(twoStarted);
            assert(!twoEnded);
            assert(!oneEnded);

            twoEnded = true;

            assert("ns:1" == uri);
            assert("n1:two" == qName);

            return NOERROR;
        }

        // fail();
        printf("EndElement=======fail~~~~~\n");
    }

    ECode Characters(ArrayOf<Char32>* ch, int start, int length) {
        String s(*ch, start, length);
        s.Trim();

        if (!s.Equals("")) {
            assert(oneStarted);
            assert(twoStarted);
            assert(!oneEnded);
            assert(!twoEnded);
            assert("text" == s);
        }
        return NOERROR;
    }

    ECode IgnorableWhitespace(ArrayOf<Char32>* ch, int start, int length) {
        // fail();
        printf("IgnorableWhitespace=======fail~~~~~\n");
        return NOERROR;
    }

    ECode ProcessingInstruction(const String& target, const String& data) {
        // fail();
        printf("processingInstruction=======fail~~~~~\n");
        return NOERROR;
    }

    ECode SkippedEntity(const String& name) {
        // fail();
        printf("skippedEntity=======fail~~~~~\n");
        return NOERROR;
    }
};

CAR_INTERFACE_IMPL(NamespaceHandler, IContentHandler)

int CTest::testNamespaces(int argc, char* argv[])
{
    // try {
        AutoPtr<NamespaceHandler> handler = new NamespaceHandler();
        parse(XML, handler);
        handler->Validate();
    // } catch (SAXException e) {
    //     throw new RuntimeException(e);
    // }
    return 0;
}

class TestDtdHandler : public CDefaultHandler2 {
public:
    String name;
    String publicId;
    String systemId;
    String ndName;
    String ndPublicId;
    String ndSystemId;
    String ueName;
    String uePublicId;
    String ueSystemId;
    String ueNotationName;

    Boolean ended;

    AutoPtr<ILocator> locator;
public:
    // @Override
    ECode StartDTD(const String& name, const String& publicId, const String& systemId) {
        this->name = name;
        this->publicId = publicId;
        this->systemId = systemId;
        return NOERROR;
    }

    // @Override
    ECode EndDTD() {
        ended = true;
        return NOERROR;
    }

    // @Override
    ECode SetDocumentLocator(ILocator* locator) {
        this->locator = locator;
        return NOERROR;
    }

    // @Override
    ECode NotationDecl(const String& name, const String& publicId, const String& systemId) {
        this->ndName = name;
        this->ndPublicId = publicId;
        this->ndSystemId = systemId;
        return NOERROR;
    }

    // @Override
    ECode UnparsedEntityDecl(const String& entityName, const String& publicId, const String& systemId, const String& notationName) {
        this->ueName = entityName;
        this->uePublicId = publicId;
        this->ueSystemId = systemId;
        this->ueNotationName = notationName;
        return NOERROR;
    }
};

TestDtdHandler* runDtdTest(String s) {
    AutoPtr<IReader> in;
    CStringReader::New(s, (IStringReader**)&in);
    TestDtdHandler* handler = new TestDtdHandler();
    assert(0 && "TODO");
    // ExpatReader reader = new ExpatReader();
    // reader.setContentHandler(handler);
    // reader.setDTDHandler(handler);
    // reader.setLexicalHandler(handler);
    // reader.parse(new InputSource(in));
    return handler;
}

int CTest::testDtdDoctype(int argc, char* argv[])
{
    TestDtdHandler* handler = runDtdTest(String("<?xml version=\"1.0\"?><!DOCTYPE foo PUBLIC 'bar' 'tee'><a></a>"));
    assert("foo" == handler->name);
    assert("bar" == handler->publicId);
    assert("tee" == handler->systemId);
    assert(handler->ended);
    return 0;
}

int CTest::testDtdUnparsedEntity_system(int argc, char* argv[])
{
    TestDtdHandler* handler = runDtdTest(String("<?xml version=\"1.0\"?><!DOCTYPE foo PUBLIC 'bar' 'tee' [ <!ENTITY ent SYSTEM 'blah' NDATA poop> ]><a></a>"));
    assert("ent" == handler->ueName);
    assert(handler->uePublicId.IsNull());
    assert("blah" == handler->ueSystemId);
    assert("poop" == handler->ueNotationName);
    return 0;
}

int CTest::testDtdUnparsedEntity_public(int argc, char* argv[])
{
    TestDtdHandler* handler = runDtdTest(String("<?xml version=\"1.0\"?><!DOCTYPE foo PUBLIC 'bar' 'tee' [ <!ENTITY ent PUBLIC 'a' 'b' NDATA poop> ]><a></a>"));
    assert("ent" == handler->ueName);
    assert("a" == handler->uePublicId);
    assert("b" == handler->ueSystemId);
    assert("poop" == handler->ueNotationName);
    return 0;
}

int CTest::testDtdNotation_system(int argc, char* argv[])
{
    TestDtdHandler* handler = runDtdTest(String("<?xml version=\"1.0\"?><!DOCTYPE foo PUBLIC 'bar' 'tee' [ <!NOTATION sn SYSTEM 'nf2'> ]><a></a>"));
    assert("sn" == handler->ndName);
    assert(handler->ndPublicId.IsNull());
    assert("nf2" == handler->ndSystemId);
    return 0;
}

int CTest::testDtdNotation_public(int argc, char* argv[])
{
    TestDtdHandler* handler = runDtdTest(String("<?xml version=\"1.0\"?><!DOCTYPE foo PUBLIC 'bar' 'tee' [ <!NOTATION pn PUBLIC 'nf1'> ]><a></a>"));
    assert("pn" == handler->ndName);
    assert("nf1" == handler->ndPublicId);
    assert(handler->ndSystemId.IsNull());
    return 0;
}

class TestCdataHandler : public CDefaultHandler2 {
public:

    int startCdata;
    int endCdata;
    StringBuffer buffer;

    TestCdataHandler()
        : startCdata(0)
        , endCdata(0)
    {}
    // @Override
    ECode Characters(ArrayOf<Char32>* ch, int start, int length) {
        buffer.AppendCharsEx(*ch, start, length);
        return NOERROR;
    }

    // @Override
    ECode StartCDATA() {
        startCdata++;
        return NOERROR;
    }

    // @Override
    ECode EndCDATA() {
        endCdata++;
        return NOERROR;
    }
};

int CTest::testCdata(int argc, char* argv[])
{
    AutoPtr<IReader> in;
    CStringReader::New(String("<a><![CDATA[<b></b>]]> <![CDATA[<c></c>]]></a>"), (IStringReader**)&in);

    TestCdataHandler* handler = new TestCdataHandler();
    assert(0 && "TODO");
    // ExpatReader reader = new ExpatReader();
    // reader.setContentHandler(handler);
    // reader.setLexicalHandler(handler);

    // reader.parse(new InputSource(in));

    // assertEquals(2, handler.startCdata);
    // assertEquals(2, handler.endCdata);
    // assertEquals("<b></b> <c></c>", handler.buffer.toString());
    return 0;
}

class TestProcessingInstrutionHandler : public CDefaultHandler2 {
public:
    String target;
    String data;

    // @Override
    ECode ProcessingInstruction(const String& target, const String& data) {
        this->target = target;
        this->data = data;
    }
};

int CTest::testProcessingInstructions(int argc, char* argv[])
{
    AutoPtr<IReader> in;
    CStringReader::New(String("<?bob lee?><a></a>"), (IStringReader**)&in);
    TestProcessingInstrutionHandler* handler = new TestProcessingInstrutionHandler();

    assert(0 && "TODO");
    // ExpatReader reader = new ExpatReader();
    // reader.setContentHandler(handler);
    // reader.parse(new InputSource(in));

    // assertEquals("bob", handler.target);
    // assertEquals("lee", handler.data);
    return 0;
}

int CTest::testExternalEntity(int argc, char* argv[])
{
    class Handler : public CDefaultHandler {

        List<String> elementNames;
        StringBuilder text;

        ECode resolveEntity(const String& publicId, const String& systemId, IInputSource** src) {
            VALIDATE_NOT_NULL(src)

            if (publicId.Equals("publicA") && systemId.Equals("systemA")) {
                AutoPtr<IStringReader> sreader;
                CStringReader::New(String("<a/>"), (IStringReader**)&sreader);
                AutoPtr<IInputSource> res;
                CInputSource::New(sreader, (IInputSource**)&res);
                *src = res;
                REFCOUNT_ADD(*src)
                return NOERROR;
            }
            else if (publicId.Equals("publicB") && systemId.Equals("systemB")) {
                /*
                 * Explicitly set the encoding here or else the parser will
                 * try to use the parent parser's encoding which is utf-16.
                 */
                String str("bob");
                AutoPtr< ArrayOf<Char32> > ochars = str.GetChars();
                AutoPtr< ArrayOf<Byte> > obyte = ArrayOf<Byte>::Alloc(ochars->GetLength());
                for (int i = 0; i < ochars->GetLength(); ++i)
                {
                    (*obyte)[i] = (*ochars)[i] & 0xFF;
                }
                AutoPtr<IByteArrayInputStream> bais;
                CByteArrayInputStream::New(obyte, (IByteArrayInputStream**)&bais);
                AutoPtr<IInputSource> inputSource;
                CInputSource::New(bais, (IInputSource**)&inputSource);
                inputSource->SetEncoding(String("utf-8"));
                *src = inputSource;
                REFCOUNT_ADD(*src)
                return NOERROR;
            }

            // throw new AssertionError();
        }

        // @Override
        ECode StartElement(const String& uri, const String& localName, const String& qName, IAttributes* attributes) {
            elementNames.PushBack(localName);
            return NOERROR;
        }

        // @Override
        ECode EndElement(const String& uri, const String& localName, const String& qName) {
            elementNames.PushBack(String("/") + localName);
            return NOERROR;
        }

        // @Override
        Characters(ArrayOf<Char32>* ch, int start, int length) {
            text.AppendCharsEx(*ch, start, length);
            return NOERROR;
        }
    };
    AutoPtr<IReader> in;
    CStringReader::New(String("<?xml version=\"1.0\"?>\n")
                        + String("<!DOCTYPE foo [\n")
                        + String("  <!ENTITY a PUBLIC 'publicA' 'systemA'>\n")
                        + String("  <!ENTITY b PUBLIC 'publicB' 'systemB'>\n")
                        + String("]>\n")
                        + String("<foo>\n")
                        + String("  &a;<b>&b;</b></foo>")
                        , (IStringReader**)&in);

    assert(0 && "TODO");
    // ExpatReader reader = new ExpatReader();
    // Handler handler = new Handler();
    // reader.setContentHandler(handler);
    // reader.setEntityResolver(handler);

    // reader.parse(new InputSource(in));

    // assertEquals(Arrays.asList("foo", "a", "/a", "b", "/b", "/foo"),
    //         handler.elementNames);
    // assertEquals("bob", handler.text.toString().trim());
    return 0;
}

int CTest::testExternalEntityDownload(int argc, char* argv[])
{
    assert(0 && "TODO");
    // final MockWebServer server = new MockWebServer();
    // server.enqueue(new MockResponse().setBody("<bar></bar>"));
    // server.play();

    // class Handler extends DefaultHandler {
    //     final List<String> elementNames = new ArrayList<String>();

    //     @Override public InputSource resolveEntity(String publicId, String systemId)
    //             throws IOException {
    //         // The parser should have resolved the systemId.
    //         assertEquals(server.getUrl("/systemBar").toString(), systemId);
    //         return new InputSource(systemId);
    //     }

    //     @Override public void startElement(String uri, String localName, String qName,
    //             Attributes attributes) {
    //         elementNames.add(localName);
    //     }

    //     @Override public void endElement(String uri, String localName, String qName) {
    //         elementNames.add("/" + localName);
    //     }
    // };

    // // 'systemBar', the external entity, is relative to 'systemFoo':
    // Reader in = new StringReader("<?xml version=\"1.0\"?>\n"
    //     + "<!DOCTYPE foo [\n"
    //     + "  <!ENTITY bar SYSTEM 'systemBar'>\n"
    //     + "]>\n"
    //     + "<foo>&bar;</foo>");
    // ExpatReader reader = new ExpatReader();
    // Handler handler = new Handler();
    // reader.setContentHandler(handler);
    // reader.setEntityResolver(handler);
    // InputSource source = new InputSource(in);
    // source.setSystemId(server.getUrl("/systemFoo").toString());
    // reader.parse(source);
    // assertEquals(Arrays.asList("foo", "bar", "/bar", "/foo"), handler.elementNames);
    return 0;
}

void CTest::validate(TestHandler* handler)
{
    assert("dagny" == handler->startElementName);
    assert("dagny" == handler->endElementName);
    assert("hello" == handler->text.ToString());
}

void CTest::parse(String xml, AutoPtr<IContentHandler> contentHandler)
{
    assert(0 && "TODO");
    // try {
    // XMLReader reader = new ExpatReader();
    // reader.setContentHandler(contentHandler);
    // reader.parse(new InputSource(new StringReader(xml)));
    // } catch (IOException e) {
    //     throw new AssertionError(e);
    // }
}

void CTest::parse(AutoPtr<IReader> in, AutoPtr<IContentHandler> contentHandler)
{
    assert(0 && "TODO");
    // XMLReader reader = new ExpatReader();
    // reader.setContentHandler(contentHandler);
    // reader.parse(new InputSource(in));
}

void CTest::parse(AutoPtr<IInputStream> in, String encoding, AutoPtr<IContentHandler> contentHandler)
{
    assert(0 && "TODO");
    // try {
    // XMLReader reader = new ExpatReader();
    // reader.setContentHandler(contentHandler);
    // InputSource source = new InputSource(in);
    // source.setEncoding(encoding.expatName);
    // reader.parse(source);
    // } catch (IOException e) {
    //     throw new AssertionError(e);
    // }
}
