const computeChecksum = module.exports.computeChecksum = function(data) {
    let index = 0;
    let sum = 0;
    while (index < data.length - 1) {
        sum += data.readUInt16BE(index);
        if (sum > 0xFFFF) {
            sum -= 0xFFFF;
        }
        index += 2;
    }
    if (index < data.length) { // Odd byte
        sum += data.readUInt8(index) << 8;
        if (sum > 0xFFFF) {
            sum -= 0xFFFF;
        }
    }
    let checksum = sum.toString(16);
    while (checksum.length < 4) {
        checksum = '0' + checksum;
    }
    return checksum;
};

module.exports.decode = function(input) {
    const params = {};
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
        else if (key === "T") {
            let len = input.readUInt8(index++);
            params.id = input.slice(index, index + len);
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
    if (params["start"] && params["source"] && params["destination"] && params["id"] &&
            params["data"] && params["checksum"] && params["end"]) {
        return params;
    }
    else {
        console.log([
            params["start"],
            params["source"],
            params["destination"],
            params["id"],
            params["data"],
            params["checksum"],
            params["end"]
        ]);
        throw new Error("Missing values");
    }
}

module.exports.encode = function(params) {
    const len = 3 + // G00 start
        2 + // S{length1} source
        params.source.length + // source
        2 + // D{length1} destination
        params.destination.length + // destination
        2 + // T{length1} transaction id
        params.id.length + // transaction id
        5 + // P{length4}
        params.data.length + // data
        4 + // K{length}{data} checksum
        3; // G01 end
    const buf = new Buffer(len);
    let index = 0;
    index += buf.write("G00", index);
    index += buf.write("S", index);
    index  = buf.writeUInt8(params.source.length, index);
    index += buf.write(params.source, index);
    index += buf.write("D", index);
    index  = buf.writeUInt8(params.destination.length, index);
    index += buf.write(params.destination, index);
    index += buf.write("T", index);
    index  = buf.writeUInt8(params.id.length, index);
    index += buf.write(params.id, index);
    index += buf.write("P", index);
    index  = buf.writeUInt32LE(params.data.length, index);
    index += buf.write(params.data, index);
    const checksum = computeChecksum(buf.slice(0, index));
    index += buf.write("K", index);
    index  = buf.writeUInt8(2, index);
    index  = buf.writeUInt16BE(parseInt(checksum, 16), index);
    index += buf.write("G01", index);
    return buf;
}