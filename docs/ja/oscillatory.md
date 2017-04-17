Oscillatory関数
=============
Gentz 氏による数値積分評価用の被積分関数を本プログラムから呼べるようにクラスにしたものである。

* [Integration Test Functions](https://www.sfu.ca/~ssurjano/integration.html)

```{.cpp}
#include <cmath>
#include <random>

/*
 * Kahan summation algorithm
 */
class Kahan {
public:
    Kahan() {
        sum = 0.0;
        c = 0.0;
    }
    void clear() {
        sum = 0.0;
        c = 0.0;
    }
    void add(const double x) {
        const double y = x - c;
        const double t = sum + y;
        c = (t - sum) - y;
        sum = t;
    }
    double get() {
        return sum;
    }
private:
    double c;
    double sum;
};

class Oscillatory {
public:
    Oscillatory(int n, const double ai[]) {
        s = n;
        a = new double[s];
        for (int i = 0; i < s; i++) {
            a[i] = ai[i];
        }
    }
    ~Oscillatory() {
        delete[] a;
    }

    double operator () (const double point[]) {
        Kahan kahan;
        for (int i = 0; i < s; i++) {
            kahan.add(a[i] * point[i]);
        }
        return cos(kahan.get());
    }
    static double set_a(int n, double ai[]) {
        Kahan kahan;
        double t = M_PI / n;
        for (int i = 0; i < n; i++) {
            ai[i] = t;
            kahan.add(t);
        }
        double diff = kahan.get() - M_PI;
        ai[n - 1] -= diff;
        return M_PI;
    }

    static double set_random_a(int n, double ai[]) {
        using namespace std;
        mt19937_64 mt(0);
        uniform_real_distribution<double> ud(0.0, M_PI);
        double p = M_PI / n;
        ai[n / 2] = 0.1;
        for (int i = 0; i < n / 2; i++) {
            double t = ud(mt);
            ai[i] = p + t;
            ai[n - i - 1] = p - t;
        }
        Kahan kahan;
        for (int i = 0; i < n; i++) {
            kahan.add(ai[i]);
        }
        double diff = kahan.get() - M_PI;
        ai[n - 1] -= diff;
        return M_PI;
    }
private:
    int s;
    double *a;
};
```