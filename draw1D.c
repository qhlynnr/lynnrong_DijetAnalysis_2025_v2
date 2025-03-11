#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>

void drawHistogram(
    TTree* tree,
    const char* branch,
    const char* histTitle = "histTitle",
    const char* outputFileName = "noName.png",
    const char* xAxisName = "",
    int nBins = 100,
    double xMin = -10,
    double xMax = 400,
    bool isLog = false
){
    TH1F h_tree(branch, histTitle, nBins, xMin, xMax);
    tree->Draw(TString::Format("%s >> %s", branch, branch).Data());
//
    TString descrptstr = "";
    TLatex *t = new TLatex(0.5,0.6,descrptstr.Data());

    TCanvas canvas(canvasTitle, canvasTitle, 800, 600);


    if (isLog == true){
        canvas.SetLogy(); // Optional: set logarithmic scale
    }


    h_tree.SetXTitle(xAxisName); // Set the x-axis label
    h_tree.Draw("HIST");

    canvas.SaveAs(outputFileName);
}

void findMax(TTree* tree,
	const char* branchName,
	const char* branchName2,
	const char* branchName3){

    double branch;
    double max_value = 0;

    int branch2;
    int branch2_value = 0;

    double branch3;
    double branch3_value = 0;

    tree->SetBranchAddress(branchName, &branch);
    tree->SetBranchAddress(branchName2, &branch2);
    tree->SetBranchAddress(branchName3, &branch3);

    Long64_t nEntries = tree->GetEntries();
    for (Long64_t i = 0; i < nEntries; i++) {
        tree->GetEntry(i);
        if (branch > max_value) {
            max_value = branch;
	    branch2_value = branch2;
	    branch3_value = branch3;
        }
    }
    cout << "The maximum value of" << branchName << "is: " << max_value << endl;
    cout << "The corresponding event number is:" << branch2_value << endl;
    cout << "Corresponding" << branchName3 << "is: " << branch3_value << endl;

}

void run(const char* inputFileName, const char* dirName) {
    const char* treeName = dirName;
    TFile* file = TFile::Open(inputFileName);
    if (!file || file->IsZombie()){
	    cerr << "No File Found" << endl;
	    return;
    }

    TDirectory *dir = (TDirectory*)file->Get(dirName);
    if (!dir){
	    cerr << "No directory Found" << endl;
	    return;
    }
    TDirectory *ParticleFlowAnalyser = (TDirectory*)file->Get("ParticleFlowAnalyser");
    TDirectory * ak4pfJetAnalyser
    TTree* jetTree = (TTree*)dir->Get(treeName);
    if (!jetTree) return;

    TTree* particleTree = (TTree*)file->Get("particleTree");
    TTree* eventTree = (TTree*)file->Get("eventTree");

    drawHistogram(eventTree,
	"jPtMiss",
	"EMiss(for jets) 10K pp collisions with R=0.4",
	"canvas",
	"jPtmiss_5200GeV_pTHat400_100K_R0p4_excludingNeutrinosLog.png",
	"jPt(GeV)",
	100,
	0,
	380,
	true
	);


}

