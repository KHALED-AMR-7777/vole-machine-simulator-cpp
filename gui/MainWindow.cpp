// Part 3 (GUI) - Vole Machine Simulator
// Qt Widgets front end. See MainWindow.h and CORE_GETTERS.txt.

#include "MainWindow.h"

#include <QApplication>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDatabase>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QSlider>
#include <QSplitter>
#include <QTableWidget>
#include <QTimer>
#include <QVBoxLayout>

#include <exception>
#include <iostream>
#include <sstream>

namespace
{
// How an item in a table is coloured.
enum Kind { KNormal = 0, KChanged = 1, KNext = 2, KLast = 3 };

// Timer interval (ms) for each speed level 1..8 of the slider.
const int kIntervals[8] = { 1000, 600, 350, 200, 100, 50, 20, 5 };

QString hex2(int v) { return QString("%1").arg(v & 0xFF, 2, 16, QLatin1Char('0')).toUpper(); }
QString hex4(int v) { return QString("%1").arg(v & 0xFFFF, 4, 16, QLatin1Char('0')).toUpper(); }
QString regName(int r) { return QString("R%1").arg(r, 0, 16).toUpper(); }

QString binary8(int v)
{
    QString b = QString("%1").arg(v & 0xFF, 8, 2, QLatin1Char('0'));
    return b.left(4) + QLatin1Char(' ') + b.right(4);
}

// The core prints messages like "Error: Invalid instruction!". The banner adds
// its own wording, so drop the leading "Error: " and the line breaks.
QString cleanCoreMessage(QString text)
{
    text = text.trimmed();
    text.replace('\n', ' ');
    if (text.startsWith("Error: "))
        text = text.mid(7);
    return text;
}

// Accepts "A", "0A", "0x0A" ... and returns false for anything else.
bool parseHexByte(QString text, int& value)
{
    text = text.trimmed();
    if (text.startsWith("0x", Qt::CaseInsensitive))
        text = text.mid(2);

    static const QRegularExpression re("^[0-9A-Fa-f]{1,2}$");
    if (!re.match(text).hasMatch())
        return false;

    value = text.toInt(nullptr, 16);
    return true;
}

// Gives a table item one of the four looks. Does nothing if it already has it,
// which keeps fast "Run" mode cheap.
void styleItem(QTableWidgetItem* item, int kind)
{
    if (item->data(Qt::UserRole + 1).toInt() == kind)
        return;
    item->setData(Qt::UserRole + 1, kind);

    QFont f = item->font();
    f.setBold(kind == KNext);
    item->setFont(f);

    switch (kind)
    {
    case KChanged:
        item->setBackground(QColor("#FFE9A8"));
        item->setForeground(QColor("#1A1A1A"));
        break;
    case KNext:
        item->setBackground(QColor("#BBD5FF"));
        item->setForeground(QColor("#0B1F4D"));
        break;
    case KLast:
        item->setBackground(QColor("#D5EED8"));
        item->setForeground(QColor("#1A1A1A"));
        break;
    default:
        item->setData(Qt::BackgroundRole, QVariant());
        item->setData(Qt::ForegroundRole, QVariant());
        break;
    }
}

void setTextIfChanged(QTableWidgetItem* item, const QString& text)
{
    if (item->text() != text)
        item->setText(text);
}

QTableWidgetItem* makeItem(const QString& text, Qt::Alignment align, bool editable)
{
    auto* item = new QTableWidgetItem(text);
    item->setTextAlignment(align);
    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    if (editable)
        flags |= Qt::ItemIsEditable;
    item->setFlags(flags);
    return item;
}
} // namespace

// ---------------------------------------------------------------------------
// HexSpinBox
// ---------------------------------------------------------------------------

HexSpinBox::HexSpinBox(QWidget* parent) : QSpinBox(parent)
{
    setRange(0, 254);
    setSingleStep(2);
}

QString HexSpinBox::textFromValue(int value) const
{
    return hex2(value);
}

int HexSpinBox::valueFromText(const QString& text) const
{
    bool ok = false;
    int v = text.toInt(&ok, 16);
    if (!ok)
        v = 0;
    if (v > 254)
        v = 254;
    return v & ~1; // instructions must start on an even address
}

