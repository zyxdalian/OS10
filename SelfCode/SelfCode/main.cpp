//
//  main.cpp
//  SelfCode
//
//  Created by Frederic on 26/11/2021.
//  Copyright © 2021 Frederic. All rights reserved.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <math.h>
#include <algorithm>

using namespace std;
typedef vector<vector<int>> mat;
typedef vector<vector<vector<int>>> cube;

#define STAGE 3
#define JOBS 10
#define MaxNumMac 4 // max numbre of machines in all stages
#define NUM_FACTORY 2

//mat JML (10, vector<int> (3, -2) ); // job machine list = jobs*stages = Number of machine
cube PT (STAGE, vector<vector<int>> (JOBS, vector<int> (MaxNumMac)) );// processing time of stage*jobs*machines

vector<int> LPT (void) {
    // calculate the mean PT
    vector<double> PT_mean (JOBS, -1);
    for (int j=0; j<JOBS; j++) {
        int pt_crr = 0; // current sum processing time of this job
        for (int s=0; s<STAGE; s++) {
            for (int m=0; m<MaxNumMac; m++) {
                if (PT[s][j][m] == INT_MAX)
                    break;
                pt_crr += PT[s][j][m];
            }
        }
        PT_mean[j] = pt_crr/STAGE;
    }
    // fin calculate the mean PT
    
    // sort and get sigma
    vector<int> sigma;
    for (int i=0; i<JOBS; i++)
        sigma.push_back(i);
    sort (sigma.begin(), sigma.end(),
         [&PT_mean] (double i1, double i2) {return PT_mean[i1] > PT_mean[i2];} );
    
    return sigma;
}

int cal_Cmax (vector<int> pai) {
    mat r(STAGE, vector<int> (MaxNumMac, 0)); // releasing time = stage*machine
    r[1][3] = INT_MAX; // no 4th machine in stage2
    r[2][3] = INT_MAX; // no 4th machine in stage3
    mat JML (JOBS, vector<int> (MaxNumMac, -2) ); // job machine list = jobs*stages = Number of machine
    //cube PT (3, vector<vector<int>> (10, vector<int> (4)) );// processing time of stage*jobs*machines
    int C_Max = -1; // max completing time
    
    // arrange 1er job
    for (int s=0; s<STAGE; s++) {
        // find machine of shortest processing time for 1er job in each stage
        int mCr = 0; // current machine
        int ptCr = PT[s][pai[0]][0]; // current releasing time
        for (int m=0; m<MaxNumMac; m++) { //m<PT[s][0].size()
            if (ptCr > PT[s][pai[0]][m]) {
                ptCr = PT[s][pai[0]][m];
                mCr = m;
//                cout << "ptCr = " << ptCr << " ";
            }
//            cout << endl;
        }
        JML[pai[0]] [s] = mCr; // assign 1er job to current machine
        if (s == 0)
            r[s][mCr] = ptCr; // calculate releasing time
        else if (s >= 1)
            r[s][mCr] = ptCr + r[s-1][ JML[pai[0]] [s-1] ];
    }
    // fin arrange 1er job
    
    int gapMax = 0;
    // iterate job from 2nd to n to find the max gap
    for (int j=1; j<pai.size(); j++) { // j = job
        // update JML
        for (int s=0; s<STAGE; s++) {
            // find machine of earlest releasing time for current job in each stage
            int mCr = 0; // current machine
            int rCr = r[s][0]; // current releasing time = earlest releasing time in this stage
            for (int m=0; m<PT[s][pai[j]].size(); m++) { //PT[stage] [job] [machine]
                if (rCr > r[s][m]) {
                    rCr = r[s][m];
                    mCr = m;
                }
            }
            JML[pai[j]] [s] = mCr; // assign No.j job to current machine
            // find Max gap among stages
            int gapCr = 0;
            if (s >= 1) {
                gapCr = r[s][ JML[pai[j]] [s] ] - r[s-1][ JML[pai[j]] [s-1] ];
                if (gapCr > gapMax) {
                    gapMax = gapCr;
                }
            }
            // update releasing time in this stage
            // attention: must update releasing time after calculated the gap
            if (s == 0)
                r[s][mCr] += PT[s][pai[j]][mCr];
            else if (s >= 1)
                r[s][mCr] = PT[s][pai[j]][mCr] + r[s-1][ JML[pai[j]] [s-1] ];
        }
        // fin find max gap
        // find Max distence
        int dis = max (0, gapMax);
        // correcting the releasing time
        for (int s=0; s<STAGE; s++) {
            // attention : moved the PT part of fonctions above.
            // because we need to update PT before calculating gap.
            if (s==0) {
                r[s][ JML[pai[j]] [s] ]  += dis;
                // fonction 19 in paper
            }
            else if (s >= 1) {
                r[s][ JML[pai[j]] [s] ]  = r[s-1][ JML[pai[j]] [s-1] ] + PT[s][pai[j]][JML[pai[j]][s]];
                // fonction 20 in paper
            }
//            cout << "gapMax = " << gapMax << endl;
//            cout << "dis = " << dis << endl;
        }
    }
    // fin arranging jobs from 2ed to fin
    
    // calculating max completing time
    for (int i=0; i<STAGE; i++) {
        for (int j=0; j<MaxNumMac; j++) {
//            cout << "r = " << r[i][j] << "; ";
            if (C_Max < r[i][j] && r[i][j] != INT_MAX) {
                C_Max = r[i][j];
            }
        }
//        cout << "C_Max = " << C_Max << " ";
//        cout << endl;
    }
    return C_Max;
}

