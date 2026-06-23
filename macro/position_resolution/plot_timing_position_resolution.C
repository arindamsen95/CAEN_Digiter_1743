#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TStyle.h>
#include <iostream>
#include <vector>

void plot_timing_position_resolution()
{
    gStyle->SetOptStat(1110);
    gStyle->SetOptFit(1111);

    const double veff = 13.75; // cm/ns

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
        new TCanvas("cfit","dtLR Fits",1600,900);

    cfit->Divide(3,2);

    for(int i=0;i<5;i++)
    {
        TFile *f = TFile::Open(files[i].c_str());

        if(!f || f->IsZombie())
        {
            std::cout << "Cannot open "
                      << files[i] << std::endl;
            continue;
        }

        TTree *t =
            (TTree*)f->Get("AnalysisTree");

        if(!t)
        {
            std::cout << "No AnalysisTree in "
                      << files[i] << std::endl;
            continue;
        }

        cfit->cd(i+1);

        TString hname = Form("hdt_%d",i);

        t->Draw(Form("dtLR>>%s(1000)",hname.Data()),
                "",
                "goff");

        TH1 *h =
            (TH1*)gROOT->FindObject(hname);

        if(!h || h->GetEntries()==0)
        {
            std::cout << "Empty histogram in "
                      << files[i] << std::endl;
            continue;
        }

        // --------------------------------
        // First fit on full histogram
        // --------------------------------

        h->Fit("gaus","Q0");

        TF1 *g1 = h->GetFunction("gaus");

        if(!g1)
        {
            std::cout << "First fit failed for "
                      << files[i] << std::endl;
            continue;
        }

        double mean1 =
            g1->GetParameter(1);

        double sigma1 =
            fabs(g1->GetParameter(2));

        // --------------------------------
        // Second fit in mean ± 3 sigma
        // --------------------------------

        TF1 *g2 =
            new TF1(Form("g2_%d",i),
                    "gaus",
                    mean1-3.0*sigma1,
                    mean1+3.0*sigma1);

        h->Fit(g2,"RQ");

        double mean2 =
            g2->GetParameter(1);

        double sigma2_ps =
            fabs(g2->GetParameter(2));

        double esigma2_ps =
            g2->GetParError(2);

        double sigma2_ns =
            sigma2_ps/1000.0;

        double esigma2_ns =
            esigma2_ps/1000.0;

        resolution[i] =
            (veff/2.0)*sigma2_ns;

        resolutionErr[i] =
            (veff/2.0)*esigma2_ns;

        std::cout
            << Form("%6.0f cm : mean = %8.2f ps, sigma = %8.2f ps, resolution = %.3f cm",
                    pos[i],
                    mean2,
                    sigma2_ps,
                    resolution[i])
            << std::endl;

        h->SetTitle(
            Form("Position %.0f cm",pos[i]));

        h->GetXaxis()->SetTitle("dtLR (ps)");
        h->GetYaxis()->SetTitle("Counts");

        h->Draw();
    }

    cfit->SaveAs("dtLR_fits.pdf");

    // ===================================
    // Position resolution plot
    // ===================================

    TCanvas *cres =
        new TCanvas("cres",
                    "Timing Position Resolution",
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
        ";Position (cm);#sigma_{x} from timing (cm)");

    gr->SetMarkerStyle(20);
    gr->SetMarkerSize(1.3);

    gr->Draw("AP");

    cres->SaveAs(
        "timing_position_resolution.pdf");
}
