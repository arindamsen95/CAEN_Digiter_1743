#include <vector>
#include <algorithm>
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TLegend.h"
#include <iostream>

void fit_waveform(int event_id = 501, int channel_id = 0)
{
    TFile *f = TFile::Open("wavecatcher_split.root");
    if (!f || f->IsZombie()) {
        std::cout << "Error opening file\n";
        return;
    }

    TTree *t = (TTree*)f->Get("ChannelTree");

    int eventID, channel;
    float baseline, amplitude, charge;
    std::vector<float> *waveform = nullptr;

    t->SetBranchAddress("eventID", &eventID);
    t->SetBranchAddress("channel", &channel);
//    t->SetBranchAddress("baseline", &baseline);
//    t->SetBranchAddress("amplitude", &amplitude);
//    t->SetBranchAddress("charge", &charge);
    t->SetBranchAddress("waveform", &waveform);

    int nentries = t->GetEntries();

    bool found = false;

    std::vector<double> x, y;

    for (int i = 0; i < nentries; i++)
    {
        t->GetEntry(i);

        if (eventID == event_id && channel == channel_id)
        {
            found = true;

            int n = waveform->size();

            x.resize(n);
            y.resize(n);

            double base = 0;
            for (int j = 0; j < 50; j++)
                base += (*waveform)[j];
            base /= 50;

            double amp = 1e9;

            for (int j = 0; j < n; j++)
            {
                x[j] = j * 0.3125;
                y[j] = (*waveform)[j];

                // amplitude (for negative pulse)
                if (y[j] < amp)
                    amp = y[j];

            }


         double dt = 0.3125; // in ns (set properly!)
         double R = 50.0; // ohms (typical oscilloscope / readout impedance)
         
         double charge = 0;
         
         for (int j = 0; j < n; j++)
         {
             double v = (*waveform)[j] - base;
             charge += v * dt;
         }
         
         charge = charge / R;

            std::cout << "Event   : " << event_id << std::endl;
            std::cout << "Channel : " << channel_id << std::endl;
            std::cout << "Baseline: " << base << std::endl;
            std::cout << "Amplitude (recalc): " << amp - base << std::endl;
            std::cout << "Charge (recalc)   : " << charge * 1000. << std::endl;

            TGraph *gr = new TGraph(n, &x[0], &y[0]);

            TCanvas *c1 = new TCanvas("c1","Waveform",800,600);
            gr->SetTitle("Waveform;Time (ns);ADC counts");
            gr->SetLineWidth(2);
            gr->Draw("AL");

            TLegend *leg = new TLegend(0.6,0.7,0.88,0.88);
            leg->AddEntry(gr, "Waveform", "l");
            leg->Draw();

            break;
        }
    }

    if (!found)
        std::cout << "Event not found!\n";
}
