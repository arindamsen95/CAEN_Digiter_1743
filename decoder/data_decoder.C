 #include <iostream>
 #include <fstream>
 #include <vector>
 #include <string>
 #include <sstream>
 #include "TFile.h"
 #include "TTree.h"
 #include <vector>
 #include <algorithm>
 #include <filesystem>
 #define WARNING "\033[1;31mWARNING : \033[0m"

  using namespace std;

  void data_decoder(TString outfile="wavecatcher_output.root", std::vector<std::string> directories={"."}) {

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
  
  Long64_t eventOffset = 0;
  bool firstEvent = true;
  std::vector<std::pair<std::string,int>> fileCounts;

  for(const auto& dirname : directories) {

    namespace fs = std::filesystem;
    if(!fs::exists(dirname)) {
        std::cout << WARNING << "Directory does not exist: " << dirname << std::endl;
        continue;
    }

    std::vector<std::string> files;

    for(const auto& entry : fs::directory_iterator(dirname)) {

       if(!entry.is_regular_file()) continue;

       std::string fname = entry.path().filename().string();

       if(fname.find("wavecatcher_run") == 0 && fname.find("Ascii.dat") != std::string::npos)
         files.push_back(entry.path().string());
    }
    
    fileCounts.push_back({dirname, (int)files.size()});

    std::sort(files.begin(), files.end());

    int maxEventInThisDir = -1;

    for(const auto& fname : files) {
    
        std::cout << "Processing : " << fname << std::endl;

        std::ifstream fin(fname);

        if(!fin.is_open()) {
            std::cout << WARNING << "Cannot open " << fname << std::endl;
            continue;
        }

        string line;

    while(getline(fin,line)) {
  
    // =====================================
    // EVENT HEADER
    // =====================================

    if(line.find("=== EVENT") != string::npos) {
    
        if(!firstEvent) eventTree->Fill();

        firstEvent = false;

        int localEventID;

        sscanf(line.c_str(), "=== EVENT %d ===", &localEventID);

        if(localEventID > maxEventInThisDir)
            maxEventInThisDir = localEventID;

        eventID = localEventID + eventOffset;

        continue;
    }

    // =====================================
    // UNIX TIME
    // =====================================

    if(line.find("UnixTime") != string::npos) {
    
        sscanf(line.c_str(), "=== UnixTime = %lf", &unixTime);
    }

    // =====================================
    // FPGA TDC
    // =====================================

    if(line.find("TDC From FPGA") != string::npos) {
    
        sscanf(line.c_str(),
               "=== UnixTime = %*lf date = %*s time = %*s == TDC From FPGA = %lld", &tdc);
    }

    // =====================================
    // CHANNEL BLOCK
    // =====================================

    if(line.find("=== CH:") != string::npos) {
    
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
            &timeCount);

        std::string s1 = leadStr;
        std::string s2 = trailStr;

        s1.erase(s1.find('.'),1);
        s2.erase(s2.find('.'),1);

        leadingTime  = std::stoll(s1);
        trailingTime = std::stoll(s2);

        if(getline(fin,line)) {
        
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

    // next directory starts after last event
    eventOffset += (maxEventInThisDir + 1);
 }
    if(!firstEvent) eventTree->Fill();
    fout->Write();
    fout->Close();

    // =====================================
    // RUN SUMMERY
    // =====================================

    cout << "\n========== \033[1;31m Run Summary \033[0m ==========\n";

    std::cout << "Number of directories : " << directories.size() << std::endl;
    int totalFiles = 0;

    for(const auto& p : fileCounts){
    cout << p.first << " : " << p.second << " files" << endl;
    totalFiles += p.second;
    }

    std::cout << "----------------------------------" << std::endl;
    std::cout << "Total files : " << totalFiles << endl;
    std::cout << "Output file : " << outfile << " " ;

    namespace fs2 = std::filesystem;
    
    auto filesize = fs2::file_size(outfile.Data());
    
    if(filesize < 1024)
        std::cout << filesize << " B " << std::endl;
    
    else if(filesize < 1024*1024)
        std::cout << Form("(%.2f KB)",
                     filesize/1024.0) << std::endl;
    
    else if(filesize < 1024ll*1024ll*1024ll)
        std::cout << Form("(%.2f MB)",
                     filesize/(1024.0*1024.0)) << std::endl;
    
    else
        std::cout << Form("(%.2f GB)",
                 filesize/(1024.0*1024.0*1024.0)) << std::endl;
    std::cout << "==================================\n" << std::endl;

}
