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

#ifndef __ELASTOS_DROID_TEXT_TEXTDIRECTIONHEURISTICS_H__
#define __ELASTOS_DROID_TEXT_TEXTDIRECTIONHEURISTICS_H__

#include "Elastos.Droid.Text.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include <elastos/core/Character.h>

using Elastos::Core::Character;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Text {

/**
 * Some objects that implement {@link TextDirectionHeuristic}. Use these with
 * the {@link BidiFormatter#unicodeWrap unicodeWrap()} methods in {@link BidiFormatter}.
 * Also notice that these direction heuristics correspond to the same types of constants
 * provided in the {@link android.view.View} class for {@link android.view.View#setTextDirection
 * setTextDirection()}, such as {@link android.view.View#TEXT_DIRECTION_RTL}.
 * <p>To support versions lower than {@link android.os.Build.VERSION_CODES#JELLY_BEAN_MR2},
 * you can use the support library's {@link android.support.v4.text.TextDirectionHeuristicsCompat}
 * class.
 *
 */
class TextDirectionHeuristics
{
private:
    /**
     * Computes the text direction based on an algorithm.  Subclasses implement
     * {@link #defaultIsRtl} to handle cases where the algorithm cannot determine the
     * direction from the text alone.
     */
    class TextDirectionHeuristicImpl
        : public Object
        , public ITextDirectionHeuristic
    {
    public:
        CAR_INTERFACE_DECL()

        TextDirectionHeuristicImpl(
            /* [in] */ ITextDirectionAlgorithm* algorithm);

        virtual ~TextDirectionHeuristicImpl();

        //@Override
        CARAPI IsRtl(
            /* [in] */ ArrayOf<Char32>* chars,
            /* [in] */ Int32 start,
            /* [in] */ Int32 count,
            /* [out] */ Boolean* result);

        CARAPI IsRtl(
            /* [in] */ ICharSequence* chars,
            /* [in] */ Int32 start,
            /* [in] */ Int32 count,
            /* [out] */ Boolean* result);

    protected:
        /**
         * Return true if the default text direction is rtl.
         */
        virtual CARAPI_(Boolean) DefaultIsRtl() = 0;

    private:
        CARAPI_(Boolean) DoCheck(
            /* [in] */ ArrayOf<Char32>* chars,
            /* [in] */ Int32 start,
            /* [in] */ Int32 count);

        AutoPtr<ITextDirectionAlgorithm> mAlgorithm;
    };

    class TextDirectionHeuristicInternal
        : public TextDirectionHeuristicImpl
    {
    public:
        TextDirectionHeuristicInternal(
            /* [in] */ ITextDirectionAlgorithm* algorithm,
            /* [in] */ Boolean defaultIsRtl);

        ~TextDirectionHeuristicInternal();

        //@Override
        CARAPI_(Boolean) DefaultIsRtl();

    private:
        Boolean mDefaultIsRtl;
    };

    /**
     * Algorithm that uses the first strong directional character to determine the paragraph
     * direction. This is the standard Unicode Bidirectional algorithm.
     */
    class FirstStrong
        : public Object
        , public ITextDirectionAlgorithm
    {
    public:
        CAR_INTERFACE_DECL()

        //@Override
        CARAPI CheckRtl(
            /* [in] */ ArrayOf<Char32>* chars,
            /* [in] */ Int32 start,
            /* [in] */ Int32 count,
            /* [out] */ Int32* result);

        virtual ~FirstStrong();

    private:
        FirstStrong();

    public:
        static AutoPtr<FirstStrong> INSTANCE;// = new FirstStrong();
    };

    /**
     * Algorithm that uses the presence of any strong directional non-format
     * character (e.g. excludes LRE, LRO, RLE, RLO) to determine the
     * direction of text.
     *
     */
    class AnyStrong
        : public Object
        , public ITextDirectionAlgorithm
    {
    public:
        CAR_INTERFACE_DECL()

        //@Override
        CARAPI CheckRtl(
            /* [in] */ ArrayOf<Char32>* chars,
            /* [in] */ Int32 start,
            /* [in] */ Int32 count,
            /* [out] */ Int32* result);

        virtual ~AnyStrong();

    private:
        AnyStrong(
            /* [in] */ Boolean lookForRtl);

    public:
        static AutoPtr<AnyStrong> INSTANCE_RTL;// = new AnyStrong(true);
        static AutoPtr<AnyStrong> INSTANCE_LTR;// = new AnyStrong(false);

    private:
        Boolean mLookForRtl;
    };

    /**
     * Algorithm that uses the Locale direction to force the direction of a paragraph.
     */
    class TextDirectionHeuristicLocale
        : public TextDirectionHeuristicImpl
    {
    public:
        TextDirectionHeuristicLocale();

        ~TextDirectionHeuristicLocale();

    protected:
        //@Override
        CARAPI_(Boolean) DefaultIsRtl();

    public:
        static AutoPtr<TextDirectionHeuristicLocale> INSTANCE;// = new TextDirectionHeuristicLocale();
    };

private:
    static Int32 IsRtlText(
        /* [in] */ Int32 directionality);

    static Int32 IsRtlTextOrFormat(
        /* [in] */ Int32 directionality);

public:
    /** Always decides that the direction is left to right. */
    static AutoPtr<ITextDirectionHeuristic> LTR;

    /** Always decides that the direction is right to left. */
    static AutoPtr<ITextDirectionHeuristic> RTL;

    /**
     * Determines the direction based on the first strong directional character,
     * including bidi format chars, falling back to left to right if it
     * finds none.  This is the default behavior of the Unicode Bidirectional
     * Algorithm.
     */
    static AutoPtr<ITextDirectionHeuristic> FIRSTSTRONG_LTR;

    /**
     * If the text contains any strong right to left non-format character, determines
     * that the direction is right to left, falling back to left to right if it
     * finds none.
     */
    static AutoPtr<ITextDirectionHeuristic> ANYRTL_LTR;

    /**
     * Force the paragraph direction to the Locale direction. Falls back to left to right.
     */
    static AutoPtr<ITextDirectionHeuristic> LOCALE;

    /**
     * Determines the direction based on the first strong directional character,
     * including bidi format chars, falling back to right to left if it
     * finds none.  This is similar to the default behavior of the Unicode
     * Bidirectional Algorithm, just with different fallback behavior.
     */
    static AutoPtr<ITextDirectionHeuristic> FIRSTSTRONG_RTL;

private:
    static const Int32 STATE_TRUE;// = 0;
    static const Int32 STATE_FALSE;// = 1;
    static const Int32 STATE_UNKNOWN;// = 2;
};

} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TEXT_TEXTDIRECTIONHEURISTICS_H__
