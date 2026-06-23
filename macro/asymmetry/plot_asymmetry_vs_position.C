#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TF1.h>
#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TStyle.h>
#include <iostream>
#include <vector>

void plot_asymmetry_vs_position()
{
    gStyle->SetOptFit(1111);
    gStyle->SetOptStat(0);

    std::vector<std::string> files = {
        "../left40_analysis.root",
        "../left20_analysis.root",
        "../center_analysis.root",
        "../right20_analysis.root",
        "../right40_analysis.root"
    };

    double pos[5] = {-40,-20,0,20,40};

    double meanA[5];
    double errA[5];

    TCanvas *cfit =
        new TCanvas("cfit",
                    "Asymmetry Distributions",
                    1600,
                    900);

    cfit->Divide(3,2);

    for(int i=0;i<5;i++)
    {
        TFile *f = TFile::Open(files[i].c_str());

        TTree *t =
            (TTree*)f->Get("AnalysisTree");

        cfit->cd(i+1);

        TString hname =
            Form("hA_%d",i);

        t->Draw(
            Form("(Q9-Q11)/(Q9+Q11)>>%s(400)",hname.Data()),
            "",
            "goff");

        TH1 *h =
            (TH1*)gROOT->FindObject(hname);

        h->Fit("gaus","Q0");

        TF1 *g1 =
            h->GetFunction("gaus");

        double mean =
            g1->GetParameter(1);

        double sigma =
            fabs(g1->GetParameter(2));

        TF1 *g2 =
            new TF1(
                Form("g2_%d",i),
                "gaus",
                mean-3*sigma,
                mean+3*sigma);

        h->Fit(g2,"RQ");

        meanA[i] =
            g2->GetParameter(1);

        errA[i] =
            g2->GetParError(1);

        h->SetTitle(
            Form("Position %.0f cm",pos[i]));

        h->GetXaxis()->SetTitle("A=(Q9-Q11)/(Q9+Q11)");
        h->GetYaxis()->SetTitle("Counts");


        h->Draw();

        std::cout
            << pos[i]
            << " cm : "
            << meanA[i]
            << std::endl;

    }

    cfit->SaveAs("asymmetry_distributions.pdf");

    TCanvas *c =
        new TCanvas("c",
                    "Asymmetry Calibration",
                    900,
                    700);

    TGraphErrors *gr =
        new TGraphErrors(
            5,
            pos,
            meanA,
            nullptr,
            errA);

    gr->SetMarkerStyle(20);
    gr->SetMarkerSize(1.4);

    gr->SetTitle(
        ";Position (cm);Mean Asymmetry");

    gr->Draw("AP");

    TF1 *line =
        new TF1("line",
                "[0]+[1]*x",
                -50,
                50);

    gr->Fit(line,"R");

    c->SaveAs(
        "asymmetry_vs_position.pdf");
}
