import struct

f = open('test_data.txt', 'r')
data = f.read()

data = "".join([ch.encode("hex") for line in data for ch in line])

def carry_around_add(a, b):
    c = a + b
    return (c & 0xffff) + (c >> 16)

def checksum(msg):
    s = 0
    for i in range(0, len(msg), 2):
        w = ord(msg[i]) + (ord(msg[i+1]) << 8)
        s = carry_around_add(s, w)
    return ~s & 0xffff

data = data.split()
data = map(lambda x: int(x,16), data)
data = struct.pack("%dB" % len(data), *data)

print ' '.join('%02X' % ord(x) for x in data)
print "Checksum: 0x%04x" % checksum(data)
