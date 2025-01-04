#pragma once 

#include "daisy.h"
using namespace daisy;

#define TEMPO_LIST_SIZE 5

class TempoFinder {
public:
    void tickQuarter() {
        uint32_t now = System::GetNow();
        uint32_t delta = now - lastTime;
        lastTime = now;

        double quarterTime = ((double)delta / 1000.);
        double newTempo = 60.0/quarterTime;

        tempoList.push_back(newTempo);

        double finalTempo = 0;
        for (auto tempo : tempoList) {
            finalTempo += tempo;
        }
        finalTempo /= tempoList.size();
        tempo = finalTempo;

        if (tempoList.size() > TEMPO_LIST_SIZE) {
            tempoList.erase(tempoList.begin());
        }
    }

    double getCurrentTempo() {
        return tempo;
    } 

private:
    uint32_t lastTime = System::GetNow();   
    std::vector<double> tempoList;
    double tempo = 120.0;
};
