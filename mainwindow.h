#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "plot_thread.h"
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>
#include <qwt_plot_spectrogram.h>

#include <qwt3d_surfaceplot.h>
#include <qwt3d_function.h>

#define NATOM 1
#define NAPAR NATOM*3

#ifndef M_PI
#define M_PI 3.14159265354
#endif

#define noise(per) ((drand48()*4.0)-2.0)*per

#define randomize()     srand48((unsigned)time(NULL))

#define max(a,b)        (((a) > (b)) ? (a) : (b))
#define min(a,b)        (((a) < (b)) ? (a) : (b))





namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void gaReset();
    Qwt3D::SurfacePlot* plot;
    Qwt3D::Function *surface3d;




private:
    Ui::MainWindow *ui;
    MyThread *thread;
    int freq;
    double x[2001], y1[2001];
    double x2[2005], y2[2005];
    QwtPlotCurve *curve1,*curve2;
    int rank, size;
    struct point {double x,y,yi;};
    struct aparam {double
                  xv,xvmin,xvmax,xvc,xvmed,xvstd,xvst,xvtot,
                  yv,yvmin,yvmax,yvc,yvmed,yvstd,yvst,yvtot,
                  zv,zvmin,zvmax,zvc,zvmed,zvstd,zvst,zvtot;};

    struct aparam atoms[NATOM];
    struct aparam *pp;
    struct generation {double r,xv[NATOM],yv[NATOM],zv[NATOM];};
    struct generation population [301];
    struct generation *gap,*pop,*pops,*popgr,*pope,*popc,*p1,*p2,*p3;
    double *rp;


    double drand(double a, double b);

    void first_population(void);
    void calc_population_statistics(void);
    void calc_generation(int gen);
    void statistics_add (void);
    void statistics_extract (void);
    void change_width_best(double alfa);
    static int compare_r(const void *, const void *);
    void parametersSet(void);
    void surfacePlot(void);

    int i,p;
    int iter,totiter,run;
    double w;
    int cm,n,np,aiter;
    double oldr;
    double xx,yy,yyo,maxr,minr;
    double alfa;
    double minx,maxx,minz;
    int type;
    int NPOPUL;
    int NITER;

    int d_interval; // timer in ms
    int d_timerId;

    QwtSymbol sym;
    QwtPlotSpectrogram *d_spectrogram;


private slots:
    void on_spinBox_timer_valueChanged(int );
    void on_comboBox_currentIndexChanged(int index);
    void on_pushButton_2_released();
    void updatePlot(int freq);
    void updatePlotSearch(int freq);
    void gaCalc(int type);

    public slots:
    void setTimerInterval(double interval);

protected:
    virtual void timerEvent(QTimerEvent *e);

};

#endif // MAINWINDOW_H
