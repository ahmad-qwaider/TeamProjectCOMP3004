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
    electrodes.append(Electrode());
    }
    batteryTimer = new QTimer(this);
    displayTimer = new QTimer(this);
    connect(batteryTimer, &QTimer::timeout, this, &MainWindow::updateBatteryCapacity);

    for (int i = 1; i <= 21; ++i) {
        ui->electrodeSelector->addItem(QString("Electrode %1").arg(i));
    }
    connect(ui->electrodeSelector, SIGNAL(currentIndexChanged(int)), this, SLOT(updateDisplayWave()));

    dateTimeHolder = ui->dateTimeEdit->dateTime(); // set in the default value
    ui->stackedFrames->setCurrentIndex(0);  // set device to an empty screen when device off
    ui->stackedWidgetPC->setCurrentIndex(0); // show menu
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::updateDisplayWave() {
    int electrodeIndex = ui->electrodeSelector->currentIndex();
    displayWaveform(electrodeIndex);
}



void MainWindow::displayWaveform(int electrodeIndex) {
    if (electrodeIndex < 0 || electrodeIndex >= electrodes.size()) return;
    Electrode &electrode = electrodes[electrodeIndex];

    QVector<double> waveform = electrode.getWaveData(1, 256);
    cout<<electrodeIndex<<endl;

    QLineSeries *series = new QLineSeries();
    for (int i = 0; i < waveform.size(); ++i) {
        series->append(i / 256.0, waveform[i]);
        std::cout << "Waveform data at index " << i << ": " << waveform[i] << std::endl;  // Print each data point

    }

    QChart *chart = chartView->chart();
    chart->removeAllSeries();
    chart->addSeries(series);
    chart->createDefaultAxes();
}



