
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/internal/utility/XmlUtils.h"
#include "elastos/droid/utility/Xml.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>
#include "elastos/droid/graphics/CBitmapFactory.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/internal/utility/CFastXmlSerializer.h"
#include "elastos/droid/utility/CBase64.h"

using Elastos::Droid::Graphics::BitmapCompressFormat_PNG;
using Elastos::Droid::Graphics::CBitmapFactory;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Utility::IBase64;
using Elastos::Droid::Utility::CBase64;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::Internal::Utility::CFastXmlSerializer;
using Elastos::Droid::Internal::Utility::IFastXmlSerializer;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::CInteger32;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger64;
using Elastos::Core::IInteger64;
using Elastos::Core::CBoolean;
using Elastos::Core::IBoolean;
using Elastos::Core::CFloat;
using Elastos::Core::IFloat;
using Elastos::Core::CDouble;
using Elastos::Core::IDouble;
using Elastos::Core::IByte;
using Elastos::Core::CString;
using Elastos::Core::CArrayOf;
using Elastos::Core::IArrayOf;
using Elastos::Core::IString;
using Elastos::Core::EIID_IByte;
using Elastos::Core::EIID_IDouble;
using Elastos::Core::EIID_IInteger32;
using Elastos::Core::EIID_IInteger64;
using Elastos::Core::EIID_IString;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CHashSet;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Slogger;
using Org::Kxml2::IO::CKXmlSerializer;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

template<typename T, typename IT>
static AutoPtr<ArrayOf<T> > IArrayOfToArray(
    /* [in] */ IArrayOf* iarray,
    /* [in] */ T typeDeduceObj1,
    /* [in] */ IT* typeDeduceObj2)
{
    if (iarray == NULL)
        return NULL;
    Int32 size;
    iarray->GetLength(&size);
    AutoPtr<ArrayOf<T> > array = size > 0 ? ArrayOf<T>::Alloc(size) : NULL;
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> item;
        iarray->Get(i, (IInterface**)&item);
        T v;
        IT::Probe(item)->GetValue(&v);
        (*array)[i] = v;
    }
    return array;
}

const String XmlUtils::TAG("XmlUtils");

ECode XmlUtils::SkipCurrentTag(
    /* [in] */ IXmlPullParser* parser)
{
    Int32 outerDepth = 0;
    parser->GetDepth(&outerDepth);
    Int32 type = 0;
    Int32 depth = 0;
    while ((parser->Next(&type), type) != IXmlPullParser::END_DOCUMENT
           && (type != IXmlPullParser::END_TAG
                   || (parser->GetDepth(&depth), depth) > outerDepth)) {
    }
    return NOERROR;
}

Int32 XmlUtils::ConvertValueToList(
    /* [in] */ ICharSequence* value,
    /* [in] */ const ArrayOf<String>& options,
    /* [in] */ Int32 defaultValue)
{
    if (NULL != value) {
        String str;
        value->ToString(&str);

        for (Int32 i = 0; i < options.GetLength(); i++) {
            if (str.Equals(options[i]))
                return i;
        }
    }

    return defaultValue;
}

Boolean XmlUtils::ConvertValueToBoolean(
    /* [in] */ const String& value,
    /* [in] */ Boolean defaultValue)
{
    Boolean result = FALSE;

    if (value.IsNull()) return defaultValue;

    if (value.Equals("1") ||  value.Equals("true") ||  value.Equals("TRUE")) {
        result = TRUE;
    }

    return result;
}

Boolean XmlUtils::ConvertValueToBoolean(
    /* [in] */ ICharSequence* value,
    /* [in] */ Boolean defaultValue)
{
    if (NULL == value) return defaultValue;

    String str;
    value->ToString(&str);
    return ConvertValueToBoolean(str, defaultValue);
}

Int32 XmlUtils::ConvertValueToInt32(
    /* [in] */ ICharSequence* charSeq,
    /* [in] */ Int32 defaultValue)
{
    if (NULL == charSeq) return defaultValue;

    String nm;
    charSeq->ToString(&nm);

    // XXX This code is copied from Integer.decode() so we don't
    // have to instantiate an Integer!

    Int32 sign = 1;
    Int32 index = 0;
    AutoPtr<ArrayOf<Char32> > charArray = nm.GetChars();
    Int32 len = charArray->GetLength();
    Int32 base = 10;

    if ('-' == (*charArray)[0]) {
        sign = -1;
        index++;
    }

    if ('0' == (*charArray)[index]) {
        //  Quick check for a zero by itself
        if (index == (len - 1)) return 0;

        Char32 c = (*charArray)[index + 1];

        if ('x' == c || 'X' == c) {
            index += 2;
            base = 16;
        }
        else {
            index++;
            base = 8;
        }
    }
    else if ('#' == (*charArray)[index]) {
        index++;
        base = 16;
    }

    return StringUtils::ParseInt32(nm.Substring(index), base) * sign;
}

Int32 XmlUtils::ConvertValueToUnsignedInt(
    /* [in] */ const String& value,
    /* [in] */ Int32 defaultValue)
{
    if (value.IsNull()) return defaultValue;

    return ParseUnsignedIntAttribute(value);
}

Int32 XmlUtils::ParseUnsignedIntAttribute(
    /* [in] */ const String& charSeq)
{
    String value = charSeq;

    // Int64 bits;
    Int32 index = 0;
    Int32 len = value.GetLength();
    Int32 base = 10;

    if ('0' == value.GetChar(index)) {
        //  Quick check for zero by itself
        if (index == (len - 1)) {
            return 0;
        }

        Char32 c = value.GetChar(index + 1);

        if ('x' == c || 'X' == c) {     //  check for hex
            index += 2;
            base = 16;
        }
        else {                        //  check for octal
            index++;
            base = 8;
        }
    }
    else if ('#' == value.GetChar(index)) {
        index++;
        base = 16;
    }

    return (Int32) StringUtils::ParseInt64(value.Substring(index), base);
}

ECode XmlUtils::WriteMapXml(
    /* [in] */ IMap* val,
    /* [in] */ IOutputStream* out) //throws XmlPullParserException, java.io.IOException
{
    AutoPtr<IXmlSerializer> serializer;
    CFastXmlSerializer::New((IXmlSerializer**)&serializer);
    FAIL_RETURN(serializer->SetOutput(out, String("utf-8")))
    FAIL_RETURN(serializer->StartDocument(String(NULL), TRUE))
    FAIL_RETURN(serializer->SetFeature(String("http://xmlpull.org/v1/doc/features.html#indent-output"), TRUE))
    FAIL_RETURN(WriteMapXml(val, String(NULL), serializer))
    return serializer->EndDocument();
}

ECode XmlUtils::WriteListXml(
    /* [in] */ IList* val,
    /* [in] */ IOutputStream* out) //throws XmlPullParserException, java.io.IOException
{
    AutoPtr<IXmlSerializer> serializer;
    Xml::NewSerializer((IXmlSerializer**)&serializer);
    // CKXmlSerializer::New((IXmlSerializer**)&serializer);
    FAIL_RETURN(serializer->SetOutput(out, String("utf-8")))
    FAIL_RETURN(serializer->StartDocument(String(NULL), TRUE))
    FAIL_RETURN(serializer->SetFeature(String("http://xmlpull.org/v1/doc/features.html#indent-output"), TRUE))
    FAIL_RETURN(WriteListXml(val, String(NULL), serializer))
    return serializer->EndDocument();
}

