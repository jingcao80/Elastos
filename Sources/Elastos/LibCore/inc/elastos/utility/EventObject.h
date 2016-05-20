#ifndef __ELASTOS_UTILITY_EVENTOBJECT_H__
#define __ELASTOS_UTILITY_EVENTOBJECT_H__

#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Object.h"

using Elastos::IO::ISerializable;

namespace Elastos {
namespace Utility {

class EventObject
    : public Object
    , public IEventObject
    , public ISerializable
{
public:
    CAR_INTERFACE_DECL()

    EventObject();

    virtual ~EventObject();

    /**
     * Constructs a new instance of this class.
     *
     * @param source
     *            the object which fired the event.
     */
    CARAPI constructor(
        /* [in] */ IObject* source);

    /**
     * Returns the object which fired the event.
     */
    virtual CARAPI GetSource(
        /* [out] */ IInterface** object);

    /**
     * Returns the string representation of this {@code EventObject}.
     */
    CARAPI ToString(
        /* [in] */ String* str);

protected:
    /*transient*/ AutoPtr<IObject> mSource;
};

} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY_EVENTOBJECT_H__
