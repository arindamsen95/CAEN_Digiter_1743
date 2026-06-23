#include <TFile.h>
#include <TTree.h>
#include <TH2D.h>
#include <TCanvas.h>

void plot_asymmetry_2D()
{
    std::vector<std::string> files = {
        "../left40_analysis.root",
        "../left20_analysis.root",
        "../center_analysis.root",
        "../right20_analysis.root",
        "../right40_analysis.root"
    };

    double pos[5] = {-40,-20,0,20,40};

    TH2D *h2 = new TH2D(
        "h2",
        ";True Position (cm);A=(Q9-Q11)/(Q9+Q11)",
        100,-50,50,
        200,-1,1);

    for(int i=0;i<5;i++)
    {
        TFile *f = TFile::Open(files[i].c_str());
        TTree *t = (TTree*)f->Get("AnalysisTree");

        double Q9,Q11;

        t->SetBranchAddress("Q9",&Q9);
        t->SetBranchAddress("Q11",&Q11);

        Long64_t n=t->GetEntries();

        for(Long64_t j=0;j<n;j++)
        {
            t->GetEntry(j);

            double A=(Q9-Q11)/(Q9+Q11);

            h2->Fill(pos[i],A);
        }

        f->Close();
    }

    TCanvas *c=new TCanvas("c","c",900,700);

    h2->Draw("COLZ");
    
    TProfile *p = h2->ProfileX();

    p->SetMarkerStyle(20);
    p->Draw("same");

    //c->SaveAs("asymmetry_2D.pdf");
}
