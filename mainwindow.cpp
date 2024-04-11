#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , isDeviceOn(false)
{
    ui->setupUi(this);
    connectAllElectrodes();
    for (int i = 1; i < 22; ++i) {
    electrodes.emplace_back(Electrode("Beta", 0));
    }
    countdownTimer = new QTimer(this);
    connect(countdownTimer, &QTimer::timeout, this, &MainWindow::updateCountdown);

    ui->stackedFrames->setCurrentIndex(0);  // set device to an empty screen when device off
}

MainWindow::~MainWindow()
{
  delete ui;
}


void MainWindow::toggleAllElectrodes(){
    for (int i = 1; i < 22; ++i) {
        QString buttonName = QString("electrodeButton_%1").arg(i);
        QPushButton *button = MainWindow::findChild<QPushButton *>(buttonName);
        int electrodeIndex = button->text().remove(0,1).toInt() - 1;

        if(button){
           activateElectrode(button);
           electrodes[electrodeIndex].switchIsActive();
        }
      }
}


void MainWindow::connectAllElectrodes(){
    for (int i = 1; i < 22; ++i) {
        QString buttonName = QString("electrodeButton_%1").arg(i);
        QPushButton *button = MainWindow::findChild<QPushButton *>(buttonName);
        connect(button, &QPushButton::clicked, this, &MainWindow::onMultipleButtonsClicked);
 }
}

// demonstrate the effect of contact lost by clicking any electrode in GUI
 void MainWindow::onMultipleButtonsClicked(){
     QPushButton *button = qobject_cast<QPushButton *>(sender());
     int electrodeIndex = button->text().remove(0,1).toInt() - 1;
     electrodes[electrodeIndex].switchIsActive();
     bool isElectrodeActive = electrodes[electrodeIndex].getIsActive();
     if(isElectrodeActive){
     activateElectrode(button);
    }
     else{
      deactivateElectrode(button);
     }
 }


void MainWindow::on_powerButton_clicked(){
  isDeviceOn = !isDeviceOn;
  if(isDeviceOn){
  ui->stackedFrames->setCurrentIndex(1); // menu screen
  }
  else{
  ui->stackedFrames->setCurrentIndex(0); // empty screen
 }
}


// change electrode color to green
void MainWindow::activateElectrode(QPushButton *button){
    button->setStyleSheet("QPushButton {"
                        "border-radius: 17px;"
                        "border: 1px solid black;"
                        "background-color: green; "
                        "color: white; "
                        "}");
}

// change electrode color to red
void MainWindow::deactivateElectrode(QPushButton *button){
    button->setStyleSheet("QPushButton {"
                        "border-radius: 17px;"
                        "border: 1px solid black;"
                        "background-color: red; "
                        "color: white; "
                        "}");
}

void MainWindow::on_newSessionButton_clicked(){
    ui->stackedFrames->setCurrentIndex(2);
    countdownTimer->start(1000);
    ui->progressBar->setMaximum(countdownTime);
    ui->progressBar->setValue(0);
    toggleAllElectrodes();

}



void MainWindow::updateCountdown()
{
    countdownTime--;

    ui->progressBar->setValue(180 - countdownTime);

    // Convert seconds to minutes and seconds
    int minutes = countdownTime / 60;
    int seconds = countdownTime % 60;

    // Example: Update a label with the remaining time
    // Ensure you have a QLabel named timeLabel in your UI
    ui->timeLabel->setText(QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0')));

    if (countdownTime <= 0) {
        countdownTimer->stop(); // Stop the timer
        countdownTime = 180;
        ui->progressBar->reset(); // To reset the progress bar
        // Optionally, perform any action after countdown ends
        std::cout<<"jafllfa"<<endl;
    }
}

