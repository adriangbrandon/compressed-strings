#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "dicts/StringDictionary.h"

using namespace std;

string prog;

map<string, uint32_t> ext_map = {
    {"pfc", PFC},
    {"hhtfc", HHTFC},
    {"rphtfc", RPHTFC},
    {"fmi", FMINDEX},
};

void printUsage() {
    cout << "Usage: " << endl;
    cout << prog << " <compressed-dict>" << endl;
    exit(1);
}

uint64_t getStreamSize(ifstream &st) {
    st.seekg(0, st.end);
    uint64_t ret = st.tellg();
    st.seekg(0, st.beg);
    return ret;
}

int main(int argc, char *argv[]) {
    prog = argv[0];
    if (argc != 2) {
        printUsage();
    }

    /* Parse input and create streams */
    string compDictFilename = argv[1];

    ifstream compDictStream(compDictFilename);

    if (!compDictStream.good()) {
        cout << "Could not find or open " << compDictFilename << ".\n";
        exit(1);
    }

    /* Create dictionary object */
    string ext =
        compDictFilename.substr(compDictFilename.find_last_of('.') + 1);
    if (ext_map.find(ext) == ext_map.end()) {
        cout << "Invalid extension for the compressed dictionary filename.\n";
        exit(1);
    }
    unique_ptr<StringDictionary> dict =
        StringDictionary::load(compDictStream, ext_map[ext]);

    /* Testing */
    using namespace std::chrono;

    string query = "";
    while (getline(cin, query)) {
        steady_clock::time_point t0, t1;
        t0 = steady_clock::now();
        query = query + "\1";
        auto it = dict->extractSubstr((uchar *)query.data(), query.size());
        cout << "Results: \n";
        int cnt = 0;
        while (it->hasNext() && cnt < 10) {
            uint_fast32_t len;
            uchar *ret = it->next(&len);
            cout << '\t';
            cout.write((char *)ret, len);
            cout << '\n';
            cnt++;
        }
        t1 = steady_clock::now();
        cout << "Query time: " << duration_cast<microseconds>(t1 - t0).count()
             << " [microsec]" << endl;
    }
    return 0;
}