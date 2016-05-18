
#include "elastos/droid/systemui/CDessertCaseView.h"
#include "R.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Animation::CAnimatorSet;
using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::Animation::EIID_IAnimatorListener;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Animation::IAnimatorSet;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::CBitmapHelper;
using Elastos::Droid::Graphics::IBitmapHelper;
using Elastos::Droid::Graphics::BitmapConfig_ALPHA_8;
using Elastos::Droid::Graphics::IBitmapFactoryOptions;
using Elastos::Droid::Graphics::CBitmapFactoryOptions;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Graphics::CBitmapFactory;
using Elastos::Droid::Graphics::IColorMatrixColorFilter;
using Elastos::Droid::Graphics::CColorMatrixColorFilter;
using Elastos::Droid::Graphics::IColorFilter;
using Elastos::Droid::Graphics::CColor;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CPoint;
using Elastos::Droid::Graphics::PaintStyle_STROKE;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::Drawable::CBitmapDrawable;
using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::View::Animation::CAnticipateOvershootInterpolator;
using Elastos::Droid::View::Animation::CDecelerateInterpolator;
using Elastos::Droid::View::Animation::IAccelerateInterpolator;
using Elastos::Droid::View::Animation::IAnticipateOvershootInterpolator;
using Elastos::Droid::View::Animation::IDecelerateInterpolator;
using Elastos::Droid::View::Animation::CAccelerateInterpolator;
using Elastos::Droid::View::IViewOverlay;
using Elastos::Droid::View::IViewPropertyAnimator;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::CImageView;
using Elastos::Droid::Widget::CFrameLayoutLayoutParams;
using Elastos::Droid::Widget::IFrameLayoutLayoutParams;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Utility::CHashSet;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {

//===============================================================
// CDessertCaseView::
//===============================================================
String CDessertCaseView::TAG("DessertCaseView");// = DessertCaseView.class.getSimpleName();
Boolean CDessertCaseView::DEBUG = FALSE;
Int32 CDessertCaseView::START_DELAY = 5000;
Int32 CDessertCaseView::DELAY = 2000;
Int32 CDessertCaseView::DURATION = 500;
Int32 CDessertCaseView::TAG_POS = 0x2000001;
Int32 CDessertCaseView::TAG_SPAN = 0x2000002;
Boolean CDessertCaseView::sInit = InitStatic();
AutoPtr<ArrayOf<Int32> > CDessertCaseView::PASTRIES;
AutoPtr<ArrayOf<Int32> > CDessertCaseView::RARE_PASTRIES;
AutoPtr<ArrayOf<Int32> > CDessertCaseView::XRARE_PASTRIES;
AutoPtr<ArrayOf<Int32> > CDessertCaseView::XXRARE_PASTRIES;
Int32 CDessertCaseView::NUM_PASTRIES;

AutoPtr<ArrayOf<Float> > CDessertCaseView::MASK;
AutoPtr<ArrayOf<Float> > CDessertCaseView::ALPHA_MASK;
AutoPtr<ArrayOf<Float> > CDessertCaseView::WHITE_MASK;
Float CDessertCaseView::SCALE = 0.25f; // natural display size will be SCALE*mCellSize
Float CDessertCaseView::PROB_2X = 0.33f;
Float CDessertCaseView::PROB_3X = 0.1f;
Float CDessertCaseView::PROB_4X = 0.01f;

CAR_OBJECT_IMPL(CDessertCaseView)
CAR_INTERFACE_IMPL(CDessertCaseView, FrameLayout, IDessertCaseView)
CDessertCaseView::CDessertCaseView()
{
    CSparseArray::New(NUM_PASTRIES, (ISparseArray**)&mDrawables);

    CHashSet::New((ISet**)&mFreeList);
    CHandler::New((IHandler**)&mHandler);

    mJuggle = new Runnable_1(this);

    mHsv = ArrayOf<Float>::Alloc(3);
    (*mHsv)[0] = 0.f;
    (*mHsv)[1] = 1.f;
    (*mHsv)[2] = .85f;

    CHashSet::New((IHashSet**)&mTmpSet);
}

ECode CDessertCaseView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CDessertCaseView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode CDessertCaseView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    FrameLayout::constructor(context, attrs, defStyle);

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);

    mStarted = FALSE;

    res->GetDimensionPixelSize(R::dimen::dessert_case_cell_size, &mCellSize);
    AutoPtr<IBitmapFactoryOptions> opts;
    CBitmapFactoryOptions::New((IBitmapFactoryOptions**)&opts);
    if (mCellSize < 512) { // assuming 512x512 images
        opts->SetInSampleSize(2);
    }
    opts->SetInMutable(TRUE);
    AutoPtr<IBitmap> loaded;
    AutoPtr<ArrayOf<AutoPtr<ArrayOf<Int32> > > > lists = ArrayOf<AutoPtr<ArrayOf<Int32> > >::Alloc(4);
    lists->Set(0, PASTRIES);
    lists->Set(1, RARE_PASTRIES);
    lists->Set(2, XRARE_PASTRIES);
    lists->Set(3, XXRARE_PASTRIES);
    for (Int32 pos = 0; pos < lists->GetLength(); pos++) {
        AutoPtr<ArrayOf<Int32> > list = (*lists)[pos];
        for (Int32 i = 0; i < list->GetLength(); i++) {
            Int32 resid = (*list)[i];
            opts->SetInBitmap(loaded);
            AutoPtr<IBitmapFactory> bmpf;
            CBitmapFactory::AcquireSingleton((IBitmapFactory**)&bmpf);
            bmpf->DecodeResource(res, resid, opts, (IBitmap**)&loaded);
            AutoPtr<IBitmapDrawable> d;
            CBitmapDrawable::New(res, ConvertToAlphaMask(loaded), (IBitmapDrawable**)&d);
            AutoPtr<IColorMatrixColorFilter> cmcf;
            CColorMatrixColorFilter::New(*ALPHA_MASK, (IColorMatrixColorFilter**)&cmcf);
            IDrawable::Probe(d)->SetColorFilter(IColorFilter::Probe(cmcf));
            IDrawable::Probe(d)->SetBounds(0, 0, mCellSize, mCellSize);
            mDrawables->Append(resid, d);
        }
    }
    loaded = NULL;
    if (DEBUG) SetWillNotDraw(FALSE);
    return NOERROR;
}

