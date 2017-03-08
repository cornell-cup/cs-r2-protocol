import sys
import R2Protocol as r2p

if __name__=="__main__":
    if sys.argv[1] == "decode":
        f = open(sys.argv[2], 'rb')
        data = f.read()
        f.close()

        f = open(sys.argv[3], 'wb')
        params = r2p.decode(data)
        data = ""
        if params is not None:
            data += "start\n"
            data += "source = {:s}\n".format(params["source"])
            data += "destination = {:s}\n".format(params["destination"])
            data += "id = {:s}\n".format(params["id"])
            data += "data = {:s}\n".format(params["data"].decode("ascii"))
            data += "checksum = {:s}\n".format(params["checksum"])
            data += "end"
        else:
            data += "invalid"

        f.write(bytes(data, "ascii"))
        f.close()

    elif sys.argv[1] == "encode":
        f = open(sys.argv[2], 'r')
        params = {}
        for line in f.readlines():
            eq = line.find("=")
            if eq >= 0:
                key = line[:eq].strip()
                value = line[eq+1:].strip()
                params[key] = value

        f.close()

        f = open(sys.argv[3], 'wb')
        if "source" in params and "destination" in params and "id" in params and "data" in params:
            data = r2p.encode(params["source"], params["destination"], params["id"], params["data"])
        else:
            data = "invalid"
        f.write(data)
        f.close()
