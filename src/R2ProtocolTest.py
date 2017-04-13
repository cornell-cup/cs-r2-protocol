import sys
import R2Protocol as r2p

if __name__=="__main__":
    if sys.argv[1] == "decode":
        f = open(sys.argv[2], 'rb')
        data = f.read()
        f.close()

        f = open(sys.argv[3], 'wb')
        params = r2p.decode(data)
        data = b""
        if params is not None:
            data += b"start\n"
            data += b"source = " + params["source"] + b"\n"
            data += b"destination = " + params["destination"] + b"\n"
            data += b"id = " + params["id"] + b"\n"
            data += b"data = " + params["data"] + b"\n"
            data += b"checksum = " + params["checksum"] + b"\n"
            data += b"end"
        else:
            data += b"invalid"

        f.write(data)
        f.close()

    elif sys.argv[1] == "encode":
        f = open(sys.argv[2], 'rb')
        params = {}
        for line in f.readlines():
            eq = line.find(b"=")
            if eq >= 0:
                key = line[:eq].strip().decode("ascii")
                value = line[eq+1:].strip()
                params[key] = value
        
        f.close()

        if "source" in params and "destination" in params and "id" in params and "data" in params:
            data = r2p.encode(params["source"], params["destination"], params["id"], params["data"])
        else:
            data = b"invalid"
        
        f = open(sys.argv[3], 'wb')
        f.write(data)
        f.close()
