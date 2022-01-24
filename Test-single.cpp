#include <string>
#include <fstream>
#include <iostream>
#include <memory>
#include <chrono>
#include "StringDictionary.h"

using namespace std;

const char BAR_CHAR = '|';
const int BAR_WIDTH = 60;

string prog;

map <string, uint32_t> ext_map = {
    {"pfc", PFC},
    {"hhtfc", HHTFC},
    {"rphtfc", RPHTFC},
    {"fmi", FMINDEX},
    };

void printUsage(){
    cerr << "Usage: " << endl;
    cerr << prog << " <compressed-dict> <uncompressed-dict>" << endl;
    exit(1);
}

void printBar(int progress){
    int left = progress*BAR_WIDTH/100;
    int right = BAR_WIDTH - left;
    if(progress < 10) cerr << " ";
    cerr << '\r' << progress << "% [";
    for(int i=0; i<left; i++) cerr << BAR_CHAR;
    for(int i=0; i<right; i++) cerr << " ";
    cerr << "]";
    cerr.flush();
}

uint64_t getStreamSize(ifstream &st){
    st.seekg(0, st.end);
    uint64_t ret = st.tellg();
    st.seekg(0, st.beg);
    return ret;
}

int main(int argc, char *argv[]){
    prog = argv[0];
    if(argc != 3){
        printUsage();
    }

    /* Parse input and create streams */
    string compDictFilename = argv[1];
    string rawDictFilename = argv[2];

    ifstream rawDictStream(rawDictFilename);
    ifstream compDictStream(compDictFilename);
    
    if(!rawDictStream.good()){
        cout << "Could not find or open " << rawDictFilename << ".\n";
        exit(1);
    } 
    if(!compDictStream.good()){
        cout << "Could not find or open " << compDictFilename << ".\n";
        exit(1);
    } 

    /* Create dictionary object */
    string ext = compDictFilename.substr(compDictFilename.find_last_of('.')+1);
    if(ext_map.find(ext) == ext_map.end()){
        cout << "Invalid extension for the compressed dictionary filename.\n";
        exit(1);
    }
	unique_ptr<StringDictionary> dict = StringDictionary::load(compDictStream, ext_map[ext]);

    /* Testing */
    using namespace std::chrono;

    steady_clock::duration locTimeDur(0);
    steady_clock::duration extTimeDur(0);

    uint64_t readStrings=0;
    uint64_t readBytes=0;
    uint64_t totalBytes = getStreamSize(rawDictStream);

    string cur;

    int progress=0;
    steady_clock::time_point testStart = steady_clock::now();
    while(getline(rawDictStream, cur, '\0')){
        // Some declarations
        steady_clock::time_point t0, t1;
        uint64_t locId; // located id
        uchar *extractedPtr; // extracted string
        uint_fast32_t strLen = cur.size();
        uchar *curPtr = (uchar*)&cur[0];

        // Run locate
        t0 = steady_clock::now();
        locId = dict->locate(curPtr, strLen);
        t1 = steady_clock::now();
        locTimeDur += t1-t0;

        // Run extract
        t0 = steady_clock::now();
        extractedPtr = dict->extract(locId, &strLen);
        t1 = steady_clock::now();
        extTimeDur += t1-t0;
        // Check extract result
        if(strLen != cur.size()){
            cout << "Extract error: returned size is not the expected.\n";
            cout << "\tReturned size: " << strLen << '\n';
            cout << "\tExpected size: " << cur.size() << '\n';
            cout << "String is " << cur << '\n';
            cout << "ID is " << locId << '\n';
            exit(1);
        }
        if(cur != (char*)extractedPtr){
            cout << "Extract error: extracted string is not the expected.\n";
            cout << "\tExtracted string: " << string((char*)extractedPtr, strLen) << '\n';
            cout << "\tExpected string: " << cur << '\n';
            cout << "ID is " << locId << '\n';
            exit(1);
        }
        
        readStrings++;
        readBytes += cur.size()+1;
        if(int(100*readBytes/totalBytes) > progress){
            progress = 100*readBytes/totalBytes;
            printBar(progress);
        }
    }
    cerr << '\n';
    steady_clock::time_point testEnd = steady_clock::now();
    

    double locTime = duration_cast<microseconds>(locTimeDur).count();
    double extTime = duration_cast<microseconds>(extTimeDur).count();

    cout << "Test successful:\n";
    cout << "\tTested strings: " << readStrings << '\n';
    cout << "\tTotal test time: " << duration_cast<seconds>(testEnd-testStart).count() << " [seconds]\n";
    cout << "\tTotal locate time: " << locTime << " [microsec]\n";
    cout << "\tMean locate time: " << locTime/readStrings << " [microsec]\n";
    cout << "\tTotal extract time: " << extTime << " [microsec]\n";
    cout << "\tMean extract time: " << extTime/readStrings << " [microsec]\n";
    return 0;
}