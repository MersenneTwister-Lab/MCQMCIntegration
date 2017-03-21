#include <MCQMCIntegration/MCQMCIntegration.h>
#include <MCQMCIntegration/DigitalNet.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>
#include <stdlib.h>
#include <errno.h>
#include "oscillatory.hpp"

using namespace std;
using namespace MCQMCIntegration;

int main(int argc, char * argv[])
{
    if (argc <= 1) {
        cout << argv[0] << " dim" << endl;
        return -1;
    }
    errno = 0;
    int dim = strtoul(argv[1], NULL, 10);
    if (errno) {
        cout << "dim must be a number." << endl;
        return -1;
    }
    double a[dim];
    //Oscillatory::set_a(dim, a);
    Oscillatory::set_random_a(dim, a);
    Oscillatory osc(dim, a);
    uint32_t s = dim;
    uint32_t m = 10;
    uint32_t N = 100;
    cout << "Oscillatory dim = " << dec << dim << endl;
    const MCQMCResult result
        = quasi_monte_carlo_integration(N,
                                        osc,
                                        SOBOL,
                                        s,
                                        m,
                                        99);
    if (isinf(result.error)) {
        cout << "calculation failed" << endl;
        return -1;
    }
    // show result
    cout << scientific << setprecision(20);
    cout << "value:" << result.value << endl;
    cout << "error:" << result.error << endl;
    cout << "theroritical value:" << 0.0 << endl;
    return 0;
}
