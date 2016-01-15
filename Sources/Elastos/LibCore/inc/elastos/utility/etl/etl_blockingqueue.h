
#ifndef __ETL_BLOCKINGQUEUE_H__
#define __ETL_BLOCKINGQUEUE_H__

#include <elastos/utility/etl/etl_queue.h>
#include <pthread.h>

_ETL_NAMESPACE_BEGIN

template <class Tp,
          class Sequence = Deque<Tp> >
class BlockingQueue;

template <class Tp, class Seq>
inline Boolean operator==(const BlockingQueue<Tp, Seq>&, const BlockingQueue<Tp, Seq>&);

template <class Tp, class Seq>
inline Boolean operator<(const BlockingQueue<Tp, Seq>&, const BlockingQueue<Tp, Seq>&);

template <class Tp, class Sequence>
class BlockingQueue : public ElLightRefBase
{

private:
    pthread_mutex_t mWaitMutex;
    pthread_cond_t mWaitCond;
};

_ETL_NAMESPACE_END

#endif //__ETL_BLOCKINGQUEUE_H__
