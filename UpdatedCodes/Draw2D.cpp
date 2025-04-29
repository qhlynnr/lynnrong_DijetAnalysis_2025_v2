#include <TFile.h>
#include <TH1F.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TH2D.h>
#include "EventInfo.h"


vector<EventInfo> ReturnAsymmetricEvents(const char* inFileName,
			CutsBranches jtetacut,
			CutsBranches HiBincut,
			CutsVector dPhicut,
			CutsVector AJcut,
			CutsVector EMisscut,
			CutsVector EMisscut2,
			CutsVector EMisscut2p5){

	TFile* inFile = new TFile(inFileName,"READ");
	if (!inFile || inFile->IsZombie()) {
		cout << "Error: Could not open the file!" << endl;
	}
			
	TTree* PFTree = (TTree*)inFile->Get("particleFlowAnalyser/pftree");
	TTree* HiTree = (TTree*)inFile->Get("hiEvtAnalyzer/HiTree");
	TTree* JetAnalyserTree = (TTree*)inFile->Get("akCs4FlowPFJetAnalyzer/t");
			
	int n, nref;
	float dPhi, A_J, Emiss;
	float jtpt1 = -1;
	float jtpt2 = -1;
	int hiBin;

	int counter = 0;
	EventInfo eventInfo;
	vector<EventInfo> eventInfoArray;

	Float_t jtpt[100];
	Float_t jteta[100];

	vector<float>* pfpt = nullptr;
	vector<float>* pfE = nullptr;
	vector<float>* pfEta = nullptr;
	vector<float>* pfPhi = nullptr;

	vector<int> event_array = {};

	JetAnalyserTree->SetBranchAddress("nref", &nref);
	JetAnalyserTree->SetBranchAddress("jtpt", jtpt);
	JetAnalyserTree->SetBranchAddress("jteta", jteta);	

	HiTree->SetBranchAddress("hiBin", &hiBin);
	
	for (int i = 0; i < PFTree->GetEntries(); i++) {
		JetAnalyserTree->GetEntry(i);
		HiTree->GetEntry(i);

		dPhi = dPhicut.cutVector[i];
		A_J = AJcut.cutVector[i];
		Emiss = EMisscut.cutVector[i];
		if (nref >= 2) {
			jtpt1 = jtpt[0];
			jtpt2 = jtpt[1];
		} else {
			jtpt1 = -1;
			jtpt2 = -1;
		}

		if (dPhi < dPhicut.cutmin || dPhi > dPhicut.cutmax) continue;
		if (A_J < AJcut.cutmin || A_J > AJcut.cutmax) continue;
		if (Emiss < EMisscut.cutmin || Emiss > EMisscut.cutmax) continue;
		if (hiBin < HiBincut.cutmin || hiBin > HiBincut.cutmax) continue;
		if (jtpt2 < 50) continue;
		if (jtpt1 < 300) continue;
		if (jteta[0] < jtetacut.cutmin || jteta[0] > jtetacut.cutmax) continue;

		event_array.push_back(i);
		cout << "Event:" << i << endl;
		cout << "A_J:" << A_J << endl;
		cout << "dPhi:" << dPhi << endl;
		cout << "Emiss:" << Emiss << endl;
		cout << "jtpt1:" << jtpt1 << endl;
		cout << "jtpt2:" << jtpt2 << endl;
		cout << "Leading jteta:" << jteta[0] << endl;
		cout << "Cent:" << hiBin << endl;
		cout << endl;

		eventInfo.event = i;
		eventInfo.jtpt1 = jtpt1;
		eventInfo.jtpt2 = jtpt2;
		eventInfo.leading_jteta = jteta[0];
		eventInfo.dPhi = dPhi;
		eventInfo.A_J = A_J;
		eventInfo.Emiss_eta2 = EMisscut2.cutVector[i];
		eventInfo.Emiss_eta2p5 = EMisscut2p5.cutVector[i];
		eventInfo.Emiss_inclusive = EMisscut.cutVector[i];
		eventInfo.cent = hiBin;
		counter++;

		eventInfoArray.push_back(eventInfo);
	}

	cout << "Total number of events: " << counter << endl;
	return eventInfoArray;
}

