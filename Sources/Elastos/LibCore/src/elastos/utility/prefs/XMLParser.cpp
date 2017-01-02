//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "Elastos.CoreLibrary.Extensions.h"
#include "XMLParser.h"
#include "AutoLock.h"
#include "COutputStreamWriter.h"
#include "CBufferedWriter.h"
#include "StringUtils.h"
#include "StringBuilder.h"
#include "CStringTokenizer.h"
#include "AbstractPreferences.h"
#include "CArrayList.h"
#include "CProperties.h"
#include "CFile.h"
#include "CFileOutputStream.h"
#include "CFileInputStream.h"
#include "CInputStreamReader.h"
#include "NodeSet.h"

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Core::AutoLock;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::IO::IOutputStreamWriter;
using Elastos::IO::COutputStreamWriter;
using Elastos::IO::CBufferedWriter;
using Elastos::IO::IWriter;
using Elastos::IO::IFlushable;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::CInputStreamReader;
using Elastos::IO::CFile;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::ICloseable;
using Elastos::Utility::CStringTokenizer;
using Org::Xml::Sax::IInputSource;
using Org::W3c::Dom::IDocument;
using Org::W3c::Dom::INode;

namespace Elastos {
namespace Utility {
namespace Prefs {

#ifndef FAIL_GOTO2
#define FAIL_GOTO2(expr, label) \
    do { \
        ec = expr; \
        if (FAILED(ec)) goto label; \
    } while(0);
#endif

const String XMLParser::PREFS_DTD_NAME = String("http://java.sun.com/dtd/preferences.dtd");
const String XMLParser::PREFS_DTD = String("<?xml version=\"1.0\" encoding=\"UTF-8\"?>")
    + String("    <!ELEMENT preferences (root)>")
    + String("    <!ATTLIST preferences EXTERNAL_XML_VERSION CDATA \"0.0\" >")
    + String("    <!ELEMENT root (map, node*) >")
    + String("    <!ATTLIST root type (system|user) #REQUIRED >")
    + String("    <!ELEMENT node (map, node*) >")
    + String("    <!ATTLIST node name CDATA #REQUIRED >")
    + String("    <!ELEMENT map (entry*) >")
    + String("    <!ELEMENT entry EMPTY >")
    + String("    <!ATTLIST entry key   CDATA #REQUIRED value CDATA #REQUIRED >");
const String XMLParser::HEADER = String("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
const String XMLParser::DOCTYPE = String("<!DOCTYPE preferences SYSTEM");

const String XMLParser::FILE_PREFS = String("<!DOCTYPE map SYSTEM 'http://java.sun.com/dtd/preferences.dtd'>");
const Float XMLParser::XML_VERSION = 1.0f;
const AutoPtr<IDocumentBuilder> XMLParser::sBuilder;
Int32 XMLParser::sIndent = -1;
//TODO:
Boolean XMLParser::sInit;// = XMLParser::StaticInit();

Boolean XMLParser::StaticInit()
{
    assert(0 && "TODO");
    // AutoPtr<IDocumentBuilderFactory> factory = DocumentBuilderFactory.newInstance();
    // factory->SetValidating(FALSE);
    // try {
    //     sBuilder = factory.newDocumentBuilder();
    // } catch (ParserConfigurationException e) {
    //     throw new Error(e);
    // }
    // sBuilder.setEntityResolver(new EntityResolver() {
    //     public InputSource resolveEntity(String publicId, String systemId)
    //     throws SAXException, IOException {
    //         if (systemId.equals(PREFS_DTD_NAME)) {
    //             InputSource result = new InputSource(new StringReader(
    //                     PREFS_DTD));
    //             result.setSystemId(PREFS_DTD_NAME);
    //             return result;
    //         }
    //         throw new SAXException("Invalid DOCTYPE declaration " + systemId);
    //     }
    // });
    // sBuilder.setErrorHandler(new ErrorHandler() {
    //     public void warning(SAXParseException e) throws SAXException {
    //         throw e;
    //     }

    //     public void error(SAXParseException e) throws SAXException {
    //         throw e;
    //     }

    //     public void fatalError(SAXParseException e) throws SAXException {
    //         throw e;
    //     }
    // });
    return TRUE;
}

XMLParser::XMLParser()
{}

ECode XMLParser::ExportPrefs(
    /* [in] */ IPreferences* prefs,
    /* [in] */ IOutputStream* stream,
    /* [in] */ Boolean withSubTree) /*throws IOException, BackingStoreException*/
{
    sIndent = -1;
    AutoPtr<IBufferedWriter> out;
    AutoPtr<IOutputStreamWriter> writer;
    COutputStreamWriter::New(stream, String("UTF-8"), (IOutputStreamWriter**)&writer);
    CBufferedWriter::New(IWriter::Probe(writer), (IBufferedWriter**)&out);
    AutoPtr<IWriter> _out = IWriter::Probe(out);
    _out->Write(HEADER);
    out->NewLine();
    out->NewLine();

    _out->Write(DOCTYPE);
    _out->Write(String(" '"));
    _out->Write(PREFS_DTD_NAME);
    _out->Write(String("'>"));
    out->NewLine();
    out->NewLine();

    AutoPtr<ArrayOf<String> > na = ArrayOf<String>::Alloc(1);
    na->Set(0, String("EXTERNAL_XML_VERSION"));

    AutoPtr<ArrayOf<String> > va = ArrayOf<String>::Alloc(1);
    va->Set(0, StringUtils::ToString(XML_VERSION));
    FlushStartTag(String("preferences"), na, va, out);

    Boolean userMode = FALSE;
    prefs->IsUserNode(&userMode);
    na->Set(0, String("type"));
    if (userMode) {
        va->Set(0, String("user"));
    } else {
        va->Set(0, String("system"));
    }

    FlushStartTag(String("root"), na, va, out);
    FlushEmptyElement(String("map"), out);

    AutoPtr<IStringTokenizer> ancestors;
    String path;
    prefs->GetAbsolutePath(&path);
    CStringTokenizer::New(path, String("/"), (IStringTokenizer**)&ancestors);
    ExportNode(ancestors, prefs, withSubTree, out);

    FlushEndTag(String("root"), out);
    FlushEndTag(String("preferences"), out);
    IFlushable::Probe(out)->Flush();
    out = NULL;
    return NOERROR;
}

ECode XMLParser::ExportNode(
    /* [in] */ IStringTokenizer* ancestors,
    /* [in] */ IPreferences* prefs,
    /* [in] */ Boolean withSubTree,
    /* [in] */ IBufferedWriter* out) /*throws IOException, BackingStoreException*/
{
    Boolean has = FALSE;
    if (ancestors->HasMoreTokens(&has), has) {
        String name;
        ancestors->GetNextToken(&name);
        AutoPtr<ArrayOf<String> > na = ArrayOf<String>::Alloc(1);
        na->Set(0, String("name"));

        AutoPtr<ArrayOf<String> > va = ArrayOf<String>::Alloc(1);
        va->Set(0, name);
        FAIL_RETURN(FlushStartTag(String("node"), na, va, out));
        if (ancestors->HasMoreTokens(&has), has) {
            FAIL_RETURN(FlushEmptyElement(String("map"), out));
            FAIL_RETURN(ExportNode(ancestors, prefs, withSubTree, out));
        } else {
            FAIL_RETURN(ExportEntries(prefs, out));
            if (withSubTree) {
                FAIL_RETURN(ExportSubTree(prefs, out));
            }
        }
        FAIL_RETURN(FlushEndTag(String("node"), out));
    }
    return NOERROR;
}

ECode XMLParser::ExportSubTree(
    /* [in] */ IPreferences* prefs,
    /* [in] */ IBufferedWriter* out) /*throws BackingStoreException, IOException */
{
    AutoPtr<ArrayOf<String> > names;
    FAIL_RETURN(prefs->GetChildrenNames((ArrayOf<String>**)&names));
    if (names->GetLength() > 0) {
        for (Int32 i = 0; i < names->GetLength(); i++) {
            AutoPtr<IPreferences> child;
            FAIL_RETURN(prefs->GetNode((*names)[i], (IPreferences**)&child));

            AutoPtr<ArrayOf<String> > na = ArrayOf<String>::Alloc(1);
            na->Set(0, String("name"));

            AutoPtr<ArrayOf<String> > va = ArrayOf<String>::Alloc(1);
            va->Set(0, (*names)[i]);
            FAIL_RETURN(FlushStartTag(String("node"), na, va, out));
            FAIL_RETURN(ExportEntries(child, out));
            FAIL_RETURN(ExportSubTree(child, out));
            FAIL_RETURN(FlushEndTag(String("node"), out));
        }
    }

    return NOERROR;
}

ECode XMLParser::ExportEntries(
    /* [in] */ IPreferences* prefs,
    /* [in] */ IBufferedWriter* out) /*throws BackingStoreException, IOException*/
{
    AutoPtr<ArrayOf<String> > keys;
    FAIL_RETURN(prefs->GetKeys((ArrayOf<String>**)&keys));
    Int32 len = keys->GetLength();
    AutoPtr<ArrayOf<String> > values = ArrayOf<String>::Alloc(len);
    for (Int32 i = 0; i < len; i++) {
        prefs->Get((*keys)[i], String(NULL), &(*values)[i]);
    }
    return ExportEntries(keys, values, out);
}

ECode XMLParser::ExportEntries(
    /* [in] */ ArrayOf<String>* keys,
    /* [in] */ ArrayOf<String>* values,
    /* [in] */ IBufferedWriter* out) /*throws IOException*/
{
    if (keys->GetLength() == 0) {
        return FlushEmptyElement(String("map"), out);
    }

    FAIL_RETURN(FlushStartTag(String("map"), out));
    for (Int32 i = 0; i < keys->GetLength(); i++) {
        if ((*values)[i] != NULL) {
            AutoPtr<ArrayOf<String> > na = ArrayOf<String>::Alloc(2);
            na->Set(0, String("key"));
            na->Set(1, String("value"));

            AutoPtr<ArrayOf<String> > va = ArrayOf<String>::Alloc(2);
            va->Set(0, (*keys)[i]);
            va->Set(1, (*values)[i]);

            FAIL_RETURN(FlushEmptyElement(String("entry"), na, va, out));
        }
    }
    return FlushEndTag(String("map"), out);
}

ECode XMLParser::FlushEndTag(
    /* [in] */ const String& tagName,
    /* [in] */ IBufferedWriter* out) /*throws IOException*/
{
    FAIL_RETURN(FlushIndent(sIndent--, out));
    FAIL_RETURN(IWriter::Probe(out)->Write(String("</")));
    FAIL_RETURN(IWriter::Probe(out)->Write(tagName));
    FAIL_RETURN(IWriter::Probe(out)->Write(String(">")));
    return out->NewLine();
}

ECode XMLParser::FlushEmptyElement(
    /* [in] */ const String& tagName,
    /* [in] */ IBufferedWriter* out) /*throws IOException*/
{
    FAIL_RETURN(FlushIndent(++sIndent, out));
    FAIL_RETURN(IWriter::Probe(out)->Write(String("<")));
    FAIL_RETURN(IWriter::Probe(out)->Write(tagName));
    FAIL_RETURN(IWriter::Probe(out)->Write(String(" />")));
    FAIL_RETURN(out->NewLine());
    sIndent--;
    return NOERROR;
}

ECode XMLParser::FlushEmptyElement(
    /* [in] */ const String& tagName,
    /* [in] */ ArrayOf<String>* attrKeys,
    /* [in] */ ArrayOf<String>* attrValues,
    /* [in] */ IBufferedWriter* out) /*throws IOException*/
{
    FAIL_RETURN(FlushIndent(++sIndent, out));
    FAIL_RETURN(IWriter::Probe(out)->Write(String("<")));
    FAIL_RETURN(IWriter::Probe(out)->Write(tagName));
    FAIL_RETURN(FlushPairs(attrKeys, attrValues, out));
    FAIL_RETURN(IWriter::Probe(out)->Write(String(" />")));
    FAIL_RETURN(out->NewLine());
    sIndent--;
    return NOERROR;
}

ECode XMLParser::FlushPairs(
    /* [in] */ ArrayOf<String>* attrKeys,
    /* [in] */ ArrayOf<String>* attrValues,
    /* [in] */ IBufferedWriter* out) /*throws IOException*/
{
    for (Int32 i = 0; i < attrKeys->GetLength(); i++) {
        FAIL_RETURN(IWriter::Probe(out)->Write(String(" ")));
        FAIL_RETURN(IWriter::Probe(out)->Write((*attrKeys)[i]));
        FAIL_RETURN(IWriter::Probe(out)->Write(String("=\"")));
        FAIL_RETURN(IWriter::Probe(out)->Write(HtmlEncode((*attrValues)[i])));
        FAIL_RETURN(IWriter::Probe(out)->Write(String("\"")));
    }

    return NOERROR;
}

ECode XMLParser::FlushIndent(
    /* [in] */ Int32 ind,
    /* [in] */ IBufferedWriter* out) /*throws IOException*/
{
    for (Int32 i = 0; i < ind; i++) {
        FAIL_RETURN(IWriter::Probe(out)->Write(String("  ")));
    }
    return NOERROR;
}

ECode XMLParser::FlushStartTag(
    /* [in] */ const String& tagName,
    /* [in] */ ArrayOf<String>* attrKeys,
    /* [in] */ ArrayOf<String>* attrValues,
    /* [in] */ IBufferedWriter* out) /*throws IOException*/
{
    FAIL_RETURN(FlushIndent(++sIndent, out));
    FAIL_RETURN(IWriter::Probe(out)->Write(String("<")));
    FAIL_RETURN(IWriter::Probe(out)->Write(tagName));
    FAIL_RETURN(FlushPairs(attrKeys, attrValues, out));
    FAIL_RETURN(IWriter::Probe(out)->Write(String(">")));
    return out->NewLine();
}

ECode XMLParser::FlushStartTag(
    /* [in] */ const String& tagName,
    /* [in] */ IBufferedWriter* out) /*throws IOException*/
{
    FAIL_RETURN(FlushIndent(++sIndent, out));
    FAIL_RETURN(IWriter::Probe(out)->Write(String("<")));
    FAIL_RETURN(IWriter::Probe(out)->Write(tagName));
    FAIL_RETURN(IWriter::Probe(out)->Write(String(">")));
    return out->NewLine();
}

String XMLParser::HtmlEncode(
    /* [in] */ const String& s)
{
    StringBuilder sb/* = new StringBuilder()*/;
    for (Int32 i = 0; i < s.GetLength(); i++) {
        Char32 c = s.GetChar(i);
        switch (c) {
        case '<':
            sb.Append("&lt;");
            break;
        case '>':
            sb.Append("&gt;");
            break;
        case '&':
            sb.Append("&amp;");
            break;
        case '"':
            sb.Append("&quot;");
            break;
        default:
            sb.AppendChar(c);
        }
    }
    return sb.ToString();
}

ECode XMLParser::ImportPrefs(
    /* [in] */ IInputStream* in) /*throws IOException, InvalidPreferencesFormatException */
{
    // try {
    // load XML document
    AutoPtr<IInputSource> is;
    AutoPtr<IDocument> doc;
    AutoPtr<IElement> preferences;
    String version, type;
    AutoPtr<INode> root;
    AutoPtr<INodeList> nl;
    AutoPtr<IPreferences> prefsRoot;
    ECode ec = NOERROR;

    //TODO
    assert(0);
    // FAIL_GOTO2(CInputSource::New(in, (IInputSource**)&is), Failed);
    FAIL_GOTO2(sBuilder->Parse(is, (IDocument**)&doc), Failed);

    // check preferences' export version
    FAIL_GOTO2(doc->GetDocumentElement((IElement**)&preferences), Failed);
    FAIL_GOTO2(preferences->GetAttribute(String("EXTERNAL_XML_VERSION"), &version), Failed);
    if (version != NULL && StringUtils::ParseFloat(version) > XML_VERSION) {
        // throw new InvalidPreferencesFormatException("Preferences version " + version +
        //         " is not supported");
        return E_INVALID_PREFERENCES_FORMAT_EXCEPTION;
    }

    // check preferences root's type
    FAIL_GOTO2(preferences->GetElementsByTagName(String("root"), (INodeList**)&nl), Failed);
    nl->Item(0, (INode**)&root);
    FAIL_GOTO2(IElement::Probe(root)->GetAttribute(String("type"), &type), Failed);

    if (type.Equals("user")) {
        FAIL_GOTO2(Preferences::UserRoot((IPreferences**)&prefsRoot), Failed);
    } else {
        FAIL_GOTO2(Preferences::SystemRoot((IPreferences**)&prefsRoot), Failed);
    }

Failed:
    if (FAILED(ec)) {
        return E_INVALID_PREFERENCES_FORMAT_EXCEPTION;
    }
    // load node
    return LoadNode(prefsRoot, IElement::Probe(root));
    // } catch (FactoryConfigurationError e) {
    //     throw new InvalidPreferencesFormatException(e);
    // } catch (SAXException e) {
    //     throw new InvalidPreferencesFormatException(e);
    // }
}

ECode XMLParser::LoadNode(
    /* [in] */ IPreferences* prefs,
    /* [in] */ IElement* node)
{
    // load preferences
    AutoPtr<INodeList> children = SelectNodeList(node, String("node"));
    AutoPtr<INodeList> entries = SelectNodeList(node, String("map/entry"));
    Int32 childNumber = 0;
    children->GetLength(&childNumber);
    AutoPtr<ArrayOf<AutoPtr<IPreferences> > > prefChildren = ArrayOf<AutoPtr<IPreferences> >::Alloc(childNumber);
    Int32 entryNumber = 0;
    entries->GetLength(&entryNumber);
    AutoPtr<AbstractPreferences> apf = (AbstractPreferences*)prefs;
    AutoPtr<Object> lock = &apf->mLock;
    {    AutoLock syncLock(lock);
        if (apf->IsRemoved()) {
            return NOERROR;
        }
        for (Int32 i = 0; i < entryNumber; i++) {
            AutoPtr<INode> entry;
            entries->Item(i, (INode**)&entry);
            String key;
            IElement::Probe(entry)->GetAttribute(String("key"), &key);
            String value;
            IElement::Probe(entry)->GetAttribute(String("value"), &value);
            prefs->Put(key, value);
        }
        // get children preferences node
        for (Int32 i = 0; i < childNumber; i++) {
            AutoPtr<INode> child;
            children->Item(i, (INode**)&child);
            String name;
            IElement::Probe(child)->GetAttribute(String("name"), &name);

            AutoPtr<IPreferences> pf;
            prefs->GetNode(name, (IPreferences**)&pf);
            prefChildren->Set(i, pf);
        }
    }

    // load children nodes after unlock
    for (Int32 i = 0; i < childNumber; i++) {
        AutoPtr<INode> child;
        children->Item(i, (INode**)&child);
        LoadNode((*prefChildren)[i], IElement::Probe(child));
    }

    return NOERROR;
}

AutoPtr<INodeList> XMLParser::SelectNodeList(
    /* [in] */ IElement* documentElement,
    /* [in] */ const String& string)
{
    AutoPtr<INodeList> result;

    AutoPtr<IArrayList> input;
    CArrayList::New((IArrayList**)&input);

    AutoPtr<ArrayOf<String> > path;
    StringUtils::Split(string, "/", (ArrayOf<String>**)&path);

    AutoPtr<INodeList> childNodes;
    INode::Probe(documentElement)->GetChildNodes((INodeList**)&childNodes);

    if((*path)[0].Equals("entry") || (*path)[0].Equals("node")) {
        Int32 len = 0;
        childNodes->GetLength(&len);
        for (Int32 i = 0; i < len; i++) {
            AutoPtr<INode> next;
            childNodes->Item(i, (INode**)&next);
            if(IElement::Probe(next)) {
                String nm;
                next->GetNodeName(&nm);
                if(nm.Equals((*path)[0])) {
                    IList::Probe(input)->Add(next);
                }
            }
        }
    } else if((*path)[0].Equals("map") && (*path)[1].Equals("entry")) {
        Int32 len = 0;
        childNodes->GetLength(&len);
        for (Int32 i = 0; i < len; i++) {
            AutoPtr<INode> next;
            childNodes->Item(i, (INode**)&next);
            if(IElement::Probe(next)) {
                String nm;
                next->GetNodeName(&nm);
                if(nm.Equals((*path)[0])) {
                    AutoPtr<INodeList> nextChildNodes;
                    next->GetChildNodes((INodeList**)&nextChildNodes);

                    Int32 len2 = 0;
                    nextChildNodes->GetLength(&len2);
                    for (Int32 j = 0; j < len2; j++) {
                        AutoPtr<INode> subnext;
                        nextChildNodes->Item(j, (INode**)&subnext);
                        if(IElement::Probe(subnext)) {
                            String str;
                            subnext->GetNodeName(&str);
                            if(str.Equals((*path)[1])) {
                                IList::Probe(input)->Add(subnext);
                            }
                        }
                    }
                }
            }
        }
    }

    AutoPtr<IIterator> iter;
    input->GetIterator((IIterator**)&iter);
    result = new NodeSet(iter);

    return result;
}

ECode XMLParser::ReadXmlPreferences(
    /* [in] */ IFile* xmlFile,
    /* [out] */ IProperties** per)
{
    VALIDATE_NOT_NULL(per);
    AutoPtr<IProperties> result;
    CProperties::New((IProperties**)&result);
    Boolean flag = FALSE;
    if (!(xmlFile->Exists(&flag), flag)) {
        AutoPtr<IFile> file;
        xmlFile->GetParentFile((IFile**)&file);
        Boolean succeeded = FALSE;
        file->Mkdirs(&succeeded);
    } else if (xmlFile->CanRead(&flag), flag) {
        AutoPtr<IReader> reader;
        AutoPtr<IFileInputStream> fis;
        AutoPtr<IInputSource> is;
        AutoPtr<IDocument> document;
        AutoPtr<IElement> el;
        Int32 length = 0;
        AutoPtr<INodeList> entries;
        FAIL_GOTO(CFileInputStream::New(xmlFile, (IFileInputStream**)&fis), finally);
        FAIL_GOTO(CInputStreamReader::New(IInputStream::Probe(fis), String("UTF-8"), (IReader**)&reader), finally);

        //TODO
        assert(0);
        // FAIL_GOTO(CInputSource::New(reader, (IInputSource**)&is), finally);
        sBuilder->Parse(is, (IDocument**)&document);

        FAIL_GOTO(document->GetDocumentElement((IElement**)&el), finally);
        entries = SelectNodeList(el, String("entry"));
        entries->GetLength(&length);
        for (Int32 i = 0; i < length; i++) {
            AutoPtr<INode> node;
            FAIL_GOTO(entries->Item(i, (INode**)&node), finally);
            String key, value, tmp;
            FAIL_GOTO(IElement::Probe(node)->GetAttribute(String("key"), &key), finally);
            FAIL_GOTO(IElement::Probe(node)->GetAttribute(String("value"), &value), finally);
            FAIL_GOTO(result->SetProperty(key, value, &tmp), finally);
        }

finally:
        //TODO
        assert(0);
        // IoUtils::CloseQuietly(reader);
    } else {
        // the prefs API requires this to be hostile towards pre-existing files
        xmlFile->Delete();
    }

    *per = result;
    REFCOUNT_ADD(*per);
    return NOERROR;
}

ECode XMLParser::WriteXmlPreferences(
    /* [in] */ IFile* xmlFile,
    /* [in] */ IProperties* properties) /*throws IOException*/
{
    AutoPtr<IFile> parent;
    xmlFile->GetParentFile((IFile**)&parent);
    AutoPtr<IFile> temporaryForWriting;
    String value;
    Int32 length = 0;
    ECode ec = NOERROR;
    Boolean ok = FALSE;
    AutoPtr<ArrayOf<String> > strKeys;
    AutoPtr<ArrayOf<String> > values;
    Int32 size = 0;
    AutoPtr<ISet> keySet;
    AutoPtr<ArrayOf<IInterface*> > keys;

    //TODO
    assert(0);
    String uuid/* = UUID.randomUUID()*/;
    FAIL_RETURN(CFile::New(parent, String("prefs-") + uuid + String(".xml.tmp"),
            (IFile**)&temporaryForWriting));

    Int32 i = 0;
    AutoPtr<IIterator> it;
    AutoPtr<IBufferedWriter> out;
    AutoPtr<IFileOutputStream> fs;
    CFileOutputStream::New(temporaryForWriting, (IFileOutputStream**)&fs);
    AutoPtr<IOutputStreamWriter> osw;
    COutputStreamWriter::New(IOutputStream::Probe(fs), String("UTF-8"), (IOutputStreamWriter**)&osw);
    CBufferedWriter::New(IWriter::Probe(osw), (IBufferedWriter**)&out);
    FAIL_GOTO(IWriter::Probe(out)->Write(HEADER), finally);
    FAIL_GOTO(out->NewLine(), finally);
    FAIL_GOTO(IWriter::Probe(out)->Write(FILE_PREFS), finally);
    FAIL_GOTO(out->NewLine(), finally);

    IMap::Probe(properties)->GetSize(&size);

    IMap::Probe(properties)->GetKeySet((ISet**)&keySet);
    strKeys = ArrayOf<String>::Alloc(size);

    keySet->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        strKeys->Set(i, TO_STR(obj));
    }

    length = strKeys->GetLength();
    values = ArrayOf<String>::Alloc(length);
    for (Int32 i = 0; i < length; i++) {
        properties->GetProperty((*strKeys)[i], &value);
        values->Set(i,value);
    }
    FAIL_GOTO(ExportEntries(strKeys, values, out), finally);
    FAIL_GOTO(ICloseable::Probe(out)->Close(), finally);

    FAIL_GOTO(temporaryForWriting->RenameTo(xmlFile, &ok), finally);
    if (!ok) {
        ec = E_IO_EXCEPTION;
        // throw new IOException("Failed to write preferences to " + xmlFile);
    }

finally:
    //TODO
    assert(0);
    // IoUtils::CloseQuietly(out);
    FAIL_RETURN(temporaryForWriting->Delete()); // no-op unless something failed
    return ec;
}

} // namespace Prefs
} // namespace Utility
} // namespace Elastos
