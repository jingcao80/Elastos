
#ifndef __ETL_EXCEPTION_DEFINES_H__
#define __ETL_EXCEPTION_DEFINES_H__

#ifndef __EXCEPTIONS
    #define try         if (TRUE)
    #define catch(X)    if (FALSE)
    #define THROW_EXCEPTION_AGAIN
#else
    #define THROW_EXCEPTION_AGAIN throw
#endif

#endif //__ETL_EXCEPTION_DEFINES_H__
