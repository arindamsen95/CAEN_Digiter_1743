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
     TFile *f = TFile::Open("wavecatcher_output.root");
     if (!f || f->IsZombie()) {
         std::cout << "Error opening file\n";
         return;
     }
 
     TTree *t = (TTree*)f->Get("ChannelTree");
 
     int eventID, channel;
     float baseline, amp=0.0, amplitude, charge, minVal, maxVal;
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
 
 
     if (!waveform->empty()) {
        auto result = std::minmax_element(waveform->begin(), waveform->end());
 
        minVal = *result.first;
        maxVal = *result.second;
 
        cout << "min val=" << minVal << " & " << "max val=" << maxVal << endl;
       }
 
      if(abs(minVal)>maxVal) {
             //double amp = 1e9;
             for (int j = 0; j < n; j++)
             {
                 x[j] = j * 0.3125;
                 y[j] = (*waveform)[j];
 
                 // amplitude (for negative pulse)
                 if (y[j] < amp)
                     amp = y[j];
 
             }
 }
 
      else {
             //double amp = 1e-9;
             for (int j = 0; j < n; j++)
             {
                 x[j] = j * 0.3125;
                 y[j] = (*waveform)[j];
 
                 // amplitude (for negative pulse)
                 if (y[j] > amp)
                     amp = y[j];
 
             }
 }
 	
 	 amplitude = amp - base;
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
             std::cout << "Amplitude (recalc): " << amplitude << std::endl;
             std::cout << "Charge (recalc)   : " << charge * 1000. << std::endl;
 
             TGraph *gr = new TGraph(n, &x[0], &y[0]);
 
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
 }
