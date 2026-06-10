 #include <vector>
 #include <algorithm>
 #include "TFile.h"
 #include "TTree.h"
 #include "TCanvas.h"
 #include "TGraph.h"
 #include "TLegend.h"
 #include <iostream>
 #include <cmath>
 #include <iomanip>
 
 void fit_waveform(TString outfile = "wavecatcher_output.root", int event_id = 501, int channel_id = 0)
 {
     TFile *f = TFile::Open(outfile);
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
     
     double storedBaseline, storedAmplitude, storedCharge;

     t->SetBranchAddress("baseline",&storedBaseline);
     t->SetBranchAddress("amplitude",&storedAmplitude);
     t->SetBranchAddress("charge",&storedCharge);
 
     Long64_t nentries = t->GetEntriesFast();
 
     bool found = false;
 
     for (Long64_t i = 0; i < nentries; i++) {

         t->GetEntry(i);
 
         if (eventID == event_id && channel == channel_id) {

             found = true;

    if (!waveform || waveform->empty()) {
        std::cout << "Empty waveform\n";
        break;
    }

 double base = 0.0;
 double minVal = (*waveform)[0];
 double maxVal = (*waveform)[0];
 double charge = 0.0;
 
 int n = waveform->size();
 int nBase = std::min(50, n);
 double dt = 0.3125;  // in ns
 double R = 50.0; // ohms (typical oscilloscope / readout impedance)
 
 
 for (int j = 0; j < n; ++j) {

     double v = (*waveform)[j];
 
     if (j < nBase)
         base += v;
 
     if (v < minVal) minVal = v;
     if (v > maxVal) maxVal = v;
 
     charge += v * dt;
 }
 
 base /= nBase;
 
 charge = (charge - base * n * dt) / R;  // in nanoCoulombs (nC)
 
 //std::cout<< "max= " << maxVal << "  " << "min= " << minVal << std::endl;
 
 double amplitude = (std::abs(minVal - base) > std::abs(maxVal - base))
        ? (minVal - base)
        : (maxVal - base);

 std::cout << "==================================\n" << std::endl;
 std::cout << left << setw(20) << "Channel ID" << "= " << channel_id << std::endl;
 std::cout << left << setw(20) << "Event ID" << "= " << event_id << std::endl;

 std::cout << "\n";
std::cout << std::left
          << std::setw(20) << "Parameters"
          << std::setw(15) << "Stored"
          << std::setw(15) << "Recalc"
          << std::setw(15) << "Difference"
          << "\n";

std::cout << std::string(65,'-') << "\n";

std::cout << std::setw(20) << "Baseline (V)"
          << std::setw(15) << storedBaseline
          << std::setw(15) << base
          << std::setw(15) << Form("%0.8f",(base-storedBaseline))
          << "\n";

std::cout << std::setw(20) << "Amplitude (V)"
          << std::setw(15) << storedAmplitude
          << std::setw(15) << amplitude
          << std::setw(15) << (amplitude-storedAmplitude)
          << "\n";

std::cout << std::setw(20) << "Charge (pC)"
          << std::setw(15) << storedCharge
          << std::setw(15) << charge*1000.
          << std::setw(15) << Form("%0.8f",(charge*1000.-storedCharge))
          << "\n";
          
 std::cout << "\n==================================\n" << std::endl;
 
  TGraph *gr = new TGraph(n);
  for(int j=0;j<n;++j)
     gr->SetPoint(j, j*dt, (*waveform)[j]);
 
  TCanvas *c1 = new TCanvas("c1","Waveform",800,600);
  gr->SetTitle(";Time (ns);ADC counts");
  gr->SetLineWidth(2);
  gr->Draw("AL");
 
  //TLegend *leg = new TLegend(0.65,0.2,0.85,0.3,"","brNDC");
  //leg->AddEntry(gr, "Waveform", "l");
  //leg->AddEntry((TObject*)nullptr, Form("Amplitude : %f Volt", amplitude), "");
  //leg->AddEntry((TObject*)nullptr, Form("Charge : %f pC", charge*1000), "");
  //leg->Draw();
 
  TPaveText *pave = new TPaveText(0.65, 0.2, 0.85, 0.3, "brNDC");
  pave->SetFillColor(0);  // White background
  pave->SetTextAlign(22); // Left-aligned (1) and vertically centered (2)
  pave->AddText(Form ("Waveform (Ch:%d , Event:%d)", channel_id, event_id));
  //pave->AddLine(0.0, 0.0, 0.0, 0.0);
  pave->AddLine(0.0, 0.72, 1.0, 0.72);
  pave->AddText(Form("Baseline : %0.3f mV", base * 1000)); 
  pave->AddText(Form("Amplitude : %0.3f mV", amplitude * 1000));
  pave->AddText(Form("Charge : %0.2f pC", charge * 1000));
  pave->Draw();
  //c1->Update();

  //delete pave;
  //delete gr;
  //delete c1; 
             break;
         }
 
     }
 
     if (!found)
         std::cout << "Event not found!\n";
         f->Close();
         delete f;
 }
