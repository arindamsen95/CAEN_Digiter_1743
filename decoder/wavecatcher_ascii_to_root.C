#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

#include "TFile.h"
#include "TTree.h"

using namespace std;

void wavecatcher_ascii_to_root(int begin_file=1, int end_file=1, TString outfile="wavecatcher_output.root")
{
    TFile *fout = new TFile(outfile,"RECREATE");

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
    Long64_t leadingTime;
    Long64_t trailingTime;
    char leadStr[64];
    char trailStr[64];

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

        cout << "\n*****************" << endl;

    // =========================
    // FILE LOOP
    // =========================
    for(int i=begin_file; i<=end_file; i++)
    {

     std::string fname;
     
     if(i == 0)
         fname = "wavecatcher_run1_Ascii.dat";
     else
         fname = Form("wavecatcher_run1_Ascii.dat_%04d", i);

        cout << "Processing : " << fname << endl;

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
                    "=== CH: %d EVENTID: %*d FCR: %d Baseline: %lf V Amplitude: %lf V Charge: %lf pC LeadingEdgeTime: %63s ns TrailingEdgeTime: %63s ns TrigCount: %d TimeCount %d ===",
                    &channel,
                    &fcr,
                    &baseline,
                    &amplitude,
                    &charge,
                    leadStr,
                    trailStr,
                    &trigCount,
                    &timeCount
                );

    std::string s1 = leadStr;
    std::string s2 = trailStr;
    
    s1.erase(s1.find('.'), 1);
    s2.erase(s2.find('.'), 1);
    
    leadingTime  = std::stoll(s1);
    trailingTime = std::stoll(s2);

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

    cout << "\033[1;31mDONE → \033[0m" << outfile << " created " << endl;
    cout << "*****************\n" << endl;
}
