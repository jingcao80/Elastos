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

#ifndef __ELASTOS_DROID_GRAPHICS_CATLAS_H__
#define __ELASTOS_DROID_GRAPHICS_CATLAS_H__

#include "_Elastos_Droid_Graphics_CAtlas.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CAtlas)
    , public Object
    , public IAtlas
{
private:
    /**
     * A policy defines how the atlas performs the packing operation.
     */
    class Policy
        : public Object
    {
    public:
        virtual ~Policy() {}

        virtual AutoPtr<IAtlasEntry> Pack(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height,
            /* [in] */ IAtlasEntry* entry) = 0;
    };

    /**
     * The Slice algorightm divides the remaining empty space either
     * horizontally or vertically after a bitmap is placed in the atlas.
     *
     * NOTE: the algorithm is explained below using a tree but is
     * implemented using a linked list instead for performance reasons.
     *
     * The algorithm starts with a single empty cell covering the entire
     * atlas:
     *
     *  -----------------------
     * |                       |
     * |                       |
     * |                       |
     * |      Empty space      |
     * |          (C0)         |
     * |                       |
     * |                       |
     * |                       |
     *  -----------------------
     *
     * The tree of cells looks like this:
     *
     * N0(free)
     *
     * The algorithm then places a bitmap B1, if possible:
     *
     *  -----------------------
     * |        |              |
     * |   B1   |              |
     * |        |              |
     * |--------               |
     * |                       |
     * |                       |
     * |                       |
     * |                       |
     *  -----------------------
     *
     *  After placing a bitmap in an empty cell, the algorithm splits
     *  the remaining space in two new empty cells. The split can occur
     *  vertically or horizontally (this is controlled by the "split
     *  decision" parameter of the algorithm.)
     *
     *  Here is for the instance the result of a vertical split:
     *
     *  -----------------------
     * |        |              |
     * |   B1   |              |
     * |        |              |
     * |--------|      C2      |
     * |        |              |
     * |        |              |
     * |   C1   |              |
     * |        |              |
     *  -----------------------
     *
     * The cells tree now looks like this:
     *
     *       C0(occupied)
     *           / \
     *          /   \
     *         /     \
     *        /       \
     *    C1(free)  C2(free)
     *
     * For each bitmap to place in the atlas, the Slice algorithm
     * will visit the free cells until it finds one where a bitmap can
     * fit. It will then split the now occupied cell and proceed onto
     * the next bitmap.
     */
    class SlicePolicy
        : public Policy
    {
    private:
        /**
         * Defines how the remaining empty space should be split up:
         * vertically or horizontally.
         */
        class SplitDecision
            : public Object
        {
        public:
            virtual ~SplitDecision() {}

            /**
             * Returns true if the remaining space defined by
             * <code>freeWidth</code> and <code>freeHeight</code>
             * should be split horizontally.
             *
             * @param freeWidth The rectWidth of the free space after packing a rectangle
             * @param freeHeight The rectHeight of the free space after packing a rectangle
             * @param rectWidth The rectWidth of the rectangle that was packed in a cell
             * @param rectHeight The rectHeight of the rectangle that was packed in a cell
             */
            virtual Boolean SplitHorizontal(
                /* [in] */ Int32 freeWidth,
                /* [in] */ Int32 freeHeight,
                /* [in] */ Int32 rectWidth,
                /* [in] */ Int32 rectHeight) = 0;
        };

        // Splits the free area horizontally to minimize the horizontal section area
        class MinAreaSplitDecision
            : public SplitDecision
        {
        public:
            TO_STRING_IMPL("CAtlas::MinAreaSplitDecision")

            Boolean SplitHorizontal(
                /* [in] */ Int32 freeWidth,
                /* [in] */ Int32 freeHeight,
                /* [in] */ Int32 rectWidth,
                /* [in] */ Int32 rectHeight);
        };

        // Splits the free area horizontally to maximize the horizontal section area
        class MaxAreaSplitDecision
            : public SplitDecision
        {
        public:
            TO_STRING_IMPL("CAtlas::MaxAreaSplitDecision")

            Boolean SplitHorizontal(
                /* [in] */ Int32 freeWidth,
                /* [in] */ Int32 freeHeight,
                /* [in] */ Int32 rectWidth,
                /* [in] */ Int32 rectHeight);
        };

        // Splits the free area horizontally if the horizontal axis is shorter
        class ShorterFreeAxisSplitDecision
            : public SplitDecision
        {
        public:
            TO_STRING_IMPL("CAtlas::ShorterFreeAxisSplitDecision")

            Boolean SplitHorizontal(
                /* [in] */ Int32 freeWidth,
                /* [in] */ Int32 freeHeight,
                /* [in] */ Int32 rectWidth,
                /* [in] */ Int32 rectHeight);
        };

        // Splits the free area horizontally if the vertical axis is shorter
        class LongerFreeAxisSplitDecision
            : public SplitDecision
        {
        public:
            TO_STRING_IMPL("CAtlas::LongerFreeAxisSplitDecision")

            Boolean SplitHorizontal(
                /* [in] */ Int32 freeWidth,
                /* [in] */ Int32 freeHeight,
                /* [in] */ Int32 rectWidth,
                /* [in] */ Int32 rectHeight);
        };

        /**
         * A cell represents a sub-rectangle of the atlas. A cell is
         * a node in a linked list representing the available free
         * space in the atlas.
         */
        class Cell
            : public Object
        {
        public:
            Int32 mX;
            Int32 mY;

            Int32 mWidth;
            Int32 mHeight;

            AutoPtr<Cell> mNext;

            Cell();

            CARAPI ToString(
                /* [in] */ String* str);
        };

    public:
        SlicePolicy(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height,
            /* [in] */ Int32 flags,
            /* [in] */ SplitDecision* splitDecision);

        AutoPtr<IAtlasEntry> Pack(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height,
            /* [in] */ IAtlasEntry* entry);

    private:
        /**
         * Attempts to pack a rectangle of specified dimensions in the available
         * empty space.
         *
         * @param cell The cell representing free space in which to pack the rectangle
         * @param prev The previous cell in the free space linked list
         * @param width The width of the rectangle to pack
         * @param height The height of the rectangle to pack
         * @param entry Stores the location of the packged rectangle, if it fits
         *
         * @return True if the rectangle was packed in the atlas, false otherwise
         */
        Boolean Insert(
            /* [in] */ Cell* cell,
            /* [in] */ Cell* prev,
            /* [in] */ Int32 width,
            /* [in] */ Int32 height,
            /* [in] */ IAtlasEntry* entry);

    private:
        friend class CAtlas;

        AutoPtr<Cell> mRoot;// = new Cell();

        AutoPtr<SplitDecision> mSplitDecision;

        Boolean mAllowRotation;
        Int32 mPadding;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Creates a new atlas with the specified algorithm and dimensions
     * in pixels. Calling this constructor is equivalent to calling
     * {@link #Atlas(Atlas.Type, int, int, int)} with {@link #FLAG_DEFAULTS}.
     *
     * @param type The algorithm to use to pack rectangles in the atlas
     * @param width The width of the atlas in pixels
     * @param height The height of the atlas in pixels
     *
     * @see #Atlas(Atlas.Type, int, int, int)
     */
    CARAPI constructor(
        /* [in] */ AtlasType type,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    /**
     * Creates a new atlas with the specified algorithm and dimensions
     * in pixels. A set of flags can also be specified to control the
     * behavior of the atlas.
     *
     * @param type The algorithm to use to pack rectangles in the atlas
     * @param width The width of the atlas in pixels
     * @param height The height of the atlas in pixels
     * @param flags Optional flags to control the behavior of the atlas:
     *              {@link #FLAG_ADD_PADDING}, {@link #FLAG_ALLOW_ROTATIONS}
     *
     * @see #Atlas(Atlas.Type, int, int)
     */
    CARAPI constructor(
        /* [in] */ AtlasType type,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 flag);

    /**
     * Packs a rectangle of the specified dimensions in this atlas.
     *
     * @param width The width of the rectangle to pack in the atlas
     * @param height The height of the rectangle to pack in the atlas
     *
     * @return An {@link Entry} instance if the rectangle was packed in
     *         the atlas, or null if the rectangle could not fit
     *
     * @see #pack(int, int, Atlas.Entry)
     */
    CARAPI Pack(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [out] */ IAtlasEntry** entry);

    /**
     * Packs a rectangle of the specified dimensions in this atlas.
     *
     * @param width The width of the rectangle to pack in the atlas
     * @param height The height of the rectangle to pack in the atlas
     * @param entry Out parameter that will be filled in with the location
     *              and attributes of the packed rectangle, can be null
     *
     * @return An {@link Entry} instance if the rectangle was packed in
     *         the atlas, or null if the rectangle could not fit
     *
     * @see #pack(int, int)
     */
    CARAPI Pack(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ IAtlasEntry* inEntry,
        /* [out] */ IAtlasEntry** outEntry);

private:
    static AutoPtr<CAtlas::Policy> FindPolicy(
        /* [in] */ AtlasType type,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 flag);

private:
    AutoPtr<Policy> mPolicy;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_CATLAS_H__
