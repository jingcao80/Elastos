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

#ifndef __ELASTOS_DROID_SETTINGS_DEVICEINFO_CPERCENTAGEBARCHART_H__
#define __ELASTOS_DROID_SETTINGS_DEVICEINFO_CPERCENTAGEBARCHART_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "_Elastos_Droid_Settings_Deviceinfo_CPercentageBarChart.h"
#include "elastos/droid/view/View.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Core::IComparable;
using Elastos::Utility::ICollection;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Deviceinfo {

/**
 * Draws a horizontal bar chart with colored slices, each represented by
 * {@link Entry}.
 */
CarClass(CPercentageBarChart)
    , public Elastos::Droid::View::View
{
public:
    class Entry
        : public Object
        , public IComparable
    {
        friend class CPercentageBarChart;
    public:
        TO_STRING_IMPL("CPercentageBarChart::Entry")

        CAR_INTERFACE_DECL()

        //@Override
        CARAPI CompareTo(
            /* [in] */ IInterface* obj,
            /* [out] */ Int32* result);

    protected:
        Entry(
            /* [in] */ Int32 order,
            /* [in] */ Float percentage,
            /* [in] */ IPaint* paint);

    public:
        Int32 mOrder;
        Float mPercentage;
        AutoPtr<IPaint> mPaint;
    };

public:
    TO_STRING_IMPL("CPercentageBarChart")

    CAR_OBJECT_DECL()

    CPercentageBarChart();

    ~CPercentageBarChart();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    /**
     * Sets the background for this chart. Callers are responsible for later
     * calling {@link #Invalidate()}.
     */
    //@Override
    CARAPI SetBackgroundColor(
        /* [in] */ Int32 color);

    /**
     * Adds a new slice to the percentage bar chart. Callers are responsible for
     * later calling {@link #Invalidate()}.
     *
     * @param percentage the total width that
     * @param color the color to draw the entry
     */
    static CARAPI_(AutoPtr<Entry>) CreateEntry(
        /* [in] */ Int32 order,
        /* [in] */ Float percentage,
        /* [in] */ Int32 color);

    CARAPI SetEntries(
        /* [in] */ ICollection* entries); //Collection<Entry>

protected:
    //@Override
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

private:
    AutoPtr<IPaint> mEmptyPaint;

    AutoPtr<ICollection> mEntries;//Collection<Entry>

    Int32 mMinTickWidth;
};

} // namespace Deviceinfo
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_DEVICEINFO_CPERCENTAGEBARCHART_H__