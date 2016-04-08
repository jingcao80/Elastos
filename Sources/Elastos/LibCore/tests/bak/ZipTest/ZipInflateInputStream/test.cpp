#include <elastos.h>
#include <stdio.h>
#include <new>
#include <Elastos.Utility.Zip.h>
#include <Elastos.CoreLibrary.h>

using namespace Elastos;
using Elastos::IO::IFile;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IInputStream;
using Elastos::Utility::Zip::IZipFile;
using Elastos::Utility::Zip::CZipFile;
using Elastos::Utility::Zip::IZipEntry;
using Elastos::Utility::Zip::IZipInputStream;
using Elastos::Utility::Zip::CZipInputStream;
using Elastos::Utility::Zip::IInflaterInputStream;
using Elastos::Utility::Zip::CInflaterInputStream;

class EnterLeaveLog
{
public:
    EnterLeaveLog(const char* str)
    {
        mString = String(str);
        printf("\n>>>> ENTER %s ====\n", str);
    }
    ~EnterLeaveLog()
    {
        printf("<<<< LEAVE %s ====\n", mString.string());
    }

private:
    String mString;
};

void ShowAllEntries(const String& zipFilename)
{
    EnterLeaveLog log("Show All entries");

    AutoPtr<IZipFile> pZipFile;
    ECode ec = CZipFile::New(zipFilename, (IZipFile**)&pZipFile);
    if (FAILED(ec)) {
        printf("Cann't open the file %s, ec = %x!\n", zipFilename.string(), ec);
        return;
    }

    AutoPtr<IObjectContainer> container;
    ec = pZipFile->GetEntries((IObjectContainer **) &container);
    if (FAILED(ec)) {
        printf(" >> Cann't Get Entries ec is %x!\n", ec);
        return;
    }

    AutoPtr<IObjectEnumerator> emu;
    container->GetObjectEnumerator((IObjectEnumerator **)&emu);

    String name;
    Boolean hasNext;
    Int64 compressedSize, crc, size, mtime;
    Int32 method;
    Boolean isDirectory;
    while(emu->MoveNext(&hasNext), hasNext) {
        AutoPtr<IInterface> itf;
        emu->Current((IInterface **)&itf);
        AutoPtr<IZipEntry> entry = IZipEntry::Probe(itf);
        entry->IsDirectory(&isDirectory);
        entry->GetName(&name);
        entry->GetCrc(&crc);
        entry->GetSize(&size);
        entry->GetCompressedSize(&compressedSize);
        entry->GetTime(&mtime);
        entry->GetMethod(&method);
        printf(" >> the entry name is %s, isDirectory: %s, time: %lld, method: %d, crc: %lld, compressedSize: %lld, cesize: %lld\n",
            name.string(), isDirectory ? "TRUE" : "FALSE", mtime, method, crc, compressedSize, size);
    }
}

void testZipInputStream(const String& zipFilename)
{
    EnterLeaveLog log("test ZipInputStream");

    AutoPtr<IFileInputStream> pfInputStream;
    AutoPtr<IZipEntry> entry;
    AutoPtr<IZipInputStream> pzInputStream;
    String name;

    ECode ec = CFileInputStream::New(zipFilename, (IFileInputStream**)&pfInputStream);
    if (FAILED(ec)) {
        printf(" >> Cann't open CFileInputStream, ec = %08x\n", ec);
        goto _EXIT_;
    }

    {
        ec = CZipInputStream::New(IInputStream::Probe(pfInputStream.Get()), (IZipInputStream**)&pzInputStream);
        if (FAILED(ec)) {
            printf(" >> Cann't create CZipInputStream, ec = %08x\n", ec);
            goto _EXIT_;
        }

        ec = pzInputStream->GetNextEntry((IZipEntry**)&entry);
        if (FAILED(ec)) {
            printf(" >> Failed to GetNextEntry. %08x\n", ec);
            goto _EXIT_;
        }

        Boolean isDirectory;
        Int64 compressedSize, crc, size, mtime;
        Int32 method;
        while (entry != NULL) {
            entry->IsDirectory(&isDirectory);
            entry->GetName(&name);
            entry->GetName(&name);
            entry->GetCrc(&crc);
            entry->GetSize(&size);
            entry->GetCompressedSize(&compressedSize);
            entry->GetTime(&mtime);
            entry->GetMethod(&method);
            printf(" >> sub entry name is %s, isDirectory: %s, time: %lld, method: %d, crc: %lld, compressedSize: %lld, cesize: %lld\n",
                name.string(), isDirectory ? "TRUE" : "FALSE", mtime, method, crc, compressedSize, size);

            entry = NULL;
            ec = pzInputStream->GetNextEntry((IZipEntry**)&entry);
            printf(" >> ec = %08x, entry: %p\n", ec, entry.Get());
            pzInputStream->CloseEntry();
        }
    }

_EXIT_:
    if (pfInputStream) pfInputStream->Close();
    if (pzInputStream) pzInputStream->Close();
}

int main(int argc, char *argv[])
{
/*
testZip
    |-dir1
    |   |-file1
    |   |-file2
    |-dir2
        |-file1
        |-file2
 */
    String zipFilename("/data/data/com.elastos.runtime/elastos/testZip.zip");

    ShowAllEntries(zipFilename);
    testZipInputStream(zipFilename);

    return 0;
}
