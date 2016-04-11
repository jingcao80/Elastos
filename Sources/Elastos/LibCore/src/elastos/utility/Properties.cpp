
#include "Properties.h"
#include "CInputStreamReader.h"
#include "CBufferedReader.h"
#include "COutputStreamWriter.h"
#include "CPrintStream.h"
#include "CCharsetHelper.h"
#include "CString.h"
#include "CDate.h"
#include "CHashTable.h"
#include "Collections.h"
#include "CSystem.h"
#include "StringBuilder.h"
#include "StringUtils.h"
#include "Character.h"
#include "AutoLock.h"
#include <cutils/log.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::Character;
using Elastos::Core::CString;
using Elastos::Core::ISystem;
using Elastos::Utility::Collections;
using Elastos::IO::IBufferedReader;
using Elastos::IO::IInputStreamReader;
using Elastos::IO::CInputStreamReader;
using Elastos::IO::CBufferedReader;
using Elastos::IO::IOutputStreamWriter;
using Elastos::IO::COutputStreamWriter;
using Elastos::IO::CPrintStream;
using Elastos::IO::IFlushable;
using Elastos::IO::Charset::ICharset;
using Elastos::IO::Charset::ICharsetHelper;
using Elastos::IO::Charset::CCharsetHelper;

namespace Elastos{
namespace Utility{

const String Properties::PROP_DTD_NAME =
    String("http://java.sun.com/dtd/properties.dtd");

const String Properties::PROP_DTD =
    String("<?xml version=\"1.0\" encoding=\"UTF-8\"?>")
        + String("    <!ELEMENT properties (comment?, entry*) >")
        + String("    <!ATTLIST properties version CDATA #FIXED \"1.0\" >")
        + String("    <!ELEMENT comment (#PCDATA) >")
        + String("    <!ELEMENT entry (#PCDATA) >")
        + String("    <!ATTLIST entry key CDATA #REQUIRED >");

const Int32 Properties::NONE = 0;
const Int32 Properties::SLASH = 1;
const Int32 Properties::UNICODE = 2;
const Int32 Properties::CONTINUE = 3;
const Int32 Properties::KEY_DONE = 4;
const Int32 Properties::IGNORE = 5;

static AutoPtr<ICharSequence> StrToCS(
    /* [in] */ const String& str)
{
    AutoPtr<ICharSequence> cs;
    CString::New(str, (ICharSequence**)&cs);
    return cs;
}

CAR_INTERFACE_IMPL(Properties, HashTable, IProperties)

Properties::Properties()
{
    HashTable::constructor();
}

ECode Properties::constructor(
    /* [in] */ Properties* properties)
{
    mDefaults = properties;
    return NOERROR;
}

void Properties::DumpString(
    /* [in] */ IStringBuilder* buffer,
    /* [in] */ const String& string,
    /* [in] */ Boolean key)
{
    AutoPtr<ArrayOf<Char32> > charArray = string.GetChars();
    Int32 i = 0;
    if (!key && i < charArray->GetLength() && (*charArray)[i] == ' ') {
        buffer->Append(String("\\ "));
        i++;
    }

    for (; i < charArray->GetLength(); i++) {
        Char32 ch = (*charArray)[i];
        switch (ch) {
        case '\t':
            buffer->Append(String("\\t"));
            break;
        case '\n':
            buffer->Append(String("\\n"));
            break;
        case '\f':
            buffer->Append(String("\\f"));
            break;
        case '\r':
            buffer->Append(String("\\r"));
            break;
        default:
            if (String("\\#!=:").IndexOf(ch) >= 0 || (key && ch == ' ')) {
                buffer->AppendChar('\\');
            }
            if (ch >= ' ' && ch <= '~') {
                buffer->AppendChar(ch);
            }
            else {
                String hex = StringUtils::ToHexString((Int32)ch);
                buffer->Append(String("\\u"));
                for (Int32 j = 0; j < 4 - hex.GetLength(); j++) {
                    buffer->Append(String("0"));
                }
                buffer->Append(hex);
            }
        }
    }
}

ECode Properties::GetProperty(
    /* [in] */ const String& name,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    AutoPtr<IInterface> result;
    HashTable::Get(StrToCS(name), (IInterface**)&result);
    String property;
    if (ICharSequence::Probe(result) != NULL)
        ICharSequence::Probe(result)->ToString(&property);
    if (property.IsNull() && mDefaults != NULL) {
        mDefaults->GetProperty(name, &property);
    }
    *str = property;
    return  NOERROR;
}

ECode Properties::GetProperty(
    /* [in] */ const String& name,
    /* [in] */ const String& defaultValue,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    AutoPtr<IInterface> result;
    HashTable::Get(StrToCS(name), (IInterface**)&result);
    String property;
    if (ICharSequence::Probe(result) != NULL)
        ICharSequence::Probe(result)->ToString(&property);
    if (property.IsNull() && mDefaults != NULL) {
        mDefaults->GetProperty(name, &property);
    }
    if (property.IsNull())
        property = defaultValue;
    *str = property;
    return  NOERROR;
}

ECode Properties::List(
    /* [in] */ IPrintStream* outstream)
{
    return ListToAppendable(IAppendable::Probe(outstream));
}

ECode Properties::List(
    /* [in] */ IPrintWriter* outwriter)
{
    return ListToAppendable(IAppendable::Probe(outwriter));
}

ECode Properties::ListToAppendable(
    /* [in] */ IAppendable* out)
{
    if (out == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<StringBuilder> sb = new StringBuilder(80);
    AutoPtr<IEnumeration> keys;
    PropertyNames((IEnumeration**)&keys);
    Boolean hasFlag(FALSE);

    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    String separator;
    system->GetLineSeparator(&separator);
    while (keys->HasMoreElements(&hasFlag), hasFlag) {
        AutoPtr<IInterface> iKey;
        keys->GetNextElement((IInterface**)&iKey);
        String key;
        if (ICharSequence::Probe(iKey))
            ICharSequence::Probe(iKey)->ToString(&key);
        sb->Append(key);
        sb->AppendChar('=');

        AutoPtr<IInterface> result;
        HashTable::Get(iKey, (IInterface**)&result);
        String property;
        if (ICharSequence::Probe(result) != NULL)
            ICharSequence::Probe(result)->ToString(&property);
        AutoPtr<Properties> def = mDefaults;
        while (property.IsNull()) {
            AutoPtr<IInterface> res;
            def->Get(StrToCS(key), (IInterface**)&res);
            if (ICharSequence::Probe(res) != NULL)
                ICharSequence::Probe(res)->ToString(&property);
            def = def->mDefaults;
        }
        if (property.GetLength() > 40) {
            sb->Append(property.Substring(0, 37));
            sb->Append(String("..."));
        }
        else {
            sb->Append(property);
        }
        sb->Append(separator);
        out->Append(sb->ToCharSequence());
        sb->SetLength(0);
    }
    return NOERROR;
}

ECode Properties::Load(
    /* [in] */ IInputStream* instream)
{
    if (instream == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<IInputStreamReader> inreader;
    FAIL_RETURN(CInputStreamReader::New(instream,String("ISO-8859-1"), (IInputStreamReader**)&inreader));
    return Load(IReader::Probe(inreader));
}

ECode Properties::Load(
    /* [in] */ IReader* inreader)
{
    AutoLock lock(this);
    if (inreader == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    Int32 mode = NONE, unicode = 0, count = 0;
    Char32 nextChar;
    AutoPtr<ArrayOf<Char32> > buf = ArrayOf<Char32>::Alloc(40);
    Int32 offset = 0, keyLength = -1, intVal = 0;
    Boolean firstChar = TRUE;

    AutoPtr<IBufferedReader> br;
    FAIL_RETURN(CBufferedReader::New(inreader, (IBufferedReader**)&br));

    while (TRUE) {
        (IReader::Probe(br))->Read(&intVal);
        if (intVal == -1) {
            break;
        }
        nextChar = (Char32)intVal;

        if (offset == buf->GetLength()) {
            AutoPtr<ArrayOf<Char32> > newBuf = ArrayOf<Char32>::Alloc(buf->GetLength() * 2);
            newBuf->Copy(buf);
            buf = newBuf;
        }
        if (mode == UNICODE) {
            Int32 digit = Character::ToDigit(nextChar, 16);
            if (digit >= 0) {
                unicode = (unicode << 4) + digit;
                if (++count < 4) {
                    continue;
                }
            }
            else if (count <= 4) {
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            mode = NONE;
            (*buf)[offset++] = (Char32)unicode;
            if (nextChar != '\n') {
                continue;
            }
        }
        if (mode == SLASH) {
            mode = NONE;
            switch (nextChar) {
                case '\r':
                    mode = CONTINUE; // Look for a following \n
                    continue;
                case '\n':
                    mode = IGNORE; // Ignore whitespace on the next line
                    continue;
                case 'b':
                    nextChar = '\b';
                    break;
                case 'f':
                    nextChar = '\f';
                    break;
                case 'n':
                    nextChar = '\n';
                    break;
                case 'r':
                    nextChar = '\r';
                    break;
                case 't':
                    nextChar = '\t';
                    break;
                case 'u':
                    mode = UNICODE;
                    unicode = count = 0;
                    continue;
            }
        }
        else {
            switch (nextChar) {
            case '#':
            case '!':
                if (firstChar) {
                    while (TRUE) {
                        (IReader::Probe(br))->Read(&intVal);
                        if (intVal == -1) {
                            break;
                        }
                        nextChar = (Char32)intVal;
                        if (nextChar == '\r' || nextChar == '\n') {
                            break;
                        }
                    }
                    continue;
                }
                break;
            case '\n':
                if (mode == CONTINUE) { // Part of a \r\n sequence
                    mode = IGNORE; // Ignore whitespace on the next line
                    continue;
                }
                // fall into the next case
            case '\r':
                mode = NONE;
                firstChar = TRUE;
                if (offset > 0 || (offset == 0 && keyLength == 0)) {
                    if (keyLength == -1) {
                        keyLength = offset;
                    }
                    String temp(*buf, offset);
                    AutoPtr<IInterface> res;
                    Put(StrToCS(temp.Substring(0, keyLength)),
                        StrToCS(temp.Substring(keyLength)),
                        (IInterface**)&res);
                }
                keyLength = -1;
                offset = 0;
                continue;
            case '\\':
                if (mode == KEY_DONE) {
                    keyLength = offset;
                }
                mode = SLASH;
                continue;
            case ':':
            case '=':
                if (keyLength == -1) { // if parsing the key
                    mode = NONE;
                    keyLength = offset;
                    continue;
                }
                break;
            }
            if (Character::IsWhitespace(nextChar)) {
                if (mode == CONTINUE) {
                    mode = IGNORE;
                }
                // if key length == 0 or value length == 0
                if (offset == 0 || offset == keyLength || mode == IGNORE) {
                    continue;
                }
                if (keyLength == -1) { // if parsing the key
                    mode = KEY_DONE;
                    continue;
                }
            }
            if (mode == IGNORE || mode == CONTINUE) {
                mode = NONE;
            }
        }
        firstChar = FALSE;
        if (mode == KEY_DONE) {
            keyLength = offset;
            mode = NONE;
        }
        (*buf)[offset++] = nextChar;
    }
    if (mode == UNICODE && count <= 4) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (keyLength == -1 && offset > 0) {
        keyLength = offset;
    }
    if (keyLength >= 0) {
        String temp(*buf, offset);
        String key = temp.Substring(0, keyLength);
        String value = temp.Substring(keyLength);
        if (mode == SLASH) {
            value += String("\u0000");
        }
        AutoPtr<IInterface> res;
        Put(StrToCS(key), StrToCS(value), (IInterface**)&res);
    }
    return  NOERROR;
}

ECode Properties::PropertyNames(
    /* [out] */ IEnumeration** names)
{
    AutoPtr<IMap> selected;
    CHashTable::New((IMap**)&selected);
    SelectProperties(selected, FALSE);
    return IDictionary::Probe(selected)->GetKeys(names);
}

ECode Properties::StringPropertyNames(
    /* [out] */ ISet** strNames)
{
    AutoPtr<IMap> stringProperties;
    CHashTable::New((IMap**)&stringProperties);
    SelectProperties(stringProperties, TRUE);
    AutoPtr<ISet> keySet;
    stringProperties->GetKeySet((ISet**)&keySet);
    return Collections::UnmodifiableSet(keySet, strNames);
}

ECode Properties::SelectProperties(
    /* [in] */ IMap* selectProperties,
    /* [in] */ Boolean isStringOnly)
{
    if (mDefaults != NULL) {
        mDefaults->SelectProperties(selectProperties, isStringOnly);
    }
    AutoPtr<IEnumeration> keys;
    GetKeys((IEnumeration**)&keys);
    Boolean hasFlag = FALSE;
    while (keys->HasMoreElements(&hasFlag), hasFlag) {
        // @SuppressWarnings("unchecked")
        AutoPtr<IInterface> key;
        keys->GetNextElement((IInterface**)&key);
        if (isStringOnly && !ICharSequence::Probe(key)) {
            // Only select property with string key and value
            continue;
        }
        AutoPtr<IInterface> value, oldValue;
        Get(key, (IInterface**)&value);
        selectProperties->Put(key, value, (IInterface**)&oldValue);
    }
    return NOERROR;
}

ECode Properties::Save(
    /* [in] */ IOutputStream* outstream,
    /* [in] */ const String& comment)
{
    return Store(outstream, comment);
}

ECode Properties::SetProperty(
    /* [in] */ const String& name,
    /* [in] */ const String& value,
    /* [out] */ String* oldValue)
{
    assert(!name.IsNull() && !value.IsNull());

    AutoPtr<IInterface> obj;
    AutoPtr<ICharSequence> seqName = StrToCS(name);
    AutoPtr<ICharSequence> seqValue = StrToCS(value);
    ECode ec = HashTable::Put(seqName, seqValue, (IInterface**)&obj);
    if (oldValue) {
        *oldValue = NULL;
        if (SUCCEEDED(ec)) {
            if (ICharSequence::Probe(obj)) {
                ICharSequence::Probe(obj)->ToString(oldValue);
            }
        }
    }
    return ec;
}

ECode Properties::Store(
    /* [in] */ IOutputStream* outstream,
    /* [in] */ const String& comment)
{
    AutoPtr<IOutputStreamWriter> outwriter;
    FAIL_RETURN(COutputStreamWriter::New(outstream,String("ISO-8859-1"), (IOutputStreamWriter**)&outwriter));
    return Store((IWriter::Probe(outwriter)), comment);
}

ECode Properties::Store(
    /* [in] */ IWriter* writer,
    /* [in] */ const String& comment)
{
    AutoLock lock(this);

    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    String lineSeparator;
    system->GetLineSeparator(&lineSeparator);
    if (!comment.IsNull()) {
        writer->Write(String("#"));
        writer->Write(comment);
        writer->Write(lineSeparator);
    }
    writer->Write(String("#"));
    AutoPtr<IDate> nowdate;
    FAIL_RETURN(CDate::New((IDate**)&nowdate));
    String outstr;
    IObject::Probe(nowdate)->ToString(&outstr);
    writer->Write(outstr);
    writer->Write(lineSeparator);

    AutoPtr<StringBuilder> sb = new StringBuilder(200);
    AutoPtr<ISet> entries;
    FAIL_RETURN(GetEntrySet((ISet**)&entries));
    AutoPtr<ArrayOf<IInterface*> > outarr;
    (ICollection::Probe(entries))->ToArray((ArrayOf<IInterface*>**)&outarr);
    for (Int32 i = 0; i < outarr->GetLength(); i++) {
        AutoPtr<IMapEntry> e = IMapEntry::Probe((*outarr)[i]);
        AutoPtr<IInterface> iKey;
        e->GetKey((IInterface**)&iKey);
        AutoPtr<IInterface> iValue;
        e->GetValue((IInterface**)&iValue);

        String key;
        ICharSequence::Probe(iKey)->ToString(&key);
        DumpString(sb, key, TRUE);
        sb->AppendChar('=');
        String value;
        ICharSequence::Probe(iValue)->ToString(&value);
        DumpString(sb, value, FALSE);
        sb->Append(lineSeparator);
        writer->Write(sb->ToString());
        sb->SetLength(0);
    }
    IFlushable::Probe(writer)->Flush();
    return  NOERROR;
}

ECode Properties::LoadFromXML(
    /* [in] */ IInputStream* instream)
{
    AutoLock lock(this);
    if (instream == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }

    assert(0 && "TODO: DocumentBuilder is not implemented!");
    return E_NOT_IMPLEMENTED;
    // if (builder == null) {
    //     DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
    //     // BEGIN android-removed: we still don't support validation.
    //     // factory.setValidating(true);
    //     // END android-removed

    //     try {
    //         builder = factory.newDocumentBuilder();
    //     } catch (ParserConfigurationException e) {
    //         throw new Error(e);
    //     }

    //     builder.setErrorHandler(new ErrorHandler() {
    //         public void warning(SAXParseException e) throws SAXException {
    //             throw e;
    //         }

    //         public void error(SAXParseException e) throws SAXException {
    //             throw e;
    //         }

    //         public void fatalError(SAXParseException e) throws SAXException {
    //             throw e;
    //         }
    //     });

    //     builder.setEntityResolver(new EntityResolver() {
    //         public InputSource resolveEntity(String publicId,
    //                 String systemId) throws SAXException, IOException {
    //             if (systemId.equals(PROP_DTD_NAME)) {
    //                 InputSource result = new InputSource(new StringReader(
    //                         PROP_DTD));
    //                 result.setSystemId(PROP_DTD_NAME);
    //                 return result;
    //             }
    //             throw new SAXException("Invalid DOCTYPE declaration: "
    //                     + systemId);
    //         }
    //     });
    // }

    // try {
    //     Document doc = builder.parse(instream);
    //     NodeList entries = doc.getElementsByTagName("entry");
    //     if (entries == null) {
    //         return;
    //     }
    //     int entriesListLength = entries.getLength();

    //     for (int i = 0; i < entriesListLength; i++) {
    //         Element entry = (Element) entries.item(i);
    //         String key = entry.getAttribute("key");
    //         String value = entry.getTextContent();

    //         /*
    //         * key != null & value != null but key or(and) value can be
    //         * empty String
    //          */
    //         put(key, value);
    //     }
    // } catch (IOException e) {
    //     throw e;
    // } catch (SAXException e) {
    //     throw new InvalidPropertiesFormatException(e);
    // }
    return  NOERROR;
}

ECode Properties::StoreToXML(
    /* [in] */ IOutputStream* os,
    /* [in] */ const String& comment)
{
    return StoreToXML(os, comment, String("UTF-8"));
}

ECode Properties::StoreToXML(
    /* [in] */ IOutputStream* os,
    /* [in] */ const String& comment,
    /* [in] */ const String& encoding)
{
    AutoLock lock(this);
    if (os == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    else if (encoding.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }

    /*
    * We can write to XML file using encoding parameter but note that some
    * aliases for encodings are not supported by the XML parser. Thus we
    * have to know canonical name for encoding used to store data in XML
    * since the XML parser must recognize encoding name used to store data.
     */

    String encodingCanonicalName("UTF-8");
    AutoPtr<ICharsetHelper> sethelper;
    CCharsetHelper::AcquireSingleton((ICharsetHelper **)&sethelper);
    AutoPtr<ICharset> charset;
    if (SUCCEEDED(sethelper->ForName(encoding, (ICharset**)&charset))) {
        charset->GetName(&encodingCanonicalName);
    }
    // catch (IllegalCharsetNameException e) {
    //     System.out.println("Warning: encoding name " + encoding
    //             + " is illegal, using UTF-8 as default encoding");
    //     encodingCanonicalName = "UTF-8";
    // } catch (UnsupportedCharsetException e) {
    //     System.out.println("Warning: encoding " + encoding
    //             + " is not supported, using UTF-8 as default encoding");
    //     encodingCanonicalName = "UTF-8";
    // }

    AutoPtr<IPrintStream> printStream;
    FAIL_RETURN(CPrintStream::New(os, FALSE, encodingCanonicalName , (IPrintStream **)&printStream));

    printStream->Print(String("<?xml version=\"1.0\" encoding=\""));
    printStream->Print(encodingCanonicalName);
    printStream->Println(String("\"?>"));

    printStream->Print(String("<!DOCTYPE properties SYSTEM \""));
    printStream->Print(PROP_DTD_NAME);
    printStream->Println(String("\">"));

    printStream->Println(String("<properties>"));

    if (!comment.IsNull()) {
        printStream->Print(String("<comment>"));
        printStream->Print(SubstitutePredefinedEntries(comment));
        printStream->Println(String("</comment>"));
    }

    AutoPtr<ISet> entries;
    FAIL_RETURN(GetEntrySet((ISet**)&entries));
    AutoPtr<ArrayOf<IInterface*> > outarr;
    (ICollection::Probe(entries))->ToArray((ArrayOf<IInterface*>**)&outarr);
    for (Int32 i = 0; i < outarr->GetLength(); i++) {
        AutoPtr<IMapEntry> e = IMapEntry::Probe((*outarr)[i]);
        AutoPtr<IInterface> iKey;
        e->GetKey((IInterface**)&iKey);
        AutoPtr<IInterface> iValue;
        e->GetValue((IInterface**)&iValue);
        String key;
        ICharSequence::Probe(iKey)->ToString(&key);
        String value;
        ICharSequence::Probe(iValue)->ToString(&value);
        printStream->Print(String("<entry key=\""));
        printStream->Print(SubstitutePredefinedEntries(key));
        printStream->Print(String("\">"));
        printStream->Print(SubstitutePredefinedEntries(value));
        printStream->Println(String("</entry>"));
    }
    printStream->Println(String("</properties>"));
    IFlushable::Probe(printStream)->Flush();
    return  NOERROR;
}

String Properties::SubstitutePredefinedEntries(
    /* [in] */ const String& str)
{
    String s, t;
    StringUtils::ReplaceAll(str, String("&"), String("&amp;"), &s);
    StringUtils::ReplaceAll(s, String("<"), String("&lt;"), &t);
    StringUtils::ReplaceAll(t, String(">"), String("&gt;"), &s);
    StringUtils::ReplaceAll(s, String("'"), String("&apos;"), &t);
    StringUtils::ReplaceAll(t, String("\""), String("&quot;"), &s);
    return s;
}

} // namespace Utility
} // namespace Elastos
