// Online C++ compiler to run C++ program online
#include<bits/stdc++.h>
using namespace std;

// bool B_exist(string path){
//     for(int i=0;i<path.length();i++){
//         if(path[i] == 'B'){
//             return true;
//         }
//     }
//     return false;
// }

void reduce_path(string path){
    for(int i=0;i<path.length();){
        if(path[i] == 'B'){
            if(path[i-1] == 'L' and path[i+1] == 'L'){
                path[i-1] = 'S';
            }else if(path[i-1] == 'R' and path[i+1] == 'R'){
                path[i-1] = 'S';
            }else if(path[i-1] == 'S' and path[i+1] == 'S'){
                path[i-1] = 'B';
            }else if(path[i-1] == 'L' and path[i+1] == 'R'){
                path[i-1] = 'B';
            }else if(path[i-1] == 'R' and path[i+1] == 'L'){
                path[i-1] = 'B';
            }else if(path[i-1] == 'L' and path[i+1] == 'S'){
                path[i-1] = 'R';
            }else if(path[i-1] == 'S' and path[i+1] == 'L'){
                path[i-1] = 'R';
            }else if(path[i-1] == 'R' and path[i+1] == 'S'){
                path[i-1] = 'L';
            }else if(path[i-1] == 'S' and path[i+1] == 'R'){
                path[i-1] = 'L';
            }
            path.erase(i, 2);
            i--;
        }else{
            i++;
        }
    }
    cout<<path;
    
}

int main() {
    // Write C++ code here
    string path = "LBLRLLBRSSBLLL";
    reduce_path(path);
    return 0;
}