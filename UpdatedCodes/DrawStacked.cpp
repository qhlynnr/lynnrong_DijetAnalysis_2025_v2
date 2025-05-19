
#include <TFile.h>
#include <TH1F.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TH2D.h>
#include "EventInfo.h"

void drawStacked(const char* inFileName){
    TFile* inFile = new TFile(inFileName,"READ");
    if (!inFile || inFile->IsZombie()) {
        cout << "Error: Could not open the file!" << endl;
    }
    TTree* JetAnalyserTree = (TTree*)inFile->Get("akCs4FlowPFJetAnalyzer/t");
    TTree* HiTree = (TTree*)inFile->Get("hiEvtAnalyzer/HiTree");
    TTree* PFTree = (TTree*)inFile->Get("particleFlowAnalyser/pftree");

    int nref, hiBin;
    Float_t jtPfCHF[100];
    Float_t jtPfNHF[100];
    Float_t jtPfNEF[100];
    Float_t jtPfCEF[100];
    Float_t jtPfMUF[100];
    Float_t jteta[100];

    vector<double> eta_bins = {-5.191,-4.889,-4.716,-4.538,
		-4.363,-4.191,-4.013,-3.839,-3.664,-3.489,-3.314,-3.139,
		-3,-2.853,-2.65,-2.5,-2.322,-2.172,-2.043,-1.93,-1.83,-1.74,
		-1.653,-1.566,-1.479,-1.392,-1.305,-1.218,-1.131,-1.044,-0.957,
		-0.87,-0.783,-0.696,-0.609,-0.522,-0.435,-0.348,-0.261,-0.174,
		-0.087,0,0.087,0.174,0.261,0.348,0.435,0.522,0.609,0.696,0.783,0.87,
		0.957,1.044,1.131,1.218,1.305,1.392,1.479,1.566,1.653,1.74,1.83,1.93,
		2.043,2.172,2.322,2.5,2.65,2.853,3,3.139,3.314,3.489,3.664,3.839,4.013,
		4.191,4.363,4.538,4.716,4.889,5.191};

    JetAnalyserTree->SetBranchAddress("jtPfCHF", jtPfCHF);
    JetAnalyserTree->SetBranchAddress("jtPfNHF", jtPfNHF);
    JetAnalyserTree->SetBranchAddress("jtPfNEF", jtPfNEF);
    JetAnalyserTree->SetBranchAddress("jtPfCEF", jtPfCEF);
    JetAnalyserTree->SetBranchAddress("jtPfMUF", jtPfMUF);
    JetAnalyserTree->SetBranchAddress("nref", &nref);
    JetAnalyserTree->SetBranchAddress("jteta", jteta);

    auto histStack = new THStack("hs", "PFflow energy fraction Stacked;#eta;Energy Fraction");

    auto histCHF = new TH1F("histCHF", "CHF Histogram", eta_bins.size()-1, eta_bins.data());
    auto histNHF = new TH1F("histNHF", "NHF Histogram", eta_bins.size()-1, eta_bins.data());
    auto histNEF = new TH1F("histNEF", "NEF Histogram", eta_bins.size()-1, eta_bins.data());
    auto histCEF = new TH1F("histCEF", "CEF Histogram", eta_bins.size()-1, eta_bins.data());
    auto histMUF = new TH1F("histMUF", "MUF Histogram", eta_bins.size()-1, eta_bins.data());

    Long64_t nEntries = JetAnalyserTree->GetEntries();
    for (Long64_t i = 0; i < nEntries; ++i) {
        JetAnalyserTree->GetEntry(i);
        for (int j = 0; j < nref; ++j) {
            histCHF->Fill(jteta[j],jtPfCHF[j]);
            histNHF->Fill(jteta[j],jtPfNHF[j]);
            histNEF->Fill(jteta[j],jtPfNEF[j]);
            histCEF->Fill(jteta[j],jtPfCEF[j]);
            histMUF->Fill(jteta[j],jtPfMUF[j]);
        }
    }

    histCHF->SetFillColor(kRed - 4);
    histCEF->SetFillColor(kOrange - 3);
    histNEF->SetFillColor(kYellow - 7);
    histNHF->SetFillColor(kGreen + 2);
    histMUF->SetFillColor(kBlue - 6);


    histStack->Add(histCHF);
    histStack->Add(histCEF);
    histStack->Add(histNEF);
    histStack->Add(histNHF);
    histStack->Add(histMUF);

    //histStack->GetXaxis()->SetTitle("Eta");
    //histStack->GetYaxis()->SetTitle("Energy Fraction");

    TCanvas* canvas = new TCanvas("canvas", "PFflow energy fraction", 800, 600);
    histStack->Draw("HIST");
    canvas->BuildLegend();
    canvas->SaveAs("Plots/EnergyFractionStacked.png"); 
}

