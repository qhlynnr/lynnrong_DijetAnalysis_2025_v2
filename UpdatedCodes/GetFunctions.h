#ifndef GETFUNCTIONS_H
#define GETFUNCTIONS_H

#include <iostream>
#include <vector>
#include <tuple>
#include <string>
#include <TFile.h>
#include <TTree.h>
#include <TMath.h>

using namespace std;

// Function to return a TTree from a file
TTree* ReturnTree(const char* inputFileName, string treeName);

// Function to calculate dPhi values from a tree
vector<float> GetDPhi(const char* inFileName, string inTreeName, const char* branchName = "jtphi");

// Function to calculate pt1, pt2, and A_J values from a tree
tuple<vector<float>, vector<float>, vector<float>> GetPt1Pt2(TTree* JetAnalyserTree);

// Function to calculate A_J values from a file
vector<float> ReturnAJ(const char* inFileName);

#endif // GETFUNCTIONS_H