QValidator::State HexSpinBox::validate(QString& input, int&) const
{
    static const QRegularExpression re("^[0-9A-Fa-f]{0,2}$");
    if (!re.match(input).hasMatch())
        return QValidator::Invalid;
    if (input.isEmpty())
        return QValidator::Intermediate;

    bool ok = false;
    int v = input.toInt(&ok, 16);
    return (ok && v <= 254) ? QValidator::Acceptable : QValidator::Intermediate;
}

// ---------------------------------------------------------------------------
// MainWindow - construction
// ---------------------------------------------------------------------------

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
    setWindowTitle("Vole Machine Simulator");
    resize(1280, 800);

    timer_ = new QTimer(this);
    connect(timer_, &QTimer::timeout, this, &MainWindow::onTick);

    buildUi();
    onSpeedChanged(speedSlider_->value());
    refresh(false);
    setState(State::NoProgram);
}

void MainWindow::buildUi()
{
    auto* central = new QWidget(this);
    auto* root = new QVBoxLayout(central);
    root->setContentsMargins(12, 12, 12, 12);
    root->setSpacing(10);

    banner_ = new QLabel(central);
    banner_->setObjectName("banner");
    banner_->setWordWrap(true);
    root->addWidget(banner_);

    root->addWidget(buildToolbar());

    auto* mainSplit = new QSplitter(Qt::Horizontal, central);
    mainSplit->addWidget(buildInstructionPanel());
    mainSplit->addWidget(buildMemoryPanel());
    mainSplit->addWidget(buildRegisterPanel());
    mainSplit->setStretchFactor(0, 4);
    mainSplit->setStretchFactor(1, 5);
    mainSplit->setStretchFactor(2, 3);
    mainSplit->setChildrenCollapsible(false);
    mainSplit->setSizes({ 380, 560, 320 });

    auto* vertSplit = new QSplitter(Qt::Vertical, central);
    vertSplit->addWidget(mainSplit);
    vertSplit->addWidget(buildBottomPanel());
    vertSplit->setStretchFactor(0, 5);
    vertSplit->setStretchFactor(1, 1);
    vertSplit->setChildrenCollapsible(false);

    root->addWidget(vertSplit, 1);
    setCentralWidget(central);
}

