#include <bits/stdc++.h>
using namespace std;

int ITEM_NUM, MAX_WEIGHT;//アイテムの個数と重さ
vector<int> itemWeight, itemValue;

int calculate_idealValue(void){
    return 1;
}

int main(){

    cin >> ITEM_NUM >> MAX_WEIGHT;//アイテムの個数と重さの限界を入力
    itemWeight.resize(ITEM_NUM), itemValue.resize(ITEM_NUM);

    for(int i = 0; i < ITEM_NUM; i++){
        cin >> itemWeight[i] >> itemValue[i];
    }


    return 0;
}