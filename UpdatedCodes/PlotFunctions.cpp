#include <iostream>
#include <TFile.h>
#include <TH1F.h>
#include <TH2D.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TLegend.h>

#include "CutsVariables.h"
#include "HistVariables.h"

void PlotBranchWithCuts(const char* inFileName,
        const char* branchName,
        HistVar1D h,
        CutsVector dPhiCut,
        CutsBranches etaCut){
    
        int nref, hiBin;
        float dPhi;
        Float_t branch[100];
        Float_t jteta[100];

    TFile* inFile = new TFile(inFileName,"READ");
    if (!inFile || inFile->IsZombie()) {
        cout << "Error: Could not open the file!" << endl;
    }

    TTree* JetAnalyzerTree = (TTree*)inFile->Get("akCs4FlowPFJetAnalyzer/t");
    TTree* HiTree = (TTree*)inFile->Get("hiEvtAnalyzer/HiTree");

    TCanvas* canvas = new TCanvas(h.canvasName, h.canvasName, 900, 900);
    TH1F h1("h1", h.histTitle, h.nbin, h.xmin, h.xmax);


    if (h.isLog == true){
        canvas->SetLogy();
    }

    JetAnalyzerTree->SetBranchAddress(etaCut.name.c_str(),jteta);
    JetAnalyzerTree->SetBranchAddress(branchName,branch);
    JetAnalyzerTree->SetBranchAddress("nref",&nref);

    for (int i = 0; i < JetAnalyzerTree->GetEntries(); i++){
        JetAnalyzerTree->GetEntry(i);
        HiTree->GetEntry(i);
        dPhi = dPhiCut.cutVector[i];
        if (dPhi > dPhiCut.cutmin && dPhi < dPhiCut.cutmax){
            for (int j = 0; j < nref; j++){
                if (jteta[j] > etaCut.cutmin && jteta[j] < etaCut.cutmax){
                    h1.Fill(branch[j]);
                }
            }
        }
    }

    gStyle->SetOptStat(0);
    gPad->SetTicks();
    h1.SetXTitle(h.xLabel); // Set the x-axis label
    h1.SetYTitle(h.yLabel);
    h1.Draw("HIST E1");

    TLatex cuts1;
    TLatex cuts2;

    cuts1.SetNDC(); // use normalized device coordinates (0–1)
    cuts1.SetTextSize(0.03);
    cuts2.SetNDC();
    cuts2.SetTextSize(0.03);

    if (dPhiCut.cutmin > 0 || dPhiCut.cutmax < TMath::Pi()){
        cuts1.DrawLatex(0.3, 0.75, Form("%.2f < %s < %.2f",dPhiCut.cutmin,dPhiCut.name.c_str(),dPhiCut.cutmax));
    }
    if (etaCut.cutmin > 0 || etaCut.cutmax < TMath::Pi()){
        cuts2.DrawLatex(0.3,0.70, Form("%.2f < |%s| < %.2f",etaCut.cutmin,etaCut.name.c_str(),etaCut.cutmax));
    }
    canvas->SaveAs(h.outFileName);
    canvas->Close();
}

void PlotTwoBranchesWithCuts(const char* inFileName,
    const char* branchName,
    HistVar1D h,
    CutsVector dPhi1Cut,
    CutsVector dPhi2Cut,
    CutsBranches etaCut){

    int nref, hiBin;
    float dPhi;
    
    Float_t branch[100];
    Float_t jteta[100];

    TFile* inFile = new TFile(inFileName,"READ");
    if (!inFile || inFile->IsZombie()) {
        cout << "Error: Could not open the file!" << endl;
    }

    TTree* JetAnalyzerTree = (TTree*)inFile->Get("akCs4FlowPFJetAnalyzer/t");
    TTree* HiTree = (TTree*)inFile->Get("hiEvtAnalyzer/HiTree");

    TCanvas* canvas = new TCanvas(h.canvasName, h.canvasName, 900, 900);
    TH1F* hist1 = new TH1F("h1", h.histTitle, h.nbin, h.xmin, h.xmax);
    TH1F* hist2 = new TH1F("h2", h.histTitle, h.nbin, h.xmin, h.xmax);

    if (h.isLog == true){
        canvas->SetLogy();
    }
    
    JetAnalyzerTree->SetBranchAddress(etaCut.name.c_str(),jteta);
    JetAnalyzerTree->SetBranchAddress(branchName,branch);
    JetAnalyzerTree->SetBranchAddress("nref",&nref);

    for (int i = 0; i < JetAnalyzerTree->GetEntries(); i++){
        JetAnalyzerTree->GetEntry(i);
        HiTree->GetEntry(i);
        dPhi = dPhi1Cut.cutVector[i];

        if (dPhi > dPhi1Cut.cutmin && dPhi < dPhi1Cut.cutmax){
            for (int j = 0; j < nref; j++){
                if (jteta[j] > etaCut.cutmin && jteta[j] < etaCut.cutmax){
                    hist1->Fill(branch[j]);
                }
            }
        }
        
        else if (dPhi > dPhi2Cut.cutmin && dPhi < dPhi2Cut.cutmax){
            for (int j = 0; j < nref; j++){
                if (jteta[j] > etaCut.cutmin && jteta[j] < etaCut.cutmax){
                    hist2->Fill(branch[j]);
                }
            }
        }
    }

    hist1->Scale(1.0 / hist1->GetEntries());
    hist2->Scale(1.0 / hist2->GetEntries());

    gStyle->SetOptStat(0);
    gPad->SetTicks();
    hist1->SetXTitle(h.xLabel); // Set the x-axis label
    hist1->SetYTitle(h.yLabel);
    hist2->SetXTitle(h.xLabel); // Set the x-axis label
    hist2->SetYTitle(h.yLabel);
    hist1->SetLineColor(kRed);
    hist2->SetLineColor(kBlue);
    hist1->SetFillColor(kRed);
    hist2->SetFillColor(kBlue);
    hist1->SetFillStyle(3004); 
    hist2->SetFillStyle(3005);
    hist1->SetLineWidth(2);
    hist2->SetLineWidth(2);


    hist1->Draw("HIST E1");
    hist2->Draw("HIST E1 SAME");

    TLegend* legend = new TLegend(0.7, 0.7, 0.9, 0.85);
    legend->AddEntry(hist1, Form("%.2f < Dphi < %.2f", dPhi1Cut.cutmin, dPhi1Cut.cutmax), "l");
    legend->AddEntry(hist2, Form("%.2f < Dphi < %.2f", dPhi2Cut.cutmin, dPhi2Cut.cutmax), "l");
    legend->Draw();

    TLatex cuts1, cuts2;

    cuts1.SetNDC(); // use normalized device coordinates (0–1)
    cuts1.SetTextSize(0.03);
    cuts2.SetNDC();
    cuts2.SetTextSize(0.03);

    if (etaCut.cutmin > 0 || etaCut.cutmax < TMath::Pi()){
        cuts2.DrawLatex(0.3,0.80, Form("%.2f < |%s| < %.2f",etaCut.cutmin,etaCut.name.c_str(),etaCut.cutmax));
    }
    canvas->SaveAs(h.outFileName);
    canvas->Close();
}

