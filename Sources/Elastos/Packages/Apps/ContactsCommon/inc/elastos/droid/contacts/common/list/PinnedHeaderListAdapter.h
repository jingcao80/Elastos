#ifndef __ELASTOS_APPS_CONTACTS_COMMON_LIST_PINNEDHEADERLISTADAPTER_H__
#define __ELASTOS_APPS_CONTACTS_COMMON_LIST_PINNEDHEADERLISTADAPTER_H__


namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace List {

/**
 * A subclass of {@link CompositeCursorAdapter} that manages pinned partition headers.
 */
class PinnedHeaderListAdapter
    /*: public CompositeCursorAdapter*/
    : public Object
    , public IPinnedHeaderListAdapter
    , public IPinnedHeaderAdapter
{
public:
    CAR_INTERFACE_DECL()

    virtual ~CPinnedHeaderListAdapter();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 initialCapacity);

    CARAPI GetPinnedPartitionHeadersEnabled(
        /* [out] */ Boolean* result);

    CARAPI SetPinnedPartitionHeadersEnabled(
        /* [in] */ Boolean flag);

    // @Override
    CARAPI GetPinnedHeaderCount(
        /* [out] */ Int32* count);

    /**
     * The default implementation creates the same type of view as a normal
     * partition header.
     */
    // @Override
    CARAPI GetPinnedHeaderView(
        /* [in] */ Int32 partition,
        /* [in] */ IView* convertView,
        /* [in] */ IViewGroup* parent,
        /* [out] */ IView** result);

    // @Override
    CARAPI ConfigurePinnedHeaders(
        /* [in] */ IPinnedHeaderListView* listView);

    // @Override
    CARAPI GetScrollPositionForHeader(
        /* [in] */ Int32 viewIndex,
        /* [out] */ Int32* result);

protected:
    CARAPI_(Boolean) IsPinnedPartitionHeaderVisible(
        /* [in] */ Int32 partition);

private:
    Boolean mPinnedPartitionHeadersEnabled;
    AutoPtr<ArrayOf<Boolean> > mHeaderVisibility;
};

} // List
} // Common
} // Contacts
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_CONTACTS_COMMON_LIST_PINNEDHEADERLISTADAPTER_H__
