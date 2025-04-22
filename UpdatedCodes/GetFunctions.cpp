#include <iostream>
#include <TFile.h>
#include <TH1F.h>
#include <TH2D.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TMath.h>

#include "CutsVariables.h"
#include "HistVariables.h"

using namespace std;

TTree* ReturnTree(const char* inputFileName, string treeName) {
    TFile* inFile = new TFile(inputFileName, "READ");
    if (!inFile || inFile->IsZombie()) {
        cout << "Error: Could not open the file!" << endl;
    }

    TTree* tree = (TTree*)inFile->Get(treeName.c_str());
    return tree;
}

vector<float> GetDPhi(const char* inFileName,
    string inTreeName,
    const char* branchName = "jtphi"){
        float phi1, phi2, dphi;
        float pi = TMath::Pi();
        Float_t jtphi[100];
        vector<float> jtdphi = {};
        
        TFile* inFile = new TFile(inFileName,"READ");
        if (!inFile || inFile->IsZombie()) {
            cout << "Error: Could not open the file!" << endl;
        }

        TTree* Tree = (TTree*)inFile->Get(inTreeName.c_str());
        Tree->SetBranchAddress(branchName,jtphi);
        for (int i = 0; i < Tree->GetEntries(); i++){
            Tree->GetEntry(i);
            phi1 = jtphi[0];
            phi2 = jtphi[1];
            dphi = phi1 - phi2;
            if (pi <= dphi && dphi <= 2 * pi){
                dphi = (dphi - 2*pi);
                //cout << "New dphi:" << dphi << endl;
            } else if (-2*pi <= dphi && dphi <= -pi){
                dphi = dphi + 2*pi;
            }
            dphi = TMath::Abs(dphi);
            //cout << dphi << endl;
            jtdphi.push_back(dphi);
        }
        return jtdphi;
    }

