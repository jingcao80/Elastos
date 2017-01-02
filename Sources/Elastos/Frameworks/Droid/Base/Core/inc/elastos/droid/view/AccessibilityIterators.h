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

#ifndef __ELASTOS_DROID_VIEW_ACCESSIBILITYITERATORS_H__
#define __ELASTOS_DROID_VIEW_ACCESSIBILITYITERATORS_H__

#include "Elastos.CoreLibrary.Text.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"

#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Text::IBreakIterator;
using Elastos::Utility::ILocale;

using Elastos::Droid::Content::IComponentCallbacks;
using Elastos::Droid::Content::Res::IConfiguration;

namespace Elastos {
namespace Droid {
namespace View {

class AccessibilityIterators
{
public:
    class AbstractTextSegmentIterator
        : public Object
        , public ITextSegmentIterator
    {
    public:
        CAR_INTERFACE_DECL()

        AbstractTextSegmentIterator();

        CARAPI Initialize(
            /* [in] */ const String& text);

    protected:
        AutoPtr<ArrayOf<Int32> > GetRange(
            /* [in] */ Int32 start,
            /* [in] */ Int32 end);

        String mText;

    private:
        AutoPtr<ArrayOf<Int32> > mSegment;
    };


    class CharacterTextSegmentIterator
        : public AbstractTextSegmentIterator
        , public IComponentCallbacks
    {
    public:
        CAR_INTERFACE_DECL()

        static AutoPtr<CharacterTextSegmentIterator> GetInstance(
            /* [in] */ ILocale* locale);

        CARAPI Initialize(
            /* [in] */ const String& text);

        CARAPI Following(
            /* [in] */ Int32 offset,
            /* [out, callee] */ ArrayOf<Int32>** array);

        CARAPI Preceding(
            /* [in] */ Int32 offset,
            /* [out, callee] */ ArrayOf<Int32>** array);

        CARAPI OnConfigurationChanged(
            /* [in] */ IConfiguration* newConfig);

        CARAPI OnLowMemory();

    protected:
        CARAPI OnLocaleChanged(
            /* [in] */ ILocale* locale);

    protected:
        CharacterTextSegmentIterator(
            /* [in] */ ILocale* locale);

    protected:
        AutoPtr<IBreakIterator> mImpl;

    private:
        static AutoPtr<CharacterTextSegmentIterator> sInstance;
        AutoPtr<ILocale> mLocale;
    };

    class WordTextSegmentIterator
        : public CharacterTextSegmentIterator
    {
    public:
        static AutoPtr<WordTextSegmentIterator> GetInstance(
            /* [in] */ ILocale* locale);

        CARAPI Following(
            /* [in] */ Int32 offset,
            /* [out, callee] */ ArrayOf<Int32>** array);

        CARAPI Preceding(
            /* [in] */ Int32 offsetoffset,
            /* [out, callee] */ ArrayOf<Int32>** array);

    protected:
        CARAPI OnLocaleChanged(
            /* [in] */ ILocale* locale);

    private:
        CARAPI_(Boolean) IsStartBoundary(
            /* [in] */ Int32 index);

        CARAPI_(Boolean) IsEndBoundary(
            /* [in] */ Int32 index);

        CARAPI_(Boolean) IsLetterOrDigit(
            /* [in] */ Int32 index);

        WordTextSegmentIterator(
            /* [in] */ ILocale* locale);

        static AutoPtr<WordTextSegmentIterator> sInstance;
    };

    class ParagraphTextSegmentIterator
        : public AbstractTextSegmentIterator
    {
    public:
        static AutoPtr<ParagraphTextSegmentIterator> GetInstance();

        CARAPI Following(
            /* [in] */ Int32 offset,
            /* [out, callee] */ ArrayOf<Int32>** array);

        CARAPI Preceding(
            /* [in] */ Int32 offset,
            /* [out, callee] */ ArrayOf<Int32>** array);

    private:
        CARAPI_(Boolean) IsStartBoundary(
            /* [in] */ Int32 index);

        CARAPI_(Boolean) IsEndBoundary(
            /* [in] */ Int32 index);

        static AutoPtr<ParagraphTextSegmentIterator> sInstance;
    };
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif