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

#include "Elastos.Droid.Content.h"
#include "elastos/droid/settings/deviceinfo/CPercentageBarChart.h"
#include "../R.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::PaintStyle_FILL;;
using Elastos::Core::EIID_IComparable;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Deviceinfo {

//===============================================================================
//                  CPercentageBarChart::Entry
//===============================================================================

CAR_INTERFACE_IMPL(CPercentageBarChart::Entry, Object, IComparable)

CPercentageBarChart::Entry::Entry(
    /* [in] */ Int32 order,
    /* [in] */ Float percentage,
    /* [in] */ IPaint* paint)
{
    mOrder = order;
    mPercentage = percentage;
    mPaint = paint;
}

ECode CPercentageBarChart::Entry::CompareTo(
    /* [in] */ IInterface* obj,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Entry* another = (Entry*)IObject::Probe(obj);
    *result = mOrder - another->mOrder;
    return NOERROR;
}

//===============================================================================
//                  CPercentageBarChart
//===============================================================================

CAR_OBJECT_IMPL(CPercentageBarChart)

CPercentageBarChart::CPercentageBarChart()
    : mMinTickWidth(1)
{}

CPercentageBarChart::~CPercentageBarChart()
{}

ECode CPercentageBarChart::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    View::constructor(context, attrs);

    CPaint::New((IPaint**)&mEmptyPaint);

    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::PercentageBarChart);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a);
    a->GetDimensionPixelSize(R::styleable::PercentageBarChart_minTickWidth, 1, &mMinTickWidth);
    Int32 emptyColor;
    a->GetColor(R::styleable::PercentageBarChart_emptyColor, IColor::BLACK, &emptyColor);
    a->Recycle();

    mEmptyPaint->SetColor(emptyColor);
    mEmptyPaint->SetStyle(PaintStyle_FILL);
    return NOERROR;
}

void CPercentageBarChart::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    View::OnDraw(canvas);

    Int32 left;
    GetPaddingLeft(&left);
    Int32 wd, heigth, paddingRight, paddingBottom;
    const Int32 right = (GetWidth(&wd), wd) - (GetPaddingRight(&paddingRight), paddingRight);
    Int32 top;
    GetPaddingTop(&top);
    const Int32 bottom = (GetHeight(&heigth), heigth) - (GetPaddingBottom(&paddingBottom), paddingBottom);

    const Int32 width = right - left;

    Boolean isLayoutRtl;
    IsLayoutRtl(&isLayoutRtl);
    using Elastos::Core::Math;
    if (isLayoutRtl) {
        Float nextX = right;

        if (mEntries != NULL) {
            AutoPtr<IIterator> iterator;
            mEntries->GetIterator((IIterator**)&iterator);
            Boolean hasNext;
            while (iterator->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj;
                iterator->GetNext((IInterface**)&obj);
                Entry* e = (Entry*)IObject::Probe(obj);

                Float entryWidth;
                if (e->mPercentage == 0.0f) {
                    entryWidth = 0.0f;
                }
                else {
                    entryWidth = Math::Max((Float)mMinTickWidth, width * e->mPercentage);
                }

                const Float lastX = nextX - entryWidth;
                if (lastX < left) {
                    canvas->DrawRect(left, top, nextX, bottom, e->mPaint);
                    return;
                }

                canvas->DrawRect(lastX, top, nextX, bottom, e->mPaint);
                nextX = lastX;
            }
        }

        canvas->DrawRect(left, top, nextX, bottom, mEmptyPaint);
    }
    else {
        Float lastX = left;

        if (mEntries != NULL) {
            AutoPtr<IIterator> iterator;
            mEntries->GetIterator((IIterator**)&iterator);
            Boolean hasNext;
            while (iterator->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj;
                iterator->GetNext((IInterface**)&obj);
                Entry* e = (Entry*)IObject::Probe(obj);

                Float entryWidth;
                if (e->mPercentage == 0.0f) {
                    entryWidth = 0.0f;
                }
                else {
                    entryWidth = Math::Max((Float)mMinTickWidth, width * e->mPercentage);
                }

                const Float nextX = lastX + entryWidth;
                if (nextX > right) {
                    canvas->DrawRect(lastX, top, right, bottom, e->mPaint);
                    return;
                }

                canvas->DrawRect(lastX, top, nextX, bottom, e->mPaint);
                lastX = nextX;
            }
        }

        canvas->DrawRect(lastX, top, right, bottom, mEmptyPaint);
    }
}

ECode CPercentageBarChart::SetBackgroundColor(
    /* [in] */ Int32 color)
{
    mEmptyPaint->SetColor(color);
    return NOERROR;
}

AutoPtr<CPercentageBarChart::Entry> CPercentageBarChart::CreateEntry(
    /* [in] */ Int32 order,
    /* [in] */ Float percentage,
    /* [in] */ Int32 color)
{
    AutoPtr<IPaint> p;
    CPaint::New((IPaint**)&p);
    p->SetColor(color);
    p->SetStyle(PaintStyle_FILL);
    AutoPtr<Entry> entry = new Entry(order, percentage, p);
    return entry;
}

ECode CPercentageBarChart::SetEntries(
    /* [in] */ ICollection* entries)
{
   mEntries = entries;
   return NOERROR;
}

} // namespace Deviceinfo
} // namespace Settings
} // namespace Droid
} // namespace Elastos