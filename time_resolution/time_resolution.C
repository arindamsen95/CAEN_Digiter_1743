void time_resolution(int channel1 = 0, int channel2 = 8)
 {
       TFile *f = TFile::Open("wavecatcher_output.root");
       if (!f || f->IsZombie()) {
           std::cout << "Error opening file\n";
           return;
       }
  
  TTree *t = (TTree*)gFile->Get("ChannelTree");
  
  Long64_t leadingTime;
  Int_t eventID, channel;
  
  t->SetBranchAddress("eventID",&eventID);
  t->SetBranchAddress("channel",&channel);
  t->SetBranchAddress("leadingTime",&leadingTime);
  
  std::map<int, Long64_t> t0;
  std::map<int, Long64_t> t8;
  
  Long64_t n = t->GetEntries();
  
  for(Long64_t i=0;i<n;i++) {
      t->GetEntry(i);
  
      if(channel==channel1) t0[eventID] = leadingTime;
      if(channel==channel2) t8[eventID] = leadingTime;
  }
  
  TH1D *hdt = new TH1D("hdt",Form("t_{%d}-t_{%d};#Delta t;Counts",channel1,channel2),500,-10000,10000);
  
  for(auto &it : t0) {
      int evt = it.first;
  
      if(t8.count(evt))
          hdt->Fill(t8[evt] - t0[evt]);
  }
  
  hdt->Draw();

 double peak = hdt->GetBinCenter(hdt->GetMaximumBin());

TF1 *g = new TF1("g","gaus",peak-2000,peak+2000);
hdt->Fit(g,"RQ");

double mean  = g->GetParameter(1);
double sigma = g->GetParameter(2);

// Refit around ±3σ
g->SetRange(mean-3*sigma, mean+3*sigma);
hdt->Fit(g,"RQ");

mean  = g->GetParameter(1);
sigma = g->GetParameter(2);

std::cout << "Mean  = " << mean  << " ps" << std::endl;
std::cout << "Sigma = " << sigma << " ps" << std::endl;
std::cout << "Single detector resolution = "
          << sigma/std::sqrt(2.0)
          << " ps" << std::endl;

 }
