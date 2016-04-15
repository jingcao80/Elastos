#include <elastos.h>
#include <elautoptr.h>
#include <stdio.h>
#include <new>
#include <Elastos.Utility.Zip.h>
#include <Elastos.CoreLibrary.h>
using namespace Elastos;

Int32 deflate(IFile *file, IZipOutputStream *pzos, const char *path, Int32 flag)
{
    printf("#%s, %d\n", __FILE__, __LINE__);

    if (path != NULL) {
        printf("$$$$$$$$$$$$$$$$$$i is %s\n", (const char *)path);
    } else {
        printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
    }

    AutoPtr<IFileInputStream> pfs = NULL;
    ECode ec;
    ec = CFileInputStream::New((IFile *)file, (IFileInputStream**)&pfs);
    if (FAILED(ec)) {
        printf("Cann't open CFileInputStream, ec = %x\n", ec);
        return -1;
    }

    AutoPtr<IBufferedInputStream> pbis = NULL;
    ec = CBufferedInputStream::New((IFileInputStream *) pfs, (IBufferedInputStream**)&pbis);
    if (FAILED(ec)) {
        printf("Cann't open CFileInputStream, ec = %x\n", ec);
        return -1;
    }

    AutoPtr<IZipEntry> entry;
    String name;
    Int32 len;
    file->GetName(&name);
    char entryname[256] = {0};

    if (path != NULL) {
            strcpy(entryname, path);
            printf("the path is %s\n", entryname);
            printf("^^^^^^^^^^ the entry name is %s\n", entryname);
    }
    if (!flag) {
        strcat(entryname, (const char *)name);
    }

    printf("@@@@@@@@@@entry name is %s\n", entryname);

    CZipEntry::New(String(entryname), (IZipEntry**)&entry);
    ec = pzos->PutNextEntry(entry);
    if (FAILED(ec)) {
        printf("PutNextEntry Failed ec = %x\n", ec);
        return -1;
    }
    ArrayOf<Byte> *buffer = ArrayOf<Byte>::Alloc(256);

    ec = pbis->ReadBufferEx(0, 256, buffer,  &len);
    printf("#%s, %d\n", __FILE__, __LINE__);
    while(len != -1 && ec == NOERROR) {
        ec = pzos->WriteBufferEx(0, len, *buffer);
        if (FAILED(ec)) {
            printf("WriteBufferEx Failed! ec = %x\n", ec);
            return -1;
        }
        pzos->Flush();
        len = 0;
        ec = pbis->ReadBufferEx(0, 256, buffer,  &len);
    }
    printf("#%s, %d\n", __FILE__, __LINE__);

    ArrayOf<Byte>::Free(buffer);
    //pzos->Finish();
    return 0;

}

Int32 compress_dir(IFile *file, IZipOutputStream *pzos, const char *path)
{
        Boolean isdir;
       	AutoPtr<IObjectContainer> container;
        AutoPtr<IObjectEnumerator> objenu;
        Boolean hasNext;
        char relname[256] = {0};
        char relnamep[256] = {0};
        if(path !=NULL) {
            strcat(relname, path);
            strcat(relnamep, path);
        }

        if (file->IsDirectory(&isdir), isdir) {
                String dirname;
                file->GetName(&dirname);
                if (path == NULL) {
                    strcpy(relnamep, (const char *) dirname);
                    strcat(relnamep, "/");
                } else {
                    strcat(relnamep, (const char *) dirname);
                    strcat(relnamep, "/");
                }
                strcat(relname, (const char *)dirname);
                strcat(relname, "/");
                printf("the dir name is %s\n", (const char *) relname);
                deflate(file, pzos, relname, 1);
                printf("deflate a new directory\n");
                file->ListFiles((IObjectContainer**)&container);
                container->GetObjectEnumerator((IObjectEnumerator**)&objenu);
                while(objenu->MoveNext(&hasNext), hasNext) {
                    AutoPtr<IInterface> itf;
                    IFile *curfile;
                    objenu->Current((IInterface**)&itf);
                    curfile = IFile::Probe(itf);
                    String name;
                    curfile->GetName(&name);
                    printf("the name is %s\n", (const char *) name);
                    if (curfile->IsDirectory(&isdir), isdir) {
                        compress_dir(curfile, pzos, relname);
                    } else {
                        printf("1---------------------- %s\n", relnamep);
                        deflate(curfile, pzos, relnamep, 0);
                        printf("2---------------------------------\n");
                    }
		}
        } //else {
          //  deflate(file,pzos);
        //}
        printf("complete one directory\n");
        return 0;
}

