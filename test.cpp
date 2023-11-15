#include <bits/stdc++.h>
using namespace std;

int ITEM_NUM, MAX_WEIGHT;//アイテムの個数と重さ
vector<int> itemWeight(ITEM_NUM), itemValue(ITEM_NUM);

int calculate_idealValue(){

}

int main(){

    cin >> ITEM_NUM >> MAX_WEIGHT;//アイテムの個数と重さの限界を入力

    for(int i = 0; i < ITEM_NUM; i++){
        cin >> itemWeight[i] >> itemValue[i];
    }


    return 0;
}