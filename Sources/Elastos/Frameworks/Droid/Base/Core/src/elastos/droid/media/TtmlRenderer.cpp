#include "elastos/droid/media/TtmlRenderer.h"
#include "elastos/droid/view/accessibility/CaptioningManager.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Text.h"
#include "elastos/droid/text/TextUtils.h"
#include <Elastos.CoreLibrary.External.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/utility/regex/Pattern.h>
#include <elastos/core/StringUtils.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Utility::ILinkedList;
using Elastos::Utility::CLinkedList;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Regex::IMatcher;
using Elastos::Utility::Regex::Pattern;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::EIID_IViewManager;
using Elastos::Droid::View::EIID_IViewGroup;
using Elastos::Droid::View::EIID_IViewParent;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewManager;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::Accessibility::CaptioningManager;
using Elastos::Droid::View::Accessibility::ICaptioningManagerCaptionStyle;
using Elastos::Droid::Widget::EIID_ILinearLayout;
using Elastos::Droid::Widget::ILinearLayout;
using Org::Xmlpull::V1::CXmlPullParserFactory;
using Org::Xmlpull::V1::CXmlPullParserFactoryHelper;
using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlPullParserFactory;
using Org::Xmlpull::V1::IXmlPullParserFactoryHelper;
using Elastos::IO::CStringReader;
using Elastos::IO::IStringReader;
using Elastos::IO::IReader;
using Elastos::Core::StringUtils;

