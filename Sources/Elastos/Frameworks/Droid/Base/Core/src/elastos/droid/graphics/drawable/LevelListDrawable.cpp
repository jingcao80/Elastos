
#include <Elastos.CoreLibrary.External.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/drawable/LevelListDrawable.h"
#include "elastos/droid/graphics/drawable/CLevelListDrawable.h"
#include "elastos/droid/R.h"

using Elastos::Droid::R;

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

LevelListDrawable::LevelListState::LevelListState(
    /* [in] */ LevelListState* orig,
    /* [in] */ LevelListDrawable* owner,
    /* [in] */ IResources* res)
    : DrawableContainerState(orig, owner, res)
{
    if (orig != NULL) {
        mLows = orig->mLows;
        mHighs = orig->mHighs;
    }
    else {
        mLows = ArrayOf<Int32>::Alloc(GetCapacity());
        mHighs = ArrayOf<Int32>::Alloc(GetCapacity());
    }
}

void LevelListDrawable::LevelListState::AddLevel(
    /* [in] */ Int32 low,
    /* [in] */ Int32 high,
    /* [in] */ IDrawable* drawable)
{
    Int32 pos = AddChild(drawable);
    (*mLows)[pos] = low;
    (*mHighs)[pos] = high;
}

Int32 LevelListDrawable::LevelListState::IndexOfLevel(
    /* [in] */ Int32 level)
{
    const ArrayOf<Int32>* lows = mLows;
    const ArrayOf<Int32>* highs = mHighs;
    const Int32 N = GetChildCount();
    for (Int32 i = 0; i < N; i++) {
        if (level >= (*lows)[i] && level <= (*highs)[i]) {
            return i;
        }
    }
    return -1;
}

ECode LevelListDrawable::LevelListState::NewDrawable(
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    return CLevelListDrawable::New(this, NULL, (ILevelListDrawable**)drawable);
}

ECode LevelListDrawable::LevelListState::NewDrawable(
    /* [in] */ IResources* res,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    return CLevelListDrawable::New(this, res, (ILevelListDrawable**)drawable);
}

void LevelListDrawable::LevelListState::GrowArray(
    /* [in] */ Int32 oldSize,
    /* [in] */ Int32 newSize)
{
    DrawableContainerState::GrowArray(oldSize, newSize);
    AutoPtr<ArrayOf<Int32> > newInts = ArrayOf<Int32>::Alloc(newSize);
    newInts->Copy(mLows, 0, oldSize);
    mLows = newInts;
    newInts = ArrayOf<Int32>::Alloc(newSize);
    newInts->Copy(mHighs, 0, oldSize);
    mHighs = newInts;
}

CAR_INTERFACE_IMPL(LevelListDrawable, DrawableContainer, ILevelListDrawable);
LevelListDrawable::LevelListDrawable()
    : mMutated(FALSE)
{
}

LevelListDrawable::LevelListDrawable(
    /* [in] */ LevelListState* state,
    /* [in] */ IResources* res)
    : mMutated(FALSE)
{
    mLevelListState = new LevelListState(state, this, res);
    SetConstantState(mLevelListState);
    Int32 level = 0;
    GetLevel(&level);
    OnLevelChange(level);
}

ECode LevelListDrawable::AddLevel(
    /* [in] */ Int32 low,
    /* [in] */ Int32 high,
    /* [in] */ IDrawable* drawable)
{
    if (drawable != NULL) {
        mLevelListState->AddLevel(low, high, drawable);
        // in case the new state matches our current state...
        Int32 level = 0;
        GetLevel(&level);
        OnLevelChange(level);
    }
    return NOERROR;
}

//@Override
Boolean LevelListDrawable::OnLevelChange(
    /* [in] */ Int32 level)
{
    Int32 idx = mLevelListState->IndexOfLevel(level);
    Boolean res = FALSE;
    if (SelectDrawable(idx, &res), res) {
        return TRUE;
    }
    return DrawableContainer::OnLevelChange(level);
}

ECode LevelListDrawable::Inflate(
    /* [in] */ IResources* r,
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ IResourcesTheme* theme)
{
    FAIL_RETURN(DrawableContainer::Inflate(r, parser, attrs, theme));

    Int32 type;

    Int32 low = 0;

    Int32 innerDepth, depth;
    parser->GetDepth(&depth);
    innerDepth = depth + 1;
    while ((parser->Next(&type), type != IXmlPullParser::END_DOCUMENT)
            && ((parser->GetDepth(&depth), depth >= innerDepth)
            || type != IXmlPullParser::END_TAG)) {
        if (type != IXmlPullParser::START_TAG) {
            continue;
        }

        String name;
        if (depth > innerDepth || (parser->GetName(&name), !name.Equals("item"))) {
            continue;
        }

        Int32 size = ArraySize(R::styleable::LevelListDrawableItem);
        AutoPtr<ArrayOf<Int32> > layout = ArrayOf<Int32>::Alloc(size);
        layout->Copy(R::styleable::LevelListDrawableItem, size);

        AutoPtr<ITypedArray> a;
        FAIL_RETURN(ObtainAttributes(r, theme, attrs, layout, (ITypedArray**)&a));

        a->GetInt32(
                R::styleable::LevelListDrawableItem_minLevel, 0, &low);
        Int32 high;
        a->GetInt32(
                R::styleable::LevelListDrawableItem_maxLevel, 0, &high);
        Int32 drawableRes;
        a->GetResourceId(
                R::styleable::LevelListDrawableItem_drawable, 0, &drawableRes);

        a->Recycle();

        if (high < 0) {
//            throw new XmlPullParserException(parser.getPositionDescription()
//                    + ": <item> tag requires a 'maxLevel' attribute");
            return E_XML_PULL_PARSER_EXCEPTION;
        }

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

        mLevelListState->AddLevel(low, high, dr);
    }

    Int32 level = 0;
    OnLevelChange((GetLevel(&level), level));
    return NOERROR;
}

ECode LevelListDrawable::Mutate(
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    AutoPtr<IDrawable> tmp;
    if (!mMutated &&
            (DrawableContainer::Mutate((IDrawable**)&tmp), tmp.Get()) == (IDrawable*)this->Probe(EIID_IDrawable)) {
        mLevelListState->mLows = mLevelListState->mLows->Clone();
        mLevelListState->mHighs = mLevelListState->mHighs->Clone();
        mMutated = TRUE;
    }
    *drawable = (IDrawable*)this->Probe(EIID_IDrawable);
    REFCOUNT_ADD(*drawable);
    return NOERROR;
}

ECode LevelListDrawable::constructor()
{
    return constructor(NULL, NULL);
}

ECode LevelListDrawable::constructor(
    /* [in] */ LevelListState* state,
    /* [in] */ IResources* res)
{
    mLevelListState = new LevelListState(state, this, res);
    SetConstantState(mLevelListState);
    Int32 level = 0;
    GetLevel(&level);
    OnLevelChange(level);
    return NOERROR;
}

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
