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

#include <Elastos.CoreLibrary.External.h>
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/content/res/CColorStateList.h"
#include "elastos/droid/graphics/Color.h"
#include "elastos/droid/utility/StateSet.h"
#include "elastos/droid/utility/MathUtils.h"
#include "elastos/droid/internal/utility/GrowingArrayUtils.h"
#include "elastos/droid/utility/Xml.h"
#include "elastos/droid/R.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Graphics::Color;
using Elastos::Droid::Utility::StateSet;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::Utility::MathUtils;
using Elastos::Droid::Internal::Utility::GrowingArrayUtils;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

static AutoPtr< ArrayOf<Int32Array > > InitEMPTY()
{
    AutoPtr< ArrayOf<Int32> > state = ArrayOf<Int32>::Alloc(1);
    (*state)[0] = 0;
    AutoPtr< ArrayOf<Int32Array > > states = ArrayOf<Int32Array >::Alloc(1);
    states->Set(0, state);
    return states;
}

const String CColorStateList::TAG("CColorStateList");
AutoPtr< ArrayOf<Int32Array > > CColorStateList::EMPTY = InitEMPTY();
HashMap<Int32, AutoPtr<IWeakReference> > CColorStateList::sCache;
Object CColorStateList::sCacheLock;

CAR_INTERFACE_IMPL(CColorStateList, Object, IColorStateList)

CAR_OBJECT_IMPL(CColorStateList)

CColorStateList::CColorStateList()
    : mDefaultColor(0xffff0000)
{}

CColorStateList::~CColorStateList()
{
}

ECode CColorStateList::ValueOf(
    /* [in] */ Int32 color,
    /* [out] */ IColorStateList** _csl)
{
    VALIDATE_NOT_NULL(_csl);
    *_csl = NULL;

    //TODO: should we collect these eventually?
    AutoLock lock(sCacheLock);

    AutoPtr<IColorStateList> csl;
    HashMap<Int32, AutoPtr<IWeakReference> >::Iterator it = sCache.Find(color);
    if (it != sCache.End()) {
        IWeakReference* wr = it->mSecond;
        wr->Resolve(EIID_IColorStateList, (IInterface**)&csl);
    }

    if (csl == NULL) {
        AutoPtr< ArrayOf<Int32> > colors = ArrayOf<Int32>::Alloc(1);
        (*colors)[0] = color;
        AutoPtr< ArrayOf<HANDLE> > emptyArray = ArrayOf<HANDLE>::Alloc(1);
        emptyArray->Set(0, (HANDLE)(*EMPTY)[0].Get());
        AutoPtr<CColorStateList> ccsl;
        FAIL_RETURN(CColorStateList::NewByFriend(emptyArray, colors, (CColorStateList**)&ccsl));
        csl = ccsl;
        AutoPtr<IWeakReferenceSource> wrs = IWeakReferenceSource::Probe(csl);
        assert(wrs != NULL);
        AutoPtr<IWeakReference> wr;
        wrs->GetWeakReference((IWeakReference**)&wr);
        sCache[color] = wr;
    }

    *_csl = csl.Get();
    REFCOUNT_ADD(*_csl);
    return NOERROR;
}

ECode CColorStateList::CreateFromXml(
    /* [in] */ IResources* r,
    /* [in] */ IXmlPullParser* parser,
    /* [out] */ IColorStateList** csl)
{
    VALIDATE_NOT_NULL(csl);

    AutoPtr<IAttributeSet> attrs = Xml::AsAttributeSet(parser);

    Int32 type;
    FAIL_RETURN(parser->Next(&type));
    while (type != IXmlPullParser::START_TAG
            && type != IXmlPullParser::END_DOCUMENT) {
        FAIL_RETURN(parser->Next(&type));
    }

    if (type != IXmlPullParser::START_TAG) {
        Slogger::E(TAG, "CColorStateList No start tag found");
        return E_XML_PULL_PARSER_EXCEPTION;
    }

    return CreateFromXmlInner(r, parser, attrs, csl);
}

