#include "Elastos.Droid.Media.h"
#include "elastos/droid/media/Tokenizer.h"
#include "elastos/droid/media/WebVttParserHelper.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/Character.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::StringUtils;
using Elastos::Core::Character;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Media {
//===============================================================
//                  Tokenizer::DataTokenizer
//===============================================================
Tokenizer::DataTokenizer::DataTokenizer()
{}

Tokenizer::DataTokenizer::~DataTokenizer()
{}

CAR_INTERFACE_IMPL(Tokenizer::DataTokenizer, Object, ITokenizerDataTokenizer);

ECode Tokenizer::DataTokenizer::constructor(
    /* [in] */ ITokenizer* host)
{
    mHost = (Tokenizer*)host;
    return NOERROR;
}

ECode Tokenizer::DataTokenizer::Start(
    /* [out] */ ITokenizerTokenizerPhase** result)
{
    VALIDATE_NOT_NULL(result);
    *result = this;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode Tokenizer::DataTokenizer::Tokenize()
{
    String line = mHost->mLine;
    Int32 end = line.GetLength();
    for (Int32 pos = mHost->mHandledLen; pos < end; pos++) {
        if (line.GetChar(pos) == '&') {
            if (ReplaceEscape(String("&amp;"), String("&"), pos) ||
                    ReplaceEscape(String("&lt;"), String("<"), pos) ||
                    ReplaceEscape(String("&gt;"), String(">"), pos) ||
                    ReplaceEscape(String("&lrm;"), String("\u200e"), pos) ||
                    ReplaceEscape(String("&rlm;"), String("\u200f"), pos) ||
                    ReplaceEscape(String("&nbsp;"), String("\u00a0"), pos)) {
                continue;
            }
        } else if (line.GetChar(pos) == '<') {
            end = pos;
            AutoPtr<ITokenizerTokenizerPhase> ttk = mHost->mTagTokenizer;
            AutoPtr<ITokenizerTokenizerPhase> ttp;
            ttk->Start((ITokenizerTokenizerPhase**)&ttp);
            mHost->mPhase = ttp;
            break;
        }
    }
    mData.Append(line.Substring(mHost->mHandledLen, end));
    // yield mData
    AutoPtr<ITokenizerOnTokenListener> listener = mHost->mListener;
    listener->OnData(mData.ToString());
    Int32 length;
    mData.GetLength(&length);
    mData.Delete(0, length);
    mHost->mHandledLen = end;
    return NOERROR;
}

Boolean Tokenizer::DataTokenizer::ReplaceEscape(
    /* [in] */ const String& escape,
    /* [in] */ const String& replacement,
    /* [in] */ Int32 pos)
{
    String line = mHost->mLine;
    if (line.StartWith(escape)) {
        Int32 len = mHost->mHandledLen;
        String str = line.Substring(len, pos);
        mData.Append(str);
        mData.Append(replacement);
        Int32 length = escape.GetLength();
        mHost->mHandledLen = pos + length;
        pos = mHost->mHandledLen - 1;
        return TRUE;
    }
    return FALSE;
}

//===============================================================
//                  Tokenizer::TagTokenizer
//===============================================================
Tokenizer::TagTokenizer::TagTokenizer()
    : mAtAnnotation(FALSE)
{}

Tokenizer::TagTokenizer::~TagTokenizer()
{}

CAR_INTERFACE_IMPL_2(Tokenizer::TagTokenizer, Object, ITokenizerTagTokenizer, ITokenizerTokenizerPhase);

ECode Tokenizer::TagTokenizer::constructor(
    /* [in] */ ITokenizer* host)
{
    mHost = (Tokenizer*)host;
    return NOERROR;
}

ECode Tokenizer::TagTokenizer::Start(
    /* [out] */ ITokenizerTokenizerPhase** result)
{
    VALIDATE_NOT_NULL(result);
    mName = mAnnotation = String("");
    mAtAnnotation = FALSE;
    *result = this;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Tokenizer::TagTokenizer::Tokenize()
{
    if (!mAtAnnotation)
        (mHost->mHandledLen)++;

    const String& line = mHost->mLine;
    Int32 len = line.GetLength();
    Int32 handledLen = mHost->mHandledLen;
    if (handledLen < len) {
        AutoPtr<ArrayOf<String> > parts;
        /**
         * Collect annotations and end-tags to closing >.  Collect tag
         * name to closing bracket or next white-space.
         */
        String subStr = line.Substring(handledLen);
        if (mAtAnnotation || line.GetChar(handledLen) == '/') {
            StringUtils::Split(subStr, String(">"), (ArrayOf<String>**)&parts);
        } else {
            StringUtils::Split(subStr, String("[\t\f >]"), (ArrayOf<String>**)&parts);
        }
        Int32 len = ((*parts)[0]).GetLength();
        String part = line.Substring(
                    handledLen, handledLen + len);
        mHost->mHandledLen += len;

        if (mAtAnnotation) {
            mAnnotation += String(" ") + part;
        } else {
            mName = part;
        }
    }

    mAtAnnotation = TRUE;

    handledLen = mHost->mHandledLen;
    if (handledLen < line.GetLength() && line.GetChar(handledLen) == '>') {
        Yield_tag();
        AutoPtr<ITokenizerTokenizerPhase> dtk = mHost->mDataTokenizer;
        AutoPtr<ITokenizerTokenizerPhase> ttp;
        dtk->Start((ITokenizerTokenizerPhase**)&ttp);
        mHost->mPhase = ttp;
        (mHost->mHandledLen)++;
    }
    return NOERROR;
}

void Tokenizer::TagTokenizer::Yield_tag()
{
    AutoPtr<ITokenizerOnTokenListener> listener = mHost->mListener;

    if (mName.StartWith(String("/"))) {
        listener->OnEnd(mName.Substring(1));
    } else {
        if (mName.GetLength() > 0 && Character::IsDigit(mName.GetChar(0))) {
            // timestamp
            // try {
            ECode ec = NOERROR;
            Int64 timestampMs;
            ec = WebVttParserHelper::ParseTimestampMs(mName, &timestampMs);
            FAIL_GOTO(ec, error);
            ec = listener->OnTimeStamp(timestampMs);
            FAIL_GOTO(ec, error);
            // } catch (NumberFormatException e) {
        error:
            {
                Slogger::D(TAG, "invalid timestamp tag: < %s >", mName.string());
            }
            // }
        } else {
            String result;
            StringUtils::ReplaceAll(mAnnotation, String("\\s+"), String(" "), &result);
            mAnnotation = result;
            if (mAnnotation.StartWith(String(" "))) {
                mAnnotation = mAnnotation.Substring(1);
            }
            if (mAnnotation.EndWith(String(" "))) {
                mAnnotation = mAnnotation.Substring(0, mAnnotation.GetLength() - 1);
            }

            AutoPtr<ArrayOf<String> > classes;
            Int32 dotAt = mName.IndexOf('.');
            if (dotAt >= 0) {
                String subStr = mName.Substring(dotAt + 1);
                StringUtils::Split(subStr, String("\\."), (ArrayOf<String>**)&classes);
                mName = mName.Substring(0, dotAt);
            }
            listener->OnStart(mName, classes.Get(), mAnnotation);
        }
    }
}

//===============================================================
//                  Tokenizer
//===============================================================
const String Tokenizer::TAG(String("Tokenizer"));

Tokenizer::Tokenizer()
{}

Tokenizer::~Tokenizer()
{}

CAR_INTERFACE_IMPL(Tokenizer, Object, ITokenizer)

ECode Tokenizer::constructor(
    /* [in] */ ITokenizerOnTokenListener* listener)
{
    AutoPtr<DataTokenizer> dt = new DataTokenizer();
    dt->constructor(this);
    mDataTokenizer = ITokenizerTokenizerPhase::Probe(dt);
    AutoPtr<TagTokenizer> dtr = new TagTokenizer();
    dtr->constructor(this);
    mTagTokenizer = ITokenizerTokenizerPhase::Probe(dtr);
    Reset();
    mListener = listener;
    return NOERROR;
}

ECode Tokenizer::Reset()
{
    return mDataTokenizer->Start((ITokenizerTokenizerPhase**)&mPhase);
}

ECode Tokenizer::Tokenize(
    /* [in] */ const String& s)
{
    mHandledLen = 0;
    mLine = s;
    Int32 length = mLine.GetLength();
    while (mHandledLen < length) {
        mPhase->Tokenize();
    }
    /* we are finished with a line unless we are in the middle of a tag */
    if (!(ITokenizerTokenizerPhase::Probe(mPhase))) {
        // yield END-OF-LINE
        mListener->OnLineEnd();
    }
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos