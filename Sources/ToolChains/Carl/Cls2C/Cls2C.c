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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <windows.h>

//argv[1] is the read file, argv[2] is write file
int main(int argc, char *argv[])
{
	int ch, total=0;
	int len;
    FILE *fRfile, *fWfile;

    fRfile = fopen(argv[1], "rb");
    if(!fRfile) {
        fprintf(stderr, "fopen %s failed!\n", argv[1]);
        return 0;
    }

    fWfile = fopen(argv[2], "wb+");
    if(!fWfile) {
        fprintf(stderr, "fopen %s failed!\n", argv[2]);
        fclose(fRfile);
        return 0;
    }

    fseek(fRfile, 0, SEEK_END);
    len=ftell(fRfile);
    if (len <= 0) {
        fprintf(stderr, "%s length should not be 0!\n", argv[1]);
        fclose(fRfile);
        fclose(fWfile);
        return 0;
    }

    fseek(fRfile, 0, SEEK_SET);
	//len = strtol(argv[1], NULL, 10);
    fprintf(fWfile, "#include<stdint.h>\n");
    fprintf(fWfile, "\n");
    fprintf(fWfile, "typedef struct _ResStruct{\n");
    fprintf(fWfile, "    unsigned int    uSize;\n");
    fprintf(fWfile, "    const char      uClsinfo[%d];\n", len +1);
    fprintf(fWfile, "}ResStruct;\n");
    fprintf(fWfile, "\n");

    fprintf(fWfile, "const ResStruct DllResource __attribute__ ((used,__section__ (\".clsresource\"))) = {\n");
	fprintf(fWfile, "\t");
    fprintf(fWfile, "%d, \n", len);
    //fprintf(fWfile, "\n");
    //fprintf(fWfile, "\n");

	do {
		fprintf(fWfile, "\t\"");
		while ((ch = fgetc(fRfile)) != EOF)
		{
			total++;
			fprintf(fWfile, "\\x%02x",ch);
			if (total % 16 == 0)
				break;
		}
		fprintf(fWfile, "\"\n");
	} while (ch != EOF);

	fprintf(fWfile, "};\n");
	fprintf(fWfile, "\n");
	fprintf(fWfile, "uintptr_t g_pDllResource = reinterpret_cast<uintptr_t>(&DllResource);\n");
	fprintf(fWfile, "\n");

    fclose(fRfile);
    fclose(fWfile);
    return 0;
}
