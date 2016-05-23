#include "Charset.h"
#include "CharBuffer.h"
#include "CCodingErrorAction.h"
#include "NativeConverter.h"
#include "CString.h"
#include "AutoLock.h"
#include "CSystem.h"
#include "CTreeMap.h"
#include "CoreUtils.h"
#include "Collections.h"
#include "ServiceLoader.h"
#include "CHashMap.h"
#include "CHashSet.h"

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::CString;
using Elastos::Utility::ServiceLoader;
using Elastos::Utility::IServiceLoader;
using Elastos::Utility::IIterator;
using Elastos::Utility::ITreeMap;
using Elastos::Utility::CTreeMap;
using Elastos::Utility::Collections;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CHashSet;
using Elastos::IO::Charset::Spi::ICharsetProvider;
using Elastos::IO::Charset::Spi::EIID_ICharsetProvider;
using Libcore::ICU::NativeConverter;

namespace Elastos {
namespace IO {
namespace Charset {

AutoPtr<IHashMap> Charset::Init_CACHED_CHARSETS()
{
    AutoPtr<IHashMap> map;
    CHashMap::New((IHashMap**)&map);
    return map;
}

INIT_PROI_3 AutoPtr<IHashMap> Charset::CACHED_CHARSETS = Init_CACHED_CHARSETS();

INIT_PROI_3 AutoPtr<ICharset> Charset::DEFAULT_CHARSET = GetDefaultCharset();

Charset::Charset()
{
    CHashSet::New((IHashSet**)&mAliasesSet);
}

Charset::~Charset()
{
}

CAR_INTERFACE_IMPL(Charset, Object, ICharset)

ECode Charset::constructor(
    /* [in] */ const String& canonicalName,
    /* [in] */ ArrayOf<String>* aliases)
{
    // check whether the given canonical name is legal
    ASSERT_SUCCEEDED(CheckCharsetName(canonicalName));
    mCanonicalName = canonicalName;
    // check each alias and put into a set
    if (mAliasesSet != NULL) {
        Int32 size = aliases->GetLength();
        Boolean outflag = FALSE;
        for (Int32 i = 0; i < size; i++) {
            ASSERT_SUCCEEDED(CheckCharsetName((*aliases)[i]));
            mAliasesSet->Add(CoreUtils::Convert((*aliases)[i]), &outflag);
        }
    }

    return NOERROR;
}

ECode Charset::AvailableCharsets(
    /* [out] */ ISortedMap** outsm)
{
    // Start with a copy of the built-in charsets...
    AutoPtr<ITreeMap> charsets;
    CTreeMap::New(/* String.CASE_INSENSITIVE_ORDER */ (ITreeMap**)&charsets);
    AutoPtr< ArrayOf<String> > outstring;
    NativeConverter::GetAvailableCharsetNames((ArrayOf<String>**)&outstring);
    for (Int32 i = 0; i < outstring->GetLength(); i++) {
        AutoPtr<ICharset> charset;
        NativeConverter::CharsetForName((*outstring)[i], (ICharset**)&charset);
        String name;
        charset->GetName(&name);
        AutoPtr<IInterface> outface;
        charsets->Put(CoreUtils::Convert(name), charset, (IInterface**)&outface);
    }

    // Add all charsets provided by all charset providers...
    AutoPtr<IServiceLoader> sl = ServiceLoader::Load(EIID_ICharsetProvider, NULL);
    if (sl) {
        AutoPtr<IIterator> iter;
        ((ServiceLoader*)sl.Get())->GetIterator((IIterator**)&iter);
        Boolean isflag = FALSE;
        while(iter->HasNext(&isflag), isflag) {
            AutoPtr<IInterface> outface;
            iter->GetNext((IInterface**)&outface);
            AutoPtr<ICharsetProvider> charsetProvider = ICharsetProvider::Probe(outface);
            AutoPtr<IIterator> it;
            charsetProvider->Charsets((IIterator**)&it);
            while(it->HasNext(&isflag), isflag) {
                it->GetNext((IInterface**)&outface);
                AutoPtr<ICharset> cs = ICharset::Probe(outface);
                // A CharsetProvider can't override a built-in Charset.
                String name;
                cs->GetName(&name);
                if (charsets->ContainsKey(CoreUtils::Convert(name), &isflag), !isflag) {
                    charsets->Put(CoreUtils::Convert(name), cs, (IInterface**)&outface);
                }
            }
        }
    }

    return Collections::UnmodifiableSortedMap(ISortedMap::Probe(charsets), outsm);
}

ECode Charset::ForName(
    /* [in] */ const String& charsetName,
    /* [out] */ ICharset** charset)
{
    VALIDATE_NOT_NULL(charset);
    *charset = NULL;

    // Is this charset in our cache?
    AutoPtr<ICharset> cs;
    {    AutoLock syncLock(CACHED_CHARSETS);
        AutoPtr<IInterface> outface;
        CACHED_CHARSETS->Get(CoreUtils::Convert(charsetName), (IInterface**)&outface);
        cs = ICharset::Probe(outface);
        if (cs != NULL) {
            *charset = cs;
            REFCOUNT_ADD(*charset)
            return NOERROR;
        }
    }

    // Is this a built-in charset supported by ICU?
    if (charsetName.IsNull()) {
        return E_ILLEGAL_CHARSET_NAME_EXCEPTION;
    }

    FAIL_RETURN(CheckCharsetName(charsetName));
    NativeConverter::CharsetForName(charsetName, (ICharset**)&cs);

    if (cs.Get() != NULL) {
        AutoPtr<ICharset> result;
        FAIL_RETURN(CacheCharset(charsetName, cs.Get(), charset));
        return NOERROR;
    }

    // Does a configured CharsetProvider have this charset?
    AutoPtr<IServiceLoader> sl = ServiceLoader::Load(EIID_ICharsetProvider, NULL);
    if (sl) {
        AutoPtr<IIterator> iter;
        ((ServiceLoader*)sl.Get())->GetIterator((IIterator**)&iter);
        Boolean isflag = FALSE;
        while(iter->HasNext(&isflag), isflag) {
            AutoPtr<IInterface> outface;
            iter->GetNext((IInterface**)&outface);
            AutoPtr<ICharsetProvider> charsetProvider = ICharsetProvider::Probe(outface);
            NativeConverter::CharsetForName(charsetName, (ICharset**)&cs);
            if (cs != NULL) {
                return CacheCharset(charsetName, cs, charset);
            }

        }
    }

    return E_UNSUPPORTED_CHARSET_EXCEPTION;
}

ECode Charset::ForNameUEE(
    /* [in] */ const String& charsetName,
    /* [out] */ ICharset** charset)
{
    return Charset::ForName(charsetName, charset);
}

ECode Charset::IsSupported(
    /* [in] */ const String& charsetName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ICharset> charset;
    ECode res = Charset::ForName(charsetName, (ICharset**)&charset);
    *result = SUCCEEDED(res) ? TRUE : FALSE;
    return res;
}

ECode Charset::Name(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mCanonicalName;
    return NOERROR;
}

ECode Charset::Aliases(
    /* [out] */ ISet** aliases)
{
    return Collections::UnmodifiableSet(ISet::Probe(mAliasesSet), aliases);
}

ECode Charset::DisplayName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mCanonicalName;
    return NOERROR;
}

ECode Charset::DisplayName(
    /* [in] */ ILocale* l,
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mCanonicalName;
    return NOERROR;
}

ECode Charset::IsRegistered(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = !mCanonicalName.StartWith("x-") && !mCanonicalName.StartWith("X-");
    return NOERROR;
}

ECode Charset::CanEncode(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

ECode Charset::Encode(
    /* [in] */ ICharBuffer* buffer,
    /* [out] */ IByteBuffer** byteBuffer)
{
    VALIDATE_NOT_NULL(byteBuffer);
    *byteBuffer = NULL;
    VALIDATE_NOT_NULL(buffer);

    AutoPtr<ICharsetEncoder> encoder;
    FAIL_RETURN(NewEncoder((ICharsetEncoder**)&encoder));

    AutoPtr<ICodingErrorAction> replace;
    CCodingErrorAction::GetREPLACE((ICodingErrorAction**)&replace);

    FAIL_RETURN(encoder->OnMalformedInput(replace));
    FAIL_RETURN(encoder->OnUnmappableCharacter(replace));
    FAIL_RETURN(encoder->Encode(buffer, byteBuffer));

    return NOERROR;
}

ECode Charset::Encode(
    /* [in] */ const String& s,
    /* [out] */ IByteBuffer** byteBuffer)
{
    VALIDATE_NOT_NULL(byteBuffer);

    AutoPtr<ICharSequence> cs;
    CString::New(s, (ICharSequence**)&cs);
    AutoPtr<ICharBuffer> charBuffer;
    CharBuffer::Wrap(cs.Get(), (ICharBuffer**)&charBuffer);

    return Encode(charBuffer, byteBuffer);
}

ECode Charset::Decode(
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ ICharBuffer** charBuffer)
{
    VALIDATE_NOT_NULL(charBuffer);
    *charBuffer = NULL;
    VALIDATE_NOT_NULL(buffer);

    AutoPtr<ICharsetDecoder> decoder;
    FAIL_RETURN(NewDecoder((ICharsetDecoder**)&decoder));

    AutoPtr<ICodingErrorAction> replace;
    CCodingErrorAction::GetREPLACE((ICodingErrorAction**)&replace);

    FAIL_RETURN(decoder->OnMalformedInput(replace))
    FAIL_RETURN(decoder->OnUnmappableCharacter(replace))
    FAIL_RETURN(decoder->Decode(buffer, charBuffer))

    return NOERROR;
}

ECode Charset::CompareTo(
    /* [in] */ IInterface* charset,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -1;
    VALIDATE_NOT_NULL(charset);
    AutoPtr<ICharset> charsetInterface = ICharset::Probe(charset);
    *result = mCanonicalName.CompareIgnoreCase(((Charset *)charsetInterface.Get())->mCanonicalName);
    return NOERROR;
}

ECode Charset::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 compare;
    CompareTo(other, &compare);
    if (compare == 0) {
        *result = TRUE;
    } else {
        *result = FALSE;
    }
    return NOERROR;
}

ECode Charset::DefaultCharset(
    /* [out] */ ICharset** charset)
{
    VALIDATE_NOT_NULL(charset);
    AutoPtr<ICharset> temp = GetDefaultCharset();
    *charset = temp;
    REFCOUNT_ADD(*charset)
    return NOERROR;
}

ECode Charset::CheckCharsetName(
    /* [in] */ const String& name)
{
    if (name.IsNullOrEmpty()) {
        return E_ILLEGAL_CHARSET_NAME_EXCEPTION;
    }

    Boolean bval;
    IsValidCharsetNameStart(name.GetChar(0), &bval);
    if (!bval) {
        // throw new IllegalCharsetNameException(name);
        return E_ILLEGAL_CHARSET_NAME_EXCEPTION;
    }

    AutoPtr<ArrayOf<Char32> > charArray = name.GetChars();
    Int32 length = charArray->GetLength();
    for (Int32 i = 0; i < length; ++i) {
        IsValidCharsetNamePart((*charArray)[i], &bval);
        if (!bval) {
            return E_ILLEGAL_CHARSET_NAME_EXCEPTION;
        }
    }

    return NOERROR;
}

ECode Charset::IsValidCharsetNameStart(
    /* [in] */ const Char32& c,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9');
    return NOERROR;
}

ECode Charset::IsValidCharsetNamePart(
    /* [in] */ const Char32& c,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')
        || c == '-' || c == '.' || c == ':' || c == '_';
    return NOERROR;
}

ECode Charset::CacheCharset(
    /* [in] */ const String& charsetName,
    /* [in] */ ICharset* cs,
    /* [out] */ ICharset** charset)
{
    VALIDATE_NOT_NULL(charset);
    *charset = NULL;
    VALIDATE_NOT_NULL(cs);

    {    AutoLock syncLock(CACHED_CHARSETS);
        // Get the canonical name for this charset, and the canonical instance from the table.
        String canonicalName;
        cs->GetName(&canonicalName);
        AutoPtr<ICharset> canonicalCharset;

        AutoPtr<IInterface> outface;
        CACHED_CHARSETS->Get(CoreUtils::Convert(canonicalName), (IInterface**)&outface);
        canonicalCharset = ICharset::Probe(outface);
        if (canonicalCharset == NULL) {
            canonicalCharset = cs;
        }

        // Cache the charset by its canonical name...
        CACHED_CHARSETS->Put(CoreUtils::Convert(canonicalName), canonicalCharset, (IInterface**)&outface);

        // And the name the user used... (Section 1.4 of http://unicode.org/reports/tr22/ means
        // that many non-alias, non-canonical names are valid. For example, "utf8" isn't an
        // alias of the canonical name "UTF-8", but we shouldn't penalize consistent users of
        // such names unduly.)
        CACHED_CHARSETS->Put(CoreUtils::Convert(charsetName), canonicalCharset, (IInterface**)&outface);

        // And all its aliases...
        AutoPtr<ISet> outset;
        cs->Aliases((ISet**)&outset);
        AutoPtr< ArrayOf<IInterface*> > outarr;
        outset->ToArray((ArrayOf<IInterface*>**)&outarr);
        for (Int32 i = 0; i < outarr->GetLength(); i++) {
            CACHED_CHARSETS->Put((*outarr)[i], canonicalCharset, (IInterface**)&outface);
        }

        *charset = canonicalCharset;
        REFCOUNT_ADD(*charset)
    }
    return NOERROR;
}

AutoPtr<ICharset> Charset::GetDefaultCharset()
{
    if (DEFAULT_CHARSET.Get() == NULL) {
        AutoPtr<Elastos::Core::CSystem> cs;
        Elastos::Core::CSystem::AcquireSingletonByFriend((Elastos::Core::CSystem**)&cs);
        AutoPtr<ISystem> system = (ISystem*)cs.Get();

        String encoding;
        system->GetProperty(String("file.encoding"), &encoding);
        AutoPtr<ICharset> result;
        ECode ec = ForName(encoding, (ICharset**)&result);
        if (result == NULL || ec == (ECode)E_UNSUPPORTED_CHARSET_EXCEPTION) {
            result = NULL;
            ForName(String("UTF-8"), (ICharset**)&result);
        }
        DEFAULT_CHARSET = result;
    }

    return DEFAULT_CHARSET;
}

} // namespace Charset
} // namespace IO
} // namespace Elastos
