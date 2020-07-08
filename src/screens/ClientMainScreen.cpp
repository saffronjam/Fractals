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
    auto labelMandelbrotControls = sfg::Label::Create("Mandelbrot Controls");
    auto labelJuliaControls = sfg::Label::Create("Julia Controls");

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
        oss << std::fixed << std::setprecision(1) << "Max Iterations: " << adjustmentIterNum->GetValue() << " ";
        labelIterNum->SetText(oss.str());
        m_fractalMgr.SetIterationCount(adjustmentIterNum->GetValue());
    });

    adjustmentIterNum->SetValue(80.0f);

    scaleIterNum->SetRequisition(sf::Vector2f(80.f, 20.f));

    auto boxIterNum = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
    boxIterNum->Pack(labelIterNum, false, false);
    boxIterNum->Pack(scaleIterNum, false, false);
    boxIterNum->SetRequisition(sf::Vector2f(150.0f, 0.0f));

    // -------------- MANDELBROT DRAW OPTIONS -------------
    auto checkButtonDrawComplexLines = sfg::CheckButton::Create("Complex lines");

    checkButtonDrawComplexLines->GetSignal(sfg::CheckButton::OnToggle).Connect([this, checkButtonDrawComplexLines] {
        m_fractalMgr.SetDrawComplexLines(checkButtonDrawComplexLines->IsActive());
    });

    // -------------- JULIA COMPLEX C ADJUSTMENTS  -----------------
    auto labelJuliaC = sfg::Label::Create();
    auto scaleJuliaCr = sfg::Scale::Create(sfg::Scale::Orientation::HORIZONTAL);
    auto scaleJuliaCi = sfg::Scale::Create(sfg::Scale::Orientation::HORIZONTAL);

    m_adjustmentJuliaCr = scaleJuliaCr->GetAdjustment();
    m_adjustmentJuliaCr->SetLower(0.0f);
    m_adjustmentJuliaCr->SetUpper(5.0f);
    m_adjustmentJuliaCr->SetMinorStep(0.05f);
    m_adjustmentJuliaCr->SetMajorStep(1.0f);

    m_adjustmentJuliaCi = scaleJuliaCi->GetAdjustment();
    m_adjustmentJuliaCi->SetLower(0.0f);
    m_adjustmentJuliaCi->SetUpper(5.0f);
    m_adjustmentJuliaCi->SetMinorStep(0.05f);
    m_adjustmentJuliaCi->SetMajorStep(1.0f);

    m_adjustmentJuliaCr->GetSignal(sfg::Adjustment::OnChange).Connect([this, labelJuliaC] {
        double real = m_adjustmentJuliaCr->GetValue() - 2.5;
        double imag = m_adjustmentJuliaCi->GetValue() - 2.5;
        char sign = imag < 0.0 ? '-' : '+';
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << real << " " << sign << " " << abs(imag) << "i";
        labelJuliaC->SetText(oss.str());
        m_fractalMgr.SetJuliaC(std::complex<double>(real, imag));
    });

    m_adjustmentJuliaCi->GetSignal(sfg::Adjustment::OnChange).Connect([this, labelJuliaC] {
        double real = m_adjustmentJuliaCr->GetValue() - 2.5;
        double imag = m_adjustmentJuliaCi->GetValue() - 2.5;
        char sign = imag < 0.0 ? '-' : '+';
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << real << " " << sign << " " << abs(imag) << "i";
        labelJuliaC->SetText(oss.str());
        m_fractalMgr.SetJuliaC(std::complex<double>(real, imag));
    });

    m_adjustmentJuliaCr->SetValue(2.5f);
    m_adjustmentJuliaCi->SetValue(2.5f);

    scaleJuliaCr->SetRequisition(sf::Vector2f(80.f, 20.f));
    scaleJuliaCi->SetRequisition(sf::Vector2f(80.f, 20.f));

    auto boxJuliaC = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
    boxJuliaC->Pack(labelJuliaC, false, false);
    boxJuliaC->Pack(scaleJuliaCr, false, false);
    boxJuliaC->Pack(scaleJuliaCi, false, false);

    // -------------- JULIA ANIMATION RADIOBUTTONS -----------------
    auto radioButtonNone = sfg::RadioButton::Create("None");
    auto radioButtonAnimate = sfg::RadioButton::Create("Animate", radioButtonNone->GetGroup());
    auto radioButtonFollowCursor = sfg::RadioButton::Create("Follow Cursor", radioButtonNone->GetGroup());

    radioButtonNone->GetSignal(sfg::ToggleButton::OnToggle).Connect([this] { m_fractalMgr.SetJuliaSetState(FractalMgr::JuliaState::None); });
    radioButtonAnimate->GetSignal(sfg::ToggleButton::OnToggle).Connect([this] { m_fractalMgr.SetJuliaSetState(FractalMgr::JuliaState::Animate); });
    radioButtonFollowCursor->GetSignal(sfg::ToggleButton::OnToggle).Connect([this] { m_fractalMgr.SetJuliaSetState(FractalMgr::JuliaState::FollowCursor); });

    radioButtonNone->SetActive(true);

    auto boxJuliaAnimation = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 3.0f);
    boxJuliaAnimation->Pack(radioButtonNone);
    boxJuliaAnimation->Pack(radioButtonAnimate);
    boxJuliaAnimation->Pack(radioButtonFollowCursor);

    // -------------- FRACTAL SET CHOICE --------------------
    auto comboBoxFractalChoice = sfg::ComboBox::Create();
    for (auto &[name, fractalSet] : m_fractalMgr.GetFractalSets())
        comboBoxFractalChoice->AppendItem(name);

    scaleJuliaCr->SetState(sfg::ComboBox::State::INSENSITIVE);
    scaleJuliaCi->SetState(sfg::ComboBox::State::INSENSITIVE);
    radioButtonNone->SetState(sfg::ComboBox::State::INSENSITIVE);
    radioButtonAnimate->SetState(sfg::ComboBox::State::INSENSITIVE);
    radioButtonFollowCursor->SetState(sfg::ComboBox::State::INSENSITIVE);

    comboBoxFractalChoice->GetSignal(sfg::ComboBox::OnSelect).Connect([this, comboBoxFractalChoice, checkButtonDrawComplexLines, scaleJuliaCr, scaleJuliaCi, radioButtonNone, radioButtonAnimate, radioButtonFollowCursor] {
        const auto selectedItem = comboBoxFractalChoice->GetSelectedItem();
        m_fractalMgr.SetFractalSet(comboBoxFractalChoice->GetItem(selectedItem));
        if (comboBoxFractalChoice->GetItem(selectedItem) == "Mandelbrot")
        {
            checkButtonDrawComplexLines->SetState(sfg::ComboBox::State::NORMAL);
            scaleJuliaCr->SetState(sfg::ComboBox::State::INSENSITIVE);
            scaleJuliaCi->SetState(sfg::ComboBox::State::INSENSITIVE);
            radioButtonNone->SetState(sfg::ComboBox::State::INSENSITIVE);
            radioButtonAnimate->SetState(sfg::ComboBox::State::INSENSITIVE);
            radioButtonFollowCursor->SetState(sfg::ComboBox::State::INSENSITIVE);
        }
        else
        {
            checkButtonDrawComplexLines->SetState(sfg::ComboBox::State::INSENSITIVE);
            scaleJuliaCr->SetState(sfg::ComboBox::State::NORMAL);
            scaleJuliaCi->SetState(sfg::ComboBox::State::NORMAL);
            radioButtonNone->SetState(sfg::ComboBox::State::NORMAL);
            radioButtonAnimate->SetState(sfg::ComboBox::State::NORMAL);
            radioButtonFollowCursor->SetState(sfg::ComboBox::State::NORMAL);
        }
    });

    comboBoxFractalChoice->SelectItem(1);

    // --------------- SUB BOXES ----------------------
    auto boxMandelbrotControls = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 5.0f);
    boxMandelbrotControls->Pack(labelMandelbrotControls);
    boxMandelbrotControls->Pack(checkButtonDrawComplexLines);

    auto boxJuliaControls = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 5.0f);
    boxJuliaControls->Pack(labelJuliaControls);
    boxJuliaControls->Pack(boxJuliaC);
    boxJuliaControls->Pack(boxJuliaAnimation);

    // -------------- ADD TO MAIN BOX ------------------
    auto mainBox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 15.0f);
    mainBox->Pack(boxIterNum, false);
    mainBox->Pack(comboBoxFractalChoice, false);
    mainBox->Pack(boxMandelbrotControls, false);
    mainBox->Pack(boxJuliaControls, false);

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
    m_fractalMgr.Update(m_adjustmentJuliaCr, m_adjustmentJuliaCi);

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