
#include <Elastos.CoreLibrary.External.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/drawable/StateListDrawable.h"
#include "elastos/droid/graphics/drawable/CStateListDrawable.h"
#include "elastos/droid/utility/StateSet.h"
#include "elastos/droid/R.h"
#include <elastos/utility/Arrays.h>

using Elastos::Droid::Utility::StateSet;
using Elastos::Droid::R;
using Elastos::Utility::Arrays;

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

const Boolean StateListDrawable::DEFAULT_DITHER;
Boolean StateListDrawable::DEBUG = FALSE;
String StateListDrawable::TAG("StateListDrawable");

CAR_INTERFACE_IMPL(StateListDrawable, DrawableContainer, IStateListDrawable)

StateListDrawable::StateListDrawable()
    : mMutated(FALSE)
{
}

ECode StateListDrawable::constructor()
{
    return constructor(NULL, NULL);
}

ECode StateListDrawable::constructor(
    /* [in] */ IDrawableConstantState* state,
    /* [in] */ IResources* res)
{
    AutoPtr<StateListState> newState = new StateListState((StateListState*)state, this, res);
    SetConstantState(newState);

    AutoPtr<ArrayOf<Int32> > states;
    GetState((ArrayOf<Int32>**)&states);
    OnStateChange(states);
    return NOERROR;
}

ECode StateListDrawable::constructor(
    /* [in] */ IDrawableConstantState* state)
{
    if (state != NULL) {
        SetConstantState((StateListState*)state);
    }
    return NOERROR;
}

ECode StateListDrawable::AddState(
    /* [in] */ ArrayOf<Int32>* stateSet,
    /* [in] */ IDrawable* drawable)
{
    if (drawable != NULL) {
        mStateListState->AddStateSet(stateSet, drawable);
        // in case the new state matches our current state...
        AutoPtr<ArrayOf<Int32> > states;
        GetState((ArrayOf<Int32>**)&states);
        OnStateChange(states);
    }
    return NOERROR;
}

ECode StateListDrawable::IsStateful(
    /* [out] */ Boolean* isStateful)
{
    VALIDATE_NOT_NULL(isStateful);
    *isStateful = TRUE;
    return NOERROR;
}

Boolean StateListDrawable::OnStateChange(
    /* [in] */ ArrayOf<Int32>* stateSet)
{
    Int32 idx = mStateListState->IndexOfStateSet(stateSet);
    // if (DEBUG) android.util.Log.i(TAG, "onStateChange " + this + " states "
    //             + Arrays.toString(stateSet) + " found " + idx);
    if (idx < 0) {
        idx = mStateListState->IndexOfStateSet(StateSet::WILD_CARD);
    }

    Boolean res = FALSE;
    if (SelectDrawable(idx, &res), res) {
        return TRUE;
    }
    return DrawableContainer::OnStateChange(stateSet);
}

ECode StateListDrawable::Inflate(
    /* [in] */ IResources* r,
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ IResourcesTheme* theme)
{
    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::StateListDrawable),
            ArraySize(R::styleable::StateListDrawable));
    AutoPtr<ITypedArray> a;
    FAIL_RETURN(ObtainAttributes(r, theme, attrs, attrIds, (ITypedArray**)&a));

    DrawableContainer::InflateWithAttributes(r, parser, a,
            R::styleable::StateListDrawable_visible);

    Boolean value;
    a->GetBoolean(R::styleable::StateListDrawable_variablePadding, FALSE, &value);
    mStateListState->SetVariablePadding(value);
    a->GetBoolean(R::styleable::StateListDrawable_constantSize, FALSE, &value);
    mStateListState->SetConstantSize(value);
    Int32 iVal = 0;
    a->GetInt32(R::styleable::StateListDrawable_enterFadeDuration, 0, &iVal);
    mStateListState->SetEnterFadeDuration(iVal);
    a->GetInt32(R::styleable::StateListDrawable_exitFadeDuration, 0, &iVal);
    mStateListState->SetExitFadeDuration(iVal);

    a->GetBoolean(R::styleable::StateListDrawable_dither, DEFAULT_DITHER, &value);
    SetDither(value);
    a->GetBoolean(R::styleable::StateListDrawable_autoMirrored, FALSE, &value);
    SetAutoMirrored(value);

    a->Recycle();

    Int32 innerDepth;
    parser->GetDepth(&innerDepth);
    innerDepth += 1;
    Int32 type, depth;
    while((parser->Next(&type), type != IXmlPullParser::END_DOCUMENT) &&
            ((parser->GetDepth(&depth), depth >= innerDepth) || type != IXmlPullParser::END_TAG)) {
        if (type != IXmlPullParser::START_TAG) {
            continue;
        }

        String name;
        if (depth > innerDepth || (parser->GetName(&name), !name.Equals("item"))) {
            continue;
        }

        Int32 drawableRes = 0;

        Int32 i;
        Int32 j = 0;
        Int32 numAttrs;
        attrs->GetAttributeCount(&numAttrs);
        AutoPtr< ArrayOf<Int32> > states = ArrayOf<Int32>::Alloc(numAttrs);
        for (i = 0; i < numAttrs; i++) {
            Int32 stateResId;
            attrs->GetAttributeNameResource(i, &stateResId);
            if (stateResId == 0) break;
            if (stateResId == R::attr::drawable) {
                attrs->GetAttributeResourceValue(i, 0, &drawableRes);
            }
            else {
                Boolean value;
                attrs->GetAttributeBooleanValue(i, FALSE, &value);
                (*states)[j++] = value ? stateResId : -stateResId;
            }
        }
        states = StateSet::TrimStateSet(states, j);

        AutoPtr<IDrawable> dr;
        if (drawableRes != 0) {
            r->GetDrawable(drawableRes, theme, (IDrawable**)&dr);
        }
        else {
            while (parser->Next(&type), type == IXmlPullParser::TEXT) {
            }
            if (type != IXmlPullParser::START_TAG) {
//                throw new XmlPullParserException(
//                        parser.getPositionDescription()
//                                + ": <item> tag requires a 'drawable' attribute or "
//                                + "child tag defining a drawable");
                return E_XML_PULL_PARSER_EXCEPTION;
            }
            Drawable::CreateFromXmlInner(r, parser, attrs, theme, (IDrawable**)&dr);
        }

        mStateListState->AddStateSet(states, dr);
    }

    AutoPtr<ArrayOf<Int32> > states;
    GetState((ArrayOf<Int32>**)&states);
    OnStateChange(states);
    return NOERROR;
}

