#include <iostream>
#include <vector>
#include <cmath>

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TGraph.h"

using namespace std;

double GetCFDTime(const std::vector<double>& wf, double dt, double frac)
{
    if(wf.empty()) return -9999;

    int n = wf.size();

    // baseline
    int nBase = std::min(50, n);
    double baseline = 0;

    for(int i=0;i<nBase;i++)
        baseline += wf[i];

    baseline /= nBase;

    // max
    double vmax = wf[0];
    for(int i=1;i<n;i++)
        vmax = std::max(vmax, wf[i]);

    double amp = vmax - baseline;
    if(amp <= 0) return -9999;

    double thr = baseline + frac*amp;

    // crossing
    for(int i=1;i<n;i++)
    {
        double v1 = wf[i-1];
        double v2 = wf[i];

        if(v1 < thr && v2 >= thr)
        {
            double t1 = (i-1)*dt;
            double t2 = i*dt;

            return t1 + (thr-v1)*(t2-t1)/(v2-v1);
        }
    }

    return -9999;
}

void CFD_Fraction_Scan(int channel1=8, int channel2=10)
{
    TFile *f = TFile::Open("wavecatcher_output.root");
    TTree *t = (TTree*)f->Get("ChannelTree");

    int eventID, channel;
    vector<double>* waveform = nullptr;

    t->SetBranchAddress("eventID",&eventID);
    t->SetBranchAddress("channel",&channel);
    t->SetBranchAddress("waveform",&waveform);

    const double dt = 0.3125; // ns

    vector<double> frac_list;
    vector<double> sigma_list;

    TCanvas *c1 = new TCanvas("c1","CFD Scan",900,700);

    // scan fractions
    for(double frac = 0.10; frac <= 0.60; frac += 0.05)
    {
        cout << "\n========================" << endl;
        cout << "Fraction = " << frac << endl;

        TH1D *hdt = new TH1D("hdt",Form(";#Deltat (ns) (t_{%d}-t_{%d}); Counts", channel1, channel2), 500,-10,10);

        double t8 = -9999;
        double t10 = -9999;
        int currentEvent = -1;

        Long64_t N = t->GetEntries();

        for(Long64_t i=0;i<N;i++)
        {
            t->GetEntry(i);

            if(!waveform) continue;

            if(currentEvent == -1)
                currentEvent = eventID;

            // new event → fill
            if(eventID != currentEvent)
            {
                if(t8>-9000 && t10>-9000)
                    hdt->Fill(t10 - t8);

                t8 = -9999;
                t10 = -9999;
                currentEvent = eventID;
            }

            double tcfd = GetCFDTime(*waveform, dt, frac);

            if(channel == channel1)
                t8 = tcfd;

            if(channel == channel2)
                t10 = tcfd;
        }

        // last event
        if(t8>-9000 && t10>-9000)
            hdt->Fill(t10 - t8);

        // fit
        int maxBin = hdt->GetMaximumBin();
        double peak = hdt->GetBinCenter(maxBin);

        TF1 *g = new TF1("g","gaus",peak-1,peak+1);
        hdt->Fit(g,"RQ");

        double sigma = g->GetParameter(2);

        frac_list.push_back(frac);
        sigma_list.push_back(sigma);

        cout << "Sigma = " << sigma*1000 << " ps" << endl;

        delete hdt;
        delete g;
    }

    // -----------------------------
    // plot sigma vs fraction
    // -----------------------------
    TGraph *gr = new TGraph(frac_list.size());

    double best_sigma = 1e9;
    double best_frac = 0;

    for(int i=0;i<frac_list.size();i++)
    {
        gr->SetPoint(i, frac_list[i], sigma_list[i]*1000.0);

        if(sigma_list[i] < best_sigma)
        {
            best_sigma = sigma_list[i];
            best_frac = frac_list[i];
        }
    }

    gr->SetTitle(Form(";CFD fraction;#sigma_{t} (ps) (t_{%d} - t_{%d})", channel1, channel2));
    gr->SetMarkerStyle(20);
    gr->Draw("APL");

    cout << "\n========================" << endl;
    cout << "BEST FRACTION = " << best_frac << endl;
    cout << "BEST SIGMA    = " << best_sigma*1000 << " ps" << endl;
    cout << "Single det    = " << best_sigma*1000/sqrt(2.0) << " ps" << endl;
    cout << "========================\n";
}