ECode XmlUtils::WriteMapXml(
    /* [in] */ IMap* val,
    /* [in] */ const String& name,
    /* [in] */ IXmlSerializer* out) //throws XmlPullParserException, java.io.IOException
{
    return WriteMapXml(val, name, out, NULL);
}

ECode XmlUtils::WriteMapXml(
    /* [in] */ IMap* val,
    /* [in] */ const String& name,
    /* [in] */ IXmlSerializer* out,
    /* [in] */ IXmlUtilsWriteMapCallback* callback) //throws XmlPullParserException, java.io.IOException
{
    if (val == NULL) {
        FAIL_RETURN(out->WriteStartTag(String(NULL), String("NULL")))
        FAIL_RETURN(out->WriteEndTag(String(NULL), String("NULL")))
        return NOERROR;
    }

    FAIL_RETURN(out->WriteStartTag(String(NULL), String("map")))
    if (name != NULL) {
        FAIL_RETURN(out->WriteAttribute(String(NULL), String("name"), name))
    }

    FAIL_RETURN(WriteMapXml(val, out, callback))

    return out->WriteEndTag(String(NULL), String("map"));
}

ECode XmlUtils::WriteMapXml(
    /* [in] */ IMap* val,
    /* [in] */ IXmlSerializer* out,
    /* [in] */ IXmlUtilsWriteMapCallback* callback) //throws XmlPullParserException, java.io.IOException
{
    if (val == NULL) {
        return NOERROR;
    }

    AutoPtr<ISet> s;
    val->GetEntrySet((ISet**)&s);
    AutoPtr<IIterator> i;
    s->GetIterator((IIterator**)&i);

    Boolean hasNext;
    while (i->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> object;
        i->GetNext((IInterface**)&object);
        AutoPtr<IMapEntry> e = IMapEntry::Probe(object);
        AutoPtr<IInterface> value, key;
        e->GetValue((IInterface**)&value);
        e->GetKey((IInterface**)&key);
        String k;
        assert(IString::Probe(key) != NULL);
        ICharSequence::Probe(key)->ToString(&k);
        FAIL_RETURN(WriteValueXml(value, k, out, callback))
    }
    return NOERROR;
}

ECode XmlUtils::WriteListXml(
    /* [in] */ IList* val,
    /* [in] */ const String& name,
    /* [in] */ IXmlSerializer* out) //throws XmlPullParserException, java.io.IOException
{
    if (val == NULL) {
        FAIL_RETURN(out->WriteStartTag(String(NULL), String("NULL")))
        return out->WriteEndTag(String(NULL), String("NULL"));
    }

    FAIL_RETURN(out->WriteStartTag(String(NULL), String("list")))
    if (!name.IsNull()) {
        FAIL_RETURN(out->WriteAttribute(String(NULL), String("name"), name))
    }

    Int32 N;
    val->GetSize(&N);
    Int32 i = 0;
    while (i < N) {
        AutoPtr<IInterface> value;
        val->Get(i, (IInterface**)&value);
        FAIL_RETURN(WriteValueXml(value, String(NULL), out))
        i++;
    }

    return out->WriteEndTag(String(NULL), String("list"));
}

ECode XmlUtils::WriteSetXml(
    /* [in] */ ISet* val,
    /* [in] */ const String& name,
    /* [in] */ IXmlSerializer* out) //throws XmlPullParserException, java.io.IOException
{
    if (val == NULL) {
        FAIL_RETURN(out->WriteStartTag(String(NULL), String("NULL")))
        return out->WriteEndTag(String(NULL), String("NULL"));
    }

    FAIL_RETURN(out->WriteStartTag(String(NULL), String("set")))
    if (!name.IsNull()) {
        FAIL_RETURN(out->WriteAttribute(String(NULL), String("name"), name))
    }

    AutoPtr<IIterator> iter;
    val->GetIterator((IIterator**)&iter);
    Boolean hasNext = FALSE;
    while (iter->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> value;
        iter->GetNext((IInterface**)&value);
        FAIL_RETURN(WriteValueXml(value, String(NULL), out))
    }

    return out->WriteEndTag(String(NULL), String("set"));
}

ECode XmlUtils::WriteByteArrayXml(
    /* [in] */ ArrayOf<Byte>* val,
    /* [in] */ const String& name,
    /* [in] */ IXmlSerializer* out) //throws XmlPullParserException, java.io.IOException
{
    if (val == NULL) {
        FAIL_RETURN(out->WriteStartTag(String(NULL), String("NULL")))
        return out->WriteEndTag(String(NULL), String("NULL"));
    }

    FAIL_RETURN(out->WriteStartTag(String(NULL), String("byte-array")))
    if (!name.IsNull()) {
        FAIL_RETURN(out->WriteAttribute(String(NULL), String("name"), name))
    }

    Int32 N = val->GetLength();
    FAIL_RETURN(out->WriteAttribute(String(NULL), String("num"), StringUtils::ToString(N)))

    AutoPtr<StringBuilder> sb = new StringBuilder(N * 2);
    for (Int32 i = 0; i < N; i++) {
        Int32 b = (*val)[i];
        Int32 h = b >> 4;
        sb->Append(h >= 10 ? ('a' + h - 10) : ('0' + h));
        h = b & 0xff;
        sb->Append(h >= 10 ? ('a' + h - 10) : ('0' + h));
    }

    FAIL_RETURN(out->WriteText(sb->ToString()))

    return out->WriteEndTag(String(NULL), String("byte-array"));
}

ECode XmlUtils::WriteInt32ArrayXml(
    /* [in] */ ArrayOf<Int32>* val,
    /* [in] */ const String& name,
    /* [in] */ IXmlSerializer* out)//throws XmlPullParserException, java.io.IOException
{
    if (val == NULL) {
        FAIL_RETURN(out->WriteStartTag(String(NULL), String("NULL")))
        return out->WriteEndTag(String(NULL), String("NULL"));
    }

    FAIL_RETURN(out->WriteStartTag(String(NULL), String("Int32-array")))
    if (!name.IsNull()) {
        FAIL_RETURN(out->WriteAttribute(String(NULL), String("name"), name))
    }

    Int32 N = val->GetLength();
    FAIL_RETURN(out->WriteAttribute(String(NULL), String("num"), StringUtils::ToString(N)))

    for (Int32 i = 0; i < N; i++) {
        FAIL_RETURN(out->WriteStartTag(String(NULL), String("item")))
        Int32 v = (*val)[i];
        FAIL_RETURN(out->WriteAttribute(String(NULL), String("value"), StringUtils::ToString(v)))
        FAIL_RETURN(out->WriteEndTag(String(NULL), String("item")))
    }

    return out->WriteEndTag(String(NULL), String("Int32-array"));
}

