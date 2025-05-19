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


vector<float> GetAJ(const char* inFileName){

    float jtpt1 = -1;
    float jtpt2 = -1;
    float A_J = 0;

    Float_t jtpt[100];
    int nref;
    
    vector<float> jtpt1vector;
    vector<float> jtpt2vector;
    vector<float> ajvector;

    TFile* inFile = new TFile(inFileName,"READ");
    if (!inFile || inFile->IsZombie()) {
        cout << "Error: Could not open the file!" << endl;
    }
    TTree* JetAnalyserTree = (TTree*)inFile->Get("akCs4FlowPFJetAnalyzer/t");

    JetAnalyserTree->SetBranchAddress("nref",&nref);
    JetAnalyserTree->SetBranchAddress("jtpt",jtpt);

    for (int i = 0; i < JetAnalyserTree->GetEntries(); i++){
        JetAnalyserTree->GetEntry(i);
	    if (nref >= 2){
		    jtpt1 = jtpt[0];
		    jtpt2 = jtpt[1];
		    A_J = (jtpt1-jtpt2)/(jtpt1+jtpt2);
		    ajvector.push_back(A_J);
	    }
    }
    return ajvector;
}

vector<float> GetEmissJets(const char* inFileName, float etacut = 2.0){

    TFile* inFile = new TFile(inFileName,"READ");
    if (!inFile || inFile->IsZombie()) {
        cout << "Error: Could not open the file!" << endl;
    }
    TTree* tree = (TTree*)inFile->Get("akCs4FlowPFJetAnalyzer/t");

    float EMissjpt = 0;
    float jptxtot, jptytot;

    int nref;
    Float_t jtpt[100];
    Float_t jtphi[100];
    Float_t jteta[100];


    vector<float> EMiss;

    tree->SetBranchAddress("nref",&nref);
    tree->SetBranchAddress("jtpt",jtpt);
    tree->SetBranchAddress("jtphi",jtphi);
    tree->SetBranchAddress("jteta",jteta);

    for (int i = 0; i < tree->GetEntries(); i++){
	    EMissjpt = 0;
	    jptxtot = 0;
	    jptytot = 0;
        tree->GetEntry(i);
        for (int j = 0; j < nref; j++){
            if (abs(jteta[j]) > etacut) continue;
            if (!jtpt[j]){
		        cerr << "Error in entry number: "<< i <<j << endl;
                break;
            }
	        else{
		        jptxtot += TMath::Cos(jtphi[j])*jtpt[j];
		        jptytot += TMath::Sin(jtphi[j])*jtpt[j];
	        }
	    }
	    EMissjpt = TMath::Sqrt(jptxtot*jptxtot + jptytot*jptytot);
	    EMiss.push_back(EMissjpt);
    }
    return EMiss;
}



vector<float> GetEmissPF(const char* inFileName, float etacut = 2.0){

    TFile* inFile = new TFile(inFileName,"READ");
    if (!inFile || inFile->IsZombie()) {
        cout << "Error: Could not open the file!" << endl;
    }
    TTree* PFTree = (TTree*)inFile->Get("particleFlowAnalyser/pftree");

    float EMisspt = 0;
    float ptxtot, ptytot;

    int n;
    vector<float>* pfpt = nullptr;
    vector<float>* pfphi = nullptr;
    vector<float>* pfeta = nullptr;
    vector<float>* pfE = nullptr;
    vector<float>* pfId = nullptr;

    vector<float> EMiss;

    PFTree->SetBranchAddress("nPF",&n);
    PFTree->SetBranchAddress("pfPt",&pfpt);
    PFTree->SetBranchAddress("pfPhi",&pfphi);
    PFTree->SetBranchAddress("pfEta",&pfeta);

    for (int i = 0; i < PFTree->GetEntries(); i++){
	    EMisspt = 0;
	    ptxtot = 0;
	    ptytot = 0;
        PFTree->GetEntry(i);
        for (int j = 0; j < n; j++){
            if (abs(pfeta->at(j)) > etacut) continue;
	        else{
                ptxtot += TMath::Cos(pfphi->at(j))*pfpt->at(j);
                ptytot += TMath::Sin(pfphi->at(j))*pfpt->at(j);
	        }
	    }
	    EMisspt = TMath::Sqrt(ptxtot*ptxtot + ptytot*ptytot);
	    EMiss.push_back(EMisspt);
    }
    return EMiss;
}
