#include "Elastos.CoreLibrary.h"
#include "Elastos.Droid.Media.h"
#include "elastos/droid/media/WebVttParser.h"
#include "elastos/droid/media/TextTrackCue.h"
#include "elastos/droid/media/CWebVttRendererTextTrackCue.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringUtils.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Core::StringUtils;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Media {
//===============================================================
//                  WebVttParser::SkipRestPhase
//===============================================================
WebVttParser::SkipRestPhase::SkipRestPhase(
    /* [in] */ WebVttParser* host)
{
    mHost = host;
}

WebVttParser::SkipRestPhase::~SkipRestPhase()
{}

CAR_INTERFACE_IMPL(WebVttParser::SkipRestPhase, Object, IWebVttParserPhase)

ECode WebVttParser::SkipRestPhase::Parse(
    /* [in] */ const String& line)
{
    return NOERROR;
}

//===============================================================
//                  WebVttParser::ParseStartPhase
//===============================================================
WebVttParser::ParseStartPhase::ParseStartPhase(
    /* [in] */ WebVttParser* host)
{
    mHost = host;
}

WebVttParser::ParseStartPhase::~ParseStartPhase()
{}

CAR_INTERFACE_IMPL(WebVttParser::ParseStartPhase, Object, IWebVttParserPhase)

ECode WebVttParser::ParseStartPhase::Parse(
    /* [in] */ const String& line)
{
    String line_;
    if (line.StartWith(String("\ufeff"))) {
        line_ = line.Substring(1);
    }

    if (!line_.Equals(String("WEBVTT")) &&
            !line_.StartWith(String("WEBVTT ")) &&
            !line_.StartWith(String("WEBVTT\t"))) {
        Slogger::W("WebVttParser::ParseStartPhase", "Not a WEBVTT header %s", line.string());
        mHost->mPhase = mHost->mSkipRest;
    } else {
        mHost->mPhase = mHost->mParseHeader;
    }
    return NOERROR;
}

//===============================================================
//                  WebVttParser::ParseCueIdPhase
//===============================================================
WebVttParser::ParseCueIdPhase::ParseCueIdPhase(
    /* [in] */ WebVttParser* host)
{
    mHost = host;
}

WebVttParser::ParseCueIdPhase::~ParseCueIdPhase()
{}

CAR_INTERFACE_IMPL(WebVttParser::ParseCueIdPhase, Object, IWebVttParserPhase)

ECode WebVttParser::ParseCueIdPhase::Parse(
    /* [in] */ const String& line)
{
    if (line.GetLength() == 0) {
        return NOERROR;
    }

    assert(mHost->mCue == NULL);

    if (line.Equals(String("NOTE")) || line.StartWith(String("NOTE "))) {
        mHost->mPhase = mHost->mParseCueText;
    }

    AutoPtr<ITextTrackCue> ttk;
    CWebVttRendererTextTrackCue::New((ITextTrackCue**)&ttk);
    AutoPtr<IVector> cts = mHost->mCueTexts;
    cts->Clear();

    mHost->mPhase = mHost->mParseCueTime;
    if (line.Contains(String("-->"))) {
        AutoPtr<IWebVttParserPhase> ps = mHost->mPhase;
        ps->Parse(line);
    } else {
        AutoPtr<ITextTrackCue> cue = mHost->mCue;
        cue->SetId(line);
    }
    return NOERROR;
}

//===============================================================
//                  WebVttParser::ParseCueTimePhase
//===============================================================
WebVttParser::ParseCueTimePhase::ParseCueTimePhase(
    /* [in] */ WebVttParser* host)
{
    mHost = host;
}

WebVttParser::ParseCueTimePhase::~ParseCueTimePhase()
{}

CAR_INTERFACE_IMPL(WebVttParser::ParseCueTimePhase, Object, IWebVttParserPhase)

