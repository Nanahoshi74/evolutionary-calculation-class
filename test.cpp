#include <bits/stdc++.h>
using namespace std;

long long item_num, max_weight;//アイテムの個数と重さ
vector<long long> item_weight, item_value;
int seed = 0;//乱数のシード
int MAX_GEN = 10;//最大世代交代数
int group_num = 10;//集団のサイズ
vector<vector<int>> chrome;//縦がgroup_num,横がitem_num のサイズになる

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

void initialize(){
    item_weight.resize(item_num,0);
    item_value.resize(item_num,0);
    chrome.resize(group_num,vector<int>(item_num,0));
    
    //i番目の個体のj番目の染色体の設定、1だったらそのj番目の品物を選ぶ、0だったら選ばない というのをランダムに
    for(int i = 0; i < group_num; i++){
        for(int j = 0; j < item_num; j++){
            chrome[i][j] = get_rand_range(0,1);
        }
    }
}

/*--------------------------------------------------------------------------------
    データ表示用関数
---------------------------------------------------------------------------------*/

void print_chrome(){
    vector<int> sum_weight(group_num), sum_value(group_num);
    for(int i = 0; i < group_num; i++){
        for(int j = 0; j < item_num; j++){
            if(chrome[i][j] == 1){
                sum_weight[i] += item_weight[j];
                sum_value[i] += item_value[j];
            }
        }
    }
    cout << "総容量 : [";
    for(int i = 0; i < group_num; i++){
        cout << sum_weight[i] << ", ";
    }
    cout << "]" << endl;
    cout << "総価値 : [";
    for(int i = 0; i < group_num; i++){
        cout << sum_value[i] << ", ";
    }
    cout << "]" << endl;
}


long long caalculate_knapsack_ideal_Value(void){
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

    cin >> item_num;
    initialize();//初期設定

    for(int i = 0; i < item_num; i++){
        cin >> item_weight[i] >> item_value[i];
    }



    return 0;
}
