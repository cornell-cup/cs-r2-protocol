#ifndef _R2_PROTOCOL
#define _R2_PROTOCOL

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct R2ProtocolPacket {
    char source[256];
    char destination[256];
    char id[256];
    uint32_t data_len;
    uint8_t * data;
    char checksum[256];
};

/**
 * Convert a two byte checksum to hex digits (big endian)
 */
void R2ProtocolChecksumToHex(char * str, uint16_t checksum) {
    char hex[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
    str[0] = hex[(checksum >> 12) & 0xf];
    str[1] = hex[(checksum >>  8) & 0xf];
    str[2] = hex[(checksum >>  4) & 0xf];
    str[3] = hex[(checksum >>  0) & 0xf];
}

/**
 * Compute the IPv4 checksum of data
 */
uint16_t R2ProtocolComputeChecksum(const uint8_t * data, uint32_t start, uint32_t end) {
    uint32_t index;
    uint32_t sum = 0;
    for (index = start; index < end - 1; index+=2) {
        sum += (data[index] << 8) | data[index+1];
        if (sum > 0xffff) {
            sum -= 0xffff;
        }
    }
    if (index < end) { // Odd byte
        sum += data[index] << 8;
        if (sum > 0xffff) {
            sum -= 0xffff;
        }
    }
    return (uint16_t) (sum & 0xffff);
}

/**
 * Decode the data, returning the index where the data reading finishes, or negative if data failed to be read
 */
int32_t R2ProtocolDecode(const uint8_t * input, uint32_t input_len, struct R2ProtocolPacket * params) {
    char * start = strstr((char *) input, "G00");
    if (start == 0) {
        return -1;
    }
    //uint32_t start = startPos - input;

    char * index = start + 3;
    char end = 0;
    while (!end && index - (char *) input < input_len) {
        char key = *(index++);
        if (key == 'S') {
            uint8_t len = (uint8_t) (*(index++));
            strncpy(params->source, index, len);
            params->source[len] = 0;
            index += len;
        }
        else if (key == 'D') {
            uint8_t len = (uint8_t) (*(index++));
            strncpy(params->destination, index, len);
            params->destination[len] = 0;
            index += len;
        }
        else if (key == 'T') {
            uint8_t len = (uint8_t) (*(index++));
            strncpy(params->id, index, len);
            params->id[len] = 0;
            index += len;
        }
        else if (key == 'P') {
            uint8_t len = ((uint8_t) (*index)) | ((uint8_t) (*(index + 1)) << 8) |
                    ((uint8_t) (*(index + 2)) << 16) | ((uint8_t) (*(index + 3)) << 24);
            index += 4;
            if (len > params->data_len) {
                len = params->data_len;
            }
            memcpy(params->data, (uint8_t *) index, len);
            params->data_len = len;
            index += len;
        }
        else if (key == 'K') {
            uint16_t computedChecksum = R2ProtocolComputeChecksum((uint8_t *) start, 0, index - 1 - start);
            uint8_t len = (uint8_t) (*(index++));
            uint16_t checksum = ((uint8_t) (*index) << 8) | ((uint8_t) (*(index + 1)));
            index += len;
            if (computedChecksum != checksum) {
                printf("Checksum does not match %04x != %04x\n", computedChecksum, checksum);
                return -1;
            }
            R2ProtocolChecksumToHex(params->checksum, checksum);
        }
        else if (key == 'G') {
            if (index - (char *) input + 2 < input_len &&
                    *(index + 1) == '0' && *(index + 2) == '1') {
                index += 2;
                end = 1;
            }
        }
    }
    return index - (char *) input;
}

uint32_t writeString(uint8_t * buf, const char * str) {
    uint32_t str_len = strlen(str);
    int i;
    for (i = 0; i < str_len; i++) {
        buf[i] = (uint8_t) str[i];
    }
    return str_len;
}

uint32_t writeByte(uint8_t * buf, uint8_t value) {
    buf[0] = value;
    return 1;
}

uint32_t writeBytes(uint8_t * buf, uint8_t  * value, uint32_t value_len) {
    int i;
    for (i = 0; i < value_len; i++) {
        buf[i] = value[i];
    }
    return value_len;
}

uint32_t writeInt(uint8_t * buf, uint32_t value) {
    buf[0] = (value >>  0) & 0xff;
    buf[1] = (value >>  8) & 0xff;
    buf[2] = (value >> 16) & 0xff;
    buf[3] = (value >> 24) & 0xff;
    return 4;
}

/**
 * Encode the data, returning the size of the encoded data, or negative if data failed to be encoded
 */
int32_t R2ProtocolEncode(struct R2ProtocolPacket * params, uint8_t * output, uint32_t output_len) {
    uint32_t len = 3 + // G00 start
        2 + // S{length1} source
        strlen(params->source) + // source
        2 + // D{length1} destination
        strlen(params->destination) + // destination
        2 + // T{length1} transaction id
        strlen(params->id) + // transaction id
        5 + // P{length4}
        params->data_len + // data
        4 + // K{length}{data} checksum
        3; // G01 end
    if (len > output_len) {
        return -2;
    }
    uint32_t index = 0;
    index += writeString(output + index, "G00");
    index += writeString(output + index, "S");
    index += writeByte(output + index, strlen(params->source));
    index += writeString(output + index, params->source);
    index += writeString(output + index, "D");
    index += writeByte(output + index, strlen(params->destination));
    index += writeString(output + index, params->destination);
    index += writeString(output + index, "T");
    index += writeByte(output + index, strlen(params->id));
    index += writeString(output + index, params->id);
    index += writeString(output + index, "P");
    index += writeInt(output + index, params->data_len);
    index += writeBytes(output + index, params->data, params->data_len);
    uint16_t checksum = R2ProtocolComputeChecksum(output, 0, index);
    index += writeString(output + index, "K");
    index += writeByte(output + index, 2);
    index += writeByte(output + index, (checksum >> 8) & 0xff);
    index += writeByte(output + index, (checksum >> 0) & 0xff);
    index += writeString(output + index, "G01");
    return len;
}

#endif