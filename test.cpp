#include <bits/stdc++.h>
using namespace std;

long long ItemNum, MaxWeight;//アイテムの個数と重さ
vector<long long> ItemWeight, ItemValue;

long long CalculateKnapsackIdealValue(void){
    vector<vector<long long>> dp(ItemNum + 10,vector<long long>(MaxWeight + 10, 0));
    for(int i = 0; i < ItemNum; i++){
        for(int w = 0; w <= MaxWeight; w++){
            if(w - ItemWeight[i] >= 0){
                if(dp[i + 1][w] < dp[i][w - ItemWeight[i]] + ItemValue[i]){
                    dp[i + 1][w] = dp[i][w - ItemWeight[i]] + ItemValue[i];
                }
            }
            if(dp[i + 1][w] < dp[i][w]){
                dp[i + 1][w] = dp[i][w];
            }
        }
    }
    
    return dp[ItemNum][MaxWeight];
}

int main(){

    cin >> ItemNum >> MaxWeight;//アイテムの個数と重さの限界を入力
    ItemWeight.resize(ItemNum), ItemValue.resize(ItemNum);

    for(int i = 0; i < ItemNum; i++){
        cin >> ItemWeight[i] >> ItemValue[i];
    }

    cout << CalculateKnapsackIdealValue() << endl;

    return 0;
}