AutoPtr<StateListDrawable::StateListState> StateListDrawable::GetStateListState()
{
    return mStateListState;
}

ECode StateListDrawable::GetStateCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = mStateListState->GetChildCount();
    return NOERROR;
}

ECode StateListDrawable::GetStateSet(
    /* [in] */ Int32 index,
    /* [out, callee] */ ArrayOf<Int32>** stateSet)
{
    VALIDATE_NOT_NULL(stateSet);
    *stateSet = (*mStateListState->mStateSets)[index];
    REFCOUNT_ADD(*stateSet);
    return NOERROR;
}

ECode StateListDrawable::GetStateDrawable(
    /* [in] */ Int32 index,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    AutoPtr<IDrawable> temp = mStateListState->GetChild(index);
    *drawable = temp;
    REFCOUNT_ADD(*drawable);
    return NOERROR;
}

ECode StateListDrawable::GetStateDrawableIndex(
    /* [in] */ ArrayOf<Int32>* stateSet,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    *index = mStateListState->IndexOfStateSet(stateSet);
    return NOERROR;
}

ECode StateListDrawable::Mutate()
{
    if (!mMutated ) {
        AutoPtr<ArrayOf< AutoPtr<ArrayOf<Int32> > > > sets = mStateListState->mStateSets;
        Int32 count = sets->GetLength();
        mStateListState->mStateSets = ArrayOf< AutoPtr<ArrayOf<Int32> > >::Alloc(count);

        for (Int32 i = 0; i < count; i++) {
            AutoPtr<ArrayOf<Int32> > set = (*sets)[i];
            if (set != NULL) {
                mStateListState->mStateSets->Set(i, set);
            }
        }
        mMutated = TRUE;
    }

    return NOERROR;
}

ECode StateListDrawable::SetLayoutDirection(
    /* [in] */ Int32 layoutDirection)
{
    DrawableContainer::SetLayoutDirection(layoutDirection);
    // Let the container handle setting its own layout direction. Otherwise,
    // we're accessing potentially unused states.
    mStateListState->SetLayoutDirection(layoutDirection);
    return NOERROR;
}

void StateListDrawable::SetConstantState(
    /* [in] */ StateListState* state)
{
    DrawableContainer::SetConstantState(state);

    mStateListState = state;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

StateListDrawable::StateListState::StateListState(
    /* [in] */ StateListState* orig,
    /* [in] */ StateListDrawable* owner,
    /* [in] */ IResources* res)
    : DrawableContainerState(orig, owner, res)
{
    if (orig != NULL) {
        mStateSets = NULL;
        Arrays::CopyOf(orig->mStateSets, orig->mStateSets->GetLength(), (ArrayOf< AutoPtr<ArrayOf<Int32> > >**)&mStateSets);
    }
    else {
        mStateSets = ArrayOf< AutoPtr<ArrayOf<Int32> > >::Alloc(GetCapacity());
    }
}

StateListDrawable::StateListState::~StateListState()
{
}

Int32 StateListDrawable::StateListState::AddStateSet(
    /* [in] */ ArrayOf<Int32>* stateSet,
    /* [in] */ IDrawable* drawable)
{
    Int32 pos = AddChild(drawable);
    mStateSets->Set(pos, stateSet);
    return pos;
}

Int32 StateListDrawable::StateListState::IndexOfStateSet(
    /* [in] */ ArrayOf<Int32>* stateSet)
{
    Int32 N = GetChildCount();
    for (Int32 i = 0; i < N; i++) {
        if (StateSet::StateSetMatches((*mStateSets)[i].Get(), stateSet)) {
            return i;
        }
    }
    return -1;
}

// IDrawableConstantState
//
ECode StateListDrawable::StateListState::NewDrawable(
    /* [out] */ IDrawable** drawable)
{
    return CStateListDrawable::New(this, NULL, drawable);
}

ECode StateListDrawable::StateListState::NewDrawable(
    /* [in] */ IResources* res,
    /* [out] */ IDrawable** drawable)
{
    return CStateListDrawable::New(this, res, drawable);
}

void StateListDrawable::StateListState::GrowArray(
    /* [in] */ Int32 oldSize,
    /* [in] */ Int32 newSize)
{
    DrawableContainerState::GrowArray(oldSize, newSize);
    AutoPtr<ArrayOf< AutoPtr<ArrayOf<Int32> > > > newStateSets
            = ArrayOf< AutoPtr<ArrayOf<Int32> > >::Alloc(newSize);
    for (Int32 i = 0; i < oldSize; i++) {
        newStateSets->Set(i, (*mStateSets)[i]);
    }

    mStateSets = newStateSets;
}

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
