module.exports.decode = function(input) {
    const params = {};
    let index = input.indexOf("G00");
    if (index < 0) return undefined;
    index += 3;
    params.start = true;
    
    let end = false;
    while (!end) {
        let key = String.fromCharCode(input.readInt8(index++));
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
            let len = input.readUInt32LE(index++);
            params.destination = input.slice(index, index + len);
            index += len;
        }
        else if (key === "G") {
            let len = 2;
            if (input.slice(index, index + len) === "01") {
                params.end = true;
                index += 2;
                end = true;
            }
        }
    }
    input.toString("utf8", index, index + 3);
    index += 3;
    return 
}

module.exports.encode = function(params) {
    const len = 3 + 2 + params.source.length + 2 + params.destination.length + 5 + params.data.length + 3;
    const buf = new Buffer(len);
    let index = 0;
    index += buf.write("G00", index);
    index += buf.write("S", index);
    index += buf.writeUInt8(params.source.length, index);
    index += buf.write(params.source, index);
    index += buf.write("D", index);
    index += buf.writeUInt8(params.destination.length, index);
    index += buf.write(params.destination, index);
    index += buf.write("P", index);
    index += buf.writeUInt32LE(params.data.length, index);
    index += buf.write(params.data, index);
    index += buf.write("G01", index);
    return buf;
}