ECode WebVttParser::ParseCueTimePhase::Parse(
    /* [in] */ const String& line)
{
    Int32 arrowAt = line.IndexOf(String("-->"));
    if (arrowAt < 0) {
        mHost->mCue = NULL;
        mHost->mPhase = mHost->mParseCueId;
        return NOERROR;
    }

    String start_ = line.Substring(0, arrowAt);
    String start = start_.Trim();
    // convert only initial and first other white-space to space
    String subStr = line.Substring(arrowAt + 3);
    String replaceStr;
    StringUtils::ReplaceFirst(subStr, String("^\\s+"), String(""), &replaceStr);
    String rest;
    StringUtils::ReplaceFirst(replaceStr, String("\\s+"), String(" "), &rest);
    Int32 spaceAt = rest.IndexOf(' ');
    String end = spaceAt > 0 ? rest.Substring(0, spaceAt) : rest;
    rest = spaceAt > 0 ? rest.Substring(spaceAt + 1) : String("");

    AutoPtr<ITextTrackCue> cue = mHost->mCue;
    //TODO
    // mCue.mStartTimeMs = ParseTimestampMs(start);
    // mCue.mEndTimeMs = ParseTimestampMs(end);
    AutoPtr<ArrayOf<String> > splits;
    StringUtils::Split(rest, String(" +"), (ArrayOf<String>**)&splits);

    Int32 length = splits->GetLength();
    for (Int32 i = 0; i < length; i++) {
        String setting = (*splits)[i];
        Int32 colonAt = setting.IndexOf(':');
        Int32 settingLen = setting.GetLength();
        if (colonAt <= 0 || colonAt == settingLen - 1) {
            continue;
        }
        String name = setting.Substring(0, colonAt);
        String value = setting.Substring(colonAt + 1);

        if (name.Equals(String("region"))) {
            cue->SetRegionId(value);
        } else if (name.Equals(String("vertical"))) {
            AutoPtr<ITextTrackCue> ttc = mHost->mCue;
            if (value.Equals(String("rl"))) {
                ttc->SetWritingDirection(ITextTrackCue::WRITING_DIRECTION_VERTICAL_RL);
            } else if (value.Equals(String("lr"))) {
                ttc->SetWritingDirection(ITextTrackCue::WRITING_DIRECTION_VERTICAL_LR);
            } else {
                Slogger::W("cue setting", "%s has invalid value%s", name.string(), value.string());
            }
                } else if (name.Equals(String("line"))) {
                    // try {
                        /* TRICKY: we know that there are no spaces in value */
                        assert(value.IndexOf(' ') < 0);
                        Boolean bRet = FALSE;
                        StringUtils::Matches(value, String(".*[^0-9].*"), &bRet);
                        if (value.EndWith(String("%"))) {
                            cue->SetSnapToLines(FALSE);
                            // mCue.mLinePosition = ParseIntPercentage(value);
                        } else if (bRet) {
                            Slogger::W("cue setting", "%s contains an invalid character %s", name.string(), value.string());
                        } else {
                            cue->SetSnapToLines(TRUE);
                            cue->SetLinePosition(StringUtils::ParseInt32(value));
                        }
                    // } catch (NumberFormatException e) {
                        // Slogger::W("cue setting", "%s is not numeric or percentage%s", name.string(), value.string());
                    // }
                    // TODO: add support for optional alignment value [,start|middle|end]
                } else if (name.Equals(String("position"))) {
                    // try {
                        //TODO
                        // mCue.mTextPosition = ParseIntPercentage(value);
                    // } catch (NumberFormatException e) {
                        // Slogger::W("cue setting", "%s is not numeric or percentage %s", name.string(), value.string());
                    // }
                } else if (name.Equals(String("size"))) {
                    // try {
                        //TODO
                        // mCue.mSize = ParseIntPercentage(value);
                    // } catch (NumberFormatException e) {
                        // Slogger::W("cue setting", "%s is not numeric or percentage %s", name.string(), value.string());
                    // }
                } else if (name.Equals(String("align"))) {
                    if (value.Equals(String("start"))) {
                        cue->SetAlignment(ITextTrackCue::ALIGNMENT_START);
                    } else if (value.Equals(String("middle"))) {
                        cue->SetAlignment(ITextTrackCue::ALIGNMENT_MIDDLE);
                    } else if (value.Equals(String("end"))) {
                        cue->SetAlignment(ITextTrackCue::ALIGNMENT_END);
                    } else if (value.Equals(String("left"))) {
                        cue->SetAlignment(ITextTrackCue::ALIGNMENT_LEFT);
                    } else if (value.Equals(String("right"))) {
                        cue->SetAlignment(ITextTrackCue::ALIGNMENT_RIGHT);
                    } else {
                        Slogger::W("cue setting", "%s has invalid value %s", name.string(), value.string());
                        continue;
                    }
                }
            }

            Int32 size;
            Int32 writingDirection;

            if (/*mCue.mLinePosition != NULL ||*/
                    (cue->GetSize(&size), size) != 100 ||
                    ((cue->GetWritingDirection(&writingDirection), writingDirection) !=
                        ITextTrackCue::WRITING_DIRECTION_HORIZONTAL)) {
                cue->SetRegionId(String(""));
            }

           mHost->mPhase = mHost->mParseCueText;
           return NOERROR;
}

//===============================================================
//                  WebVttParser::ParseCueTextPhase
//===============================================================
WebVttParser::ParseCueTextPhase::ParseCueTextPhase(
    /* [in] */ WebVttParser* host)
{
    mHost = host;
}

WebVttParser::ParseCueTextPhase::~ParseCueTextPhase()
{}

CAR_INTERFACE_IMPL(WebVttParser::ParseCueTextPhase, Object, IWebVttParserPhase)