Boolean CDessertCaseView::InitStatic()
{
    PASTRIES = ArrayOf<Int32>::Alloc(2);
    (*PASTRIES)[0] = R::drawable::dessert_kitkat;      // used with permission
    (*PASTRIES)[1] = R::drawable::dessert_android;      // thx irina

    RARE_PASTRIES = ArrayOf<Int32>::Alloc(8);
    (*RARE_PASTRIES)[0] = R::drawable::dessert_cupcake;     // 2009
    (*RARE_PASTRIES)[1] = R::drawable::dessert_donut;       // 2009
    (*RARE_PASTRIES)[2] = R::drawable::dessert_eclair;      // 2009
    (*RARE_PASTRIES)[3] = R::drawable::dessert_froyo;       // 2010
    (*RARE_PASTRIES)[4] = R::drawable::dessert_gingerbread; // 2010
    (*RARE_PASTRIES)[5] = R::drawable::dessert_honeycomb;   // 2011
    (*RARE_PASTRIES)[6] = R::drawable::dessert_ics;         // 2011
    (*RARE_PASTRIES)[7] = R::drawable::dessert_jellybean;   // 2012

    XRARE_PASTRIES = ArrayOf<Int32>::Alloc(4);
    (*XRARE_PASTRIES)[0] = R::drawable::dessert_petitfour;   // the original and still delicious

    (*XRARE_PASTRIES)[1] = R::drawable::dessert_donutburger; // remember kids, this was long before cronuts

    (*XRARE_PASTRIES)[2] = R::drawable::dessert_flan;        //     sholes final approach
                                    //     landing gear punted to flan
                                    //     runway foam glistens
                                    //         -- mcleron

    (*XRARE_PASTRIES)[3] = R::drawable::dessert_keylimepie;  // from an alternative timeline

    XXRARE_PASTRIES = ArrayOf<Int32>::Alloc(3);
    (*XXRARE_PASTRIES)[0] = R::drawable::dessert_zombiegingerbread; // thx hackbod
    (*XXRARE_PASTRIES)[1] = R::drawable::dessert_dandroid;    // thx morrildl
    (*XXRARE_PASTRIES)[2] = R::drawable::dessert_jandycane;   // thx nes

    NUM_PASTRIES = PASTRIES->GetLength() + RARE_PASTRIES->GetLength()
        + XRARE_PASTRIES->GetLength() + XXRARE_PASTRIES->GetLength();

    Float array_MASK[] = {
            0.f,  0.f,  0.f,  0.f, 255.f,
            0.f,  0.f,  0.f,  0.f, 255.f,
            0.f,  0.f,  0.f,  0.f, 255.f,
            1.f,  0.f,  0.f,  0.f, 0.f
    };
    MASK = ArrayOf<Float>::Alloc(array_MASK, sizeof(array_MASK) / sizeof(array_MASK[0]));

    Float array_ALPHA_MASK[] = {
            0.f,  0.f,  0.f,  0.f, 255.f,
            0.f,  0.f,  0.f,  0.f, 255.f,
            0.f,  0.f,  0.f,  0.f, 255.f,
            0.f,  0.f,  0.f,  1.f, 0.f
    };
    ALPHA_MASK = ArrayOf<Float>::Alloc(array_ALPHA_MASK, sizeof(array_ALPHA_MASK) / sizeof(array_ALPHA_MASK[0]));

    Float array_WHITE_MASK[] = {
            0.f,  0.f,  0.f,  0.f, 255.f,
            0.f,  0.f,  0.f,  0.f, 255.f,
            0.f,  0.f,  0.f,  0.f, 255.f,
            -1.f,  0.f,  0.f,  0.f, 255.f
    };
    WHITE_MASK = ArrayOf<Float>::Alloc(array_WHITE_MASK, sizeof(array_WHITE_MASK) / sizeof(array_WHITE_MASK[0]));
    return TRUE;
}

