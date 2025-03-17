#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <iostream>
#include <fastjet/PseudoJet.hh>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TRandom3.h>
#include <TLegend.h>
#include "fastjet/ClusterSequence.hh"


using namespace std;
using namespace fastjet;

// Standard FastJet Code
// Input: .root file with ievent, px, py, pz, E
// Output: .root with TTree with Leading and Subleading Jets
// REMEMBER TO CHANGE INPUT FILE


void ParticleToJet(const string &inFileName, const string &outputFileName = ""){

   // Open the file containing particle data
    TFile *inputFile = new TFile(inFileName.c_str(), "READ");
    if (!inputFile || inputFile->IsZombie()) {
    	std::cerr << "Error: Cannot open ROOT file!" << std::endl;
    }

    TTree *tree = (TTree*)inputFile->Get("eventsTree");
    if (!tree) {
        std::cerr << "Error: Cannot find TTree 'eventTree' in the file!" << std::endl;
    }


    map<int,vector<PseudoJet>> event_particles;
    int iEvent = tree->GetEntries();
    int eventNumParts, pdg;
    double px, py, pz, E;
    string outFileName;

    tree->SetBranchAddress("eventNum", &eventNumParts);
    tree->SetBranchAddress("pdgId", &pdg);
    tree->SetBranchAddress("px", &px);
    tree->SetBranchAddress("py", &py);
    tree->SetBranchAddress("pz", &pz);
    tree->SetBranchAddress("E", &E);


    for (int i = 0; i < iEvent; ++i){
        tree->GetEntry(i);
	//No Neutrinos
        if (abs(pdg) != 12 && abs(pdg) != 14 && abs(pdg) != 16){
            event_particles[eventNumParts].push_back(PseudoJet(px, py, pz, E));
	} else {
	    cout << "pdgId not clustered:" << pdg << endl;
	}
    }

    // Define outFileName
    TFile *file = TFile::Open(outputFileName.c_str(), "RECREATE");
    if (!file || file->IsZombie()) {
        cerr << "Error opening ROOT file" << endl;
    }

    // Declare Tree variables and Branch variables
    TTree *particleTree = new TTree("particleTree","Tree Storing Particle Data");
    TTree *jetTree = new TTree("jetTree", "Tree storing leading and subleading jet pT");
    TTree *eventTree = new TTree("eventTree", "Tree storing per-event data");

    int  nConstituents, eventNum;
    double jPt, jPx, jPy, jPz, jE, jPhi, jEta; 
    double jPt1, jEta1, jPhi1, jE1, jPt2, A_J;
    double pTMiss, jPtMiss,pt,pEta,pPhi;

    particleTree->Branch("pt",&pt,"pt/D");
    particleTree->Branch("px",&px,"px/D");
    particleTree->Branch("py",&py,"py/D");
    particleTree->Branch("pz",&pz,"pz/D");
    particleTree->Branch("pEta",&pEta,"pEta/D");
    particleTree->Branch("pPhi",&pPhi,"pPhi/D");
    particleTree->Branch("eventNum",&eventNum,"eventNum/I");

    jetTree->Branch("nConstituents", &nConstituents, "nConstituents/I"); 
    jetTree->Branch("jPt" , &jPt,  "jPt/D" );
    jetTree->Branch("jPx" , &jPx, "jPx/D");
    jetTree->Branch("jPy" , &jPy, "jPy/D");
    jetTree->Branch("jPz" , &jPz, "jPz/D");
    jetTree->Branch("jE" , &jE, "jE/D");
    jetTree->Branch("jEta", &jEta, "jEta/D");
    jetTree->Branch("jPhi", &jPhi, "jPhi/D");
    jetTree->Branch("eventNum",&eventNum,"eventNum/I");

    eventTree->Branch("eventNum",&eventNum,"eventNum/I");
    eventTree->Branch("pTMiss",&pTMiss,"pTMiss/D");
    eventTree->Branch("jPtMiss",&jPtMiss,"jPtMiss/D");

    eventTree->Branch("jPt1", &jPt1, "jPt1/D");  
    eventTree->Branch("jEta1", &jEta1, "jEta/D");  
    eventTree->Branch("jPhi1", &jPhi1, "jPhi1/D");  
    eventTree->Branch("jE1", &jE1, "jE1/D");  

    eventTree->Branch("jPt2", &jPt2, "jPt2/D");
    eventTree->Branch("A_J",&A_J,"A_J/D"); 


    // Define the jet clustering algorithm: anti-kT algorithm with R = 0.2
    	double R = 0.4;
    	JetDefinition jet_def(antikt_algorithm, R);

    // Iterate over events	
    for (const auto& event : event_particles) {
        int eventNumber = event.first;
        const vector<PseudoJet>& particles = event.second;
        // Cluster the particles into jets for the current event
        ClusterSequence cs(particles, jet_def);
        vector<PseudoJet> jets = sorted_by_pt(cs.inclusive_jets());
	
        // Output the leading and subleading jets for each event
	eventNum = eventNumber;

	pTMiss = 0;	
	double pXMiss = 0;
	double pYMiss = 0;

	// Particle Loop
	for (const auto& particle : particles) {
	    px = particle.px();
	    py = particle.py();
	    pz = particle.pz();
	    pt = particle.pt();
	    pPhi = particle.phi_std();
	    pEta = particle.eta();

	    pXMiss += px;
	    pYMiss += py;
	
	    particleTree->Fill();
	}

	pTMiss = sqrt(pXMiss * pXMiss + pYMiss * pYMiss);

	if (eventNumber % 1000 == 0){
		cout << "particle pt:" << pt << endl;
		cout << "pT Miss:" << pTMiss << endl;
        }
	
	//Trying two different ways of finding jptmissing
	fastjet::PseudoJet jPt_total(0, 0, 0, 0);
	double jPt_missing_x = 0;
	double jPt_missing_y = 0;

	// Jet Loop
	for (const auto& jet : jets) {

	    //Regular Jets
	    jPx = jet.px();
	    jPy = jet.py();
            jPz = jet.pz();
            jPt = jet.pt();
	    jE = jet.E();
	    nConstituents = jet.constituents().size();
	    jPhi = jet.phi_std();
            jEta = jet.eta();
	    
	    jPt_missing_x += jPx;
	    jPt_missing_y += jPy;
	
            jetTree->Fill();
	}

	jPtMiss = sqrt(jPt_missing_x*jPt_missing_x + jPt_missing_y*jPt_missing_y);

        if (eventNumber % 1000 == 0){
		cout << "jPtMiss:" << jPtMiss << endl;
		cout << "Jet PT:" << jPt << endl;
        }

	
	//Select Leading and Subleading Jets
        if (jets.size() >= 2) {
	    jPt1 = jets[0].pt();
	    jPhi1 = jets[0].phi_std();
	    jEta1 = jets[0].eta();
	    jE1 = jets[0].E();

	    jPt2 = jets[1].pt();

        } else if (jets.size() == 1) {
	    jPt1 = jets[0].pt();
	    jPt2 = -1;
            cout << "Only one jet found. Leading jet pT: " << jets[0].pt() << endl;

        } else {
            cout << "No jets found." << endl;
	    jPt1 = -1;
	    jPt2 = -1;
        }

	A_J = (jPt1 - jPt2)/(jPt1 + jPt2);
	eventTree->Fill();
    }

    file->Write();
    file->Close();
}

int main(int argc, char* argv[]) {

    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input.txt> <output.root>\n";
        return 1;
    }

    string inputFileName(argv[1]);
    string outputFileName(argv[2]);

    cout << "Input file: " << argv[1] << endl;
    cout << "Output ROOT file: " << argv[2] << endl;


    ParticleToJet(inputFileName, outputFileName);

    return 0;
}