ECode WebVttParser::ParseCueTextPhase::Parse(
    /* [in] */ const String& line)
{
    if (line.GetLength() == 0) {
        mHost->YieldCue();
        mHost->mPhase = mHost->mParseCueId;
    }
    else if (mHost->mCue != NULL) {
        AutoPtr<ICharSequence> cs;
        CString::New(line, (ICharSequence**)&cs);
        AutoPtr<IVector> cts = mHost->mCueTexts;
        cts->Add(cs);
    }
    return NOERROR;
}

//===============================================================
//                  WebVttParser
//===============================================================
const String WebVttParser::TAG(String("WebVttParser"));

CAR_INTERFACE_IMPL(WebVttParser, Object, IWebVttParser);

WebVttParser::WebVttParser()
{}

WebVttParser::~WebVttParser()
{}

ECode WebVttParser::constructor(
    /* [in] */ IWebVttCueListener* listener)
{
    AutoPtr<SkipRestPhase> skipRest = new SkipRestPhase(this);
    mSkipRest = IWebVttParserPhase::Probe(skipRest);
    AutoPtr<ParseStartPhase> parseStart = new ParseStartPhase(this);
    mParseStart = IWebVttParserPhase::Probe(parseStart);
    AutoPtr<ParseCueIdPhase> parseCueId = new ParseCueIdPhase(this);
    mParseCueId = IWebVttParserPhase::Probe(parseCueId);
    AutoPtr<ParseCueTimePhase> parseCueTime = new ParseCueTimePhase(this);
    mParseCueTime = IWebVttParserPhase::Probe(parseCueTime);
    AutoPtr<ParseCueTextPhase> parseCueText = new ParseCueTextPhase(this);
    mParseCueText = IWebVttParserPhase::Probe(parseCueText);

    mPhase = mParseStart;
    mBuffer = String("");
    mListener = listener;

    return NOERROR;
}

ECode WebVttParser::Parse(
    /* [in] */ const String& s)
{
    Boolean trailingCR = FALSE;
    String result;
    StringUtils::ReplaceAll(s, String("\0"), String("\ufffd"), &result);
    String ret;
    StringUtils::ReplaceAll(mBuffer + result, String("\r\n"), String("\n"), &ret);
    mBuffer = ret;

    /* keep trailing '\r' in case matching '\n' arrives in next packet */
    if (mBuffer.EndWith(String("\r"))) {
        trailingCR = TRUE;
        Int32 len = mBuffer.GetLength();
        mBuffer = mBuffer.Substring(0, len - 1);
    }

    AutoPtr<ArrayOf<String> > lines;
    StringUtils::Split(mBuffer, String("[\r\n]"), (ArrayOf<String>**)&lines);
    Int32 length = lines->GetLength();
    for (Int32 i = 0; i < length - 1; i++) {
        mPhase->Parse((*lines)[i]);
    }

    mBuffer = (*lines)[length - 1];
    if (trailingCR)
        mBuffer += String("\r");

    return NOERROR;
}

ECode WebVttParser::Eos()
{
    Int32 len = mBuffer.GetLength();
    if (mBuffer.EndWith(String("\r"))) {
        mBuffer = mBuffer.Substring(0, len - 1);
    }

    mPhase->Parse(mBuffer);
    mBuffer = String("");

    YieldCue();
    mPhase = mParseStart;
    return NOERROR;
}

ECode WebVttParser::YieldCue()
{
    Int32 size = 0;
    mCueTexts->GetSize(&size);
    if (mCue != NULL && size > 0) {
        AutoPtr<ArrayOf<ICharSequence*> > strings;
        mCueTexts->ToArray((ArrayOf<IInterface*>**)&strings);
        ((TextTrackCue*)mCue.Get())->mStrings = strings;
        mCueTexts->Clear();
        mListener->OnCueParsed(mCue);
    }
    mCue = NULL;
    return NOERROR;
}

void WebVttParser::Log_warning(
    /* [in] */ const String& nameType,
    /* [in] */ const String& name,
    /* [in] */ const String& message,
    /* [in] */ const String& subMessage,
    /* [in] */ const String& value)
{
    Slogger::W(TAG, "%s '%s' %s ('%s' %s)", nameType.string(), name.string(),
                message.string(), value.string(), subMessage.string());
}

void WebVttParser::Log_warning(
    /* [in] */ const String& nameType,
    /* [in] */ const String& name,
    /* [in] */ const String& message,
    /* [in] */ const String& value)
{
    Slogger::W(TAG, "%s '%s' %s ('%s')", nameType.string(), name.string(), message.string(), value.string());
}

void WebVttParser::Log_warning(
    /* [in] */ const String& message,
    /* [in] */ const String& value)
{
    Slogger::W(TAG, "%s ('%s')", message.string(), value.string());
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos