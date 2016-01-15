
#include "Transliterator.h"
#include <unicode/uniset.h>
#include <ElStringByteSink.h>
#include <unicode/strenum.h>
#include <unicode/translit.h>

namespace Libcore {
namespace ICU {

CAR_INTERFACE_IMPL(Transliterator, Object, ITransliterator)

// return a String object create from an UnicodeString object
String UnicodeStringToString(const UnicodeString& us)
{
    String s;
    ElStringByteSink sink(&s);
    us.toUTF8(sink);
    return s;
}

ECode Transliterator::constructor(
    /* [in] */ const String& id)
{
    mPeer = Create(id);
    return NOERROR;
}


Transliterator::~Transliterator()
{
    Destroy(mPeer);
    mPeer = 0;
}

static NATIVE(Transliterator)* fromPeer(Int64 peer) {
    return reinterpret_cast<NATIVE(Transliterator)*>(static_cast<uintptr_t>(peer));
}

ECode Transliterator::GetAvailableIDs(
    /* [out, callee] */ ArrayOf<String>** ids)
{
    UErrorCode status = U_ZERO_ERROR;
    StringEnumeration* e = NATIVE(Transliterator)::getAvailableIDs(status);

    if (!U_SUCCESS(status)) {
        *ids = NULL;
        return NOERROR;
    }

    Int32 count = e->count(status);
    if (!U_SUCCESS(status)) {
        *ids = NULL;
        return NOERROR;
    }

    AutoPtr<ArrayOf<String> > result = ArrayOf<String>::Alloc(count);
    for (Int32 i = 0; i < count; ++i) {
        const UnicodeString* string = e->snext(status);
        if (!U_SUCCESS(status)) {
            *ids = NULL;
            return NOERROR;
        }
        (*result)[i] = UnicodeStringToString(*string);
    }
    *ids = result;
    REFCOUNT_ADD(*ids);
    return NOERROR;
}

ECode Transliterator::Transliterate(
    /* [in] */ const String& s,
    /* [out] */ String* rev)
{
    VALIDATE_NOT_NULL(rev)
    *rev = Transliterate(mPeer, s);
    return NOERROR;
}

Int64 Transliterator::Create(
    /* [in] */ const String& id)
{
    if (id.IsNull()) {
        return 0;
    }
    UErrorCode status = U_ZERO_ERROR;
    NATIVE(Transliterator)* t = NATIVE(Transliterator)::createInstance(UnicodeString::fromUTF8(id.string()), UTRANS_FORWARD, status);
    if (!U_SUCCESS(status)) {
        return 0;
    }
    return reinterpret_cast<uintptr_t>(t);
}

void Transliterator::Destroy(
    /* [in] */ Int64 peer)
{
    delete fromPeer(peer);
}

String Transliterator::Transliterate(
    /* [in] */ Int64 peer,
    /* [in] */ const String& s)
{
    NATIVE(Transliterator)* t = fromPeer(peer);
    if(s.IsNull()) {
        return String(NULL);
    }

    UnicodeString us = UnicodeString::fromUTF8(s.string());
    t->transliterate(us);
    return UnicodeStringToString(us);
}

} // ICU
} // Libcore
