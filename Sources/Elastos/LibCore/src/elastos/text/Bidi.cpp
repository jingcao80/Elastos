#include "Bidi.h"
#include <unicode/ubidi.h>
#include "CBidi.h"

using Elastos::Core::IBoolean;
using Elastos::Core::IInteger32;
using Elastos::Core::EIID_IBoolean;
using Elastos::Core::EIID_IInteger32;
using Elastos::Text::IAttributedCharacterIteratorAttribute;
using Elastos::Text::EIID_IAttributedCharacterIteratorAttribute;


DEFINE_CONVERSION_FOR(Elastos::Text::Bidi::BidiRun, IInterface)


namespace Elastos {
namespace Text {

Bidi::BidiRun::BidiRun(
    /* [in] */ Int32 start,
    /* [in] */ Int32 limit,
    /* [in] */ Int32 level)
    : mStart(start)
    , mLimit(limit)
    , mLevel(level)
{
}

Int32 Bidi::BidiRun::GetLevel()
{
    return mLevel;
}

Int32 Bidi::BidiRun::GetLimit()
{
    return mLimit;
}

Int32 Bidi::BidiRun::GetStart()
{
    return mStart;
}

//===================================================
// Bidi
//===================================================
CAR_INTERFACE_IMPL(Bidi, Object, IBidi)

const Int32 Bidi::UBiDiDirection_UBIDI_LTR = 0;
const Int32 Bidi::UBiDiDirection_UBIDI_RTL = 1;
const Int32 Bidi::UBiDiDirection_UBIDI_MIXED = 2;

Bidi::Bidi()
    : mBaseLevel(0)
    , mLength(0)
    , mDirection(0)
    , mUnidirectional(FALSE)
{}

Bidi::~Bidi()
{
}

ECode Bidi::constructor(
    /* [in] */ IAttributedCharacterIterator* paragraph)
{
    if (paragraph == NULL) {
        //throw new IllegalArgumentException("paragraph is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 begin = 0;
    ICharacterIterator::Probe(paragraph)->GetBeginIndex(&begin);
    Int32 end = 0;
    ICharacterIterator::Probe(paragraph)->GetEndIndex(&end);
    Int32 length = end - begin;
    if (length < 0)  {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr< ArrayOf<Char32> > text = ArrayOf<Char32>::Alloc(length + 1) ; // One more char for AttributedCharacterIterator.DONE
    if (text == NULL) {
        return E_OUT_OF_MEMORY_ERROR;
    }

    if (length != 0) {
        ICharacterIterator::Probe(paragraph)->GetFirst(&(*text)[0]);
    } else {
        Char32 c(0);
        ICharacterIterator::Probe(paragraph)->GetFirst(&c);
    }

    // First check the RUN_DIRECTION attribute.
    Int32 flags = IBidi::DIRECTION_DEFAULT_LEFT_TO_RIGHT;
    AutoPtr<IInterface> direction;
    // paragraph->GetAttribute(
    //     (IAttributedCharacterIteratorAttribute*)TextAttribute::RUN_DIRECTION->Probe(EIID_IAttributedCharacterIteratorAttribute),
    //     (IInterface**)&direction);
    if (direction != NULL && direction->Probe(EIID_IBoolean) != NULL) {
        AutoPtr<IBoolean> b = (IBoolean *)direction->Probe(EIID_IBoolean);
        Boolean value;
        b->GetValue(&value);
        // if (value == TextAttribute::RUN_DIRECTION_LTR) {
        //     flags = IBidi::DIRECTION_LEFT_TO_RIGHT;
        // }
        // else {
        //     flags = IBidi::DIRECTION_RIGHT_TO_LEFT;
        // }
    }

    // Retrieve the text and gather BIDI_EMBEDDINGS
    AutoPtr< ArrayOf<Byte> > embeddings = NULL;
    Int32 textLimit = 1, i = 1;
    while (i < length) {
        AutoPtr<IInterface> embedding;
        // paragraph->GetAttribute(
        //     (IAttributedCharacterIteratorAttribute*)TextAttribute::BIDI_EMBEDDING->Probe(EIID_IAttributedCharacterIteratorAttribute),
        //     (IInterface**)&embedding);
        if (embedding != NULL && embedding->Probe(EIID_IInteger32) != NULL) {
            AutoPtr<IInteger32> integer = (IInteger32 *)embedding->Probe(EIID_IInteger32);
            Int32 embLevel;
            integer->GetValue(&embLevel);

            if (embeddings == NULL) {
                embeddings = ArrayOf<Byte>::Alloc(length);
            }

            for (; i < textLimit; i++) {
                ICharacterIterator::Probe(paragraph)->GetNext(&(*text)[i]);
                (*embeddings)[i - 1] = (Byte)embLevel;
            }
        }
        else {
            for (; i < textLimit; i++) {
                ICharacterIterator::Probe(paragraph)->GetNext(&(*text)[i]);
            }
        }
        // paragraph->GetRunLimit(
        //     (IAttributedCharacterIteratorAttribute*)TextAttribute::BIDI_EMBEDDING->Probe(EIID_IAttributedCharacterIteratorAttribute),
        //     &textLimit);
        textLimit = textLimit - begin + 1;
    }

    // Apply NumericShaper to the text
    AutoPtr<IInterface> numericShaper;
    // paragraph->GetAttribute(
    //     (IAttributedCharacterIteratorAttribute*)TextAttribute::NUMERIC_SHAPING->Probe(EIID_IAttributedCharacterIteratorAttribute),
    //     (IInterface**)&numericShaper);
    // if (numericShaper != NULL && numericShaper->Probe(EIID_INumericShaper) != NULL ) {
    //     AutoPtr<INumericShaper> ns = (INumericShaper*)numericShaper->Probe(EIID_INumericShaper);
    //     ns->Shape(text, 0, length);
    // }

    Int64 ubidi = 0ll;
    //try {
    FAIL_RETURN(CreateUBiDi(text, 0, embeddings, 0, length, flags, &ubidi));
    ReadBidiInfo(ubidi);
    //} finally {
    Ubidi_close(ubidi);
    //}
    return NOERROR;
}

ECode Bidi::constructor(
    /* [in] */ ArrayOf<Char32>* text,
    /* [in] */ Int32 textStart,
    /* [in] */ ArrayOf<Byte>* embeddings,
    /* [in] */ Int32 embStart,
    /* [in] */ Int32 paragraphLength,
    /* [in] */ Int32 flags)
{
    if (text == NULL || text->GetLength() - textStart < paragraphLength) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (embeddings != NULL) {
        if (embeddings->GetLength() - embStart < paragraphLength) {
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    if (textStart < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (embStart < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (paragraphLength < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int64 ubidi = 0ll;
    //try {
    FAIL_RETURN(CreateUBiDi(text, textStart, embeddings, embStart,
            paragraphLength, flags, &ubidi));
    ReadBidiInfo(ubidi);
    //} finally {
    Ubidi_close(ubidi);
    //}
    return NOERROR;
}

ECode Bidi::constructor(
    /* [in] */ const String& paragraph,
    /* [in] */ Int32 flags)
{
    AutoPtr< ArrayOf<Char32> > text = paragraph.GetChars();
    FAIL_RETURN(constructor(text, 0, NULL, 0, text->GetLength(), flags));

    return NOERROR;
}

ECode Bidi::constructor(
    /* [in] */ Int64 pBidi)
{
    ReadBidiInfo(pBidi);
    return NOERROR;
}

ECode Bidi::CreateUBiDi(
    /* [in] */ ArrayOf<Char32>* text,
    /* [in] */ Int32 textStart,
    /* [in] */ ArrayOf<Byte>* embeddings,
    /* [in] */ Int32 embStart,
    /* [in] */ Int32 paragraphLength,
    /* [in] */ Int32 flags,
    /* [out] */ Int64* uBiDi)
{
    VALIDATE_NOT_NULL(uBiDi);

    AutoPtr< ArrayOf<Char32> > realText;
    AutoPtr< ArrayOf<Byte> > realEmbeddings;

    if (text == NULL || text->GetLength() - textStart < paragraphLength) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    realText = ArrayOf<Char32>::Alloc(paragraphLength);
    if (realText == NULL) {
        return E_OUT_OF_MEMORY_ERROR;
    }

    for (Int32 i = 0; i < paragraphLength; ++i) {
        (*realText)[i] = (*text)[textStart + i];
    }

    if (embeddings != NULL) {
        if (embeddings->GetLength() - embStart < paragraphLength) {
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        if (paragraphLength > 0) {
            AutoPtr<CBidi> temp;
            CBidi::NewByFriend(text, textStart, NULL, 0, paragraphLength, flags,
                    (CBidi**)&temp);
            realEmbeddings = ArrayOf<Byte>::Alloc(paragraphLength);
            for (Int32 i = 0; i < paragraphLength; ++i) {
                (*realEmbeddings)[i] = (*(temp->mOffsetLevel))[i];
            }

            for (Int32 i = 0; i < paragraphLength; i++) {
                Byte e = (*embeddings)[i];
                if (e > 127) {
                    (*realEmbeddings)[i] = (Byte)UBIDI_LEVEL_OVERRIDE + ~e + 1;
                }
                else if (e > 0) {
                    (*realEmbeddings)[i] = e;
                }
                else {
                    (*realEmbeddings)[i] |= (Byte)UBIDI_LEVEL_OVERRIDE;
                }
            }
        }
    }

    if (flags > 1 || flags < -2) {
        flags = 0;
    }

    Int64 bidi = 0ll;
    bidi = Ubidi_open();
    ECode ec = Ubidi_setPara(bidi, realText, paragraphLength,
            flags, realEmbeddings);
    if (FAILED(ec)) {
        Ubidi_close(bidi);
        return ec;
    }

    *uBiDi = bidi;
    return NOERROR;
}

void Bidi::ReadBidiInfo(
    /* [in] */ Int64 pBidi)
{
    mLength = Ubidi_getLength(pBidi);

    mOffsetLevel = NULL;
    if (mLength != 0) {
        Ubidi_getLevels(pBidi, (ArrayOf<Byte>**)&mOffsetLevel);
    }

    mBaseLevel = Ubidi_getParaLevel(pBidi);

    Int32 runCount;
    Ubidi_countRuns(pBidi, &runCount);
    mRuns = NULL;
    if (runCount == 0) {
        mUnidirectional = TRUE;
    } else if (runCount < 0) {
    } else {
        Ubidi_getRuns(pBidi, (ArrayOf<BidiRun*>**)&mRuns);

        // Simplified case for one run which has the base level
        if (runCount == 1 &&  (*mRuns)[0]->GetLevel() == mBaseLevel) {
            mUnidirectional = TRUE;
            mRuns = NULL;
        }
    }

    mDirection = Ubidi_getDirection(pBidi);
}

ECode Bidi::BaseIsLeftToRight(
    /* [out] */ Boolean* isLeftToRight)
{
    VALIDATE_NOT_NULL(isLeftToRight);
    *isLeftToRight = (mBaseLevel % 2 == 0);
    return NOERROR;
}

ECode Bidi::CreateLineBidi(
    /* [in] */ Int32 lineStart,
    /* [in] */ Int32 lineLimit,
    /* [out] */ IBidi** lineBidi)
{
    VALIDATE_NOT_NULL(lineBidi)
    *lineBidi = NULL;

    if (lineStart < 0 || lineLimit < 0 || lineLimit > mLength || lineStart > lineLimit) {
        //throw new IllegalArgumentException("Invalid ranges (start=" + lineStart + ", " +
        //        "limit=" + lineLimit + ", length=" + length + ")");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr< ArrayOf<Char32> > text = ArrayOf<Char32>::Alloc(mLength);
    if (text == NULL) {
        return E_OUT_OF_MEMORY_ERROR;
    }

    for (Int32 i = 0; i < mLength; i++) {
        (*text)[i] = 'a';
    }
    AutoPtr< ArrayOf<Byte> > embeddings = ArrayOf<Byte>::Alloc(mLength);
    for (Int32 i = 0; i < mLength; i++) {
        (*embeddings)[i] = (Byte)-(*mOffsetLevel)[i];
    }

    Boolean isLefttoRight;
    BaseIsLeftToRight(&isLefttoRight);
    Int32 dir = isLefttoRight ? IBidi::DIRECTION_LEFT_TO_RIGHT : IBidi::DIRECTION_RIGHT_TO_LEFT;
    Int64 parent = 0ll;

    FAIL_RETURN(CreateUBiDi(text, 0, embeddings, 0, mLength, dir, &parent));
    ECode ec = NOERROR;
    if (lineStart == lineLimit) {
        ec = CreateEmptyLineBidi(parent, lineBidi);
    }
    else {
        Int64 line;
        Ubidi_setLine(parent, lineStart, lineLimit, &line);
        ec = CBidi::New(line, lineBidi);
    }
    Ubidi_close(parent);
    return ec;
}

ECode Bidi::CreateEmptyLineBidi(
    /* [in] */ Int64 parent,
    /* [out] */ IBidi** lineBidi)
{
    VALIDATE_NOT_NULL(lineBidi);
    *lineBidi = NULL;

    // ICU4C doesn't allow this case, but the RI does.
    AutoPtr<CBidi> result;
    FAIL_RETURN(CBidi::NewByFriend(parent, (CBidi**)&result))
    result->mLength = 0;
    result->mOffsetLevel = NULL;
    result->mRuns = NULL;
    result->mUnidirectional = TRUE;

    *lineBidi = IBidi::Probe(result);
    REFCOUNT_ADD(*lineBidi);
    return NOERROR;
}

ECode Bidi::GetBaseLevel(
    /* [out] */ Int32* baseLevel)
{
    VALIDATE_NOT_NULL(baseLevel);
    *baseLevel = mBaseLevel;
    return NOERROR;
}

ECode Bidi::GetLength(
    /* [out] */ Int32* length)
{
    VALIDATE_NOT_NULL(length);
    *length = mLength;
    return NOERROR;
}

ECode Bidi::GetLevelAt(
    /* [in] */ Int32 offset,
    /* [out] */ Int32* level)
{
    VALIDATE_NOT_NULL(level);
    if (offset >= 0 && offset < mLength) {
        *level = (*mOffsetLevel)[offset] & ~UBIDI_LEVEL_OVERRIDE;
        return NOERROR;
    }
    else {
        *level = mBaseLevel;
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
}

ECode Bidi::GetRunCount(
    /* [out] */ Int32* runCount)
{
    VALIDATE_NOT_NULL(runCount);
    *runCount = mUnidirectional ? 1 : mRuns->GetLength();
    return NOERROR;
}

ECode Bidi::GetRunLevel(
    /* [in] */ Int32 run,
    /* [out] */ Int32* runLevel)
{
    VALIDATE_NOT_NULL(runLevel);
    *runLevel = mUnidirectional ? mBaseLevel : (*mRuns)[run]->GetLevel();
    return NOERROR;
}

ECode Bidi::GetRunLimit(
    /* [in] */ Int32 run,
    /* [out] */ Int32* runLimit)
{
    VALIDATE_NOT_NULL(runLimit);
    *runLimit = mUnidirectional ? mLength : (*mRuns)[run]->GetLimit();;
    return NOERROR;

}

ECode Bidi::GetRunStart(
    /* [in] */ Int32 run,
    /* [out] */ Int32* runStart)
{
    VALIDATE_NOT_NULL(runStart);
    *runStart = mUnidirectional ? 0 : (*mRuns)[run]->GetStart();
    return NOERROR;
}

ECode Bidi::IsLeftToRight(
    /* [out] */ Boolean* isLefttoRight)
{
    VALIDATE_NOT_NULL(isLefttoRight);
    *isLefttoRight = (mDirection == UBiDiDirection_UBIDI_LTR);
    return NOERROR;
}

ECode Bidi::IsMixed(
    /* [out] */ Boolean* isMixed)
{
    VALIDATE_NOT_NULL(isMixed);
    *isMixed = (mDirection == UBiDiDirection_UBIDI_MIXED);
    return NOERROR;
}

ECode Bidi::IsRightToLeft(
    /* [out] */ Boolean* isRightToLeft)
{
    VALIDATE_NOT_NULL(isRightToLeft);
    *isRightToLeft = (mDirection == UBiDiDirection_UBIDI_RTL);
    return NOERROR;
}

ECode Bidi::ReorderVisually(
    /* [in] */ ArrayOf<Byte>* levels,
    /* [in] */ Int32 levelStart,
    /* [in] */ ArrayOf<IInterface*>* objects,
    /* [in] */ Int32 objectStart,
    /* [in] */ Int32 count)
{
    if (count < 0 || levelStart < 0 || objectStart < 0
            || count > levels->GetLength() - levelStart
            || count > objects->GetLength() - objectStart) {
        //throw new IllegalArgumentException("Invalid ranges (levels=" + levels.length +
        //        ", levelStart=" + levelStart + ", objects=" + objects.length +
        //        ", objectStart=" + objectStart + ", count=" + count + ")");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr< ArrayOf<Byte> > realLevels = ArrayOf<Byte>::Alloc(count);
    for (Int32 i = 0; i < count; i++) {
        (*realLevels)[i] = (*levels)[levelStart + i];
    }

    AutoPtr< ArrayOf<Int32> > indices = Ubidi_reorderVisual(realLevels, count);

    AutoPtr< ArrayOf<IInterface *> > result = ArrayOf<IInterface *>::Alloc(count);
    Int32 i = 0;
    for (; i < count; i++) {
        (*result)[i] = (*objects)[objectStart + (*indices)[i]];
    }

    for (i = 0; i < count; i++) {
        objects->Set(objectStart + i, (*result)[i]);
    }

    return NOERROR;
}

ECode Bidi::RequiresBidi(
    /* [in] */ ArrayOf<Char32>* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 limit,
    /* [out] */ Boolean * result)
{
    if (limit < 0 || start < 0 || start > limit || limit > text->GetLength()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IBidi> bidi;
    CBidi::New(text, start, NULL, 0, limit - start, 0, (IBidi**)&bidi);
    Boolean isLefttoRight;
    bidi->IsLeftToRight(&isLefttoRight);
    *result = !isLefttoRight;
    return NOERROR;
}


struct BiDiData {
    BiDiData(UBiDi* biDi) : mBiDi(biDi), mEmbeddingLevels(NULL) {
    }

    ~BiDiData() {
        ubidi_close(mBiDi);
    }

    UBiDiLevel* embeddingLevels() {
        return reinterpret_cast<UBiDiLevel*>(mEmbeddingLevels != NULL ? mEmbeddingLevels->GetPayload() : NULL);
    }

    void setEmbeddingLevels(ArrayOf<Byte>* newEmbeddingLevels) {
        if (newEmbeddingLevels != NULL) {
            mEmbeddingLevels = newEmbeddingLevels->Clone();
        }
        else mEmbeddingLevels = NULL;
    }

    UBiDi* uBiDi() {
        return mBiDi;
    }

private:
    UBiDi* mBiDi;
    AutoPtr<ArrayOf<Byte> > mEmbeddingLevels;

    // Disallow copy and assignment.
    BiDiData(const BiDiData&);
    void operator=(const BiDiData&);
};

static BiDiData* biDiData(Int64 ptr)
{
    return reinterpret_cast<BiDiData*>(static_cast<uintptr_t>(ptr));
}

static UBiDi* uBiDi(Int64 ptr)
{
    return reinterpret_cast<BiDiData*>(static_cast<uintptr_t>(ptr))->uBiDi();
}

static ECode icu4_error(UErrorCode errorCode)
{
    if (errorCode <= U_ZERO_ERROR || errorCode >= U_ERROR_LIMIT) {
        return NOERROR;
    }

    switch (errorCode) {
    case U_ILLEGAL_ARGUMENT_ERROR:
        return E_ILLEGAL_ARGUMENT_EXCEPTION;

    case U_INDEX_OUTOFBOUNDS_ERROR:
    case U_BUFFER_OVERFLOW_ERROR:
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;

    case U_UNSUPPORTED_ERROR:
        return E_UNSUPPORTED_OPERATION_EXCEPTION;

    default:
        return E_RUNTIME_EXCEPTION;
    }
}

Int64 Bidi::Ubidi_open()
{
    return reinterpret_cast<uintptr_t>(new BiDiData(ubidi_open()));
}

void Bidi::Ubidi_close(
    /* [in] */ Int64 pBiDi)
{
    delete biDiData(pBiDi);
}

ECode Bidi::Ubidi_setPara(
    /* [in] */ Int64 pBiDi,
    /* [in] */ ArrayOf<Char32>* text,
    /* [in] */ Int32 length,
    /* [in] */ Int32 paraLevel,
    /* [in] */ ArrayOf<Byte>* embeddingLevels)
{
    BiDiData* data = biDiData(pBiDi);
    // Copy the new embedding levels from the Java heap to the native heap.
    data->setEmbeddingLevels(embeddingLevels);

    if (text == NULL) {
        return NOERROR;
    }
    UErrorCode err = U_ZERO_ERROR;
    Int32 len = text->GetLength();
    UChar chars[len];
    for (Int32 i = 0; i < len; ++i) {
        chars[i] = (*text)[i];
    }
    ubidi_setPara(data->uBiDi(), chars, length, paraLevel,
            data->embeddingLevels(), &err);
    return icu4_error(err);
}

ECode Bidi::Ubidi_setLine(
    /* [in] */ Int64 pParaBiDi,
    /* [in] */ Int32 start,
    /* [in] */ Int32 limit,
    /* [out] */ Int64* line)
{
    VALIDATE_NOT_NULL(line);

    UErrorCode err = U_ZERO_ERROR;
    UBiDi* sized = ubidi_openSized(limit - start, 0, &err);
    ECode ec = icu4_error(err);
    if (FAILED(ec)) {
        *line = 0;
        return ec;
    }

    BiDiData* lineData = new BiDiData(sized);
    ubidi_setLine(uBiDi(pParaBiDi), start, limit, lineData->uBiDi(), &err);
    FAIL_RETURN(icu4_error(err));
    *line = reinterpret_cast<uintptr_t>(lineData);
    return NOERROR;
}

Int32 Bidi::Ubidi_getDirection(
    /* [in] */ Int64 pBiDi)
{
    return ubidi_getDirection(uBiDi(pBiDi));
}

Int32 Bidi::Ubidi_getLength(
    /* [in] */ Int64 pBiDi)
{
    return ubidi_getLength(uBiDi(pBiDi));
}

Byte Bidi::Ubidi_getParaLevel(
    /* [in] */ Int64 pBiDi)
{
    return ubidi_getParaLevel(uBiDi(pBiDi));
}

ECode Bidi::Ubidi_getLevels(
    /* [in] */ Int64 pBiDi,
    /* [out] */ ArrayOf<Byte>** levelsByte)
{
    VALIDATE_NOT_NULL(levelsByte);

    UErrorCode err = U_ZERO_ERROR;
    const UBiDiLevel* levels = ubidi_getLevels(uBiDi(pBiDi), &err);
    ECode ec = icu4_error(err);
    if (FAILED(ec)) {
        *levelsByte = NULL;
        return ec;
    }
    Int32 len = ubidi_getLength(uBiDi(pBiDi));
    AutoPtr<ArrayOf<Byte> > result = ArrayOf<Byte>::Alloc(len);
    memcpy(result->GetPayload(), reinterpret_cast<const Byte*>(levels), len);
    *levelsByte = result;
    REFCOUNT_ADD(*levelsByte)

    return NOERROR;
}

ECode Bidi::Ubidi_countRuns(
    /* [in] */ Int64 pBiDi,
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    UErrorCode err = U_ZERO_ERROR;
    Int32 c = ubidi_countRuns(uBiDi(pBiDi), &err);
    FAIL_RETURN(icu4_error(err));
    *count = c;
    return NOERROR;
}

ECode Bidi::Ubidi_getRuns(
    /* [in] */ Int64 pBidi,
    /* [out, callee] */ ArrayOf<BidiRun*>** runs)
{
    VALIDATE_NOT_NULL(runs);
    *runs = NULL;

    UBiDi* ubidi = uBiDi(pBidi);
    UErrorCode err = U_ZERO_ERROR;
    Int32 runCount = ubidi_countRuns(ubidi, &err);
    ECode ec = icu4_error(err);
    if (FAILED(ec)) {
        return ec;
    }

    // jmethodID bidiRunConstructor = env->GetMethodID(JniConstants::bidiRunClass, "<init>", "(III)V");
    // jobjectArray runs = env->NewObjectArray(runCount, JniConstants::bidiRunClass, NULL);
    AutoPtr<ArrayOf<BidiRun*> > temp = ArrayOf<BidiRun*>::Alloc(runCount);
    UBiDiLevel level = 0;
    Int32 start = 0;
    Int32 limit = 0;
    for (Int32 i = 0; i < runCount; ++i) {
        ubidi_getLogicalRun(ubidi, start, &limit, &level);
        AutoPtr<BidiRun> run = new BidiRun(start, limit, level);
        temp->Set(i, run);
        start = limit;
    }

    *runs = temp;
    REFCOUNT_ADD(*runs)
    return NOERROR;
}

AutoPtr<ArrayOf<Int32> > Bidi::Ubidi_reorderVisual(
    /* [in] */ ArrayOf<Byte>* levelBytes,
    /* [in] */ Int32 length)
{
    if (levelBytes == NULL) {
        return NULL;
    }

    const UBiDiLevel* levels = reinterpret_cast<const UBiDiLevel*>(levelBytes->GetPayload());

    Int32 indexMap[length];
    ubidi_reorderVisual(levels, length, &indexMap[0]);

    AutoPtr<ArrayOf<Int32> > result = ArrayOf<Int32>::Alloc(length);
    result->Copy(&indexMap[0], length);
    return result;
}

} // namespace Text
} // namespace Elastos
