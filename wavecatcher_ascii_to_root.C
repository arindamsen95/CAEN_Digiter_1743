#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

#include "TFile.h"
#include "TTree.h"

using namespace std;

void wavecatcher_ascii_to_root(int begin_file=1, int end_file=1)
{
    TFile *fout = new TFile("wavecatcher_split.root","RECREATE");

    // =========================
    // 1. EVENT TREE
    // =========================
    TTree *eventTree = new TTree("EventTree","Event level info");

    int eventID;
    double unixTime;
    long long tdc;

    eventTree->Branch("eventID",&eventID);
    eventTree->Branch("unixTime",&unixTime);
    eventTree->Branch("tdc",&tdc);

    // =========================
    // 2. CHANNEL TREE
    // =========================
    TTree *chTree = new TTree("ChannelTree","Channel level info");

    int ch_eventID;
    int channel;
    int fcr;
    int trigCount;
    int timeCount;

    double baseline;
    double amplitude;
    double charge;
    double leadingTime;
    double trailingTime;

    vector<double> *waveform = new vector<double>();

    chTree->Branch("eventID",&ch_eventID);
    chTree->Branch("channel",&channel);
    chTree->Branch("fcr",&fcr);
    chTree->Branch("trigCount",&trigCount);
    chTree->Branch("timeCount",&timeCount);

    chTree->Branch("baseline",&baseline);
    chTree->Branch("amplitude",&amplitude);
    chTree->Branch("charge",&charge);
    chTree->Branch("leadingTime",&leadingTime);
    chTree->Branch("trailingTime",&trailingTime);

    chTree->Branch("waveform",&waveform);

    // =========================
    // FILE LOOP
    // =========================
    for(int i=begin_file;i<=end_file;i++)
    {
        char fname[256];
        sprintf(fname,"wavecatcher_run1_Ascii.dat_%04d", i);

        cout << "Processing " << fname << endl;

        ifstream fin(fname);
        if(!fin.is_open()){
            cout << "Cannot open " << fname << endl;
            continue;
        }

        string line;

        while(getline(fin,line))
        {
            // =====================
            // EVENT HEADER
            // =====================
            if(line.find("=== EVENT") != string::npos)
            {
                sscanf(line.c_str(),"=== EVENT %d ===",&eventID);

                // fill event tree ONCE per event
                eventTree->Fill();
            }

            if(line.find("UnixTime") != string::npos)
            {
                sscanf(line.c_str(),"=== UnixTime = %lf",&unixTime);
            }

            if(line.find("TDC From FPGA") != string::npos)
            {
                sscanf(line.c_str(),
                    "=== UnixTime = %*lf date = %*s time = %*s == TDC From FPGA = %lld",
                    &tdc
                );
            }

            // =====================
            // CHANNEL BLOCK
            // =====================
            if(line.find("=== CH:") != string::npos)
            {
                waveform->clear();

                ch_eventID = eventID;

                sscanf(line.c_str(),
                    "=== CH: %d EVENTID: %*d FCR: %d Baseline: %lf V Amplitude: %lf V Charge: %lf pC LeadingEdgeTime: %lf ns TrailingEdgeTime: %lf ns TrigCount: %d TimeCount %d ===",
                    &channel,
                    &fcr,
                    &baseline,
                    &amplitude,
                    &charge,
                    &leadingTime,
                    &trailingTime,
                    &trigCount,
                    &timeCount
                );

                // waveform line
                if(getline(fin,line))
                {
                    stringstream ss(line);
                    double val;
                    while(ss >> val)
                        waveform->push_back(val);
                }

                chTree->Fill();
            }
        }

        fin.close();
    }

    fout->Write();
    fout->Close();

    cout << "DONE → wavecatcher_split.root created" << endl;
}
