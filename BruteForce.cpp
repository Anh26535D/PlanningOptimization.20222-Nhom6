#include <bits/stdc++.h>
#define NMAX 1000
#define KMAX 1000

using namespace std;

int n_items, n_trucks;
int items[NMAX][2]; //items[i][0] is w[i], items[i][1] is l[i]
int trucks[KMAX][3]; //trucks[k][0] is W[k], trucks[k][1] is L[k], trucks[k][2] is c[k]
int priorTrucks[KMAX];
int t[NMAX], x[NMAX], y[NMAX], o[NMAX];
int t_solution[NMAX], x_solution[NMAX], y_solution[NMAX], o_solution[NMAX];
int f=0, f_best=INT_MAX;

bool compare(int &a, int &b) {
    int value_a = trucks[a][0]*trucks[a][1];
    int value_b = trucks[b][0]*trucks[b][1];
    return value_a > value_b;
}

void sortTrucks(){
    for(int k=0; k<n_trucks; ++k){
        priorTrucks[k] = k;
    }
    sort(priorTrucks, priorTrucks + n_trucks, compare);
}

void init(){
    for(int i=0; i<n_items; ++i){
        t[i] = -1; t_solution[i] = -1;
        x[i] = -1; x_solution[i] = -1;
        y[i] = -1; y_solution[i] = -1;
        o[i] = -1; o_solution[i] = -1;
    }

    sortTrucks();
}

void printOptSolution(){
    for(int i=0; i<n_items; ++i){
        cout<<i+1<<" "<<t_solution[i]+1<<" "<<x_solution[i]<<" "<<y_solution[i]<<" "<<o_solution[i]<<"\n";
    }
}

void printSolution(){
    for(int i=0; i<n_items; ++i){
        cout<<i+1<<" "<<t[i]+1<<" "<<x[i]<<" "<<y[i]<<" "<<o[i]<<"\n";
    }    
}

int isOverlapping(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) {
    if(x1 + w1 <= x2) return false;
    if(x2 + w2 <= x1) return false;
    if(y1 + h1 <= y2) return false;
    if(y2 + h2 <= y1) return false;
    return true;
}

bool check(int n, int k, int xn, int yn, int on){
    int w=items[n][0], l=items[n][1];
    int W=trucks[k][0], L=trucks[k][1];
    if(on==1){
        swap(w,l);
    }
    if((0<=xn) && (xn+w <= W) && (0<=yn) && (yn+l <= L)){
        for(int i=0; i<n; ++i){
            if(t[i] == k){
                int wi = items[i][0], li = items[i][1];
                int xi = x[i], yi = y[i];
                if(o[i] == 1){
                    swap(wi, li);
                }
                if(isOverlapping(xi, yi, wi, li, xn, yn, w, l)){
                    return false;
                }
            }
        }
        return true;
    }
    return false;
}

void update(int n, int k, int xn, int yn, int on){
    t[n] = k;
    x[n] = xn;
    y[n] = yn;
    o[n] = on;
}

void restore(int n, int k, int xn, int yn, int on){
    t[n] = -1;
    x[n] = 0;
    y[n] = 0;
    o[n] = 0;
}

int calCurrentCost(int n){
    int f=0;
    int inTruck[KMAX]={0};
    for(int i=0; i<n; ++i){
        inTruck[t[i]] += 1;
    }
    for(int i=0; i<n_trucks; ++i){
        if(inTruck[i] > 0){
            f += trucks[i][2];
        }
    }   
    return f;
}

void Try(int n){
    for(int tr=0; tr<n_trucks; ++tr){
        int k = priorTrucks[tr]; 
        int m = min(items[n][0],items[n][1]);
        for(int xn=0; xn<trucks[k][0] - m; xn+=10){
            for(int yn=0; yn<trucks[k][1] - m; yn+=10){
                for(int on=0; on<2; ++on){
                    if(check(n, k, xn, yn, on)){
                        update(n, k, xn, yn, on);
                        if(n == n_items-1){
                            int f = calCurrentCost(n);
                            if(f < f_best){
                                f_best = f;
                                for(int i=0; i<n_items; ++i){
                                    t_solution[i] = t[i];
                                    x_solution[i] = x[i];
                                    y_solution[i] = y[i];
                                    o_solution[i] = o[i];
                                }
                            }
                        }else{
                            int f = calCurrentCost(n);
                            if(f<f_best){
                                Try(n+1);
                            }
                        }
                        restore(n, k, xn, yn, on);
                    }               
                }
            }
        }
    }
}

int main(){
    freopen("INPUT.txt", "r", stdin);
    // freopen("OUTPUT.txt", "w", stdout);
    cin>>n_items>>n_trucks;
    for(int i=0; i<n_items; ++i){
        cin>>items[i][0]>>items[i][1];
    }
    for(int k=0; k<n_trucks; ++k){
        cin>>trucks[k][0]>>trucks[k][1]>>trucks[k][2];
    }
    init();
    Try(0);
    printOptSolution();
    return 0;
}