AutoPtr<IBitmap> CDessertCaseView::ConvertToAlphaMask(
    /* [in] */ IBitmap* b)
{
    Int32 w = 0, h = 0;
    b->GetWidth(&w);
    b->GetHeight(&h);
    AutoPtr<IBitmapHelper> helper;
    CBitmapHelper::AcquireSingleton((IBitmapHelper**)&helper);
    AutoPtr<IBitmap> a;
    helper->CreateBitmap(w, h, BitmapConfig_ALPHA_8, (IBitmap**)&a);
    AutoPtr<ICanvas> c;
    CCanvas::New(a, (ICanvas**)&c);
    AutoPtr<IPaint> pt;
    CPaint::New((IPaint**)&pt);
    AutoPtr<IColorMatrixColorFilter> p;
    CColorMatrixColorFilter::New(*MASK, (IColorMatrixColorFilter**)&p);
    pt->SetColorFilter(IColorFilter::Probe(p));
    c->DrawBitmap(b, 0.0f, 0.0f, pt);
    return a;
}

ECode CDessertCaseView::Start()
{
    if (!mStarted) {
        mStarted = TRUE;
        FillFreeList(DURATION * 4);
    }
    Boolean res = FALSE;
    mHandler->PostDelayed(mJuggle, START_DELAY, &res);
    return NOERROR;
}

ECode CDessertCaseView::Stop()
{
    mStarted = FALSE;
    mHandler->RemoveCallbacks(mJuggle);
    return NOERROR;
}

Int32 CDessertCaseView::Pick(
    /* [in] */ ArrayOf<Int32>* a)
{
    return (*a)[(Int32)(Elastos::Core::Math::Random() * a->GetLength())];
}

AutoPtr<IInterface> CDessertCaseView::Pick(
    /* [in] */ ArrayOf<IInterface*>* a)
{
    return (*a)[(Int32)(Elastos::Core::Math::Random() * a->GetLength())];
}

AutoPtr<IInterface> CDessertCaseView::Pick(
    /* [in] */ ISparseArray* sa)
{
    Int32 s = 0;
    sa->GetSize(&s);
    AutoPtr<IInterface> node;
    sa->ValueAt((Int32)(Elastos::Core::Math::Random() * s), (IInterface**)&node);
    return node;
}