void MainWindow::toggleAllElectrodes(bool activate){

    if(activate){
       toggleBlueLight(true);
    }
    else{
       contactLossTracker = 0; // cause all electrodes will be deactivated
       toggleBlueLight(false);
    }

    for (int i = 1; i < 22; ++i) {
        QString buttonName = QString("electrodeButton_%1").arg(i);
        QPushButton *button = MainWindow::findChild<QPushButton *>(buttonName);
        int electrodeIndex = button->text().remove(0,1).toInt() - 1;        

        if(button){
           if(activate){
           activateElectrode(button);
           }
           else{
               deactivateElectrode(button);
           }
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

    if(!isSessionRunning){ // if no session is running you cannot connect or disconnect the electrodes
        return;
    }

     QPushButton *button = qobject_cast<QPushButton *>(sender());
     int electrodeIndex = button->text().remove(0,1).toInt() - 1;
     electrodes[electrodeIndex].switchIsActive();
     bool isElectrodeActive = electrodes[electrodeIndex].getIsActive();

     if(isElectrodeActive){
     activateElectrode(button);
     contactLossTracker--;
     toggleRedLight(false);
      if((contactLossTracker == 0) && !(currentSession==nullptr)){ // am i reconnect? is the session still there or has it been nuked
         currentSession->getProgressTimer()->start();
         currentSession->resumeTimers();
         toggleBlueLight(true);
      }

    }
     else{
      toggleRedLight(true);
      deactivateElectrode(button);
      contactLossTracker++;
      if(killTimerSent== false){ // having issues with spamming connect and unconnect * edit i think this fixed it issue was multiple signal overlapping
        interuptionProtocol(); // trigger the exuction protocol timer
      }
      toggleBlueLight(false);
     }
 }


void MainWindow::on_powerButton_clicked(){
  isDeviceOn = !isDeviceOn;
  if(isDeviceOn){
   showMainScreen(); // menu screen
   batteryTimer->start(batteryTime);
   ui->Battery->setText(QString::number(batteryPercentage) + "%");

  }
  else{
   if(currentSession!=nullptr && isSessionRunning){
       terminateSession(false);
   }
   ui->stackedFrames->setCurrentIndex(0); // empty screen
   ui->Battery->setText("");
   toggleBlueLight(false);
   toggleGreenLight(false);
   toggleRedLight(false);
   batteryTimer->stop();
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
    if(!(batteryPercentage<40) && contactLossTracker == 0){ // need enough batter to start, low battery is at 10% so when it hit 10% we wipe, dont let them start incase it will hit it
        toggleAllElectrodes(true);
        ui->stackedFrames->setCurrentIndex(2); // switch to session progress screen
        intitializeSession(); // we bind a new therapy object to the current session option
        sessionDuration = currentSession->getSessionLength()/1000;  // convert ms to seconds
        countdownTime = sessionDuration;
        connect(currentSession->getProgressTimer(), &QTimer::timeout, this, &MainWindow::updateCountdown); // connect
        connect(currentSession->getElectrodeTimer(), &QTimer::timeout, this, &MainWindow::GreenlightBlinking);
        currentSession->getProgressTimer()->start(1000); // start progress timer for countdown
        currentSession->initializeEventTimers(); // start main event loop in session
        batteryTimer->start(batteryTime/3);
        ui->progressBar->setMaximum(countdownTime);
        ui->progressBar->setValue(0);
    }
}



void MainWindow::updateCountdown()
{
    if(!(batteryPercentage<40)){ // is battery low? is session paused? we need to fix this portion and have lowbatter occur at like 10 percent with warning..
        countdownTime--;
        ui->progressBar->setValue(sessionDuration - countdownTime);

        // Convert seconds to minutes and seconds
        int minutes = countdownTime / 60;
        int seconds = countdownTime % 60;


        ui->timeLabel->setText(QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0')));

        if (currentSession->getState() == SessionState::COMPLETE && currentSession->getProgressTimer()->isActive()) {
            currentSession->getProgressTimer()->stop(); // Stop the progress timer in session
            currentSession->getProgressTimer()->disconnect(); // disconnect
            currentSession->endSession(); // stop event timers inside session (they are disonnected inside)
            //countdownTime = sessionDuration;
            isSessionRunning = false;
            sessionsData.append(currentSession->generateSessionData()); // append the new sesssion data to the session log console
            appendToSessionLogConsole(); // append the new sesssion data to the session log console
            currentSession = nullptr; // reset session
            showMainScreen(); // back to main menu
            batteryTimer->start(batteryTime); //  battery back to nromal rate
            ui->timeLabel->setText(QString("00:29")); // so it starts at 00:29 for the next session
            toggleAllElectrodes(false);
            toggleBlueLight(false);

        }
    }else{
        // fornow it will isnta stop and return main menu we should probbaly add not saying crash due to low battery and then put it back to main
        terminateSession(false);
        ui->progressBar->reset(); // To reset the progress bar
        showLowBatteryScreen();

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
        ui->greenLight->setStyleSheet("background-color: rgb(47, 202, 9)");
   }
    else{
        ui->greenLight->setStyleSheet("background-color: rgb(245, 249, 236)");
    }
}

void MainWindow::GreenlightBlinking(){
    blinkingImmitation =  !blinkingImmitation;
    toggleGreenLight(blinkingImmitation);
}

void MainWindow::on_TimeAndDateButton_clicked()
{
    if(isDeviceOn == true){
        ui->stackedFrames->setCurrentIndex(4);
    }
}


void MainWindow::on_EnterTimeButton_clicked()
{
    showMainScreen();
    dateTimeHolder = ui->dateTimeEdit->dateTime();
}


void MainWindow::on_MenuButton_clicked()
{
  if(isDeviceOn == true){
    if(isSessionRunning){
        return;
 //     terminateSession(); I DONT THINK WE SHOULD TERMINATE HERE
    }
    showMainScreen();
  }
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
    // is it on? we dont wanna just click play for no reason
    if((isDeviceOn==true) && (contactLossTracker==0)){
        // check has this currentSession been initialized yet. ie was a session in progress?
        if(!(currentSession==nullptr)){
            // is the timer running? if not then start it
            if(!(currentSession->getProgressTimer()->isActive())){
                currentSession->getProgressTimer()->start();
                currentSession->resumeTimers();
            }
        }
    }
}


void MainWindow::on_PauseButton_clicked()
{
    if(killTimerSent== false){
        interuptionProtocol();
    }

}

void MainWindow::interuptionProtocol(){
    // check has this currentSession been initialized yet. ie was a session in progress?
    if(!(currentSession==nullptr) && currentSession->getProgressTimer()->isActive()){
        currentSession->getProgressTimer()->stop();
        currentSession->pauseTimers();
        killTimerSent = true;
        // singl shot QTimer via lamda expression to send a single signal after x ms. this will terminate if session not active in time. // keep at 5000ms for testing
        // change to 5min for final draft.
        QTimer::singleShot(5000, this, [=](){
            if(currentSession!= nullptr && !currentSession->isActive()){
                terminateSession(true);
            }
            killTimerSent = false; // allow this signal to be sent again(prevents redundant calls)
        });

    }
}


void MainWindow::on_sessionLogButton_clicked()
{
    ui->stackedFrames->setCurrentIndex(3);
}

void MainWindow::intitializeSession(){
    // CHANGE the 2 TIMES HERE TO ADJUST SESSION TIME WHEN TESTING
    currentSession = new Session(dateTimeHolder, 5000, 1000, electrodes); // takes in datetime, baselinetime, treatment time per electrode, QVector of electrodes.
    isSessionRunning = true;

}

void MainWindow::terminateSession(bool turnOfDevice){

    if(!isSessionRunning){
        return;
    }

    qDebug() << "terminating session";
    if(currentSession->getProgressTimer()->isActive()){
    currentSession->getProgressTimer()->stop();
    }
    currentSession->endSession();
    currentSession->getProgressTimer()->disconnect();
    isSessionRunning = false;
    currentSession = nullptr;
    ui->timeLabel->setText(QString("00:29")); // so it starts at 00:29 for the next session
    toggleAllElectrodes(false);
    toggleRedLight(false);
    if(isDeviceOn == true){
        if(turnOfDevice){
        on_powerButton_clicked(); // turn of the device
        }
        else{
         batteryTimer->start(batteryTime); // DONT FORGET TO REMOVE the spec mention when session termminates device should turn of  automaically
        }
    }

}

void MainWindow::appendToSessionLogConsole(){
  completedSessionsCount++;
  ui->SessionLogConsole->append(QString("Session " + QString::number(completedSessionsCount) + ": \nDate and Time: %1\n")
                                  .arg(dateTimeHolder.toString("yyyy-MM-dd hh:mm:ss")));
}


void MainWindow::on_UploadToPCButton_clicked()
{
    ui->PCLogConsole->clear();
    int sessionIndex  = 0;

      for (const auto& session : sessionsData) {
          sessionIndex++;
          ui->PCLogConsole->append(session.toString(sessionIndex) + "\n");
      }
}

void MainWindow::showLowBatteryScreen() {
    ui->stackedFrames->setCurrentIndex(5); // Switch to low battery screen
    QTimer::singleShot(5000, this, &MainWindow::showMainScreen); // Wait for 3 seconds before running returnToMainScreen
}

void MainWindow::showMainScreen() {
    ui->stackedFrames->setCurrentIndex(1); // Switch back to the main screen
}

void MainWindow::on_StopButton_clicked()
{
    if(isDeviceOn == true){
      if(isSessionRunning){
        terminateSession(false);
      }
      showMainScreen();
    }
}

void MainWindow::on_SessionLogButtonPC_clicked()
{
    ui->stackedWidgetPC->setCurrentIndex(1); // navigat to session log  screen
}


void MainWindow::on_GraphButtonPC_clicked()
{
   ui->stackedWidgetPC->setCurrentIndex(2); // navigate to graph screen
}

void MainWindow::on_BackButton_clicked()
{
    ui->stackedWidgetPC->setCurrentIndex(0); // Back to menu
}

void MainWindow::on_BackButton_2_clicked()
{
    ui->stackedWidgetPC->setCurrentIndex(0); // Back to menu
}



