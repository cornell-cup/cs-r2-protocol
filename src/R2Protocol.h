#ifndef _R2_PROTOCOL
#define _R2_PROTOCOL

#include <cstdint>
#include <string>
#include <vector>

namespace R2Protocol {
    struct Packet {
        std::string source;
        std::string destination;
        std::vector<uint8_t> data;
        std::string checksum;
    };

    uint16_t computeChecksum(std::vector<uint8_t>& data, uint32_t start, uint32_t end) {
        uint32_t index;
        uint32_t sum = 0;
        for (index = start; index < end - 1; index+=2) {
            sum += data[index] << 8 | data[index];
            if (sum > 0xFFFF) {
                sum -= 0xFFFF;
            }
        }
        if (index < end) { // Odd byte
            sum += data[index] << 8;
            if (sum > 0xFFFF) {
                sum -= 0xFFFF;
            }
        }
        /*
        char hex[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
        char checksum[] = {
            hex[(sum >>  0) & 0xf],
            hex[(sum >>  4) & 0xf],
            hex[(sum >>  8) & 0xf],
            hex[(sum >> 12) & 0xf]
        };
        */
        return (uint16_t) (sum & 0xffff);
    }

    Packet decode(std::vector<uint8_t>& data) {
        Packet params;
        let start = input.indexOf("G00");
        if (start < 0) {
            throw new Error("Cannot find starting sequence");
        }
        let index = start + 3;
        params.start = true;

        let end = false;
        while (!end && index < input.length) {
            let key = String.fromCharCode(input.readUInt8(index++));
            if (key === "S") {
                let len = input.readUInt8(index++);
                params.source = input.slice(index, index + len);
                index += len;
            }
            else if (key === "D") {
                let len = input.readUInt8(index++);
                params.destination = input.slice(index, index + len);
                index += len;
            }
            else if (key === "P") {
                let len = input.readUInt32LE(index);
                index += 4;
                params.data = input.slice(index, index + len);
                index += len;
            }
            else if (key === "G") {
                let len = 2;
                if (input.toString("utf8", index, index + len) === "01") {
                    params.end = true;
                    index += 2;
                    end = true;
                }
            }
            else if (key === "K") {
                const checksum = computeChecksum(input.slice(start, index-1));
                let len = input.readUInt8(index++);
                params.checksum = input.toString("hex", index, index + len);
                if (checksum !== params.checksum) {
                    throw new Error("Checksum does not match " + checksum + " !== " + params.checksum);
                }
                index += len;
            }
        }
        if (params["start"] && params["source"] && params["destination"] && params["data"] && params["checksum"] && params["end"]) {
            return params;
        }
        else {
            console.log([
                params["start"],
                params["source"],
                params["destination"],
                params["data"],
                params["checksum"],
                params["end"]
            ]);
            throw new Error("Missing values");
        }
    }

    uint32_t writeString(std::vector<uint8_t>& buf, uint32_t index, std::string str) {
        buf.insert(buf.begin() + index, str.begin(), str.end());
        return str.length();
    }

    uint32_t writeByte(std::vector<uint8_t>& buf, uint32_t index, uint8_t value) {
        buf[index] = value;
        return 1;
    }

    uint32_t writeBytes(std::vector<uint8_t>& buf, uint32_t index, std::vector<uint8_t>& value) {
        buf.insert(buf.begin() + index, value.begin(), value.end());
        return value.size();
    }

    uint32_t writeInt(std::vector<uint8_t>& buf, uint32_t index, uint32_t value) {
        buf[index  ] = (value >>  0) & 0xff;
        buf[index+1] = (value >>  8) & 0xff;
        buf[index+2] = (value >> 16) & 0xff;
        buf[index+3] = (value >> 24) & 0xff;
        return 4;
    }

    std::vector<uint8_t> encode(Packet params) {
        uint32_t len = 3 + // G00 start
            2 + // S{length1} source
            params.source.length() + // source
            2 + // D{length1} destination
            params.destination.length() + // destination
            5 + // P{length4}
            params.data.size() + // data
            4 + // K{length}{data} checksum
            3; // G01 end
        std::vector<uint8_t> buf(len);
        uint32_t index = 0;
        index += writeString(buf, index, "G00");
        index += writeString(buf, index, "S");
        index += writeByte(buf, index, params.source.length());
        index += writeString(buf, index, params.source);
        index += writeString(buf, index, "D");
        index += writeByte(buf, index, params.destination.length());
        index += writeString(buf, index, params.destination);
        index += writeString(buf, index, "P");
        index += writeInt(buf, index, params.data.size());
        index += writeBytes(buf, index, params.data);
        uint16_t checksum = computeChecksum(buf, 0, index);
        index += writeString(buf, index, "K");
        index += writeByte(buf, index, 2);
        index += writeByte(buf, index, (checksum >> 8) & 0xff);
        index += writeByte(buf, index, (checksum >> 0) & 0xff);
        index += writeString(buf, index, "G01");
        return buf;
    }
}

#endif