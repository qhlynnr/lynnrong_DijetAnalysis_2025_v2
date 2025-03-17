#include <TFile.h>
#include <TH1F.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TMath.h>

using namespace std;

TTree* ReturnTree(const char* inputFileName,
		const char* dirName,
		const char* treeName){

    TFile* file = TFile::Open(inputFileName);
    if (!file || file->IsZombie()){
	cerr << "Error: No File Found" << endl;
	return nullptr;
    }

    TDirectory* dir = (TDirectory*)file->Get(dirName);
    if (!dir){
	cerr << "Error: No directory Found named: "<< dirName << endl;
	return nullptr;
    }

    TTree* tree = (TTree*)dir->Get(treeName);
    if (!tree){
        cerr << "Error: No Tree Named:" << treeName << endl;
        return  nullptr;
    }
    return tree;

}

void DrawHistVectorNoTree(vector<float>& dataVec,
		const char* xLabel = "xLabel",
		const char* canvasName = "canvasName",
                const char* histTitle = "default title",
                const char* outFileName = "default.png",
                int nbin = 50,
                int xmin = 0,
                int xmax = 200,
		bool isLog = false){

    TCanvas* canvas = new TCanvas(canvasName, canvasName, 800, 600);
    TH1F h1("h1", histTitle, nbin, xmin, xmax);
    if (isLog == true){    
        canvas->SetLogy();
    }

    for (float value : dataVec){
	h1.Fill(value);
    }
   

    h1.SetXTitle(xLabel); // Set the x-axis label
    h1.Draw("HIST");
    canvas->SaveAs(outFileName);
}
void DrawHistFVector(TTree* tree,
		const char* branchName, //must be a float_t array
		const char* indexName = "nref",
		const char* canvasName = "canvasName",
                const char* histTitle = "default title",
                const char* outFileName = "default.png",
                int nbin = 50,
                int xmin = 0,
                int xmax = 200,
		int pfidset = 0,
		bool isLog = false){
    int n;
    vector<float>* branch = nullptr;
    vector<int>* pfid = nullptr;

    tree->SetBranchAddress(indexName,&n);
    tree->SetBranchAddress(branchName,&branch);
    tree->SetBranchAddress("pfId",&pfid);

    cout << "In Function Branch Name:" << branchName << endl;
    cout << "pfid set: " << pfidset << endl;

    tree->Scan("pfEta:pfPhi");


    TCanvas* canvas = new TCanvas(canvasName, canvasName, 800, 600);
    TH1F h1("h1", histTitle, nbin, xmin, xmax);

    if (isLog == true){    
        canvas->SetLogy();
    }
   
    for (int i = 0; i < tree->GetEntries(); i++){
        tree->GetEntry(i);
        for (int j = 0; j < n; j++){
	    if (!branch->empty()){
		if (pfid->at(j) == pfidset){
		    //cout << branch->at(j) << endl;
                    h1.Fill(branch->at(j));
		}
            }
        }
    }

    h1.SetXTitle(branchName); // Set the x-axis label
    h1.Draw("HIST");
    canvas->SaveAs(outFileName);
}

void DrawHistFArray(TTree* tree,
		const char* branchName, //must be a float_t array
		const char* indexName = "nref",
		const char* canvasName = "canvasName",
                const char* histTitle = "default title",
                const char* outFileName = "default.png",
                int nbin = 50,
                int xmin = 0,
                int xmax = 200,
		bool isLog = false){
    int n;
    Float_t branch[100];

    tree->SetBranchAddress(indexName,&n);
    tree->SetBranchAddress(branchName,branch);

    if (!n||!branch[0]){
	    cerr << "BranchAintFound" << endl;
	    cerr << branchName << endl;
	    cerr << indexName << endl;
	    return;
    }

    TCanvas* canvas = new TCanvas(canvasName, canvasName, 800, 600);
    TH1F h1("h1", histTitle, nbin, xmin, xmax);

    if (isLog == true){    
        canvas->SetLogy();
    }
   

    for (int i = 0; i < tree->GetEntries(); i++){
        tree->GetEntry(i);
        if (tree->GetEntry(i) <= 0) {  // Check if GetEntry fails
            cerr << "Error: Failed to read entry " << i << endl;
            break;
        }
        for (int j = 0; j < n; j++){
            if (!branch[j]){
                cerr << "Error here in Drawhist: " << endl;
                break;
            }
            else{
                    h1.Fill(branch[j]);
		}
        }
    }

    h1.SetXTitle(branchName); // Set the x-axis label
    h1.Draw("HIST");
    canvas->SaveAs(outFileName);
}