QWidget* MainWindow::buildToolbar()
{
    auto* bar = new QWidget(this);
    auto* row = new QHBoxLayout(bar);
    row->setContentsMargins(0, 0, 0, 0);
    row->setSpacing(8);

    // --- step 1: program ---
    auto* step1 = new QLabel("<b>1</b>&nbsp; Program", bar);
    openBtn_ = new QPushButton("Open program...", bar);
    openBtn_->setObjectName("openBtn");
    openBtn_->setShortcut(QKeySequence::Open);
    openBtn_->setToolTip("Choose a Vole program (.txt file with one 4-digit hex instruction per line)  [Ctrl+O]");

    fileLabel_ = new QLabel("no file chosen", bar);
    fileLabel_->setMinimumWidth(150);
    fileLabel_->setStyleSheet("color: gray;");

    auto* addrLabel = new QLabel("Start at address", bar);
    startSpin_ = new HexSpinBox(bar);
    startSpin_->setObjectName("startSpin");
    startSpin_->setToolTip("Where in memory the first instruction goes (hex, must be even: 00, 02, 04 ...)");

    loadBtn_ = new QPushButton("Load", bar);
    loadBtn_->setObjectName("loadBtn");
    loadBtn_->setToolTip("Put the program into memory and get the CPU ready  [Ctrl+L]");
    loadBtn_->setShortcut(QKeySequence("Ctrl+L"));

    // --- step 2: run ---
    auto* step2 = new QLabel("<b>2</b>&nbsp; Run", bar);
    stepBtn_ = new QPushButton("Step", bar);
    stepBtn_->setObjectName("stepBtn");
    stepBtn_->setMinimumHeight(34);
    stepBtn_->setToolTip("Run exactly one instruction  [F10]");
    stepBtn_->setShortcut(QKeySequence("F10"));

    runBtn_ = new QPushButton("Run", bar);
    runBtn_->setObjectName("runBtn");
    runBtn_->setMinimumHeight(34);
    runBtn_->setMinimumWidth(80);
    runBtn_->setToolTip("Run the program automatically. Press again to pause at any moment  [F5]");
    runBtn_->setShortcut(QKeySequence("F5"));

    speedLabel_ = new QLabel(bar);
    speedLabel_->setMinimumWidth(80);
    speedSlider_ = new QSlider(Qt::Horizontal, bar);
    speedSlider_->setObjectName("speedSlider");
    speedSlider_->setRange(1, 8);
    speedSlider_->setValue(4);
    speedSlider_->setMaximumWidth(140);
    speedSlider_->setToolTip("How fast Run goes");

    restartBtn_ = new QPushButton("Restart", bar);
    restartBtn_->setObjectName("restartBtn");
    restartBtn_->setToolTip("Reload the program from its file and start again  [Ctrl+R]");
    restartBtn_->setShortcut(QKeySequence("Ctrl+R"));

    clearBtn_ = new QPushButton("Clear machine", bar);
    clearBtn_->setObjectName("clearBtn");
    clearBtn_->setToolTip("Empty memory, registers and screen");

    row->addWidget(step1);
    row->addWidget(openBtn_);
    row->addWidget(fileLabel_);
    row->addSpacing(6);
    row->addWidget(addrLabel);
    row->addWidget(startSpin_);
    row->addWidget(loadBtn_);
    row->addSpacing(24);
    row->addWidget(step2);
    row->addWidget(stepBtn_);
    row->addWidget(runBtn_);
    row->addWidget(speedSlider_);
    row->addWidget(speedLabel_);
    row->addStretch(1);
    row->addWidget(restartBtn_);
    row->addWidget(clearBtn_);

    connect(openBtn_, &QPushButton::clicked, this, &MainWindow::chooseFile);
    connect(loadBtn_, &QPushButton::clicked, this, &MainWindow::loadProgram);
    connect(stepBtn_, &QPushButton::clicked, this, &MainWindow::stepOnce);
    connect(runBtn_, &QPushButton::clicked, this, &MainWindow::toggleRun);
    connect(restartBtn_, &QPushButton::clicked, this, &MainWindow::restartProgram);
    connect(clearBtn_, &QPushButton::clicked, this, &MainWindow::clearMachine);
    connect(speedSlider_, &QSlider::valueChanged, this, &MainWindow::onSpeedChanged);

    return bar;
}

QWidget* MainWindow::buildInstructionPanel()
{
    auto* box = new QGroupBox("Program  (each instruction = 2 memory cells)", this);
    auto* lay = new QVBoxLayout(box);

    insnTable_ = new QTableWidget(128, 3, box);
    insnTable_->setObjectName("insnTable");
    insnTable_->setHorizontalHeaderLabels({ "Addr", "Hex", "What it does" });
    insnTable_->verticalHeader()->setVisible(false);
    insnTable_->verticalHeader()->setDefaultSectionSize(24);
    insnTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    insnTable_->setSelectionMode(QAbstractItemView::NoSelection);
    insnTable_->setFocusPolicy(Qt::NoFocus);
    insnTable_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    insnTable_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    insnTable_->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    insnTable_->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    for (int i = 0; i < 128; ++i)
    {
        insnTable_->setItem(i, 0, makeItem(hex2(i * 2), Qt::AlignCenter, false));
        insnTable_->setItem(i, 1, makeItem("0000", Qt::AlignCenter, false));
        insnTable_->setItem(i, 2, makeItem("", Qt::AlignLeft | Qt::AlignVCenter, false));
    }

    auto* hint = new QLabel("Blue = next instruction to run.  Green = the one just run.  "
                            "Yellow = something that just changed.", box);
    hint->setWordWrap(true);
    hint->setStyleSheet("color: gray;");

    lay->addWidget(insnTable_);
    lay->addWidget(hint);
    return box;
}

