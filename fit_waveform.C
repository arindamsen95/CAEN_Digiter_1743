 #include <vector>
 #include <algorithm>
 #include "TFile.h"
 #include "TTree.h"
 #include "TCanvas.h"
 #include "TGraph.h"
 #include "TLegend.h"
 #include <iostream>
 #include <cmath>
 
 void fit_waveform(int event_id = 501, int channel_id = 0)
 {
     TFile *f = TFile::Open("wavecatcher_output.root");
     if (!f || f->IsZombie()) {
         std::cout << "Error opening file\n";
         return;
     }
 
     TTree *t = (TTree*)f->Get("ChannelTree");
 
     int eventID, channel;
     std::vector<double> *waveform = nullptr;
 
     t->SetBranchAddress("eventID", &eventID);
     t->SetBranchAddress("channel", &channel);
     t->SetBranchAddress("waveform", &waveform);
 
     Long64_t nentries = t->GetEntriesFast();
 
     bool found = false;
 
     for (Long64_t i = 0; i < nentries; i++)
     {
         t->GetEntry(i);
 
         if (eventID == event_id && channel == channel_id)
         {
             found = true;

    if (!waveform || waveform->empty())
    {
        std::cout << "Empty waveform\n";
        break;
    }

    int n = waveform->size();

    double amp = (*waveform)[0];

    int nBase = std::min(50, n);

    double base = 0.0;
    for (int j = 0; j < nBase; ++j)
        base += (*waveform)[j];

    base /= nBase;
 
 
        auto result = std::minmax_element(waveform->begin(), waveform->end());
 
        double minVal = *result.first;
        double maxVal = *result.second;
        amp = (std::fabs(minVal) > std::fabs(maxVal)) ? minVal : maxVal;
 
        cout << "min val=" << minVal << " & " << "max val=" << maxVal << endl;
 
          double dt = 0.3125; // in ns (set properly!)
          double R = 50.0; // ohms (typical oscilloscope / readout impedance)
          double charge = 0;
          double amplitude = 0;

       for(int j=0;j<n;++j)
          {
             double v = (*waveform)[j];

             charge += (v-base)*dt;
          }
 	
 	 amplitude = amp - base;
          
          charge = charge / R;
 
             std::cout << "Channel : " << channel_id << " && " "Event : " << event_id << std::endl;
             std::cout << "Baseline: " << base << std::endl;
             std::cout << "Amplitude (recalc): " << amplitude << std::endl;
             std::cout << "Charge (recalc)   : " << charge * 1000. << std::endl;
 
             TGraph *gr = new TGraph(n);
             for(int j=0;j<n;++j)
                gr->SetPoint(j, j*dt, (*waveform)[j]);
 
             TCanvas *c1 = new TCanvas("c1","Waveform",800,600);
             gr->SetTitle(";Time (ns);ADC counts");
             gr->SetLineWidth(2);
             gr->Draw("AL");
 
 //            TLegend *leg = new TLegend(0.65,0.2,0.85,0.3,"","brNDC");
 //            leg->AddEntry(gr, "Waveform", "l");
 //            leg->AddEntry((TObject*)nullptr, Form("Amplitude : %f Volt", amplitude), "");
 //            leg->AddEntry((TObject*)nullptr, Form("Charge : %f pC", charge*1000), "");
 //            leg->Draw();
 
     TPaveText *pave = new TPaveText(0.65, 0.2, 0.85, 0.3, "brNDC");
     pave->SetFillColor(0);  // White background
     pave->SetTextAlign(22); // Left-aligned (1) and vertically centered (2)
     pave->AddText("Waveform");
    //pave->AddLine(0.0, 0.0, 0.0, 0.0);
     pave->AddLine(0.0, 0.72, 1.0, 0.72);
     pave->AddText(Form("Baseline : %0.3f mV", base * 1000)); 
     pave->AddText(Form("Amplitude : %0.3f mV", amplitude * 1000));
     pave->AddText(Form("Charge : %0.2f pC", charge * 1000));
     pave->Draw();
 
             break;
         }
 
     }
 
     if (!found)
         std::cout << "Event not found!\n";
         f->Close();
         delete f;
 }