namespace Elastos{
namespace Droid {
namespace Media {
//================================================================================
//                      TtmlRenderer
//================================================================================
const String TtmlRenderer::MEDIA_MIMETYPE_TEXT_TTML(String("application/ttml+xml"));

TtmlRenderer::TtmlRenderer()
{}

TtmlRenderer::~TtmlRenderer()
{}

CAR_INTERFACE_IMPL_2(TtmlRenderer, Object, ITtmlRenderer, ISubtitleControllerRenderer)

ECode TtmlRenderer::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    return NOERROR;
}


ECode TtmlRenderer::Supports(
    /* [in] */ IMediaFormat* format,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean flag = FALSE;
    format->ContainsKey(IMediaFormat::KEY_MIME, &flag);
    if (flag) {
        String str;
        format->GetString(IMediaFormat::KEY_MIME, &str);
        *result = str.Equals(MEDIA_MIMETYPE_TEXT_TTML);
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode TtmlRenderer::CreateTrack(
    /* [in] */ IMediaFormat* format,
    /* [out] */ ISubtitleTrack** result)
{
    VALIDATE_NOT_NULL(result);

    if (mRenderingWidget == NULL) {
        mRenderingWidget = new TtmlRenderingWidget(mContext);
    }
    AutoPtr<TtmlTrack> tt = new TtmlTrack(mRenderingWidget, format);
    *result = ISubtitleTrack::Probe(tt);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//================================================================================
//                      TtmlUtils
//================================================================================
static AutoPtr<IPattern> initCLOCK_TIME()
{
    AutoPtr<IPattern> p;
    Pattern::Compile(
            String("^([0-9][0-9]+):([0-9][0-9]):([0-9][0-9])")
            + String("(?:(\\.[0-9]+)|:([0-9][0-9])(?:\\.([0-9]+))?)?$"), (IPattern**)&p);
    return p;
}

static AutoPtr<IPattern> initOFFSET_TIME()
{
    AutoPtr<IPattern> p;
    Pattern::Compile(String("^([0-9]+(?:\\.[0-9]+)?)(h|m|s|ms|f|t)$"), (IPattern**)&p);
    return p;
}

const AutoPtr<IPattern> TtmlUtils::CLOCK_TIME = initCLOCK_TIME();

const AutoPtr<IPattern> TtmlUtils::OFFSET_TIME = initOFFSET_TIME();

ECode TtmlUtils::ParseTimeExpression(
    /* [in] */ const String& time,
    /* [in] */ Int32 frameRate,
    /* [in] */ Int32 subframeRate,
    /* [in] */ Int32 tickRate,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IMatcher> matcher;
    CLOCK_TIME->Matcher(time, (IMatcher**)&matcher);
    Boolean flag = FALSE;
    matcher->Matches(&flag);
    if (flag) {
        String hours;
        IMatchResult::Probe(matcher)->Group(1, &hours);
        Double durationSeconds = StringUtils::ParseInt64(hours) * 3600;
        String minutes;
        IMatchResult::Probe(matcher)->Group(2, &minutes);
        durationSeconds += StringUtils::ParseInt64(minutes) * 60;
        String seconds;
        IMatchResult::Probe(matcher)->Group(3, &seconds);
        durationSeconds += StringUtils::ParseInt64(seconds);
        String fraction;
        IMatchResult::Probe(matcher)->Group(4, &fraction);
        durationSeconds += (fraction != NULL) ? StringUtils::ParseDouble(fraction) : 0;
        String frames;
        IMatchResult::Probe(matcher)->Group(5, &frames);
        durationSeconds += (frames != NULL) ? ((Double)StringUtils::ParseInt64(frames)) / frameRate : 0;
        String subframes;
        IMatchResult::Probe(matcher)->Group(6, &subframes);
        durationSeconds += (subframes != NULL) ? ((Double)StringUtils::ParseInt64(subframes))
                / subframeRate / frameRate
                    : 0;
        return (Int64)(durationSeconds * 1000);
    }
    matcher = NULL;
    OFFSET_TIME->Matcher(time, (IMatcher**)&matcher);
    matcher->Matches(&flag);
    if (flag) {
        String timeValue;
        IMatchResult::Probe(matcher)->Group(1, &timeValue);
        Double value = StringUtils::ParseDouble(timeValue);
        String unit;
        IMatchResult::Probe(matcher)->Group(2, &unit);
        if (unit.Equals(String("h"))) {
            value *= 3600LL * 1000000LL;
        } else if (unit.Equals(String("m"))) {
            value *= 60 * 1000000;
        } else if (unit.Equals(String("s"))) {
            value *= 1000000;
        } else if (unit.Equals(String("ms"))) {
            value *= 1000;
        } else if (unit.Equals(String("f"))) {
            value = value / frameRate * 1000000;
        } else if (unit.Equals(String("t"))) {
            value = value / tickRate * 1000000;
        }
            return (Int64)value;
        }

    Slogger::E("TtmlUtils", "Malformed time expression : %s" , time.string());
    return E_NUMBER_FORMAT_EXCEPTION;
}

ECode TtmlUtils::ApplyDefaultSpacePolicy(
    /* [in] */ const String& in,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    return ApplySpacePolicy(in, TRUE, result);
}

ECode TtmlUtils::ApplySpacePolicy(
    /* [in] */ const String& in,
    /* [in] */ Boolean treatLfAsSpace,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    // Removes CR followed by LF. ref:
    // http://www.w3.org/TR/xml/#sec-line-ends
    String crRemoved;
    StringUtils::ReplaceAll(in, String("\r\n"), String("\n"), &crRemoved);

    // Apply suppress-at-line-break="auto" and
    // white-space-treatment="ignore-if-surrounding-linefeed"
    String spacesNeighboringLfRemoved;
    StringUtils::ReplaceAll(crRemoved, String(" *\n *"), String("\n"), &spacesNeighboringLfRemoved);
    // Apply linefeed-treatment="treat-as-space"
    String lfToSpace;
    if (treatLfAsSpace) {
        StringUtils::ReplaceAll(spacesNeighboringLfRemoved, String("\n"), String(" "), &lfToSpace);
    } else {
        lfToSpace = spacesNeighboringLfRemoved;
    }
    // Apply white-space-collapse="true"
    String spacesCollapsed;
    StringUtils::ReplaceAll(lfToSpace, String("[ \t\\x0B\f\r]+"), String(" "), &spacesCollapsed);
    *result = spacesCollapsed;
    return NOERROR;
}

ECode TtmlUtils::ExtractText(
    /* [in] */ TtmlNode* root,
    /* [in] */ Int64 startUs,
    /* [in] */ Int64 endUs,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    StringBuilder text ;
    ExtractText(root, startUs, endUs, &text, FALSE);
    return StringUtils::ReplaceAll(text.ToString(), String("\n$"), String(""), result);
}

ECode TtmlUtils::ExtractTtmlFragment(
    /* [in] */ TtmlNode* root,
    /* [in] */ Int64 startUs,
    /* [in] */ Int64 endUs,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    StringBuilder fragment;
    ExtractTtmlFragment(root, startUs, endUs, &fragment);
    *result = fragment.ToString();
    return NOERROR;
}

void TtmlUtils::ExtractText(
    /* [in] */ TtmlNode* node,
    /* [in] */ Int64 startUs,
    /* [in] */ Int64 endUs,
    /* [in] */ StringBuilder* out,
    /* [in] */ Boolean inPTag)
{
    Boolean flag = FALSE;
    node->IsActive(startUs, endUs, &flag);
    if (node->mName.Equals(ITtmlUtils::PCDATA) && inPTag) {
        out->Append(node->mText);
    } else if (node->mName.Equals(ITtmlUtils::TAG_BR) && inPTag) {
        out->Append(String("\n"));
    } else if (node->mName.Equals(ITtmlUtils::TAG_METADATA)) {
        // do nothing.
    } else if (flag) {
        Boolean pTag = node->mName.Equals(ITtmlUtils::TAG_P);
        Int32 length;
        out->GetLength(&length);
        Int32 size;
        node->mChildren->GetSize(&size);
        AutoPtr<IInterface> obj;
        AutoPtr<TtmlNode> tn;
        for (Int32 i = 0; i < size; ++i) {
            obj = NULL;
            node->mChildren->Get(i, (IInterface**)&obj);
            tn = NULL;
            tn = (TtmlNode*)ITtmlNode::Probe(obj);
            ExtractText(tn.Get(), startUs, endUs, out, pTag || inPTag);
        }
        Int32 len;
        out->GetLength(&len);
        if (pTag && length != len) {
            out->Append(String("\n"));
        }
    }
}

void TtmlUtils::ExtractTtmlFragment(
    /* [in] */ TtmlNode* node,
    /* [in] */ Int64 startUs,
    /* [in] */ Int64 endUs,
    /* [in] */ StringBuilder* out)
{
    Boolean flag = FALSE;
    node->IsActive(startUs, endUs, &flag);
    if (node->mName.Equals(ITtmlUtils::PCDATA)) {
        out->Append(node->mText);
    } else if (node->mName.Equals(ITtmlUtils::TAG_BR)) {
        out->Append(String("<br/>"));
    } else if (flag) {
        out->Append(String("<"));
        out->Append(node->mName);
        out->Append(node->mAttributes);
        out->Append(">");
        Int32 size;
        node->mChildren->GetSize(&size);
        AutoPtr<IInterface> obj;
        AutoPtr<TtmlNode> tn;
        for (Int32 i = 0; i < size; ++i) {
            obj = NULL;
            node->mChildren->Get(i, (IInterface**)&obj);
            tn = NULL;
            tn = (TtmlNode*)ITtmlNode::Probe(obj);
            ExtractTtmlFragment(tn.Get(), startUs, endUs, out);
        }
        out->Append(String("</"));
        out->Append(node->mName);
        out->Append(String(">"));
    }
}

//================================================================================
//                      TtmlCue
//================================================================================
TtmlCue::TtmlCue(
    /* [in] */ Int64 startTimeMs,
    /* [in] */ Int64 endTimeMs,
    /* [in] */ const String& text,
    /* [in] */ const String& ttmlFragment)
{
    mStartTimeMs = startTimeMs;
    mEndTimeMs = endTimeMs;
    mText = text;
    mTtmlFragment = ttmlFragment;
}

TtmlCue::~TtmlCue()
{}

CAR_INTERFACE_IMPL(TtmlCue, Object, ISubtitleTrackCue)

//================================================================================
//                      TtmlNode
//================================================================================
TtmlNode::TtmlNode(
    /* [in] */ const String& name,
    /* [in] */ const String& attributes,
    /* [in] */ const String& text,
    /* [in] */ Int64 startTimeMs,
    /* [in] */ Int64 endTimeMs,
    /* [in] */ ITtmlNode* parent,
    /* [in] */ Int64 runId)
{
    mName = name;
    mAttributes = attributes;
    mText = text;
    mStartTimeMs = startTimeMs;
    mEndTimeMs = endTimeMs;
    mParent = parent;
    mRunId = runId;
}

TtmlNode::~TtmlNode()
{}

CAR_INTERFACE_IMPL(TtmlNode, Object, ITtmlNode)

ECode TtmlNode::IsActive(
    /* [in] */ Int64 startTimeMs,
    /* [in] */ Int64 endTimeMs,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mEndTimeMs > startTimeMs && mStartTimeMs < endTimeMs;
    return NOERROR;
}
//================================================================================
//                      TtmlParser
//================================================================================
const String TtmlParser::TAG(String("TtmlParser"));
const Int32 TtmlParser::DEFAULT_FRAMERATE = 30;
const Int32 TtmlParser::DEFAULT_SUBFRAMERATE = 1;
const Int32 TtmlParser::DEFAULT_TICKRATE = 1;

TtmlParser::TtmlParser(
    /* [in] */ ITtmlNodeListener* listener)
    : mListener(listener)
{
}

TtmlParser::~TtmlParser()
{}

CAR_INTERFACE_IMPL(TtmlParser, Object, ITtmlParser)

ECode TtmlParser::Parse(
    /* [in] */ const String& ttmlText,
    /* [in] */ Int64 runId)
{
    mParser = NULL;
    mCurrentRunId = runId;
    LoadParser(ttmlText);
    ParseTtml();
    return NOERROR;
}

void TtmlParser::LoadParser(
    /* [in] */ const String& ttmlFragment)
{
    AutoPtr<IXmlPullParserFactory> factory;
    AutoPtr<IXmlPullParserFactoryHelper> xmlHelper;
    CXmlPullParserFactoryHelper::AcquireSingleton((IXmlPullParserFactoryHelper**)&xmlHelper);
    xmlHelper->NewInstance((IXmlPullParserFactory**)&factory);
    factory->SetNamespaceAware(FALSE);
    factory->NewPullParser((IXmlPullParser**)&mParser);

    AutoPtr<IStringReader> in;
    CStringReader::New(ttmlFragment, (IStringReader**)&in);
    mParser->SetInput(IReader::Probe(in));
}

void TtmlParser::ExtractAttribute(
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ Int32 i,
    /* [in] */ StringBuilder* outSdr)
{
    outSdr->Append(String(" "));
    String name;
    parser->GetAttributeName(i, &name);
    outSdr->Append(name);
    outSdr->Append(String("=\""));
    String value;
    parser->GetAttributeValue(i, &value);
    outSdr->Append(value);
    outSdr->Append(String("\""));
}

void TtmlParser::ParseTtml()
{
    AutoPtr<ILinkedList> nodeStack;
    CLinkedList::New((ILinkedList**)&nodeStack);
    Int32 depthInUnsupportedTag = 0;
    Boolean active = TRUE;
    AutoPtr<IInterface> obj;
    AutoPtr<TtmlNode> parent;
    Boolean flag = FALSE;
    while (!(IsEndOfDoc(&flag), flag)) {
        Int32 eventType;
        mParser->GetEventType(&eventType);
        obj = NULL;
        nodeStack->PeekLast((IInterface**)&obj);
        parent = NULL;
        parent = (TtmlNode*)ITtmlNode::Probe(obj);
        String name;
        mParser->GetName(&name);
        if (active) {
            if (eventType == IXmlPullParser::START_TAG) {
                Boolean tag = FALSE;
                if (!(IsSupportedTag(name, &tag), tag)) {
                    Slogger::W(TAG, "Unsupported tag %s is ignored.", name.string());
                    depthInUnsupportedTag++;
                    active = FALSE;
                } else {
                    AutoPtr<TtmlNode> node;
                    ParseNode(parent, (TtmlNode**)&node);
                    nodeStack->AddLast(ITtmlNode::Probe(node));
                    if (parent != NULL) {
                        parent->mChildren->Add(ITtmlNode::Probe(node));
                    }
                }
            } else if (eventType == IXmlPullParser::TEXT) {
                String ptext;
                mParser->GetText(&ptext);
                String text;
                TtmlUtils::ApplyDefaultSpacePolicy(ptext, &text);
                if (!TextUtils::IsEmpty(text)) {
                    AutoPtr<TtmlNode> tn = new TtmlNode(ITtmlUtils::PCDATA, String(""), text, 0, ITtmlUtils::INVALID_TIMESTAMP,
                            parent, mCurrentRunId);
                    parent->mChildren->Add(ITtmlNode::Probe(tn));
                }
            } else if (eventType == IXmlPullParser::END_TAG) {
                if (name.Equals(ITtmlUtils::TAG_P)) {
                    obj = NULL;
                    nodeStack->GetLast((IInterface**)&obj);
                    AutoPtr<TtmlNode> n = NULL;
                    n = (TtmlNode*)ITtmlNode::Probe(obj);
                    mListener->OnTtmlNodeParsed(n);
                } else if (name.Equals(ITtmlUtils::TAG_TT)) {
                    obj = NULL;
                    nodeStack->GetLast((IInterface**)&obj);
                    AutoPtr<TtmlNode> d = NULL;
                    d = (TtmlNode*)ITtmlNode::Probe(obj);
                    mListener->OnRootNodeParsed(d);
                }
                obj = NULL;
                nodeStack->RemoveLast((IInterface**)&obj);
            }
        } else {
            if (eventType == IXmlPullParser::START_TAG) {
                depthInUnsupportedTag++;
            } else if (eventType == IXmlPullParser::END_TAG) {
                depthInUnsupportedTag--;
                if (depthInUnsupportedTag == 0) {
                    active = TRUE;
                }
            }
        }
        Int32 i;
        mParser->Next(&i);
    }
}

ECode TtmlParser::ParseNode(
    /* [in] */ TtmlNode* parent,
    /* [out] */ TtmlNode** result)
{
    VALIDATE_NOT_NULL(result);

    Int32 eventType;
    mParser->GetEventType(&eventType);
    if (!(eventType == IXmlPullParser::START_TAG)) {
        *result = NULL;
        return NOERROR;
    }
    StringBuilder attrStr;
    Int64 start = 0;
    Int64 end = ITtmlUtils::INVALID_TIMESTAMP;
    Int64 dur = 0;
    Int32 count;
    mParser->GetAttributeCount(&count);
    for (Int32 i = 0; i < count; ++i) {
        String attr;
        mParser->GetAttributeName(i, &attr);
        String value;
        mParser->GetAttributeValue(i, &value);
        // TODO: check if it's safe to ignore the namespace of attributes as follows.
        String str;
        StringUtils::ReplaceFirst(attr, String("^.*:"), String(""), &str);
        attr = str;
        if (attr.Equals(ITtmlUtils::ATTR_BEGIN)) {
            TtmlUtils::ParseTimeExpression(value, DEFAULT_FRAMERATE,
                DEFAULT_SUBFRAMERATE, DEFAULT_TICKRATE, &start);
        } else if (attr.Equals(ITtmlUtils::ATTR_END)) {
            TtmlUtils::ParseTimeExpression(value, DEFAULT_FRAMERATE, DEFAULT_SUBFRAMERATE,
                DEFAULT_TICKRATE, &end);
        } else if (attr.Equals(ITtmlUtils::ATTR_DURATION)) {
            TtmlUtils::ParseTimeExpression(value, DEFAULT_FRAMERATE, DEFAULT_SUBFRAMERATE,
                    DEFAULT_TICKRATE, &dur);
        } else {
            ExtractAttribute(mParser.Get(), i, &attrStr);
        }
    }
    if (parent != NULL) {
        start += parent->mStartTimeMs;
        if (end != ITtmlUtils::INVALID_TIMESTAMP) {
            end += parent->mStartTimeMs;
        }
    }
    if (dur > 0) {
        if (end != ITtmlUtils::INVALID_TIMESTAMP) {
            Slogger::E(TAG, "'dur' and 'end' attributes are defined at the same time.'end' value is ignored.");
        }
        end = start + dur;
    }
    if (parent != NULL) {
        // If the end time remains unspecified, then the end point is
        // interpreted as the end point of the external time interval.
        if (end == ITtmlUtils::INVALID_TIMESTAMP &&
                parent->mEndTimeMs != ITtmlUtils::INVALID_TIMESTAMP &&
                end > parent->mEndTimeMs) {
            end = parent->mEndTimeMs;
        }
    }
    String pName;
    mParser->GetName(&pName);
    AutoPtr<TtmlNode> node = new TtmlNode(pName, attrStr.ToString(), String(NULL), start, end,
            parent, mCurrentRunId);
    *result = node;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode TtmlParser::IsEndOfDoc(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 eventType;
    mParser->GetEventType(&eventType);
    *result = (eventType == IXmlPullParser::END_DOCUMENT);
    return NOERROR;
}

ECode TtmlParser::IsSupportedTag(
    /* [in] */ const String& tag,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (tag.Equals(ITtmlUtils::TAG_TT) || tag.Equals(ITtmlUtils::TAG_HEAD) ||
            tag.Equals(ITtmlUtils::TAG_BODY) || tag.Equals(ITtmlUtils::TAG_DIV) ||
            tag.Equals(ITtmlUtils::TAG_P) || tag.Equals(ITtmlUtils::TAG_SPAN) ||
            tag.Equals(ITtmlUtils::TAG_BR) || tag.Equals(ITtmlUtils::TAG_STYLE) ||
            tag.Equals(ITtmlUtils::TAG_STYLING) || tag.Equals(ITtmlUtils::TAG_LAYOUT) ||
            tag.Equals(ITtmlUtils::TAG_REGION) || tag.Equals(ITtmlUtils::TAG_METADATA) ||
            tag.Equals(ITtmlUtils::TAG_SMPTE_IMAGE) || tag.Equals(ITtmlUtils::TAG_SMPTE_DATA) ||
            tag.Equals(ITtmlUtils::TAG_SMPTE_INFORMATION)) {
        *result = TRUE;
    }
    *result = FALSE;
    return NOERROR;
}

//================================================================================
//                      TtmlTrack
//================================================================================
const String TtmlTrack::TAG(String("TtmlTrack"));

TtmlTrack::TtmlTrack(
    /* [in] */ ITtmlRenderingWidget* renderingWidget,
    /* [in] */ IMediaFormat* format)
{
    SubtitleTrack::constructor(format);

    mParser = new TtmlParser(this);
    mRenderingWidget = (TtmlRenderingWidget*)renderingWidget;
    mParsingData = String("");
}

TtmlTrack::~TtmlTrack()
{}

CAR_INTERFACE_IMPL_3(TtmlTrack, Object, ITtmlNodeListener, ISubtitleTrack, IMediaTimeProviderOnMediaTimeListener)

ECode TtmlTrack::GetRenderingWidget(
    /* [out] */ ISubtitleTrackRenderingWidget** result)
{
    VALIDATE_NOT_NULL(result);
    *result = ISubtitleTrackRenderingWidget::Probe(mRenderingWidget);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode TtmlTrack::OnData(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Boolean eos,
    /* [in] */ Int64 runID)
{
    // try {
    // TODO: handle UTF-8 conversion properly
    String str(*data);

    // implement intermixing restriction for TTML.
    {    AutoLock syncLock(mParser);
        if (mCurrentRunID != NULL && runID != mCurrentRunID) {
            Slogger::E("TtmlTrack", "Run #%lld in progress.  Cannot process run #%d", mCurrentRunID, runID);
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        mCurrentRunID = runID;
        mParsingData += str;
        if (eos) {
            // try {
            mParser->Parse(mParsingData, mCurrentRunID);
            // } catch (XmlPullParserException e) {
                // e.printStackTrace();
            // } catch (IOException e) {
                // e.printStackTrace();
            // }
            FinishedRun(runID);
            mParsingData = String("");
            mCurrentRunID = NULL;
        }
    }
    // } catch (java.io.UnsupportedEncodingException e) {
        // Log.w(TAG, "subtitle data is not UTF-8 encoded: " + e);
    // }
    return NOERROR;
}

ECode TtmlTrack::OnTtmlNodeParsed(
    /* [in] */ ITtmlNode* node)
{
    mTtmlNodes.PushBack((TtmlNode*)node);
    AddTimeEvents(node);
    return NOERROR;
}

ECode TtmlTrack::OnRootNodeParsed(
    /* [in] */ ITtmlNode* node)
{
    mRootNode = (TtmlNode*)node;
    AutoPtr<ISubtitleTrackCue> cue;
    GetNextResult((ISubtitleTrackCue**)&cue);
    while (cue != NULL) {
        Boolean flag = FALSE;
        AddCue(cue, &flag);
    }
    mRootNode = NULL;
    mTtmlNodes.Clear();
    mTimeEvents.Clear();
    return NOERROR;
}

ECode TtmlTrack::UpdateView(
    /* [in] */ IVector* activeCues)
{
    if (!mVisible) {
        // don't keep the state if we are not visible
        return NOERROR;
    }

    if (DEBUG && mTimeProvider != NULL) {
        Int64 vol;
        ECode ec = mTimeProvider->GetCurrentTimeUs(FALSE, TRUE, &vol);
        if (SUCCEEDED(ec)) {
            Slogger::D(TAG, "at %lld ms the active cues are:", vol/1000);
        }

        if (ec == (ECode)E_ILLEGAL_STATE_EXCEPTION) {
            Slogger::D(TAG, "at (illegal state) the active cues are:");
        }
        // try {
        // } catch (IllegalStateException e) {
            // Slogger::D(TAG, "at (illegal state) the active cues are:");
        // }
    }

    return mRenderingWidget->SetActiveCues(activeCues);
}

ECode TtmlTrack::GetNextResult(
    /* [out] */ ISubtitleTrackCue** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<TtmlCue> cue;
    while (mTimeEvents.GetSize() >= 2) {
        Int64 start;
        // mTimeEvents.PopFront();
        Int64 end;
        // mTimeEvents.First();
        AutoPtr<List<AutoPtr<TtmlNode> > > activeCues
            = GetActiveNodes(start, end);
        if (!activeCues->IsEmpty()) {
            String ret;
            TtmlUtils::ExtractText(mRootNode, start, end, &ret);
            String str;
            TtmlUtils::ApplySpacePolicy(ret, FALSE, &str);
            String str_;
            TtmlUtils::ExtractTtmlFragment(mRootNode, start, end, &str_);
            cue = NULL;
            cue = new TtmlCue(start, end, str, str_);
            *result = cue;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
    }
    *result = NULL;
    return NOERROR;
}

void TtmlTrack::AddTimeEvents(ITtmlNode* node)
{
    mTimeEvents.Insert(((TtmlNode*)node)->mStartTimeMs);
    mTimeEvents.Insert(((TtmlNode*)node)->mEndTimeMs);
    Int32 size;
    ((TtmlNode*)node)->mChildren->GetSize(&size);
    AutoPtr<IInterface> obj;
    AutoPtr<ITtmlNode> tn;
    for (Int32 i = 0; i < size; ++i) {
        obj = NULL;
        ((TtmlNode*)node)->mChildren->Get(i, (IInterface**)&obj);
        tn = NULL;
        tn = ITtmlNode::Probe(obj);
        AddTimeEvents(tn.Get());
    }
}

AutoPtr<List<AutoPtr<TtmlNode> > > TtmlTrack::GetActiveNodes(
    /* [in] */ Int64 startTimeUs,
    /* [in] */ Int64 endTimeUs)
{
    Int32 size = mTtmlNodes.GetSize();
    AutoPtr<List<AutoPtr<TtmlNode> > > activeNodes = new List<AutoPtr<TtmlNode> >();
    List<AutoPtr<TtmlNode> >::Iterator it = mTtmlNodes.Begin();
    Boolean flag = FALSE;
    for (; it != mTtmlNodes.End(); ++it){
        AutoPtr<TtmlNode> node = *it;
        node->IsActive(startTimeUs, endTimeUs, &flag);
        if (flag) {
            activeNodes->PushBack(node);
        }
    }
    return activeNodes;

}

//================================================================================
//                      TtmlRenderingWidget
//================================================================================
TtmlRenderingWidget::TtmlRenderingWidget(
    /* [in] */ IContext* context)
{
    TtmlRenderingWidget(context, NULL);
}

TtmlRenderingWidget::TtmlRenderingWidget(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    TtmlRenderingWidget(context, attrs, 0);
}

TtmlRenderingWidget::TtmlRenderingWidget(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    TtmlRenderingWidget(context, attrs, defStyleAttr, 0);
}

TtmlRenderingWidget::TtmlRenderingWidget(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    LinearLayout::constructor(context, attrs, defStyleAttr, defStyleRes);
    // Cannot render text over video when layer type is hardware.
    SetLayerType(IView::LAYER_TYPE_SOFTWARE, NULL);

    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::CAPTIONING_SERVICE, (IInterface**)&obj);
    AutoPtr<CaptioningManager> captionManager = (CaptioningManager*)ICaptioningManager::Probe(obj);
    AutoPtr<TextView> tv = new TextView();
    tv->constructor(context);
    mTextView = tv;
    AutoPtr<ICaptioningManagerCaptionStyle> style;
    captionManager->GetUserStyle((ICaptioningManagerCaptionStyle**)&style);
    Int32 color = ((CaptioningManager::CaptionStyle*)style.Get())->mForegroundColor;
    mTextView->SetTextColor(color);
    AddView(mTextView, LayoutParams::MATCH_PARENT, LayoutParams::MATCH_PARENT);
    mTextView->SetGravity(IGravity::BOTTOM | IGravity::CENTER_HORIZONTAL);
}

TtmlRenderingWidget::~TtmlRenderingWidget()
{}

CAR_INTERFACE_IMPL_7(TtmlRenderingWidget, Object, ISubtitleTrackRenderingWidget, ITtmlRenderingWidget,
    ILinearLayout, IViewGroup, IViewParent, IViewManager, IView/*,*/
/*    IDrawableCallback, IKeyEventCallback, IAccessibilityEventSource*/)

ECode TtmlRenderingWidget::SetOnChangedListener(
    /* [in] */ ISubtitleTrackRenderingWidgetOnChangedListener* listener)
{
    mListener = listener;
    return NOERROR;
}

ECode TtmlRenderingWidget::SetSize(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    Int32 widthSpec = MeasureSpec::MakeMeasureSpec(width, MeasureSpec::EXACTLY);
    Int32 heightSpec = MeasureSpec::MakeMeasureSpec(height, MeasureSpec::EXACTLY);

    Measure(widthSpec, heightSpec);
    return Layout(0, 0, width, height);
}

ECode TtmlRenderingWidget::SetVisible(
    /* [in] */ Boolean visible)
{
    if (visible) {
        SetVisibility(IView::VISIBLE);
    } else {
        SetVisibility(IView::GONE);
    }
    return NOERROR;
}

ECode TtmlRenderingWidget::OnAttachedToWindow()
{
    return ISubtitleTrackRenderingWidget::Probe(this)->OnAttachedToWindow();
}

ECode TtmlRenderingWidget::OnDetachedFromWindow()
{
    return ISubtitleTrackRenderingWidget::Probe(this)->OnDetachedFromWindow();
}

ECode TtmlRenderingWidget::SetActiveCues(
    /* [in] */ IVector* activeCues)
{
    Int32 count;
    activeCues->GetSize(&count);
    String subtitleText(String(""));
    AutoPtr<IInterface> obj;
    for (Int32 i = 0; i < count; i++) {
        activeCues->Get(i, (IInterface**)&obj);
        AutoPtr<TtmlCue> cue =  (TtmlCue*)ISubtitleTrackCue::Probe(obj);
        subtitleText += cue->mText + String("\n");
    }
    mTextView->SetText(StringUtils::ParseCharSequence(subtitleText).Get());

    if (mListener != NULL) {
        mListener->OnChanged(this);
    }
    return NOERROR;
}

ECode TtmlRenderingWidget::Draw(
    /* [in] */ ICanvas* c)
{
    return NOERROR;
}

} // namespace Elastos
} // namespace Droid
} // namespace Media