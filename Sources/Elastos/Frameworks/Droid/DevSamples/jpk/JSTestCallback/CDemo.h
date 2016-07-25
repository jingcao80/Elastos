#ifndef __CDEMO_H__
#define __CDEMO_H__

#include "_CDemo.h"

#include <elastos/core/Object.h>

CarClass(CDemo)
    , public Object
    , public IDemo
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI addEventHandler(
        /* [in] */ IEventListener* listener);

    CARAPI doTask();

private:
    AutoPtr<IEventListener> m_Listener;
};

#endif // __CDEMO_H__