tuple<vector<float>,vector<float>,vector<float>>GetPt1Pt2(TTree* JetAnalyserTree){

    float jtpt1 = -1;
    float jtpt2 = -1;
    float A_J = 0;
    Float_t jtpt[100];
    int nref;
    
    vector<float> jtpt1vector;
    vector<float> jtpt2vector;
    vector<float> ajvector;

    JetAnalyserTree->SetBranchAddress("nref",&nref);
    JetAnalyserTree->SetBranchAddress("jtpt",jtpt);

    for (int i = 0; i < JetAnalyserTree->GetEntries(); i++){
        JetAnalyserTree->GetEntry(i);
	vector<float> orderedJtpt = {};
        for (int j = 0; j < nref; j++){
            if (!jtpt[j]){
		cerr << "Error in entry number: "<< i <<j << endl;
                break;
            }
            else{
                orderedJtpt.push_back(jtpt[j]);
	    }
	}

	sort(orderedJtpt.begin(),orderedJtpt.end(),greater<float>());
	if (nref >= 2){
		jtpt1 = orderedJtpt[0];
		jtpt2 = orderedJtpt[1];
		A_J = (jtpt1-jtpt2)/(jtpt1+jtpt2);
		jtpt1vector.push_back(jtpt1);
		jtpt2vector.push_back(jtpt2);
		ajvector.push_back(A_J);
	}
    }
    return make_tuple(jtpt1vector,jtpt2vector, ajvector);
}

vector<float> GetEmiss(TTree* tree){
    float EMissjpt = 0;
    float jptxtot, jptytot;

    int nref;
    Float_t jtpt[100];
    Float_t jtphi[100];

    vector<float> EMiss;

    tree->SetBranchAddress("nref",&nref);
    tree->SetBranchAddress("jtpt",jtpt);
    tree->SetBranchAddress("jtphi",jtphi);

    for (int i = 0; i < tree->GetEntries(); i++){
	EMissjpt = 0;
	jptxtot = 0;
	jptytot = 0;
        tree->GetEntry(i);
        for (int j = 0; j < nref; j++){
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

void draw2D(TTree* tree){
	int n;
	vector<float>* pfpt = nullptr;
	vector<float>* pfE = nullptr;
	vector<float>* pfEta = nullptr;
	vector<float>* pfPhi = nullptr;

	vector<int> event_array = {};

	tree->SetBranchAddress("nPF",&n);
	tree->SetBranchAddress("pfPt",&pfpt);
	tree->SetBranchAddress("pfE",&pfE);
	tree->SetBranchAddress("pfPhi",&pfPhi);
	tree->SetBranchAddress("pfEta",&pfEta);

        auto c1 = new TCanvas("c","canvas",0,0,600,400);
        TH2D* graph = new TH2D ("h2","Distribution of 1 Event;Phi;Eta", 50, -6, 6, 50,-4,4);

/*
	for(int i = 0; i < tree->GetEntries(); i++){
		tree->GetEntry(i);
		for(int j = 0;j < n;j++){
			if (pfE->at(j) > 2000){
				cout << "event: " << i << " part index: " << j << endl;
				cout << "pfpt: " << pfpt->at(j) << endl;
				cout << "pfE: " << pfE->at(j) << endl;
				event_array.push_back(i);
				cout << endl;
			}
		}
	}*/

	tree->GetEntry(5139);
	for(int i = 0; i < n; i++){
		graph->Fill(pfPhi->at(i),pfEta->at(i),pfpt->at(i));
	}
	graph->Draw("LEGO");
	c1->SetTheta(45.);
    	c1->SetPhi(60.);
	c1->SetLogz();
	c1->SaveAs("Plots_partialskim_Event5139_ptE2801_Logz.jpg");

}

void run(const char* inputFileName) {
    TTree* JetAnalyserTree = ReturnTree(inputFileName,"akCs4FlowPFJetAnalyzer","t");
    TTree* PFTree = ReturnTree(inputFileName,"particleFlowAnalyser","pftree");
    auto pt1pt2Tuple = GetPt1Pt2(JetAnalyserTree);

    vector<float> jtpt1,jtpt2,A_J,EMiss,HMiss;

    jtpt1 = get<0>(GetPt1Pt2(JetAnalyserTree));
    jtpt2 = get<1>(GetPt1Pt2(JetAnalyserTree));
    A_J = get<2>(GetPt1Pt2(JetAnalyserTree));
    EMiss = GetEmiss(JetAnalyserTree);

    //draw2D(PFTree);

    string branchnamearr[] = {"pfPt","pfEta","pfPhi"};
    int xmaxarr[] = {750,6,6};
    int xminarr[] = {0,-6,-6};
    bool boolarr[] = {true,false,false};

    for (int i = 1;i < 7;i++){
	    for (int j = 1;j < 3;j++){
		    string branchname = branchnamearr[j];
		    string title = "";
		    string filename = "";

		    title += branchname;
		    title += " of particle flow object pfId = ";
		    title += to_string(i);

		    filename += "Plots_";
		    filename += branchname;
		    filename += "_pfid";
		    filename += to_string(i);
		    filename += ".png";
		    
		    cout << "pfID: " << i << endl;
		    cout << "branchName: "<< branchname << endl;
		    cout << "xmax: " << xmaxarr[j] << endl;
		    cout << "xmin: " << xminarr[j] << endl;
		    cout << filename << endl;

		    DrawHistFVector(PFTree,
			branchname.c_str(), //must be a float_t array
			"nPF",
			title.c_str(),
			title.c_str(),
			filename.c_str(),
			300,
			xminarr[j],
			xmaxarr[j],
			i,
			boolarr[j]);}
    }
}