void draw2D(const char* inFileName,
		CutsBranches jtetacut,
		CutsBranches HiBincut,
		CutsVector dPhicut,
		CutsVector AJcut,
		CutsVector EMisscut2,
		CutsVector EMisscut2p5,
		vector<EventInfo> eventInfoArray){

	TFile* inFile = new TFile(inFileName,"READ");
	if (!inFile || inFile->IsZombie()) {
		cout << "Error: Could not open the file!" << endl;
	}
			
	TTree* PFTree = (TTree*)inFile->Get("particleFlowAnalyser/pftree");
	TTree* HiTree = (TTree*)inFile->Get("hiEvtAnalyzer/HiTree");
			
	int n;
	float dPhi;

	vector<float>* pfpt = nullptr;
	vector<float>* pfE = nullptr;
	vector<float>* pfEta = nullptr;
	vector<float>* pfPhi = nullptr;

	PFTree->SetBranchAddress("nPF",&n);
	PFTree->SetBranchAddress("pfPt",&pfpt);
	PFTree->SetBranchAddress("pfE",&pfE);
	PFTree->SetBranchAddress("pfPhi",&pfPhi);
	PFTree->SetBranchAddress("pfEta",&pfEta);

	// Define the eta bins
	vector<double> eta_bins = {-5.191,-4.889,-4.716,-4.538,
		-4.363,-4.191,-4.013,-3.839,-3.664,-3.489,-3.314,-3.139,
		-3,-2.853,-2.65,-2.5,-2.322,-2.172,-2.043,-1.93,-1.83,-1.74,
		-1.653,-1.566,-1.479,-1.392,-1.305,-1.218,-1.131,-1.044,-0.957,
		-0.87,-0.783,-0.696,-0.609,-0.522,-0.435,-0.348,-0.261,-0.174,
		-0.087,0,0.087,0.174,0.261,0.348,0.435,0.522,0.609,0.696,0.783,0.87,
		0.957,1.044,1.131,1.218,1.305,1.392,1.479,1.566,1.653,1.74,1.83,1.93,
		2.043,2.172,2.322,2.5,2.65,2.853,3,3.139,3.314,3.489,3.664,3.839,4.013,
		4.191,4.363,4.538,4.716,4.889,5.191};


    auto c1 = new TCanvas("c1","canvas",0,0,900,700);
    TH2D* graph = new TH2D ("h2","Particle Flow Pt Distribution of 1 Event in 2D;#phi;#eta;pt", 
		72, -TMath::Pi(), TMath::Pi(), eta_bins.size()-1, eta_bins.data());
	for(int i = 0; i < eventInfoArray.size(); i++){
		PFTree->GetEntry(eventInfoArray[i].event);
		graph->Reset(); // Clear the histogram before filling it with new data
	
		for(int j = 0; j < n; j++){
			graph->Fill(pfPhi->at(j), pfEta->at(j), pfpt->at(j));
		}
		gStyle->SetOptStat(0);
		gPad->SetFrameBorderMode(0);
		graph->GetXaxis()->SetAxisColor(0);
		graph->GetYaxis()->SetAxisColor(0);
		graph->GetZaxis()->SetAxisColor(0); // optional for color scale
		graph->GetXaxis()->SetTitleOffset(1.5);

		gPad->SetFrameLineColor(0);
		
		graph->Draw("LEGO2 Z");

		c1->SetTheta(45.);
		c1->SetPhi(100.);
		//c1->SetLogz();

		vector<string> cutTexts = {
			Form("%.2f < %s < %.2f", jtetacut.cutmin, jtetacut.name.c_str(), jtetacut.cutmax),
			Form("%.2f < %s < %.2f", dPhicut.cutmin, dPhicut.name.c_str(), dPhicut.cutmax),
			Form("%.2f < %s < %.2f", HiBincut.cutmin, HiBincut.name.c_str(), HiBincut.cutmax),
			Form("%.2f < %s < %.2f", AJcut.cutmin, AJcut.name.c_str(), AJcut.cutmax),
			Form("%.2f < %s", EMisscut2p5.cutmin, EMisscut2p5.name.c_str()),
			"jtpt2 > 50GeV",
			"jtpt1 > 300GeV",
		};

		vector<TLatex> cuts(cutTexts.size());

		for (size_t j = 0; j < cuts.size(); ++j) {
			cuts[j].SetNDC();
			cuts[j].SetTextSize(0.02);
			cuts[j].DrawLatex(0.1, 0.90 - j * 0.03, cutTexts[j].c_str());
		}

		TLatex latex;
        latex.SetNDC();
        latex.SetTextSize(0.02);
		EventInfo eventInfo = eventInfoArray[i];
        latex.DrawLatex(0.7, 0.90, Form("Event: %d", eventInfo.event));
        latex.DrawLatex(0.7, 0.88, Form("jtpt1: %.2f", eventInfo.jtpt1));
		latex.DrawLatex(0.7, 0.86, Form("jtpt2: %.2f", eventInfo.jtpt2));
		latex.DrawLatex(0.7, 0.84, Form("jteta1: %.2f", eventInfo.leading_jteta));
        latex.DrawLatex(0.7, 0.82, Form("dPhi: %.2f", eventInfo.dPhi));
        latex.DrawLatex(0.7, 0.80, Form("A_J: %.2f", eventInfo.A_J));
        latex.DrawLatex(0.7, 0.78, Form("Emiss (eta < 2): %.2f", eventInfo.Emiss_eta2));
		latex.DrawLatex(0.7, 0.76, Form("Emiss (eta < 2.5): %.2f", eventInfo.Emiss_eta2p5));
		latex.DrawLatex(0.7, 0.74, Form("Emiss (inclusive): %.2f", eventInfo.Emiss_inclusive));
        latex.DrawLatex(0.7, 0.72, Form("Cent: %d", eventInfo.cent));

		string outFileName = Form("Plots/2DDiagrams_CorrectBinning/HiForestPartialskim_Event%d_pf_correctbinning.png",
			eventInfoArray[i].event);

		c1->SaveAs(outFileName.c_str());
	}
}
