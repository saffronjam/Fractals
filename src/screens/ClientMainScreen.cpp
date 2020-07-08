#include "ClientMainScreen.h"
#include "AppClient.h"

ClientMainScreen::ClientMainScreen(AppClient &parent)
    : m_parent(parent)
{
}

ClientMainScreen::~ClientMainScreen()
{
}

void ClientMainScreen::Build()
{
}

void ClientMainScreen::Destroy()
{
}

void ClientMainScreen::OnEntry()
{
    Camera::Zoom(200.0f);

    // -------------- ALL LABELS ------------------

    // --------------  NUMBER OF ITERATIONS CONTROLLER ------------------
    auto labelIterNum = sfg::Label::Create();
    auto scaleIterNum = sfg::Scale::Create(sfg::Scale::Orientation::HORIZONTAL);

    auto adjustmentIterNum = scaleIterNum->GetAdjustment();
    adjustmentIterNum->SetLower(10.0f);
    adjustmentIterNum->SetUpper(500.0f);
    adjustmentIterNum->SetMinorStep(10.0f);
    adjustmentIterNum->SetMajorStep(500.0f);

    adjustmentIterNum->GetSignal(sfg::Adjustment::OnChange).Connect([adjustmentIterNum, labelIterNum, this] {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(1) << "Iterations: " << adjustmentIterNum->GetValue() << " ";
        labelIterNum->SetText(oss.str());
        m_fractalMgr.SetIterationCount(adjustmentIterNum->GetValue());
    });

    adjustmentIterNum->SetValue(100.0f);

    scaleIterNum->SetRequisition(sf::Vector2f(80.f, 20.f));

    auto boxIterNum = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
    boxIterNum->Pack(labelIterNum, false, false);
    boxIterNum->Pack(scaleIterNum, false, false);
    boxIterNum->SetRequisition(sf::Vector2f(150.0f, 0.0f));

    // --------------  NUMBER OF ITERATIONS CONTROLLER ------------------
    auto labelJuliaC = sfg::Label::Create();
    auto scaleJuliaCr = sfg::Scale::Create(sfg::Scale::Orientation::HORIZONTAL);
    auto scaleJuliaCi = sfg::Scale::Create(sfg::Scale::Orientation::HORIZONTAL);

    auto adjustmentJuliaCr = scaleJuliaCr->GetAdjustment();
    adjustmentJuliaCr->SetLower(0.0f);
    adjustmentJuliaCr->SetUpper(5.0f);
    adjustmentJuliaCr->SetMinorStep(0.05f);
    adjustmentJuliaCr->SetMajorStep(1.0f);

    auto adjustmentJuliaCi = scaleJuliaCi->GetAdjustment();
    adjustmentJuliaCi->SetLower(0.0f);
    adjustmentJuliaCi->SetUpper(5.0f);
    adjustmentJuliaCi->SetMinorStep(0.05f);
    adjustmentJuliaCi->SetMajorStep(1.0f);

    adjustmentJuliaCr->GetSignal(sfg::Adjustment::OnChange).Connect([this, labelJuliaC, adjustmentJuliaCr, adjustmentJuliaCi] {
        double real = adjustmentJuliaCr->GetValue() - 2.5;
        double imag = adjustmentJuliaCi->GetValue() - 2.5;
        char sign = imag < 0.0 ? '-' : '+';
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << real << " " << sign << " " << imag << "i";
        labelJuliaC->SetText(oss.str());
        m_fractalMgr.SetJuliaC(std::complex<double>(real, imag));
    });

    adjustmentJuliaCi->GetSignal(sfg::Adjustment::OnChange).Connect([this, labelJuliaC, adjustmentJuliaCr, adjustmentJuliaCi] {
        double real = adjustmentJuliaCr->GetValue() - 2.5;
        double imag = adjustmentJuliaCi->GetValue() - 2.5;
        char sign = imag < 0.0 ? '-' : '+';
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << real << " " << sign << " " << imag << "i";
        labelJuliaC->SetText(oss.str());
        m_fractalMgr.SetJuliaC(std::complex<double>(real, imag));
    });

    adjustmentJuliaCr->SetValue(2.5f);
    adjustmentJuliaCi->SetValue(2.5f);

    scaleJuliaCr->SetRequisition(sf::Vector2f(80.f, 20.f));
    scaleJuliaCi->SetRequisition(sf::Vector2f(80.f, 20.f));

    auto boxJuliaC = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
    boxJuliaC->Pack(labelJuliaC, false, false);
    boxJuliaC->Pack(scaleJuliaCr, false, false);
    boxJuliaC->Pack(scaleJuliaCi, false, false);

    // -------------- FRACTAL SET CHOICE --------------------
    auto comboBoxFractalChoice = sfg::ComboBox::Create();
    for (auto &[name, fractalSet] : m_fractalMgr.GetFractalSets())
        comboBoxFractalChoice->AppendItem(name);
    comboBoxFractalChoice->SelectItem(1);

    comboBoxFractalChoice->GetSignal(sfg::ComboBox::OnSelect).Connect([this, comboBoxFractalChoice] {
        const auto selectedItem = comboBoxFractalChoice->GetSelectedItem();
        m_fractalMgr.SetFractalSet(comboBoxFractalChoice->GetItem(selectedItem));
    });

    // --------------- SUB BOXES ----------------------

    // -------------- ADD TO MAIN BOX ------------------
    auto mainBox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 15.0f);
    mainBox->Pack(boxIterNum, false);
    mainBox->Pack(comboBoxFractalChoice, false);
    mainBox->Pack(boxJuliaC, false);

    // -------------- ADD TO MAIN WINDOW ------------------
    auto window = sfg::Window::Create(sfg::Window::Style::BACKGROUND);
    window->SetPosition(sf::Vector2f(Window::GetWidth() - 200.0f, 0.0f));
    window->SetRequisition(sf::Vector2f(200.0f, Window::GetHeight()));
    window->Add(mainBox);

    GuiMgr::Add(window);
}

void ClientMainScreen::OnExit()
{
}

void ClientMainScreen::Update()
{
    m_fractalMgr.Update();

    std::ostringstream oss;
    oss << "FPS: " << Clock::GetFPS() << " Frametime: " << Clock::Delta().asSeconds() << "s";

    Window::SetTitle(oss.str());
}

void ClientMainScreen::Draw()
{
    m_fractalMgr.Draw();
}

int ClientMainScreen::GetNextScreenIndex() const
{
    return SCREENINDEX_NO_SCREEN;
}

int ClientMainScreen::GetPreviousScreenIndex() const
{
    return SCREENINDEX_NO_SCREEN;
}