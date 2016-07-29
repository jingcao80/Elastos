#ifndef __CAR_BRIDGE_CAN_DOWN_DELETE_H
# define __CAR_BRIDGE_CAN_DOWN_DELETE_H

# include "macros.h"



CAR_BRIDGE_NAMESPACE_BEGIN

class CanDownDelete {
public:
    virtual ~CanDownDelete() = default;

protected:
    CanDownDelete(void) = default;

private:
    CanDownDelete(CanDownDelete const &canDownDelete) = delete;

    CanDownDelete(CanDownDelete &&canDownDelete) = delete;

    CanDownDelete &operator=(CanDownDelete const &canDownDelete) = delete;

    CanDownDelete &operator=(CanDownDelete &&canDownDelete) = delete;
};

CAR_BRIDGE_NAMESPACE_END

#endif