Int32 CDessertCaseView::Random_color()
{
//        return 0xFF000000 | (int) (Math.random() * (Float) 0xFFFFFF); // totally random
    Int32 COLORS = 12;
    (*mHsv)[0] = Irand(0, COLORS) * (360.f/COLORS);
    AutoPtr<IColor> color;
    CColor::AcquireSingleton((IColor**)&color);
    Int32 c = 0;
    color->HSVToColor(mHsv, &c);
    return c;
}

// synchronized
void CDessertCaseView::OnSizeChanged (
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 oldw,
    /* [in] */ Int32 oldh)
{
    AutoLock lock(this);
    FrameLayout::OnSizeChanged(w, h, oldw, oldh);
    if (mWidth == w && mHeight == h) return;

    Boolean wasStarted = mStarted;
    if (wasStarted) {
        Stop();
    }

    mWidth = w;
    mHeight = h;

    mCells = NULL;
    RemoveAllViewsInLayout();
    mFreeList->Clear();

    mRows = mHeight / mCellSize;
    mColumns = mWidth / mCellSize;

    mCells = ArrayOf<IView*>::Alloc(mRows * mColumns);

    if (DEBUG) Logger::V(TAG, "New dimensions: %dx%d", mColumns, mRows);

    SetScaleX(SCALE);
    SetScaleY(SCALE);
    SetTranslationX(0.5f * (mWidth - mCellSize * mColumns) * SCALE);
    SetTranslationY(0.5f * (mHeight - mCellSize * mRows) * SCALE);

    for (Int32 j = 0; j < mRows; j++) {
        for (Int32 i = 0; i < mColumns; i++) {
            AutoPtr<IPoint> pt;
            CPoint::New(i, j, (IPoint**)&pt);
            mFreeList->Add(pt);
        }
    }

    if (wasStarted) {
        Start();
    }
}

ECode CDessertCaseView::FillFreeList()
{
    return FillFreeList(DURATION);
}

//synchronized
ECode CDessertCaseView::FillFreeList(
    /* [in] */ Int32 animationLen)
{
    AutoLock lock(this);
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IFrameLayoutLayoutParams> lp;
    CFrameLayoutLayoutParams::New(mCellSize, mCellSize, (IFrameLayoutLayoutParams**)&lp);

    Boolean bEmp = FALSE;
    while (! (mFreeList->IsEmpty(&bEmp), bEmp)) {
        AutoPtr<IIterator> it;
        mFreeList->GetIterator((IIterator**)&it);
        AutoPtr<IInterface> nt;
        it->GetNext((IInterface**)&nt);
        AutoPtr<IPoint> pt = IPoint::Probe(nt);
        mFreeList->Remove(pt);
        Int32 i = 0, j = 0;
        pt->Get(&i, &j);

        if ((*mCells)[j*mColumns+i] != NULL) continue;
        AutoPtr<IImageView> v;
        CImageView::New(ctx, (IImageView**)&v);
        IView::Probe(v)->SetOnClickListener(new OnClickListener(this, v));

        Int32 c = Random_color();
        IView::Probe(v)->SetBackgroundColor(c);

        Float which = Frand();
        AutoPtr<IDrawable> d;
        AutoPtr<IInterface> pD;
        if (which < 0.0005f) {
            mDrawables->Get(Pick(XXRARE_PASTRIES), (IInterface**)&pD);
        }
        else if (which < 0.005f) {
            mDrawables->Get(Pick(XRARE_PASTRIES), (IInterface**)&pD);
        }
        else if (which < 0.5f) {
            mDrawables->Get(Pick(RARE_PASTRIES), (IInterface**)&pD);
        }
        else if (which < 0.7f) {
            mDrawables->Get(Pick(PASTRIES), (IInterface**)&pD);
        }
        else {
            pD = NULL;
        }

        if (pD != NULL) {
            d = IDrawable::Probe(pD);
            AutoPtr<IViewOverlay> ol;
            IView::Probe(v)->GetOverlay((IViewOverlay**)&ol);
            ol->Add(d);
        }

        IViewGroupLayoutParams::Probe(lp)->SetWidth(mCellSize);
        IViewGroupLayoutParams::Probe(lp)->SetHeight(mCellSize);
        AddView(IView::Probe(v), IViewGroupLayoutParams::Probe(lp));
        Place(IView::Probe(v), pt, FALSE);
        if (animationLen > 0) {
            AutoPtr<IInterface> obj;
            IView::Probe(v)->GetTag(TAG_SPAN, (IInterface**)&obj);
            Int32 value = 0;
            IInteger32::Probe(obj)->GetValue(&value);
            Float s = value;
            IView::Probe(v)->SetScaleX(0.5f * s);
            IView::Probe(v)->SetScaleY(0.5f * s);
            IView::Probe(v)->SetAlpha(0.f);
            AutoPtr<IViewPropertyAnimator> anim;
            IView::Probe(v)->Animate((IViewPropertyAnimator**)&anim);
            anim->WithLayer();
            anim->ScaleX(s);
            anim->ScaleY(s);
            anim->Alpha(1.f);
            anim->SetDuration(animationLen);
        }
    }
    return NOERROR;
}

