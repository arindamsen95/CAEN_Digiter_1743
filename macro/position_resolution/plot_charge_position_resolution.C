#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TStyle.h>
#include <iostream>
#include <vector>

void plot_charge_position_resolution()
{
    gStyle->SetOptStat(1110);
    gStyle->SetOptFit(1111);

    const double lambda = 52.4; // cm

    std::vector<std::string> files = {
        "../left40_analysis.root",
        "../left20_analysis.root",
        "../center_analysis.root",
        "../right20_analysis.root",
        "../right40_analysis.root"
    };

    double pos[5] = {-40,-20,0,20,40};

    double resolution[5];
    double resolutionErr[5];

    TCanvas *cfit =
        new TCanvas("cfit",
                    "Qratio Fits",
                    1600,
                    900);

    cfit->Divide(3,2);

    for(int i=0;i<5;i++)
    {
        TFile *f =
            TFile::Open(files[i].c_str());

        if(!f || f->IsZombie())
        {
            std::cout
                << "Cannot open "
                << files[i]
                << std::endl;
            continue;
        }

        TTree *t =
            (TTree*)f->Get("AnalysisTree");

        if(!t)
        {
            std::cout
                << "No AnalysisTree in "
                << files[i]
                << std::endl;
            continue;
        }

        cfit->cd(i+1);

        TString hname =
            Form("hq_%d",i);

        t->Draw(
            Form("Qratio>>%s(500)",hname.Data()),
            "",
            "goff");

        TH1 *h =
            (TH1*)gROOT->FindObject(hname);

        if(!h || h->GetEntries()==0)
        {
            std::cout
                << "Empty histogram in "
                << files[i]
                << std::endl;
            continue;
        }

        // --------------------------
        // First fit
        // --------------------------

        h->Fit("gaus","Q0");

        TF1 *g1 =
            h->GetFunction("gaus");

        if(!g1)
        {
            std::cout
                << "First fit failed for "
                << files[i]
                << std::endl;
            continue;
        }

        double mean1 =
            g1->GetParameter(1);

        double sigma1 =
            fabs(g1->GetParameter(2));

        // --------------------------
        // Second fit
        // --------------------------

        TF1 *g2 =
            new TF1(
                Form("g2_%d",i),
                "gaus",
                mean1-3.0*sigma1,
                mean1+3.0*sigma1);

        h->Fit(g2,"RQ");

        double mean2 =
            g2->GetParameter(1);

        double sigma2 =
            fabs(g2->GetParameter(2));

        double esigma2 =
            g2->GetParError(2);

        resolution[i] =
            (lambda/2.0)*sigma2;

        resolutionErr[i] =
            (lambda/2.0)*esigma2;

        std::cout
            << Form("%6.0f cm : mean = %8.4f, sigma = %8.4f, resolution = %.3f cm",
                    pos[i],
                    mean2,
                    sigma2,
                    resolution[i])
            << std::endl;

        h->SetTitle(
            Form("Position %.0f cm",
                 pos[i]));

        h->GetXaxis()->SetTitle(
            "ln(Q_{9}/Q_{11})");

        h->GetYaxis()->SetTitle(
            "Counts");

        h->Draw();
    }

    cfit->SaveAs(
        "Qratio_fits.pdf");

    // ==================================
    // Resolution vs position
    // ==================================

    TCanvas *cres =
        new TCanvas(
            "cres",
            "Charge Position Resolution",
            900,
            700);

    TGraphErrors *gr =
        new TGraphErrors(
            5,
            pos,
            resolution,
            nullptr,
            resolutionErr);

    gr->SetTitle(
        ";Position (cm);#sigma_{x} from charge ratio (cm)");

    gr->SetMarkerStyle(20);
    gr->SetMarkerSize(1.3);

    gr->Draw("AP");

    cres->SaveAs(
        "charge_position_resolution.pdf");
}
