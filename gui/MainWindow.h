// Part 3 (GUI) - Vole Machine Simulator
// Qt Widgets front end for the team's Machine class.
//
// The GUI never touches Memory / CPU / Registers directly. It only talks to
// Machine, through the public functions listed in CORE_GETTERS.txt.

#pragma once

#include <QMainWindow>
#include <QSpinBox>
#include <QString>
#include <array>
#include <cstdint>

#include "Machine.h"

class QTableWidget;
class QTableWidgetItem;
class QPlainTextEdit;
class QLabel;
class QPushButton;
class QSlider;
class QTimer;

// Spin box that shows its value as two hex digits (00, 02, ... FE).
class HexSpinBox : public QSpinBox
{
public:
    explicit HexSpinBox(QWidget* parent = nullptr);

protected:
    QString textFromValue(int value) const override;
    int valueFromText(const QString& text) const override;
    QValidator::State validate(QString& input, int& pos) const override;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

    // Remember a program file and load it at the start address in the spin box.
    void openProgram(const QString& path);

private slots:
    void chooseFile();
    void loadProgram();
    void stepOnce();
    void toggleRun();
    void restartProgram();
    void clearMachine();
    void onTick();
    void onSpeedChanged(int level);
    void onMemoryEdited(QTableWidgetItem* item);
    void onRegisterEdited(QTableWidgetItem* item);

private:
    enum class State { NoProgram, Ready, Running, Paused, Halted, Error };

    // building the window
    void buildUi();
    QWidget* buildToolbar();
    QWidget* buildInstructionPanel();
    QWidget* buildMemoryPanel();
    QWidget* buildRegisterPanel();
    QWidget* buildBottomPanel();

    // behaviour
    bool doStep();                 // runs one instruction, returns true if it can continue
    void stopTimer();
    void setState(State s, const QString& detail = QString());
    void refresh(bool highlightChanges);
    void updateButtons();
    void log(const QString& text);
    void logCaptured(const std::string& text);

    static QString describe(uint16_t word);

    Machine machine_;

    State state_ = State::NoProgram;
    QString currentFile_;
    QString lastDir_;
    int startAddress_ = 0;
    int steps_ = 0;
    int lastExecuted_ = -1;        // address of the last instruction that was fetched
    bool updating_ = false;        // true while the code itself fills the tables

    std::array<uint8_t, 256> prevMem_{};
    std::array<uint8_t, 16> prevReg_{};

    // widgets
    QLabel* banner_ = nullptr;
    QLabel* fileLabel_ = nullptr;
    HexSpinBox* startSpin_ = nullptr;
    QPushButton* openBtn_ = nullptr;
    QPushButton* loadBtn_ = nullptr;
    QPushButton* stepBtn_ = nullptr;
    QPushButton* runBtn_ = nullptr;
    QPushButton* restartBtn_ = nullptr;
    QPushButton* clearBtn_ = nullptr;
    QSlider* speedSlider_ = nullptr;
    QLabel* speedLabel_ = nullptr;
    QTableWidget* insnTable_ = nullptr;
    QTableWidget* memTable_ = nullptr;
    QTableWidget* regTable_ = nullptr;
    QLabel* pcLabel_ = nullptr;
    QLabel* irLabel_ = nullptr;
    QLabel* nextLabel_ = nullptr;
    QLabel* stepsLabel_ = nullptr;
    QPlainTextEdit* screenView_ = nullptr;
    QPlainTextEdit* logView_ = nullptr;
    QTimer* timer_ = nullptr;
};
