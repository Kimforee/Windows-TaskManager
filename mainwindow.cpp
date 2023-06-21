
// mainwindow.cpp
#include "mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QToolButton>
#include <QTabWidget>
#include <QMessageBox>
#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <Pdh.h>
#include <QTimer>
#include <iostream>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QDebug>
#include <comutil.h>
#include <Wbemidl.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <windows.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <qcustomplot.h>
using namespace std ;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    resize(1024, 700);
}
void MainWindow::setupUI()
{
    column  = 3;
    QTabWidget *tabWidget  = new QTabWidget(this);
    QWidget *centralWidget = new QWidget(this);
    QWidget *tab1 = new QWidget(this);
    QWidget *tab2 = new QWidget(this);
    QWidget *tab3 = new QWidget(this);

    setCentralWidget(centralWidget);

    Table = new QTableWidget(this);
    Table->setColumnCount(5);
    Table->setRowCount(1);

    timer = new QTimer(this);
    timer->setInterval(1000);
    timer->start();

    QTableWidgetItem *verticalHeaderItem = new QTableWidgetItem("CPU");
    Table->setVerticalHeaderItem(0,verticalHeaderItem);
    Table->verticalHeader  ()->setSectionResizeMode(QHeaderView::ResizeToContents);
    Table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    Table->verticalHeader  ()->resizeSection(0, 50);
    Table->horizontalHeader()->setVisible(false);
    Table->setFixedHeight(30);

    processTable = new QTableWidget(this);
    processTable->setColumnCount(8);
    processTable->setHorizontalHeaderLabels({"PID","Process Name","I/O Operation",
                                             "CPU Usage","CPU Time","Elapsed Time",
                                             "Memory Usage",
                                             "Private Usage"});
    processTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QPushButton *refreshButton  = new QPushButton("Refresh");
    QPushButton *terminateButton= new QPushButton("Terminate");
    QPushButton *boostButton    = new QPushButton("Boost");
    QPushButton *pauseButton    = new QPushButton("Stop");
    QToolButton *tabButton1     = new QToolButton();
    QToolButton *tabButton2     = new QToolButton();
    QToolButton *tabButton3     = new QToolButton();

    connect(refreshButton,  &QPushButton::clicked, this, &MainWindow::refreshProcesses);
    connect(terminateButton,&QPushButton::clicked, this, &MainWindow::terminateProcess);
    connect(pauseButton,    &QPushButton::clicked, this, &MainWindow::pausetimer);
    connect(boostButton,    &QPushButton::clicked, this, &MainWindow::BoostProcess);
    connect(processTable->horizontalHeader(), &QHeaderView::sectionClicked, this, &MainWindow::sortTableByColumn);
    connect(timer, SIGNAL(timeout()), this, SLOT(refreshProcesses()));
    connect(tabButton1, &QToolButton::clicked, [tabWidget]() {tabWidget->setCurrentIndex(0);});
    connect(tabButton2, &QToolButton::clicked, [tabWidget]() {tabWidget->setCurrentIndex(1);});
    connect(tabButton3, &QToolButton::clicked, [tabWidget]() {tabWidget->setCurrentIndex(2);});

    QHBoxLayout *navBar = new QHBoxLayout;
    navBar->addWidget(tabButton1);
    navBar->addWidget(tabButton2);
    navBar->addWidget(tabButton3);

    tabWidget->addTab(tab1, "Processes");
    tabWidget->addTab(tab2, "Performance");
    tabWidget->addTab(tab3, "Other");

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(refreshButton);
    buttonLayout->addWidget(terminateButton);
    buttonLayout->addWidget(boostButton);
    buttonLayout->addWidget(pauseButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addWidget(tabWidget);

    QVBoxLayout *tab1Layout = new QVBoxLayout(tab1);
    tab1Layout->addWidget(Table);
    tab1Layout->addWidget(processTable);
    tab1Layout->addLayout(buttonLayout);

    QVBoxLayout *tab2Layout = new QVBoxLayout(tab2);
    tab1Layout->addWidget(Table);
    tab1Layout->addWidget(processTable);
    tab1Layout->addLayout(buttonLayout);

    QVBoxLayout *tab3Layout = new QVBoxLayout(tab3);
    tab1Layout->addWidget(Table);
    tab1Layout->addWidget(processTable);
    tab1Layout->addLayout(buttonLayout);
//  setLayout(mainLayout);
}
double MainWindow::GetCpuUsage()
{
    PDH_STATUS status;
    // Open a query object
    PDH_HQUERY query;
    status = PdhOpenQuery(NULL, 0, &query);
    if (status != ERROR_SUCCESS)
    {
        // Error handling
        return -1.0;
    }

    // Add the counter to the query
    PDH_HCOUNTER counter;
    status = PdhAddCounter(query, L"\\Processor(_Total)\\% Processor Time", 0, &counter);
    if (status != ERROR_SUCCESS){
        PdhCloseQuery(query);
        return -1.0;
    }
    // Collect the query data
    status = PdhCollectQueryData(query);
    if (status != ERROR_SUCCESS){
        PdhCloseQuery(query);
        return -1.0;
    }
        Sleep(100);

    // Collect the query data again to get updated values
    status = PdhCollectQueryData(query);
    if (status != ERROR_SUCCESS){
        PdhCloseQuery(query);
        return -1.0;
    }

    // Get the formatted value of the counter
    PDH_FMT_COUNTERVALUE counterValue;
    status = PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, NULL, &counterValue);
    if (status != ERROR_SUCCESS){
        PdhCloseQuery(query);
        return -1.0;
    }

    // Close the query object
    PdhCloseQuery(query);
    return counterValue.doubleValue;
}

