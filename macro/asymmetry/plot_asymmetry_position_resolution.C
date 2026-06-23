#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TLegend.h>
#include <TStyle.h>
#include <iostream>
#include <vector>

void plot_asymmetry_position_resolution()
{
    gStyle->SetOptStat(0);
    gStyle->SetOptFit(1111);

    const double slopeA = -0.015994;

    std::vector<std::string> files = {
        "../left40_analysis.root",
        "../left20_analysis.root",
        "../center_analysis.root",
        "../right20_analysis.root",
        "../right40_analysis.root"
    };

    double pos[5] = {-40,-20,0,20,40};

    double res[5];
    double eres[5];

    TCanvas *cfit =
        new TCanvas("cfit",
                    "Asymmetry Fits",
                    1600,
                    900);

    cfit->Divide(3,2);

    for(int i=0;i<5;i++)
    {
        TFile *f =
            TFile::Open(files[i].c_str());

        if(!f || f->IsZombie())
            continue;

        TTree *t =
            (TTree*)f->Get("AnalysisTree");

        if(!t)
            continue;

        cfit->cd(i+1);

        TH1D *h =
            new TH1D(Form("hA_%d",i),
                     Form("Position %.0f cm",pos[i]),
                     500,
                     -1,
                     1);

        t->Draw(
            Form("(Q9-Q11)/(Q9+Q11)>>hA_%d",i),
            "",
            "goff");

        if(h->GetEntries()==0)
            continue;

        // first fit

        h->Fit("gaus","Q0");

        TF1 *g1 =
            h->GetFunction("gaus");

        double mean1 =
            g1->GetParameter(1);

        double sigma1 =
            fabs(g1->GetParameter(2));

        // second fit

        TF1 *g2 =
            new TF1(Form("g2_%d",i),
                    "gaus",
                    mean1-2*sigma1,
                    mean1+2*sigma1);

        h->Fit(g2,"RQ");

        double sigmaA =
            fabs(g2->GetParameter(2));

        double esigmaA =
            g2->GetParError(2);

        res[i] =
            sigmaA / fabs(slopeA);

        eres[i] =
            esigmaA / fabs(slopeA);

        std::cout
            << pos[i]
            << " cm : sigmaA = "
            << sigmaA
            << "  resolution = "
            << res[i]
            << " cm"
            << std::endl;

        h->GetXaxis()->SetTitle("Asymmetry");
        h->GetYaxis()->SetTitle("Counts");

        h->Draw();
    }

    cfit->SaveAs(
        "asymmetry_fits.pdf");

    TCanvas *cres =
        new TCanvas("cres",
                    "Asymmetry Position Resolution",
                    900,
                    700);

    TGraphErrors *gr =
        new TGraphErrors(
            5,
            pos,
            res,
            nullptr,
            eres);

    gr->SetMarkerStyle(20);
    gr->SetMarkerSize(1.3);

    gr->SetTitle(
        ";Position (cm);#sigma_{x} from Asymmetry (cm)");

    gr->Draw("AP");

    cres->SaveAs(
        "asymmetry_position_resolution.pdf");
}
