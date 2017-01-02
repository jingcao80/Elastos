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

#include "Elastos.CoreLibrary.Utility.h"
#include "NativeCollation.h"
#include "ElStringByteSink.h"
#include <unicode/ucol.h>
#include <unicode/ucoleitr.h>
#include <utils/UniquePtr.h>

namespace Libcore {
namespace ICU {

HashMap<Int32, UnicodeString*> NativeCollation::sUstrMap;
Mutex NativeCollation::sUstrMapLock;

#define UCOL_MAX_BUFFER 128

static UCollator* toCollator(
    /* [in] */ Int64 address)
{
    return reinterpret_cast<UCollator*>(static_cast<uintptr_t>(address));
}

static UCollationElements* toCollationElements(
   /* [in] */ Int64 address)
{
   return reinterpret_cast<UCollationElements*>(static_cast<uintptr_t>(address));
}

extern ECode maybeThrowIcuException(const char* provider, UErrorCode errorCode);

NativeCollation::NativeCollation()
{}

void NativeCollation::CloseCollator(
    /* [in] */ Int64 collatoraddress)
{
    ucol_close(toCollator(collatoraddress));
}

void NativeCollation::CloseElements(
    /* [in] */ Int64 address)
{
    ucol_closeElements(toCollationElements(address));
    Mutex::AutoLock lock(sUstrMapLock);
    HashMap<Int32, UnicodeString*>::Iterator find = sUstrMap.Find(address);
    if (find != sUstrMap.End()) {
        delete find->mSecond;
        sUstrMap.Erase(address);
    }
}

Int32 NativeCollation::Compare(
    /* [in] */ Int64 collatoraddress,
    /* [in] */ const String& source,
    /* [in] */ const String& target)
{
    if (source.IsNull()) {
        return 0;
    }
    if (target.IsNull()) {
        return 0;
    }

    return ucol_strcoll(toCollator(collatoraddress),
            UnicodeString::fromUTF8(source.string()).getBuffer(),
            UnicodeString::fromUTF8(source.string()).length(),
            UnicodeString::fromUTF8(target.string()).getBuffer(),
            UnicodeString::fromUTF8(target.string()).length());
}

ECode NativeCollation::GetAttribute(
    /* [in] */ Int64 collatoraddress,
    /* [in] */ Int32 type,
    /* [out] */ Int32* result)
{
    UErrorCode status = U_ZERO_ERROR;
    *result = ucol_getAttribute(toCollator(collatoraddress),
            (UColAttribute)type, &status);
    return maybeThrowIcuException("ucol_getAttribute", status);
}

ECode NativeCollation::GetCollationElementIterator(
    /* [in] */ Int64 collatoraddress,
    /* [in] */ const String& source,
    /* [out] */ Int64* coleitr)
{
    if (source.IsNull()) {
        *coleitr = -1;
        return NOERROR;
    }

    UnicodeString* ustr = new UnicodeString(UnicodeString::fromUTF8(source.string()));
    UErrorCode status = U_ZERO_ERROR;
    UCollationElements* result = ucol_openElements(toCollator(collatoraddress),
        ustr->getBuffer(), ustr->length(), &status);
    ECode ec = maybeThrowIcuException("ucol_openElements", status);
    if (FAILED(ec)) {
        delete ustr;
        return ec;
    }
    *coleitr = static_cast<Int32>(reinterpret_cast<uintptr_t>(result));
    Mutex::AutoLock lock(sUstrMapLock);
    sUstrMap[*coleitr] = ustr;
    return NOERROR;
}

Int32 NativeCollation::GetMaxExpansion(
    /* [in] */ Int64 address,
    /* [in] */ Int32 order)
{
    return ucol_getMaxExpansion(toCollationElements(address), order);
}

Int32 NativeCollation::GetOffset(
    /* [in] */ Int64 address)
{
    return ucol_getOffset(toCollationElements(address));
}

String NativeCollation::GetRules(
    /* [in] */ Int64 collatoraddress)
{
    int32_t length = 0;
    UnicodeString rules = ucol_getRules(toCollator(collatoraddress), &length);
    String s("");
    ElStringByteSink sink(&s);
    rules.toUTF8(sink);
    return s;
}

AutoPtr<ArrayOf<Byte> > NativeCollation::GetSortKey(
    /* [in] */ Int64 collatoraddress,
    /* [in] */ const String& source)
{
    if (source.IsNull()) {
        return NULL;
    }
    const UCollator* collator  = toCollator(collatoraddress);
    uint8_t byteArray[UCOL_MAX_BUFFER/*128*/ * 2];
    UniquePtr<uint8_t[]> largerByteArray;
    uint8_t* usedByteArray = byteArray;
    UnicodeString ustring = UnicodeString::fromUTF8(source.string());
    const UChar* chars = ustring.getBuffer();
    size_t charCount = ustring.length();
    size_t byteArraySize = ucol_getSortKey(collator, chars, charCount, usedByteArray, sizeof(byteArray) - 1);
    if (byteArraySize > sizeof(byteArray) - 1) {
        // didn't fit, try again with a larger buffer.
        largerByteArray.reset(new uint8_t[byteArraySize + 1]);
        usedByteArray = largerByteArray.get();
        byteArraySize = ucol_getSortKey(collator, chars, charCount, usedByteArray, byteArraySize);
    }
    if (byteArraySize == 0) {
        return NULL;
    }

    AutoPtr<ArrayOf<Byte> > result = ArrayOf<Byte>::Alloc(byteArraySize);
    for (size_t i = 0; i < byteArraySize; ++i) {
        (*result)[i] = usedByteArray[i];
    }

    return result;
}

ECode NativeCollation::GetNext(
    /* [in] */ Int64 address,
    /* [out] */ Int32* result)
{
    UErrorCode status = U_ZERO_ERROR;
    *result = ucol_next(toCollationElements(address), &status);
    return maybeThrowIcuException("ucol_next", status);
}

ECode NativeCollation::OpenCollator(
    /* [in] */ ILocale* locale,
    /* [out] */ Int64 * value)
{
    String s;
    locale->ToLanguageTag(&s);
    return OpenCollator(s, value);
}

ECode NativeCollation::OpenCollator(
    /* [in] */ const String& languageTag,
    /* [out] */ Int64* address)
{
    if (languageTag.string() == NULL) {
        *address = 0;
        return NOERROR;
    }
    UErrorCode status = U_ZERO_ERROR;
    UCollator* c = ucol_open(languageTag.string(), &status);
    FAIL_RETURN(maybeThrowIcuException("ucol_open", status));
    *address = static_cast<Int64>(reinterpret_cast<uintptr_t>(c));
    return NOERROR;
}

ECode NativeCollation::OpenCollatorFromRules(
    /* [in] */ const String& rules,
    /* [in] */ Int32 normalizationmode,
    /* [in] */ Int32 collationstrength,
    /* [out] */ Int64* address)
{
    if (rules.IsNull()) {
        *address = -1;
        return NOERROR;
    }

    UErrorCode status = U_ZERO_ERROR;
    UCollator* c = ucol_openRules(UnicodeString::fromUTF8(rules.string()).getBuffer(),
            UnicodeString::fromUTF8(rules.string()).length(),
            UColAttributeValue(normalizationmode),
            UCollationStrength(collationstrength),NULL, &status);
    FAIL_RETURN(maybeThrowIcuException("ucol_openRules", status));
    *address = static_cast<Int32>(reinterpret_cast<uintptr_t>(c));
    return NOERROR;
}

ECode NativeCollation::GetPrevious(
    /* [in] */ Int64 address,
    /* [out] */ Int32* result)
{
    UErrorCode status = U_ZERO_ERROR;
    *result = ucol_previous(toCollationElements(address), &status);
    return maybeThrowIcuException("ucol_previous", status);
}

void NativeCollation::Reset(
    /* [in] */ Int64 address)
{
    ucol_reset(toCollationElements(address));
}

ECode NativeCollation::SafeClone(
    /* [in] */ Int64 collatoraddress,
    /* [out] */ Int64* address)
{
    UErrorCode status = U_ZERO_ERROR;
    Int32 bufferSize = U_COL_SAFECLONE_BUFFERSIZE;
    UCollator* c = ucol_safeClone(toCollator(collatoraddress), NULL, &bufferSize, &status);
    FAIL_RETURN(maybeThrowIcuException("ucol_safeClone", status));
    *address = static_cast<Int32>(reinterpret_cast<uintptr_t>(c));
    return NOERROR;
}

ECode NativeCollation::SetAttribute(
    /* [in] */ Int64 collatoraddress,
    /* [in] */ Int32 type,
    /* [in] */ Int32 value)
{
    UErrorCode status = U_ZERO_ERROR;
    ucol_setAttribute(toCollator(collatoraddress), (UColAttribute)type,
            (UColAttributeValue)value, &status);
    return maybeThrowIcuException("ucol_setAttribute", status);
}

ECode NativeCollation::SetOffset(
    /* [in] */ Int64 address,
    /* [in] */ Int32 offset)
{
    UErrorCode status = U_ZERO_ERROR;
    ucol_setOffset(toCollationElements(address), offset, &status);
    return maybeThrowIcuException("ucol_setOffset", status);
}

ECode NativeCollation::SetText(
    /* [in] */ Int64 address,
    /* [in] */ const String& source)
{
    UnicodeString* ustr = new UnicodeString(UnicodeString::fromUTF8(source.string()));
    UErrorCode status = U_ZERO_ERROR;
    ucol_setText(toCollationElements(address),
        ustr->getBuffer(), ustr->length(), &status);

    ECode ec = maybeThrowIcuException("ucol_setText", status);
    if (FAILED(ec)) {
        delete ustr;
        return ec;
    }
    Mutex::AutoLock lock(sUstrMapLock);
    delete sUstrMap[address];
    sUstrMap[address] = ustr;

    return NOERROR;
}

} // namespace ICU
} // namespace Libcore
