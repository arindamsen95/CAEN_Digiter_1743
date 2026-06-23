#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TF1.h>
#include <TStyle.h>
#include <TLatex.h>
#include <TLegend.h>

void plot_reconstructed_vs_true()
{
    gStyle->SetOptFit(1111);

    const int N = 5;

    double xtrue[N] = {-40,-20,0,20,40};

    double xrec[N] = {
        -39.327,
        -21.2127,
        -1.20274,
         22.1515,
         24.4724
    };

    // use reconstructed sigma as y error

    double ey[N] = {
         3.97673,
         5.90995,
         6.8383,
         7.15557,
        26.1401
    };

    double ex[N] = {0,0,0,0,0};

    TCanvas *c =
        new TCanvas("c",
                    "Reconstructed vs True Position",
                    900,
                    700);

    TGraphErrors *gr =
        new TGraphErrors(
            N,
            xtrue,
            xrec,
            ex,
            ey);

    gr->SetTitle(
        ";True Position (cm);Reconstructed Position (cm)");

    gr->SetMarkerStyle(20);
    gr->SetMarkerSize(1.5);

    gr->Draw("AP");

    TF1 *line =
        new TF1("line",
                "[0]+[1]*x",
                -50,
                50);

    line->SetParameters(0,1);

    gr->Fit(line,"R");

    line->SetLineColor(kRed+1);
    
    // ideal line

    TF1 *ideal =
        new TF1("ideal",
                "x",
                -50,
                50);

    ideal->SetLineStyle(2);
    ideal->SetLineWidth(2);
    ideal->SetLineColor(kBlue+1);
    ideal->Draw("same");

	TLegend *leg = new TLegend(0.15,0.65,0.45,0.80);
	leg->AddEntry(line,  "Reconstructed line", "l");
	leg->AddEntry(ideal, "Ideal line (y = x)", "l");
	leg->SetBorderSize(0);
	leg->SetFillStyle(0);
	leg->Draw();

    TLatex latex;
    latex.SetNDC();

    latex.DrawLatex(
        0.15,
        0.85,
        Form("Intercept = %.2f cm",
             line->GetParameter(0)));

    latex.DrawLatex(
        0.15,
        0.80,
        Form("Slope = %.3f",
             line->GetParameter(1)));

    c->SaveAs("reconstructed_vs_true_position.pdf");
}