ECode XmlUtils::WriteInt64ArrayXml(
    /* [in] */ ArrayOf<Int64>* val,
    /* [in] */ const String& name,
    /* [in] */ IXmlSerializer* out)//throws XmlPullParserException, java.io.IOException
{
    if (val == NULL) {
        FAIL_RETURN(out->WriteStartTag(String(NULL), String("NULL")))
        return out->WriteEndTag(String(NULL), String("NULL"));
    }

    FAIL_RETURN(out->WriteStartTag(String(NULL), String("Int64-array")))
    if (!name.IsNull()) {
        FAIL_RETURN(out->WriteAttribute(String(NULL), String("name"), name))
    }

    Int32 N = val->GetLength();
    FAIL_RETURN(out->WriteAttribute(String(NULL), String("num"), StringUtils::ToString(N)))

    for (Int32 i = 0; i < N; i++) {
        FAIL_RETURN(out->WriteStartTag(String(NULL), String("item")))
        Int64 v = (*val)[i];
        FAIL_RETURN(out->WriteAttribute(String(NULL), String("value"), StringUtils::ToString(v)))
        FAIL_RETURN(out->WriteEndTag(String(NULL), String("item")))
    }

    return out->WriteEndTag(String(NULL), String("Int64-array"));
}

ECode XmlUtils::WriteDoubleArrayXml(
    /* [in] */ ArrayOf<Double>* val,
    /* [in] */ const String& name,
    /* [in] */ IXmlSerializer* out)//throws XmlPullParserException, java.io.IOException
{
    if (val == NULL) {
        FAIL_RETURN(out->WriteStartTag(String(NULL), String("NULL")))
        return out->WriteEndTag(String(NULL), String("NULL"));
    }

    FAIL_RETURN(out->WriteStartTag(String(NULL), String("Double-array")))
    if (!name.IsNull()) {
        FAIL_RETURN(out->WriteAttribute(String(NULL), String("name"), name))
    }

    Int32 N = val->GetLength();
    FAIL_RETURN(out->WriteAttribute(String(NULL), String("num"), StringUtils::ToString(N)))

    for (Int32 i = 0; i < N; i++) {
        FAIL_RETURN(out->WriteStartTag(String(NULL), String("item")))
        FAIL_RETURN(out->WriteAttribute(String(NULL), String("value"), StringUtils::ToString((*val)[i])))
        FAIL_RETURN(out->WriteEndTag(String(NULL), String("item")))
    }

    return out->WriteEndTag(String(NULL), String("Double-array"));
}

ECode XmlUtils::WriteStringArrayXml(
    /* [in] */ ArrayOf<String>* val,
    /* [in] */ const String& name,
    /* [in] */ IXmlSerializer* out)//throws XmlPullParserException, java.io.IOException
{
    if (val == NULL) {
        FAIL_RETURN(out->WriteStartTag(String(NULL), String("NULL")))
        return out->WriteEndTag(String(NULL), String("NULL"));
    }

    FAIL_RETURN(out->WriteStartTag(String(NULL), String("String-array")))
    if (!name.IsNull()) {
        FAIL_RETURN(out->WriteAttribute(String(NULL), String("name"), name))
    }

    Int32 N = val->GetLength();
    FAIL_RETURN(out->WriteAttribute(String(NULL), String("num"), StringUtils::ToString(N)))

    for (Int32 i = 0; i < N; i++) {
        FAIL_RETURN(out->WriteStartTag(String(NULL), String("item")))
        FAIL_RETURN(out->WriteAttribute(String(NULL), String("value"), (*val)[i]))
        FAIL_RETURN(out->WriteEndTag(String(NULL), String("item")))
    }

    return out->WriteEndTag(String(NULL), String("String-array"));
}

ECode XmlUtils::WriteValueXml(
    /* [in] */ IInterface* v,
    /* [in] */ const String& name,
    /* [in] */ IXmlSerializer* out) //throws XmlPullParserException, java.io.IOException
{
    return WriteValueXml(v, name, out, NULL);
}

ECode XmlUtils::WriteValueXml(
    /* [in] */ IInterface* v,
    /* [in] */ const String& name,
    /* [in] */ IXmlSerializer* out,
    /* [in] */ IXmlUtilsWriteMapCallback* callback) //throws XmlPullParserException, java.io.IOException
{
    String typeStr, value;
    if (v == NULL) {
        FAIL_RETURN(out->WriteStartTag(String(NULL), String("NULL")))
        if (!name.IsNull()) {
            FAIL_RETURN(out->WriteAttribute(String(NULL), String("name"), name))
        }
        return out->WriteEndTag(String(NULL), String("NULL"));
    }
    else if (IString::Probe(v) != NULL) {
        FAIL_RETURN(out->WriteStartTag(String(NULL), String("String")))
        if (!name.IsNull()) {
            FAIL_RETURN(out->WriteAttribute(String(NULL), String("name"), name))
        }
        return out->WriteEndTag(String(NULL), String("String"));
    }
    else if (IInteger32::Probe(v) != NULL) {
        typeStr = "Int32";
        Int32 tmp = 0;
        IInteger32::Probe(v)->GetValue(&tmp);
        value = StringUtils::ToString(tmp);
    }
    else if (IInteger64::Probe(v) != NULL) {
        typeStr = "Int64";
        Int64 tmp = 0;
        IInteger64::Probe(v)->GetValue(&tmp);
        value = StringUtils::ToString(tmp);
    }
    else if (IFloat::Probe(v) != NULL) {
        typeStr = "Float";
        Float tmp;
        IFloat::Probe(v)->GetValue(&tmp);
        value = StringUtils::ToString(tmp);
    }
    else if (IDouble::Probe(v) != NULL) {
        typeStr = "Double";
        Double tmp = 0;
        IDouble::Probe(v)->GetValue(&tmp);
        value = StringUtils::ToString(tmp);
    }
    else if (IBoolean::Probe(v) != NULL) {
        typeStr = "Boolean";
        Boolean tmp = FALSE;
        IBoolean::Probe(v)->GetValue(&tmp);
        value = StringUtils::ToString(tmp);
    }
    //Combine byte[] Int32[] long[] double[] string[]
    else if (IArrayOf::Probe(v) != NULL) {
        AutoPtr<IArrayOf> av = IArrayOf::Probe(v);
        InterfaceID id;
        av->GetTypeId(&id);
        if (id == EIID_IByte) {
            AutoPtr<ArrayOf<Byte> > array = IArrayOfToArray(av, (Byte)0, (IByte*)NULL);
            return WriteByteArrayXml(array, name, out);
        }
        else if (id == EIID_IInteger32) {
            AutoPtr<ArrayOf<Int32> > array = IArrayOfToArray(av, (Int32)0, (IInteger32*)NULL);
            return WriteInt32ArrayXml(array, name, out);
        }
        else if (id == EIID_IInteger64) {
            AutoPtr<ArrayOf<Int64> > array = IArrayOfToArray(av, (Int64)0, (IInteger64*)NULL);
            return WriteInt64ArrayXml(array, name, out);
        }
        else if (id == EIID_IDouble) {
            AutoPtr<ArrayOf<Double> > array = IArrayOfToArray(av, (Double)0, (IDouble*)NULL);
            return WriteDoubleArrayXml(array, name, out);
        }
        else if (id == EIID_IString) {
            Int32 size;
            av->GetLength(&size);
            AutoPtr<ArrayOf<String> > array = size > 0 ? ArrayOf<String>::Alloc(size) : NULL;
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> item;
                av->Get(i, (IInterface**)&item);
                String str;
                ICharSequence::Probe(item)->ToString(&str);
                (*array)[i] = str;
            }
            return WriteStringArrayXml(array, name, out);
        }

        return NOERROR;
    }
    else if (IMap::Probe(v) != NULL) {
        return WriteMapXml(IMap::Probe(v), name, out);
    }
    else if (IList::Probe(v) != NULL) {
        return WriteListXml(IList::Probe(v), name, out);
    }
    else if (ISet::Probe(v) != NULL) {
        return WriteSetXml(ISet::Probe(v), name, out);
    }
    else if (ICharSequence::Probe(v) != NULL) {
        // XXX This is to allow us to at least write something if
        // we encounter styled text...  but it means we will drop all
        // of the styling information. :(
        FAIL_RETURN(out->WriteStartTag(String(NULL), String("string")))
        if (!name.IsNull()) {
            FAIL_RETURN(out->WriteAttribute(String(NULL), String("name"), name))
        }
        String value;
        FAIL_RETURN(out->WriteText((ICharSequence::Probe(v)->ToString(&value), value)))
        return out->WriteEndTag(String(NULL), String("string"));
    }
    else if (callback != NULL) {
        callback->WriteUnknownObject(v, name, out);
    }
    else {
        Slogger::E(TAG, "WriteValueXml: unable to write value ");
        return E_RUNTIME_EXCEPTION;
    }

    FAIL_RETURN(out->WriteStartTag(String(NULL), typeStr))
    if (!name.IsNull()) {
        FAIL_RETURN(out->WriteAttribute(String(NULL), String("name"), name))
    }
    out->WriteAttribute(String(NULL), String("value"), value);
    return out->WriteEndTag(String(NULL), typeStr);
}

