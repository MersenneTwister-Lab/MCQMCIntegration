#include <iostream>
#include <iomanip>
#include <MCQMCIntegration/DigitalNet.h>

using namespace MCQMCIntegration;
using namespace std;

namespace {
    struct test_data_t {
        DigitalNetID id;
        double data[2][4];
    };

    test_data_t test_data[] = {
        {NX, {{0, 0, 0, 0}, {0.611765, 0.6, 0.859375, 0.411765}}},
        {SOBOL, {{0, 0, 0, 0}, {0.5, 0.5, 0.5, 0.5}}},
        {NXLW, {{0, 0, 0, 0}, {0.878349, 0.663337, 0.750795, 0.434933}}},
        {SOLW, {{0, 0, 0, 0}, {0.674217, 0.522469, 0.777076, 0.645491}}},
    };

    int check(int dim, const double point[], const double expect[])
    {
        const double eps = 0.000001;
        bool match = true;
        for (int i = 0; i < dim; i++) {
            if (abs(point[i] - expect[i]) > eps) {
                match = false;
                break;
            }
        }
        if (match) {
            return 0;
        }
        cout << "point = (";
        for (int i = 0; i < dim; i++) {
            cout << point[i] << " ";
        }
        cout << ")" << endl;
        cout << "expected = (";
        for (int i = 0; i < dim; i++) {
            cout << expect[i] << " ";
        }
        cout << ")" << endl;
        return -1;
    }

    int test()
    {
        size_t size = sizeof(test_data) / sizeof(test_data_t);
        for (size_t i = 0; i < size; i++) {
            DigitalNet<uint64_t> dn(test_data[i].id, 4, 10);
            const double *point = dn.getPoint();
            int r = check(4, point, test_data[i].data[0]);
            if (r < 0) {
                cout << "id = " << test_data[i].id << endl;
                return -1;
            }
            dn.nextPoint();
            r = check(4, point, test_data[i].data[1]);
            if (r < 0) {
                cout << "id = " << test_data[i].id << endl;
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
