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

#include "CVm.h"
#include "sqlitejni.h"

namespace Elastos {
namespace Sql {
namespace SQLite {

CAR_OBJECT_IMPL(CVm)

CAR_INTERFACE_IMPL(CVm, Object, IVm)

CVm::CVm()
    : mHandle(0)
    , mError_code(0)
{
}

CVm::~CVm()
{
    Finalize();
}

#if HAVE_SQLITE_COMPILE
#if HAVE_SQLITE3
static void Free_tab(void *mem)
{
    char **p = (char **) mem;
    int i, n;

    if (!p) {
    return;
    }
    p -= 1;
    mem = (void *) p;
    n = ((int *) p)[0];
    p += n * 2 + 2 + 1;
    for (i = 0; i < n; i++) {
    if (p[i]) {
        free(p[i]);
    }
    }
    free(mem);
}
#endif
#endif

static Int32 Callback(void *udata, int ncol, char **data, char **cols)
{
    handle *h = (handle *) udata;

    if (h->cb) {
    AutoPtr<ArrayOf<String> > arr;
    Int32 i = 0;
    if (h->row1) {
            arr = ArrayOf<String>::Alloc(ncol);
            for (i = 0; i < ncol; i++) {
                if (cols[i]) {
                    (*arr)[i] = String(cols[i]);
                }
            }
            h->row1 = 0;
            ((ICallback *)h->cb)->Columns(arr);

#if HAVE_BOTH_SQLITE
        if (h->is3) {
        mid = (*env)->GetMethodID(env, cls, "types",
                      "([Ljava/lang/String;)V");

        if (mid && h->stmt) {
            arr = (*env)->NewObjectArray(env, ncol,
                         C_java_lang_String, 0);
            for (i = 0; i < ncol; i++) {
            const char *ctype =
                sqlite3_column_decltype(h->stmt, i);

            if (!ctype) {
                switch (sqlite3_column_type(h->stmt, i)) {
                case SQLITE_INTEGER: ctype = "integer"; break;
                case SQLITE_FLOAT:   ctype = "double";  break;
                default:
#if defined(SQLITE_TEXT) && defined(SQLITE3_TEXT) && (SQLITE_TEXT != SQLITE3_TEXT)
                case SQLITE_TEXT:
#else
#ifdef SQLITE3_TEXT
                case SQLITE3_TEXT:
#endif
#endif
                         ctype = "text";    break;
                case SQLITE_BLOB:    ctype = "blob";    break;
                case SQLITE_NULL:    ctype = "null";    break;
                }
            }
            if (ctype) {
                transstr ty;

                trans2utf(env, 1, 0, ctype, &ty);
                (*env)->SetObjectArrayElement(env, arr, i,
                              ty.jstr);
                exc = (*env)->ExceptionOccurred(env);
                if (exc) {
                (*env)->DeleteLocalRef(env, exc);
                return 1;
                }
                (*env)->DeleteLocalRef(env, ty.jstr);
            }
            }
            (*env)->CallVoidMethod(env, h->cb, mid, arr);
            exc = (*env)->ExceptionOccurred(env);
            if (exc) {
            (*env)->DeleteLocalRef(env, exc);
            return 1;
            }
            (*env)->DeleteLocalRef(env, arr);
        }
        } else {
        if (h->ver >= 0x020506 && cols[ncol]) {
            mid = (*env)->GetMethodID(env, cls, "types",
                          "([Ljava/lang/String;)V");

            if (mid) {
            arr = (*env)->NewObjectArray(env, ncol,
                             C_java_lang_String, 0);
            for (i = 0; i < ncol; i++) {
                if (cols[i + ncol]) {
                transstr ty;

                trans2utf(env, h->haveutf, h->enc,
                      cols[i + ncol], &ty);
                (*env)->SetObjectArrayElement(env, arr, i,
                                  ty.jstr);
                exc = (*env)->ExceptionOccurred(env);
                if (exc) {
                    (*env)->DeleteLocalRef(env, exc);
                    return 1;
                }
                (*env)->DeleteLocalRef(env, ty.jstr);
                }
            }
            (*env)->CallVoidMethod(env, h->cb, mid, arr);
            exc = (*env)->ExceptionOccurred(env);
            if (exc) {
                (*env)->DeleteLocalRef(env, exc);
                return 1;
            }
            (*env)->DeleteLocalRef(env, arr);
            }
        }
        }
#else
#if HAVE_SQLITE2
        if (h->ver >= 0x020506 && cols[ncol]) {
        mid = (*env)->GetMethodID(env, cls, "types",
                      "([Ljava/lang/String;)V");

        if (mid) {
            arr = (*env)->NewObjectArray(env, ncol,
                         C_java_lang_String, 0);
            for (i = 0; i < ncol; i++) {
            if (cols[i + ncol]) {
                transstr ty;

                trans2utf(env, h->haveutf, h->enc,
                      cols[i + ncol], &ty);
                (*env)->SetObjectArrayElement(env, arr, i,
                              ty.jstr);
                exc = (*env)->ExceptionOccurred(env);
                if (exc) {
                (*env)->DeleteLocalRef(env, exc);
                return 1;
                }
                (*env)->DeleteLocalRef(env, ty.jstr);
            }
            }
            (*env)->CallVoidMethod(env, h->cb, mid, arr);
            exc = (*env)->ExceptionOccurred(env);
            if (exc) {
            (*env)->DeleteLocalRef(env, exc);
            return 1;
            }
            (*env)->DeleteLocalRef(env, arr);
        }
        }
#endif
#if HAVE_SQLITE3
        if (h->stmt) {
            arr = ArrayOf<String>::Alloc(ncol);
            for (i = 0; i < ncol; i++) {
                const char *ctype = sqlite3_column_decltype(h->stmt, i);

                if (!ctype) {
                    switch (sqlite3_column_type(h->stmt, i)) {
                        case SQLITE_INTEGER:
                            ctype = "integer";
                            break;
                        case SQLITE_FLOAT:
                            ctype = "double";
                            break;
                        default:
#if defined(SQLITE_TEXT) && defined(SQLITE3_TEXT) && (SQLITE_TEXT != SQLITE3_TEXT)
                        case SQLITE_TEXT:
#else
#ifdef SQLITE3_TEXT
                        case SQLITE3_TEXT:
#endif
#endif
                            ctype = "text";
                            break;
                        case SQLITE_BLOB:
                            ctype = "blob";
                            break;
                        case SQLITE_NULL:
                            ctype = "null";
                            break;
                    }
                }
                if (ctype) {
                    (*arr)[i] = String(ctype);
                }
            }
            ((ICallback *)h->cb)->Types(arr);
        }
#endif
#endif
    }
    if (data) {
            Boolean rc = FALSE;
            arr = ArrayOf<String>::Alloc(ncol);
            for (i = 0; arr && i < ncol; i++) {
                if (data[i]) {
                    (*arr)[i] = String(data[i]);
                }
            }
            ((ICallback *)h->cb)->Newrow(arr,&rc);
            return rc != FALSE;
        }
    }
    return 0;
}

ECode CVm::Step(
    /* [in] */ ICallback * cb,
    /* [out] */ Boolean * value)
{
#if HAVE_SQLITE_COMPILE
    hvm *v = (hvm *)mHandle;

    if (v && v->vm && v->h) {
    Int32 ret = 0, tmp = 0;
    Int64 ncol = 0;
#if HAVE_SQLITE3
    freemem *freeproc = 0;
    const char **blob = 0;
#endif
    const char **data = 0, **cols = 0;
#if HAVE_BOTH_SQLITE
    if (v->is3) {
        ret = sqlite3_step((sqlite3_stmt *) v->vm);
        if (ret == SQLITE_DONE && v->hh.row1) {
        ncol = sqlite3_column_count((sqlite3_stmt *) v->vm);
        if (ncol > 0) {
            data = calloc(ncol * 3 + 3 + 1, sizeof (char *));
            if (data) {
            data[0] = (const char *) ncol;
            ++data;
            cols = data + ncol + 1;
            blob = cols + ncol + 1;
            freeproc = Free_tab;
            } else {
            ret = SQLITE_NOMEM;
            }
        }
        if (ret != SQLITE_NOMEM) {
            Int32 i = 0;

            for (i = 0; i < ncol; i++) {
            cols[i] =
                sqlite3_column_name((sqlite3_stmt *) v->vm, i);
            }
        }
        } else if (ret == SQLITE_ROW) {
        ncol = sqlite3_data_count((sqlite3_stmt *) v->vm);
        if (ncol > 0) {
            data = calloc(ncol * 3 + 3 + 1, sizeof (char *));
            if (data) {
            data[0] = (const char *) ncol;
            ++data;
            cols = data + ncol + 1;
            blob = cols + ncol + 1;
            freeproc = Free_tab;
            } else {
            ret = SQLITE_NOMEM;
            }
        }
        if (ret != SQLITE_NOMEM) {
            Int32 i = 0;

            for (i = 0; i < ncol; i++) {
            cols[i] =
                sqlite3_column_name((sqlite3_stmt *) v->vm, i);
            if (sqlite3_column_type((sqlite3_stmt *) v->vm, i)
                == SQLITE_BLOB) {
                unsigned char *src = (unsigned char *)
                sqlite3_column_blob((sqlite3_stmt *) v->vm, i);
                Int32 n = sqlite3_column_bytes((sqlite3_stmt *) v->vm,i);

                if (src) {
                data[i] = malloc(n * 2 + 4);
                if (data[i]) {
                    Int32 k;
                    char *p = (char *) data[i];

                    blob[i] = data[i];
                    *p++ = 'X';
                    *p++ = '\'';
                    for (k = 0; k < n; k++) {
                    *p++ = xdigits[src[k] >> 4];
                    *p++ = xdigits[src[k] & 0x0F];
                    }
                    *p++ = '\'';
                    *p++ = '\0';
                }
                }
            } else {
                data[i] = (const char *)
                sqlite3_column_text((sqlite3_stmt *) v->vm, i);
            }
            }
        }
        }
    } else {
        tmp = 0;
        ret = sqlite_step((sqlite_vm *) v->vm, &tmp, &data, &cols);
        ncol = tmp;
    }
#else
#if HAVE_SQLITE2
    tmp = 0;
    ret = sqlite_step((sqlite_vm *) v->vm, &tmp, &data, &cols);
    ncol = tmp;
#endif
#if HAVE_SQLITE3
    ret = sqlite3_step((sqlite3_stmt *) v->vm);
    if (ret == SQLITE_DONE && v->hh.row1) {
        ncol = sqlite3_column_count((sqlite3_stmt *) v->vm);
        if (ncol > 0) {
            data = (const char **)calloc(ncol * 3 + 3 + 1, sizeof (char *));
            if (data) {
                data[0] = (const char *) ncol;
                ++data;
                cols = data + ncol + 1;
                blob = cols + ncol + 1;
                freeproc = Free_tab;
            } else {
                ret = SQLITE_NOMEM;
            }
        }
        if (ret != SQLITE_NOMEM) {
        Int32 i = 0;
        for (i = 0; i < ncol; i++) {
            cols[i] = sqlite3_column_name((sqlite3_stmt *) v->vm, i);
        }
        }
    } else if (ret == SQLITE_ROW) {
        ncol = sqlite3_data_count((sqlite3_stmt *) v->vm);
        if (ncol > 0) {
            data = (const char **)calloc(ncol * 3 + 3 + 1, sizeof (char *));
            if (data) {
                data[0] = (const char *) ncol;
                ++data;
                cols = data + ncol + 1;
                blob = cols + ncol + 1;
                freeproc = Free_tab;
            } else {
                ret = SQLITE_NOMEM;
            }
        }

        if (ret != SQLITE_NOMEM) {
            Int32 i = 0;
            for (i = 0; i < ncol; i++) {
                cols[i] = sqlite3_column_name((sqlite3_stmt *) v->vm, i);
                if (sqlite3_column_type((sqlite3_stmt *) v->vm, i) == SQLITE_BLOB) {
                    unsigned char *src = (unsigned char *)
                        sqlite3_column_blob((sqlite3_stmt *) v->vm, i);
                    Int32 n =
                        sqlite3_column_bytes((sqlite3_stmt *) v->vm, i);
                    if (src) {
                        data[i] = (const char *)malloc(n * 2 + 4);
                        if (data[i]) {
                        Int32 k;
                        char *p = (char *) data[i];

                        blob[i] = data[i];
                        *p++ = 'X';
                        *p++ = '\'';
                        for (k = 0; k < n; k++) {
                            *p++ = xdigits[src[k] >> 4];
                            *p++ = xdigits[src[k] & 0x0F];
                        }
                        *p++ = '\'';
                        *p++ = '\0';
                        }
                    }
                } else {
                data[i] = (char *)
                    sqlite3_column_text((sqlite3_stmt *) v->vm, i);
                }
            }
        }
    }
#endif
#endif
    if (ret == SQLITE_ROW) {
        v->hh.cb = cb;
#if HAVE_BOTH_SQLITE
        if (v->is3) {
        v->hh.stmt = (sqlite3_stmt *) v->vm;
        }
#else
#if HAVE_SQLITE3
        v->hh.stmt = (sqlite3_stmt *) v->vm;
#endif
#endif
        Int32 outcall = Callback((void *) &v->hh, ncol, (char **) data, (char **) cols);
#if HAVE_SQLITE3
        if (data && freeproc) {
            freeproc((void *) data);
        }
#endif
        // exc = (*env)->ExceptionOccurred(env);
        // if (exc) {
        // (*env)->DeleteLocalRef(env, exc);
        // goto dofin;
        // }
        if (!outcall)
        {
            goto dofin;
        }
        *value = TRUE;
        return NOERROR;
    } else if (ret == SQLITE_DONE) {
dofin:
        if (v->hh.row1 && cols) {
        v->hh.cb = cb;
#if HAVE_BOTH_SQLITE
        if (v->is3) {
            v->hh.stmt = (sqlite3_stmt *) v->vm;
        }
#else
#if HAVE_SQLITE3
        v->hh.stmt = (sqlite3_stmt *) v->vm;
#endif
#endif
        Callback((void *) &v->hh, ncol, (char **) 0, (char **) cols);
#if HAVE_SQLITE3
        if (data && freeproc) {
            freeproc((void *) data);
        }
#endif
        // exc = (*env)->ExceptionOccurred(env);
        // if (exc) {
        //     (*env)->DeleteLocalRef(env, exc);
        // }
        }
#if HAVE_BOTH_SQLITE
        if (v->is3) {
        sqlite3_finalize((sqlite3_stmt *) v->vm);
        } else {
        sqlite_finalize((sqlite_vm *) v->vm, 0);
        }
#else
#if HAVE_SQLITE2
        sqlite_finalize((sqlite_vm *) v->vm, 0);
#endif
#if HAVE_SQLITE3
        sqlite3_finalize((sqlite3_stmt *) v->vm);
#endif
#endif
        v->vm = 0;
        *value = FALSE;
        return NOERROR;
    }
#if HAVE_BOTH_SQLITE
    if (v->is3) {
        sqlite3_finalize((sqlite3_stmt *) v->vm);
    } else {
        sqlite_finalize((sqlite_vm *) v->vm, 0);
    }
#else
#if HAVE_SQLITE2
    sqlite_finalize((sqlite_vm *) v->vm, 0);
#endif
#if HAVE_SQLITE3
    sqlite3_finalize((sqlite3_stmt *) v->vm);
#endif
#endif
    mError_code = ret;
    v->vm = 0;
    //throwex(env, "error in step");
    *value = FALSE;
    return E_SQL_EXCEPTION;
    } else {
    //throwex(env, "vm already closed");
        *value = FALSE;
        return E_NULL_POINTER_EXCEPTION;
    }
#else
    //throwex(env, "unsupported");
    *value = FALSE;
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
#endif
    *value = FALSE;
    return NOERROR;
}

ECode CVm::Compile(
    /* [out] */ Boolean * value)
{
#if HAVE_SQLITE_COMPILE
    hvm *v = (hvm *)mHandle;
    void *svm = 0;
    char *err = 0;
#ifdef HAVE_SQLITE2
    char *errfr = 0;
#endif
    const char *tail;
    Int32 ret;

    if (v && v->vm) {
#if HAVE_BOTH_SQLITE
    if (v->is3) {
        sqlite3_finalize((sqlite3_stmt *) v->vm);
    } else {
        sqlite_finalize((sqlite_vm *) v->vm, 0);
    }
#else
#if HAVE_SQLITE2
    sqlite_finalize((sqlite_vm *) v->vm, 0);
#endif
#if HAVE_SQLITE3
    sqlite3_finalize((sqlite3_stmt *) v->vm);
#endif
#endif
    v->vm = 0;
    }
    if (v && v->h && v->h->sqlite) {
    if (!v->tail) {
        *value = FALSE;
        return E_NULL_POINTER_EXCEPTION;
    }
#if HAVE_BOTH_SQLITE
    if (v->is3) {
#if HAVE_SQLITE3_PREPARE_V2
        ret = sqlite3_prepare_v2((sqlite3 *) v->h->sqlite, v->tail, -1,
                     (sqlite3_stmt **) &svm, &tail);
#else
        ret = sqlite3_prepare((sqlite3 *) v->h->sqlite, v->tail, -1,
                  (sqlite3_stmt **) &svm, &tail);
#endif
        if (ret != SQLITE_OK) {
        if (svm) {
            sqlite3_finalize((sqlite3_stmt *) svm);
            svm = 0;
        }
        err = (char *) sqlite3_errmsg((sqlite3 *) v->h->sqlite);
        }
    } else {
        ret = sqlite_compile((sqlite *) v->h->sqlite, v->tail,
                 &tail, (sqlite_vm **) &svm, &errfr);
        if (ret != SQLITE_OK) {
        err = errfr;
        if (svm) {
            sqlite_finalize((sqlite_vm *) svm, 0);
            svm = 0;
        }
        }
    }
#else
#if HAVE_SQLITE2
    ret = sqlite_compile((sqlite *) v->h->sqlite, v->tail,
                 &tail, (sqlite_vm **) &svm, &errfr);
    if (ret != SQLITE_OK) {
        err = errfr;
        if (svm) {
        sqlite_finalize((sqlite_vm *) svm, 0);
        svm = 0;
        }
    }
#endif
#if HAVE_SQLITE3
#if HAVE_SQLITE3_PREPARE_V2
    ret = sqlite3_prepare_v2((sqlite3 *) v->h->sqlite,
                 v->tail, -1, (sqlite3_stmt **) &svm, &tail);
#else
    ret = sqlite3_prepare((sqlite3 *) v->h->sqlite,
                  v->tail, -1, (sqlite3_stmt **) &svm, &tail);
#endif
    if (ret != SQLITE_OK) {
        if (svm) {
        sqlite3_finalize((sqlite3_stmt *) svm);
        svm = 0;
        }
        err = (char *) sqlite3_errmsg((sqlite3 *) v->h->sqlite);
    }
#endif
#endif
    if (ret != SQLITE_OK) {
        mError_code = ret;
        v->tail = 0;
        //throwex(env, err ? err : "error in compile/prepare");
        *value = FALSE;
        return E_SQL_EXCEPTION;
#if HAVE_SQLITE2
        if (errfr) {
        sqlite_freemem(errfr);
        }
#endif
        *value = FALSE;
        return NOERROR;
    }
#if HAVE_SQLITE2
    if (errfr) {
        sqlite_freemem(errfr);
    }
#endif
    if (!svm) {
        v->tail = 0;
        *value = FALSE;
        return E_NULL_POINTER_EXCEPTION;
    }
    v->vm = svm;
    v->tail = (char *) tail;
    v->hh.row1 = 1;
    *value = TRUE;
    return NOERROR;
    } else {
        //throwex(env, "vm already closed");
        *value = FALSE;
        return E_NULL_POINTER_EXCEPTION;
    }

#else
    //throwex(env, "unsupported");
    *value = FALSE;
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
#endif
    *value = FALSE;
    return NOERROR;
}

ECode CVm::Stop()
{
#if HAVE_SQLITE_COMPILE
    Int32 final = 0;
    hvm *v = (hvm *)mHandle;

    if (v) {
    if (v->h) {
        handle *h = v->h;
        hvm *vv, **vvp;

        vvp = &h->vms;
        vv = *vvp;
        while (vv) {
        if (vv == v) {
            *vvp = vv->next;
            break;
        }
        vvp = &vv->next;
        vv = *vvp;
        }
    }
    if (v->vm) {
#if HAVE_BOTH_SQLITE
        if (v->is3) {
        sqlite3_finalize((sqlite3_stmt *) v->vm);
        } else {
        sqlite_finalize((sqlite_vm *) v->vm, 0);
        }
#else
#if HAVE_SQLITE2
        sqlite_finalize((sqlite_vm *) v->vm, 0);
#endif
#if HAVE_SQLITE3
        sqlite3_finalize((sqlite3_stmt *) v->vm);
#endif
#endif
        v->vm = 0;
    }
    free(v);
    mHandle = 0;
    return NOERROR;
    }
    if (!final) {
    //throwex(env, "vm already closed");
        return E_NULL_POINTER_EXCEPTION;
    }
#else
    //throwex(env, "unsupported");
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
#endif
    return NOERROR;
}

ECode CVm::Finalize()
{
#if HAVE_SQLITE_COMPILE
    Int32 final = 1;
    hvm *v = (hvm *)mHandle;

    if (v) {
    if (v->h) {
        handle *h = v->h;
        hvm *vv, **vvp;

        vvp = &h->vms;
        vv = *vvp;
        while (vv) {
        if (vv == v) {
            *vvp = vv->next;
            break;
        }
        vvp = &vv->next;
        vv = *vvp;
        }
    }
    if (v->vm) {
#if HAVE_BOTH_SQLITE
        if (v->is3) {
        sqlite3_finalize((sqlite3_stmt *) v->vm);
        } else {
        sqlite_finalize((sqlite_vm *) v->vm, 0);
        }
#else
#if HAVE_SQLITE2
        sqlite_finalize((sqlite_vm *) v->vm, 0);
#endif
#if HAVE_SQLITE3
        sqlite3_finalize((sqlite3_stmt *) v->vm);
#endif
#endif
        v->vm = 0;
    }
    free(v);
    mHandle = (Int64)0;
    return NOERROR;
    }
    if (!final) {
    //throwex(env, "vm already closed");
        return E_NULL_POINTER_EXCEPTION;
    }
#endif
    return NOERROR;
}

} // namespace SQLite
} // namespace Sql
} // namespace Elastos