QWidget* MainWindow::buildMemoryPanel()
{
    auto* box = new QGroupBox("Memory  (256 cells, double-click a cell to change it)", this);
    auto* lay = new QVBoxLayout(box);

    memTable_ = new QTableWidget(16, 16, box);
    memTable_->setObjectName("memTable");

    QStringList cols, rows;
    for (int i = 0; i < 16; ++i)
    {
        cols << QString("%1").arg(i, 0, 16).toUpper();
        rows << hex2(i * 16);
    }
    memTable_->setHorizontalHeaderLabels(cols);
    memTable_->setVerticalHeaderLabels(rows);
    memTable_->horizontalHeader()->setMinimumSectionSize(20);
    memTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    memTable_->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    memTable_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    memTable_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    memTable_->setSelectionMode(QAbstractItemView::SingleSelection);
    memTable_->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    for (int a = 0; a < 256; ++a)
    {
        const bool screenPort = (a == 255);
        auto* item = makeItem("00", Qt::AlignCenter, !screenPort);
        item->setToolTip(screenPort ? "FF is the screen: storing a value here prints it on the Screen panel"
                                    : QString("Address %1").arg(hex2(a)));
        memTable_->setItem(a / 16, a % 16, item);
    }

    connect(memTable_, &QTableWidget::itemChanged, this, &MainWindow::onMemoryEdited);

    lay->addWidget(memTable_);
    return box;
}

QWidget* MainWindow::buildRegisterPanel()
{
    auto* panel = new QWidget(this);
    auto* lay = new QVBoxLayout(panel);
    lay->setContentsMargins(0, 0, 0, 0);

    // --- CPU box ---
    auto* cpuBox = new QGroupBox("CPU", panel);
    auto* grid = new QGridLayout(cpuBox);

    QFont big = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    big.setPointSize(big.pointSize() + 5);
    big.setBold(true);

    pcLabel_ = new QLabel("00", cpuBox);
    pcLabel_->setObjectName("pcLabel");
    pcLabel_->setFont(big);
    pcLabel_->setToolTip("Program counter: address of the next instruction");

    irLabel_ = new QLabel("0000", cpuBox);
    irLabel_->setObjectName("irLabel");
    irLabel_->setFont(big);
    irLabel_->setToolTip("Instruction register: the instruction that was fetched last");

    nextLabel_ = new QLabel("-", cpuBox);
    nextLabel_->setObjectName("nextLabel");
    nextLabel_->setWordWrap(true);

    stepsLabel_ = new QLabel("0", cpuBox);
    stepsLabel_->setObjectName("stepsLabel");

    grid->addWidget(new QLabel("PC", cpuBox), 0, 0);
    grid->addWidget(pcLabel_, 0, 1);
    grid->addWidget(new QLabel("IR", cpuBox), 0, 2);
    grid->addWidget(irLabel_, 0, 3);
    grid->addWidget(new QLabel("Next:", cpuBox), 1, 0);
    grid->addWidget(nextLabel_, 1, 1, 1, 3);
    grid->addWidget(new QLabel("Steps run:", cpuBox), 2, 0);
    grid->addWidget(stepsLabel_, 2, 1, 1, 3);
    grid->setColumnStretch(1, 1);
    grid->setColumnStretch(3, 1);

    // --- registers ---
    auto* regBox = new QGroupBox("Registers  (double-click Hex to change)", panel);
    auto* regLay = new QVBoxLayout(regBox);

    regTable_ = new QTableWidget(16, 3, regBox);
    regTable_->setObjectName("regTable");
    regTable_->setHorizontalHeaderLabels({ "Hex", "Decimal", "Binary" });
    QStringList names;
    for (int i = 0; i < 16; ++i)
        names << regName(i);
    regTable_->setVerticalHeaderLabels(names);
    regTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    regTable_->verticalHeader()->setDefaultSectionSize(24);
    regTable_->setSelectionMode(QAbstractItemView::SingleSelection);
    regTable_->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    for (int i = 0; i < 16; ++i)
    {
        regTable_->setItem(i, 0, makeItem("00", Qt::AlignCenter, true));
        regTable_->setItem(i, 1, makeItem("0", Qt::AlignCenter, false));
        regTable_->setItem(i, 2, makeItem("0000 0000", Qt::AlignCenter, false));
    }
    connect(regTable_, &QTableWidget::itemChanged, this, &MainWindow::onRegisterEdited);

    regLay->addWidget(regTable_);

    lay->addWidget(cpuBox);
    lay->addWidget(regBox, 1);
    return panel;
}

