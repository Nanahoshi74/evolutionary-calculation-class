#include <bits/stdc++.h>
using namespace std;

long long item_num, max_weight;//アイテムの個数と重さ
vector<long long> item_weight, item_value;
int seed = 0;//乱数のシード
int MAX_GEN = 10;//最大世代交代数
int group_num = 10;//集団のサイズ
vector<vector<int>> chrome, next_chrome;//縦がgroup_num,横がitem_num のサイズになる
vector<int> value_sum, weight_sum;//価値と重りの合計を保存する、初期化でgroup_numで初期化をする
vector<vector<int>> elite;

/*-------------------------------------------------------------------------------
   疑似乱数
--------------------------------------------------------------------------------*/
int get_rand_range(int min_val, int max_val) {
    // 乱数生成器
    static mt19937 mt(seed);

    // [min_val, max_val] の一様分布整数 (int) の分布生成器
    uniform_int_distribution<int> get_rand_uni_int( min_val, max_val );

    // 乱数を生成
    return get_rand_uni_int(mt);
}

int get_rand_range2(int min_val, int max_val) {
    // 乱数生成器
    static mt19937 mt;//シード指定なし

    // [min_val, max_val] の一様分布整数 (int) の分布生成器
    uniform_int_distribution<int> get_rand_uni_int( min_val, max_val );

    // 乱数を生成
    return get_rand_uni_int(mt);
}

/*--------------------------------------------------------------------------------
    初期データの設定
---------------------------------------------------------------------------------*/

void initialize(){
    item_weight.resize(item_num, 0);
    item_value.resize(item_num, 0);
    chrome.resize(group_num, vector<int>(item_num,0)), elite.resize(2, vector<int>(item_num, 0));
    next_chrome.resize(group_num, vector<int>(item_num));
    value_sum.resize(group_num, 0);
    weight_sum.resize(group_num, 0);
    
    //i番目の個体のj番目の染色体の設定、1だったらそのj番目の品物を選ぶ、0だったら選ばない というのをランダムに
    for(int i = 0; i < group_num; i++){
        for(int j = 0; j < item_num; j++){
            chrome[i][j] = get_rand_range(0, 1);
        }
    }
}

void caluculate_evaluation(){
    for(int i = 0; i < group_num; i++){
        for(int j = 0; j < item_num; j++){
            if(chrome[i][j] == 1){
                weight_sum[i] += item_weight[j];
                value_sum[i] += item_value[j];
            }
            if(weight_sum[i] > max_weight){
                value_sum[i] = 0;//もし重さの合計が制限の重さを超えたら価値を0として最低評価とする
            }
        }
    }
    cout << *max_element(value_sum.begin(), value_sum.end()) << endl;
}

/*----------------------------------------------------------------------------------------
    ルーレット選択
------------------------------------------------------------------------------------------*/

void selection(){
    vector<int> ranked_index;//適応度の高いもののインデックスから降順になるような配列
    for(int i = 0; i < group_num; i++){
        ranked_index[i] = i;
    }

    sort(ranked_index.begin(), ranked_index.end(), [&](int i, int j){
        return value_sum[i] > value_sum[j];//適応度が高い個体のインデックス→低い個体のインデックスへソート
    });

    for(int i = 0; i < 2; i++){
        for(int j = 0; j < item_num; j++){
            elite[i][j] = chrome[ranked_index[i]][j];//上位2個体の遺伝子配列をコピー
        }
    }
    //ルーレット選択
    int total_value = 0;
    for(int i = 0; i < group_num; i++){
        total_value += value_sum[i];
    }
    for(int i = 0; i < group_num; i++){
        //次の世代の遺伝子集団を決めていく
        int rondom_value = get_rand_range(0, total_value);
        int tmp_sum = 0;
        for(int j = 0; j < group_num; j++){
            tmp_sum += value_sum[j];
            if(tmp_sum >= rondom_value){
                for(int k = 0; k < item_num; k++){
                    next_chrome[i][k] = chrome[j][k];
                }
                //選ばれた個体が決まったのでbreakで次の個体決めへ
                break;
            }
        }
    }
}

/*----------------------------------------------------------------------------------
    2点交叉
------------------------------------------------------------------------------------*/

void two_point_crossing(){
    for(int i = 0; i < group_num-1; i += 2){
        int crossing = get_rand_range(0,100);
        if(crossing < 95){
            int r1 = get_rand_range(0, item_num - 1);
            int r2 = get_rand_range(r1, item_num-1);
            vector<vector<int>> child(2, vector<int>(item_num));
            
            for(int j = 0; j < item_num; j++){
                if(r1 <= j && j <= r2){
                    child[0][j] = next_chrome[i+1][j];
                    child[1][j] = next_chrome[i][j];
                }
                else{
                    child[0][j] = next_chrome[i][j];
                    child[1][j] = next_chrome[i+1][j];
                }
            }
            for(int j = 0; j < item_num; j++){
                next_chrome[i][j] = child[0][j];
                next_chrome[i+1][j] = child[1][j];
            }
        }
    }
}

/*--------------------------------------------------------------------------------
    突然変異
----------------------------------------------------------------------------------*/
void mutation(){
    for(int i = 0; i < group_num; i++){
        int mutantrate = get_rand_range(0, 100);
        if(mutantrate < 3){
            int m = get_rand_range(0, item_num-1);
            next_chrome[i][m] = (next_chrome[i][m] + 1) % 2;
        }
    }
}

/*-------------------------------------------------------------------------------
   世代交代
----------------------------------------------------------------------------------*/
void change_generation(){
    for(int i = 0; i < group_num; i++){
        for(int j = 0; j < item_num; j++){
            if(i == 0 || i == 1){
                chrome[i][j] = elite[i][j];
            }
            else{
                chrome[i][j] = next_chrome[i][j];
            }
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
    for(int i = 0; i < MAX_GEN; i++){
        
    }

    return 0;
}
