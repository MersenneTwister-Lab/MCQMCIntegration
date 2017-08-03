#include <iostream>
#include <iomanip>
#include <MCQMCIntegration/DigitalNet.h>

using namespace MCQMCIntegration;
using namespace std;

namespace {
    struct test_data_t {
        DigitalNetID id;
        int s_min;
        int s_max;
        int s;
        int m_min;
        int m_max;
    };

    test_data_t test_data[] = {
        {NX, 4, 32, 4, 10, 18},
        {SOBOL, 2, 21201, 4, 10, 18},
        {NXLW, 4, 32, 4, 10, 18},
        {SOLW, 2, 100, 4, 10, 18}};

    void get_s_minmax(DigitalNetID id, int * min, int * max)
    {
        *min = getSMin(id);
        *max = getSMax(id);
    }

    void get_m_minmax(DigitalNetID id, int s, int * min, int * max)
    {
        *min = getMMin(id, s);
        *max = getMMax(id, s);
    }

    int test()
    {
        size_t size = sizeof(test_data) / sizeof(test_data_t);
        for (size_t i = 0; i < size; i++) {
            int min;
            int max;
            get_s_minmax(test_data[i].id, &min, &max);
            if (min > test_data[i].s_min ||
                max < test_data[i].s_max) {
                cout << "id = " << test_data[i].id << endl;
                cout << "s_min = " << min << endl;
                cout << "s_max = " << max << endl;
                cout << "should be s_min = " << test_data[i].s_min;
                cout << "should be s_max = " << test_data[i].s_max;
                return -1;
            }
            get_m_minmax(test_data[i].id, test_data[i].s, &min, &max);
            if (min > test_data[i].m_min ||
                max < test_data[i].m_max) {
                cout << "id = " << test_data[i].id << endl;
                cout << "s = " << test_data[i].s << endl;
                cout << "m_min = " << min << endl;
                cout << "m_max = " << max << endl;
                cout << "should be m_min = " << test_data[i].m_min;
                cout << "should be m_max = " << test_data[i].m_max;
                return -1;
            }
        }
        return 0;
    }
}

int main()
{
    return test();
}
