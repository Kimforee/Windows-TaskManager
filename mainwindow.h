#include <QMainWindow>
#include <QTableWidget>
#include <Windows.h>
#include <Pdh.h>
#include <QTimer>
#include <QString>
#include <QtCharts>
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void   RAM();
    void   pausetimer();
    void   terminateProcess();
    void   BoostProcess();
    void   sortTableByColumn(int column);
    int    refreshProcesses();
    double GetCpuUsage();
    DWORD  GetProcessUsageIO(DWORD pid);
    bool   EnablePrivilege(const wchar_t* privilegeName);
    void   updateGraph(double cpuUsage);

private:
    QTableWidget *processTable;
    QTableWidget *Table;
    void setupUI();
    PDH_HQUERY   cpuQuery;
    PDH_HCOUNTER cpuTotal;
    QTimer *timer;
    QTimer *timer1;
    QString totalPhysicalMemoryString;
    int  column;
    bool isSortingEnabled;
    QTableWidgetItem* nameItem ;
    QTableWidgetItem* usageItem ;
    double totalCPUTime;
    double kernelTimeSec;
    double userTimeSec;
    double cpuUsage;
    QLineSeries *cpuSeries;
    QLineSeries *ramSeries;
    double ttime;
    double rtime;
    QChartView *chartView;
    QChartView *ramView;
    QChart *chart;
    QChart *ramchart;
    QValueAxis *axisX;
    QValueAxis *ramaxisX;
    QValueAxis *ramaxisY;
};