QWidget* MainWindow::buildBottomPanel()
{
    auto* split = new QSplitter(Qt::Horizontal, this);

    auto* screenBox = new QGroupBox("Screen  (what the program stores to address FF)", split);
    auto* screenLay = new QVBoxLayout(screenBox);
    screenView_ = new QPlainTextEdit(screenBox);
    screenView_->setObjectName("screenView");
    screenView_->setReadOnly(true);
    screenView_->setLineWrapMode(QPlainTextEdit::NoWrap);
    screenView_->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    screenLay->addWidget(screenView_);

    auto* logBox = new QGroupBox("Messages", split);
    auto* logLay = new QVBoxLayout(logBox);
    logView_ = new QPlainTextEdit(logBox);
    logView_->setObjectName("logView");
    logView_->setReadOnly(true);
    logView_->setMaximumBlockCount(500);
    logLay->addWidget(logView_);

    split->addWidget(screenBox);
    split->addWidget(logBox);
    split->setStretchFactor(0, 1);
    split->setStretchFactor(1, 1);
    split->setChildrenCollapsible(false);
    return split;
}

// ---------------------------------------------------------------------------
// MainWindow - slots
// ---------------------------------------------------------------------------

void MainWindow::chooseFile()
{
    const QString path = QFileDialog::getOpenFileName(
        this, "Open Vole program", lastDir_, "Text files (*.txt);;All files (*)");
    if (!path.isEmpty())
        openProgram(path);
}

void MainWindow::openProgram(const QString& path)
{
    currentFile_ = path;
    lastDir_ = QFileInfo(path).absolutePath();
    fileLabel_->setText(QFileInfo(path).fileName());
    fileLabel_->setToolTip(path);
    fileLabel_->setStyleSheet(QString());
    loadProgram();
}

void MainWindow::loadProgram()
{
    if (currentFile_.isEmpty())
    {
        banner_->setText("Choose a program file first: click \"Open program...\".");
        banner_->setStyleSheet("QLabel{background:#FDE3E1;color:#6B1410;border-radius:6px;padding:8px 12px;}");
        return;
    }

    stopTimer();
    if (state_ == State::Running)
        setState(State::Paused);

    const int address = startSpin_->value();

    // The core reports its problems with cout. A GUI program has no console,
    // so we catch that text and show it in the Messages panel instead.
    std::ostringstream captured;
    std::streambuf* oldBuf = std::cout.rdbuf(captured.rdbuf());
    bool ok = false;
    std::string failure;
    try
    {
        ok = machine_.loadProgram(currentFile_.toLocal8Bit().toStdString(), address);
    }
    catch (const std::exception& e)
    {
        failure = e.what();
    }
    std::cout.rdbuf(oldBuf);

    logCaptured(captured.str());

    if (!ok)
    {
        QString why = cleanCoreMessage(QString::fromStdString(captured.str()));
        if (why.isEmpty())
            why = cleanCoreMessage(QString::fromStdString(failure));
        if (why.isEmpty())
            why = "unknown error";

        log(QString("Could not load \"%1\": %2").arg(QFileInfo(currentFile_).fileName(), why));
        banner_->setText(QString("Could not load the program: %1").arg(why));
        banner_->setStyleSheet("QLabel{background:#FDE3E1;color:#6B1410;border-radius:6px;padding:8px 12px;}");
        updateButtons();
        return;
    }

    startAddress_ = address;
    steps_ = 0;
    lastExecuted_ = -1;
    log(QString("Loaded \"%1\" at address %2.").arg(QFileInfo(currentFile_).fileName(), hex2(address)));
    setState(State::Ready);
    refresh(false);
}

void MainWindow::stepOnce()
{
    doStep();
}

void MainWindow::toggleRun()
{
    if (state_ == State::Running)
    {
        stopTimer();
        setState(State::Paused);
        return;
    }

    if (state_ != State::Ready && state_ != State::Paused)
        return;

    setState(State::Running);
    timer_->start();
}

