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

using namespace std;
typedef vector<vector<int>> mat;
typedef vector<vector<vector<int>>> cube;

//int ReadFile (ifstream f) {
//    if (f.fail())
//        printf("open failed \n");
//    mat data = mat (n, vector<int> (n, 0));
//    while (!f.eof()) {
//        for (int i=0; i<n; i++) {
//            for (int j=0; j<n; j++) {
//                f >> data[i][j];
//            }
//        }
//    }
//    for (int i=0; i<n; i++) {
//        for (int j=0; j<n; j++) {
//            cout << data[i][j] << " ";
//        }
//        cout << endl;
//    }
//    return 1;
//};

#define STAGE 3
#define JOBS 10
#define MaxNumMac 4 // max numbre of machines in all stages
#define NUM_FACTORY 2

mat r(3, vector<int> (4, 0)); // releasing time of each machine = stage * machines
vector<int> pai (10, 0); // scheduling jobs
mat JML (10, vector<int> (3, -2) ); // job machine list = jobs*stages = Number of machine
cube PT (3, vector<vector<int>> (10, vector<int> (4)) );// processing time of stage*jobs*machines

void PrintMat (mat aMat, int l, int c) {
    for (int i=0; i<l; i++) {
        for (int j=0; j<c; j++)
            cout << (aMat[i][j] + 1) << " ";
        cout << endl;
    }
}

int cal_Cmax (vector<int> pai) {
    
    int C_Max = -1; // max completing time
    
    // arrange 1er job
    for (int s=0; s<STAGE; s++) {
        // find machine of shortest processing time for 1er job in each stage
        int mCr = 0; // current machine
        int ptCr = PT[s][0][0]; // current releasing time
        for (int m=0; m<MaxNumMac; m++) { //m<PT[s][0].size()
            if (ptCr > PT[s][0][m]) {
                ptCr = PT[s][0][m];
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
            cout << "gapMax = " << gapMax << endl;
            cout << "dis = " << dis << endl;
        }
        // JML test success
    }
    // fin arranging jobs from 2ed to fin
    
    // calculating max completing time
    for (int i=0; i<STAGE; i++) {
        for (int j=0; j<MaxNumMac; j++) {
            cout << "r = " << r[i][j] << "; ";
            if (C_Max < r[i][j] && r[i][j] != INT_MAX) {
                C_Max = r[i][j];
            }
        }
        cout << "C_Max = " << C_Max << " ";
        cout << endl;
    }
    PrintMat(JML, JOBS, STAGE);
    return C_Max;
}

mat DNEH (vector<int> sigma) {
    mat pais (NUM_FACTORY, vector<int> ());
    
    for (int f=0; f<NUM_FACTORY; f++) {
        pais[f].push_back(sigma[f]);
    }
    
    for (int j=NUM_FACTORY; j<sigma.size(); j++) {
        
        for (int f=0; f<NUM_FACTORY; f++) {
            
        }
    }
    
    return pais;
}

int main(int argc, const char * argv[]) {
//    ifstream f1;
//    f1.open
//    ("/Users/frederic/Documents/UTT/5_21A/OS10/prj/codes/SelfCode/SelfCode/DHFSP_10_5_3_1_job_pt.txt");
//    ReadFile(f1);
//    vector<int> a;
//
////    data >> a;
    // in stage 0, processing time of [job] in [machine]
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
    
    r[1][3] = INT_MAX; // no 3rd machine in stage2
    r[2][3] = INT_MAX; // no 3rd machine in stage3
    
    pai = {0,1,2,3,4,5};
    int test = cal_Cmax(pai);
    cout << pai.size() << "jobs arranged, C_max = " << test << endl;
    
    return 0;
}
