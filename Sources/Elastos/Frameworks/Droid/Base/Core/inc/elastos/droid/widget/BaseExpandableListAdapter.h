
#ifndef __ELASTOS_DROID_WIDGET_BASEEXPANDABLELISTADAPTER_H__
#define __ELASTOS_DROID_WIDGET_BASEEXPANDABLELISTADAPTER_H__

#include "Elastos.Droid.Database.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Database::IDataSetObserver;
using Elastos::Droid::Database::IDataSetObservable;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Widget {

/**
 * Base class for a {@link ExpandableListAdapter} used to provide data and Views
 * from some data to an expandable list view.
 * <p>
 * Adapters inheriting this class should verify that the base implementations of
 * {@link #getCombinedChildId(Int64, Int64)} and {@link #getCombinedGroupId(Int64)}
 * are correct in generating unique IDs from the group/children IDs.
 * <p>
 * @see SimpleExpandableListAdapter
 * @see SimpleCursorTreeAdapter
 */
class BaseExpandableListAdapter
    : public Object
    , public IBaseExpandableListAdapter
    , public IExpandableListAdapter
    , public IHeterogeneousExpandableList
{
public:
    CAR_INTERFACE_DECL();

    BaseExpandableListAdapter();

    virtual CARAPI RegisterDataSetObserver(
        /* [in] */ IDataSetObserver* observer);

    virtual CARAPI UnregisterDataSetObserver(
        /* [in] */ IDataSetObserver* observer);

    /**
     * @see DataSetObservable#notifyInvalidated()
     */
    virtual CARAPI NotifyDataSetInvalidated();

    /**
     * @see DataSetObservable#notifyChanged()
     */
    virtual CARAPI NotifyDataSetChanged();

    virtual CARAPI AreAllItemsEnabled(
        /* [out] */ Boolean* enabled);

    virtual CARAPI OnGroupCollapsed(
        /* [in] */ Int32 groupPosition);

    virtual CARAPI OnGroupExpanded(
        /* [in] */ Int32 groupPosition);

    /**
     * Override this method if you foresee a clash in IDs based on this scheme:
     * <p>
     * Base implementation returns a Int64:
     * <li> bit 0: Whether this ID points to a child (unset) or group (set), so for this method
     *             this bit will be 1.
     * <li> bit 1-31: Lower 31 bits of the groupId
     * <li> bit 32-63: Lower 32 bits of the childId.
     * <p>
     * {@inheritDoc}
     */
    virtual CARAPI GetCombinedChildId(
        /* [in] */ Int64 groupId,
        /* [in] */ Int64 childId,
        /* [out] */ Int64* id);

    /**
     * Override this method if you foresee a clash in IDs based on this scheme:
     * <p>
     * Base implementation returns a Int64:
     * <li> bit 0: Whether this ID points to a child (unset) or group (set), so for this method
     *             this bit will be 0.
     * <li> bit 1-31: Lower 31 bits of the groupId
     * <li> bit 32-63: Lower 32 bits of the childId.
     * <p>
     * {@inheritDoc}
     */
    virtual CARAPI GetCombinedGroupId(
        /* [in] */ Int64 groupId,
        /* [out] */ Int64* id);

    /**
     * {@inheritDoc}
     */
    virtual CARAPI IsEmpty(
        /* [out] */ Boolean* empty);

    /**
     * {@inheritDoc}
     * @return 0 for any group or child position, since only one child type count is declared.
     */
    virtual CARAPI GetChildType(
        /* [in] */ Int32 groupPosition,
        /* [in] */ Int32 childPosition,
        /* [out] */ Int32* type);

    /**
     * {@inheritDoc}
     * @return 1 as a default value in BaseExpandableListAdapter.
     */
    virtual CARAPI GetChildTypeCount(
        /* [out] */ Int32* count);

    /**
     * {@inheritDoc}
     * @return 0 for any groupPosition, since only one group type count is declared.
     */
    virtual CARAPI GetGroupType(
        /* [in] */ Int32 groupPosition,
        /* [out] */ Int32* type);

    /**
     * {@inheritDoc}
     * @return 1 as a default value in BaseExpandableListAdapter.
     */
    virtual CARAPI GetGroupTypeCount(
        /* [out] */ Int32* count);

    virtual CARAPI GetGroupCount(
        /* [out] */ Int32* count) = 0;

private:
    AutoPtr<IDataSetObservable> mDataSetObservable ;

};

}// namespace Elastos
}// namespace Droid
}// namespace Widget

#endif
