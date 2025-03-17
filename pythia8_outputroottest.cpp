#include "Pythia8/Pythia.h"
#include "TFile.h"
#include "TTree.h"
#include <fstream>
#include <cmath>

using namespace Pythia8;
using namespace std;

int main() {

    // Initialize Pythia
    Pythia pythia;
    
    double pTHatMinValue = 400; 
    double sqrts = 5200; 
    int nEvents = 100000;  // Number of events to generate

    // Beam parameters: proton-proton collisions at 5.36 TeV
    pythia.readString("Beams:idA = 2212"); // Proton
    pythia.readString("Beams:idB = 2212"); // Proton
    pythia.readString("Beams:eCM = " + to_string(sqrts)); 
    pythia.readString("PhaseSpace:pTHatMin = " + to_string(pTHatMinValue));
    pythia.readString("HardQCD:all = on");
    pythia.readString("Random:setSeed = off");

    // Set minimum pT hat for hard scattering
    
    // Initialize Pythia
    pythia.init();

    // Open output file for writing particle information
    string outFileName = "pythia8Jets_pp_";
    outFileName += to_string(static_cast<int>(round(sqrts)));  
    outFileName += "GeV_";
    outFileName += to_string(static_cast<int>(round(pTHatMinValue)));
    outFileName += "pTHat_";
    //outFileName += to_string(nEvents);
    outFileName += "100K";
    outFileName += "Events";
    outFileName += ".root";

    //cout << "Center-of-Mass Energy (eCM): " << pythia.settings.parm("Beams:eCM") << " GeV" << endl;
    //cout << "pTHatMin: " << pythia.settings.parm("PhaseSpace:pTHatMin") << " GeV" << endl;

    //NEW try setting up root
    TFile *file = new TFile(outFileName.c_str(), "RECREATE");
    TTree *tree = new TTree("eventsTree", "Pythia8 Event Data");
    
    int eventNum, pdgId;
    double px,py,pz,E;
	

    tree->Branch("eventNum",&eventNum,"eventNum/I");
    tree->Branch("pdgId",&pdgId,"pdgId/I");

    tree->Branch("px",&px,"px/D");
    tree->Branch("py",&py,"py/D");
    tree->Branch("pz",&pz,"pz/D");
    tree->Branch("E",&E,"E/D");

    // Loop over events
    for (int iEvent = 0; iEvent < nEvents; ++iEvent) {
	eventNum = iEvent;
        if (!pythia.next()) continue;  // Generate next event
        // Loop over all particles in the event
        for (int i = 0; i < pythia.event.size(); ++i) {
            // Only consider final state particles
            if (pythia.event[i].isFinal()) {
		pdgId = pythia.event[i].id();
		px = pythia.event[i].px();
                py = pythia.event[i].py();
                pz = pythia.event[i].pz();
                E = pythia.event[i].e();

                // Print to console (for debugging/monitoring)
                //printf("%d %d %d  %lf %lf %lf %lf\n",
                  //    iEvent, i, pythia.event[i].id(),
                    //  pythia.event[i].px(), pythia.event[i].py(),
                      //pythia.event[i].pz(), pythia.event[i].e());
           
	        tree->Fill();
            }
        }
    }
    tree->Write();
    file->Close();

    // Done
    pythia.stat();
    return 0;
}
