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
    vector<float> Emiss5 = GetEmissJets(inputFileName,5.0);
    vector<float> EmissPF2 = GetEmissPF(inputFileName,2.0);
    vector<float> EmissPF2p5 = GetEmissPF(inputFileName,2.5);
    vector<float> EmissPFInclusive = GetEmissPF(inputFileName,5);


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
    CutsBranches HiBinCut_low{"HiTree", "hiBin", 0, 20}; 
    CutsBranches HiBinCut_mid{"HiTree", "hiBin", 20, 60};        
    CutsBranches HiBinCut_high{"HiTree", "hiBin", 60, 160};               
       
    CutsVector AJcut{"A_J",AJValues, 0.8,1.0};  
    CutsVector EMisscut{"EMiss",Emiss5, 350,100000};  
    CutsVector EMisscut2{"EMiss2",Emiss2, 350,100000};  
    CutsVector EMisscut2p5{"EMiss",Emiss2p5, 350,100000};  

    cout << "Starting EMiss Inclusive Event Array" << endl;
    
    CutsBranches HiBranches[] = {HiBinCut_low,HiBinCut_mid,HiBinCut_high};

    vector<EventInfo> eventInfo = ReturnAsymmetricEvents(inputFileName,
        jtEtaCut,
        HiBinCut_low,
        dPhiCut_high,
        AJcut,
        EMisscut,
        EMisscut2,
        EMisscut2p5);

    vector<string> lines = HistogramSubtitle(jtEtaCut,
        HiBinCut_low,
        dPhiCut_high,
        AJcut,
        EMisscut2,
        EMisscut2p5);



    for (int EventNum = 0; EventNum < eventInfo.size(); EventNum++) {

        TCanvas* c1 = new TCanvas("c1", "2x3 Grid", 1600, 1200);
        c1->Divide(3, 2); // 3 columns, 2 rows
        c1->SetTheta(45.);
		c1->SetPhi(100.);
        
        TPad* overlay = new TPad("overlay", "overlay", 0, 0, 1, 1);
        overlay->SetFillStyle(4000); // Transparent fill
        overlay->SetFrameFillStyle(0);
        overlay->Draw();
        overlay->cd(); // Switch to the overlay pad

        TLatex latex;
        latex.SetNDC(); // Normalize coordinates to pad (0-1)
        latex.SetTextFont(42);
        latex.SetTextSize(0.016);
        for (size_t j = 0; j < lines.size(); ++j) {
            latex.DrawLatex(0.05, 0.92 - j * 0.014, lines[j].c_str());
        }

        vector<string> infoTexts = EventInfoPrint(eventInfo[0]);

        for (size_t j = 0; j < infoTexts.size(); ++j) {
            latex.DrawLatex(0.17, 0.92 - j * 0.015, infoTexts[j].c_str());
        }

        //for (int eventNum = 0, eventNum < eventInfo.size(); ++eventNum) {
        for (int i = 0; i < 6; i++) {
            cout << "Processing Plot: " << i << endl;
            c1->cd(i+1);
            TH2D* histogramTest = draw2DNew(inputFileName,eventInfo[EventNum].event,i);
                histogramTest->Draw("LEGO2 Z");
            }
            
        c1->SaveAs(Form("Plots/2DDiagrams_Grid/HiForestPartialskim_Event%d_pf_DiffPFEnergy.png",eventInfo[EventNum].event));
    }
    //   }
           
    /*
    for (int i = 0; i < 3; i++){
        //Plot3Hist(inputFileName,EmissPF2,EmissPF2p5,EmissPFInclusive, HiBranches[i]);
    }
    cout << "End" << endl;
    drawStackedPF(inputFileName);*/

    
    // cout << "Starting EMiss Inclusive Event Array" << endl;
    // vector<EventInfo> eve ntInfo = ReturnAsymmetricEvents(inputFileName,
    //     jtEtaCut,
    //     HiBinCut,
    //     dPhiCut_high,
    //     AJcut,
    //     EMisscut,
    //     EMisscut2,
    //     EMisscut2p5);

    // cout << "End" << endl;
    // cout << endl;                            


    // draw2D(inputFileName,jtEtaCut,HiBinCut,dPhiCut_high,AJcut,EMisscut2,EMisscut2p5,eventInfo);

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

