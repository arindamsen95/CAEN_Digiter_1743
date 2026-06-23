void plot_reconstructed_position_asymmetry()
{
    gStyle->SetOptFit(1111);

    const double a = 0.0974005;
    const double b = -0.015994;

    std::vector<std::string> files = {
        "../left40_analysis.root",
        "../left20_analysis.root",
        "../center_analysis.root",
        "../right20_analysis.root",
        "../right40_analysis.root"
    };

    double truePos[5] = {-40,-20,0,20,40};

    TCanvas *c =
        new TCanvas("c",
                    "Asymmetry Reconstruction",
                    1600,
                    900);

    c->Divide(3,2);

    for(int i=0;i<5;i++)
    {
        TFile *f =
            TFile::Open(files[i].c_str());

        TTree *t =
            (TTree*)f->Get("AnalysisTree");

        c->cd(i+1);

        TH1D *h =
            new TH1D(
                Form("h%d",i),
                Form("True position %.0f cm",
                     truePos[i]),
                400,
                -60,
                60);

        TString expr =
            Form(
            "(((Q9-Q11)/(Q9+Q11))-(%f))/(%f)>>h%d",
            a,b,i);

        t->Draw(expr,"","goff");

        h->Fit("gaus","Q");

        TF1 *g =
            h->GetFunction("gaus");

        std::cout
            << "True position = "
            << truePos[i]
            << " cm"
            << "   reconstructed mean = "
            << g->GetParameter(1)
            << " cm"
            << "   sigma = "
            << g->GetParameter(2)
            << " cm"
            << std::endl;

        h->GetXaxis()->SetTitle("Reconstructed position (cm)");
        h->GetYaxis()->SetTitle("Counts");

        h->Draw();
    }

    c->SaveAs(
        "asymmetry_reconstructed_position.pdf");
}
