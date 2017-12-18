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

#ifndef __ELASTOS_DROID_VIEW_INPUTMETHOD_CCURSORANCHORINFO_H__
#define __ELASTOS_DROID_VIEW_INPUTMETHOD_CCURSORANCHORINFO_H__

#include "Elastos.Droid.Graphics.h"
#include "_Elastos_Droid_View_InputMethod_CCursorAnchorInfo.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Graphics::IRectF;

using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Os {

class AndroidParcelUtils;

}
}
}

namespace Elastos {
namespace Droid {
namespace View {
namespace InputMethod {

CarClass(CCursorAnchorInfo)
    , public Object
    , public ICursorAnchorInfo
    , public IParcelable
{
public:
    /**
     * Builder for {@link CursorAnchorInfo}. This class is not designed to be thread-safe.
     */
    class Builder
        : public Object
        , public ICursorAnchorInfoBuilder
    {
    public:
        CAR_INTERFACE_DECL()

        Builder();

        /**
         * Sets the text range of the selection. Calling this can be skipped if there is no
         * selection.
         */
        CARAPI SetSelectionRange(
            /* [in] */ Int32 newStart,
            /* [in] */ Int32 newEnd);

        /**
         * Sets the text range of the composing text. Calling this can be skipped if there is
         * no composing text.
         * @param composingTextStart index where the composing text starts.
         * @param composingText the entire composing text.
         */
        CARAPI SetComposingText(
            /* [in] */ Int32 composingTextStart,
            /* [in] */ ICharSequence* composingText);

        /**
         * Sets the location of the text insertion point (zero width cursor) as a rectangle in
         * local coordinates. Calling this can be skipped when there is no text insertion point;
         * however if there is an insertion point, editors must call this method.
         * @param horizontalPosition horizontal position of the insertion marker, in the local
         * coordinates that will be transformed with the transformation matrix when rendered on the
         * screen. This should be calculated or compatible with
         * {@link Layout#getPrimaryHorizontal(int)}.
         * @param lineTop vertical position of the insertion marker, in the local coordinates that
         * will be transformed with the transformation matrix when rendered on the screen. This
         * should be calculated or compatible with {@link Layout#getLineTop(int)}.
         * @param lineBaseline vertical position of the insertion marker, in the local coordinates
         * that will be transformed with the transformation matrix when rendered on the screen. This
         * should be calculated or compatible with {@link Layout#getLineBaseline(int)}.
         * @param lineBottom vertical position of the insertion marker, in the local coordinates
         * that will be transformed with the transformation matrix when rendered on the screen. This
         * should be calculated or compatible with {@link Layout#getLineBottom(int)}.
         * @param flags flags of the insertion marker. See {@link #FLAG_HAS_VISIBLE_REGION} for
         * example.
         */
        CARAPI SetInsertionMarkerLocation(
            /* [in] */ Float horizontalPosition,
            /* [in] */ Float lineTop,
            /* [in] */ Float lineBaseline,
            /* [in] */ Float lineBottom,
            /* [in] */ Int32 flags);

        /**
         * Adds the bounding box of the character specified with the index.
         *
         * @param index index of the character in Java chars units. Must be specified in
         * ascending order across successive calls.
         * @param left x coordinate of the left edge of the character in local coordinates.
         * @param top y coordinate of the top edge of the character in local coordinates.
         * @param right x coordinate of the right edge of the character in local coordinates.
         * @param bottom y coordinate of the bottom edge of the character in local coordinates.
         * @param flags flags for this character bounds. See {@link #FLAG_HAS_VISIBLE_REGION},
         * {@link #FLAG_HAS_INVISIBLE_REGION} and {@link #FLAG_IS_RTL}. These flags must be
         * specified when necessary.
         * @throws IllegalArgumentException If the index is a negative value, or not greater than
         * all of the previously called indices.
         */
        CARAPI AddCharacterBounds(
            /* [in] */ Int32 index,
            /* [in] */ Float left,
            /* [in] */ Float top,
            /* [in] */ Float right,
            /* [in] */ Float bottom,
            /* [in] */ Int32 flags);

        /**
         * Sets the matrix that transforms local coordinates into screen coordinates.
         * @param matrix transformation matrix from local coordinates into screen coordinates. null
         * is interpreted as an identity matrix.
         */
        CARAPI SetMatrix(
            /* [in] */ IMatrix* matrix);

        /**
         * @return {@link CursorAnchorInfo} using parameters in this {@link Builder}.
         * @throws IllegalArgumentException if one or more positional parameters are specified but
         * the coordinate transformation matrix is not provided via {@link #setMatrix(Matrix)}.
         */
        CARAPI Build(
            /* [out] */ ICursorAnchorInfo** info);

        /**
         * Resets the internal state so that this instance can be reused to build another
         * instance of {@link CursorAnchorInfo}.
         */
        CARAPI Reset();

    public:
        Int32 mSelectionStart;
        Int32 mSelectionEnd;
        Int32 mComposingTextStart;
        AutoPtr<ICharSequence> mComposingText;
        Float mInsertionMarkerHorizontal;
        Float mInsertionMarkerTop;
        Float mInsertionMarkerBaseline;
        Float mInsertionMarkerBottom;
        Int32 mInsertionMarkerFlags;
        AutoPtr<ISparseRectFArrayBuilder> mCharacterBoundsArrayBuilder;
        AutoPtr<IMatrix> mMatrix;
        Boolean mMatrixInitialized;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    /**
     * Used to package this object into a {@link Parcel}.
     *
     * @param dest The {@link Parcel} to be written.
     * @param flags The flags used for parceling.
     */
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetHashCode(
        /* [out] */ Int32* hc);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ICursorAnchorInfoBuilder* builder);

    /**
     * Returns the index where the selection starts.
     * @return {@code -1} if there is no selection.
     */
    CARAPI GetSelectionStart(
        /* [out] */ Int32* result);

    /**
     * Returns the index where the selection ends.
     * @return {@code -1} if there is no selection.
     */
    CARAPI GetSelectionEnd(
        /* [out] */ Int32* result);

    /**
     * Returns the index where the composing text starts.
     * @return {@code -1} if there is no composing text.
     */
    CARAPI GetComposingTextStart(
        /* [out] */ Int32* result);

    /**
     * Returns the entire composing text.
     * @return {@code null} if there is no composition.
     */
    CARAPI GetComposingText(
        /* [out] */ ICharSequence** result);

    /**
     * Returns the flag of the insertion marker.
     * @return the flag of the insertion marker. {@code 0} if no flag is specified.
     */
    CARAPI GetInsertionMarkerFlags(
        /* [out] */ Int32* result);

    /**
     * Returns the horizontal start of the insertion marker, in the local coordinates that will
     * be transformed with {@link #getMatrix()} when rendered on the screen.
     * @return x coordinate that is compatible with {@link Layout#getPrimaryHorizontal(int)}.
     * Pay special care to RTL/LTR handling.
     * {@code java.lang.Float.NaN} if not specified.
     * @see Layout#getPrimaryHorizontal(int)
     */
    CARAPI GetInsertionMarkerHorizontal(
        /* [out] */ Float* result);

    /**
     * Returns the vertical top position of the insertion marker, in the local coordinates that
     * will be transformed with {@link #getMatrix()} when rendered on the screen.
     * @return y coordinate that is compatible with {@link Layout#getLineTop(int)}.
     * {@code java.lang.Float.NaN} if not specified.
     */
    CARAPI GetInsertionMarkerTop(
        /* [out] */ Float* result);

    /**
     * Returns the vertical baseline position of the insertion marker, in the local coordinates
     * that will be transformed with {@link #getMatrix()} when rendered on the screen.
     * @return y coordinate that is compatible with {@link Layout#getLineBaseline(int)}.
     * {@code java.lang.Float.NaN} if not specified.
     */
    CARAPI GetInsertionMarkerBaseline(
        /* [out] */ Float* result);

    /**
     * Returns the vertical bottom position of the insertion marker, in the local coordinates
     * that will be transformed with {@link #getMatrix()} when rendered on the screen.
     * @return y coordinate that is compatible with {@link Layout#getLineBottom(int)}.
     * {@code java.lang.Float.NaN} if not specified.
     */
    CARAPI GetInsertionMarkerBottom(
        /* [out] */ Float* result);

    /**
     * Returns a new instance of {@link RectF} that indicates the location of the character
     * specified with the index.
     * @param index index of the character in a Java chars.
     * @return the character bounds in local coordinates as a new instance of {@link RectF}.
     */
    CARAPI GetCharacterBounds(
        /* [in] */ Int32 index,
        /* [out] */ IRectF** rect);

    /**
     * Returns the flags associated with the character bounds specified with the index.
     * @param index index of the character in a Java chars.
     * @return {@code 0} if no flag is specified.
     */
    CARAPI GetCharacterBoundsFlags(
        /* [in] */ Int32 index,
        /* [out] */ Int32* result);

    /**
     * Returns a new instance of {@link android.graphics.Matrix} that indicates the transformation
     * matrix that is to be applied other positional data in this class.
     * @return a new instance (copy) of the transformation matrix.
     */
    CARAPI GetMatrix(
        /* [out] */ IMatrix** matrix);

private:
    /**
     * Compares two float values. Returns {@code true} if {@code a} and {@code b} are
     * {@link Float#NaN} at the same time.
     */
    static CARAPI_(Boolean) AreSameFloatImpl(
        /* [in] */ Float a,
        /* [in] */ Float b);

private:
    friend class Elastos::Droid::Os::AndroidParcelUtils;

    static const String TAG;
    /**
     * The index of the first character of the selected text (inclusive). {@code -1} when there is
     * no text selection.
     */
    Int32 mSelectionStart;

    /**
     * The index of the first character of the selected text (exclusive). {@code -1} when there is
     * no text selection.
     */
    Int32 mSelectionEnd;

    /**
     * The index of the first character of the composing text (inclusive). {@code -1} when there is
     * no composing text.
     */
    Int32 mComposingTextStart;

    /**
     * The text, tracked as a composing region.
     */
    AutoPtr<ICharSequence> mComposingText;

    /**
     * Flags of the insertion marker. See {@link #FLAG_HAS_VISIBLE_REGION} for example.
     */
    Int32 mInsertionMarkerFlags;

    /**
     * Horizontal position of the insertion marker, in the local coordinates that will be
     * transformed with the transformation matrix when rendered on the screen. This should be
     * calculated or compatible with {@link Layout#getPrimaryHorizontal(int)}. This can be
     * {@code java.lang.Float.NaN} when no value is specified.
     */
    Float mInsertionMarkerHorizontal;

    /**
     * Vertical position of the insertion marker, in the local coordinates that will be
     * transformed with the transformation matrix when rendered on the screen. This should be
     * calculated or compatible with {@link Layout#getLineTop(int)}. This can be
     * {@code java.lang.Float.NaN} when no value is specified.
     */
    Float mInsertionMarkerTop;

    /**
     * Vertical position of the insertion marker, in the local coordinates that will be
     * transformed with the transformation matrix when rendered on the screen. This should be
     * calculated or compatible with {@link Layout#getLineBaseline(int)}. This can be
     * {@code java.lang.Float.NaN} when no value is specified.
     */
    Float mInsertionMarkerBaseline;

    /**
     * Vertical position of the insertion marker, in the local coordinates that will be
     * transformed with the transformation matrix when rendered on the screen. This should be
     * calculated or compatible with {@link Layout#getLineBottom(int)}. This can be
     * {@code java.lang.Float.NaN} when no value is specified.
     */
    Float mInsertionMarkerBottom;

    /**
     * Container of rectangular position of characters, keyed with character index in a unit of
     * Java chars, in the local coordinates that will be transformed with the transformation matrix
     * when rendered on the screen.
     */
    AutoPtr<ISparseRectFArray> mCharacterBoundsArray;

    /**
     * Transformation matrix that is applied to any positional information of this class to
     * transform local coordinates into screen coordinates.
     */
    AutoPtr<IMatrix> mMatrix;
};

} // namespace InputMethod
} // namespace View
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_VIEW_INPUTMETHOD_CCURSORANCHORINFO_H__
