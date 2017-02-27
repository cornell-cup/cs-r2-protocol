#include <fstream>
#include <iostream>

#include "R2Protocol.h"

using std::string;
using std::vector;

int main(int argc, char ** argv) {
    string action(argv[1]);
    if (action == "decode") {
        std::ifstream fin(argv[2]);
        string contents((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());
        std::vector<char> data(contents.c_str(), contents.c_str() + contents.size() + 1);
    }
    else if (action == "encode") {
        std::ifstream fin(argv[2]);
        string line, key, value;

        R2Protocol::Packet params = {};
        while (fin >> line) {
            int eq = line.find("=");
            if (eq >= 0) {
                key = line.substr(0, eq);
                key = key.erase(key.find_last_not_of(" \n\r\t") + 1);
                value = line.substr(eq + 1, line.length() - eq - 1);
                value = value.erase(value.find_last_not_of(" \n\r\t") + 1);
                if (key == "source") {
                    params.source = value;
                }
                else if (key == "destination") {
                    params.destination = value;
                }
                else if (key == "data") {
                    params.data = std::vector<unsigned char>(value.c_str(), value.c_str() + value.size() + 1);
                }
                else if (key == "checksum") {
                    params.checksum = value;
                }
            }
        }
        R2Protocol::encode(params);
    }
}