ECode XmlUtils::ReadMapXml(
    /* [in] */ IInputStream* in,
    /* [out] */ IHashMap** map)//throws XmlPullParserException, java.io.IOException
{
    VALIDATE_NOT_NULL(map)
    AutoPtr<IXmlPullParser> parser;
    FAIL_RETURN(Xml::NewPullParser((IXmlPullParser**)&parser))
    parser->SetInput(in, String(NULL));
    AutoPtr<ArrayOf<String> > name = ArrayOf<String>::Alloc(1);
    AutoPtr<IInterface> obj;
    FAIL_RETURN(ReadValueXml(parser, name, (IInterface**)&obj))
    *map = IHashMap::Probe(obj);
    REFCOUNT_ADD(*map)
    return NOERROR;
}

ECode XmlUtils::ReadListXml(
    /* [in] */ IInputStream* in,
    /* [out] */ IArrayList** list)//throws XmlPullParserException, java.io.IOException
{
    VALIDATE_NOT_NULL(list)
    AutoPtr<IXmlPullParser> parser;
    FAIL_RETURN(Xml::NewPullParser((IXmlPullParser**)&parser))
    parser->SetInput(in, String(NULL));
    AutoPtr<ArrayOf<String> > name = ArrayOf<String>::Alloc(1);
    AutoPtr<IInterface> obj;
    FAIL_RETURN(ReadValueXml(parser, name, (IInterface**)&obj))
    *list = IArrayList::Probe(obj);
    REFCOUNT_ADD(*list)
    return NOERROR;
}

ECode XmlUtils::ReadSetXml(
    /* [in] */ IInputStream* in,
    /* [out] */ IHashSet** set)//throws XmlPullParserException, java.io.IOException
{
    VALIDATE_NOT_NULL(set)
    AutoPtr<IXmlPullParser> parser;
    FAIL_RETURN(Xml::NewPullParser((IXmlPullParser**)&parser))
    parser->SetInput(in, String(NULL));
    AutoPtr<ArrayOf<String> > name = ArrayOf<String>::Alloc(1);
    AutoPtr<IInterface> obj;
    FAIL_RETURN(ReadValueXml(parser, name, (IInterface**)&obj))
    *set = IHashSet::Probe(obj);
    REFCOUNT_ADD(*set)
    return NOERROR;
}

ECode XmlUtils::ReadThisMapXml(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ const String& endTag,
    /* [in] */ ArrayOf<String>* name,
    /* [out] */ IHashMap** map)
{
    return ReadThisMapXml(parser, endTag, name, NULL, map);
}

ECode XmlUtils::ReadThisMapXml(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ const String& endTag,
    /* [in] */ ArrayOf<String>* name,
    /* [in] */ IXmlUtilsReadMapCallback* callback,
    /* [out] */ IHashMap** map)//throws XmlPullParserException, java.io.IOException
{
    VALIDATE_NOT_NULL(map)
    CHashMap::New(map);

    Int32 eventType = 0;
    FAIL_RETURN(parser->GetEventType(&eventType))
    do {
        if (eventType == IXmlPullParser::START_TAG) {
            AutoPtr<IInterface> val;
            FAIL_RETURN(ReadThisValueXml(parser, name, callback, (IInterface**)&val))
            AutoPtr<ICharSequence> key;
            CString::New((*name)[0], (ICharSequence**)&key);
            (*map)->Put(TO_IINTERFACE(key), val);
        }
        else if (eventType == IXmlPullParser::END_TAG) {
            String n;
            parser->GetName(&n);
            if (n.Equals(endTag)) {
                return NOERROR;
            }
            Slogger::E(TAG, "Expected %s end tag at: %s", endTag.string(), n.string());
            return E_XML_PULL_PARSER_EXCEPTION;
        }
        FAIL_RETURN(parser->Next(&eventType))
    } while (eventType != IXmlPullParser::END_DOCUMENT);

    Slogger::E(TAG, "Document ended before %s end tag", endTag.string());
    return E_XML_PULL_PARSER_EXCEPTION;
}

ECode XmlUtils::ReadThisListXml(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ const String& endTag,
    /* [in] */ ArrayOf<String>* name,
    /* [out] */ IArrayList** list)//throws XmlPullParserException, java.io.IOException
{
    return ReadThisListXml(parser, endTag, name, NULL, list);
}

ECode XmlUtils::ReadThisListXml(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ const String& endTag,
    /* [in] */ ArrayOf<String>* name,
    /* [in] */ IXmlUtilsReadMapCallback* callback,
    /* [out] */ IArrayList** list)//throws XmlPullParserException, java.io.IOException
{
    VALIDATE_NOT_NULL(list)
    CArrayList::New(list);

    Int32 eventType = 0;
    FAIL_RETURN(parser->GetEventType(&eventType))
    do {
        if (eventType == IXmlPullParser::START_TAG) {
            AutoPtr<IInterface> val;
            FAIL_RETURN(ReadThisValueXml(parser, name, callback, (IInterface**)&val))
            (*list)->Add(val);
            //System.out.println("Adding to list: " + val);
        }
        else if (eventType == IXmlPullParser::END_TAG) {
            String n;
            FAIL_RETURN(parser->GetName(&n))
            if (n.Equals(endTag)) {
                return NOERROR;
            }
            Slogger::E(TAG, "Expected %s end tag at: %s", endTag.string(), n.string());
            return E_XML_PULL_PARSER_EXCEPTION;
        }
        FAIL_RETURN(parser->Next(&eventType))
    } while (eventType != IXmlPullParser::END_DOCUMENT);

    Slogger::E(TAG, "Document ended before %s end tag", endTag.string());
    return E_XML_PULL_PARSER_EXCEPTION;
}

ECode XmlUtils::ReadThisSetXml(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ const String& endTag,
    /* [in] */ ArrayOf<String>* name,
    /* [out] */ IHashSet** set)//throws XmlPullParserException, java.io.IOException
{
    return ReadThisSetXml(parser, endTag, name, NULL, set);
}

