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
	fprintf(fWfile, "int g_pDllResource = (int)&DllResource;\n");
	fprintf(fWfile, "\n");

    fclose(fRfile);
    fclose(fWfile);
    return 0;
}