ECode CDessertCaseView::Place(
    /* [in] */ IView* v,
    /* [in] */ Boolean animate)
{
    AutoPtr<IPoint> pt;
    CPoint::New(Irand(0, mColumns), Irand(0, mRows), (IPoint**)&pt);
    return Place(v, pt, animate);
}

// we don't have .withLayer() on general Animators
AutoPtr<IAnimatorListener> CDessertCaseView::MakeHardwareLayerListener(
    /* [in] */ IView* v)
{
    return new AnimatorListenerAdapter_1(v);
}

ECode CDessertCaseView::Place(
    /* [in] */ IView* v,
    /* [in] */ IPoint* pt,
    /* [in] */ Boolean animate)
{
    AutoLock lock(this);
    Int32 i = 0, j = 0;
    pt->Get(&i, &j);
    Float rnd = Frand();
    AutoPtr<IInterface> t;
    v->GetTag(TAG_POS, (IInterface**)&t);
    if (t != NULL) {
        AutoPtr<ArrayOf<IPoint*> > arr = GetOccupied(v);
        for (Int32 i = 0; i < arr->GetLength(); i++) {
            AutoPtr<IPoint> oc = (*arr)[i];
            mFreeList->Add(oc);
            Int32 x = 0, y = 0;
            oc->Get(&x, &y);
            (*mCells)[y * mColumns + x] = NULL;
        }
    }
    Int32 scale = 1;
    if (rnd < PROB_4X) {
        if (!(i >= mColumns-3 || j >= mRows-3)) {
            scale = 4;
        }
    }
    else if (rnd < PROB_3X) {
        if (!(i >= mColumns-2 || j >= mRows-2)) {
            scale = 3;
        }
    }
    else if (rnd < PROB_2X) {
        if (!(i == mColumns-1 || j == mRows-1)) {
            scale = 2;
        }
    }

    v->SetTag(TAG_POS, pt);
    AutoPtr<IInteger32> pInt;
    CInteger32::New(scale, (IInteger32**)&pInt);
    v->SetTag(TAG_SPAN, pInt);

    mTmpSet->Clear();

    AutoPtr<ArrayOf<IPoint*> > occupied = GetOccupied(v);
    for (Int32 i = 0; i < occupied->GetLength(); i++) {
        AutoPtr<IPoint> oc = (*occupied)[i];
        Int32 x = 0, y = 0;
        oc->Get(&x, &y);
        AutoPtr<IView> squatter = (*mCells)[y * mColumns + x];
        if (squatter != NULL) {
            mTmpSet->Add(squatter);
        }
    }

    AutoPtr<IIterator> it;
    mTmpSet->GetIterator((IIterator**)&it);
    Boolean bHasMore = FALSE;
    while ((it->HasNext(&bHasMore), bHasMore)) {
        AutoPtr<IInterface> pSq;
        it->GetNext((IInterface**)&pSq);
        AutoPtr<IView> squatter = IView::Probe(pSq);
        AutoPtr<ArrayOf<IPoint*> > arr = GetOccupied(squatter);
        for (Int32 i = 0; i < arr->GetLength(); i++) {
            AutoPtr<IPoint> sq = (*arr)[i];
            mFreeList->Add(sq);
            Int32 x = 0, y = 0;
            sq->Get(&x, &y);
            (*mCells)[y * mColumns + x] = NULL;
        }
        if (!Object::Equals(squatter->Probe(EIID_IInterface), v->Probe(EIID_IInterface))) {
            squatter->SetTag(TAG_POS, NULL);
            if (animate) {
                AutoPtr<IViewPropertyAnimator> anim;
                squatter->Animate((IViewPropertyAnimator**)&anim);
                anim->WithLayer();
                anim->ScaleX(0.5f);
                anim->ScaleY(0.5f);
                anim->Alpha(0);
                anim->SetDuration(DURATION);
                AutoPtr<IAccelerateInterpolator> ai;
                CAccelerateInterpolator::New((IAccelerateInterpolator**)&ai);
                anim->SetInterpolator(ITimeInterpolator::Probe(ai));
                anim->SetListener(new AnimatorListener(this, squatter));
                anim->Start();
            }
            else {
                RemoveView(squatter);
            }
        }
    }

    for (Int32 i = 0; i < occupied->GetLength(); i++) {
        AutoPtr<IPoint> oc = (*occupied)[i];
        Int32 x = 0, y = 0;
        oc->Get(&x, &y);
        (*mCells)[y * mColumns + x] = v;
        mFreeList->Remove(oc);
    }

    Float rot = (Float)Irand(0, 4) * 90.f;

    if (animate) {
        v->BringToFront();

        AutoPtr<IObjectAnimatorHelper> helper;
        CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
        AutoPtr<IAnimatorSet> set1;
        CAnimatorSet::New((IAnimatorSet**)&set1);
        AutoPtr<IObjectAnimator> oa1, oa2;
        AutoPtr<ArrayOf<Float> > fa = ArrayOf<Float>::Alloc(1);
        (*fa)[0] = scale;
        helper->OfFloat(v, View::SCALE_X, fa, (IObjectAnimator**)&oa1);
        helper->OfFloat(v, View::SCALE_Y, fa, (IObjectAnimator**)&oa2);
        AutoPtr<ArrayOf<IAnimator*> > array = ArrayOf<IAnimator*>::Alloc(2);
        array->Set(0, IAnimator::Probe(oa1));
        array->Set(1, IAnimator::Probe(oa2));
        set1->PlayTogether(array);
        AutoPtr<IAnticipateOvershootInterpolator> p;
        CAnticipateOvershootInterpolator::New((IAnticipateOvershootInterpolator**)&p);
        IAnimator::Probe(set1)->SetInterpolator(ITimeInterpolator::Probe(p));
        IAnimator::Probe(set1)->SetDuration(DURATION);

        AutoPtr<IAnimatorSet> set2;
        CAnimatorSet::New((IAnimatorSet**)&set2);
        oa1 = oa2 = NULL;
        (*fa)[0] = rot;
        helper->OfFloat(v, View::ROTATION, fa, (IObjectAnimator**)&oa1);
        (*fa)[0] = i* mCellSize + (scale-1) * mCellSize /2;
        helper->OfFloat(v, View::X, fa, (IObjectAnimator**)&oa2);
        AutoPtr<IObjectAnimator> oa3;
        (*fa)[0] = j * mCellSize + (scale - 1) * mCellSize / 2;
        helper->OfFloat(v, View::Y, fa, (IObjectAnimator**)&oa3);

        AutoPtr<ArrayOf<IAnimator*> > array2 = ArrayOf<IAnimator*>::Alloc(3);
        array2->Set(0, IAnimator::Probe(oa1));
        array2->Set(1, IAnimator::Probe(oa2));
        array2->Set(2, IAnimator::Probe(oa3));
        set2->PlayTogether(array2);
        AutoPtr<IDecelerateInterpolator> di;
        CDecelerateInterpolator::New((IDecelerateInterpolator**)&di);
        IAnimator::Probe(set2)->SetInterpolator(ITimeInterpolator::Probe(di));
        IAnimator::Probe(set2)->SetDuration(DURATION);

        IAnimator::Probe(set1)->AddListener(MakeHardwareLayerListener(v));

        IAnimator::Probe(set1)->Start();
        IAnimator::Probe(set2)->Start();
    }
    else {
        v->SetX(i * mCellSize + (scale-1) * mCellSize /2);
        v->SetY(j * mCellSize + (scale-1) * mCellSize /2);
        v->SetScaleX((Float) scale);
        v->SetScaleY((Float) scale);
        v->SetRotation(rot);
    }
    return NOERROR;
}

