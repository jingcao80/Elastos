//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

/* dlfcn.h

   Copyright 1998, 1999, 2000, 2001 Red Hat, Inc.

This file is part of Cygwin.

This software is a copyrighted work licensed under the terms of the
Cygwin license.  Please consult the file "CYGWIN_LICENSE" for
details. */

#ifndef _DLFCN_H
#define _DLFCN_H

extern "C" {

/* declarations used for dynamic linking support routines */
extern void*        dlopen(const char*  filename, int flag);
extern int          dlclose(void*  handle);
extern const char*  dlerror(void);
extern void*        dlsym(void*  handle, const char*  symbol);
extern int dlGetClassInfo(void *handle, void **pAddress, int *pSize);

/* following doesn't exist in Win32 API .... */
#define RTLD_DEFAULT  ((void*) 0x10)
#define RTLD_NEXT     ((void*) 0x20)

/* valid values for mode argument to dlopen */
#define RTLD_LAZY	1	/* lazy function call binding */
#define RTLD_NOW	2	/* immediate function call binding */
#define RTLD_GLOBAL	4	/* symbols in this dlopen'ed obj are visible to other dlopen'ed objs */
#define RTLD_LOCAL  8

}

#endif /* _DLFCN_H */