ECode XmlUtils::ReadThisSetXml(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ const String& endTag,
    /* [in] */ ArrayOf<String>* name,
    /* [in] */ IXmlUtilsReadMapCallback* callback,
    /* [out] */ IHashSet** set)//throws XmlPullParserException, java.io.IOException
{
    VALIDATE_NOT_NULL(set)
    CHashSet::New(set);

    Int32 eventType = 0;
    FAIL_RETURN(parser->GetEventType(&eventType))
    do {
        if (eventType == IXmlPullParser::START_TAG) {
            AutoPtr<IInterface> val;
            FAIL_RETURN(ReadThisValueXml(parser, name, callback, (IInterface**)&val))
            (*set)->Add(val);
            //System.out.println("Adding to set: " + val);
        }
        else if (eventType == IXmlPullParser::END_TAG) {
            String n;
            FAIL_RETURN(parser->GetName(&n))
            if (n.Equals(endTag)) {
                return NOERROR;
            }
            Slogger::E(TAG, "Expected %s end tag at: %s", endTag.string(), n.string());
            return E_XML_PULL_PARSER_EXCEPTION;
        }
        FAIL_RETURN(parser->Next(&eventType))
    } while (eventType != IXmlPullParser::END_DOCUMENT);

    Slogger::E(TAG, "Document ended before %s end tag", endTag.string());
    return E_XML_PULL_PARSER_EXCEPTION;
}

ECode XmlUtils::ReadThisInt32ArrayXml(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ const String& endTag,
    /* [in] */ ArrayOf<String>* name,
    /* [out] */ ArrayOf<Int32>** array)//throws XmlPullParserException, java.io.IOException
{
    VALIDATE_NOT_NULL(array)
    String value;
    FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("num"), &value))
    Int32 num = StringUtils::ParseInt32(value);

    Int32 next;
    FAIL_RETURN(parser->Next(&next))

    *array = ArrayOf<Int32>::Alloc(num);
    (*array)->AddRef();
    Int32 i = 0;

    Int32 eventType = 0;
    FAIL_RETURN(parser->GetEventType(&eventType))
    do {
        if (eventType == IXmlPullParser::START_TAG) {
            String n;
            FAIL_RETURN(parser->GetName(&n))
            if (n.Equals("item")) {
                FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("value"), &value))
                (*array)->Set(i, StringUtils::ParseInt32(value));
            }
            else {
                Slogger::E(TAG, "Expected item tag at: %s", n.string());
                return E_XML_PULL_PARSER_EXCEPTION;
            }
        }
        else if (eventType == IXmlPullParser::END_TAG) {
            String n;
            FAIL_RETURN(parser->GetName(&n))
            if (n.Equals(endTag)) {
                return NOERROR;
            }
            else if (n.Equals("item")) {
                i++;
            }
            else {
                Slogger::E(TAG, "Expected %s end tag at: %s", endTag.string(), n.string());
                return E_XML_PULL_PARSER_EXCEPTION;
            }
        }
        FAIL_RETURN(parser->Next(&eventType))
    } while (eventType != IXmlPullParser::END_DOCUMENT);

    Slogger::E(TAG, "Document ended before %s end tag", endTag.string());
    return E_XML_PULL_PARSER_EXCEPTION;
}

ECode XmlUtils::ReadThisInt64ArrayXml(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ const String& endTag,
    /* [in] */ ArrayOf<String>* name,
    /* [out] */ ArrayOf<Int64>** array)//throws XmlPullParserException, java.io.IOException
{
    VALIDATE_NOT_NULL(array)
    String value;
    FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("num"), &value))
    Int32 num = StringUtils::ParseInt32(value);

    Int32 next;
    FAIL_RETURN(parser->Next(&next))

    *array = ArrayOf<Int64>::Alloc(num);
    (*array)->AddRef();
    Int32 i = 0;

    Int32 eventType = 0;
    FAIL_RETURN(parser->GetEventType(&eventType))
    do {
        if (eventType == IXmlPullParser::START_TAG) {
            String n;
            FAIL_RETURN(parser->GetName(&n))
            if (n.Equals("item")) {
                FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("value"), &value))
                (*array)->Set(i, StringUtils::ParseInt64(value));
            }
            else {
                Slogger::E(TAG, "Expected item tag at: %s", n.string());
                return E_XML_PULL_PARSER_EXCEPTION;
            }
        }
        else if (eventType == IXmlPullParser::END_TAG) {
            String n;
            FAIL_RETURN(parser->GetName(&n))
            if (n.Equals(endTag)) {
                return NOERROR;
            }
            else if (n.Equals("item")) {
                i++;
            }
            else {
                Slogger::E(TAG, "Expected %s end tag at: %s", endTag.string(), n.string());
                return E_XML_PULL_PARSER_EXCEPTION;
            }
        }
        FAIL_RETURN(parser->Next(&eventType))
    } while (eventType != IXmlPullParser::END_DOCUMENT);

    Slogger::E(TAG, "Document ended before %s end tag", endTag.string());
    return E_XML_PULL_PARSER_EXCEPTION;
}

ECode XmlUtils::ReadThisDoubleArrayXml(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ const String& endTag,
    /* [in] */ ArrayOf<String>* name,
    /* [out] */ ArrayOf<Double>** array)//throws XmlPullParserException, java.io.IOException
{
    VALIDATE_NOT_NULL(array)
    String value;
    FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("num"), &value))
    Int32 num = StringUtils::ParseInt32(value);

    Int32 next;
    FAIL_RETURN(parser->Next(&next))

    *array = ArrayOf<Double>::Alloc(num);
    (*array)->AddRef();
    Int32 i = 0;

    Int32 eventType = 0;
    FAIL_RETURN(parser->GetEventType(&eventType))
    do {
        if (eventType == IXmlPullParser::START_TAG) {
            String n;
            FAIL_RETURN(parser->GetName(&n))
            if (n.Equals("item")) {
                FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("value"), &value))
                (*array)->Set(i, StringUtils::ParseDouble(value));
            }
            else {
                Slogger::E(TAG, "Expected item tag at: %s", n.string());
                return E_XML_PULL_PARSER_EXCEPTION;
            }
        }
        else if (eventType == IXmlPullParser::END_TAG) {
            String n;
            FAIL_RETURN(parser->GetName(&n))
            if (n.Equals(endTag)) {
                return NOERROR;
            }
            else if (n.Equals("item")) {
                i++;
            }
            else {
                Slogger::E(TAG, "Expected %s end tag at: %s", endTag.string(), n.string());
                return E_XML_PULL_PARSER_EXCEPTION;
            }
        }
        FAIL_RETURN(parser->Next(&eventType))
    } while (eventType != IXmlPullParser::END_DOCUMENT);

    Slogger::E(TAG, "Document ended before %s end tag", endTag.string());
    return E_XML_PULL_PARSER_EXCEPTION;
}