AutoPtr<ArrayOf<IPoint*> > CDessertCaseView::GetOccupied(
    /* [in] */ IView* v)
{
    AutoPtr<IInterface> pTS;
    v->GetTag(TAG_SPAN, (IInterface**)&pTS);
    AutoPtr<IInteger32> pInt = IInteger32::Probe(pTS);
    Int32 scale = 0;
    pInt->GetValue(&scale);
    AutoPtr<IInterface> pPt;
    v->GetTag(TAG_POS, (IInterface**)&pPt);
    AutoPtr<IPoint> pt = IPoint::Probe(pPt);
    if (pt == NULL || scale == 0) {
        AutoPtr<IPoint> ip;
        CPoint::New((IPoint**)&ip);
        AutoPtr<ArrayOf<IPoint*> > arr = ArrayOf<IPoint*>::Alloc(1);
        (*arr)[0] = ip;
        return arr;
    }

    AutoPtr<ArrayOf<IPoint*> > result = ArrayOf<IPoint*>::Alloc(scale * scale);
    Int32 p = 0;
    for (Int32 i = 0; i < scale; i++) {
        for (Int32 j = 0; j < scale; j++) {
            Int32 x = 0, y = 0;
            pt->Get(&x, &y);
            CPoint::New(x + i, y + j, &((*result)[p++]));
        }
    }
    return result;
}

