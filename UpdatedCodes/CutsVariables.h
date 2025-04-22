#ifndef CUTSVARIABLES_H
#define CUTSVARIABLES_H
#include <vector>
#include <string>
#include <iostream>

struct CutsVector{
    string name;
	vector<float> cutVector;
	float cutmin;
	float cutmax;
};

struct CutsBranches{
    string treename;
    string name;
    float cutmin;
    float cutmax;
};

#endif