int main(int argc, char *argv[])
{
    printf("\n\n\n\n\n");
    printf("\n\n\n\n\n");
    printf("\n\n\n\n\n");
    printf("\n\n\n\n\n");
    IFile *file = NULL;
    ECode ec = NOERROR;
    ec = CFile::New(String("/data/data/com.elastos.runtime/elastos/TextViewDemo/"), &file);
/*
    String teststr("////////////////////////////");
    printf("the str is %s", (const char *)teststr);
    String relpath("");
    relpath.Append("TextViewDemo");
    printf("the str is %s\n", (const char *)relpath);
    relpath.Append("/");
    printf("the str is %s\n", (const char *)relpath);
    printf("-------------------------------dfdfd\n");
*/
    AutoPtr<IFileOutputStream> pfo = NULL;
    ec = CFileOutputStream::New(String("/data/data/com.elastos.runtime/elastos.zip"), (IFileOutputStream**)&pfo);
    if (FAILED(ec)) {
        printf("Cann't Create CFileOutputStream, ec = %x\n", ec);
        return -1;
    }


    /*AutoPtr<IBufferedOutputStream> pbos = NULL;
    ec = CBufferedOutputStream::New((IFileOutputStream *)pfo, (IBufferedOutputStream**)&pbos);
    if (FAILED(ec)) {
        printf("Cann't Create CZipOutputStream, ec = %x\n", ec);
        return -1;
    }
    */

    AutoPtr<IZipOutputStream> pzos;
    ec = CZipOutputStream::New(pfo, (IZipOutputStream**)&pzos);
    if (FAILED(ec)) {
        printf("Cann't Create ZipOutputStream %x\n", ec);
        return -1;
    }

    compress_dir(file, pzos, NULL);
    pzos->Finish();
    pzos->Close();
/*    IFile *file = NULL;
    ECode ec = NOERROR;

    ec = CFile::New(String("/data/data/com.elastos.runtime/elastos/android.pdf"), &file);
    if (FAILED(ec)) {
        printf("Cann't Create File\n");
        return -1;
    }


    IFileInputStream *pfs = NULL;
    ec = CFileInputStream::New((IFile *)file, &pfs);
    if (FAILED(ec)) {
    	printf("Cann't open CFileInputStream, ec = %x\n", ec);
        return -1;
    }

    IBufferedInputStream *pbis = NULL;
    ec = CBufferedInputStream::New((IFileInputStream *) pfs, &pbis);
    if (FAILED(ec)) {
    	printf("Cann't open CFileInputStream, ec = %x\n", ec);
        return -1;
    }


    ArrayOf<Byte> *buffer = ArrayOf<Byte>::Alloc(1024);
    Int32 len;

    IFileOutputStream *pfo = NULL;
    ec = CFileOutputStream::New(String("/data/data/com.elastos.runtime/elastos/android.zip"), &pfo);
    if (FAILED(ec)) {
    	printf("Cann't Create CFileOutputStream, ec = %x\n", ec);
        return -1;
    }


    IBufferedOutputStream *pbos = NULL;
    ec = CBufferedOutputStream::New((IFileOutputStream *)pfo, &pbos);
    if (FAILED(ec)) {
    	printf("Cann't Create CZipOutputStream, ec = %x\n", ec);
        return -1;
    }

    IZipOutputStream *pzos = NULL;
    ec = CZipOutputStream::New(pbos, &pzos);
    if (FAILED(ec)) {
    	printf("Cann't Create CZipOutputStream, ec = %x\n", ec);
        return -1;
    }


    printf("%s, %d\n", __FILE__, __LINE__);
        IZipEntry *entry = NULL;
        ec = CZipEntry::New(String("android.pdf"), &entry);
        if (FAILED(ec)) {
            printf("%s, %d\n", __FILE__, __LINE__);
            return -1;
        } else {
            printf("%s, %d\n", __FILE__, __LINE__);
        }
        ec = pzos->PutNextEntry(entry);
        if (FAILED(ec)) {
            printf("%s, %d\n", __FILE__, __LINE__);
            return -1;
        } else {
            printf("PutEntry Successed\n");
        }
    ec = pbis->ReadBufferEx(0, 256, buffer,  &len);

        while(len != -1 && ec == NOERROR) {
        ec = pzos->WriteBufferEx(0, len, *buffer);
        if (FAILED(ec)) {
            printf("WriteBufferEx Failed! ec = %x\n", ec);
            return -1;
        }
        printf("%s, %d\n", __FILE__, __LINE__);
        pzos->Flush();
        //pzos->Finish();
        len = 0;
        ec = pbis->ReadBufferEx(0, 256, buffer,  &len);
        //printf("&&&&&&&&%s, %d,ec = %x, len = %d\n", __FILE__, __LINE__, ec, len );
    }


    pzos->Finish();
    pbis->Close();
    pzos->Close();
*/
    return 0;
}
