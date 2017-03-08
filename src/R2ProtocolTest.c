#include "R2Protocol.h"

#include <stdint.h>
#include <stdio.h>

int main(int argc, char ** argv) {
    if (strncmp(argv[1], "decode", 6) == 0) {
        FILE * fin = fopen(argv[2], "rb");
        fseek(fin, 0, SEEK_END);
        uint32_t data_len = ftell(fin);
        rewind(fin);

        uint8_t * data = (uint8_t *) malloc(data_len  * sizeof(uint8_t));
        fread(data, data_len, 1, fin);
        fclose(fin);

        FILE * fout = fopen(argv[3], "wb");

        struct R2ProtocolPacket params;
        if (R2ProtocolDecode(data, data_len, &params) >= 0) {
            fwrite("start\n", 1, 6, fout);
            fwrite("source = ", 1, 9, fout);
            fwrite(params.source, 1, strlen(params.source), fout);
            fwrite("\n", 1, 1, fout);
            fwrite("destination = ", 1, 14, fout);
            fwrite(params.destination, 1, strlen(params.destination), fout);
            fwrite("\n", 1, 1, fout);
            fwrite("id = ", 1, 5, fout);
            fwrite(params.id, 1, strlen(params.id), fout);
            fwrite("\n", 1, 1, fout);
            fwrite("data = ", 1, 7, fout);
            fwrite(params.data, 1, params.data_len, fout);
            fwrite("\n", 1, 1, fout);
            fwrite("checksum = ", 1, 11, fout);
            fwrite(params.checksum, 1, strlen(params.checksum), fout);
            fwrite("\n", 1, 1, fout);
            fwrite("end", 1, 3, fout);
        }
        else {
            fwrite("invalid", 1, 7, fout);
        }
        fclose(fout);
    }
    else if (strncmp(argv[1], "encode", 6) == 0) {
        FILE * fin = fopen(argv[2], "rb");
        char line[4096];
        char key[256], value[256];
        int key_len, value_len;

        struct R2ProtocolPacket params;
        while (fgets(line, sizeof(line), fin)) {
            char * eqPos = strchr(line, '=');
            int line_len = strlen(line);
            if (eqPos != 0) {
                int eq = eqPos - line;
                key_len = eq - 1;
                strncpy(key, line, key_len);
                key[key_len] = 0;
                value_len = line_len - eq - 3;
                strncpy(value, line + eq + 2, value_len);
                value[value_len] = 0;
                if (strncmp(key, "source", 6) == 0) {
                    strncpy(params.source, value, value_len);
                    params.source[value_len] = 0;
                }
                else if (strncmp(key, "destination", 11) == 0) {
                    strncpy(params.destination, value, value_len);
                    params.destination[value_len] = 0;
                }
                else if (strncmp(key, "id", 2) == 0) {
                    strncpy(params.id, value, value_len);
                    params.id[value_len] = 0;
                }
                else if (strncmp(key, "data", 4) == 0) {
                    params.data = (uint8_t *) malloc(value_len * sizeof(uint8_t));
                    memcpy(params.data, value, value_len);
                    params.data_len = value_len;
                }
                else if (strncmp(key, "checksum", 8) == 0) {
                    strncpy(params.checksum, value, value_len);
                    params.checksum[value_len] = 0;
                }
            }
        }
        fclose(fin);

        FILE * fout = fopen(argv[3], "wb");
        uint8_t * output;
        int32_t output_len;
        if ((output_len = R2ProtocolEncode(&params, &output)) >= 0) {
            fwrite(output, 1, output_len, fout);
        }
        else {
            fwrite("invalid", 1, 7, fout);
        }
        fclose(fout);
    }
}
