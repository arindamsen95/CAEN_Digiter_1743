#include <iostream>
#include <vector>
#include <cmath>
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TF1.h"
#include "TCanvas.h"

double GetCFDTime(const std::vector<double>& wf, double dt, double frac)
{
    if(wf.empty()) return -9999;

    int n = wf.size();

    //------------------------------------------------
    // baseline from first 50 samples
    //------------------------------------------------

    int nBase = std::min(50,n);

    double baseline = 0;

    for(int i=0;i<nBase;i++)
        baseline += wf[i];

    baseline /= nBase;

    //------------------------------------------------
    // pulse maximum
    //------------------------------------------------

    double vmax = wf[0];

    for(int i=1;i<n;i++)
        if(wf[i] > vmax)
            vmax = wf[i];

    double amp = vmax - baseline;

    if(amp <= 0)
        return -9999;

    //------------------------------------------------
    // CFD threshold
    //------------------------------------------------

    double threshold = baseline + frac*amp;

    //------------------------------------------------
    // threshold crossing
    //------------------------------------------------

    for(int i=1;i<n;i++)
    {
        double v1 = wf[i-1];
        double v2 = wf[i];

        if(v1 < threshold && v2 >= threshold)
        {
            double t1 = (i-1)*dt;
            double t2 = i*dt;

            return t1 + (threshold-v1)*(t2-t1)/(v2-v1);
        }
    }

    return -9999;
}

void CFD_TimeResolution(double frac = 0.4, int channel1=9, int channel2=11)
{
    double dt = 0.3125; // in ns
    TFile *f = TFile::Open("wavecatcher_output.root");

    TTree *t = (TTree*)f->Get("ChannelTree");

    int eventID;
    int channel;

    std::vector<double> *waveform = nullptr;

    t->SetBranchAddress("eventID",&eventID);
    t->SetBranchAddress("channel",&channel);
    t->SetBranchAddress("waveform",&waveform);

    //------------------------------------------------
    // histogram
    //------------------------------------------------
    TH1D *hdt = new TH1D("hdt",Form(";#Deltat (ns) (t_{%d}-t_{%d});Counts", channel1, channel2), 1000, -10, 10);

    Long64_t nentries = t->GetEntries();

    double t8 = -9999;
    double t10 = -9999;

    int currentEvent = -1;

    for(Long64_t i=0;i<nentries;i++)
    {
        t->GetEntry(i);

        if(currentEvent==-1)
            currentEvent = eventID;

        //------------------------------------------------
        // new event
        //------------------------------------------------

        if(eventID != currentEvent)
        {
            if(t8>-9000 && t10>-9000)
                hdt->Fill(t10-t8);

            t8 = -9999;
            t10 = -9999;

            currentEvent = eventID;
        }

        //------------------------------------------------
        // channel 8
        //------------------------------------------------
        if(channel==channel1)
            t8 = GetCFDTime(*waveform,dt,frac);

        //------------------------------------------------
        // channel 10
        //------------------------------------------------
        if(channel==channel2)
            t10 = GetCFDTime(*waveform,dt,frac);
    }

    //------------------------------------------------
    // last event
    //------------------------------------------------
    if(t8>-9000 && t10>-9000)
        hdt->Fill(t10-t8);

    //------------------------------------------------
    // draw
    //------------------------------------------------
    TCanvas *c1 = new TCanvas("c1", "Time Resolution", 900, 700);

    hdt->Draw();

    //------------------------------------------------
    // peak search
    //------------------------------------------------
    int maxBin = hdt->GetMaximumBin();

    double peak = hdt->GetBinCenter(maxBin);

    //------------------------------------------------
    // gaussian fit
    //------------------------------------------------
    TF1 *g = new TF1("g","gaus", peak-1.0, peak+1.0);

    hdt->Fit(g,"RQ");

    double sigma = g->GetParameter(2);

    double sigma_ps = sigma*1000.0;
    
    TLatex latex;
    latex.SetNDC();
    latex.SetTextSize(0.03);
    latex.DrawLatex(0.15, 0.85, Form("CFD fraction = %.2f", frac));
    latex.DrawLatex(0.15, 0.82, Form("Time Resolution = %.3lf ps", sigma_ps));

    std::cout << "\n========================\n";
    std::cout << "Sigma = " << sigma << " ns\n";
    std::cout << "Time Resolution = " << sigma_ps << " ps\n";
    std::cout << "========================\n\n";
}
