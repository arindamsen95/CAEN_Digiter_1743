#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <filesystem>

void plot_charge(const char* filename="center.root")
{

//gStyle->SetOptStat(0);

gStyle->SetTitleSize(0.04,"XY");
gStyle->SetLabelSize(0.035,"XY");

gStyle->SetTitleOffset(1.0,"X");
gStyle->SetTitleOffset(1.6,"Y");

    TFile *f = TFile::Open(filename);

    TTree *t =
      (TTree*)f->Get("ChannelTree");

    int channel;
    double charge;

    t->SetBranchAddress("channel",&channel);
    t->SetBranchAddress("charge",&charge);

    TH1D *h0  = new TH1D("h0",
        "Reference Detector;Charge (pC);Counts",
        200,-100,100);

    TH1D *h9  = new TH1D("h9",
        "Left SiPM;Charge (pC);Counts",
        200,-100,100);

    TH1D *h11 = new TH1D("h11",
        "Right SiPM;Charge (pC);Counts",
        200,-100,100);

    Long64_t n=t->GetEntries();

    for(Long64_t i=0;i<n;i++)
    {
        t->GetEntry(i);

        if(channel==0)
            h0->Fill(charge);

        if(channel==9)
            h9->Fill(charge);

        if(channel==11)
            h11->Fill(charge);
    }

    TCanvas *c =
      new TCanvas("c","Charge",1200,400);

    c->Divide(3,1);

    c->cd(1);
    gPad->SetLeftMargin(0.15);
    h0->Draw();

    c->cd(2);
    gPad->SetLeftMargin(0.15);
    h9->Draw();

    c->cd(3);
    gPad->SetLeftMargin(0.15);
    h11->Draw();

    std::filesystem::path p(filename);

    std::string outname = "charge_spectra_" + p.stem().string() + ".pdf";

    c->SaveAs(outname.c_str());
}