// Calculate overall RAM usage
void MainWindow::RAM()
{
    MEMORYSTATUSEX memoryStatus;
    memoryStatus.dwLength = sizeof(memoryStatus);
    GlobalMemoryStatusEx(&memoryStatus);
    // Total RAM
    QString totalMemory = "TOTAL RAM: " + QString::number
                         (static_cast<double>(memoryStatus.ullTotalPhys) / (1024.0 * 1024.0 * 1024.0), 'f', 2) + " GB";
    QTableWidgetItem* totalMemoryItem = new QTableWidgetItem(totalMemory);
    Table->setItem(0,2,totalMemoryItem);
    // RAM Available
    QString label = "AVAILABLE RAM: ";
    QTableWidgetItem* availableMemoryItem = new QTableWidgetItem
    (label+QString::number(static_cast<double>(memoryStatus.ullAvailPhys) / (1024 * 1024 * 1024),'f',2) + " GB");
    Table->setItem(0,3,availableMemoryItem);

}
void MainWindow::pausetimer()
{
    timer->stop();
}
void MainWindow::sortTableByColumn(int column)
{
    column  = column;
    processTable->sortItems(column, Qt::DescendingOrder);
    processTable->horizontalHeader()->setSortIndicator(column, Qt::DescendingOrder);
}
int MainWindow::refreshProcesses()
{
    processTable->clearContents();
    Table->clearContents();
    timer->start();
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);


    double cpuUsage = GetCpuUsage();
    if (cpuUsage >= 0)
    {
        // Display or use the CPU usage value
        qDebug() << "CPU Usage: " << cpuUsage << "%";
    }
    else
    {
        // Error occurred while retrieving CPU usage
        qDebug() << "Error retrieving CPU usage.";
    }
    QTableWidgetItem *cpuUsageItem = new QTableWidgetItem
        (QString::number(cpuUsage, 'f', 1) + "%");
    Table->setItem(0,1,cpuUsageItem);

    RAM();

    // Calculate total number of Processors
    int processorCount;
    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);
    processorCount = systemInfo.dwNumberOfProcessors;
    QTableWidgetItem *processor = new QTableWidgetItem
    (QString::number(processorCount, 'f', 1) + " Processors");
    Table->setItem(0,4,processor);

    // Get the memory usage for each process
    DWORD processes[1024], cbNeeded;
    if (EnumProcesses(processes, sizeof(processes), &cbNeeded))
    {
        DWORD processCount = cbNeeded / sizeof(DWORD);
        for (DWORD i = 0; i < processCount; i++)
        {
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processes[i]);
            if (hProcess != NULL)
            {
                PROCESS_MEMORY_COUNTERS_EX memoryInfo;
                if (GetProcessMemoryInfo(hProcess, (PROCESS_MEMORY_COUNTERS*)&memoryInfo, sizeof(memoryInfo)))
                {
                    QTableWidgetItem* memoryUsageItem = new QTableWidgetItem(QString::number(memoryInfo.WorkingSetSize /(1024 * 1024),'f',2) + " MB");
                    processTable->setItem(i, 6, memoryUsageItem);

                    QTableWidgetItem* privateUsageItem = new QTableWidgetItem(QString::number(memoryInfo.PrivateUsage / (1024 * 1024), 'f', 2) + " MB");
                    processTable->setItem(i, 7, privateUsageItem);

//                    QTableWidgetItem* pagefileUsageItem = new QTableWidgetItem(QString::number(memoryInfo.PagefileUsage / (1024 * 1024), 'f', 2) + " MB");
//                    processTable->setItem(i, 10, pagefileUsageItem);
                }
                CloseHandle(hProcess);
            }
        }
    }
    // Calculate CPU usage for each process
    // totalCPUTime = userTime + kernelTime
    // elapsedCPUTime = currentTime - createTime
    // cpuUsagePercentage = (totalCPUTime / elapsedCPUTime) * 100

    if (Process32First(hSnapshot, &pe32))
    {
        int row = 0;
        do {
            QTableWidgetItem *pidItem  = new QTableWidgetItem(QString::number(pe32.th32ProcessID));
            QTableWidgetItem *nameItem = new QTableWidgetItem(QString::fromWCharArray(pe32.szExeFile));
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
                              PROCESS_VM_READ, FALSE,
                              pe32.th32ProcessID);
            if (hProcess != NULL) {

                FILETIME initialTime;
                GetSystemTimeAsFileTime(&initialTime);
                FILETIME createTime = initialTime;
                FILETIME processKernelTime = initialTime;

                ULONGLONG prevKernelTime = 0;
                ULONGLONG prevUserTime = 0;
                bool initiater  = true;

                // CALCULATING TOTAL CPU TIME
                FILETIME exitTime, processUserTime;
                // Get process time
                GetProcessTimes(hProcess, &createTime, &exitTime,
                                &processKernelTime, &processUserTime);

                // GET THE KERNELTIME OF A PROCESS
                ULARGE_INTEGER kernelTimeValue;
                kernelTimeValue.LowPart = processKernelTime.dwLowDateTime;
                kernelTimeValue.HighPart = processKernelTime.dwHighDateTime;
                ULONGLONG kernelTimeNs = kernelTimeValue.QuadPart / 100;

                // GET THE USERTIME OF A PROCESS
                ULARGE_INTEGER userTimeValue;
                userTimeValue.LowPart = processUserTime.dwLowDateTime;
                userTimeValue.HighPart = processUserTime.dwHighDateTime;
                ULONGLONG userTimeNs = userTimeValue.QuadPart / 100;

                // Calculate the CPU time differences
                ULONGLONG kernelTimeDiff = kernelTimeNs - prevKernelTime;
                ULONGLONG userTimeDiff = userTimeNs - prevUserTime;
                
                // Convert nanoseconds to seconds
                if(initiater){
                    ULONGLONG kernelTimeSec = (kernelTimeNs);
                    ULONGLONG userTimeSec = (userTimeNs);
                    totalCPUTime = static_cast<double>(kernelTimeSec + userTimeSec)/1e4;
                    initiater = false;
                }
                else{
                    ULONGLONG kernelTimeSec = (kernelTimeDiff);
                    ULONGLONG userTimeSec = (userTimeDiff);
                    totalCPUTime = static_cast<double>(kernelTimeSec + userTimeSec)/1e4;
                }
                // CALCULATING TOTAL ELAPSED CPU TIME
                FILETIME currentTime;
                GetSystemTimeAsFileTime(&currentTime);
                ULARGE_INTEGER createTimeUL;
                createTimeUL.LowPart = createTime.dwLowDateTime;
                createTimeUL.HighPart = createTime.dwHighDateTime;

                ULARGE_INTEGER currentTimeUL;
                currentTimeUL.LowPart = currentTime.dwLowDateTime;
                currentTimeUL.HighPart = currentTime.dwHighDateTime;

                // Calculate the total elapsed time in 100-nanosecond intervals
                ULONGLONG totalElapsedTime = (currentTimeUL.QuadPart -
                                              createTimeUL.QuadPart)/100;

                ULONGLONG totalElapsedTimes = totalElapsedTime/1e4;
                totalElapsedTimes = static_cast<double>(totalElapsedTimes);
                double cpuUsage = 0.0;  // Default value if totalElapsedTime is zero

                // Calculate CPU usage of a process
                if (totalElapsedTimes != 0.0) {
                    cpuUsage = ((totalCPUTime) / totalElapsedTimes*100);
                }
                else
                {
                    cpuUsage = (totalElapsedTimes/(totalCPUTime*100 ) );
                }

                // Set values in table using QTableWidgetItem
                QTableWidgetItem *cpuUsageItem = new QTableWidgetItem
                    (QString::number(cpuUsage, 'f', 1) + "%");
//                QTableWidgetItem *kernel = new QTableWidgetItem
//                    (QString::number(kernelTimeSec, 'f', 1) + "ns");
//                QTableWidgetItem *user = new QTableWidgetItem
//                    (QString::number(userTimeSec, 'f', 1) + "ns");
                QTableWidgetItem *e1 = new QTableWidgetItem
                    (QString::number(totalCPUTime, 'f', 1) + "ms");
                QTableWidgetItem *e2 = new QTableWidgetItem
                    (QString::number(totalElapsedTimes, 'f', 1) + "ms");

                processTable->setItem(row, 3, cpuUsageItem);  
                processTable->setItem(row, 4, e1);
                processTable->setItem(row, 5, e2);
            }

            DWORD  ioUsageItem = GetProcessUsageIO(pe32.th32ProcessID);
            processTable->setItem(row, 0, pidItem);
            processTable->setItem(row, 1, nameItem);
            processTable->setItem(row, 2, new QTableWidgetItem
            (QString::number(ioUsageItem/1048576.0, 'f', 2)+"MB"));
            row++;
            CloseHandle(hProcess);
        } while (Process32Next(hSnapshot, &pe32));
        sortTableByColumn(column);
        processTable->setRowCount(row);
    }
    CloseHandle(hSnapshot);
    return 0;
}
// Calculate the space used in Input/Output Operations
DWORD MainWindow::GetProcessUsageIO(DWORD pid)
{
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (hProcess == NULL)
    {
        return 0;
    }
    // Getting the internet usage of the process.
    IO_COUNTERS ioCounters;
    GetProcessIoCounters(hProcess, &ioCounters);
    DWORD inputoutputBytes = ioCounters.OtherOperationCount;
    CloseHandle(hProcess);
    return inputoutputBytes;
}
void MainWindow::BoostProcess()
{
    if (EnablePrivilege(SE_DEBUG_NAME))
    {
        qDebug() << "Privileges attained successfully ---------------";
    }
    else
    {
        qDebug() << "Failed to attain privileges ----------------";
    }
    int selectedRow = processTable->currentRow();
    if (selectedRow >= 0)
    {
        QString pidString = processTable->item(selectedRow, 0)->text();
        DWORD pid = pidString.toULong();
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
        if (hProcess != NULL)
        {
            BOOL boostvalue;
            if(GetProcessPriorityBoost(hProcess , &boostvalue))
              {
                qDebug() << "inside step 1 bro" << boostvalue << "----------------";
                BOOL statechange = !boostvalue;
                qDebug() << "inside step 1.2 bro and pid is" << pid << "----------------";
                if (SetProcessPriorityBoost(GetCurrentProcess(), 0))
                {
                    qDebug() << "inside step 2 bro" << statechange << "----------------";
                    QTableWidgetItem *selectedpid = processTable->item(selectedRow, 0);
                    QColor highlightColor = statechange ? Qt::yellow : Qt::white;
                    selectedpid->setBackground(highlightColor);
                    if(statechange)
                    {
                        QMessageBox::information(this, "Boost", "Process Boosted successfully.");
                        refreshProcesses();
                    }
                    else{
                        QMessageBox::information(this, "Unboost", "Process Unboosted successfully.");
                    }
                }
                else{
                    QMessageBox::warning(this, "Boost", "Failed to set priority.");
                    qDebug() << "failed to set priority" << statechange << "----------------";
                    DWORD errorCode = GetLastError();
                    LPWSTR errorMessage = nullptr;
                    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                  NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                  reinterpret_cast<LPWSTR>(&errorMessage), 0, NULL);
                    QMessageBox::critical(this, "Boost", QString("Failed to set process priority boost. Error: %1").arg(QString::fromWCharArray(errorMessage)));
                    LocalFree(errorMessage);
                }
            }
            else
            {
                QMessageBox::warning(this, "Boost", "Failed to Boost the process.");
            }
            CloseHandle(hProcess);
        }
        else
        {
            QMessageBox::warning(this, "Boost", "Failed to open the process.");
        }
    }
    else
    {
        QMessageBox::warning(this, "Boost", "No process selected.");
    }
}
void MainWindow::terminateProcess()
{
    int selectedRow = processTable->currentRow();
    if (selectedRow >= 0)
    {
        QString pidString = processTable->item(selectedRow, 0)->text();
        DWORD pid = pidString.toULong();
        HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
        if (hProcess != NULL)
        {
            if (TerminateProcess(hProcess, 0))
            {
                QMessageBox::information(this, "Termination", "Process terminated successfully.");
                processTable->removeRow(selectedRow);
            }
            else
            {
                QMessageBox::warning(this, "Termination", "Failed to terminate the process.");
            }
            CloseHandle(hProcess);
        }
        else
        {
            QMessageBox::warning(this, "Termination", "Failed to open the process.");
        }
    }
    else
    {
        QMessageBox::warning(this, "Termination", "No process selected.");
    }
}
// Checks for necessary privileges for the current process
bool MainWindow::EnablePrivilege(const wchar_t* privilegeName)
{
    HANDLE hToken;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
    {
        return false;
    }
    TOKEN_PRIVILEGES tokenPrivileges;
    tokenPrivileges.PrivilegeCount = 1;
    tokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    if (!LookupPrivilegeValue(nullptr, privilegeName, &tokenPrivileges.Privileges[0].Luid))
    {
        CloseHandle(hToken);
        return false;
    }
    if (!AdjustTokenPrivileges(hToken, FALSE, &tokenPrivileges, sizeof(TOKEN_PRIVILEGES), nullptr, nullptr))
    {
        CloseHandle(hToken);
        return false;
    }
    CloseHandle(hToken);
    return true;
}
