#include "CDemo.h"

CAR_INTERFACE_IMPL(CDemo, Object, IDemo)
CAR_OBJECT_IMPL(CDemo)

ECode CDemo::addEventHandler(
    /* [in] */ IEventListener* listener)
{
    m_Listener = listener;
    return NOERROR;
}

ECode CDemo::doTask()
{
    m_Listener->OnEvent(9);
    return NOERROR;
}