ECode XmlUtils::ReadThisStringArrayXml(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ const String& endTag,
    /* [in] */ ArrayOf<String>* name,
    /* [out] */ ArrayOf<String>** array)//throws XmlPullParserException, java.io.IOException
{
    VALIDATE_NOT_NULL(array)
    String value;
    FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("num"), &value))
    Int32 num = StringUtils::ParseInt32(value);

    Int32 next;
    FAIL_RETURN(parser->Next(&next))

    *array = ArrayOf<String>::Alloc(num);
    (*array)->AddRef();
    Int32 i = 0;

    Int32 eventType = 0;
    FAIL_RETURN(parser->GetEventType(&eventType))
    do {
        if (eventType == IXmlPullParser::START_TAG) {
            String n;
            FAIL_RETURN(parser->GetName(&n))
            if (n.Equals("item")) {
                FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("value"), &value))
                (*array)->Set(i, value);
            }
            else {
                Slogger::E(TAG, "Expected item tag at: %s", n.string());
                return E_XML_PULL_PARSER_EXCEPTION;
            }
        }
        else if (eventType == IXmlPullParser::END_TAG) {
            String n;
            FAIL_RETURN(parser->GetName(&n))
            if (n.Equals(endTag)) {
                return NOERROR;
            }
            else if (n.Equals("item")) {
                i++;
            }
            else {
                Slogger::E(TAG, "Expected %s end tag at: %s", endTag.string(), n.string());
                return E_XML_PULL_PARSER_EXCEPTION;
            }
        }
        FAIL_RETURN(parser->Next(&eventType))
    } while (eventType != IXmlPullParser::END_DOCUMENT);

    Slogger::E(TAG, "Document ended before %s end tag", endTag.string());
    return E_XML_PULL_PARSER_EXCEPTION;
}

ECode XmlUtils::ReadValueXml(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ ArrayOf<String>* name,
    /* [out] */ IInterface** value) //throws XmlPullParserException, java.io.IOException
{
    VALIDATE_NOT_NULL(value)
    Int32 eventType = 0;
    FAIL_RETURN(parser->GetEventType(&eventType))
    do {
        if (eventType == IXmlPullParser::START_TAG) {
            return ReadThisValueXml(parser, name, NULL, value);
        }
        else if (eventType == IXmlPullParser::END_TAG) {
            String n;
            FAIL_RETURN(parser->GetName(&n))
            Slogger::E(TAG, "Unexpected end tag at: %s", n.string());
            return E_XML_PULL_PARSER_EXCEPTION;
        }
        else if (eventType == IXmlPullParser::TEXT) {
            String n;
            FAIL_RETURN(parser->GetName(&n))
            Slogger::E(TAG, "Unexpected text: %s", n.string());
            return E_XML_PULL_PARSER_EXCEPTION;
        }
        FAIL_RETURN(parser->Next(&eventType))
    } while (eventType != IXmlPullParser::END_DOCUMENT);

    Slogger::E(TAG, "Unexpected end of document");
    return E_XML_PULL_PARSER_EXCEPTION;
}

ECode XmlUtils::ReadThisValueXml(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ ArrayOf<String>* name,
    /* [in] */ IXmlUtilsReadMapCallback* callback,
    /* [out] */ IInterface** ret)//throws XmlPullParserException, java.io.IOException
{
    VALIDATE_NOT_NULL(ret)
    String valueName;
    FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("name"), &valueName))
    String tagName;
    FAIL_RETURN(parser->GetName(&tagName))

    //System.out.println("Reading this value tag: " + tagName + ", name=" + valueName);

    AutoPtr<IInterface> res;

    if (tagName.Equals("NULL")) {
        res = NULL;
    }
    else if (tagName.Equals("String")) {
        String value("");
        Int32 eventType;
        while ((parser->Next(&eventType), eventType) != IXmlPullParser::END_DOCUMENT) {
            if (eventType == IXmlPullParser::END_TAG) {
                String n;
                FAIL_RETURN(parser->GetName(&n))
                if (n.Equals("String")) {
                    (*name)[0] = valueName;
                    //System.out.println("Returning value for " + valueName + ": " + value);
                    AutoPtr<ICharSequence> v;
                    CString::New(value, (ICharSequence**)&v);
                    *ret = v;
                    REFCOUNT_ADD(*ret);
                    return NOERROR;
                }

                Slogger::E(TAG, "Unexpected end tag in <string>: %s", n.string());
                return E_XML_PULL_PARSER_EXCEPTION;
            }
            else if (eventType == IXmlPullParser::TEXT) {
                String t;
                FAIL_RETURN(parser->GetText(&t))
                value += t;
            }
            else if (eventType == IXmlPullParser::START_TAG) {
                String n;
                FAIL_RETURN(parser->GetName(&n))
                Slogger::E(TAG, "Unexpected start tag in <string>: %s", n.string());
                return E_XML_PULL_PARSER_EXCEPTION;
            }
        }

        Slogger::E(TAG, "Unexpected end of document in <string>");
        return E_XML_PULL_PARSER_EXCEPTION;
    }
    else if ((ReadThisPrimitiveValueXml(parser, tagName, (IInterface**)&res), res) != NULL) {
        // all work already done by readThisPrimitiveValueXml
    }
    else if (tagName.Equals("Int32-array")) {
        Int32 t = 0;
        FAIL_RETURN(parser->Next(&t))
        AutoPtr<ArrayOf<Int32> > array;
        FAIL_RETURN(ReadThisInt32ArrayXml(parser, String("Int32-array"), name, (ArrayOf<Int32>**)&array))
        AutoPtr<IArrayOf> av;
        CArrayOf::New(EIID_IInteger32, array->GetLength(), (IArrayOf**)&av);
        for (Int32 i = 0; i < array->GetLength(); i++) {
            AutoPtr<IInteger32> item;
            CInteger32::New((*array)[i], (IInteger32**)&item);
            av->Set(i, TO_IINTERFACE(item));
        }
        *ret = av;
        REFCOUNT_ADD(*ret);
        //System.out.println("Returning value for " + valueName + ": " + res);
        (*name)[0] = valueName;
        return NOERROR;
    }
    else if (tagName.Equals("Int64-array")) {
        Int32 t = 0;
        FAIL_RETURN(parser->Next(&t))
        AutoPtr<ArrayOf<Int64> > array;
        FAIL_RETURN(ReadThisInt64ArrayXml(parser, String("Int64-array"), name, (ArrayOf<Int64>**)&array))
        AutoPtr<IArrayOf> av;
        CArrayOf::New(EIID_IInteger64, array->GetLength(), (IArrayOf**)&av);
        for (Int32 i = 0; i < array->GetLength(); i++) {
            AutoPtr<IInteger64> item;
            CInteger64::New((*array)[i], (IInteger64**)&item);
            av->Set(i, TO_IINTERFACE(item));
        }
        *ret = av;
        REFCOUNT_ADD(*ret);
        //System.out.println("Returning value for " + valueName + ": " + res);
        (*name)[0] = valueName;
        return NOERROR;
    }
    else if (tagName.Equals("Double-array")) {
        Int32 t = 0;
        FAIL_RETURN(parser->Next(&t))
        AutoPtr<ArrayOf<Double> > array;
        FAIL_RETURN(ReadThisDoubleArrayXml(parser, String("Double-array"), name, (ArrayOf<Double>**)&array))
        AutoPtr<IArrayOf> av;
        CArrayOf::New(EIID_IDouble, array->GetLength(), (IArrayOf**)&av);
        for (Int32 i = 0; i < array->GetLength(); i++) {
            AutoPtr<IDouble> item;
            CDouble::New((*array)[i], (IDouble**)&item);
            av->Set(i, TO_IINTERFACE(item));
        }
        *ret = av;
        REFCOUNT_ADD(*ret);
        //System.out.println("Returning value for " + valueName + ": " + res);
        (*name)[0] = valueName;
        return NOERROR;
    }
    else if (tagName.Equals("String-array")) {
        Int32 t = 0;
        FAIL_RETURN(parser->Next(&t))
        AutoPtr<ArrayOf<String> > array;
        FAIL_RETURN(ReadThisStringArrayXml(parser, String("String-array"), name, (ArrayOf<String>**)&array))
        AutoPtr<IArrayOf> av;
        CArrayOf::New(EIID_IString, array->GetLength(), (IArrayOf**)&av);
        for (Int32 i = 0; i < array->GetLength(); i++) {
            AutoPtr<IString> item;
            CString::New((*array)[i], (IString**)&item);
            av->Set(i, TO_IINTERFACE(item));
        }
        *ret = av;
        REFCOUNT_ADD(*ret);
        //System.out.println("Returning value for " + valueName + ": " + res);
        (*name)[0] = valueName;
        return NOERROR;
    }
    else if (tagName.Equals("map")) {
        Int32 t = 0;
        FAIL_RETURN(parser->Next(&t))
        AutoPtr<IHashMap> mv;
        FAIL_RETURN(ReadThisMapXml(parser, String("map"), name, (IHashMap**)&mv))
        (*name)[0] = valueName;
        *ret = mv;
        REFCOUNT_ADD(*ret);
        //System.out.println("Returning value for " + valueName + ": " + res);
        return NOERROR;
    }
    else if (tagName.Equals("list")) {
        Int32 t = 0;
        FAIL_RETURN(parser->Next(&t))
        AutoPtr<IArrayList> av;
        FAIL_RETURN(ReadThisListXml(parser, String("list"), name, (IArrayList**)&av))
        (*name)[0] = valueName;
        //System.out.println("Returning value for " + valueName + ": " + res);
        *ret = av;
        REFCOUNT_ADD(*ret);
        return NOERROR;
    }
    else if (tagName.Equals("set")) {
        Int32 t = 0;
        FAIL_RETURN(parser->Next(&t))
        AutoPtr<IHashSet> oc;
        FAIL_RETURN(ReadThisSetXml(parser, String("set"), name, (IHashSet**)&oc))
        (*name)[0] = valueName;
        //System.out.println("Returning value for " + valueName + ": " + res);
        *ret = oc;
        REFCOUNT_ADD(*ret);
        return NOERROR;
    }
    else if (callback != NULL) {
        FAIL_RETURN(callback->ReadThisUnknownObjectXml(parser, tagName, (IInterface**)&res))
        (*name)[0] = valueName;
        return NOERROR;
    }
    else {
        Slogger::E(TAG, "Unknown tag: %s", tagName.string());
        return E_XML_PULL_PARSER_EXCEPTION;
    }

    // Skip through to end tag.
    Int32 eventType;
    while ((parser->Next(&eventType), eventType) != IXmlPullParser::END_DOCUMENT) {
        String n;
        FAIL_RETURN(parser->GetName(&n))
        if (eventType == IXmlPullParser::END_TAG) {
            if (n.Equals(tagName)) {
                (*name)[0] = valueName;
                //System.out.println("Returning value for " + valueName + ": " + res);
                *ret = res;
                REFCOUNT_ADD(*ret);
                return NOERROR;
            }

            Slogger::E(TAG, "Unexpected end tag in <%s>: %s", tagName.string(), n.string());
            return E_XML_PULL_PARSER_EXCEPTION;
        }
        else if (eventType == IXmlPullParser::TEXT) {
            Slogger::E(TAG, "Unexpected text in <%s>: %s", tagName.string(), n.string());
            return E_XML_PULL_PARSER_EXCEPTION;
        }
        else if (eventType == IXmlPullParser::START_TAG) {
            Slogger::E(TAG, "Unexpected start tag in <%s>: %s", tagName.string(), n.string());
            return E_XML_PULL_PARSER_EXCEPTION;
        }
    }

    Slogger::E(TAG, "Unexpected end of document in <%s>", tagName.string());
    return E_XML_PULL_PARSER_EXCEPTION;
}