ECode CColorStateList::CreateFromXmlInner(
    /* [in] */ IResources* r,
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IAttributeSet* attrs,
    /* [out] */ IColorStateList** csl)
{
    VALIDATE_NOT_NULL(csl)
    *csl = NULL;

    String name;
    FAIL_RETURN(parser->GetName(&name));
    if (!name.Equals("selector")) {
        String pos;
        parser->GetPositionDescription(&pos);
        Slogger::E(TAG, "CColorStateList%s : invalid drawable tag %s",
            pos.string(), name.string());
        return E_XML_PULL_PARSER_EXCEPTION;
    }

    AutoPtr<CColorStateList> ccsl;
    FAIL_RETURN(CColorStateList::NewByFriend((CColorStateList**)&ccsl));
    *csl = (IColorStateList*)ccsl.Get();
    REFCOUNT_ADD(*csl);
    return ccsl->Inflate(r, parser, attrs);
}

ECode CColorStateList::WithAlpha(
    /* [in] */ Int32 alpha,
    /* [out] */ IColorStateList** colorState)
{
    VALIDATE_NOT_NULL(colorState);

    Int32 length = mColors->GetLength();
    AutoPtr< ArrayOf<Int32> > colors = ArrayOf<Int32>::Alloc(length);

    for (Int32 i = 0; i < length; i++) {
        (*colors)[i] = ((*mColors)[i] & 0xFFFFFF) | (alpha << 24);
    }

    AutoPtr< ArrayOf<HANDLE> > array = ArrayOf<HANDLE>::Alloc(mStateSpecs->GetLength());
    for (Int32 i = 0; i < mStateSpecs->GetLength(); ++i) {
        array->Set(i, (HANDLE)(*mStateSpecs)[i].Get());
    }
    return CColorStateList::New(array, colors, colorState);
}

ECode CColorStateList::Inflate(
    /* [in] */ IResources* r,
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IAttributeSet* attrs)
{
    Int32 type;

    Int32 innerDepth;
    parser->GetDepth(&innerDepth);
    innerDepth++;

    Int32 depth;

    Int32 listAllocated = 20;
    Int32 listSize = 0;
    AutoPtr< ArrayOf<Int32> > colorList = ArrayOf<Int32>::Alloc(listAllocated);
    AutoPtr< ArrayOf<Int32Array > > stateSpecList = ArrayOf<Int32Array >::Alloc(listAllocated);
    String name;
    while ((parser->Next(&type), type) != IXmlPullParser::END_DOCUMENT
            && ((parser->GetDepth(&depth), depth) >= innerDepth
                    || type != IXmlPullParser::END_TAG)) {
        if (type != IXmlPullParser::START_TAG) {
            continue;
        }

        if (depth > innerDepth || (parser->GetName(&name), !name.Equals("item"))) {
            continue;
        }

        Int32 alphaRes = 0;
        Float alpha = 1.0f;
        Int32 colorRes = 0;
        Int32 color = 0xffff0000;
        Boolean haveColor = FALSE;

        Int32 i;
        Int32 j = 0;
        Int32 numAttrs;
        attrs->GetAttributeCount(&numAttrs);
        AutoPtr< ArrayOf<Int32> > stateSpec = ArrayOf<Int32>::Alloc(numAttrs);
        for (i = 0; i < numAttrs; i++) {
            Int32 stateResId;
            attrs->GetAttributeNameResource(i, &stateResId);
            if (stateResId == 0)
                break;

            if (stateResId == R::attr::alpha) {
                attrs->GetAttributeResourceValue(i, 0, &alphaRes);
                if (alphaRes == 0) {
                    attrs->GetAttributeFloatValue(i, 1.0f, &alpha);
                }
            }
            else if (stateResId == R::attr::color) {
                attrs->GetAttributeResourceValue(i, 0, &colorRes);
                if (colorRes == 0) {
                    attrs->GetAttributeIntValue(i, color, &color);
                    haveColor = TRUE;
                }
            }
            else {
                Boolean value;
                attrs->GetAttributeBooleanValue(i, FALSE, &value);
                (*stateSpec)[j++] = value ? stateResId : -stateResId;
            }
        }
        AutoPtr< ArrayOf<Int32> > temp = StateSet::TrimStateSet(stateSpec, j);
        stateSpec = temp;

        if (colorRes != 0) {
            r->GetColor(colorRes, &color);
        }
        else if (!haveColor) {
            String pos;
            parser->GetPositionDescription(&pos);
            Slogger::E(TAG, "CColorStateList %s: <item> tag requires a 'android:color' attribute.", pos.string());
            return E_XML_PULL_PARSER_EXCEPTION;
        }

       if (alphaRes != 0) {
            r->GetFloat(alphaRes, &alpha);
        }

        // Apply alpha modulation.
        Int32 ca = Color::Alpha(color);
        Int32 alphaMod = MathUtils::Constrain((Int32) (ca * alpha), 0, 255);
        color = (color & 0xFFFFFF) | (alphaMod << 24);

        if (listSize == 0 || stateSpec->GetLength() == 0) {
            mDefaultColor = color;
        }

        if (listSize + 1 >= listAllocated) {
            listAllocated = GrowingArrayUtils::GrowSize(listSize + 1);

            AutoPtr< ArrayOf<Int32> > ncolor = ArrayOf<Int32>::Alloc(listAllocated);
            ncolor->Copy(colorList, 0, listSize);

            AutoPtr< ArrayOf<Int32Array > > nstate = ArrayOf<Int32Array >::Alloc(listAllocated);
            nstate->Copy(stateSpecList, 0, listSize);

            colorList = ncolor;
            stateSpecList = nstate;
        }

        colorList->Set(listSize, color);
        stateSpecList->Set(listSize, stateSpec.Get());
        listSize++;
    }

    mColors = ArrayOf<Int32>::Alloc(listSize);
    mColors->Copy(colorList, 0, listSize);

    mStateSpecs = ArrayOf<Int32Array >::Alloc(listSize);
    mStateSpecs->Copy(stateSpecList, 0, listSize);

    return NOERROR;
}

