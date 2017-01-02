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

#ifndef _ELASTOS_DROID_MEDIA_TTMLRENDERER_H__
#define _ELASTOS_DROID_MEDIA_TTMLRENDERER_H__

#include "elastos/droid/media/SubtitleController.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/media/SubtitleTrack.h"
#include "elastos/droid/widget/LinearLayout.h"
#include "elastos/droid/widget/TextView.h"
#include "Elastos.Droid.Media.h"
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/Set.h>
#include <elastos/core/Object.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Media::ISubtitleTrackRenderingWidget;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Widget::LinearLayout;
using Elastos::Droid::Widget::TextView;
using Org::Xmlpull::V1::IXmlPullParser;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::Set;
using Elastos::Utility::IList;
using Elastos::Utility::Regex::IMatchResult;
using Elastos::Utility::Regex::IPattern;
using Elastos::Core::StringBuilder;

namespace Elastos{
namespace Droid {
namespace Media {

class TtmlRenderingWidget;
class TtmlRenderer
    : public ISubtitleControllerRenderer
    , public ITtmlRenderer
    , public Object
{
public:
    TtmlRenderer();

    virtual ~TtmlRenderer();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI Supports(
        /* [in] */ IMediaFormat* format,
        /* [out] */ Boolean* result);

    CARAPI CreateTrack(
        /* [in] */ IMediaFormat* format,
        /* [out] */ ISubtitleTrack** result);

private:
    AutoPtr<IContext> mContext;
    static const String MEDIA_MIMETYPE_TEXT_TTML;
    AutoPtr<TtmlRenderingWidget> mRenderingWidget;
};

class TtmlNode;

class TtmlUtils
{
public:
    static CARAPI ParseTimeExpression(
        /* [in] */ const String& time,
        /* [in] */ Int32 frameRate,
        /* [in] */ Int32 subframeRate,
        /* [in] */ Int32 tickRate,
        /* [out] */ Int64* result);

    static CARAPI ApplyDefaultSpacePolicy(
        /* [in] */ const String& in,
        /* [out] */ String* result);

    static CARAPI ApplySpacePolicy(
        /* [in] */ const String& in,
        /* [in] */ Boolean treatLfAsSpace,
        /* [out] */ String* result);

    static CARAPI ExtractText(
        /* [in] */ TtmlNode* root,
        /* [in] */ Int64 startUs,
        /* [in] */ Int64 endUs,
        /* [out] */ String* result);

    static CARAPI ExtractTtmlFragment(
        /* [in] */ TtmlNode* root,
        /* [in] */ Int64 startUs,
        /* [in] */ Int64 endUs,
        /* [out] */ String* result);

private:
    static CARAPI_(void) ExtractText(
        /* [in] */ TtmlNode* node,
        /* [in] */ Int64 startUs,
        /* [in] */ Int64 endUs,
        /* [in] */ StringBuilder* out,
        /* [in] */ Boolean inPTag);

