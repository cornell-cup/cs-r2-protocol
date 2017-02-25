import filecmp
import subprocess


def checkEqual(f1, f2):
    return filecmp.cmp(f1, f2, shallow=False)

if __name__=="__main__":
    TEST_ENCODED = "data/encoded.data"
    TEST_DECODED = "data/decoded.data"
    TEMP_OUTPUT = "tmp.data"
    
    implementations = [
        { # javascript
            "execArgs": ["node", "src/r2interface-test.js"]
        },
        #{ # python
        #    "execArgs": ["python", "src/r2interface-test.py"]
        #}
    ]
    
    for impl in implementations:
        # Setup the implementation (e.g. compile)
        if "setupArgs" in impl:
            subprocess.call(impl.setupArgs)
        
        # Run tests
        subprocess.call(impl["execArgs"] + ["decode", TEST_ENCODED, TEMP_OUTPUT])
        assert(checkEqual(TEST_DECODED, TEMP_OUTPUT))
        subprocess.call(impl["execArgs"] + ["encode", TEST_DECODED, TEMP_OUTPUT])
        assert(checkEqual(TEST_ENCODED, TEMP_OUTPUT))
    
    print("Done")
