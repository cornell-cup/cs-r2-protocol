const fs = require("fs");
const r2i = require("./r2interface.js");

const action = process.argv[2];
const input = fs.readFileSync(process.argv[3]);
let data;
if (action === "decode") {
    const params = r2i.decode(input);
    if (params) {
        data = [
            params.start ? "start" : "",
            params.end ? "end" : ""
        ].join("\n");
    }
    else {
        data = "invalid";
    }
}
else if (action === "encode") {
    const lines = input.toString().split("\n");
    const params = {};
    for (const line of lines) {
        const eq = line.indexOf("=");
        if (eq >= 0) {
            params[line.substring(0, eq).trim()] = line.substring(eq + 1).trim();
        }
        else {
            params[line.trim()] = true;
        }
    }
    if (params["start"] && params["source"] && params["destination"] && params["data"] && params["checksum"] && params["end"]) {
        data = r2i.encode(params);
    }
    else {
        data = "invalid";
    }
}
fs.writeFileSync(process.argv[4], data);