    static CARAPI_(void) ExtractTtmlFragment(
        /* [in] */ TtmlNode* node,
        /* [in] */ Int64 startUs,
        /* [in] */ Int64 endUs,
        /* [in] */ StringBuilder* out);

private:
    static const AutoPtr<IPattern> CLOCK_TIME;
    static const AutoPtr<IPattern> OFFSET_TIME;
};

class TtmlCue
    : public SubtitleTrack::Cue
{
public:
    TtmlCue(
        /* [in] */ Int64 startTimeMs,
        /* [in] */ Int64 endTimeMs,
        /* [in] */ const String& text,
        /* [in] */ const String& ttmlFragment);

    virtual ~TtmlCue();

    CAR_INTERFACE_DECL()

public:
    String mText;
    String mTtmlFragment;
};

class TtmlNode
    : public Object
    , public ITtmlNode
{
public:
    TtmlNode(
        /* [in] */ const String& name,
        /* [in] */ const String& attributes,
        /* [in] */ const String& text,
        /* [in] */ Int64 startTimeMs,
        /* [in] */ Int64 endTimeMs,
        /* [in] */ ITtmlNode* parent,
        /* [in] */ Int64 runId);

    virtual ~TtmlNode();

    CAR_INTERFACE_DECL()

    CARAPI IsActive(
        /* [in] */ Int64 startTimeMs,
        /* [in] */ Int64 endTimeMs,
        /* [out] */ Boolean* result);

public:
    String mName;
    String mAttributes;
    ITtmlNode* mParent;
    String mText;
    AutoPtr<IList> mChildren;
    Int64 mRunId;
    Int64 mStartTimeMs;
    Int64 mEndTimeMs;
};

class TtmlParser
    : public Object
    , public ITtmlParser
{
public:
    TtmlParser(
        /* [in] */ ITtmlNodeListener* listener);

    ~TtmlParser();

    CAR_INTERFACE_DECL()

    CARAPI Parse(
        /* [in] */ const String& ttmlText,
        /* [in] */ Int64 runId);

private:
    CARAPI_(void) LoadParser(
        /* [in] */ const String& ttmlFragment);

    CARAPI_(void) ExtractAttribute(
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ Int32 i,
        /* [in] */ StringBuilder* outSdr);

    CARAPI_(void) ParseTtml();

    CARAPI ParseNode(
        /* [in] */ TtmlNode* parent,
        /* [out] */ TtmlNode** result);

    CARAPI IsEndOfDoc(
        /* [out] */ Boolean* result);

    static CARAPI IsSupportedTag(
        /* [in] */ const String& tag,
        /* [out] */ Boolean* result);
public:
    static const String TAG;

private:
    // TODO: read and apply the following attributes if specified.
    static const Int32 DEFAULT_FRAMERATE;
    static const Int32 DEFAULT_SUBFRAMERATE;
    static const Int32 DEFAULT_TICKRATE;

    AutoPtr<IXmlPullParser> mParser;
    AutoPtr<ITtmlNodeListener> mListener;
    Int64 mCurrentRunId;
};

class TtmlTrack
    : public SubtitleTrack
    , public ITtmlNodeListener
{
public:
    TtmlTrack(
        /* [in] */ ITtmlRenderingWidget* renderingWidget,
        /* [in] */ IMediaFormat* format);

    virtual ~TtmlTrack();

    CAR_INTERFACE_DECL()

    CARAPI GetRenderingWidget(
        /* [out] */ ISubtitleTrackRenderingWidget** result);

    CARAPI OnData(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Boolean eos,
        /* [in] */ Int64 runID);

    CARAPI OnTtmlNodeParsed(
        /* [in] */ ITtmlNode* node);

    CARAPI OnRootNodeParsed(
        /* [in] */ ITtmlNode* node);

    CARAPI UpdateView(
        /* [in] */ IVector* activeCues);

    CARAPI GetNextResult(
        /* [out] */ ISubtitleTrackCue** result);

private:
    CARAPI_(void) AddTimeEvents(ITtmlNode* node);

    AutoPtr<List<AutoPtr<TtmlNode> > > GetActiveNodes(
        /* [in] */ Int64 startTimeUs,
        /* [in] */ Int64 endTimeUs);
private:
    static const String TAG;

    AutoPtr<TtmlParser> mParser;/* = new TtmlParser(this);*/
    AutoPtr<TtmlRenderingWidget> mRenderingWidget;
    String mParsingData;
    Int64 mCurrentRunID;

    List<AutoPtr<TtmlNode> >  mTtmlNodes;
    Set<Int64> mTimeEvents;
    AutoPtr<TtmlNode> mRootNode;
};

class TtmlRenderingWidget
    : public LinearLayout
    , public ISubtitleTrackRenderingWidget
    , public ITtmlRenderingWidget
{
public:
    TtmlRenderingWidget(
        /* [in] */ IContext* context);

    TtmlRenderingWidget(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    TtmlRenderingWidget(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    TtmlRenderingWidget(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    ~TtmlRenderingWidget();

    CAR_INTERFACE_DECL()

    CARAPI SetOnChangedListener(
        /* [in] */ ISubtitleTrackRenderingWidgetOnChangedListener* listener);

    CARAPI SetSize(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI SetVisible(
        /* [in] */ Boolean visible);

    CARAPI OnAttachedToWindow();

    CARAPI OnDetachedFromWindow();

    CARAPI SetActiveCues(
        /* [in] */ IVector* activeCues);

    CARAPI Draw(
        /* [in] */ ICanvas* c);

private:
    /** Callback for rendering changes. */
    AutoPtr<ISubtitleTrackRenderingWidgetOnChangedListener> mListener;
    AutoPtr<TextView> mTextView;
};

} // namespace Elastos
} // namespace Droid
} // namespace Media

#endif // _ELASTOS_DROID_MEDIA_TTMLRENDERER_H__