void drawStackedPF(const char* inFileName){
    TFile* inFile = new TFile(inFileName,"READ");
    if (!inFile || inFile->IsZombie()) {
        cout << "Error: Could not open the file!" << endl;
    }
    TTree* JetAnalyserTree = (TTree*)inFile->Get("akCs4FlowPFJetAnalyzer/t");
    TTree* HiTree = (TTree*)inFile->Get("hiEvtAnalyzer/HiTree");
    TTree* PFTree = (TTree*)inFile->Get("particleFlowAnalyser/pftree");

    int nref, hiBin, n;

    vector<double> eta_bins = {-5.191,-4.889,-4.716,-4.538,
        -4.363,-4.191,-4.013,-3.839,-3.664,-3.489,-3.314,-3.139,
        -3,-2.853,-2.65,-2.5,-2.322,-2.172,-2.043,-1.93,-1.83,-1.74,
        -1.653,-1.566,-1.479,-1.392,-1.305,-1.218,-1.131,-1.044,-0.957,
        -0.87,-0.783,-0.696,-0.609,-0.522,-0.435,-0.348,-0.261,-0.174,
        -0.087,0,0.087,0.174,0.261,0.348,0.435,0.522,0.609,0.696,0.783,0.87,
        0.957,1.044,1.131,1.218,1.305,1.392,1.479,1.566,1.653,1.74,1.83,1.93,
        2.043,2.172,2.322,2.5,2.65,2.853,3,3.139,3.314,3.489,3.664,3.839,4.013,
        4.191,4.363,4.538,4.716,4.889,5.191};

    vector<float>* pfpt = nullptr;
    vector<float>* pfE = nullptr;
    vector<float>* pfEta = nullptr;
    vector<float>* pfPhi = nullptr;
    vector<int>* pfId = nullptr;

    PFTree->SetBranchAddress("pfPt",&pfpt);
    PFTree->SetBranchAddress("pfE",&pfE);
    PFTree->SetBranchAddress("pfPhi",&pfPhi);
    PFTree->SetBranchAddress("pfEta",&pfEta);
    PFTree->SetBranchAddress("pfId",&pfId);
    PFTree->SetBranchAddress("nPF",&n);

    auto histStack = new THStack("hs", "PFflow Energy Stacked;#eta;Energy");
    auto histCH = new TH1F("histCH", "CH;#eta;Energy", eta_bins.size()-1, eta_bins.data());
    auto histNH = new TH1F("histNH", "NH;#eta;Energy", eta_bins.size()-1, eta_bins.data());
    auto histCE = new TH1F("histCE", "CE;#eta;Energy", eta_bins.size()-1, eta_bins.data());
    auto histMU = new TH1F("histMU", "MU;#eta;Energy", eta_bins.size()-1, eta_bins.data());
    auto histNE = new TH1F("histNE", "NE;#eta;Energy", eta_bins.size()-1, eta_bins.data());

    int numCH = 0;
    int numNH = 0;
    int numCE = 0;
    int numMU = 0;
    int numNE = 0;
    Long64_t nEntries = JetAnalyserTree->GetEntries();
    for (Long64_t i = 0; i < nEntries; ++i) {
        PFTree->GetEntry(i);
        if (i%1000 == 0) {
            cout << "Processing event: " << i << endl;
        }
        for (int j = 0; j < n; ++j) {
            if (pfId->at(j) == 1) {
                numCH++;
                histCH->Fill(pfEta->at(j),pfE->at(j));
            }
            else if (pfId->at(j) == 2) {
                numCE++;
                histCE->Fill(pfEta->at(j),pfE->at(j));
            }
            else if (pfId->at(j) == 3) {
                numMU++;
                histMU->Fill(pfEta->at(j),pfE->at(j));
            }
            else if (pfId->at(j) == 4) {
                numNE++;
                histNE->Fill(pfEta->at(j),pfE->at(j));
            }
            else if (pfId->at(j) == 5) {
                numNH++;
                histNH->Fill(pfEta->at(j),pfE->at(j));
            }
        }
    }


    histStack->Add(histCH);
    histStack->Add(histNE);
    histStack->Add(histNH);
    histStack->Add(histMU);
    histStack->Add(histCE);

    histCH->SetFillColor(kRed - 4);
    histNE->SetFillColor(kOrange - 3);
    histNH->SetFillColor(kYellow - 7);
    histMU->SetFillColor(kGreen + 2);
    histCE->SetFillColor(kBlue - 6);

    TCanvas* canvasCH = new TCanvas("canvasCH", "CH;#eta;Energy Fraction", 800, 600);
    histCH->Draw("HIST");
    canvasCH->SaveAs("Plots/EnergyFractionPF/CH_EFrac.png");

    TCanvas* canvasNE = new TCanvas("canvasNE", "NE;#eta;Energy Fraction", 800, 600);
    histNE->Draw("HIST");
    canvasNE->SaveAs("Plots/EnergyFractionPF/NE_EFrac.png");

    TCanvas* canvasNH = new TCanvas("canvasNH", "NH;#eta;Energy Fraction", 800, 600);
    histNH->Draw("HIST");
    canvasNH->SaveAs("Plots/EnergyFractionPF/NH_EFrac.png");

    TCanvas* canvasMU = new TCanvas("canvasMU", "MU;#eta;Energy Fraction", 800, 600);
    histMU->Draw("HIST");
    canvasMU->SaveAs("Plots/EnergyFractionPF/MU_EFrac.png");

    TCanvas* canvasCE = new TCanvas("canvasCE", "CE;#eta;Energy Fraction", 800, 600);
    histCE->Draw("HIST");
    canvasCE->SaveAs("Plots/EnergyFractionPF/CE_EFrac.png");

    TCanvas* canvasGrid = new TCanvas("canvasGrid", "2x3 Grid", 1200, 800);
    canvasGrid->Divide(3, 2);

    canvasGrid->cd(1);
    histStack->Draw("HIST");

    canvasGrid->cd(2);
    histCH->Draw("HIST");

    canvasGrid->cd(3);
    histNE->Draw("HIST");

    canvasGrid->cd(4);
    histNH->Draw("HIST");

    canvasGrid->cd(5);
    histMU->Draw("HIST");

    canvasGrid->cd(6);
    histCE->Draw("HIST");

    canvasGrid->SaveAs("Plots/EnergyFractionPF/PFEnergyGrid.png");
    cout << "numCH: " << numCH << endl;
    cout << "numNH: " << numNH << endl;
    cout << "numCE: " << numCE << endl;
    cout << "numMU: " << numMU << endl;
    cout << "numNE: " << numNE << endl;


    //histStack->GetXaxis()->SetTitle("Eta");
    //histStack->GetYaxis()->SetTitle("Energy Fraction");

    TCanvas* canvas = new TCanvas("canvas", "PFflow energy fraction", 800, 600);
    histStack->Draw("HIST");
    auto legend = canvas->BuildLegend();

    legend->SetX1NDC(0.7);
    legend->SetY1NDC(0.7);
    legend->SetX2NDC(0.9);
    legend->SetY2NDC(0.9);

    canvas->SaveAs("Plots/EnergyFractionPF/EnergyFractionStackedPF_2.png"); 
}
