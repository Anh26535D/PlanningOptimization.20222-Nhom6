#include <bits/stdc++.h>
#define NMAX 1000
#define KMAX 1000

using namespace std;

int n_packs, n_bins;
int packs[NMAX][2]; //packs[i][0] is w[i], packs[i][1] is l[i]
int bins[KMAX][3]; //bins[k][0] is W[k], bins[k][1] is L[k], bins[k][2] is c[k]
int priorBins[KMAX];
int t[NMAX], x[NMAX], y[NMAX], o[NMAX];
int t_solution[NMAX], x_solution[NMAX], y_solution[NMAX], o_solution[NMAX];
int f=0, f_best=INT_MAX;

bool compare(int &a, int &b) {
    int value_a = bins[a][0]*bins[a][1];
    int value_b = bins[b][0]*bins[b][1];
    return value_a > value_b;
}

void sortBins(){
    for(int k=0; k<n_bins; ++k){
        priorBins[k] = k;
    }
    sort(priorBins, priorBins + n_bins, compare);
}

void init(){
    for(int i=0; i<n_packs; ++i){
        t[i] = -1; t_solution[i] = -1;
        x[i] = -1; x_solution[i] = -1;
        y[i] = -1; y_solution[i] = -1;
        o[i] = -1; o_solution[i] = -1;
    }

    sortBins();
}

void printOptSolution(){
    for(int i=0; i<n_packs; ++i){
        cout<<i+1<<" "<<t_solution[i]+1<<" "<<x_solution[i]<<" "<<y_solution[i]<<" "<<o_solution[i]<<"\n";
    }
}

void printSolution(){
    for(int i=0; i<n_packs; ++i){
        cout<<i+1<<" "<<t[i]+1<<" "<<x[i]<<" "<<y[i]<<" "<<o[i]<<"\n";
    }    
}

int isOverlapping(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) {
    int left = max(x1, x2);
    int bottom = max(y1, y2);
    int right = min(x1 + w1, x2 + w2);
    int top = min(y1 + h1, y2 + h2);
    int width = max(0, right - left);
    int height = max(0, top - bottom);
    return width*height > 0;
}

bool check(int n, int k, int xn, int yn, int on){
    int w=packs[n][0], l=packs[n][1];
    int W=bins[k][0], L=bins[k][1];
    if(on==1){
        swap(w,l);
    }
    if((0<=xn) && (xn+w <= W) && (0<=yn) && (yn+l <= L)){
        for(int i=0; i<n; ++i){
            if(t[i] == k){
                int wi = packs[i][0], li = packs[i][1];
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
    for(int i=0; i<n_bins; ++i){
        if(inTruck[i] > 0){
            f += bins[i][2];
        }
    }   
    return f;
}

void Try(int n){
    for(int tr=0; tr<n_bins; ++tr){
        int k = priorBins[tr]; 
        int m = min(packs[n][0],packs[n][1]);
        for(int xn=0; xn<bins[k][0] - m; ++xn){
            for(int yn=0; yn<bins[k][1] - m; ++yn){
                for(int on=0; on<2; ++on){
                    if(check(n, k, xn, yn, on)){
                        update(n, k, xn, yn, on);
                        if(n == n_packs-1){
                            int f = calCurrentCost(n);
                            if(f < f_best){
                                f_best = f;
                                for(int i=0; i<n_packs; ++i){
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
    freopen("data.in", "r", stdin);
    freopen("data.out", "w", stdout);
    cin>>n_packs>>n_bins;
    for(int i=0; i<n_packs; ++i){
        cin>>packs[i][0]>>packs[i][1];
    }
    for(int k=0; k<n_bins; ++k){
        cin>>bins[k][0]>>bins[k][1]>>bins[k][2];
    }
    init();
    Try(0);
    printOptSolution();
    return 0;
}