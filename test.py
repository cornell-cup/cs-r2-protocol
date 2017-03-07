import filecmp
import os
import subprocess


def checkEqual(f1, f2):
    return filecmp.cmp(f1, f2, shallow=False)

if __name__=="__main__":
    TEST_ENCODED = "data/encoded.data"
    TEST_DECODED = "data/decoded.data"
    TEMP_OUTPUT = "tmp/data"

    if not os.path.isdir("tmp"):
        os.mkdir("tmp")

    implementations = [
        { # javascript
            "name": "JavaScript",
            "execArgs": ["node", "src/R2ProtocolTest.js"]
        },
        { # C++
            "name": "C++",
            "setupArgs": ["g++", "-std=c++11", "src/R2ProtocolTest.cpp", "-I", "src", "-o", "tmp/R2Protocol.exe"],
            "execArgs": ["tmp/R2Protocol.exe"]
        },
        #{ # python
        #    "execArgs": ["python", "src/r2interface-test.py"]
        #}
    ]

    for impl in implementations:
        # Setup the implementation (e.g. compile)
        if "setupArgs" in impl:
            subprocess.call(impl["setupArgs"])

        # Run tests
        print("Testing {}".format(impl["name"]))
        subprocess.call(impl["execArgs"] + ["decode", TEST_ENCODED, TEMP_OUTPUT])
        assert(checkEqual(TEST_DECODED, TEMP_OUTPUT))
        subprocess.call(impl["execArgs"] + ["encode", TEST_DECODED, TEMP_OUTPUT])
        assert(checkEqual(TEST_ENCODED, TEMP_OUTPUT))
        print("Passed {}".format(impl["name"]))

    print("Done")