ECode CColorStateList::IsStateful(
    /* [out] */ Boolean* isStateful)
{
    VALIDATE_NOT_NULL(isStateful);
    *isStateful = mStateSpecs->GetLength() > 1;

    return NOERROR;
}

ECode CColorStateList::IsOpaque(
    /* [out] */ Boolean* isOpaque)
{
    VALIDATE_NOT_NULL(isOpaque);

    Int32 n = mColors->GetLength();
    for (Int32 i = 0; i < n; i++) {
        if (Color::Alpha((*mColors)[i]) != 0xFF) {
            *isOpaque = FALSE;
            return NOERROR;
        }
    }

    *isOpaque = TRUE;
    return NOERROR;
}

ECode CColorStateList::GetColorForState(
    /* [in] */ ArrayOf<Int32>* stateSet,
    /* [in] */ Int32 defaultColor,
    /* [out] */ Int32* color)
{
    VALIDATE_NOT_NULL(color);
    Int32 setLength = mStateSpecs->GetLength();
    for (Int32 i = 0; i < setLength; i++) {
        ArrayOf<Int32>* stateSpec = (*mStateSpecs)[i];
        if (StateSet::StateSetMatches(stateSpec, stateSet)) {
            *color = (*mColors)[i];
            return NOERROR;
        }
    }
    *color = defaultColor;

    return NOERROR;
}

ECode CColorStateList::GetDefaultColor(
    /* [out] */ Int32* color)
{
    VALIDATE_NOT_NULL(color);
    *color = mDefaultColor;

    return NOERROR;
}

AutoPtr< ArrayOf<Int32Array > > CColorStateList::GetStates()
{
    return mStateSpecs;
}

AutoPtr< ArrayOf<Int32> > CColorStateList::GetColors()
{
    return mColors;
}