mat DNEH (vector<int> sigma) {
    mat pais (NUM_FACTORY, vector<int> ());
    
    // assign a job for each factory
    for (int f=0; f<NUM_FACTORY; f++) {
        pais[f].push_back(sigma[f]);
    }
    
    // assign the rest jobs
    for (int j=NUM_FACTORY; j<sigma.size(); j++) {
        
        vector<int> test_pai;
        int best_CMax = INT_MAX; // best C max for this job in all factories
        int best_Fac = -1; // best C_max found in this factory
        int best_Pos = -1; // best C_max found in this position (in this factory) = numbre of switch
        
        // put this job in each factory
        for (int f=0; f<NUM_FACTORY; f++) {
            test_pai = pais[f];
            int CMax_crr = INT_MAX; // current C_max
            int c_s = 0; // counter of switch
            test_pai.push_back(sigma[j]);
            int n_test_pai = test_pai.size();
            // now test all positions to find the best C_Max in this factory
            CMax_crr = cal_Cmax(test_pai);
            if (CMax_crr < best_CMax) {
                best_CMax = CMax_crr;
                best_Fac = f;
                best_Pos = c_s;
            }
            while (test_pai[0] != sigma[j]) {
                int pos_crr = n_test_pai - c_s - 1;
                swap(test_pai[pos_crr], test_pai[pos_crr-1]);
                c_s ++;
                CMax_crr = cal_Cmax(test_pai);
                if (CMax_crr < best_CMax) {
                    best_CMax = CMax_crr;
                    best_Fac = f;
                    best_Pos = c_s;
                }
            }
            // fin while (test all positions in this factory)
            
        }
        // fin j for (put this job in all factories)
        
        pais[best_Fac].push_back(sigma[j]);
        int n = sizeof(pais[best_Fac]);
        for (int i=0; i<best_Pos; i++) {
            swap(pais[best_Fac][n-i], pais[best_Fac][n-i-1]);
        }
    }
    // fin f for (assign the rest jobs)
    
    return pais;
}

int FindMaxCMax (const mat& pais) {
    // find max CMax among all factories
    int res = cal_Cmax(pais[0]);
    for (int f=0; f<NUM_FACTORY; f++) {
        res = cal_Cmax(pais[f]) > res ? cal_Cmax(pais[f]) : res;
    }
    return res;
}
void ShowCMax (const mat& pais) {
    // show the results
    for (int f=0; f<NUM_FACTORY; f++)
        cout << "Fac " << f+1 << " C_max = " << cal_Cmax(pais[f]) << endl;
    // find max CMax among all factories
    int res = FindMaxCMax(pais);
    cout << "C_Max = " << res << endl;
}