void MainWindow::restartProgram()
{
    loadProgram();
}

void MainWindow::clearMachine()
{
    stopTimer();
    machine_.reset(0);
    steps_ = 0;
    lastExecuted_ = -1;
    log("Machine cleared.");
    setState(State::NoProgram, "Machine cleared. Press Load to load the program again, or open another one.");
    refresh(false);
}

void MainWindow::onTick()
{
    if (!doStep())
        stopTimer();
}

void MainWindow::onSpeedChanged(int level)
{
    const int interval = kIntervals[qBound(1, level, 8) - 1];
    timer_->setInterval(interval);
    speedLabel_->setText(QString("%1 steps/s").arg(1000 / interval));
}

void MainWindow::onMemoryEdited(QTableWidgetItem* item)
{
    if (updating_ || !item)
        return;

    const int address = item->row() * 16 + item->column();
    if (address == 255)
        return;

    int value = 0;
    if (!parseHexByte(item->text(), value))
    {
        log(QString("Memory %1: type 1 or 2 hex digits (00 to FF).").arg(hex2(address)));
        refresh(false);
        return;
    }

    machine_.setMemory(address, static_cast<uint8_t>(value));
    log(QString("Memory %1 set to %2.").arg(hex2(address), hex2(value)));
    refresh(true);
    if (state_ == State::Paused || state_ == State::Ready)
        setState(state_);
}

void MainWindow::onRegisterEdited(QTableWidgetItem* item)
{
    if (updating_ || !item || item->column() != 0)
        return;

    int value = 0;
    if (!parseHexByte(item->text(), value))
    {
        log(QString("%1: type 1 or 2 hex digits (00 to FF).").arg(regName(item->row())));
        refresh(false);
        return;
    }

    machine_.setRegister(item->row(), static_cast<uint8_t>(value));
    log(QString("%1 set to %2.").arg(regName(item->row()), hex2(value)));
    refresh(true);
}

// ---------------------------------------------------------------------------
// MainWindow - behaviour
// ---------------------------------------------------------------------------

bool MainWindow::doStep()
{
    if (state_ != State::Ready && state_ != State::Paused && state_ != State::Running)
        return false;

    const int pcBefore = machine_.getPC();

    std::ostringstream captured;
    std::streambuf* oldBuf = std::cout.rdbuf(captured.rdbuf());
    bool ok = false;
    std::string failure;
    try
    {
        ok = machine_.runStep();
    }
    catch (const std::exception& e)
    {
        failure = e.what();
    }
    std::cout.rdbuf(oldBuf);

    const std::string text = captured.str();
    logCaptured(text);

    // The PC moves only when an instruction was really fetched.
    if (machine_.getPC() != pcBefore)
    {
        lastExecuted_ = pcBefore;
        ++steps_;
    }

    const bool halted = machine_.isHalted();

    if (!ok)
    {
        QString why = cleanCoreMessage(QString::fromStdString(text));
        if (why.isEmpty())
            why = cleanCoreMessage(QString::fromStdString(failure));
        if (why.isEmpty())
            setState(State::Halted);
        else
            setState(State::Error, why);
        refresh(true);
        return false;
    }

    if (halted)
    {
        setState(State::Halted);
        refresh(true);
        return false;
    }

    if (state_ != State::Running)
        setState(State::Paused);
    refresh(true);
    return true;
}

void MainWindow::stopTimer()
{
    if (timer_)
        timer_->stop();
}

