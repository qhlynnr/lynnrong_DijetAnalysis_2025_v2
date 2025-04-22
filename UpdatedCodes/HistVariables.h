#ifndef HISTVARIABLES_H
#define HISTVARIABLES_H
#include <TMath.h>
#include <iostream>
#include <string>
#include <vector>


struct HistVar1D{
	const char* canvasName = "CanvasName";
	const char* histTitle = "Hist Title";
    const char* outFileName = "Plots/Unknown.png";
	const char* xLabel = "xLabel";
    const char* yLabel = "yLabel";
    int nbin = 50;
    float xmin = 0;
    float xmax = TMath::Pi()+0.0001;
    float ymin = 0;
    float ymax = 100;
    bool isLog = false;
};

#endif