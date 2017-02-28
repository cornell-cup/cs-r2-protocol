#include <fstream>
#include <iostream>

#include "R2Protocol.h"

using std::string;
using std::vector;

std::string trim(std::string str) {
    int start = str.find_first_not_of(" \n\r\t");
    int end = str.find_last_not_of(" \n\r\t") + 1;
    return str.substr(start, end - start);
}

int main(int argc, char ** argv) {
    string action(argv[1]);
    if (action == "decode") {
        std::ifstream fin(argv[2]);
        std::ofstream fout(argv[3]);
        std::vector<unsigned char> data((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());
        fin.close();
        R2Protocol::Packet params = {};
        if (R2Protocol::decode(data, params) >= 0) {
            fout.write("start\n", 6);
            fout.write("source = ", 9);
            fout.write((char *) &params.source[0], params.source.size());
            fout.write("\n", 1);
            fout.write("destination = ", 14);
            fout.write((char *) &params.destination[0], params.destination.size());
            fout.write("\n", 1);
            fout.write("data = ", 7);
            fout.write((char *) &params.data[0], params.data.size());
            fout.write("\n", 1);
            fout.write("checksum = ", 11);
            fout.write((char *) &params.checksum[0], params.checksum.size());
            fout.write("\n", 1);
            fout.write("end", 3);
        }
        else {
            fout.write("invalid", 7);
        }
        fout.close();
    }
    else if (action == "encode") {
        std::ifstream fin(argv[2]);
        string line, key, value;

        R2Protocol::Packet params = {};
        while (std::getline(fin, line)) {
            int eq = line.find("=");
            if (eq >= 0) {
                key = trim(line.substr(0, eq));
                value = trim(line.substr(eq + 1, line.length() - eq - 1));
                if (key == "source") {
                    params.source = value;
                }
                else if (key == "destination") {
                    params.destination = value;
                }
                else if (key == "data") {
                    params.data = std::vector<unsigned char>(value.c_str(), value.c_str() + value.size());
                }
                else if (key == "checksum") {
                    params.checksum = value;
                }
            }
        }
        fin.close();

        std::ofstream fout(argv[3]);
        std::vector<unsigned char> output;
        if (R2Protocol::encode(params, output) >= 0) {
            fout.write((char *) &output[0], output.size());
        }
        else {
            fout.write("invalid", 7);
        }
        fout.close();
    }
}
