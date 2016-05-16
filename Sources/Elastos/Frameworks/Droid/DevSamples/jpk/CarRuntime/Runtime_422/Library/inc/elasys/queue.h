//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================
/*
     -------------------------------------------------------------------
    |                                                                   |
    |    Queue Class                                                    |
    |    ===========================================================    |
    |    This Queue Class has been implemented with templates and       |
    |    the size is determined dynamically at initialization.          |
    |                                                                   |
    |    The actual amount of space allocated for the Queue will be     |
    |    one more element space than the defined maximum size. This     |
    |    is useful for implementing the Queue in a circular method.     |
    |                                                                   |
    |    To understand the circular implementation, think of the        |
    |    array as a circle. When you reach the end of the array, you    |
    |    wrap around to the beginning of the array.                     |
    |                                                                   |
    |    So, when an element is dequeued, the Queue doesn't shift.      |
    |    Instead, you updated an indicator of the start of the queue.   |
    |                                                                   |
     -------------------------------------------------------------------
*/

#ifndef __Queue_H__
#define __Queue_H__

#include <stdio.h>
#include <assert.h>

//#define _DBGQUEUE

//-------------------------------------------------
// Main structure of Queue Class:
//-------------------------------------------------

class Queue
{
public:
    Queue(int ElemSize=4, int MaxSize=32);
    ~Queue(void);

    int     Enqueue(const void *pElem); // Adds Item to Queue end
    int     Dequeue(/*Out*/void *pElem);// Returns Item from Queue
#if defined(_DBGQUEUE)
    void    Dump(void);
#endif

public:
    const int   ELEM_SIZ;
    const int   MAX_NUM;    // The actual spaces will be one more than this
    unsigned char *Data;    // The actual Data array
    int         Head,       // Numbered location of the head and tail
                Tail,       //
                Drops;      // element droped when queue FIFO is full
};

//-------------------------------------------------
// Implementation of Queue Class:
//-------------------------------------------------

// Queue Constructor function
Queue::Queue(int ElemSize, int MaxSize) :
    ELEM_SIZ( ElemSize ),// Initialize the constant
    MAX_NUM( MaxSize )   // Initialize the constant
{
    // This extra space added will allow us to distinguish between
    // the Head and the Tail locations.
    Data = new unsigned char[ELEM_SIZ*(MAX_NUM + 1)];
#if defined(_DBGQUEUE)
    printf("Queue %d*%d\n", ELEM_SIZ, MAX_NUM);
    memset(Data, 0, ELEM_SIZ*(MAX_NUM + 1));
#endif
    Head = Tail = Drops = 0;
}

// Queue Destructor function
Queue::~Queue(void)
{
    delete[] Data;
}

#if defined(_DBGQUEUE)
// Dump() function
void Queue::Dump(void)
{
    int Avail = (Head >= Tail)?(Head - Tail):(MAX_NUM + 1 - Tail + Head);
    printf("H %d\tT %d\tN %d\tD %d\n",
        Head, Tail, Avail, Drops);

    int i;
    for (i = 0; i <= MAX_NUM; i++)
        printf("%d(%x)\t", i, Data[ELEM_SIZ*i]);
    printf("%d\n", i);
    return ;
}
#endif

// Enqueue() function, Call by ISR
int Queue::Enqueue(const void *pElem)
{
    int Avail = Tail - Head;
    if ((Avail != 1) && (Avail != -MAX_NUM)) {
        // Where do we put it.
        Avail = Head;
        if (ELEM_SIZ == 4)
            ((unsigned long *)Data)[ Avail++ ] = *(unsigned long *)pElem;
        else if (ELEM_SIZ == 8)
            ((unsigned __int64 *)Data)[ Avail++ ] = *(unsigned __int64 *)pElem;
        else
            memcpy(Data + ELEM_SIZ*Avail++, pElem, ELEM_SIZ);
        if (Avail > MAX_NUM) Avail = 0;
        Head = Avail;//Update ptr.
        return 0;
    }
    else {  //No room. Just drop it.
        Drops++;
    }
    return -1;
}

// Dequeue() function Call by single thread
int Queue::Dequeue(/*Out*/void *pElem)
{
    // How much can we get ?
    int Avail = Head; /* cache against interrupts */
    Avail = (Avail >= Tail)?(Avail - Tail):(MAX_NUM + 1 - Tail + Avail);
    assert(Avail >= 0  && Avail <= MAX_NUM);

    if (Avail == 0) return -1;//Must return invalid element

    Avail = Tail;
#if defined(_DBGQUEUE)
    memcpy(pElem, Data + ELEM_SIZ*Avail, ELEM_SIZ);
    memset(Data + ELEM_SIZ*Avail++, 0, ELEM_SIZ);//Clean
    assert(*(unsigned long *)pElem > 0);
#else
    if (ELEM_SIZ == 4)
        *(unsigned long *)pElem = ((unsigned long *)Data)[ Avail++ ];
    else if (ELEM_SIZ == 8)
        *(unsigned __int64 *)pElem = ((unsigned __int64 *)Data)[ Avail++ ];
    else
        memcpy(pElem, Data + ELEM_SIZ*Avail++, ELEM_SIZ);
#endif
    if (Avail > MAX_NUM) Avail = 0;

    Tail = Avail;//Update ptr.

    return 0;
}

#endif /*__Queue_H__*/
