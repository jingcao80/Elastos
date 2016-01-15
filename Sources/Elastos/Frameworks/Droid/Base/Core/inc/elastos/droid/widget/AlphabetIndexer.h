
#ifndef __ELASTOS_DROID_WIDGET_ALPHABETINDEXER_H__
#define __ELASTOS_DROID_WIDGET_ALPHABETINDEXER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/database/DataSetObserver.h"

using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::IDataSetObserver;
using Elastos::Droid::Database::DataSetObserver;
using Elastos::Droid::Database::EIID_IDataSetObserver;
using Elastos::Droid::Utility::ISparseInt32Array;

using Elastos::Core::ICharSequence;
using Elastos::Text::ICollator;

namespace Elastos {
namespace Droid {
namespace Widget {

/**
 * A helper class for adapters that implement the SectionIndexer interface.
 * If the items in the adapter are sorted by simple alphabet-based sorting, then
 * this class provides a way to do fast indexing of large lists using binary search.
 * It caches the indices that have been determined through the binary search and also
 * invalidates the cache if changes occur in the cursor.
 * <p/>
 * Your adapter is responsible for updating the cursor by calling {@link #setCursor} if the
 * cursor changes. {@link #getPositionForSection} method does the binary search for the starting
 * index of a given section (alphabet).
 */
class AlphabetIndexer
    : public DataSetObserver
    , public ISectionIndexer
    , public IAlphabetIndexer
{
public:
    CAR_INTERFACE_DECL()

    AlphabetIndexer();

    /**
     * Constructs the indexer.
     * @param cursor the cursor containing the data set
     * @param sortedColumnIndex the column number in the cursor that is sorted
     *        alphabetically
     * @param alphabet string containing the alphabet, with space as the first character.
     *        For example, use the string " ABCDEFGHIJKLMNOPQRSTUVWXYZ" for English indexing.
     *        The characters must be uppercase and be sorted in ascii/unicode order. Basically
     *        characters in the alphabet will show up as preview letters.
     */
    CARAPI constructor(
        /* [in] */ ICursor* cursor,
        /* [in] */ Int32 sortedColumnIndex,
        /* [in] */ ICharSequence* alphabet);

    ~AlphabetIndexer();

    /**
     * Returns the section array constructed from the alphabet provided in the constructor.
     * @return the section array
     */
    CARAPI GetSections(
        /* [out, callee] */ ArrayOf<IInterface*>** sections);

    /**
     * Sets a new cursor as the data set and resets the cache of indices.
     * @param cursor the new cursor to use as the data set
     */
    CARAPI SetCursor(
        /* [in] */ ICursor* cursor);

    /**
     * Performs a binary search or cache lookup to find the first row that
     * matches a given section's starting letter.
     * @param sectionIndex the section to search for
     * @return the row index of the first occurrence, or the nearest next letter.
     * For instance, if searching for "T" and no "T" is found, then the first
     * row starting with "U" or any higher letter is returned. If there is no
     * data following "T" at all, then the list size is returned.
     */
    CARAPI GetPositionForSection(
        /* [in] */ Int32 sectionIndex,
        /* [out] */ Int32* position);

    /**
     * Returns the section index for a given position in the list by querying the item
     * and comparing it with all items in the section array.
     */
    CARAPI GetSectionForPosition(
        /* [in] */ Int32 position,
        /* [out] */ Int32* section);

    /*
     * @hide
     */
    CARAPI OnChanged();

    /*
     * @hide
     */
    CARAPI OnInvalidated();

protected:
    /**
     * Default implementation compares the first character of word with letter.
     */
    virtual CARAPI_(Int32) Compare(
        /* [in] */ const String& word,
        /* [in] */ const String& letter);

protected:
    /**
     * Cursor that is used by the adapter of the list view.
     */
    AutoPtr<ICursor> mDataCursor;

    /**
     * The index of the cursor column that this list is sorted on.
     */
    Int32 mColumnIndex;

    /**
     * The string of characters that make up the indexing sections.
     */
    AutoPtr<ICharSequence> mAlphabet;

private:
    /**
     * Cached length of the alphabet array.
     */
    Int32 mAlphabetLength;

    /**
     * This contains a cache of the computed indices so far. It will get reset whenever
     * the dataset changes or the cursor changes.
     */
    AutoPtr<ISparseInt32Array> mAlphaMap;

    /**
     * Use a collator to compare strings in a localized manner.
     */
    AutoPtr<ICollator> mCollator;

    /**
     * The section array converted from the alphabet string.
     */
    AutoPtr<ArrayOf<String> > mAlphabetArray;
};

}// namespace Elastos
}// namespace Droid
}// namespace Widget

#endif
