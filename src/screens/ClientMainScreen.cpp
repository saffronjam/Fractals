#include "ClientMainScreen.h"
#include "AppClient.h"

ClientMainScreen::ClientMainScreen(AppClient &parent)
        : m_parent(parent),
          m_updatePerformanceLabelsTimer(0.0f)
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
    FPSLimiter::SetDesiredFPS(144.0f);

    // -------------- ALL LABELS ------------------
    auto labelMandelbrotControls = sfg::Label::Create("---- Mandelbrot Controls ----");
    auto labelJuliaControls = sfg::Label::Create("---- Julia Controls ----");
    auto labelPalette = sfg::Label::Create("---- Palette ----");
    auto labelPerformance = sfg::Label::Create("---- Performance ----");
    auto labelPanHint = sfg::Label::Create("Hold down both mouse buttons to pan around");
    auto labelZoomHint = sfg::Label::Create("Use your mouse wheel to zoom");
    m_labelFPS = sfg::Label::Create("");
    m_labelFrametime = sfg::Label::Create("");

    labelPanHint->SetRequisition(sf::Vector2f(180.0f, 0.0f));
    labelZoomHint->SetRequisition(sf::Vector2f(180.0f, 0.0f));

    labelPanHint->SetLineWrap(true);
    labelZoomHint->SetLineWrap(true);

    // --------------  NUMBER OF ITERATIONS CONTROLLER ------------------
    auto labelIterNum = sfg::Label::Create();
    auto scaleIterNum = sfg::Scale::Create(sfg::Scale::Orientation::HORIZONTAL);

    auto adjustmentIterNum = scaleIterNum->GetAdjustment();
    adjustmentIterNum->SetLower(10.0f);
    adjustmentIterNum->SetUpper(500.0f);
    adjustmentIterNum->SetMinorStep(1.0f);
    adjustmentIterNum->SetMajorStep(500.0f);

    adjustmentIterNum->GetSignal(sfg::Adjustment::OnChange).Connect([adjustmentIterNum, labelIterNum, this]
                                                                    {
                                                                        std::ostringstream oss;
                                                                        oss << std::fixed << std::setprecision(1)
                                                                            << "Max Iterations: "
                                                                            << adjustmentIterNum->GetValue() << " ";
                                                                        labelIterNum->SetText(oss.str());
                                                                        m_fractalMgr.SetComputeIterationCount(adjustmentIterNum->GetValue());
                                                                    });

    adjustmentIterNum->SetValue(80.0f);

    scaleIterNum->SetRequisition(sf::Vector2f(80.f, 20.f));

    auto boxIterNum = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
    boxIterNum->Pack(labelIterNum, false, false);
    boxIterNum->Pack(scaleIterNum, false, false);
    boxIterNum->SetRequisition(sf::Vector2f(150.0f, 0.0f));

    // -------------- MANDELBROT DRAW OPTIONS -------------
    auto checkButtonDrawComplexLines = sfg::CheckButton::Create("Complex lines");

    checkButtonDrawComplexLines->GetSignal(sfg::CheckButton::OnToggle).Connect([this, checkButtonDrawComplexLines]
                                                                               {
                                                                                   if (checkButtonDrawComplexLines->IsActive())
                                                                                       m_fractalMgr.SetMandelbrotState(Mandelbrot::State::ComplexLines);
                                                                                   else
                                                                                       m_fractalMgr.SetMandelbrotState(Mandelbrot::State::None);
                                                                               });

    // -------------- JULIA COMPLEX C ADJUSTMENTS  -----------------
    auto labelLeftJuliaC = sfg::Label::Create("R: ");
    auto labelRightJuliaC = sfg::Label::Create("I: ");
    auto scaleJuliaCr = sfg::Scale::Create(sfg::Scale::Orientation::HORIZONTAL);
    auto scaleJuliaCi = sfg::Scale::Create(sfg::Scale::Orientation::HORIZONTAL);

    auto entryJuliaCr = sfg::Entry::Create("");
    auto entryJuliaCi = sfg::Entry::Create("");

    entryJuliaCr->SetState(sfg::Entry::State::INSENSITIVE);
    entryJuliaCi->SetState(sfg::Entry::State::INSENSITIVE);

    m_adjustmentJuliaCr = scaleJuliaCr->GetAdjustment();
    m_adjustmentJuliaCr->SetLower(0.0f);
    m_adjustmentJuliaCr->SetUpper(5.0f);
    m_adjustmentJuliaCr->SetMinorStep(0.005f);
    m_adjustmentJuliaCr->SetMajorStep(0.005f);

    m_adjustmentJuliaCi = scaleJuliaCi->GetAdjustment();
    m_adjustmentJuliaCi->SetLower(0.0f);
    m_adjustmentJuliaCi->SetUpper(5.0f);
    m_adjustmentJuliaCi->SetMinorStep(0.005f);
    m_adjustmentJuliaCi->SetMajorStep(0.005f);

    m_adjustmentJuliaCr->GetSignal(sfg::Adjustment::OnChange).Connect([this] { m_fractalMgr.SetJuliaCR(m_adjustmentJuliaCr->GetValue() - 2.5f); });
    m_adjustmentJuliaCi->GetSignal(sfg::Adjustment::OnChange).Connect([this] { m_fractalMgr.SetJuliaCI(m_adjustmentJuliaCi->GetValue() - 2.5f); });

    entryJuliaCr->GetSignal(sfg::Entry::OnTextChanged).Connect([this, entryJuliaCr]
                                                               {
                                                                   try
                                                                   {
                                                                       float result = std::stof(std::string(entryJuliaCr->GetText())) + 2.5f;
                                                                       m_adjustmentJuliaCr->SetValue(result);
                                                                   }
                                                                   catch (const std::invalid_argument &e)
                                                                   {
                                                                   }
                                                               });
    entryJuliaCi->GetSignal(sfg::Entry::OnTextChanged).Connect([this, entryJuliaCi]
                                                               {
                                                                   try
                                                                   {
                                                                       float result = std::stof(std::string(entryJuliaCi->GetText())) + 2.5f;
                                                                       m_adjustmentJuliaCi->SetValue(result);
                                                                   }
                                                                   catch (const std::invalid_argument &e)
                                                                   {
                                                                   }
                                                               });

    m_adjustmentJuliaCr->SetValue(2.5f);
    m_adjustmentJuliaCi->SetValue(2.5f);

    scaleJuliaCr->SetRequisition(sf::Vector2f(120.f, 20.f));
    scaleJuliaCi->SetRequisition(sf::Vector2f(120.f, 20.f));
    entryJuliaCr->SetRequisition(sf::Vector2f(65.f, 0.f));
    entryJuliaCi->SetRequisition(sf::Vector2f(65.f, 0.f));

    auto boxJuliaCReal = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL, 1.0f);
    boxJuliaCReal->Pack(labelLeftJuliaC);
    boxJuliaCReal->Pack(entryJuliaCr);

    auto boxJuliaCImag = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL, 1.0f);
    boxJuliaCImag->Pack(labelRightJuliaC);
    boxJuliaCImag->Pack(entryJuliaCi);

    auto boxJuliaCNumber = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL, 10.0f);
    boxJuliaCNumber->Pack(boxJuliaCReal);
    boxJuliaCNumber->Pack(boxJuliaCImag);

    auto boxJuliaC = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 3.0f);
    boxJuliaC->Pack(boxJuliaCNumber);
    boxJuliaC->Pack(scaleJuliaCr);
    boxJuliaC->Pack(scaleJuliaCi);

    // -------------- JULIA ANIMATION RADIOBUTTONS -----------------
    auto radioButtonNone = sfg::RadioButton::Create("None");
    auto radioButtonAnimate = sfg::RadioButton::Create("Animate", radioButtonNone->GetGroup());
    auto radioButtonFollowCursor = sfg::RadioButton::Create("Follow Cursor", radioButtonNone->GetGroup());

    radioButtonNone->GetSignal(sfg::ToggleButton::OnToggle).Connect(
            [this] { m_fractalMgr.SetJuliaState(Julia::State::None); });
    radioButtonAnimate->GetSignal(sfg::ToggleButton::OnToggle).Connect(
            [this] { m_fractalMgr.SetJuliaState(Julia::State::Animate); });
    radioButtonFollowCursor->GetSignal(sfg::ToggleButton::OnToggle).Connect(
            [this] { m_fractalMgr.SetJuliaState(Julia::State::FollowCursor); });

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

    comboBoxFractalChoice->GetSignal(sfg::ComboBox::OnSelect).Connect(
            [this, comboBoxFractalChoice,
                    checkButtonDrawComplexLines,
                    scaleJuliaCr, scaleJuliaCi,
                    radioButtonNone, radioButtonAnimate, radioButtonFollowCursor,
                    entryJuliaCr, entryJuliaCi]
            {
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
                    entryJuliaCr->SetState(sfg::ComboBox::State::INSENSITIVE);
                    entryJuliaCi->SetState(sfg::ComboBox::State::INSENSITIVE);
                }
                else
                {
                    checkButtonDrawComplexLines->SetState(sfg::ComboBox::State::INSENSITIVE);
                    scaleJuliaCr->SetState(sfg::ComboBox::State::NORMAL);
                    scaleJuliaCi->SetState(sfg::ComboBox::State::NORMAL);
                    radioButtonNone->SetState(sfg::ComboBox::State::NORMAL);
                    radioButtonAnimate->SetState(sfg::ComboBox::State::NORMAL);
                    radioButtonFollowCursor->SetState(sfg::ComboBox::State::NORMAL);
                    entryJuliaCr->SetState(sfg::ComboBox::State::NORMAL);
                    entryJuliaCi->SetState(sfg::ComboBox::State::NORMAL);
                }
            });

    comboBoxFractalChoice->SelectItem(1);

    // --------------- PALETTE CHOICE -------------------
    auto radioButtonPalFiery = sfg::RadioButton::Create("Fiery");
    auto radioButtonPalUV = sfg::RadioButton::Create("UV", radioButtonPalFiery->GetGroup());
    auto radioButtonPalGreyScale = sfg::RadioButton::Create("Greyscale", radioButtonPalFiery->GetGroup());
    auto radioButtonPalRainbow = sfg::RadioButton::Create("Rainbow", radioButtonPalFiery->GetGroup());

    radioButtonPalFiery->GetSignal(sfg::RadioButton::OnToggle).Connect([this] { m_fractalMgr.SetPalette(FractalSet::Palette::Fiery); });
    radioButtonPalUV->GetSignal(sfg::RadioButton::OnToggle).Connect([this] { m_fractalMgr.SetPalette(FractalSet::Palette::UV); });
    radioButtonPalGreyScale->GetSignal(sfg::RadioButton::OnToggle).Connect([this] { m_fractalMgr.SetPalette(FractalSet::Palette::GreyScale); });
    radioButtonPalRainbow->GetSignal(sfg::RadioButton::OnToggle).Connect([this] { m_fractalMgr.SetPalette(FractalSet::Palette::Rainbow); });

    radioButtonPalFiery->SetActive(true);

    auto boxPalChoice = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
    boxPalChoice->Pack(labelPalette);
    boxPalChoice->Pack(radioButtonPalFiery);
    boxPalChoice->Pack(radioButtonPalUV);
    boxPalChoice->Pack(radioButtonPalGreyScale);
    boxPalChoice->Pack(radioButtonPalRainbow);

    // --------------- SUB BOXES ----------------------
    auto boxMainControls = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 5.0f);
    boxMainControls->Pack(comboBoxFractalChoice);
    boxMainControls->Pack(boxIterNum);

    auto boxMandelbrotControls = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 5.0f);
    boxMandelbrotControls->Pack(labelMandelbrotControls);
    boxMandelbrotControls->Pack(checkButtonDrawComplexLines);

    auto boxJuliaControls = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 5.0f);
    boxJuliaControls->Pack(labelJuliaControls);
    boxJuliaControls->Pack(boxJuliaC);
    boxJuliaControls->Pack(boxJuliaAnimation);

    auto boxPerformance = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 5.0f);
    boxPerformance->Pack(labelPerformance);
    boxPerformance->Pack(m_labelFPS);
    boxPerformance->Pack(m_labelFrametime);

    // -------------- ADD TO MAIN BOX ------------------
    auto mainBox = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 25.0f);
    mainBox->Pack(boxMainControls, false);
    mainBox->Pack(boxMandelbrotControls, false);
    mainBox->Pack(boxJuliaControls, false);
    mainBox->Pack(boxPalChoice, false);
    mainBox->Pack(boxPerformance, false);
    mainBox->Pack(labelPanHint, false);
    mainBox->Pack(labelZoomHint, false);

    // -------------- ADD TO MAIN WINDOW ------------------
    m_guiWindow = sfg::Window::Create(sfg::Window::Style::BACKGROUND);
    m_guiWindow->SetPosition(sf::Vector2f(static_cast<float>(Window::GetWidth()) - 200.0f, 0.0f));
    m_guiWindow->SetRequisition(sf::Vector2f(200.0f, Window::GetHeight()));
    m_guiWindow->Add(mainBox);

    GuiMgr::Add(m_guiWindow);

    Window::AddCallback<Window::OnResize>([this](const auto &nonused)
                                          {
                                              m_guiWindow->SetPosition(sf::Vector2f(static_cast<float>(Window::GetWidth()) - 200.0f, 0.0f));
                                              m_guiWindow->SetRequisition(sf::Vector2f(200.0f, Window::GetHeight()));
                                          });
}

void ClientMainScreen::OnExit()
{
}

void ClientMainScreen::Update()
{
    m_fractalMgr.Update(m_adjustmentJuliaCr, m_adjustmentJuliaCi);

    m_updatePerformanceLabelsTimer += Clock::Delta().asSeconds();
    if (m_updatePerformanceLabelsTimer > 0.5f)
    {
        std::ostringstream oss;
        oss << "FPS: " << std::fixed << std::setprecision(0) << Clock::GetFPS() << "   \t\n";
        oss << "Frametime: " << std::fixed << std::setprecision(4) << Clock::Delta().asSeconds() << "s"
            << "   \t\n";
        m_labelFrametime->SetText(oss.str());
        m_updatePerformanceLabelsTimer = 0.0f;
    }

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