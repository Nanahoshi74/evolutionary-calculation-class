#include <bits/stdc++.h>
using namespace std;

int item_num, max_weight;//アイテムの個数と重さ
vector<long long> item_weight, item_value;
random_device seed_gen;
const int seed = 10;//シードの値
bool use_seed = true;//シードを使うかどうか
const int MAX_GEN = 1000;//最大世代交代数
const int group_num = 10000;//集団のサイズ
const int crossing_persent = 95;//交叉率
const int mutation_persent = 20;//突然変異率
const int tournament_size = 10;//トーナメント選択のトーナメントサイズ
vector<vector<int>> chrome, next_chrome;//縦がgroup_num,横がitem_num のサイズになる
vector<long long> value_sum, weight_sum;//価値と重りの合計を保存する、初期化でgroup_numで初期化をする
vector<vector<int>> elite;//エリート個体を保存するための変数
int generation = 1;//世代カウント用の変数
bool is_Exist_Ideal_Value = false;//理想の値をもつ個体が全体の中で存在したかどうか
long long max_value = -1;//全世代の中の価値の最大値
int first_appear_index = -1;

//グラフ作成のためのcsv読み込み
ofstream ofs1("/home/nanahoshi74/evolutionary-calculation-class/case2_一様交叉_突然変異20%_ルーレット選択.csv");

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

    参考プログラムとの違い
        - アイテムの重さや価値、個数、制限重量は他のプログラムから作成
        - 0, 1はランダムで選ぶがシードは固定(以後のランダムもシード固定)
        - 初期化用関数にした
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

/*-----------------------------------------------------------------------------
   評価計算
   参考プログラムとの違い
        - 関数化した 
        - 標準出力はしていない
-------------------------------------------------------------------------------*/

void caluculate_evaluation(){
    weight_sum.assign(group_num, 0);
    value_sum.assign(group_num, 0);

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
}

/*----------------------------------------------------------------------------------------
    ルーレット選択
    参考プログラムとの違い
        - エリート選択の方法はsort して記述量削減
        - もし価値合計が0の割合が集団の95%以上なら重さの低いものからエリートとする
        - 関数化した
------------------------------------------------------------------------------------------*/