ECode XmlUtils::ReadThisPrimitiveValueXml(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ const String& tagName,
    /* [out] */ IInterface** ret) //throws XmlPullParserException, java.io.IOException
{
    // try {
    AutoPtr<IInterface> res;
    if (tagName.Equals("Int32")) {
        String value;
        FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("value"), &value))
        AutoPtr<IInteger32> iv;
        CInteger32::New(StringUtils::ParseInt32(value), (IInteger32**)&iv);
        res = iv;
    }
    else if (tagName.Equals("Int64")) {
        String value;
        FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("value"), &value))
        AutoPtr<IInteger64> iv;
        CInteger64::New(StringUtils::ParseInt64(value), (IInteger64**)&iv);
        res = iv;
    }
    else if (tagName.Equals("Float")) {
        String value;
        FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("value"), &value))
        AutoPtr<IFloat> fv;
        CFloat::New(StringUtils::ParseFloat(value), (IFloat**)&fv);
        res = fv;
    }
    else if (tagName.Equals("Double")) {
        String value;
        FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("value"), &value))
        AutoPtr<IDouble> dv;
        CDouble::New(StringUtils::ParseDouble(value), (IDouble**)&dv);
        res = dv;
    }
    else if (tagName.Equals("Boolean")) {
        String value;
        FAIL_RETURN(parser->GetAttributeValue(String(NULL), String("value"), &value))
        AutoPtr<IBoolean> bv;
        CBoolean::New(ConvertValueToBoolean(value, FALSE), (IBoolean**)&bv);
        res = bv;
    }
    *ret = res;
    REFCOUNT_ADD(*ret)
    return NOERROR;

    // } catch (NullPointerException e) {
    //     throw new XmlPullParserException("Need value attribute in <" + tagName + ">");
    // } catch (NumberFormatException e) {
    //     throw new XmlPullParserException(
    //             "Not a number in value attribute in <" + tagName + ">");
    // }
}

ECode XmlUtils::BeginDocument(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ const String& firstElementName)
{
    Int32 type;
    ECode ec = NOERROR;
    while ((ec = parser->Next(&type), type) != IXmlPullParser::START_TAG
               && type != IXmlPullParser::END_DOCUMENT) {
        FAIL_RETURN(ec);
    }

    if (type != IXmlPullParser::START_TAG) {
        // throw new XmlPullParserException("No start tag found");
        return E_XML_PULL_PARSER_EXCEPTION;
    }

    String name;
    if (parser->GetName(&name), !name.Equals(firstElementName)) {
        // throw new XmlPullParserException("Unexpected start tag: found " + parser.getName() +
        //         ", expected " + firstElementName);
        return E_XML_PULL_PARSER_EXCEPTION;
    }
    return NOERROR;
}

ECode XmlUtils::NextElement(
    /* [in] */ IXmlPullParser* parser)
{
    Int32 type;
    while ((parser->Next(&type), type) != IXmlPullParser::START_TAG
               && type != IXmlPullParser::END_DOCUMENT) {
        ;
    }
    return NOERROR;
}

Boolean XmlUtils::NextElementWithin(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ Int32 outerDepth)
{
    for (;;) {
        Int32 type, depth;
        FAIL_RETURN(parser->Next(&type))
        if (type == IXmlPullParser::END_DOCUMENT
            || (type == IXmlPullParser::END_TAG && (parser->GetDepth(&depth), depth) == outerDepth)) {
            return FALSE;
        }
        if (type == IXmlPullParser::START_TAG
                && (parser->GetDepth(&depth), depth) == outerDepth + 1) {
            return TRUE;
        }
    }
}