void MainWindow::setState(State s, const QString& detail)
{
    state_ = s;
    if (s != State::Running)
        stopTimer();

    QString text;
    QString bg = "#E3F0FF";
    QString fg = "#0B2A55";

    // Text of the next instruction, used in a few messages.
    QString next;
    {
        const int pc = machine_.getPC();
        if (pc < 255)
            next = describe(static_cast<uint16_t>((machine_.getMemory(pc) << 8) | machine_.getMemory(pc + 1)));
    }

    switch (s)
    {
    case State::NoProgram:
        bg = "#EEF0F3";
        fg = "#2B2F36";
        text = detail.isEmpty()
                   ? "Welcome!  1) Click \"Open program...\" and pick a Vole program (.txt).   "
                     "2) Press Step to run one instruction, or Run to run them all."
                   : detail;
        break;
    case State::Ready:
        text = QString("Program loaded at address %1. Press Step to run one instruction, "
                       "or Run to run them all.").arg(hex2(startAddress_));
        break;
    case State::Running:
        text = "Running...  press Pause to stop at any moment.";
        break;
    case State::Paused:
        text = QString("Paused after %1 step(s).  Next instruction: %2").arg(steps_).arg(next);
        break;
    case State::Halted:
        bg = "#E2F5E4";
        fg = "#14401B";
        text = QString("Finished: the program reached HALT after %1 step(s). "
                       "Press Restart to run it again.").arg(steps_);
        break;
    case State::Error:
        bg = "#FDE3E1";
        fg = "#6B1410";
        text = QString("Stopped by an error: %1  Press Restart to try again.").arg(detail);
        break;
    }

    banner_->setText(text);
    banner_->setStyleSheet(QString("QLabel{background:%1;color:%2;border-radius:6px;padding:8px 12px;}")
                               .arg(bg, fg));
    updateButtons();
}

