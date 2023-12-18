#include <bits/stdc++.h>
using namespace std;

long long item_num, max_weight;//アイテムの個数と重さ
vector<long long> item_weight, item_value;
random_device seed_gen;
long long seed = 3;
bool use_seed = true;
long long MAX_GEN = 1000;//最大世代交代数
long long group_num = 10000;//集団のサイズ
vector<vector<long long>> chrome, next_chrome;//縦がgroup_num,横がitem_num のサイズになる
vector<long long> value_sum, weight_sum;//価値と重りの合計を保存する、初期化でgroup_numで初期化をする
vector<vector<long long>> elite;
vector<long long> sub_value_sum;
long long generation = 1;
/*-------------------------------------------------------------------------------
   疑似乱数
--------------------------------------------------------------------------------*/
long long get_rand_range(long long min_val, long long max_val) {
    // 乱数生成器
    static mt19937_64 mt(seed_gen());
    static mt19937_64 mt2(seed);

    // [min_val, max_val] の一様分布整数 (int) の分布生成器
    uniform_int_distribution<long long> get_rand_uni_int( min_val, max_val );

    // 乱数を生成
    if(!use_seed) return get_rand_uni_int(mt);
    else return get_rand_uni_int(mt2);
}

/*--------------------------------------------------------------------------------
    初期データの設定
---------------------------------------------------------------------------------*/

void initialize(){
    item_weight.resize(item_num, 0);
    item_value.resize(item_num, 0);
    chrome.resize(group_num, vector<long long>(item_num,0)), elite.resize(2, vector<long long>(item_num, 0));
    next_chrome.resize(group_num, vector<long long>(item_num));
    value_sum.resize(group_num, 0);
    weight_sum.resize(group_num, 0);
    
    //i番目の個体のj番目の染色体の設定、1だったらそのj番目の品物を選ぶ、0だったら選ばない というのをランダムに
    for(int i = 0; i < group_num; i++){
        for(int j = 0; j < item_num; j++){
            chrome[i][j] = get_rand_range(0, 1);
        }
    }
    // for(int i = 0; i < group_num; i++){
    //     for(int j = 0; j < item_num; j++){
    //         cout << chrome[i][j];
    //     }
    //     cout << endl;
    // }
    // exit(0);
}

void caluculate_evaluation(){
    weight_sum.assign(group_num, 0);
    value_sum.assign(group_num, 0);
    sub_value_sum.assign(group_num, 0);
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
    // cout << *max_element(value_sum.begin(), value_sum.end()) << endl;
}

/*----------------------------------------------------------------------------------------
    ルーレット選択
------------------------------------------------------------------------------------------*/

void selection(){
    vector<long long> ranked_index(group_num);//適応度の高いもののインデックスから降順になるような配列
    for(int i = 0; i < group_num; i++){
        ranked_index[i] = i;
    }
    long long zero_cnt = 0;//重量オーバーした個数を数える変数
    for(int i = 0; i < group_num; i++){
        if(value_sum[i] == 0){
            zero_cnt++;
        }
    }
    //重量オーバーした個体の割合が95%を超えていたら重量が低いのを高い評価値とする
    if(((double)zero_cnt / (double)group_num) >= 0.95){
        sort(ranked_index.begin(), ranked_index.end(), [&](int i, int j){
            return weight_sum[i] < weight_sum[j];
        });
    }
    //そうでないなら価値が高い方を高い評価値とする
    else{
        sort(ranked_index.begin(), ranked_index.end(), [&](int i, int j){
            return value_sum[i] > value_sum[j];//適応度が高い個体のインデックス→低い個体のインデックスへソート
        });
    }

    for(int i = 0; i < 2; i++){
        for(int j = 0; j < item_num; j++){
            elite[i][j] = chrome[ranked_index[i]][j];//上位2個体の遺伝子配列をコピー
        }
    }
    //ルーレット選択
    long long total_value = 0;
    for(int i = 0; i < group_num; i++){
        total_value += value_sum[i];
    }
    for(int i = 0; i < group_num; i++){
        //次の世代の遺伝子集団を決めていく
        long long rondom_value = get_rand_range(0, total_value);
        long long tmp_sum = 0;
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
        long long crossing = get_rand_range(0,100);
        if(crossing < 60){
            long long r1 = get_rand_range(0, item_num - 1);
            long long r2 = get_rand_range(r1, item_num-1);
            vector<vector<long long>> child(2, vector<long long>(item_num));
            
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
        long long mutantrate = get_rand_range(0, 100);
        if(mutantrate < 20){
            long long m = get_rand_range(0, item_num-1);
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
    cout << "世代 : " << generation << endl;
    cout << "最大値は : " << *max_element(value_sum.begin(), value_sum.end()) << endl;  
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

    cin >> item_num >> max_weight;
    initialize();//初期設定

    for(int i = 0; i < item_num; i++){
        cin >> item_weight[i] >> item_value[i];
    }

    // cout << "理想の値は" << " " << caalculate_knapsack_ideal_Value() << endl;
    for(int i = 0; i < MAX_GEN; i++){
        caluculate_evaluation();
        selection();
        two_point_crossing();
        mutation();
        change_generation();
        print_chrome();
        generation++;
    }

    // cout << *max_element(value_sum.begin(), value_sum.end()) << endl;
    cout << "理想の値は" << " " << caalculate_knapsack_ideal_Value() << endl;

    return 0;
}
