#ifndef _R2_PROTOCOL
#define _R2_PROTOCOL

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef R2_PROTOCOL_MAX_SOURCE_LENGTH
#   define R2_PROTOCOL_MAX_SOURCE_LENGTH 256
#endif
#ifndef R2_PROTOCOL_MAX_DESTINATION_LENGTH
#   define R2_PROTOCOL_MAX_DESTINATION_LENGTH 256
#endif
#ifndef R2_PROTOCOL_MAX_ID_LENGTH
#   define R2_PROTOCOL_MAX_ID_LENGTH 256
#endif
#ifndef R2_PROTOCOL_MAX_CHECKSUM_LENGTH
#   define R2_PROTOCOL_MAX_CHECKSUM_LENGTH 256
#endif
struct R2ProtocolPacket {
    char source[R2_PROTOCOL_MAX_SOURCE_LENGTH];
    char destination[R2_PROTOCOL_MAX_DESTINATION_LENGTH];
    char id[R2_PROTOCOL_MAX_ID_LENGTH];
    uint32_t data_len;
    uint8_t * data;
    char checksum[R2_PROTOCOL_MAX_CHECKSUM_LENGTH];
};

/**
 * Convert a two byte checksum to hex digits (big endian)
 */
void R2ProtocolChecksumToHex(char * str, uint16_t checksum);

/**
 * Compute the IPv4 checksum of data
 */
uint16_t R2ProtocolComputeChecksum(const uint8_t * data, uint32_t start, uint32_t end);

/**
 * Decode the data, returning the index where the data reading finishes, or negative if data failed to be read
 */
int32_t R2ProtocolDecode(const uint8_t * input, uint32_t input_len, struct R2ProtocolPacket * params);

uint32_t R2ProtocolWriteString(uint8_t * buf, const char * str);

uint32_t R2ProtocolWriteByte(uint8_t * buf, uint8_t value);

uint32_t R2ProtocolWriteBytes(uint8_t * buf, uint8_t  * value, uint32_t value_len);

uint32_t R2ProtocolWriteInt(uint8_t * buf, uint32_t value);

/**
 * Encode the data, returning the size of the encoded data, or negative if data failed to be encoded
 */
int32_t R2ProtocolEncode(struct R2ProtocolPacket * params, uint8_t * output, uint32_t output_len);

#endif