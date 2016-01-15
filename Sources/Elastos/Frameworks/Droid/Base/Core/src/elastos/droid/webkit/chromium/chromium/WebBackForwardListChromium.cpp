
namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Chromium {
namespace Chromium {

/* package */
WebBackForwardListChromium::WebBackForwardListChromium(
    /* [in] */ NavigationHistory* nav_history)
{
    mCurrentIndex = nav_history.getCurrentEntryIndex();
    mHistroryItemList = new ArrayList<WebHistoryItemChromium>(nav_history.getEntryCount());
    for (Int32 i = 0; i < nav_history.getEntryCount(); ++i) {
        mHistroryItemList.add(
                new WebHistoryItemChromium(nav_history.getEntryAtIndex(i)));
    }
}

// Clone constructor.
WebBackForwardListChromium::WebBackForwardListChromium(
    /* [in] */ List<WebHistoryItemChromium> list,
    /* [in] */ Int32 currentIndex)
    : mHistroryItemList(list)
    , mCurrentIndex(currentIndex)
{
}

/**
 * See {@link android.webkit.WebBackForwardList#getCurrentItem}.
 */
//@Override
//synchronized
ECode WebBackForwardListChromium::GetCurrentItem(
    /* [out] */ IWebHistoryItem** item)
{
    VALIDATE_NOT_NULL(item);

    if (GetSize() == 0) {
        *item = NULL;
        return NOERROR;
    }
    else {
        Int32 index;
        GetCurrentIndex(&index);
        return GetItemAtIndex(index, item);
    }
}

/**
 * See {@link android.webkit.WebBackForwardList#getCurrentIndex}.
 */
//@Override
//synchronized
ECode WebBackForwardListChromium::GetCurrentIndex(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    *index = mCurrentIndex;
    return NOERROR;
}

/**
 * See {@link android.webkit.WebBackForwardList#getItemAtIndex}.
 */
//@Override
//synchronized
ECode WebBackForwardListChromium::GetItemAtIndex(
    /* [in] */ Int32 index,
    /* [out] */ IWebHistoryItem** item)
{
    VALIDATE_NOT_NULL(item);

    if (index < 0 || index >= GetSize()) {
        *item = NULL;
        return NOERROR;
    }
    else {
        *item = mHistroryItemList.get(index);
        return NOERROR;
    }
}

/**
 * See {@link android.webkit.WebBackForwardList#getSize}.
 */
//@Override
//synchronized
ECode WebBackForwardListChromium::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);
    *size = mHistroryItemList.size();
    return NOERROR;
}

/**
 * See {@link android.webkit.WebBackForwardList#clone}.
 */
//@Override
//synchronized
ECode WebBackForwardListChromium::Clone(
    /* [out] */ WebBackForwardListChromium** chromium)
{
    VALIDATE_NOT_NULL(chromium);

    Int32 size;
    GetSize(&size);
    List<WebHistoryItemChromium> list =
            new ArrayList<WebHistoryItemChromium>(size);
    for (Int32 i = 0; i < size; ++i) {
        list.add(mHistroryItemList.get(i).clone());
    }

    *chromium = new WebBackForwardListChromium(list, mCurrentIndex);

    return NOERROR;
}

} // namespace Chromium
} // namespace Chromium
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
