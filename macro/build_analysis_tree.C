#include <map>
#include <filesystem>

struct EventData
{
    double Q0=0,Q9=0,Q11=0;

    double A0=0,A9=0,A11=0;

    Long64_t T0=0,T9=0,T11=0;

    bool has0=false;
    bool has9=false;
    bool has11=false;
};

void build_analysis_tree(const char* infile="../center.root")
{   
    TFile fin(infile);
    
    if(fin.IsZombie()) {
    std::cerr << "Cannot create file\n";
    return;
    }    

    TTree *ch = (TTree*)fin.Get("ChannelTree");

    int eventID;
    int channel;

    double charge;
    double amplitude;

    Long64_t leadingTime;

    ch->SetBranchAddress("eventID",&eventID);
    ch->SetBranchAddress("channel",&channel);

    ch->SetBranchAddress("charge",&charge);
    ch->SetBranchAddress("amplitude",&amplitude);

    ch->SetBranchAddress(
        "leadingTime",&leadingTime);

    std::map<int,EventData> events;

    Long64_t n=ch->GetEntries();

    for(Long64_t i=0;i<n;i++)
    {
        ch->GetEntry(i);

        auto &ev=events[eventID];

        if(channel==0)
        {
            ev.Q0=charge;
            ev.A0=amplitude;
            ev.T0=leadingTime;
            ev.has0=true;
        }

        if(channel==9)
        {
            ev.Q9=charge;
            ev.A9=amplitude;
            ev.T9=leadingTime;
            ev.has9=true;
        }

        if(channel==11)
        {
            ev.Q11=charge;
            ev.A11=amplitude;
            ev.T11=leadingTime;
            ev.has11=true;
        }
    }

    std::filesystem::path p(infile);
    std::string outname = p.stem().string() + "_analysis.root" ;

    TFile fout(outname.c_str(), "RECREATE");
    
    if(fout.IsZombie()) {
    std::cerr << "Cannot create file\n";
    return;
    }

    TTree out("AnalysisTree", "Merged event tree");

    double Q0,Q9,Q11;
    double A0,A9,A11;

    Long64_t T0,T9,T11;

    double Qsum;
    double Qratio;

    double dtLR;
    double dtLRef;
    double dtRRef;

    out.Branch("Q0",&Q0);
    out.Branch("Q9",&Q9);
    out.Branch("Q11",&Q11);

    out.Branch("A0",&A0);
    out.Branch("A9",&A9);
    out.Branch("A11",&A11);

    out.Branch("T0",&T0);
    out.Branch("T9",&T9);
    out.Branch("T11",&T11);

    out.Branch("Qsum",&Qsum);

    out.Branch("Qratio",&Qratio);

    out.Branch("dtLR",&dtLR);

    out.Branch("dtLRef",&dtLRef);

    out.Branch("dtRRef",&dtRRef);

    for(auto &x : events)
    {
        auto &ev=x.second;

        if(!(ev.has0 &&
             ev.has9 &&
             ev.has11))
            continue;

        Q0=ev.Q0;
        Q9=ev.Q9;
        Q11=ev.Q11;

        A0=ev.A0;
        A9=ev.A9;
        A11=ev.A11;

        T0=ev.T0;
        T9=ev.T9;
        T11=ev.T11;

        Qsum=Q9+Q11;

        Qratio=log(Q9/Q11);

        dtLR=T9-T11;

        dtLRef=T9-T0;

        dtRRef=T11-T0;

        out.Fill();
    }

    out.Write();
    fout.Close();
}