ECode XmlUtils::ReadInt32Attribute(
    /* [in] */ IXmlPullParser* in,
    /* [in] */ const String& name,
    /* [in] */ Int32 defaultValue,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    String value;
    FAIL_RETURN(in->GetAttributeValue(String(NULL), name, &value))
    *result = StringUtils::ParseInt32(value, 10, defaultValue);
    return NOERROR;
}

ECode XmlUtils::ReadInt32Attribute(
    /* [in] */ IXmlPullParser* in,
    /* [in] */ const String& name,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    String value;
    FAIL_RETURN(in->GetAttributeValue(String(NULL), name, &value))
    *result = StringUtils::ParseInt32(value);
    return NOERROR;
}

ECode XmlUtils::WriteInt32Attribute(
    /* [in] */ IXmlSerializer* out,
    /* [in] */ const String& name,
    /* [in] */ Int32 value)
{
    return out->WriteAttribute(String(NULL), name, StringUtils::ToString(value));
}

ECode XmlUtils::ReadInt64Attribute(
    /* [in] */ IXmlPullParser* in,
    /* [in] */ const String& name, Int64 defaultValue,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    String value;
    FAIL_RETURN(in->GetAttributeValue(String(NULL), name, &value))
    *result = StringUtils::ParseInt64(value, 10, defaultValue);
    return NOERROR;
}

ECode XmlUtils::ReadInt64Attribute(
    /* [in] */ IXmlPullParser* in,
    /* [in] */ const String& name,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    String value;
    FAIL_RETURN(in->GetAttributeValue(String(NULL), name, &value))
    *result = StringUtils::ParseInt64(value);
    return NOERROR;
}

ECode XmlUtils::WriteInt64Attribute(
    /* [in] */ IXmlSerializer* out,
    /* [in] */ const String& name,
    /* [in] */ Int64 value)
{
    return out->WriteAttribute(String(NULL), name, StringUtils::ToString(value));
}

ECode XmlUtils::ReadFloatAttribute(
    /* [in] */ IXmlPullParser* in,
    /* [in] */ const String& name,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    String value;
    FAIL_RETURN(in->GetAttributeValue(String(NULL), name, &value))
    *result = StringUtils::ParseFloat(value);
    return NOERROR;
}

ECode XmlUtils::WriteFloatAttribute(
    /* [in] */ IXmlSerializer* out,
    /* [in] */ const String& name,
    /* [in] */ Float value)
{
    return out->WriteAttribute(String(NULL), name, StringUtils::ToString(value));
}

ECode XmlUtils::ReadBooleanAttribute(
    /* [in] */ IXmlPullParser* in,
    /* [in] */ const String& name,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    String value;
    FAIL_RETURN(in->GetAttributeValue(String(NULL), name, &value))
    *result = value.EqualsIgnoreCase("TRUE");
    return NOERROR;
}

ECode XmlUtils::ReadBooleanAttribute(
    /* [in] */ IXmlPullParser* in,
    /* [in] */ const String& name,
    /* [in] */ Boolean defaultValue,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    String value;
    FAIL_RETURN(in->GetAttributeValue(String(NULL), name, &value))
    if (value == NULL) {
        *result = defaultValue;
    }
    else {
        *result = value.EqualsIgnoreCase("TRUE");
    }
    return NOERROR;
}

ECode XmlUtils::WriteBooleanAttribute(
    /* [in] */ IXmlSerializer* out,
    /* [in] */ const String& name,
    /* [in] */ Boolean value)
{
    return out->WriteAttribute(String(NULL), name, StringUtils::ToString(value));
}

ECode XmlUtils::ReadUriAttribute(
    /* [in] */ IXmlPullParser* in,
    /* [in] */ const String& name,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    String value;
    FAIL_RETURN(in->GetAttributeValue(String(NULL), name, &value))
    *result = NULL;
    return value != NULL ? Uri::Parse(value, result) : NOERROR;
}

ECode XmlUtils::WriteUriAttribute(
    /* [in] */ IXmlSerializer* out,
    /* [in] */ const String& name,
    /* [in] */ IUri* value)
{
    if (value != NULL) {
        String str;
        IObject::Probe(value)->ToString(&str);
        return out->WriteAttribute(String(NULL), name, str);
    }
    return NOERROR;
}

ECode XmlUtils::ReadStringAttribute(
    /* [in] */ IXmlPullParser* in,
    /* [in] */ const String& name,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return in->GetAttributeValue(String(NULL), name, result);
}

ECode XmlUtils::WriteStringAttribute(
    /* [in] */ IXmlSerializer* out,
    /* [in] */ const String& name,
    /* [in] */ const String& value)
{
    if (value != NULL) {
        return out->WriteAttribute(String(NULL), name, value);
    }
    return NOERROR;
}

ECode XmlUtils::ReadByteArrayAttribute(
    /* [in] */ IXmlPullParser* in,
    /* [in] */ const String& name,
    /* [out, callee] */ ArrayOf<Byte>** array)
{
    VALIDATE_NOT_NULL(array)
    *array = NULL;
    String value;
    FAIL_RETURN(in->GetAttributeValue(String(NULL), name, &value))
    if (value != NULL) {
        AutoPtr<IBase64> base64;
        CBase64::AcquireSingleton((IBase64**)&base64);
        return base64->Decode(value, IBase64::DEFAULT, array);
    }
    return NOERROR;
}

ECode XmlUtils::WriteByteArrayAttribute(
    /* [in] */ IXmlSerializer* out,
    /* [in] */ const String& name,
    /* [in] */ ArrayOf<Byte>* value)
{
    if (value != NULL) {
        AutoPtr<IBase64> base64;
        CBase64::AcquireSingleton((IBase64**)&base64);
        String str;
        base64->EncodeToString(value, IBase64::DEFAULT, &str);
        return out->WriteAttribute(String(NULL), name, str);
    }
    return NOERROR;
}

ECode XmlUtils::ReadBitmapAttribute(
    /* [in] */ IXmlPullParser* in,
    /* [in] */ const String& name,
    /* [out] */ IBitmap** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    AutoPtr<ArrayOf<Byte> > value;
    ReadByteArrayAttribute(in, name, (ArrayOf<Byte>**)&value);
    if (value != NULL) {
        AutoPtr<IBitmapFactory> factory;
        CBitmapFactory::AcquireSingleton((IBitmapFactory**)&factory);
        return factory->DecodeByteArray(value, 0, value->GetLength(), result);
    }
    return NOERROR;
}

ECode XmlUtils::WriteBitmapAttribute(
    /* [in] */ IXmlSerializer* out,
    /* [in] */ const String& name,
    /* [in] */ IBitmap* value)
{
    if (value != NULL) {
        AutoPtr<IByteArrayOutputStream> os;
        CByteArrayOutputStream::New((IByteArrayOutputStream**)&os);
        Boolean res;
        value->Compress(BitmapCompressFormat_PNG, 90, IOutputStream::Probe(os), &res);
        AutoPtr<ArrayOf<Byte> > array;
        os->ToByteArray((ArrayOf<Byte>**)&array);
        return WriteByteArrayAttribute(out, name, array);
    }
    return NOERROR;
}

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos
