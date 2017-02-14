#include <MCQMCIntegration/MCQMCIntegration.h>
#include <MCQMCIntegration/DigitalNet.h>
#include <iostream>
#include <string>
#include <random>
#include <cmath>

using namespace std;
using namespace MCQMCIntegration;

class Sphere4D {
public:
    Sphere4D(double x, double y, double z, double w, double r) {
        cx = x;
        cy = y;
        cz = z;
        cw = w;
        rl = r;
    }
    double operator()(const double p[]) {
        double x = p[0] - cx;
        double y = p[1] - cy;
        double z = p[2] - cz;
        double w = p[3] - cw;
        double work = sqrt(x * x + y * y + z * z + w * w);
        if (work <= rl) {
            return 1.0;
        } else {
            return 0.0;
        }
    }
private:
    double cx;
    double cy;
    double cz;
    double cw;
    double rl;
};

int main()
{
    Sphere4D sphere4d(0.0, 0.0, 0.0, 0.0, 1.0);
    uint32_t s = 4;
    uint32_t m = 10;
    uint32_t N = 1000;
    uniform_real_distribution<double> dist(0.0, 1.0);
    mt19937_64 mt;
    const MCQMCResult result
        = monte_carlo_integration(s,
                                  m,
                                  N,
                                  sphere4d,
                                  mt,
                                  dist,
                                  99);
    if (isinf(result.error)) {
        cout << "calculation failed" << endl;
        return -1;
    }
    // show result
    // V = (π^2)/2
    cout << "value:" << result.value << endl;
    cout << "error:" << result.error << endl;
    cout << "V/16 :" << M_PI * M_PI / 2.0 / 16.0 << endl;
    return 0;
}