Float CDessertCaseView::Frand()
{
    return (Float)(Elastos::Core::Math::Random());
}

Float CDessertCaseView::Frand(
    /* [in] */ Float a,
    /* [in] */ Float b)
{
    return (Frand() * (b-a) + a);
}

Int32 CDessertCaseView::Irand(
    /* [in] */ Int32 a,
    /* [in] */ Int32 b)
{
    return (Int32)(Frand(a, b));
}

void CDessertCaseView::OnDraw(
    /* [in] */ ICanvas* c)
{
    FrameLayout::OnDraw(c);
    if (!DEBUG) return;

    AutoPtr<IPaint> pt;
    CPaint::New((IPaint**)&pt);
    pt->SetStyle(PaintStyle_STROKE);
    pt->SetColor(0xFFCCCCCC);
    pt->SetStrokeWidth(2.0f);

    AutoPtr<IRect> check;
    CRect::New((IRect**)&check);
    Int32 N = 0;
    GetChildCount(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IView> stone;
        GetChildAt(i, (IView**)&stone);

        stone->GetHitRect(check);

        c->DrawRect(check, pt);
    }
}

//===============================================================
// CDessertCaseView::RescalingContainer
//===============================================================
CDessertCaseView::RescalingContainer::RescalingContainer(
    /* [in] */ IContext* context)// : FrameLayout(context)
{
    FrameLayout::constructor(context);
    SetSystemUiVisibility(0
            | IView::SYSTEM_UI_FLAG_FULLSCREEN
            | IView::SYSTEM_UI_FLAG_HIDE_NAVIGATION
            | IView::SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
            | IView::SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
            | IView::SYSTEM_UI_FLAG_IMMERSIVE_STICKY
    );
}

void CDessertCaseView::RescalingContainer::SetView(
    /* [in] */ IDessertCaseView* v)
{
    AddView(IView::Probe(v));
    mView = v;
}

ECode CDessertCaseView::RescalingContainer::OnLayout (
    /* [in] */ Boolean changed,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    Float w = right-left;
    Float h = bottom-top;
    Int32 w2 = (Int32) (w / mView->SCALE / 2);
    Int32 h2 = (Int32) (h / mView->SCALE / 2);
    Int32 cx = (Int32) (left + w * 0.5f);
    Int32 cy = (Int32) (top + h * 0.5f);
    IView::Probe(mView)->Layout(cx - w2, cy - h2, cx + w2, cy + h2);
    return NOERROR;
}

void CDessertCaseView::RescalingContainer::SetDarkness(
    /* [in] */ Float p)
{
    mDarkness = p;
    GetDarkness();
    Int32 x = (Int32) (p * 0xff);
    SetBackgroundColor(x << 24 & 0xFF000000);
}