void roulette_selection(){
    vector<int> ranked_index(group_num);//適応度の高いもののインデックスから降順になるような配列
    for(int i = 0; i < group_num; i++){
        ranked_index[i] = i;
    }
    int zero_cnt = 0;//重量オーバーした個数を数える変数
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
   トーナメント方式
   参考プログラムとの違い
        - トーナメント戦略を自分で実装した
        - エリート選択の方法はルーレット選択に記載のと方法と同じ方法を採用
        - 関数化した
------------------------------------------------------------------------------------*/

void tournament_selection(){
    vector<int> ranked_index(group_num);//適応度の高いもののインデックスから降順になるような配列
    for(int i = 0; i < group_num; i++){
        ranked_index[i] = i;
    }
    int zero_cnt = 0;//重量オーバーした個数を数える変数
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
    //トーナメント選択
    for(int i = 0; i < group_num; i++){
        int selected_index = 0;
        long long selected_value = -1;
        for(int j = 0; j < tournament_size; j++){
            int tmp_index = get_rand_range(0, group_num-1);
            long long tmp_value = value_sum[tmp_index];
            if(tmp_value > selected_value){
                selected_value = tmp_value;
                selected_index = tmp_index;
            }
        }
        for(int j = 0; j < item_num; j++){
            next_chrome[i][j] = chrome[selected_index][j];
        }
    }
}

/*----------------------------------------------------------------------------------
    2点交叉
    参考プログラムとの違い
        - 交叉率をグローバル変数で設定できるようにした
        - 関数化した
------------------------------------------------------------------------------------*/

void two_point_crossing(){
    for(int i = 0; i < group_num-1; i += 2){
        int crossing = get_rand_range(0, 100);
        if(crossing < crossing_persent){
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

/*-------------------------------------------------------------------------------
    一様交叉
    参考プログラムとの違い
        - 一様交叉を自分で実装した
        - 関数化した
---------------------------------------------------------------------------------*/

void binomial_crossover(){
    for(int i = 0; i < group_num-1; i += 2){
        int crossing = get_rand_range(0, 100);
        if(crossing < crossing_persent){
            vector<vector<int>> child(2, vector<int>(item_num));
            vector<int> mask(item_num);
            for(int j = 0; j < item_num; j++){
                int p = get_rand_range(0, 100);
                if(p <= 50){
                    mask[j] = 1;
                }
                else{
                    mask[j] = 0;
                }
            }
            for(int j = 0; j < item_num; j++){
                if(mask[j] == 1){
                    child[0][j] = next_chrome[i][j];
                    child[1][j] = next_chrome[i+1][j];
                }
                else{
                    child[1][j] = next_chrome[i+1][j];
                    child[0][j] = next_chrome[i][j];
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
    参考プログラムとの違い
        - 突然変異率をグローバル変数で設定できるようにした
        - 関数化した
----------------------------------------------------------------------------------*/
void mutation(){
    for(int i = 0; i < group_num; i++){
        int mutantrate = get_rand_range(0, 100);
        if(mutantrate < mutation_persent){
            int m = get_rand_range(0, item_num-1);
            next_chrome[i][m] = (next_chrome[i][m] + 1) % 2;
        }
    }
}

/*-------------------------------------------------------------------------------
   世代交代
   参考プログラムとの違い
        - 関数化した
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
    参考プログラムとの違い
        - ここは独自で実装した
        - 実行中に経過が見やすいようにしたり、CSV出力できるようにした
---------------------------------------------------------------------------------*/

void print_chrome(long long ideal_value){
    cout << "世代 : " << generation << endl;
    max_value = *max_element(value_sum.begin(), value_sum.end());//その世代の価値の最大値
    if(max_value == ideal_value){
        is_Exist_Ideal_Value = true;
        if(first_appear_index == -1){
            first_appear_index = generation;
        }
    }

    long long tmp_sum = 0;
    for(int i = 0; i < group_num; i++){
        tmp_sum += value_sum[i];
    }
    long long average_value = tmp_sum / group_num;//その世代の価値の平均値
    cout << "価値の最大値は : " << max_value << " | ";
    cout << "価値の平均値は : " << average_value << endl;

    //csvファイルに出力する
    ofs1 << generation << ',' << max_value;
    ofs1 << ',' << generation << ',' << average_value << endl;
}

/*-----------------------------------------------------------------
  理論値を動的計画法で求める関数
    - 自分で実装をした
------------------------------------------------------------------*/

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

    cin >> item_num >> max_weight;//アイテムの個数と詰め込める最大の重さを入力
    initialize();//初期設定

    for(int i = 0; i < item_num; i++){
        cin >> item_weight[i] >> item_value[i];//アイテムそれぞれの重さと価値を入力
    }
    long long ideal_value = caalculate_knapsack_ideal_Value();
    cout << "理想の値は" << " " << ideal_value << " です" <<endl;
    sleep(5);//理想の値を見るため5秒停止

    ofs1 << "世代" << ',' << "価値最大値" << ',' << "世代" << ',' << "価値平均" << endl;

    for(int i = 0; i < MAX_GEN; i++){
        caluculate_evaluation();//適応度計算
        roulette_selection();//ルーレット選択
        // tournament_selection();//トーナメント選択
        // two_point_crossing();//二点交叉
        binomial_crossover();//一様交叉
        mutation();//突然変異
        change_generation();//世代交代
        print_chrome(ideal_value);//データ表示用の関数
        generation++;
    }

    cout << "理想の値は" << " " << caalculate_knapsack_ideal_Value() << endl;
    cout << "得られた最大の値は : " << max_value << " でした" << endl;
    if(is_Exist_Ideal_Value){
        cout << "理想の値を初めて持った個体は " << first_appear_index << " 世代に存在しました"<< endl;
    }
    else{
        cout << "理想の値を持つ個体が存在しませんでした" << endl;
    }

    return 0;
}
