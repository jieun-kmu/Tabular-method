#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <iostream>
using namespace std;

vector<vector<string>> init(int bitnum, int mintermnum, vector<int> minterm) {
    // 1의 개수에 따른 minterm의 2진수 배열 
    vector<vector<string>> bit(bitnum+1); 
    for (int i = 0; i < mintermnum; i++) {
        int num = minterm[i];
        int onecount = 0;
        int count = 0;
        string binary = "0";
        while(num > 0) {
            if (num%2 == 0) binary.insert(0, "0");
            else { binary.insert(0, "1"); onecount++; }
            num = num/2; 
            count++;
        }
        while (count != bitnum) { binary.insert(0, "0"); count++; }
        bit[onecount].push_back(binary);
    }
    return bit;
}

void deduplicateStr(vector<string>& str) {
        sort(str.begin(), str.end());
        auto last = unique(str.begin(), str.end());
        str.erase(last, str.end());
}

void deduplicateInt(vector<int>& integer) {
        sort(integer.begin(), integer.end());
        auto last = unique(integer.begin(), integer.end());
        integer.erase(last, integer.end());
}

void findPI(vector<vector<string>> bit, int bitnum, vector<string>& answer) {
    vector<vector<string>> next(bit.size()-1);
    for(int i = 0; i < bit.size()-1; i++) {
        for(int j = 0; j < bit[i].size(); j++) {
            for(int k = 0; k < bit[i+1].size(); k++) {
                int diff = 0;
                int diffindex = -1;
                int onecount = 0;
                for(int n = 0; n < bitnum; n++) {
                    if(bit[i][j][n] != bit[i+1][k][n]) {
                        diff++;
                        if(diff == 1) diffindex = n;
                    } else { 
                        if(bit[i][j][n] == '1') onecount++; 
                    }
                } 
                if (diff == 1) {
                    string str = bit[i][j];
                    str[diffindex] = '2';
                    str[bitnum] = '0';
                    next[onecount].push_back(str);
                    bit[i][j][bitnum] = '1';
                    bit[i+1][k][bitnum] = '1';
                }
            }
        }
    }
    // 기존 테이블에서 combine 되지 않은 string push
    int exit = 0;
    for(int i = 0; i < bit.size(); i++) {
        for(int j = 0; j < bit[i].size(); j++) {
            if(bit[i][j][bitnum] == '0') {
                bit[i][j].pop_back();
                answer.push_back(bit[i][j]);
            } else exit++;
        }
    }
    // 다음 테이블 정렬 및 중복 제거
    for(int i = 0; i < next.size(); i++) deduplicateStr(next[i]);
    if (exit == 0 || bit.size() < 2) { sort(answer.begin(), answer.end()); return; }
    findPI(next, bitnum, answer);
}

void findEPI(vector<vector<string>> bit, vector<string>& answer, vector<vector<int>>& epi, vector<string>& exceptepi) {
    // minterm의 2진수 배열
    vector<string> bitlist;
    for(int i = 0; i < bit.size(); i++) {
        for(int j = 0; j < bit[i].size(); j++) {
            bit[i][j].pop_back();
            bitlist.push_back(bit[i][j]);
        }
    } sort(bitlist.begin(), bitlist.end());
    // epi 표 생성
    for(int i = 0; i < answer.size(); i++) {
        for(int j = 0; j < bitlist.size(); j++) {
            int check = 0;
            for(int k = 0; k < answer[i].size(); k++) {
                if(answer[i][k] != '2' && answer[i][k] != bitlist[j][k]) check++;
            } if (check == 0) epi[i][j] = 1;
        }
    }
    // epi 찾기
    vector<string> epiAnswer; // epi로 출력될 pi
    vector<int> epiIdx; // epi로 선택된 pi의 idx
    for(int j = 0; j < bitlist.size(); j++) {
        int count = 0;
        int idx;
        string str;
        for(int i = 0; i < answer.size(); i++) {
            if(epi[i][j] == 1) {
                count++;
                str = answer[i];
                idx = i;
            }
        }
        if(count == 1) {
            epiAnswer.push_back(str);
            epiIdx.push_back(idx);
        } 
    }
    deduplicateStr(epiAnswer);
    deduplicateInt(epiIdx);
    // epi 삭제 -> epi 표에서 epi 및 관련 minterm 제거
    // epi와 관련된 경우 값을 0으로
    for (int i = 0; i < epiIdx.size(); i++) {
        for (int j = 0; j < bitlist.size(); j++) {
            if (epi[epiIdx[i]][j] == 1) {
                for (int k = 0; k < epi.size(); k++) epi[k][j] = 0;
            }
        } 
    }
    // epi 배열에서 전체 0인 pi 제거
    for (int i = 0; i < epi.size(); i++) {
        int count = 0;
        for (int j = 0; j < epi[i].size(); j++) if (epi[i][j] == 1) count++;
        if (count == 0) {
            exceptepi.erase(exceptepi.begin()+i);
            epi.erase(epi.begin()+i);
        }
    }
    answer.push_back("EPI");
    for (int i = 0; i < epiAnswer.size(); i++) {
        answer.push_back(epiAnswer[i]);
    }
}

