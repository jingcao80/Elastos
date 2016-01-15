#ifndef __ELASTOS_DROID_TEXT_DYNAMICLAYOUT_H__
#define __ELASTOS_DROID_TEXT_DYNAMICLAYOUT_H__

#include "elastos/droid/text/Layout.h"
#include "elastos/droid/text/PackedIntVector.h"
#include "elastos/droid/text/PackedObjectVector.h"
#include "elastos/droid/text/CStaticLayout.h"

namespace Elastos {
namespace Droid {
namespace Text {

/**
 * DynamicLayout is a text layout that updates itself as the text is edited.
 * <p>This is used by widgets to control text layout. You should not need
 * to use this class directly unless you are implementing your own widget
 * or custom display object, or need to call
 * {@link android.graphics.Canvas#drawText(java.lang.CharSequence, int, int, float, float, android.graphics.Paint)
 *  Canvas.drawText()} directly.</p>
 */
class DynamicLayout
    : public Layout
    , public IDynamicLayout
{
private:
    class ChangeWatcher
        : public Object
        , public ITextWatcher
        , public ISpanWatcher
        , public INoCopySpan
    {
    public:
        CAR_INTERFACE_DECL()

        ChangeWatcher(
            /* [in] */ IWeakReference* layout);

        CARAPI BeforeTextChanged(
            /* [in] */ ICharSequence* s,
            /* [in] */ Int32 where,
            /* [in] */ Int32 before,
            /* [in] */ Int32 after);

        CARAPI OnTextChanged(
            /* [in] */ ICharSequence* s,
            /* [in] */ Int32 where,
            /* [in] */ Int32 before,
            /* [in] */ Int32 after);

        CARAPI AfterTextChanged(
            /* [in] */ IEditable* s);

        CARAPI OnSpanAdded(
            /* [in] */ ISpannable* s,
            /* [in] */ IInterface* o,
            /* [in] */ Int32 start,
            /* [in] */ Int32 end);

        CARAPI OnSpanRemoved(
            /* [in] */ ISpannable* s,
            /* [in] */ IInterface* o,
            /* [in] */ Int32 start,
            /* [in] */ Int32 end);

        CARAPI OnSpanChanged(
            /* [in] */ ISpannable* s,
            /* [in] */ IInterface* o,
            /* [in] */ Int32 start,
            /* [in] */ Int32 end,
            /* [in] */ Int32 nstart,
            /* [in] */ Int32 nend);

    private:
        CARAPI_(void) Reflow(
            /* [in] */ ICharSequence* s,
            /* [in] */ Int32 where,
            /* [in] */ Int32 before,
            /* [in] */ Int32 after);

    private:
//        WeakReference<DynamicLayout> mLayout;
        AutoPtr<IWeakReference> mLayout;
    };

public:
    CAR_INTERFACE_DECL()

    DynamicLayout();

    /**
     * Make a layout for the specified text that will be updated as
     * the text is changed.
     */
    CARAPI constructor(
        /* [in] */ ICharSequence* base,
        /* [in] */ ITextPaint* paint,
        /* [in] */ Int32 width,
        /* [in] */ LayoutAlignment align,
        /* [in] */ Float spacingmult,
        /* [in] */ Float spacingadd,
        /* [in] */ Boolean includepad);

    /**
     * Make a layout for the transformed text (password transformation
     * being the primary example of a transformation)
     * that will be updated as the base text is changed.
     */
    CARAPI constructor(
        /* [in] */ ICharSequence* base,
        /* [in] */ ICharSequence* display,
        /* [in] */ ITextPaint* paint,
        /* [in] */ Int32 width,
        /* [in] */ LayoutAlignment align,
        /* [in] */ Float spacingmult,
        /* [in] */ Float spacingadd,
        /* [in] */ Boolean includepad);

    /**
     * Make a layout for the transformed text (password transformation
     * being the primary example of a transformation)
     * that will be updated as the base text is changed.
     * If ellipsize is non-null, the Layout will ellipsize the text
     * down to ellipsizedWidth.
     */
    CARAPI constructor(
        /* [in] */ ICharSequence* base,
        /* [in] */ ICharSequence* display,
        /* [in] */ ITextPaint* paint,
        /* [in] */ Int32 width,
        /* [in] */ LayoutAlignment align,
        /* [in] */ Float spacingmult,
        /* [in] */ Float spacingadd,
        /* [in] */ Boolean includepad,
        /* [in] */ TextUtilsTruncateAt ellipsize,
        /* [in] */ Int32 ellipsizedWidth);

    /**
     * Make a layout for the transformed text (password transformation
     * being the primary example of a transformation)
     * that will be updated as the base text is changed.
     * If ellipsize is non-null, the Layout will ellipsize the text
     * down to ellipsizedWidth.
     * *
     * *@hide
     */
    CARAPI constructor(
        /* [in] */ ICharSequence* base,
        /* [in] */ ICharSequence* display,
        /* [in] */ ITextPaint* paint,
        /* [in] */ Int32 width,
        /* [in] */ LayoutAlignment align,
        /* [in] */ ITextDirectionHeuristic* textDir,
        /* [in] */ Float spacingmult,
        /* [in] */ Float spacingadd,
        /* [in] */ Boolean includepad,
        /* [in] */ TextUtilsTruncateAt ellipsize,
        /* [in] */ Int32 ellipsizedWidth);

public:
    /**
     * This method is called every time the layout is reflowed after an edition.
     * It updates the internal block data structure. The text is split in blocks
     * of contiguous lines, with at least one block for the entire text.
     * When a range of lines is edited, new blocks (from 0 to 3 depending on the
     * overlap structure) will replace the set of overlapping blocks.
     * Blocks are listed in order and are represented by their ending line number.
     * An index is associated to each block (which will be used by display lists),
     * this class simply invalidates the index of blocks overlapping a modification.
     *
     * This method is package private and not private so that it can be tested.
     *
     * @param startLine the first line of the range of modified lines
     * @param endLine the last line of the range, possibly equal to startLine, lower
     * than getLineCount()
     * @param newLineCount the number of lines that will replace the range, possibly 0
     *
     * @hide
     */
    CARAPI UpdateBlocks(
        /* [in] */ Int32 startLine,
        /* [in] */ Int32 endLine,
        /* [in] */ Int32 newLineCount);

    /**
     * This package private method is used for test purposes only
     * @hide
     */
    CARAPI SetBlocksDataForTest(
        /* [in] */ ArrayOf<Int32>* blockEndLines,
        /* [in] */ ArrayOf<Int32>* blockIndices,
        /* [in] */ Int32 numberOfBlocks);

    /**
     * @hide
     */
    CARAPI GetBlockEndLines(
        /* [out, calllee] */ ArrayOf<Int32>** lines);

    /**
     * @hide
     */
    CARAPI GetBlockIndices(
        /* [out, calllee] */ ArrayOf<Int32>** indices);

    /**
     * @hide
     */
    CARAPI GetNumberOfBlocks(
        /* [out] */ Int32* result);

    /**
     * @hide
     */
    CARAPI GetIndexFirstChangedBlock(
        /* [out] */ Int32* result);

    /**
     * @hide
     */
    CARAPI SetIndexFirstChangedBlock(
        /* [in] */ Int32 block);

    //@Override
    CARAPI GetLineCount(
        /* [out] */ Int32* result);

    //@Override
    CARAPI GetLineTop(
        /* [in] */ Int32 line,
        /* [out] */ Int32* result);

    //@Override
    CARAPI GetLineDescent(
        /* [in] */ Int32 line,
        /* [out] */ Int32* result);

    //@Override
    CARAPI GetLineStart(
        /* [in] */ Int32 line,
        /* [out] */ Int32* result);

    //@Override
    CARAPI GetLineContainsTab(
        /* [in] */ Int32 line,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI GetParagraphDirection(
        /* [in] */ Int32 line,
        /* [out] */ Int32* result);

    //@Override
    CARAPI GetLineDirections(
        /* [in] */ Int32 line,
        /* [out] */ ILayoutDirections** result);

    //@Override
    CARAPI GetTopPadding(
        /* [out] */ Int32* result);

    //@Override
    CARAPI GetBottomPadding(
        /* [out] */ Int32* result);

    //@Override
    CARAPI GetEllipsizedWidth(
        /* [out] */ Int32* result);

    //@Override
    CARAPI GetEllipsisStart(
        /* [in] */ Int32 line,
        /* [out] */ Int32* result);

    //@Override
    CARAPI GetEllipsisCount(
        /* [in] */ Int32 line,
        /* [out] */ Int32* result);

private:
    CARAPI_(void) Reflow(
        /* [in] */ ICharSequence* s,
        /* [in] */ Int32 where,
        /* [in] */ Int32 before,
        /* [in] */ Int32 after);

    /**
     * Create the initial block structure, cutting the text into blocks of at least
     * BLOCK_MINIMUM_CHARACTER_SIZE characters, aligned on the ends of paragraphs.
     */
    CARAPI_(void) CreateBlocks();

    /**
     * Create a new block, ending at the specified character offset.
     * A block will actually be created only if has at least one line, i.e. this offset is
     * not on the end line of the previous block.
     */
    CARAPI_(void) AddBlockAtOffset(
        /* [in] */ Int32 offset);

    static AutoPtr<CStaticLayout> GetStaticLayout();

    static CARAPI_(void) SetStaticLayout(
        /* [in] */ CStaticLayout* layout);

public:
    /**
     * Value used in mBlockIndices when a block has been created or recycled and indicating that its
     * display list needs to be re-created.
     * @hide
     */
    static const Int32 INVALID_BLOCK_INDEX = -1;

private:
    static const Int32 PRIORITY = 128;
    static const Int32 BLOCK_MINIMUM_CHARACTER_LENGTH = 400;

    AutoPtr<ICharSequence> mBase;
    AutoPtr<ICharSequence> mDisplay;
    AutoPtr<ChangeWatcher> mWatcher;
    Boolean mIncludePad;
    Boolean mEllipsize;
    Int32 mEllipsizedWidth;
    TextUtilsTruncateAt mEllipsizeAt;

    AutoPtr<PackedIntVector> mInts;
    AutoPtr< PackedObjectVector<ILayoutDirections> > mObjects;

    // Stores the line numbers of the last line of each block (inclusive)
    AutoPtr< ArrayOf<Int32> > mBlockEndLines;
    // The indices of this block's display list in TextView's internal display list array or
    // INVALID_BLOCK_INDEX if this block has been invalidated during an edition
    AutoPtr< ArrayOf<Int32> > mBlockIndices;
    // Number of items actually currently being used in the above 2 arrays
    Int32 mNumberOfBlocks;
    // The first index of the blocks whose locations are changed
    Int32 mIndexFirstChangedBlock;

    Int32 mTopPadding, mBottomPadding;

    static AutoPtr<CStaticLayout> sStaticLayout;
    static Boolean sIsStaticLayoutInited;

    static Object sLock;

    static const Int32 START = 0;
    static const Int32 DIR = START;
    static const Int32 TAB = START;
    static const Int32 TOP = 1;
    static const Int32 DESCENT = 2;
    static const Int32 COLUMNS_NORMAL = 3;

    static const Int32 ELLIPSIS_START = 3;
    static const Int32 ELLIPSIS_COUNT = 4;
    static const Int32 COLUMNS_ELLIPSIZE = 5;

    static const Int32 START_MASK = 0x1FFFFFFF;
    static const Int32 DIR_SHIFT  = 30;
    static const Int32 TAB_MASK   = 0x20000000;

    static const Int32 ELLIPSIS_UNDEFINED = 0x80000000;
};

} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TEXT_DYNAMICLAYOUT_H__
