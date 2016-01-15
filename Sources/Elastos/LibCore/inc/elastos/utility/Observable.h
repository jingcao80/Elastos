
#ifndef __ELASTOS_UTILITY_OBSERVABLE_H__
#define __ELASTOS_UTILITY_OBSERVABLE_H__

#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Utility {

class ECO_PUBLIC Observable
    : public Object
    , public IObservable
{
public:
    CAR_INTERFACE_DECL()

    Observable();

    virtual ~Observable();

    CARAPI constructor();

    /**
     * Adds the specified observer to the list of observers. If it is already
     * registered, it is not added a second time.
     *
     * @param observer
     *            the Observer to add.
     */
    CARAPI AddObserver(
        /* [in] */ IObserver* observer);

    /**
     * Returns the number of observers registered to this {@code Observable}.
     *
     * @return the number of observers.
     */
    CARAPI CountObservers(
        /* [out] */ Int32* value);

    /**
     * Removes the specified observer from the list of observers. Passing null
     * won't do anything.
     *
     * @param observer
     *            the observer to remove.
     */
    CARAPI DeleteObserver(
        /* [in] */ IObserver* observer);

    /**
     * Removes all observers from the list of observers.
     */
    CARAPI DeleteObservers();

    /**
     * Returns the changed flag for this {@code Observable}.
     *
     * @return {@code true} when the changed flag for this {@code Observable} is
     *         set, {@code false} otherwise.
     */
    CARAPI HasChanged(
        /* [out] */ Boolean* value);

    /**
     * If {@code hasChanged()} returns {@code true}, calls the {@code update()}
     * method for every observer in the list of observers using null as the
     * argument. Afterwards, calls {@code clearChanged()}.
     * <p>
     * Equivalent to calling {@code notifyObservers(null)}.
     */
    CARAPI NotifyObservers();

    /**
     * If {@code hasChanged()} returns {@code true}, calls the {@code update()}
     * method for every Observer in the list of observers using the specified
     * argument. Afterwards calls {@code clearChanged()}.
     *
     * @param data
     *            the argument passed to {@code update()}.
     */
    CARAPI NotifyObservers(
        /* [in] */ IInterface* data);

    /**
     * Clears the changed flag for this {@code Observable}. After calling
     * {@code clearChanged()}, {@code hasChanged()} will return {@code false}.
     */
    CARAPI ClearChanged();

    /**
     * Sets the changed flag for this {@code Observable}. After calling
     * {@code setChanged()}, {@code hasChanged()} will return {@code true}.
     */
    CARAPI SetChanged();

protected:
    AutoPtr<IList> mObservers; // = new ArrayList<Observer>();

    Boolean mChanged; // = false;
};

} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY_OBSERVABLE_H__
