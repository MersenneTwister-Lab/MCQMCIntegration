準モンテカルロ法による高次元数値積分の例
=============================

この例では、
まず#includeとnamespaceの指定をしている。

````{#prepare1 .cpp}
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
````

ここで被積分関数はGentz氏の数値積分評価用の被積分関数のひとつOscillatoryにしている。

* [oscillatory](oscillatory.html)

以下はmain関数になる。超球の体積の公式より求められる値と比較している。

````{#main .cpp}
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
````

100次元を指定した場合の実行結果は以下のようになる。

```
$ ./qmc_largesobol_example 100
Oscillatory dim = 100
value:-6.73256329934908935575e-04
error:5.89032361674119309819e-03
theroritical value:0.00000000000000000000e+00
```
