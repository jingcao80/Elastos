
#include "NativeConverter.h"
#include "charset/CCodingErrorAction.h"

#include <unicode/utypes.h>
#include <unicode/ucnv.h>
#include <unicode/uniset.h>
#include <unicode/ucnv_cb.h>
#include <unicode/ustring.h>
#include <ustrenum.h> // For UStringEnumeration.
#include "CharsetICU.h"
#include <elastos/utility/etl/Vector.h>
#include <utils/UniquePtr.h>


using Elastos::Utility::Etl::Vector;
using Elastos::IO::Charset::CCodingErrorAction;
using Elastos::IO::Charset::CharsetICU;

namespace Libcore {
namespace ICU {

extern String UnicodeStringToString(const UnicodeString& us);

enum {
    NativeConverter_REPORT = 0,
    NativeConverter_IGNORE = 1,
    NativeConverter_REPLACE = 2
};

static const Int32 MAX_REPLACEMENT_LENGTH = 32;

struct DecoderCallbackContext {
    UChar replacementChars[MAX_REPLACEMENT_LENGTH];
    size_t replacementCharCount;
    UConverterToUCallback onUnmappableInput;
    UConverterToUCallback onMalformedInput;
};

struct EncoderCallbackContext {
    char replacementBytes[MAX_REPLACEMENT_LENGTH];
    size_t replacementByteCount;
    UConverterFromUCallback onUnmappableInput;
    UConverterFromUCallback onMalformedInput;
};

static bool shouldCodecThrow(Boolean flush, UErrorCode error) {
    if (flush) {
        return (error != U_BUFFER_OVERFLOW_ERROR && error != U_TRUNCATED_CHAR_FOUND);
    } else {
        return (error != U_BUFFER_OVERFLOW_ERROR && error != U_INVALID_CHAR_FOUND && error != U_ILLEGAL_CHAR_FOUND);
    }
}

static UConverter* toUConverter(Int64 address)
{
    return reinterpret_cast<UConverter*>(static_cast<uintptr_t>(address));
}

extern ECode maybeThrowIcuException(UErrorCode errorCode);

static ECode collectStandardNames(
    const char* canonicalName,
    const char* standard,
    Vector<String>& result,
    bool* rev)
{
    UErrorCode status = U_ZERO_ERROR;
    UStringEnumeration e(ucnv_openStandardNames(canonicalName, standard, &status));
    if (!U_SUCCESS(status)) {
        *rev = false;
        return maybeThrowIcuException(status);
    }

    Int32 count = e.count(status);
    if (!U_SUCCESS(status)) {
        *rev = false;
        return maybeThrowIcuException(status);
    }

    for (Int32 i = 0; i < count; ++i) {
        const UnicodeString* string = e.snext(status);
        if (!U_SUCCESS(status)) {
            *rev = false;
            return maybeThrowIcuException(status);
        }
        String s = UnicodeStringToString(*string);
        if (s.IndexOf("+,") < 0) {
            result.PushBack(s);
        }
    }

    *rev = true;
    return NOERROR;
}

static const char* getICUCanonicalName(const char* name)
{
    UErrorCode error = U_ZERO_ERROR;
    const char* canonicalName = NULL;
    if ((canonicalName = ucnv_getCanonicalName(name, "MIME", &error)) != NULL) {
        return canonicalName;
    } else if ((canonicalName = ucnv_getCanonicalName(name, "IANA", &error)) != NULL) {
        return canonicalName;
    } else if ((canonicalName = ucnv_getCanonicalName(name, "", &error)) != NULL) {
        return canonicalName;
    } else if ((canonicalName = ucnv_getAlias(name, 0, &error)) != NULL) {
        // We have some aliases in the form x-blah .. match those first.
        return canonicalName;
    } else if (strstr(name, "x-") == name) {
        // Check if the converter can be opened with the name given.
        error = U_ZERO_ERROR;
        LocalUConverterPointer cnv(ucnv_open(name + 2, &error));
        if (U_SUCCESS(error)) {
          return name + 2;
        }
    }
    return NULL;
}

// If a charset listed in the IANA Charset Registry is supported by an implementation
// of the Java platform then its canonical name must be the name listed in the registry.
// Many charsets are given more than one name in the registry, in which case the registry
// identifies one of the names as MIME-preferred. If a charset has more than one registry
// name then its canonical name must be the MIME-preferred name and the other names in
// the registry must be valid aliases. If a supported charset is not listed in the IANA
// registry then its canonical name must begin with one of the strings "X-" or "x-".
static String getJavaCanonicalName(const char* icuCanonicalName)
{
    UErrorCode status = U_ZERO_ERROR;

    // Check to see if this is a well-known MIME or IANA name.
    const char* cName = NULL;
    if ((cName = ucnv_getStandardName(icuCanonicalName, "MIME", &status)) != NULL) {
        return String(cName);
    } else if ((cName = ucnv_getStandardName(icuCanonicalName, "IANA", &status)) != NULL) {
        return String(cName);
    }

    // Check to see if an alias already exists with "x-" prefix, if yes then
    // make that the canonical name.
    int32_t aliasCount = ucnv_countAliases(icuCanonicalName, &status);
    for (int i = 0; i < aliasCount; ++i) {
        const char* name = ucnv_getAlias(icuCanonicalName, i, &status);
        if (name != NULL && name[0] == 'x' && name[1] == '-') {
            return String(name);
        }
    }

    // As a last resort, prepend "x-" to any alias and make that the canonical name.
    status = U_ZERO_ERROR;
    const char* name = ucnv_getStandardName(icuCanonicalName, "UTR22", &status);
    if (name == NULL && strchr(icuCanonicalName, ',') != NULL) {
        name = ucnv_getAlias(icuCanonicalName, 1, &status);
    }
    // If there is no UTR22 canonical name then just return the original name.
    if (name == NULL) {
        name = icuCanonicalName;
    }
    UniquePtr<char[]> result(new char[2 + strlen(name) + 1]);
    strcpy(&result[0], "x-");
    strcat(&result[0], name);
    return String(&result[0]);
}


ECode NativeConverter::Decode(
    /* [in] */ Int64 address,
    /* [in] */ ArrayOf<Byte>* source,
    /* [in] */ Int32 sourceEnd,
    /* [in] */ ArrayOf<Char32>* target,
    /* [in] */ Int32 targetEnd,
    /* [in] */ ArrayOf<Int32>* data,
    /* [in] */ Boolean flush,
    /* [out] */ Int32* result)
{
    UConverter* cnv = toUConverter(address);
    if (cnv == NULL) {
        *result = U_ILLEGAL_ARGUMENT_ERROR;
        return E_INVALID_ARGUMENT;
    }
    if (source->GetPayload() == NULL) {
        *result = U_ILLEGAL_ARGUMENT_ERROR;
        return E_INVALID_ARGUMENT;
    }
    if (target->GetPayload() == NULL) {
        *result = U_ILLEGAL_ARGUMENT_ERROR;
        return E_INVALID_ARGUMENT;
    }
    if (data->GetPayload() == NULL) {
        *result = U_ILLEGAL_ARGUMENT_ERROR;
        return E_INVALID_ARGUMENT;
    }

    // Do the conversion.
    Int32* sourceOffset = &(*data)[0];
    Int32* targetOffset = &(*data)[1];
    const char* mySource = reinterpret_cast<const char*>(source->GetPayload() + *sourceOffset);
    const char* mySourceLimit = reinterpret_cast<const char*>(source->GetPayload() + sourceEnd);
    AutoPtr<ArrayOf<UChar> > temp = ArrayOf<UChar>::Alloc(target->GetLength());
    UChar* cTarget = temp->GetPayload() + *targetOffset;
    const UChar* cTargetLimit = temp->GetPayload() + targetEnd;
    UErrorCode errorCode = U_ZERO_ERROR;
    ucnv_toUnicode(cnv, &cTarget, cTargetLimit, &mySource, mySourceLimit,
            NULL, flush, &errorCode);
    for (Int32 i = *targetOffset; i < temp->GetLength(); ++i) {
        (*target)[i] = (*temp)[i];
    }
    *sourceOffset = mySource - reinterpret_cast<const char*>(source->GetPayload()) - *sourceOffset;
    *targetOffset = cTarget - temp->GetPayload() - *targetOffset;

    // If there was an error, count the problematic bytes.
    if (errorCode == U_ILLEGAL_CHAR_FOUND || errorCode == U_INVALID_CHAR_FOUND) {
        int8_t invalidByteCount = 32;
        char invalidBytes[32] = {'\0'};
        UErrorCode minorErrorCode = U_ZERO_ERROR;
        ucnv_getInvalidChars(cnv, invalidBytes, &invalidByteCount, &minorErrorCode);
        if (U_SUCCESS(minorErrorCode)) {
            (*data)[2] = invalidByteCount;
        }
    }

    // Managed code handles some cases; throw all other errors.
    if (shouldCodecThrow(flush, errorCode)) {
        return maybeThrowIcuException(errorCode);
    }
    *result = errorCode;
    return NOERROR;
}

ECode NativeConverter::Encode(
    /* [in] */ Int64 address,
    /* [in] */ ArrayOf<Char32>* source,
    /* [in] */ Int32 sourceEnd,
    /* [in] */ ArrayOf<Byte>* target,
    /* [in] */ Int32 targetEnd,
    /* [in] */ ArrayOf<Int32>* data,
    /* [in] */ Boolean flush,
    /* [out] */ Int32* result)
{
    UConverter* cnv = toUConverter(address);
    if (cnv == NULL) {
        *result = U_ILLEGAL_ARGUMENT_ERROR;
        return E_INVALID_ARGUMENT;
    }
    if (source->GetPayload() == NULL) {
        *result = U_ILLEGAL_ARGUMENT_ERROR;
        return E_INVALID_ARGUMENT;
    }
    if (target->GetPayload() == NULL) {
        *result = U_ILLEGAL_ARGUMENT_ERROR;
        return E_INVALID_ARGUMENT;
    }
    if (data->GetPayload() == NULL) {
        *result = U_ILLEGAL_ARGUMENT_ERROR;
        return E_INVALID_ARGUMENT;
    }

    // Do the conversion.
    Int32* sourceOffset = &((*data)[0]);
    Int32* targetOffset = &((*data)[1]);
    const UChar* mySource = (UChar*) source->GetPayload() + *sourceOffset;
    const UChar* mySourceLimit = (UChar*) source->GetPayload() + sourceEnd;
    char* cTarget = reinterpret_cast<char*>(target->GetPayload() + *targetOffset);
    const char* cTargetLimit = reinterpret_cast<const char*>(target->GetPayload() + targetEnd);
    UErrorCode errorCode = U_ZERO_ERROR;
    ucnv_fromUnicode(cnv , &cTarget, cTargetLimit, &mySource, mySourceLimit, NULL, (UBool) flush, &errorCode);
    *sourceOffset = mySource - (UChar*) (source->GetPayload()) - *sourceOffset;
    *targetOffset = (reinterpret_cast<Byte*>(cTarget) - target->GetPayload());

    // If there was an error, count the problematic characters.
    if (U_ILLEGAL_CHAR_FOUND == errorCode
        || U_INVALID_CHAR_FOUND == errorCode
        || U_TRUNCATED_CHAR_FOUND == errorCode) {
        int8_t invalidUCharCount = 32;
        UChar invalidUChars[32];
        UErrorCode minorErrorCode = U_ZERO_ERROR;
        ucnv_getInvalidUChars(cnv, invalidUChars, &invalidUCharCount, &minorErrorCode);
        if (U_SUCCESS(minorErrorCode)) {
            (*data)[2] = invalidUCharCount;
        }
    }

    // Managed code handles some cases; throw all other errors.
    if (shouldCodecThrow(flush, errorCode)) {
        return maybeThrowIcuException(errorCode);
    }

    *result = errorCode;
    return NOERROR;
}

ECode NativeConverter::OpenConverter(
    /* [in] */ const String& converterName,
    /* [out] */ Int64* value)
{
    if (converterName.IsNull()) {
        *value = 0;
        return NOERROR;
    }
    UErrorCode status = U_ZERO_ERROR;
    UConverter* cnv = ucnv_open(converterName.string(), &status);
    if (!U_SUCCESS(status)) {
        return maybeThrowIcuException(status);
    }
    *value = reinterpret_cast<uintptr_t>(cnv);
    return NOERROR;
}

ECode NativeConverter::CloseConverter(
    /* [in] */ Int64 address)
{
    ucnv_close(toUConverter(address));
    return NOERROR;
}

ECode NativeConverter::ResetByteToChar(
    /* [in] */ Int64 address)
{
    UConverter* cnv = toUConverter(address);
    if (cnv) {
        ucnv_resetToUnicode(cnv);
    }
    return NOERROR;
}

ECode NativeConverter::ResetCharToByte(
    /* [in] */ Int64 address)
{
    UConverter* cnv = toUConverter(address);
    if (cnv) {
        ucnv_resetFromUnicode(cnv);
    }
    return NOERROR;
}

ECode NativeConverter::GetSubstitutionBytes(
    /* [in] */ Int64 address,
    /* [out, callee] */ ArrayOf<Byte> ** outarray)
{
    UConverter* cnv = toUConverter(address);
    if (cnv == NULL) {
        *outarray = NULL;
        return NOERROR;
    }
    UErrorCode status = U_ZERO_ERROR;
    char replacementBytes[MAX_REPLACEMENT_LENGTH];
    int8_t len = sizeof(replacementBytes);
    ucnv_getSubstChars(cnv, replacementBytes, &len, &status);
    if (!U_SUCCESS(status)) {
        *outarray = ArrayOf<Byte>::Alloc(0);
        REFCOUNT_ADD(*outarray);
        return NOERROR;
    }
    AutoPtr<ArrayOf<Byte> > result = ArrayOf<Byte>::Alloc(len);
    if (NULL == result) {
        *outarray = NULL;
        return NOERROR;
    }
    memcpy(result->GetPayload(), replacementBytes, len);
    *outarray = result;
    REFCOUNT_ADD(*outarray);
    return NOERROR;
}

ECode NativeConverter::GetMaxBytesPerChar(
    /* [in] */ Int64 address,
    /* [out] */ Int32* value)
{
    UConverter* cnv = toUConverter(address);
    *value = (cnv != NULL) ? ucnv_getMaxCharSize(cnv) : -1;
    return NOERROR;
}

ECode NativeConverter::GetMinBytesPerChar(
    /* [in] */ Int64 address,
    /* [out] */ Int32* value)
{
    UConverter* cnv = toUConverter(address);
    *value = (cnv != NULL) ? ucnv_getMinCharSize(cnv) : -1;
    return NOERROR;
}

ECode NativeConverter::GetAveBytesPerChar(
    /* [in] */ Int64 address,
    /* [out] */ Float* value)
{
    UConverter* cnv = toUConverter(address);
    *value = (cnv != NULL) ? ((ucnv_getMaxCharSize(cnv) + ucnv_getMinCharSize(cnv)) / 2.0) : -1;
    return NOERROR;
}

ECode NativeConverter::GetAveCharsPerByte(
    /* [in] */ Int64 handle,
    /* [out] */ Float* value)
{
    Int32 m;
    GetMaxBytesPerChar(handle, &m);
    *value = (1 / (Float) m);
    return NOERROR;
}

ECode NativeConverter::Contains(
    /* [in] */ const String& name1,
    /* [in] */ const String& name2,
    /* [out] */ Boolean* value)
{
    if (name1.IsNull()) {
        *value = FALSE;
        return NOERROR;
    }
    if (name2.IsNull()) {
        *value = FALSE;
        return NOERROR;
    }

    UErrorCode errorCode = U_ZERO_ERROR;
    LocalUConverterPointer converter1(ucnv_open(name1.string(), &errorCode));
    UnicodeSet set1;
    ucnv_getUnicodeSet(&*converter1, set1.toUSet(), UCNV_ROUNDTRIP_SET, &errorCode);

    LocalUConverterPointer converter2(ucnv_open(name2.string(), &errorCode));
    UnicodeSet set2;
    ucnv_getUnicodeSet(&*converter2, set2.toUSet(), UCNV_ROUNDTRIP_SET, &errorCode);

    *value = U_SUCCESS(errorCode) && set1.containsAll(set2);
    return NOERROR;
}

ECode NativeConverter::GetAvailableCharsetNames(
    /* [out, callee] */ ArrayOf<String> ** outarray)
{
    Int32 num = ucnv_countAvailable();
    AutoPtr<ArrayOf<String> > result = ArrayOf<String>::Alloc(num);
    if (NULL == result) {
        *outarray = NULL;
        return NOERROR;
    }
    for (Int32 i = 0; i < num; ++i) {
        const char* name = ucnv_getAvailableName(i);
        String javaCannoicalName = getJavaCanonicalName(name);
        if (javaCannoicalName.IsNull()) {
            *outarray = NULL;
            return NOERROR;
        }
        (*result)[i] = javaCannoicalName;
    }
    *outarray = result;
    return NOERROR;
}

static void CHARSET_ENCODER_CALLBACK(const void* rawContext, UConverterFromUnicodeArgs* args,
        const UChar* codeUnits, int32_t length, UChar32 codePoint, UConverterCallbackReason reason,
        UErrorCode* status)
{
    if (!rawContext) {
        return;
    }
    const EncoderCallbackContext* ctx = reinterpret_cast<const EncoderCallbackContext*>(rawContext);
    switch(reason) {
    case UCNV_UNASSIGNED:
        ctx->onUnmappableInput(ctx, args, codeUnits, length, codePoint, reason, status);
        return;
    case UCNV_ILLEGAL:
    case UCNV_IRREGULAR:
        ctx->onMalformedInput(ctx, args, codeUnits, length, codePoint, reason, status);
        return;
    case UCNV_CLOSE:
        delete ctx;
        return;
    default:
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        return;
    }
}

static void encoderReplaceCallback(const void* rawContext,
        UConverterFromUnicodeArgs* fromArgs, const UChar*, int32_t, UChar32,
        UConverterCallbackReason, UErrorCode* err) {
    if (rawContext == NULL) {
        return;
    }
    const EncoderCallbackContext* context = reinterpret_cast<const EncoderCallbackContext*>(rawContext);
    *err = U_ZERO_ERROR;
    ucnv_cbFromUWriteBytes(fromArgs, context->replacementBytes, context->replacementByteCount, 0, err);
}

static UConverterFromUCallback getFromUCallback(int32_t mode) {
    switch(mode) {
    case NativeConverter_IGNORE: return UCNV_FROM_U_CALLBACK_SKIP;
    case NativeConverter_REPLACE: return encoderReplaceCallback;
    case NativeConverter_REPORT: return UCNV_FROM_U_CALLBACK_STOP;
    }
    abort();
}

ECode NativeConverter::CharsetForName(
    /* [in] */ const String& charsetName,
    /* [out] */ICharset** outset)
{
    VALIDATE_NOT_NULL(outset)
    *outset = NULL;

    if (charsetName.IsNull()) {
        return NOERROR;
    }

    // Get ICU's canonical name for this charset.
    const char* icuCanonicalName = getICUCanonicalName(charsetName.string());
    if (icuCanonicalName == NULL) {
        return NOERROR;
    }

    // Get Java's canonical name for this charset.
    String javaCannoicalName = getJavaCanonicalName(icuCanonicalName);

    // Check that this charset is supported.
    {
        // ICU doesn't offer any "isSupported", so we just open and immediately close.
        UErrorCode error = U_ZERO_ERROR;
        LocalUConverterPointer cnv(ucnv_open(icuCanonicalName, &error));
        if (!U_SUCCESS(error)) {
            return NOERROR;
        }
    }

    // Get the aliases for this charset.
    Vector<String> aliases;
    bool rev;
    collectStandardNames(icuCanonicalName, "IANA", aliases, &rev);
    if (!rev) {
        return NOERROR;
    }
    collectStandardNames(icuCanonicalName, "MIME", aliases, &rev);
    if (!rev) {
        return NOERROR;
    }
    collectStandardNames(icuCanonicalName, "JAVA", aliases, &rev);
    if (!rev) {
        return NOERROR;
    }
    collectStandardNames(icuCanonicalName, "WINDOWS", aliases, &rev);
    if (!rev) {
        return NOERROR;
    }
    size_t size = aliases.GetSize();
    AutoPtr<ArrayOf<String> > aliases_array = ArrayOf<String>::Alloc(size);
    for (size_t i = 0; i < size; ++i) {
        (*aliases_array)[i] = aliases[i];
    }
    AutoPtr<CharsetICU> icu = new CharsetICU();
    icu->constructor(javaCannoicalName, String(icuCanonicalName), aliases_array);
    *outset = icu.Get();
    REFCOUNT_ADD(*outset);
    return NOERROR;
}

ECode NativeConverter::SetCallbackDecode(
    /* [in] */ Int64 converterHandle,
    /* [in] */ ICharsetDecoder* decoder)
{
    AutoPtr<ICodingErrorAction> cm;
    AutoPtr<ICodingErrorAction> cu;
    decoder->MalformedInputAction((ICodingErrorAction**)&cm);
    decoder->UnmappableCharacterAction((ICodingErrorAction**)&cu);

    Int32 im, iu;
    TranslateCodingErrorAction(cm, &im);
    TranslateCodingErrorAction(cu, &iu);
    String s;
    decoder->Replacement(&s);
    return SetCallbackDecode(converterHandle, im, iu, s);
}

ECode NativeConverter::SetCallbackEncode(
    /* [in] */ Int64 converterHandle,
    /* [in] */ ICharsetEncoder* encoder)
{
    AutoPtr<ICodingErrorAction> cm;
    AutoPtr<ICodingErrorAction> cu;
    encoder->GetMalformedInputAction((ICodingErrorAction**)&cm);
    encoder->GetUnmappableCharacterAction((ICodingErrorAction**)&cu);

    Int32 im, iu;
    TranslateCodingErrorAction(cm, &im);
    TranslateCodingErrorAction(cu, &iu);
    AutoPtr<ArrayOf<Byte> > bytes;
    encoder->GetReplacement((ArrayOf<Byte>**)&bytes);
    return SetCallbackEncode(converterHandle, im, iu, bytes);
}

ECode NativeConverter::TranslateCodingErrorAction(
    /* [in] */ ICodingErrorAction* action,
    /* [out] */ Int32* rev)
{
    VALIDATE_NOT_NULL(rev)
    *rev = -1;

    AutoPtr<ICodingErrorAction> report;
    CCodingErrorAction::GetREPORT((ICodingErrorAction**)&report);
    AutoPtr<ICodingErrorAction> ignore;
    CCodingErrorAction::GetIGNORE((ICodingErrorAction**)&ignore);
    AutoPtr<ICodingErrorAction> replace;
    CCodingErrorAction::GetREPLACE((ICodingErrorAction**)&replace);
    if (action == report) {
        *rev = 0;
        return NOERROR;
    } else if (action == ignore) {
        *rev = 1;
        return NOERROR;
    } else if (action == replace) {
        *rev = 2;
        return NOERROR;
    }
    return E_ASSERTION_ERROR;
}

static void decoderIgnoreCallback(const void*, UConverterToUnicodeArgs*, const char*, int32_t, UConverterCallbackReason, UErrorCode* err) {
    // The icu4c UCNV_FROM_U_CALLBACK_SKIP callback requires that the context is NULL, which is
    // never true for us.
    *err = U_ZERO_ERROR;
}

static void decoderReplaceCallback(const void* rawContext,
        UConverterToUnicodeArgs* toArgs, const char*, int32_t, UConverterCallbackReason,
        UErrorCode* err) {
    if (!rawContext) {
        return;
    }
    const DecoderCallbackContext* context = reinterpret_cast<const DecoderCallbackContext*>(rawContext);
    *err = U_ZERO_ERROR;
    ucnv_cbToUWriteUChars(toArgs,context->replacementChars, context->replacementCharCount, 0, err);
}

static UConverterToUCallback getToUCallback(int32_t mode) {
    switch (mode) {
    case NativeConverter_IGNORE: return decoderIgnoreCallback;
    case NativeConverter_REPLACE: return decoderReplaceCallback;
    case NativeConverter_REPORT: return UCNV_TO_U_CALLBACK_STOP;
    }
    abort();
}

static void CHARSET_DECODER_CALLBACK(const void* rawContext, UConverterToUnicodeArgs* args,
        const char* codeUnits, int32_t length,
        UConverterCallbackReason reason, UErrorCode* status) {
    if (!rawContext) {
        return;
    }
    const DecoderCallbackContext* ctx = reinterpret_cast<const DecoderCallbackContext*>(rawContext);
    switch(reason) {
    case UCNV_UNASSIGNED:
        ctx->onUnmappableInput(ctx, args, codeUnits, length, reason, status);
        return;
    case UCNV_ILLEGAL:
    case UCNV_IRREGULAR:
        ctx->onMalformedInput(ctx, args, codeUnits, length, reason, status);
        return;
    case UCNV_CLOSE:
        delete ctx;
        return;
    default:
        *status = U_ILLEGAL_ARGUMENT_ERROR;
        return;
    }
}

ECode NativeConverter::SetCallbackDecode(
    /* [in] */ Int64 address,
    /* [in] */ Int32 onMalformedInput,
    /* [in] */ Int32 onUnmappableInput,
    /* [in] */ const String& javaReplacement)
{
    UConverter* cnv = toUConverter(address);
    if (cnv == NULL) {
        return E_INVALID_ARGUMENT;
    }

    UConverterToUCallback oldCallback;
    const void* oldCallbackContext;
    ucnv_getToUCallBack(cnv, &oldCallback, &oldCallbackContext);

    DecoderCallbackContext* callbackContext = const_cast<DecoderCallbackContext*>(
            reinterpret_cast<const DecoderCallbackContext*>(oldCallbackContext));
    if (callbackContext == NULL) {
        callbackContext = new DecoderCallbackContext;
    }

    callbackContext->onMalformedInput = getToUCallback(onMalformedInput);
    callbackContext->onUnmappableInput = getToUCallback(onUnmappableInput);

    if (javaReplacement.IsNull()) {
        return E_INVALID_ARGUMENT;
    }
    u_strncpy(callbackContext->replacementChars, (UChar*) (javaReplacement.GetChars()->GetPayload()), javaReplacement.GetLength());
    callbackContext->replacementCharCount = javaReplacement.GetLength();

    UErrorCode errorCode = U_ZERO_ERROR;
    ucnv_setToUCallBack(cnv, CHARSET_DECODER_CALLBACK, callbackContext, NULL, NULL, &errorCode);
    return maybeThrowIcuException(errorCode);
}

ECode NativeConverter::SetCallbackEncode(
    /* [in] */ Int64 address,
    /* [in] */ Int32 onMalformedInput,
    /* [in] */ Int32 onUnmappableInput,
    /* [in] */ ArrayOf<Byte>* javaReplacement)
{
    UConverter* cnv = toUConverter(address);
    if (cnv == NULL) {
        return E_INVALID_ARGUMENT;
    }

    UConverterFromUCallback oldCallback = NULL;
    const void* oldCallbackContext = NULL;
    ucnv_getFromUCallBack(cnv, &oldCallback, const_cast<const void**>(&oldCallbackContext));

    EncoderCallbackContext* callbackContext = const_cast<EncoderCallbackContext*>(
            reinterpret_cast<const EncoderCallbackContext*>(oldCallbackContext));
    if (callbackContext == NULL) {
        callbackContext = new EncoderCallbackContext;
    }

    callbackContext->onMalformedInput = getFromUCallback(onMalformedInput);
    callbackContext->onUnmappableInput = getFromUCallback(onUnmappableInput);

    if (javaReplacement->GetPayload() == NULL) {
        return E_INVALID_ARGUMENT;
    }
    memcpy(callbackContext->replacementBytes, javaReplacement->GetPayload(), javaReplacement->GetLength());
    callbackContext->replacementByteCount = javaReplacement->GetLength();

    UErrorCode errorCode = U_ZERO_ERROR;
    ucnv_setFromUCallBack(cnv, CHARSET_ENCODER_CALLBACK, callbackContext, NULL, NULL, &errorCode);
    return maybeThrowIcuException(errorCode);
}

}    // ICU
}    // Libcore