AutoPtr<IColorStateList> CColorStateList::AddFirstIfMissing(
    /* [in] */ IColorStateList* colorStateList,
    /* [in] */ Int32 state,
    /* [in] */ Int32 color)
{
    CColorStateList* csl = (CColorStateList*)colorStateList;
    AutoPtr< ArrayOf<Int32Array > > inputStates = csl->GetStates();
    for (Int32 i = 0; i < inputStates->GetLength(); i++) {
        ArrayOf<Int32>* inputState = (*inputStates)[i];
        for (Int32 j = 0; j < inputState->GetLength(); j++) {
            if ((*inputState)[i] == state) {
                return colorStateList;
            }
        }
    }

    AutoPtr< ArrayOf<Int32Array > > outputStates = ArrayOf<Int32Array >::Alloc(inputStates->GetLength() + 1);
    outputStates->Copy(1, inputStates, 0, inputStates->GetLength());
    AutoPtr<ArrayOf<Int32> > sa = ArrayOf<Int32>::Alloc(1);
    sa->Set(0, state);
    outputStates->Set(0, sa);

    AutoPtr<ArrayOf<HANDLE> > states = ArrayOf<HANDLE>::Alloc(outputStates->GetLength());
    for (Int32 i = 0; i < outputStates->GetLength(); ++i) {
        states->Set(i, (HANDLE)(*outputStates)[i].Get());
    }

    AutoPtr<ArrayOf<Int32> > inputColors = csl->GetColors();
    AutoPtr<ArrayOf<Int32> > outputColors = ArrayOf<Int32>::Alloc(inputColors->GetLength() + 1);
    outputColors->Copy(1, inputColors, 0, inputColors->GetLength());
    outputColors->Set(0, color);

    AutoPtr<CColorStateList> result;
    CColorStateList::NewByFriend(states, outputColors, (CColorStateList**)&result);
    return (IColorStateList*)result.Get();
}

ECode CColorStateList::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    Int32 N = mStateSpecs->GetLength();
    dest->WriteInt32(N);
    for (Int32 i = 0; i < N; i++) {
        dest->WriteArrayOf((HANDLE)(*mStateSpecs)[i].Get());
    }
    dest->WriteArrayOf((HANDLE)mColors.Get());
    return NOERROR;
}

ECode CColorStateList::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    Int32 N;
    source->ReadInt32(&N);
    AutoPtr< ArrayOf<Int32Array > > stateSpec = ArrayOf<Int32Array >::Alloc(N);
    for (Int32 i = 0; i < N; i++) {
        source->ReadArrayOf((HANDLE*)&(*stateSpec)[i]);
    }
    AutoPtr< ArrayOf<Int32> > colors;
    source->ReadArrayOf((HANDLE*)&colors);

    assert(stateSpec->GetLength() == colors->GetLength());

    mStateSpecs = stateSpec;
    mColors = colors;

    if (mStateSpecs && mStateSpecs->GetLength() > 0) {
        mDefaultColor = (*colors)[0];

        for (Int32 i = 0; i < mStateSpecs->GetLength(); ++i) {
            if ((*mStateSpecs)[i]->GetLength() == 0) {
                mDefaultColor = (*colors)[i];
            }
        }
    }

    return NOERROR;
}

ECode CColorStateList::constructor()
{
    return NOERROR;
}

ECode CColorStateList::constructor(
    /* [in] */ ArrayOf<HANDLE>* states,
    /* [in] */ ArrayOf<Int32>* colors)
{
    mColors = colors;

    if (states) {
        mStateSpecs = ArrayOf<Int32Array >::Alloc(states->GetLength());
        Int32Array array;
        for (Int32 i = 0; i < states->GetLength(); ++i) {
            array = (ArrayOf<Int32>*)(*states)[i];
            mStateSpecs->Set(i, array);
        }
    }
    else {
        mStateSpecs = NULL;
    }

    if (mStateSpecs && mStateSpecs->GetLength() > 0) {
        mDefaultColor = (*colors)[0];

        for (Int32 i = 0; i < mStateSpecs->GetLength(); ++i) {
            if ((*mStateSpecs)[i]->GetLength() == 0) {
                mDefaultColor = (*colors)[i];
            }
        }
    }
    return NOERROR;
}

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos

