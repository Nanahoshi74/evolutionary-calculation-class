#include <bits/stdc++.h>
using namespace std;

int seed = 0;

long long get_rand_range(long long min_val, long long max_val) {
    // 乱数生成器
    static mt19937 mt(seed);

    // [min_val, max_val] の一様分布整数 (int) の分布生成器
    uniform_int_distribution<long long> get_rand_uni_int( min_val, max_val );

    // 乱数を生成
    return get_rand_uni_int(mt);
}

int main(){
    // long long n = get_rand_range(1, 100);
    long long n = 100;
    long long w = get_rand_range(1, 1e5) * 10;
    cout << n << " " << w << endl;
    for(int i = 0; i < n; i++){
        long long W = get_rand_range(1, w) / 10;
        long long V = get_rand_range(1, 1e9);
        cout << W << " " << V << endl;
    }

    return 0;
}