void findRow(vector<vector<int>> epi, vector<string> exceptepi, vector<string>& answer) {
    vector<pair<int, int>> rowpair;
    vector<int> row;
    vector<int> rowdelete;
    int in, out, icount, jcount;
    answer.push_back("ROW");
    if(epi.empty()) return;
    for(int i = 0; i < epi.size()-1; i++) {
        for(int j = i+1; j < epi.size(); j++) {
            in = out = icount = jcount = 0;
            for(int k = 0; k < epi[i].size(); k++) {
                if(epi[i][k] == 1) icount++;
                if(epi[j][k] == 1) jcount++;
                if(epi[i][k] == 1 && epi[j][k] == 0) in++;
                if(epi[i][k] == 0 && epi[j][k] == 1) out++;
            }
            if(icount == 0 || jcount == 0) continue;
            if(in >= 0 && out == 0) rowpair.push_back(make_pair(i, j));
            if(out > 0 && in == 0) rowpair.push_back(make_pair(j, i));
        }
    }
    if (rowpair.size() == 0) return;
    row.push_back(rowpair[0].first);
    rowdelete.push_back(rowpair[0].second);
    for(int i = 1; i < rowpair.size(); i++) {
        bool check = false;
        for(int j = 0; j < row.size(); j++) {
            if(rowpair[i].second == rowdelete[j]) check = true;
        }
        if(!check) {
            row.push_back(rowpair[i].first);
            rowdelete.push_back(rowpair[i].second);
        }
    }    
    deduplicateInt(row);
    for (int i = 0; i < row.size(); i++) {
        answer.push_back(exceptepi[row[i]]);
    }
}

void findCol(vector<vector<int>> epi, vector<string> exceptepi, vector<string>& answer) {
    vector<pair<int, int>> colpair;
    vector<int> col;
    vector<int> coldelete;
    int in, out, icount, jcount;
    answer.push_back("COL");
    if(epi.empty()) {return;}

    for(int i = 0; i < epi[0].size()-1; i++) {
        for(int j = i+1; j < epi[0].size(); j++) {
            in = out = icount = jcount = 0;
            for(int k = 0; k < epi.size(); k++) {
                if(epi[k][i] == 1) icount++;
                if(epi[k][j] == 1) jcount++;
                if(epi[k][i] == 1 && epi[k][j] == 0) in++;
                if(epi[k][i] == 0 && epi[k][j] == 1) out++;
            }
            if(icount == 0 || jcount == 0) continue;
            if(out >= 0 && in == 0) colpair.push_back(make_pair(i, j));
            if(in > 0 && out == 0) colpair.push_back(make_pair(j, i));
        }
    } 
    if (colpair.size() == 0) return;
    col.push_back(colpair[0].first);
    coldelete.push_back(colpair[0].second);
    for(int i = 1; i < colpair.size(); i++) {
        bool check = false;
        for(int j = 0; j < col.size(); j++) {
            if(colpair[i].second == coldelete[j]) check = true; 
        }
        if(!check) {
            col.push_back(colpair[i].first);
            coldelete.push_back(colpair[i].second);
        }
    }
    deduplicateInt(col);
    for (int i = 0; i < col.size(); i++) {
        answer.push_back(to_string(col[i]));
    }
}

int main() {
    int bitnum, mintermnum, min;
    vector<int> minterm;
    vector<string> answer;
    cin >> bitnum >> mintermnum;
    for (int i = 0; i < mintermnum; i++) {
        cin >> min;
        minterm.push_back(min);
    }
    
    vector<vector<string>> bit = init(bitnum, mintermnum, minterm); 
    findPI(bit, bitnum, answer);
    vector<int> v(mintermnum, 0);
    vector<vector<int>> epi(answer.size(), v);
    vector<string> exceptepi = answer;
    findEPI(bit, answer, epi, exceptepi);
    findRow(epi, exceptepi, answer);
    for(int i = 0; i < answer.size(); i++) {
        for(int j = 0; j < bitnum; j++) {
            if(answer[i][j] == '2') answer[i][j] = '-';
        }
    }
    findCol(epi, exceptepi, answer);
    // 출력
    for(int i = 0; i < answer.size(); i++) {
        cout << answer[i] << " ";
    } cout << endl;
}
