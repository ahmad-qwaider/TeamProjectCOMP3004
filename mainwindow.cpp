#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , isDeviceOn(false)
    , isSessionRunning(false)
{
    ui->setupUi(this);
    QChart *chart = new QChart();
    chartView = new QChartView(chart, this);
    chartView->setRenderHint(QPainter::Antialiasing);
    ui->graphLayout->addWidget(chartView);
    connectAllElectrodes();
    for (int i = 0; i < 21; ++i) {
    electrodes.emplace_back(Electrode());
    }
    countdownTimer = new QTimer(this);
    batteryTimer = new QTimer(this);
    displayTimer = new QTimer(this);
    connect(countdownTimer, &QTimer::timeout, this, &MainWindow::updateCountdown);
    connect(batteryTimer, &QTimer::timeout, this, &MainWindow::updateBatteryCapacity);

    dateTimeHolder = ui->dateTimeEdit->dateTime(); // set in the default value
    ui->stackedFrames->setCurrentIndex(0);  // set device to an empty screen when device off
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::startWaveformDisplay() {
    connect(displayTimer, &QTimer::timeout, this, [this]() {
        static int electrodeIndex = 0;  // Example to cycle through electrodes
        displayWaveform(electrodeIndex);
        electrodeIndex = (electrodeIndex + 1) % electrodes.size();
    });
    displayTimer->start(1000);  // Update the waveform display every 500 ms
}


void MainWindow::displayWaveform(int electrodeIndex) {
    if (electrodeIndex < 0 || electrodeIndex >= electrodes.size()) return;
    Electrode &electrode = electrodes[electrodeIndex];

    QVector<double> waveform = electrode.getWaveData(1, 256);

    QLineSeries *series = new QLineSeries();
    for (int i = 0; i < waveform.size(); ++i) {
        series->append(i / 256.0, waveform[i]);
    }

    QChart *chart = chartView->chart();
    chart->removeAllSeries();
    chart->addSeries(series);
    chart->createDefaultAxes();
}



void MainWindow::toggleAllElectrodes(){
    for (int i = 1; i < 22; ++i) {
        QString buttonName = QString("electrodeButton_%1").arg(i);
        QPushButton *button = MainWindow::findChild<QPushButton *>(buttonName);
        int electrodeIndex = button->text().remove(0,1).toInt() - 1;
        toggleBlueLight(true);

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
     contactLossTracker--;
     toggleRedLight(false);

      if(contactLossTracker ==  0){
         countdownTimer->start(1000);
         toggleBlueLight(true);
      }

    }
     else{
      deactivateElectrode(button);
      contactLossTracker++;
      countdownTimer->stop();
      toggleRedLight(true);
      toggleBlueLight(false);
     }
 }


void MainWindow::on_powerButton_clicked(){
  isDeviceOn = !isDeviceOn;
  if(isDeviceOn){
   ui->stackedFrames->setCurrentIndex(1); // menu screen
   batteryTimer->start(batteryTime);
   ui->Battery->setText(QString::number(batteryPercentage) + "%");
   if(isSessionRunning){
      countdownTimer->start(1000);
  }

  }
  else{
   ui->stackedFrames->setCurrentIndex(0); // empty screen
   ui->Battery->setText("");
   toggleBlueLight(false);
   toggleGreenLight(false);
   toggleRedLight(false);
   batteryTimer->stop();
   countdownTimer->stop();
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
    isSessionRunning = true;
    countdownTimer->start(1000);
    batteryTimer->start(batteryTime/3);
    ui->progressBar->setMaximum(countdownTime);
    ui->progressBar->setValue(0);
    toggleAllElectrodes();
//    startWaveformDisplay();

}



void MainWindow::updateCountdown()
{
    countdownTime--;

    ui->progressBar->setValue(sessionDuration - countdownTime);
    std::cout<<countdownTime<<endl;

    // Convert seconds to minutes and seconds
    int minutes = countdownTime / 60;
    int seconds = countdownTime % 60;


    ui->timeLabel->setText(QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0')));

    if (countdownTime <= 0) {
        countdownTimer->stop(); // Stop the timer
        countdownTime = sessionDuration;
        isSessionRunning = false;
        sessionsData.emplace_back(dateTimeHolder, 100, 200);
        appendToSessionLogConsole(); // append the new sesssion data to the session log console
        ui->progressBar->reset(); // To reset the progress bar
    }
}

void MainWindow::toggleRedLight(bool turnON){
    if(turnON && isDeviceOn){
        ui->redLight->setStyleSheet("background-color: rgb(255, 40, 40)");
   }
    else if(contactLossTracker  == 0 || !isDeviceOn){
        ui->redLight->setStyleSheet("background-color: rgb(250, 230, 230)");
    }
}

void MainWindow::toggleBlueLight(bool turnON){
    if(turnON && isDeviceOn && contactLossTracker  == 0){
        ui->blueLight->setStyleSheet("background-color: rgb(31, 188, 255)");
   }
    else{
        ui->blueLight->setStyleSheet("background-color: rgb(220, 249, 255)");
    }
}

void MainWindow::toggleGreenLight(bool turnON){
    if(turnON && isDeviceOn && contactLossTracker  == 0){
        ui->greenLight->setStyleSheet("background-color: green");
   }
    else{
        ui->greenLight->setStyleSheet("background-color: rgb(245, 249, 236)");
    }
}

void MainWindow::on_TimeAndDateButton_clicked()
{
    ui->stackedFrames->setCurrentIndex(4);
}


void MainWindow::on_EnterTimeButton_clicked()
{
    ui->stackedFrames->setCurrentIndex(1);
    dateTimeHolder = ui->dateTimeEdit->dateTime();
 //   std::cout<<dateTimeHolder.toString().toStdString()<<endl;
}


void MainWindow::on_MenuButton_clicked()
{
  ui->stackedFrames->setCurrentIndex(1);
}


void MainWindow::updateBatteryCapacity()
{
    batteryCapacityTracker--;
    batteryPercentage = (batteryCapacityTracker * 100) / totalBatteryCapacity;
    ui->Battery->setText(QString::number(batteryPercentage) + "%");

    if (batteryCapacityTracker <= 0) {
        batteryTimer->stop(); // Stop the timer
    }
}

void MainWindow::on_PlayButton_clicked()
{
    countdownTimer->start(1000);

}


void MainWindow::on_PauseButton_clicked()
{
    countdownTimer->stop();
}


void MainWindow::on_sessionLogButton_clicked()
{
    ui->stackedFrames->setCurrentIndex(3);
}

void MainWindow::appendToSessionLogConsole(){
  ui->SessionLogConsole->append(QString("Date and Time: %1\n")
                                .arg(dateTimeHolder.toString("yyyy-MM-dd hh:mm:ss")));
}


void MainWindow::on_UploadToPCButton_clicked()
{
    ui->PCLogConsole->clear();

      for (const auto& session : sessionsData) {
          ui->PCLogConsole->append(session.toString() + "\n");
      }
}

