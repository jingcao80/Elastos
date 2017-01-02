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

#include "CManifest.h"
#include "ManifestReader.h"
#include "CStreams.h"
#include "CHashMap.h"
#include "CString.h"
#include "CAttributes.h"
#include "CName.h"
#include "CharBuffer.h"
#include "ByteBuffer.h"
//#include "CCoderResultHelper.h"
//#include "CCharsets.h"

using Elastos::IO::IBuffer;
using Elastos::IO::ICharBuffer;
using Elastos::IO::CharBuffer;
using Elastos::IO::ByteBuffer;
//using Elastos::IO::Charset::CCharsets;
using Elastos::IO::Charset::ICharsets;
using Elastos::IO::Charset::ICharset;
using Elastos::IO::Charset::ICoderResult;
using Elastos::IO::Charset::ICoderResultHelper;
//using Elastos::IO::Charset::CCoderResultHelper;
using Libcore::IO::IStreams;
using Libcore::IO::CStreams;
using Elastos::Core::CString;
using Elastos::Core::EIID_ICloneable;
using Elastos::Utility::CHashMap;

namespace Elastos {
namespace Utility {
namespace Jar {

static AutoPtr<IName> InitNAME_ATTRIBUTE()
{
    AutoPtr<IName> attrib;
    CName::New(String("Name"), (IName**)&attrib);
    return attrib;
}

static AutoPtr<ArrayOf<Byte> > InitLINE_SEPARATOR()
{
   AutoPtr<ArrayOf<Byte> > arr = ArrayOf<Byte>::Alloc(2);
   arr->Set(0, '\r');
   arr->Set(1, '\n');
   return arr;
}

static AutoPtr<ArrayOf<Byte> > InitVALUE_SEPARATOR()
{
   AutoPtr<ArrayOf<Byte> > arr = ArrayOf<Byte>::Alloc(2);
   arr->Set(0, ':');
   arr->Set(1, ' ');
   return arr;
}

const AutoPtr<IName> CManifest::NAME_ATTRIBUTE = InitNAME_ATTRIBUTE();
const AutoPtr<ArrayOf<Byte> > CManifest::LINE_SEPARATOR = InitLINE_SEPARATOR();
const AutoPtr<ArrayOf<Byte> > CManifest::VALUE_SEPARATOR = InitVALUE_SEPARATOR();

CAR_INTERFACE_IMPL_2(CManifest, Object, IManifest, ICloneable)

CAR_OBJECT_IMPL(CManifest)

CManifest::CManifest()
    : mMainEnd(0)
{
    CHashMap::New((IMap**)&mEntries);
    CAttributes::New((IAttributes**)&mMainAttributes);
}

CManifest::~CManifest()
{
}

ECode CManifest::constructor()
{
    return NOERROR;
}

ECode CManifest::constructor(
    /* [in] */ IInputStream* is)
{
    AutoPtr<IStreams> streams;
    CStreams::AcquireSingleton((IStreams**)&streams);
    AutoPtr<ArrayOf<Byte> > bytes;
    streams->ReadFully(is, (ArrayOf<Byte>**)&bytes);
    return Read(bytes);
}

ECode CManifest::constructor(
    /* [in] */ IManifest* mani)
{
    // TODO: Add your code here
    AutoPtr<ICloneable> clone = ICloneable::Probe(((CManifest*)mani)->mMainAttributes);
    AutoPtr<IInterface> obj;
    FAIL_RETURN(clone->Clone((IInterface**)&obj));
    mMainAttributes = IAttributes::Probe(obj);

    AutoPtr<IMap> map;
    mani->GetEntries((IMap**)&map);
    clone = ICloneable::Probe(map);
    obj = NULL;
    clone->Clone((IInterface**)&obj);
    mEntries = IMap::Probe(obj);
    return NOERROR;
}

ECode CManifest::constructor(
        /* [in] */ ArrayOf<Byte> * manifestBytes,
        /* [in] */ Boolean readChunks)
{
    if (readChunks) {
        mChunks = new Elastos::Utility::Etl::HashMap<String, AutoPtr<Chunk> >();
    }

    return Read(manifestBytes);
}

ECode CManifest::Clear()
{
    mEntries->Clear();
    IMap::Probe(mMainAttributes)->Clear();
    return NOERROR;
}

ECode CManifest::GetAttributes(
    /* [in] */ const String& name,
    /* [out, callee] */ IAttributes** attrib)
{
    VALIDATE_NOT_NULL(attrib)
    AutoPtr<IMap> map;
    GetEntries((IMap**)&map);
    AutoPtr<ICharSequence> cs;
    CString::New(name, (ICharSequence**)&cs);
    AutoPtr<IInterface> obj;
    map->Get(cs, (IInterface**)&obj);
    *attrib = IAttributes::Probe(obj);
    REFCOUNT_ADD(*attrib)
    return NOERROR;
}

ECode CManifest::GetEntries(
    /* [out, callee] */ IMap** entries)
{
    VALIDATE_NOT_NULL(entries)
    *entries = mEntries;
    REFCOUNT_ADD(*entries)
    return NOERROR;
}

ECode CManifest::GetMainAttributes(
    /* [out] */ IAttributes** attrib)
{
    VALIDATE_NOT_NULL(attrib)
    *attrib = mMainAttributes;
    REFCOUNT_ADD(*attrib)
    return NOERROR;
}

ECode CManifest::Clone(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    AutoPtr<IManifest> manifest;
    CManifest::New(this, (IManifest**)&manifest);
    *object = TO_IINTERFACE(manifest);
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode CManifest::Write(
    /* [in] */ IOutputStream* os)
{
    return Write(this, os);
}

ECode CManifest::Read(
    /* [in] */ IInputStream* is)
{
    AutoPtr<ArrayOf<Byte> > buf;
    AutoPtr<IStreams> stream;
    FAIL_RETURN(CStreams::AcquireSingleton((IStreams**)&stream))
    FAIL_RETURN(stream->ReadFullyNoClose(is, (ArrayOf<Byte>**)&buf))

    return Read(buf);
}

ECode CManifest::Read(
        /* [in] */ ArrayOf<Byte> * buf)
{
    if (buf == NULL || buf->GetLength() == 0) {
        return NOERROR;
    }

    AutoPtr<ManifestReader> im = new ManifestReader(buf, mMainAttributes);
    mMainEnd = im->GetEndOfMainSection();
    return im->ReadEntries(mEntries, mChunks);
}

ECode CManifest::GetHashCode(
    /* [out] */ Int32* hashcode)
{
    AutoPtr<IMap> map;
    GetEntries((IMap**)&map);
    Int32 h1 = Object::GetHashCode(mMainAttributes);
    Int32 h2 = Object::GetHashCode(map);
    *hashcode = h1 ^ h2;
    return NOERROR;
}

ECode CManifest::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* isEqual)
{
    VALIDATE_NOT_NULL(isEqual)
    *isEqual = FALSE;
    if (obj == NULL) {
        return NOERROR;
    }

    IManifest* om = IManifest::Probe(obj);
    if (om == NULL) return NOERROR;

    Boolean equal;
    FAIL_RETURN(((CAttributes*)mMainAttributes.Get())->Equals(((CManifest*)om)->mMainAttributes, &equal))
    if (!equal) {
        *isEqual = equal;
        return NOERROR;
    }

    AutoPtr<IMap> m1, m2;
    GetEntries((IMap**)&m1);
    om->GetEntries((IMap**)&m2);
    *isEqual = Object::Equals(m1, m2);
    return NOERROR;
}

AutoPtr<CManifest::Chunk> CManifest::GetChunk(
    /* [in] */ const String& name)
{
    if (mChunks) {
        Elastos::Utility::Etl::HashMap<String, AutoPtr<Chunk> >::Iterator it;
        it = mChunks->Find(name);
        if (it != mChunks->End()) {
            return it->mSecond;
        }
    }
    return NULL;
}

ECode CManifest::RemoveChunks()
{
    mChunks = NULL;
    return NOERROR;
}

ECode CManifest::GetMainAttributesEnd(
    /* [out] */ Int32* mainAttrib)
{
    VALIDATE_NOT_NULL(mainAttrib)
    *mainAttrib = mMainEnd;
    return NOERROR;
}

ECode CManifest::Write(
    /* [in] */ IManifest* manifest,
    /* [in] */ IOutputStream* out)
{
    //CharsetEncoder encoder = StandardCharsets.UTF_8.newEncoder();
    AutoPtr<ICharsets> charsets;
    AutoPtr<ICharset> charset;
    // CCharsets::AcquireSingleton((ICharsets**)&charsets);
    charsets->GetUTF_8((ICharset**)&charset);
    AutoPtr<ICharsetEncoder> encoder;
    charset->NewEncoder((ICharsetEncoder**)&encoder);

    AutoPtr<IByteBuffer> buffer;
    ByteBuffer::Allocate(LINE_LENGTH_LIMIT, (IByteBuffer**)&buffer);

    CManifest* cm = (CManifest*)manifest;

    String version;
    AutoPtr<IName> versionName = CName::MANIFEST_VERSION;
    cm->mMainAttributes->GetValue(versionName, &version);

    if (version == NULL) {
        versionName = CName::SIGNATURE_VERSION;
        cm->mMainAttributes->GetValue(versionName, &version);
    }

    if (version != NULL) {
        FAIL_RETURN(WriteEntry(out, versionName, version, encoder, buffer))
        AutoPtr<ISet> keyset;
        IMap::Probe(cm->mMainAttributes)->GetKeySet((ISet**)&keyset);
        AutoPtr<IIterator> entries;
        keyset->GetIterator((IIterator**)&entries);
        Boolean hasNext = FALSE;
        String val;
        Boolean equal;
        while (entries->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> outface;
            entries->GetNext((IInterface**)&outface);
            AutoPtr<IName> name = IName::Probe(outface);
            if((name->Equals(versionName, &equal), !equal)) {
                cm->mMainAttributes->GetValue(name, &val);
                WriteEntry(out, name, val, encoder, buffer);
            }
        }
    }

    FAIL_RETURN(out->Write(LINE_SEPARATOR))

    AutoPtr<IMap> entries;
    manifest->GetEntries((IMap**)&entries);
    AutoPtr<ISet> keySet;
    entries->GetKeySet((ISet**)&keySet);
    AutoPtr<IIterator> it;
    keySet->GetIterator((IIterator**)&it);
    Boolean hasNext;
    String key;
    String val;

    while(it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> elm;
        it->GetNext((IInterface**)&elm);
        ICharSequence::Probe(elm)->ToString(&key);
        WriteEntry(out, CName::NAME, key, encoder, buffer);
        AutoPtr<IInterface> attrib;
        entries->Get(elm, (IInterface**)&attrib);
        AutoPtr<ISet> keySet;
        IMap::Probe(attrib)->GetKeySet((ISet**)&keySet);
        AutoPtr<IIterator> it;
        keySet->GetIterator((IIterator**)&it);
        while(it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> elm, value;
            it->GetNext((IInterface**)&elm);
            AutoPtr<IName> name = IName::Probe(elm);
            IMap::Probe(attrib)->Get(name.Get(), (IInterface**)&value);
            ICharSequence::Probe(value)->ToString(&val);
            FAIL_RETURN(WriteEntry(out, name, val, encoder, buffer))
        }
        FAIL_RETURN(out->Write(LINE_SEPARATOR))
    }
    return NOERROR;
}

ECode CManifest::WriteEntry(
    /* [in] */ IOutputStream* os,
    /* [in] */ IName* name,
    /* [in] */ const String& value,
    /* [in] */ ICharsetEncoder* encoder,
    /* [in] */IByteBuffer* bBuf)
{
    String nameString;
    name->GetName(&nameString);
    AutoPtr<ArrayOf<Byte> > bytes = nameString.GetBytes();
    FAIL_RETURN(os->Write(bytes))
    FAIL_RETURN(os->Write(VALUE_SEPARATOR))

    FAIL_RETURN(encoder->Reset())
    IBuffer::Probe(bBuf)->Clear();
    IBuffer::Probe(bBuf)->SetLimit(LINE_LENGTH_LIMIT - nameString.GetLength() - 2);

    AutoPtr<ArrayOf<Char32> > chars = value.GetChars();
    AutoPtr<ICharBuffer> cBuf;
    CharBuffer::Wrap(chars, (ICharBuffer**)&cBuf);

    AutoPtr<ICoderResultHelper> crHelper;
    // CCoderResultHelper::AcquireSingleton((ICoderResultHelper**)&crHelper);
    AutoPtr<ICoderResult> underflow;
    crHelper->GetUNDERFLOW((ICoderResult**)&underflow);

    while (TRUE) {
        AutoPtr<ICoderResult> r;
        encoder->Encode(cBuf, bBuf, TRUE, (ICoderResult**)&r);
        if (underflow == r) {
            r = NULL;
            encoder->Flush(bBuf, (ICoderResult**)&r);
        }
        AutoPtr<ArrayOf<Byte> > bBufArray;
        Int32 arrayOffset, position;
        bBuf->GetArray((ArrayOf<Byte>**)&bBufArray);
        IBuffer::Probe(bBuf)->GetArrayOffset(&arrayOffset);
        IBuffer::Probe(bBuf)->GetPosition(&position);
        os->Write(bBufArray, arrayOffset, position);
        os->Write(LINE_SEPARATOR);
        if (underflow == r) {
            break;
        }
        os->Write(' ');
        IBuffer::Probe(bBuf)->Clear();
        IBuffer::Probe(bBuf)->SetLimit(LINE_LENGTH_LIMIT - 1);
    }
    return NOERROR;
}

} // namespace Jar
} // namespace Utility
} // namespace Elastos