void MainWindow::updateButtons()
{
    const bool running = (state_ == State::Running);
    const bool canGo = (state_ == State::Ready || state_ == State::Paused);

    openBtn_->setEnabled(!running);
    loadBtn_->setEnabled(!running && !currentFile_.isEmpty());
    startSpin_->setEnabled(!running);
    stepBtn_->setEnabled(canGo);
    runBtn_->setEnabled(canGo || running);
    runBtn_->setText(running ? "Pause" : "Run");
    restartBtn_->setEnabled(!currentFile_.isEmpty());
    clearBtn_->setEnabled(true);

    const auto triggers = running
                              ? QAbstractItemView::NoEditTriggers
                              : (QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    memTable_->setEditTriggers(triggers);
    regTable_->setEditTriggers(triggers);
}

// Reads everything from the Machine and redraws all panels.
void MainWindow::refresh(bool highlightChanges)
{
    updating_ = true;

    const int pc = machine_.getPC();
    // The blue "next instruction" mark only makes sense while the program can still run.
    const bool active = (state_ == State::Ready || state_ == State::Running || state_ == State::Paused);

    // --- memory grid ---
    std::array<uint8_t, 256> mem{};
    for (int a = 0; a < 256; ++a)
    {
        mem[a] = machine_.getMemory(a);

        QTableWidgetItem* item = memTable_->item(a / 16, a % 16);
        setTextIfChanged(item, hex2(mem[a]));

        int kind = KNormal;
        if (active && (a == pc || a == pc + 1))
            kind = KNext;
        else if (highlightChanges && mem[a] != prevMem_[a])
            kind = KChanged;
        else if (lastExecuted_ >= 0 && (a == lastExecuted_ || a == lastExecuted_ + 1))
            kind = KLast;
        styleItem(item, kind);
    }

    // --- instruction list ---
    const int nextRow = pc / 2;
    const int lastRow = lastExecuted_ >= 0 ? lastExecuted_ / 2 : -1;
    for (int i = 0; i < 128; ++i)
    {
        const uint16_t word = static_cast<uint16_t>((mem[i * 2] << 8) | mem[i * 2 + 1]);
        setTextIfChanged(insnTable_->item(i, 1), hex4(word));
        setTextIfChanged(insnTable_->item(i, 2), describe(word));

        int kind = KNormal;
        if (active && i == nextRow)
            kind = KNext;
        else if (i == lastRow)
            kind = KLast;
        for (int c = 0; c < 3; ++c)
            styleItem(insnTable_->item(i, c), kind);
    }
    if (active && nextRow < 128)
        insnTable_->scrollToItem(insnTable_->item(nextRow, 0), QAbstractItemView::EnsureVisible);

    // --- registers ---
    std::array<uint8_t, 16> reg{};
    for (int i = 0; i < 16; ++i)
    {
        reg[i] = machine_.getRegister(i);
        setTextIfChanged(regTable_->item(i, 0), hex2(reg[i]));
        setTextIfChanged(regTable_->item(i, 1), QString::number(reg[i]));
        setTextIfChanged(regTable_->item(i, 2), binary8(reg[i]));

        const int kind = (highlightChanges && reg[i] != prevReg_[i]) ? KChanged : KNormal;
        for (int c = 0; c < 3; ++c)
            styleItem(regTable_->item(i, c), kind);
    }

    // --- CPU box ---
    pcLabel_->setText(hex2(pc));
    irLabel_->setText(hex4(machine_.getIR()));
    if (!active)
        nextLabel_->setText("-");
    else if (pc < 255)
        nextLabel_->setText(describe(static_cast<uint16_t>((mem[pc] << 8) | mem[pc + 1])));
    else
        nextLabel_->setText("(PC is outside program memory)");
    stepsLabel_->setText(QString::number(steps_));

    // --- screen ---
    const QString raw = QString::fromStdString(machine_.getScreenOutput()).trimmed();
    if (raw.isEmpty())
    {
        screenView_->setPlainText("(nothing written yet)");
    }
    else
    {
        QString values, chars;
        const QStringList tokens = raw.split(' ', Qt::SkipEmptyParts);
        for (const QString& tok : tokens)
        {
            const int v = tok.toInt(nullptr, 16);
            values += tok.rightJustified(2, '0') + "  ";
            chars += (v >= 32 && v < 127 ? QString(QChar(v)) : QString('.')) + "   ";
        }
        screenView_->setPlainText("Values (hex): " + values.trimmed() + "\nAs text:      " + chars.trimmed());
    }

    prevMem_ = mem;
    prevReg_ = reg;
    updating_ = false;
}

void MainWindow::log(const QString& text)
{
    logView_->appendPlainText(text);
}

void MainWindow::logCaptured(const std::string& text)
{
    const QStringList lines = QString::fromStdString(text).split('\n', Qt::SkipEmptyParts);
    for (const QString& line : lines)
    {
        const QString t = line.trimmed();
        if (!t.isEmpty())
            log(t);
    }
}

// Plain-English meaning of one 16-bit instruction word.
// (Kept in the GUI on purpose: it only explains instructions to the user and
// does not run them. The real work is done by the CPU class.)
QString MainWindow::describe(uint16_t w)
{
    const int op = (w >> 12) & 0xF;
    const int r = (w >> 8) & 0xF;
    const int x = (w >> 4) & 0xF;
    const int y = w & 0xF;
    const int xy = w & 0xFF;

    if (w == 0)
        return QString();

    const QString bad = QString("INVALID instruction");

    switch (op)
    {
    case 0x1:
        return QString("LOAD   %1 <- memory[%2]").arg(regName(r), hex2(xy));
    case 0x2:
        return QString("LOAD   %1 <- the value %2").arg(regName(r), hex2(xy));
    case 0x3:
        if (xy == 0xFF)
            return QString("STORE  %1 -> SCREEN").arg(regName(r));
        return QString("STORE  %1 -> memory[%2]").arg(regName(r), hex2(xy));
    case 0x4:
        if (r != 0)
            return bad;
        return QString("MOVE   %1 -> %2").arg(regName(x), regName(y));
    case 0x5:
        return QString("ADD    %1 = %2 + %3  (integers)").arg(regName(r), regName(x), regName(y));
    case 0x6:
        return QString("ADD    %1 = %2 + %3  (floats)").arg(regName(r), regName(x), regName(y));
    case 0x7:
        return QString("OR     %1 = %2 OR %3").arg(regName(r), regName(x), regName(y));
    case 0x8:
        return QString("AND    %1 = %2 AND %3").arg(regName(r), regName(x), regName(y));
    case 0x9:
        return QString("XOR    %1 = %2 XOR %3").arg(regName(r), regName(x), regName(y));
    case 0xA:
        if (x != 0)
            return bad;
        return QString("ROTATE %1 right by %2 bit(s)").arg(regName(r)).arg(y);
    case 0xB:
        return QString("JUMP   to address %1 if %2 = R0").arg(hex2(xy), regName(r));
    case 0xC:
        if (w != 0xC000)
            return bad;
        return QString("HALT");
    default:
        return bad;
    }
}
