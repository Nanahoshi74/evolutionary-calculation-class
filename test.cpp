#include <bits/stdc++.h>
using namespace std;

long long item_num, max_weight;//アイテムの個数と重さ
vector<long long> item_weight, item_value;
int seed = 0;//乱数のシード

/*-------------------------------------------------------------------------------
   疑似乱数
--------------------------------------------------------------------------------*/
int get_rand_range(int min_val, int max_val ) {
    // 乱数生成器
    static mt19937 mt(seed);//

    // [min_val, max_val] の一様分布整数 (int) の分布生成器
    uniform_int_distribution<int> get_rand_uni_int( min_val, max_val );

    // 乱数を生成
    return get_rand_uni_int(mt);
}

/*--------------------------------------------------------------------------------
    初期データの設定
---------------------------------------------------------------------------------*/

void 

long long CalculateKnapsackIdealValue(void){
    //動的計画法によりナップザック問題の最適解を求める関数
    vector<vector<long long>> dp(item_num + 10,vector<long long>(max_weight + 10, 0));
    for(int i = 0; i < item_num; i++){
        for(int w = 0; w <= max_weight; w++){
            if(w - item_weight[i] >= 0){
                if(dp[i + 1][w] < dp[i][w - item_weight[i]] + item_value[i]){
                    dp[i + 1][w] = dp[i][w - item_weight[i]] + item_value[i];
                }
            }
            if(dp[i + 1][w] < dp[i][w]){
                dp[i + 1][w] = dp[i][w];
            }
        }
    }
    
    return dp[item_num][max_weight];
}

int main(){

    cin >> item_num >> max_weight;//アイテムの個数と重さの限界を入力
    item_weight.resize(item_num), item_value.resize(item_num);

    for(int i = 0; i < item_num; i++){
        cin >> item_weight[i] >> item_value[i];
    }

    const int MAX_GEN = 10;//世代交代数

    

    

    return 0;
}