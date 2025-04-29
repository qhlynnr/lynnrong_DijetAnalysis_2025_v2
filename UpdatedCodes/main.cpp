#include <iostream>
#include <TFile.h>
#include <TH1F.h>
#include <TH2D.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TLegend.h>

void run(const char* inputFileName) {
    string JetAnalyserTreeString = "akCs4FlowPFJetAnalyzer/t";
    string PFTreeString = "particleFlowAnalyser/pftree";
    string HiTreeString = "hiEvtAnalyzer/HiTree";

    TTree* JetAnalyserTree = ReturnTree(inputFileName,JetAnalyserTreeString);
    TTree* PFTree = ReturnTree(inputFileName, PFTreeString);
    TTree* HiTree = ReturnTree(inputFileName, HiTreeString);

    vector<float> Dphi = GetDPhi(inputFileName,JetAnalyserTreeString);
    vector<float> AJValues = GetAJ(inputFileName);
    vector<float> Emiss2 = GetEmissJets(inputFileName,2.0);
    vector<float> Emiss2p5 = GetEmissJets(inputFileName,2.5);
    vector<float> Emiss3 = GetEmissJets(inputFileName,3.0);
    vector<float> Emiss5 = GetEmissJets(inputFileName,5.0);
    

    HistVar1D h = {
        "CanvasName",
        "jtPfCHF Distribution of high vs low pT Events, sqrt(sNN) = 5360 GeV",
        "Plots/JetPFProperties/Plot_Skim_jtPfCHF.png",
        "jtPfCHF",
        "Events Normalized to Unity",
        50,0,1,
        0,1,false
    };


    //CutsVector dPhiCut_low{"dPhi",Dphi, 0, 0.8};
    CutsVector dPhiCut_high{"dPhi",Dphi, TMath::Pi()*(7.0/8.0), TMath::Pi()};
    CutsBranches jtEtaCut{"JetAnalyzer", "jteta", -2, 2};        
    CutsBranches pfEtaCut{"pftree", "pfEta", -2, 2};     
    CutsBranches HiBinCut{"HiTree", "hiBin", 0, 20};        
    CutsVector AJcut{"A_J",AJValues, 0.8,1.0};  
    CutsVector EMisscut{"EMiss",Emiss5, 350,100000};  
    CutsVector EMisscut2{"EMiss2",Emiss2, 350,100000};  
    CutsVector EMisscut2p5{"EMiss",Emiss2p5, 350,100000};  


    cout << "Starting EMiss Inclusive Event Array" << endl;
    vector<EventInfo> eventInfo = ReturnAsymmetricEvents(inputFileName,
        jtEtaCut,
        HiBinCut,
        dPhiCut_high,
        AJcut,
        EMisscut,
        EMisscut2,
        EMisscut2p5);

    cout << "End" << endl;
    cout << endl;                            


    draw2D(inputFileName,jtEtaCut,HiBinCut,dPhiCut_high,AJcut,EMisscut2,EMisscut2p5,eventInfo);

    //string branchnamearr[] = {"jtPfCHF","jtPfNEF","jtPfNHF","jtPfCEF","jtPfMUF"};
    
	// for (int j = 0;j < 5 ;j++){
    //     string branchname = branchnamearr[j];
    //     string title = "";
    //     string filename = "";

    //     title += branchname;
    //     title += " of high vs low dPhi Events, sqrt(sNN) = 5360 GeV";

    //     filename += "Plots/JetPFProperties/HiForestSkimPlots_";
    //     filename += branchname;
    //     filename += "_HighLowdPhiComp.png";		    

    //     cout << "BranchName: " << branchname << endl;
    //     cout << "Title: " << title << endl;
    //     cout << filename << endl;
    //     cout << endl;

    //     h.histTitle = title.c_str();
    //     h.outFileName = filename.c_str();
    //     h.xLabel = branchname.c_str();

    //     PlotTwoBranchesWithCuts(inputFileName, branchname.c_str(), h, dPhiCut_low, dPhiCut_high, jtEtaCut);
    // }

}

