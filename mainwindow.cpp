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
  toggleAllElectrodes();
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