void ShowDNEH (mat aMat) { // show job assignments among factories
    int f = 1;
    for ( auto p = begin(aMat) ; p != end(aMat); ++p) {
        cout << "Jobs in Fac " << f++ << ": ";
        for ( auto q = begin(*p); q != end(*p); ++q ) {
            cout << *q + 1 << ' ';
        }
        cout<<endl;
    }
}

void NewPais(mat& pais, int fac, int pos, int tar_fac){
    pais[tar_fac].push_back(pais[fac][pos]);
    int n = pais[fac].size();
    for(int i=pos; i<n-1; i++){
        pais[fac][i]=pais[fac][i+1];
    }
    pais[fac].pop_back();
}
int FindBetterPais (mat&best_pais, mat& new_pais, const int& CMax0, const int& tf) {
    int pos = new_pais[tf].size()-1;
    int better_CMax = CMax0;
    while (pos != 0) {
        if (FindMaxCMax(new_pais) < better_CMax) {
            best_pais = new_pais;
            better_CMax = FindMaxCMax(new_pais);
        }
        swap(new_pais[tf][pos], new_pais[tf][pos-1]);
        pos --;
    }
    if (FindMaxCMax(new_pais) < better_CMax) {
        best_pais = new_pais;
        better_CMax = FindMaxCMax(new_pais);
    }
    return better_CMax;
}
mat LS_Insert (const mat& pais, const int& CMax0) {
    mat best_pais = pais;
    int best_CMax = CMax0;
    // extract job from factory f
    int f = -1;
    for (int i=0; i<NUM_FACTORY; i++) {
        if (cal_Cmax(pais[i]) == CMax0)
            f = i;
    }
    for (int pos=0; pos<pais[f].size(); pos++ ) { // extract job in position 'pos' in fac 'f'
        for (int tf=0; tf<NUM_FACTORY; tf++) { // tf = target factory, insert job to this fac
            
            if (tf != f) {
                mat new_pais = pais;
                // extract job from position pos of fac f to fac tf
                // modifie the new_pais directely
                NewPais(new_pais, f, pos, tf);
                // test if we can find a better solution, if none, res = -1
                int res = FindBetterPais(best_pais, new_pais, best_CMax, tf);
            }
            
        }
    }
    return best_pais;
}

int main(int argc, const char * argv[]) {

    PT[0]={{73,70,76,76},
     {3,3,9,4},
     {78,79,89,83},
     {63,61,59,60},
     {60,65,59,60},
     {42,38,51,32},
     {17,17,22,13},
     {95,93,97,95},
     {9,14,5,6},
     {5,4,4,3}};

    PT[1]={{57,49,59,INT_MAX},
     {49,41,50,INT_MAX},
     {24,11,15,INT_MAX},
     {94,84,93,INT_MAX},
     {32,34,35,INT_MAX},
     {57,49,48,INT_MAX},
     {31,27,28,INT_MAX},
     {59,52,52,INT_MAX},
     {31,40,35,INT_MAX},
     {48,49,49,INT_MAX}};

    PT[2]={{4,10,4,INT_MAX},
     {22,12,18,INT_MAX},
     {93,95,99,INT_MAX},
     {58,53,46,INT_MAX},
     {75,77,89,INT_MAX},
     {75,75,76,INT_MAX},
     {58,72,70,INT_MAX},
     {81,87,85,INT_MAX},
     {98,71,89,INT_MAX},
     {97,93,96,INT_MAX}};
    
    vector<int> sigma = LPT();
    mat my_pais;
    
    cout << "using DNEH : " << endl;
    my_pais = DNEH(sigma);
    ShowCMax(my_pais);
    ShowDNEH(my_pais);
    cout << endl;
    
    cout << "using LS_Insert to improve the result : " << endl;
    mat new_pais = LS_Insert(my_pais, FindMaxCMax(my_pais));
    ShowCMax(new_pais);
    ShowDNEH(new_pais);
    
    return 0;
}
