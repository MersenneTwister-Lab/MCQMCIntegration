#include <inttypes.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdio>
#include <cerrno>
#include <cstring>
#include <stdexcept>
#include "sobolpoint.h"

using namespace std;
using namespace MCQMCIntegration;

int main(int argc, char * argv[])
{
    errno = 0;
    if (argc < 4) {
        cout << "usage:" << endl;
        cout << argv[0] << " s m data-file-name"
             << endl;
        return 1;
    }
    uint32_t s = strtoul(argv[1], NULL, 10);
    if (errno) {
        cout << "s must be a number" << endl;
        return -1;
    }
    uint32_t m = strtoul(argv[2], NULL, 10);
    if (errno) {
        cout << "m must be a number" << endl;
        return -1;
    }
    string datafile = argv[3];
    // Open Data File
    ifstream ifs(datafile, ios::in | ios::binary);
    if (!ifs) {
        cout << "can't open " << datafile << endl;
        return -1;
    }
    uint64_t data[s * m];
    bool success = get_sobol_base(ifs, s, m, data);
    ifs.close();
    if (! success) {
        return -1;
    }
    cout << "64" << " ";
    cout << dec << s << " ";
    cout << dec << m << " ";
    for (uint32_t i = 0; i < s * m; i++) {
#if 0
        if (i % m == 0) {
            cout << endl;
        }
#endif
        cout << dec << data[i] << " ";
    }
    cout << endl;
    return 0;
}
