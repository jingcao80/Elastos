//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __ELANS_H__
#define __ELANS_H__

#ifdef __cplusplus

#define _ELASTOS                    ::Elastos::
#define _ELASTOS_NAMESPACE_BEGIN    namespace Elastos {
#define _ELASTOS_NAMESPACE_END      }
#define _ELASTOS_NAMESPACE_USING    using namespace Elastos;

#else

#define _ELASTOS
#define _ELASTOS_NAMESPACE_BEGIN
#define _ELASTOS_NAMESPACE_END
#define _ELASTOS_NAMESPACE_USING

#endif // __cplusplus

#endif //__ELANS_H__