Float CDessertCaseView::RescalingContainer::GetDarkness()
{
    return mDarkness;
}

//===============================================================
// CDessertCaseView::Runnable_1
//===============================================================
CDessertCaseView::Runnable_1::Runnable_1(
    /* [in] */ CDessertCaseView* owner)
    : mOwner(owner)
{
}

ECode CDessertCaseView::Runnable_1::Run()
{
    Int32 N = 0;
    mOwner->GetChildCount(&N);

    Int32 K = 1; //irand(1,3);
    for (Int32 i = 0; i < K; i++) {
        AutoPtr<IView> child;
        IViewGroup::Probe(mOwner)->GetChildAt((Int32) (Elastos::Core::Math::Random() * N), (IView**)&child);
        mOwner->Place(child, TRUE);
    }

    mOwner->FillFreeList();

    if (mOwner->mStarted) {
        Boolean res = FALSE;
        mOwner->mHandler->PostDelayed(mOwner->mJuggle, DELAY, &res);
    }
    return NOERROR;
}

//===============================================================
// CDessertCaseView::AnimatorListenerAdapter_1
//===============================================================
CDessertCaseView::AnimatorListenerAdapter_1::AnimatorListenerAdapter_1(
    /* [in] */ IView* v)
    : mV(v)
{
}

ECode CDessertCaseView::AnimatorListenerAdapter_1::OnAnimationStart(
    /* [in] */ IAnimator* animator)
{
    mV->SetLayerType(IView::LAYER_TYPE_HARDWARE, NULL);
    mV->BuildLayer();
    return NOERROR;
}

ECode CDessertCaseView::AnimatorListenerAdapter_1::OnAnimationEnd(
    /* [in] */ IAnimator* animator)
{
    mV->SetLayerType(IView::LAYER_TYPE_NONE, NULL);
    return NOERROR;
}

//===============================================================
// CDessertCaseView::AnimatorListener::
//===============================================================
CAR_INTERFACE_IMPL(CDessertCaseView::AnimatorListener, Object, IAnimatorListener)
CDessertCaseView::AnimatorListener::AnimatorListener(
    /* [in] */ CDessertCaseView* owner,
    /* [in] */ IView* view)
    : mOwner(owner)
    , mView(view)
{
}

ECode CDessertCaseView::AnimatorListener::OnAnimationStart(
    /* [in] */ IAnimator* animator)
{
    return NOERROR;
}

ECode CDessertCaseView::AnimatorListener::OnAnimationEnd(
    /* [in] */ IAnimator* animator)
{
    mOwner->RemoveView(mView);
    return NOERROR;
}

ECode CDessertCaseView::AnimatorListener::OnAnimationCancel(
    /* [in] */ IAnimator* animator)
{
    return NOERROR;
}

ECode CDessertCaseView::AnimatorListener::OnAnimationRepeat(
    /* [in] */ IAnimator* animator)
{
    return NOERROR;
}

//===============================================================
// CDessertCaseView::OnClickListener
//===============================================================
CAR_INTERFACE_IMPL(CDessertCaseView::OnClickListener, Object, IViewOnClickListener)
CDessertCaseView::OnClickListener::OnClickListener(
    /* [in] */ CDessertCaseView* owner,
    /* [in] */ IImageView* v)
    : mOwner(owner)
    , mV(v)
{
}

ECode CDessertCaseView::OnClickListener::OnClick(
    /* [in] */ IView* view)
{
    mOwner->Place(IView::Probe(mV), TRUE);
    Boolean res = FALSE;
    IView::Probe(mOwner)->PostDelayed(new Runnable_2(mOwner), DURATION/2, &res);
    return NOERROR;
}

//===============================================================
// CDessertCaseView::Runnable_2
//===============================================================
CDessertCaseView::Runnable_2::Runnable_2(
    /* [in] */ CDessertCaseView* owner)
    : mOwner(owner)
{
}

ECode CDessertCaseView::Runnable_2::Run()
{
    mOwner->FillFreeList();
    return NOERROR;
}

} // namespace SystemUI
} // namepsace Droid
} // namespace Elastos