void plotNhistograms(const vector<TH1*>& hists) {
    int N = hists.size();
    if (N == 0) return;

    // Compute layout: make it roughly square
    int nCols = ceil(sqrt(N));
    int nRows = ceil(double(N) / nCols);

    TCanvas* c = new TCanvas("c", "N Plots", 300*nCols, 300*nRows);
    c->Divide(nCols, nRows);

    for (int i = 0; i < N; ++i) {
        c->cd(i+1);
        hists[i]->Draw();
    }

    c->SaveAs("n_plots.png");
}

//Edit when I get home

void Plot3Hist(const char* inFileName,
    vector<float> Emiss2,
    vector<float> Emiss2p5,
    vector<float> EmissInclusive,
    CutsBranches HiBinCut){
    
    TFile* inFile = new TFile(inFileName,"READ");
    if (!inFile || inFile->IsZombie()) {
        cout << "Error: Could not open the file!" << endl;
    }

    TTree* HiTree = (TTree*)inFile->Get("hiEvtAnalyzer/HiTree");

    int nref, hiBin, n;

    HiTree->SetBranchAddress("hiBin", &hiBin);

    TH1F* hEmissPF2 = new TH1F("hEmissPF2", "EmissPF Comparison;E_{T} Miss PF;Number of Events", 50, 0, 1000);
    TH1F* hEmissPF2p5 = new TH1F("hEmissPF2p5", "EmissPF Comparison;E_{T} Miss PF;Number of Events", 50, 0, 1000);
    TH1F* hEmissPFInclusive = new TH1F("hEmissPFInclusive", "EmissPF Comparison;E_{T} Miss PF;Number of Events", 50, 0, 1000);

    // Fill histograms
    for (int i = 0; i < Emiss2.size(); i++) {
        HiTree->GetEntry(i);
        if (hiBin < HiBinCut.cutmin || hiBin > HiBinCut.cutmax) continue; // Apply the cut
        cout << "EMiss2:" << Emiss2.at(i) << endl;
        cout << "HiBin:" << hiBin << endl;
        hEmissPF2->Fill(Emiss2.at(i));
        hEmissPF2p5->Fill(Emiss2p5.at(i));
        hEmissPFInclusive->Fill(EmissInclusive.at(i));
    }

    // Normalize histograms to unity
    hEmissPF2->Scale(1.0 / hEmissPF2->Integral());
    hEmissPF2p5->Scale(1.0 / hEmissPF2p5->Integral());
    hEmissPFInclusive->Scale(1.0 / hEmissPFInclusive->Integral());

    // Create a canvas and draw the histograms
    TCanvas* c1 = new TCanvas("c1", "EmissPF Comparison for different eta bins", 800, 600);
    hEmissPF2->SetLineColor(kRed);
    hEmissPF2p5->SetLineColor(kBlue);
    hEmissPFInclusive->SetLineColor(kGreen+2); // Change green to magenta for less jarring color

    hEmissPF2->Draw("HIST");
    hEmissPF2p5->Draw("HIST SAME");
    hEmissPFInclusive->Draw("HIST SAME");

    // Add HiBin cut text to the plot
    TLatex hiBinCutText;
    hiBinCutText.SetNDC();
    hiBinCutText.SetTextSize(0.03);
    hiBinCutText.DrawLatex(0.3, 0.85, Form("HiBin: %.0f  - %.0f ", HiBinCut.cutmin, HiBinCut.cutmax));

    // Add a legend
    TLegend* legend = new TLegend(0.7, 0.7, 0.9, 0.9);
    legend->AddEntry(hEmissPF2, "|eta| < 2.0)", "l");
    legend->AddEntry(hEmissPF2p5, "|eta| < 2.5)", "l");
    legend->AddEntry(hEmissPFInclusive, "Inclusive", "l");
    legend->Draw();
    gStyle->SetOptStat(0);
    
    c1->SaveAs(Form("Plots/EmissPF_Comparison_quickdraw_%.0fto%.0f.png",HiBinCut.cutmin,HiBinCut.cutmax));
}