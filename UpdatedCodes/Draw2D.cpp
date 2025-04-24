#include <TFile.h>
#include <TH1F.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TH2D.h>

struct Result {
	vector<int> event_array = {};
	vector<string> cutNames = {};
};

vector<int> ReturnAsymmetricEvents(const char* inFileName,
			CutsBranches jtetacut,
			CutsBranches HiBincut,
			CutsVector dPhicut,
			CutsVector AJcut,
			CutsVector EMisscut){
	Result result;

	TFile* inFile = new TFile(inFileName,"READ");
	if (!inFile || inFile->IsZombie()) {
		cout << "Error: Could not open the file!" << endl;
	}
			
	TTree* PFTree = (TTree*)inFile->Get("particleFlowAnalyser/pftree");
	TTree* HiTree = (TTree*)inFile->Get("hiEvtAnalyzer/HiTree");
	TTree* JetAnalyserTree = (TTree*)inFile->Get("akCs4FlowPFJetAnalyzer/t");
			
	int n, nref;
	float dPhi,A_J,Emiss;
	float jtpt1 = -1;
	float jtpt2 = -1;
	int hiBin;

	int counter = 0;

	Float_t jtpt[100];
	Float_t jteta[100];

	vector<float>* pfpt = nullptr;
	vector<float>* pfE = nullptr;
	vector<float>* pfEta = nullptr;
	vector<float>* pfPhi = nullptr;

	vector<int> event_array = {};
		
	PFTree->SetBranchAddress("nPF",&n);
	PFTree->SetBranchAddress("pfPt",&pfpt);
	PFTree->SetBranchAddress("pfE",&pfE);
	PFTree->SetBranchAddress("pfPhi",&pfPhi);
	PFTree->SetBranchAddress("pfEta",&pfEta);

	JetAnalyserTree->SetBranchAddress("nref",&nref);
	JetAnalyserTree->SetBranchAddress("jtpt",jtpt);
	JetAnalyserTree->SetBranchAddress("jteta",jteta);

	HiTree->SetBranchAddress("hiBin",&hiBin);
	
	for(int i = 0; i < PFTree->GetEntries(); i++){
		PFTree->GetEntry(i);
		JetAnalyserTree->GetEntry(i);
		HiTree->GetEntry(i);

		dPhi = dPhicut.cutVector[i];
		A_J = AJcut.cutVector[i];
		Emiss = EMisscut.cutVector[i];
		if (nref >= 2){
			jtpt1 = jtpt[0];
			jtpt2 = jtpt[1];
		}
		else{
			jtpt1 = -1;
			jtpt2 = -1;
		}

		if (dPhi < dPhicut.cutmin || dPhi > dPhicut.cutmax) continue;
		if (A_J < AJcut.cutmin || A_J > AJcut.cutmax) continue;
		if (Emiss < EMisscut.cutmin || Emiss > EMisscut.cutmax) continue;
		if (hiBin < HiBincut.cutmin || hiBin > HiBincut.cutmax) continue;
		if (jtpt2 < 50) continue;
		if (jtpt1 < 100) continue;
		if (jteta[0] < jtetacut.cutmin || jteta[0] > jtetacut.cutmax) continue;
		cout << "event: " << i << endl;
		for(int j = 0;j < n;j++){
			//cout << " part index: " << j << endl;
			//cout << "pfpt: " << pfpt->at(j) << endl;
			//cout << "pfE: " << pfE->at(j) << endl;
			//cout << "pfEta: " << pfEta->at(j) << endl;	
		//	if (pfEta->at(j) < pfetacut.cutmin || pfEta->at(j) > pfetacut.cutmax) continue;
		}
		cout << "jtpt1: " << jtpt1 << endl;
		cout << "jtpt2: " << jtpt2 << endl;
		cout << "Leading jteta: " << jteta[0] << endl;
		cout << "dPhi: " << dPhi << endl;
		cout << "A_J: " << A_J << endl;
		cout << "Emiss: " << Emiss << endl;
		cout << "Cent: " << hiBin << endl;
		event_array.push_back(i);
		cout << endl;
		counter++;
	}
	cout << "Total number of events: " << counter << endl;
	return event_array;
}

void draw2D(const char* inFileName,
		vector<int> event_array,
		CutsBranches jtetacut,
		CutsBranches HiBincut,
		CutsVector dPhicut,
		CutsVector AJcut,
		CutsVector EMisscut){

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

    auto c1 = new TCanvas("c1","canvas",0,0,900,600);
    TH2D* graph = new TH2D ("h2","Pt Distribution of 1 Event in 2D;#phi;#eta;pt", 50, -TMath::Pi(), TMath::Pi(), 50,-2,2);
	for(int i = 0; i < event_array.size(); i++){
		PFTree->GetEntry(event_array[i]);
		graph->Reset(); // Clear the histogram before filling it with new data
	
		for(int j = 0; j < n; j++){
			graph->Fill(pfPhi->at(j), pfEta->at(j), pfpt->at(j));
		}
		gStyle->SetOptStat(0);
		graph->Draw("LEGO2 Z");
		c1->SetTheta(45.);
		c1->SetPhi(60.);
		//c1->SetLogz();

		vector<string> cutTexts = {
			Form("%.2f < %s < %.2f", jtetacut.cutmin, jtetacut.name.c_str(), jtetacut.cutmax),
			Form("%.2f < %s < %.2f", dPhicut.cutmin, dPhicut.name.c_str(), dPhicut.cutmax),
			Form("%.2f < %s < %.2f", HiBincut.cutmin, HiBincut.name.c_str(), HiBincut.cutmax),
			Form("%.2f < %s < %.2f", AJcut.cutmin, AJcut.name.c_str(), AJcut.cutmax),
			Form("%.2f < %s", EMisscut.cutmin, EMisscut.name.c_str()),
			"jtpt2 > 50",
			"jtpt1 > 100",
		};

		vector<TLatex> cuts(cutTexts.size());

		for (size_t j = 0; j < cuts.size(); ++j) {
			cuts[j].SetNDC();
			cuts[j].SetTextSize(0.03);
			cuts[j].DrawLatex(0.1, 0.90 - j * 0.05, cutTexts[j].c_str());
		}

		string outFileName = Form("Plots/2DDiagrams_SmallEta/HiForestPartialskim_Event%d_pf.jpg",
			event_array[i]);

		c1->SaveAs(outFileName.c_str());
	}
}
