
#include "elastos/droid/text/DynamicLayout.h"
#include "elastos/droid/text/CDynamicLayout.h"
#include "elastos/droid/text/TextDirectionHeuristics.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/text/CStaticLayout.h"
#include "elastos/droid/internal/utility/ArrayUtils.h"
#include "elastos/droid/internal/utility/GrowingArrayUtils.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>

using Elastos::Droid::Graphics::IPaintFontMetricsInt;
using Elastos::Droid::Text::Style::IUpdateLayout;
using Elastos::Droid::Text::Style::EIID_IWrapTogetherSpan;
using Elastos::Droid::Internal::Utility::ArrayUtils;
using Elastos::Droid::Internal::Utility::GrowingArrayUtils;

namespace Elastos {
namespace Droid {
namespace Text {

CAR_INTERFACE_IMPL_3(DynamicLayout::ChangeWatcher, Object, ITextWatcher, ISpanWatcher, INoCopySpan)

DynamicLayout::ChangeWatcher::ChangeWatcher(
    /* [in] */ IWeakReference* layout)
    : mLayout(layout)
{
}

void DynamicLayout::ChangeWatcher::Reflow(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 where,
    /* [in] */ Int32 before,
    /* [in] */ Int32 after)
{
    AutoPtr<IDynamicLayout> dl;
    mLayout->Resolve(EIID_IDynamicLayout, (IInterface**)&dl);

    if (dl != NULL) {
        DynamicLayout* layout = (DynamicLayout*)dl.Get();
        layout->Reflow(s, where, before, after);
    }
    else if (s != NULL && ISpannable::Probe(s) != NULL) {
        ISpannable::Probe(s)->RemoveSpan(TO_IINTERFACE(this));
    }
}

ECode DynamicLayout::ChangeWatcher::BeforeTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 where,
    /* [in] */ Int32 before,
    /* [in] */ Int32 after)
{
    // Intentionally empty
    return NOERROR;
}

ECode DynamicLayout::ChangeWatcher::OnTextChanged(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 where,
    /* [in] */ Int32 before,
    /* [in] */ Int32 after)
{
    Reflow(s, where, before, after);
    return NOERROR;
}

ECode DynamicLayout::ChangeWatcher::AfterTextChanged(
    /* [in] */ IEditable* s)
{
    // Intentionally empty
    return NOERROR;
}

ECode DynamicLayout::ChangeWatcher::OnSpanAdded(
    /* [in] */ ISpannable* s,
    /* [in] */ IInterface* o,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    if (o != NULL && IUpdateLayout::Probe(o) != NULL) {
        Reflow(ICharSequence::Probe(s), start, end - start, end - start);
    }
    return NOERROR;
}

ECode DynamicLayout::ChangeWatcher::OnSpanRemoved(
    /* [in] */ ISpannable* s,
    /* [in] */ IInterface* o,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    if (o != NULL && IUpdateLayout::Probe(o) != NULL) {
        Reflow(ICharSequence::Probe(s), start, end - start, end - start);
    }
    return NOERROR;
}

ECode DynamicLayout::ChangeWatcher::OnSpanChanged(
    /* [in] */ ISpannable* s,
    /* [in] */ IInterface* o,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ Int32 nstart,
    /* [in] */ Int32 nend)
{
    if (o != NULL && IUpdateLayout::Probe(o) != NULL) {
        Reflow(ICharSequence::Probe(s), start, end - start, end - start);
        Reflow(ICharSequence::Probe(s), nstart, nend - nstart, nend - nstart);
    }
    return NOERROR;
}


const Int32 DynamicLayout::PRIORITY;
const Int32 DynamicLayout::BLOCK_MINIMUM_CHARACTER_LENGTH;
const Int32 DynamicLayout::INVALID_BLOCK_INDEX;

Boolean DynamicLayout::sIsStaticLayoutInited = FALSE;
AutoPtr<CStaticLayout> DynamicLayout::sStaticLayout;
Object DynamicLayout::sLock;

const Int32 DynamicLayout::START;
const Int32 DynamicLayout::DIR;
const Int32 DynamicLayout::TAB;
const Int32 DynamicLayout::TOP;
const Int32 DynamicLayout::DESCENT;
const Int32 DynamicLayout::COLUMNS_NORMAL;
const Int32 DynamicLayout::ELLIPSIS_START;
const Int32 DynamicLayout::ELLIPSIS_COUNT;
const Int32 DynamicLayout::COLUMNS_ELLIPSIZE;
const Int32 DynamicLayout::START_MASK;
const Int32 DynamicLayout::DIR_SHIFT;
const Int32 DynamicLayout::TAB_MASK;
const Int32 DynamicLayout::ELLIPSIS_UNDEFINED;

CAR_INTERFACE_IMPL(DynamicLayout, Layout, IDynamicLayout)

AutoPtr<CStaticLayout> DynamicLayout::GetStaticLayout()
{
    if (!sIsStaticLayoutInited) {
        assert(sStaticLayout == NULL);
        sIsStaticLayoutInited = TRUE;
        CStaticLayout::NewByFriend(NULL, (CStaticLayout**)&sStaticLayout);
    }
    return sStaticLayout;
}

void DynamicLayout::SetStaticLayout(
    /* [in] */ CStaticLayout* layout)
{
    sStaticLayout = layout;
}

DynamicLayout::DynamicLayout()
    : mIncludePad(FALSE)
    , mEllipsize(FALSE)
    , mEllipsizedWidth(0)
    , mNumberOfBlocks(0)
    , mIndexFirstChangedBlock(0)
    , mTopPadding(0)
    , mBottomPadding(0)
{}

/**
 * Make a layout for the specified text that will be updated as
 * the text is changed.
 */
ECode DynamicLayout::constructor(
    /* [in] */ ICharSequence* base,
    /* [in] */ ITextPaint* paint,
    /* [in] */ Int32 width,
    /* [in] */ LayoutAlignment align,
    /* [in] */ Float spacingmult,
    /* [in] */ Float spacingadd,
    /* [in] */ Boolean includepad)
{
    return constructor(base, base, paint, width, align, spacingmult, spacingadd,
        includepad);
}

/**
 * Make a layout for the transformed text (password transformation
 * being the primary example of a transformation)
 * that will be updated as the base text is changed.
 */
ECode DynamicLayout::constructor(
    /* [in] */ ICharSequence* base,
    /* [in] */ ICharSequence* display,
    /* [in] */ ITextPaint* paint,
    /* [in] */ Int32 width,
    /* [in] */ LayoutAlignment align,
    /* [in] */ Float spacingmult,
    /* [in] */ Float spacingadd,
    /* [in] */ Boolean includepad)
{
    return constructor(base, display, paint, width, align, spacingmult, spacingadd,
        includepad, -1, 0);
}

/**
 * Make a layout for the transformed text (password transformation
 * being the primary example of a transformation)
 * that will be updated as the base text is changed.
 * If ellipsize is non-null, the Layout will ellipsize the text
 * down to ellipsizedWidth.
 */
ECode DynamicLayout::constructor(
    /* [in] */ ICharSequence* base,
    /* [in] */ ICharSequence* display,
    /* [in] */ ITextPaint* paint,
    /* [in] */ Int32 width,
    /* [in] */ LayoutAlignment align,
    /* [in] */ Float spacingmult,
    /* [in] */ Float spacingadd,
    /* [in] */ Boolean includepad,
    /* [in] */ TextUtilsTruncateAt ellipsize,
    /* [in] */ Int32 ellipsizedWidth)
{
    AutoPtr<ITextDirectionHeuristic> ltr = TextDirectionHeuristics::FIRSTSTRONG_LTR;
    return constructor(base, display, paint, width, align, ltr,
        spacingmult, spacingadd, includepad, ellipsize, ellipsizedWidth);
}

/**
 * Make a layout for the transformed text (password transformation
 * being the primary example of a transformation)
 * that will be updated as the base text is changed.
 * If ellipsize is non-null, the Layout will ellipsize the text
 * down to ellipsizedWidth.
 * *
 * *@hide
 */
ECode DynamicLayout::constructor(
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
    /* [in] */ Int32 ellipsizedWidth)
{
    IPaint* p = IPaint::Probe(paint);
    AutoPtr<ICharSequence> paramCS;
    if (ellipsize == TextUtilsTruncateAt_NONE) {
        paramCS = display;
    }
    else {
        if (display != NULL && ISpanned::Probe(display)) {
            paramCS = (Ellipsizer*)new SpannedEllipsizer(display);
        }
        else {
            paramCS = new Ellipsizer(display);
        }
    }
    FAIL_RETURN(Layout::constructor(paramCS,
          paint, width, align, textDir, spacingmult, spacingadd))

    mBase = base;
    mDisplay = display;

    if (ellipsize != TextUtilsTruncateAt_NONE) {
        mInts = new PackedIntVector(COLUMNS_ELLIPSIZE);
        mEllipsizedWidth = ellipsizedWidth;
        mEllipsizeAt = ellipsize;
    }
    else {
        mInts = new PackedIntVector(COLUMNS_NORMAL);
        mEllipsizedWidth = width;
        mEllipsizeAt = TextUtilsTruncateAt_NONE;
    }

    mObjects = new PackedObjectVector<ILayoutDirections>(1);

    mIncludePad = includepad;

    /*
     * This is annoying, but we can't refer to the layout until
     * superclass construction is finished, and the superclass
     * constructor wants the reference to the display text.
     *
     * This will break if the superclass constructor ever actually
     * cares about the content instead of just holding the reference.
     */
    if (ellipsize != TextUtilsTruncateAt_NONE) {
        AutoPtr<ICharSequence> csq;
        GetText((ICharSequence**)&csq);
        AutoPtr<Ellipsizer> e = (Ellipsizer*)csq.Get();
        e->mLayout = this;
        e->mWidth = ellipsizedWidth;
        e->mMethod = ellipsize;
        mEllipsize = TRUE;
    }

    // Initial state is a single line with 0 characters (0 to 0),
    // with top at 0 and bottom at whatever is natural, and
    // undefined ellipsis.

    AutoPtr< ArrayOf<Int32> > start;

    if (ellipsize != TextUtilsTruncateAt_NONE) {
        start = ArrayOf<Int32>::Alloc(COLUMNS_ELLIPSIZE);
        (*start)[ELLIPSIS_START] = ELLIPSIS_UNDEFINED;
    }
    else {
        start = ArrayOf<Int32>::Alloc(COLUMNS_NORMAL);
    }

    //Directions[] dirs = new Directions[] { DIRS_ALL_LEFT_TO_RIGHT };
    AutoPtr< ArrayOf<ILayoutDirections*> > dirs = ArrayOf<ILayoutDirections*>::Alloc(1);
    dirs->Set(0, DIRS_ALL_LEFT_TO_RIGHT);

    AutoPtr<IPaintFontMetricsInt> fm;
    p->GetFontMetricsInt((IPaintFontMetricsInt**)&fm);
    Int32 asc, desc;
    fm->GetAscent(&asc);
    fm->GetDescent(&desc);

    (*start)[DIR] = ILayout::DIR_LEFT_TO_RIGHT << DIR_SHIFT;
    (*start)[TOP] = 0;
    (*start)[DESCENT] = desc;
    mInts->InsertAt(0, start);

    (*start)[TOP] = desc - asc;
    mInts->InsertAt(1, start);

    mObjects->InsertAt(0, dirs);
    // Update from 0 characters to whatever the real text is
    Int32 len;
    base->GetLength(&len);
    Reflow(base, 0, 0, len);

    if (ISpannable::Probe(base) != NULL) {
        if (mWatcher == NULL) {
            IWeakReferenceSource* wrs = this;
            AutoPtr<IWeakReference> wr;
            wrs->GetWeakReference((IWeakReference**)&wr);
            mWatcher = new ChangeWatcher(wr);
        }

        // Strip out any watchers for other DynamicLayouts.
        AutoPtr<ISpannable> sp = ISpannable::Probe(base);
        ICharSequence::Probe(sp)->GetLength(&len);
        AutoPtr< ArrayOf<IInterface*> > spans;
        //ChangeWatcher
        ISpanned::Probe(sp)->GetSpans(0, len, EIID_ITextWatcher, (ArrayOf<IInterface*>**)&spans);
        for (Int32 i = 0; i < spans->GetLength(); i++) {
            sp->RemoveSpan((*spans)[i]);
        }

        base->GetLength(&len);
        sp->SetSpan(mWatcher->Probe(EIID_IInterface), 0, len,
                   ISpanned::SPAN_INCLUSIVE_INCLUSIVE |
                   (PRIORITY << ISpanned::SPAN_PRIORITY_SHIFT));
    }

    return NOERROR;
}

void DynamicLayout::Reflow(
    /* [in] */ ICharSequence* s,
    /* [in] */ Int32 where,
    /* [in] */ Int32 before,
    /* [in] */ Int32 after)
{
    if (s != mBase) {
        return;
    }

    AutoPtr<ICharSequence> text = mDisplay;
    Int32 len;
    text->GetLength(&len);

    // seek back to the start of the paragraph
    Int32 find = TextUtils::LastIndexOf(text, '\n', where - 1);
    if (find < 0) {
        find = 0;
    }
    else {
        find = find + 1;
    }

    {
        Int32 diff = where - find;
        before += diff;
        after += diff;
        where -= diff;
    }

    // seek forward to the end of the paragraph

    Int32 look = TextUtils::IndexOf(text, '\n', where + after);
    if (look < 0) {
        look = len;
    }
    else {
        look++; // we want the index after the \n
    }

    Int32 change = look - (where + after);
    before += change;
    after += change;

    // seek further out to cover anything that is forced to wrap together

    if (ISpanned::Probe(text) != NULL) {
        AutoPtr<ISpanned> sp = ISpanned::Probe(text);
        Boolean again;

        do {
            again = FALSE;

            AutoPtr< ArrayOf<IInterface*> > force;
            sp->GetSpans(where, where + after,
                    EIID_IWrapTogetherSpan, (ArrayOf<IInterface*>**)&force);

            for (Int32 i = 0; i < force->GetLength(); i++) {
                Int32 st, en;
                sp->GetSpanStart((*force)[i], &st);
                sp->GetSpanEnd((*force)[i], &en);

                if (st < where) {
                    again = TRUE;

                    Int32 diff = where - st;
                    before += diff;
                    after += diff;
                    where -= diff;
                }

                if (en > where + after) {
                    again = TRUE;

                    Int32 diff = en - (where + after);
                    before += diff;
                    after += diff;
                }
            }
        } while(again);
    }

    // find affected region of old layout
    Int32 startline, startv, endline, count;
    GetLineForOffset(where, &startline);
    GetLineTop(startline, &startv);

    GetLineForOffset(where + before, &endline);
    if (where + after == len) {
        GetLineCount(&endline);
    }
    Int32 endv;
    GetLineTop(endline, &endv);
    GetLineCount(&count);
    Boolean islast = (endline == count);

    // generate new layout for affected text
    AutoPtr<CStaticLayout> reflowed;

    {
        AutoLock lock(&sLock);

        reflowed = GetStaticLayout();
        SetStaticLayout(NULL);
    }

    if (reflowed == NULL) {
        CStaticLayout::NewByFriend(NULL, (CStaticLayout**)&reflowed);
    }
    else {
        reflowed->Prepare();
    }

    AutoPtr<ITextPaint> tp;
    GetPaint((ITextPaint**)&tp);
    Int32 width;
    GetWidth(&width);
    AutoPtr<ITextDirectionHeuristic> dir;
    GetTextDirectionHeuristic((ITextDirectionHeuristic**)&dir);
    Float multiplier, spacingAdd;
    GetSpacingMultiplier(&multiplier);
    GetSpacingAdd(&spacingAdd);
    reflowed->Generate(text, where, where + after, tp, width, dir, multiplier,
        spacingAdd, FALSE, TRUE, mEllipsizedWidth, mEllipsizeAt);
    Int32 n;
    reflowed->GetLineCount(&n);

    // If the new layout has a blank line at the end, but it is not
    // the very end of the buffer, then we already have a line that
    // starts there, so disregard the blank line.
    Int32 offset;
    if (where + after != len && (reflowed->GetLineStart(n - 1, &offset), offset) == where + after) {
        n--;
    }

    // remove affected lines from old layout
    mInts->DeleteAt(startline, endline - startline);
    mObjects->DeleteAt(startline, endline - startline);

    // adjust offsets in layout for new height and offsets

    Int32 ht;
    reflowed->GetLineTop(n, &ht);
    Int32 toppad = 0, botpad = 0;

    if (mIncludePad && startline == 0) {
        reflowed->GetTopPadding(&toppad);
        mTopPadding = toppad;
        ht -= toppad;
    }
    if (mIncludePad && islast) {
        reflowed->GetBottomPadding(&botpad);
        mBottomPadding = botpad;
        ht += botpad;
    }

    mInts->AdjustValuesBelow(startline, START, after - before);
    mInts->AdjustValuesBelow(startline, TOP, startv - endv + ht);

    // insert new layout

    AutoPtr< ArrayOf<Int32> > ints;

    if (mEllipsize) {
        ints = ArrayOf<Int32>::Alloc(COLUMNS_ELLIPSIZE);
        (*ints)[ELLIPSIS_START] = ELLIPSIS_UNDEFINED;
    }
    else {
        ints = ArrayOf<Int32>::Alloc(COLUMNS_NORMAL);
    }

    AutoPtr< ArrayOf<ILayoutDirections*> > objects = ArrayOf<ILayoutDirections*>::Alloc(1);

    Int32 start, paraDir;
    Boolean bval;
    for (Int32 i = 0; i < n; i++) {
        reflowed->GetLineStart(i, &start);
        reflowed->GetParagraphDirection(i, &paraDir);
        reflowed->GetLineContainsTab(i, &bval);
        (*ints)[START] =  start | (paraDir << DIR_SHIFT) | (bval ? TAB_MASK : 0);

        Int32 top;
        reflowed->GetLineTop(i, &top);
        top += startv;
        if (i > 0) {
            top -= toppad;
        }
        (*ints)[TOP] = top;

        Int32 desc;
        reflowed->GetLineDescent(i, &desc);
        if (i == n - 1) {
            desc += botpad;
        }

        (*ints)[DESCENT] = desc;

        AutoPtr<ILayoutDirections> directions;
        reflowed->GetLineDirections(i, (ILayoutDirections**)&directions);
        objects->Set(0, directions);

        if (mEllipsize) {
            reflowed->GetEllipsisStart(i, &(*ints)[ELLIPSIS_START]);
            reflowed->GetEllipsisCount(i, &(*ints)[ELLIPSIS_COUNT]);
        }

        mInts->InsertAt(startline + i, ints);
        mObjects->InsertAt(startline + i, objects);
    }

    UpdateBlocks(startline, endline - 1, n);

    {
        AutoLock lock(&sLock);

        SetStaticLayout(reflowed);
        reflowed->Finish();
    }
}

/**
 * Create the initial block structure, cutting the text into blocks of at least
 * BLOCK_MINIMUM_CHARACTER_SIZE characters, aligned on the ends of paragraphs.
 */
void DynamicLayout::CreateBlocks()
{
    Int32 offset = BLOCK_MINIMUM_CHARACTER_LENGTH;
    mNumberOfBlocks = 0;
    AutoPtr<ICharSequence> text = mDisplay;

    while (TRUE) {
        offset = TextUtils::IndexOf(text, '\n', offset);
        if (offset < 0) {
            Int32 len = 0;
            text->GetLength(&len);
            AddBlockAtOffset(len);
            break;
        }
        else {
            AddBlockAtOffset(offset);
            offset += BLOCK_MINIMUM_CHARACTER_LENGTH;
        }
    }

    // mBlockIndices and mBlockEndLines should have the same length
    mBlockIndices = ArrayOf<Int32>::Alloc(mBlockEndLines->GetLength());
    for (Int32 i = 0; i < mBlockEndLines->GetLength(); i++) {
        (*mBlockIndices)[i] = INVALID_BLOCK_INDEX;
    }
}

/**
 * Create a new block, ending at the specified character offset.
 * A block will actually be created only if has at least one line, i.e. this offset is
 * not on the end line of the previous block.
 */
void DynamicLayout::AddBlockAtOffset(
    /* [in] */ Int32 offset)
{
    Int32 line;
    GetLineForOffset(offset, &line);

    if (mBlockEndLines == NULL) {
        // Initial creation of the array, no test on previous block ending line
        mBlockEndLines = ArrayUtils::NewUnpaddedInt32Array(1);
        (*mBlockEndLines)[mNumberOfBlocks] = line;
        mNumberOfBlocks++;
        return;
    }

    Int32 previousBlockEndLine = (*mBlockEndLines)[mNumberOfBlocks - 1];
    if (line > previousBlockEndLine) {
        mBlockEndLines = GrowingArrayUtils::Append(mBlockEndLines, mNumberOfBlocks, line);
        mNumberOfBlocks++;
    }
}

ECode DynamicLayout::UpdateBlocks(
    /* [in] */ Int32 startLine,
    /* [in] */ Int32 endLine,
    /* [in] */ Int32 newLineCount)
{
    if (mBlockEndLines == NULL) {
        CreateBlocks();
        return NOERROR;
    }

    Int32 firstBlock = -1;
    Int32 lastBlock = -1;
    for (Int32 i = 0; i < mNumberOfBlocks; i++) {
        if ((*mBlockEndLines)[i] >= startLine) {
            firstBlock = i;
            break;
        }
    }
    for (Int32 i = firstBlock; i < mNumberOfBlocks; i++) {
        if ((*mBlockEndLines)[i] >= endLine) {
            lastBlock = i;
            break;
        }
    }
    const Int32 lastBlockEndLine = (*mBlockEndLines)[lastBlock];

    Boolean createBlockBefore = startLine > (firstBlock == 0 ? 0 :
            (*mBlockEndLines)[firstBlock - 1] + 1);
    Boolean createBlock = newLineCount > 0;
    Boolean createBlockAfter = endLine < (*mBlockEndLines)[lastBlock];

    Int32 numAddedBlocks = 0;
    if (createBlockBefore) numAddedBlocks++;
    if (createBlock) numAddedBlocks++;
    if (createBlockAfter) numAddedBlocks++;

    const Int32 numRemovedBlocks = lastBlock - firstBlock + 1;
    const Int32 newNumberOfBlocks = mNumberOfBlocks + numAddedBlocks - numRemovedBlocks;

    if (newNumberOfBlocks == 0) {
        // Even when text is empty, there is actually one line and hence one block
        (*(mBlockEndLines.Get()))[0] = 0;
        (*(mBlockIndices.Get()))[0] = INVALID_BLOCK_INDEX;
        mNumberOfBlocks = 1;
        return NOERROR;
    }

    if (newNumberOfBlocks > mBlockEndLines->GetLength()) {
        Int32 newSize = Elastos::Core::Math::Max(mBlockEndLines->GetLength() * 2, newNumberOfBlocks);
        AutoPtr< ArrayOf<Int32> > blockEndLines = ArrayUtils::NewUnpaddedInt32Array(newSize);
        AutoPtr< ArrayOf<Int32> > blockIndices = ArrayOf<Int32>::Alloc(blockEndLines->GetLength());
        blockEndLines->Copy(mBlockEndLines, firstBlock);
        blockIndices->Copy(mBlockIndices, firstBlock);
        blockEndLines->Copy(firstBlock + numAddedBlocks, mBlockEndLines, lastBlock + 1, mNumberOfBlocks - lastBlock - 1);
        blockIndices->Copy(firstBlock + numAddedBlocks, mBlockIndices, lastBlock + 1, mNumberOfBlocks - lastBlock - 1);
        mBlockEndLines = blockEndLines;
        mBlockIndices = blockIndices;
    }
    else {
        mBlockEndLines->Copy(firstBlock + numAddedBlocks, mBlockEndLines, lastBlock + 1, mNumberOfBlocks - lastBlock - 1);
        mBlockIndices->Copy(firstBlock + numAddedBlocks, mBlockIndices, lastBlock + 1, mNumberOfBlocks - lastBlock - 1);
    }

    mNumberOfBlocks = newNumberOfBlocks;
    Int32 newFirstChangedBlock;
    Int32 deltaLines = newLineCount - (endLine - startLine + 1);
    if (deltaLines != 0) {
        // Display list whose index is >= mIndexFirstChangedBlock is valid
        // but it needs to update its drawing location.
        newFirstChangedBlock = firstBlock + numAddedBlocks;
        for (Int32 i = newFirstChangedBlock; i < mNumberOfBlocks; i++) {
            (*mBlockEndLines)[i] += deltaLines;
        }
    }
    else {
        newFirstChangedBlock = mNumberOfBlocks;
    }
    mIndexFirstChangedBlock = Elastos::Core::Math::Min(mIndexFirstChangedBlock, newFirstChangedBlock);


    Int32 blockIndex = firstBlock;
    if (createBlockBefore) {
        (*mBlockEndLines)[blockIndex] = startLine - 1;
        (*mBlockIndices)[blockIndex] = INVALID_BLOCK_INDEX;
        blockIndex++;
    }

    if (createBlock) {
        (*mBlockEndLines)[blockIndex] = startLine + newLineCount - 1;
        (*mBlockIndices)[blockIndex] = INVALID_BLOCK_INDEX;
        blockIndex++;
    }

    if (createBlockAfter) {
        (*mBlockEndLines)[blockIndex] = lastBlockEndLine + deltaLines;
        (*mBlockIndices)[blockIndex] = INVALID_BLOCK_INDEX;
    }

    return NOERROR;
}

/**
 * This package private method is used for test purposes only
 * @hide
 */
ECode DynamicLayout::SetBlocksDataForTest(
    /* [in] */ ArrayOf<Int32>* blockEndLines,
    /* [in] */ ArrayOf<Int32>* blockIndices,
    /* [in] */ Int32 numberOfBlocks)
{
    mBlockEndLines = ArrayOf<Int32>::Alloc(blockEndLines->GetLength());
    mBlockIndices = ArrayOf<Int32>::Alloc(blockIndices->GetLength());
    mBlockEndLines->Copy(blockEndLines);
    mBlockIndices->Copy(blockIndices);
    mNumberOfBlocks = numberOfBlocks;
    return NOERROR;
}

/**
 * @hide
 */
ECode DynamicLayout::GetBlockEndLines(
    /* [out, calllee] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mBlockEndLines;
    return NOERROR;
}

/**
 * @hide
 */
ECode DynamicLayout::GetBlockIndices(
    /* [out, calllee] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mBlockIndices;
    return NOERROR;
}

/**
 * @hide
 */
ECode DynamicLayout::GetNumberOfBlocks(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mNumberOfBlocks;
    return NOERROR;
}

/**
 * @hide
 */
ECode DynamicLayout::GetIndexFirstChangedBlock(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIndexFirstChangedBlock;
    return NOERROR;
}

/**
 * @hide
 */
ECode DynamicLayout::SetIndexFirstChangedBlock(
    /* [in] */ Int32 block)
{
    mIndexFirstChangedBlock = block;
    return NOERROR;
}

//@Override
ECode DynamicLayout::GetLineCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mInts->Size() - 1;
    return NOERROR;
}

//@Override
ECode DynamicLayout::GetLineTop(
    /* [in] */ Int32 line,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mInts->GetValue(line, TOP);
    return NOERROR;
}

//@Override
ECode DynamicLayout::GetLineDescent(
    /* [in] */ Int32 line,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mInts->GetValue(line, DESCENT);
    return NOERROR;
}

//@Override
ECode DynamicLayout::GetLineStart(
    /* [in] */ Int32 line,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mInts->GetValue(line, START) & START_MASK;
    return NOERROR;
}

//@Override
ECode DynamicLayout::GetLineContainsTab(
    /* [in] */ Int32 line,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (mInts->GetValue(line, TAB) & TAB_MASK) != 0;
    return NOERROR;
}

//@Override
ECode DynamicLayout::GetParagraphDirection(
    /* [in] */ Int32 line,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mInts->GetValue(line, DIR) >> DIR_SHIFT;
    return NOERROR;
}

//@Override
ECode DynamicLayout::GetLineDirections(
    /* [in] */ Int32 line,
    /* [out] */ ILayoutDirections** result)
{
    VALIDATE_NOT_NULL(result)

    assert(line >= 0);
    AutoPtr<ILayoutDirections> ld;
    if (mObjects != NULL) {
        ld = mObjects->GetValue(line, 0);
    }
    *result = ld;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

//@Override
ECode DynamicLayout::GetTopPadding(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mTopPadding;
    return NOERROR;
}

//@Override
ECode DynamicLayout::GetBottomPadding(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mBottomPadding;
    return NOERROR;
}

//@Override
ECode DynamicLayout::GetEllipsizedWidth(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mEllipsizedWidth;
    return NOERROR;
}

//@Override
ECode DynamicLayout::GetEllipsisStart(
    /* [in] */ Int32 line,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    if (mEllipsizeAt == -1) {
        return NOERROR;
    }

    *result = mInts->GetValue(line, ELLIPSIS_START);
    return NOERROR;
}

//@Override
ECode DynamicLayout::GetEllipsisCount(
    /* [in] */ Int32 line,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    if (mEllipsizeAt == -1) {
        return NOERROR;
    }

    *result = mInts->GetValue(line, ELLIPSIS_COUNT);
    return NOERROR;
}

} // namespace Text
} // namepsace Droid
} // namespace Elastos
