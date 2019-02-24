#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMainWindow>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_map.h>
#include <qwt_symbol.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_color_map.h>
  #include <qwt3d_surfaceplot.h>
  #include <qwt3d_function.h>
#include "plot_thread.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <float.h>

  using namespace Qwt3D;

  class Surface3d : public Function
  {
  public:
      int typem;

    Surface3d(SurfacePlot& pw, int type)
    :Function(pw)
    {
        typem=type;
    }

    double operator()(double x, double y)
    {
        if (typem==0) return x*sin(sqrt(fabs(x)))+y*sin(sqrt(fabs(y))); //schwefel
       if (typem==1) return 20.0+x*x+y*y-10.0*(cos(2.0*M_PI*x)+cos(2.0*M_PI*y)); // rastrigin
       if (typem==2) return x*x+y*y; // dejong
        if (typem==3) return ((x*x+y*y)/4000.0) - cos(x)* cos(y/sqrt(2.0))+1; //griewank
    }


  };



class SchwefelData: public QwtRasterData
{
public:
    SchwefelData():
        QwtRasterData(qwtBoundingRect(QwtSeriesData(-500, 500, -500.0, 500.0))
    {
    }

    virtual QwtRasterData *copy() const
    {
        return new SchwefelData();
    }

    virtual QwtInterval range() const
    {
        return QwtInterval(-840.0, 840.0);
    }

    virtual double value(double x, double y) const
    {
        const double v1=x*sin(sqrt(fabs(x)))+y*sin(sqrt(fabs(y)));
        return v1;
    }
};

class RastriginData: public QwtRasterData
{
public:
    RastriginData():
        QwtRasterData(QwtDoubleRect(-6, 6, -6.0, 6.0))
    {
    }

    virtual QwtRasterData *copy() const
    {
        return new RastriginData();
    }

    virtual QwtInterval range() const
    {
        return QwtInterval(QwtSeriesData(0.0, 80.0));
    }

    virtual double value(double x, double y) const
    {
        const double v1=20.0+x*x+y*y-10.0*(cos(2.0*M_PI*x)+cos(2.0*M_PI*y));
        return v1;
    }
};

class DeJongData: public QwtRasterData
{
public:
    DeJongData():
        QwtRasterData(QwtDoubleRect(-6, 6, -6.0, 6.0))
    {
    }

    virtual QwtRasterData *copy() const
    {
        return new DeJongData();
    }

    virtual QwtInterval range() const
    {
        return QwtInterval(QwtSeriesData(0.0, 500000.0));
    }

    virtual double value(double x, double y) const
    {
        const double v1=x*x+y*y;
        return v1;
    }
};

class GriewankData: public QwtRasterData
{
public:
    GriewankData():
        QwtRasterData(QwtDoubleRect(-6, 6, -6.0, 6.0))
    {
    }

    virtual QwtRasterData *copy() const
    {
        return new GriewankData();
    }

    virtual QwtInterval range() const
    {
        return QwtInterval(QwtSeriesData(0.0, 3.0));
    }

    virtual double value(double x, double y) const
    {
        const double v1=((x*x+y*y)/4000.0) - cos(x)* cos(y/sqrt(2.0))+1;
        return v1;
    }
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    parametersSet();
        gaReset();

    d_spectrogram = new QwtPlotSpectrogram();

    curve1 = new QwtPlotCurve("Convergence");
    //ui->qwtPlot->setTitle("<font size=-2>Convergence</font>");
    ui->qwtPlot->setAxisTitle(QwtPlot::xBottom, "<small>Deneme Sayisi</small>");
    ui->qwtPlot->setAxisTitle(QwtPlot::yLeft, "<small>Genel Minimum</small>");
        ui->qwtPlot->setAxisMaxMajor(QwtPlot::xBottom,4);

    ui->qwtPlot->setAxisFont(QwtPlot::xBottom, QFont( "Times", 9 ) );
    ui->qwtPlot->setAxisFont(QwtPlot::yLeft, QFont( "Times", 9 ) );

    curve2 = new QwtPlotCurve("Searches");
    //ui->qwtPlot_search->setTitle("<font size=-2>Search Paths</font>");
    ui->qwtPlot_search->setAxisTitle(QwtPlot::xBottom, "<small>x1</small>");
    ui->qwtPlot_search->setAxisTitle(QwtPlot::yLeft, "<small>x2</small>");
       ui->qwtPlot_search->setAxisMaxMajor(QwtPlot::xBottom,5);


    //sym.setStyle(QwtSymbol::XCross);
      sym.setStyle(QwtSymbol::Cross);
    //sym.setStyle(QwtSymbol::Ellipse);
    sym.setPen(QColor(Qt::darkMagenta));
    sym.setSize(10);
    curve2->setSymbol(sym);
    curve2->setStyle(QwtPlotCurve::NoCurve);

    //ui->qwtPlot_search->setStyle(QwtSymbol::XCross);
    ui->qwtPlot_search->setAxisMaxMajor(QwtPlot::xBottom,4);
    ui->qwtPlot_search->setAxisMaxMajor(QwtPlot::yLeft,4);

    ui->qwtPlot_search->setAxisFont(QwtPlot::xBottom, QFont( "Times", 9 ) );
    ui->qwtPlot_search->setAxisFont(QwtPlot::yLeft, QFont( "Times", 9 ) );



   ui->qwtPlot_search->setAxisScale(QwtPlot::yLeft, minx, maxx);
   ui->qwtPlot_search->setAxisScale(QwtPlot::xBottom, minx, maxx);





    QwtLinearColorMap colorMap(Qt::darkCyan, Qt::red);
    colorMap.addColorStop(0.1, Qt::cyan);
    colorMap.addColorStop(0.6, Qt::green);
    colorMap.addColorStop(0.95, Qt::yellow);
    d_spectrogram->setColorMap(colorMap);



ui->qwtPlot->replot();
ui->qwtPlot_search->replot();

randomize();
type=ui->comboBox->currentIndex();


    //glWidget = new GLWidget;
    //ui->verticalLayout_opengl->addWidget(glWidget);


    plot = new SurfacePlot();
    ui->verticalLayout_plot3d->addWidget(plot);

    on_comboBox_currentIndexChanged(type);

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_2_released()
{
    NITER=ui->spinBox_niter->value();
    ui->listWidget_results->clear();
    //thread = new MyThread(NITER,this);
    //connect(thread, SIGNAL(freqChanged(int)),this, SLOT(gaCalc(int)));
    //ui->pushButton_2->setEnabled(false);
    gaReset();
    //thread->start();
    int timei;
timei=ui->spinBox_timer->value();
setTimerInterval((double) timei);
}


void MainWindow::updatePlot(int freq)
{

curve1->setData(x, y1, freq);

curve1->attach(ui->qwtPlot);

ui->qwtPlot->replot();

}

void MainWindow::updatePlotSearch(int freq)
{
 popgr=&population[0];
    for (int ig=0;ig<NPOPUL;ig++) {
    x2[ig]=popgr->xv[0];
    y2[ig]=popgr->yv[0];
    popgr++;
    }

curve2->setData(x2, y2, freq);
curve2->attach(ui->qwtPlot_search);
ui->qwtPlot_search->replot();

}


void MainWindow::gaReset()
{

       ui->qwtPlot_search->setAxisScale(QwtPlot::yLeft, minx, maxx);
   ui->qwtPlot_search->setAxisScale(QwtPlot::xBottom, minx, maxx);
   ui->qwtPlot_search->replot();
    //qDebug()<<"Burdayimmmmmmmmmm";
    iter=0;oldr=1.0e50;
    alfa=ui->doubleSpinBox_alfa->value();
    NPOPUL=ui->spinBox_npopul->value();
    minx=ui->spinBox_min->value();
    maxx=ui->spinBox_max->value();

    ui->lineEdit_r->setText("");
    ui->lineEdit_x1->setText("");
    ui->lineEdit_x2->setText("");
    ui->lineEdit_iter->setText("");
    pops=&population[0];
    pope=&population[NPOPUL-1];
    popc=&population[NPOPUL];
    pp=&atoms[0];
    for (i=0;i<NATOM;i++)
    {
        pp->xvmed=0.0;pp->xvstd=0.0;pp->xvst=0.0;pp->xvtot=0.0;pp->xv=99;pp->xvmin=minx;pp->xvmax=maxx;
        pp->yvmed=0.0;pp->yvstd=0.0;pp->yvst=0.0;pp->yvtot=0.0;pp->yv=99;pp->yvmin=minx;pp->yvmax=maxx;
        pp->zvmed=0.0;pp->zvstd=0.0;pp->zvst=0.0;pp->zvtot=0.0;pp->zv=99;pp->zvmin=minx;pp->zvmax=maxx;
        pp++;
    }

    first_population();

    qsort(pops,NPOPUL,sizeof(generation), compare_r);
    calc_population_statistics();

}

void MainWindow::gaCalc(int iter)
{


    ui->pushButton_2->setEnabled(true);
}





double MainWindow::drand(double a, double b)
{

  double sayi;
  do {
  sayi=(drand48()*(b-a))+a;

  } while (sayi>maxx || sayi<minx);
   return sayi;

}



void MainWindow::calc_generation(int np)
{
        int ni;
        //int nj;
        double potential;
        //double radius;
        totiter++;
        pop=&population[np];

        pp=&atoms[0];
        for(i=0;i<NATOM;i++)
        {
                pop->xv[i]=drand(pp->xvmin,pp->xvmax);
                pop->yv[i]=drand(pp->yvmin,pp->yvmax);
                pop->zv[i]=drand(pp->zvmin,pp->zvmax);
                pp++;
        }

        pop->r=0.0;

        pp=&atoms[0];
        potential=0.0;
        for (ni=0;ni<NATOM;ni++)
         {

        //radius= pow((  pow((pop->xv[ni]-pop->xv[nj]),2.0)+ pow((pop->yv[ni]-pop->yv[nj]),2.0)+pow((pop->zv[ni]-pop->zv[nj]),2.0) ), (1.0/2.0) );
        if (type==0) potential=pop->xv[ni]*sin(sqrt(fabs(pop->xv[ni])))+pop->yv[ni]*sin(sqrt(fabs(pop->yv[ni]))) ; //schwefel
        if (type==1) potential=20.0+pop->xv[ni]*pop->xv[ni]+pop->yv[ni]*pop->yv[ni]-10.0*(cos(2.0*M_PI*pop->xv[ni])+cos(2.0*M_PI*pop->yv[ni])); //rastrigin
        if (type==2) potential=pop->xv[ni]*pop->xv[ni]+pop->yv[ni]*pop->yv[ni]; //dejong
        if (type==3) potential=((pop->xv[ni]*pop->xv[ni]+pop->yv[ni]*pop->yv[ni])/4000.0) - cos(pop->xv[ni])* cos(pop->yv[ni]/sqrt(2.0))+1; //griewank
                pp++;
        }
        pop->r=potential;
   }


void MainWindow::first_population(void)
{


        for (n=0;n<NPOPUL;n++)
        {
        calc_generation(n);
        pp=&atoms[0];
        for (np=0;np<NATOM;np++)
                {
                pp->xvtot=pp->xvtot+pop->xv[np];pp->xvst=pp->xvst+(pop->xv[np]*pop->xv[np]);
                pp->yvtot=pp->yvtot+pop->yv[np];pp->yvst=pp->yvst+(pop->yv[np]*pop->yv[np]);
                pp->zvtot=pp->zvtot+pop->zv[np];pp->zvst=pp->zvst+(pop->zv[np]*pop->zv[np]);
                pp++;
                }
        }


}



void MainWindow::statistics_add ()
{
      int n;
        pp=&atoms[0];
        for (n=0;n<NATOM;n++)
        {
                pp->xvtot = pp->xvtot+popc->xv[n];pp->xvst = pp->xvst+popc->xv[n]*popc->xv[n];
                pp->yvtot = pp->yvtot+popc->yv[n];pp->yvst = pp->yvst+popc->yv[n]*popc->yv[n];
                pp->zvtot = pp->zvtot+popc->zv[n];pp->zvst = pp->zvst+popc->zv[n]*popc->zv[n];
        pp++;
        }

}

void MainWindow::statistics_extract ()
{
         int n;
        pp=&atoms[0];
        for (n=0;n<NATOM;n++)
        {
                pp->xvtot = pp->xvtot-popc->xv[n];pp->xvst = pp->xvst-popc->xv[n]*popc->xv[n];
                pp->yvtot = pp->yvtot-popc->yv[n];pp->yvst = pp->yvst-popc->yv[n]*popc->yv[n];
                pp->zvtot = pp->zvtot-popc->zv[n];pp->zvst = pp->zvst-popc->zv[n]*popc->zv[n];

                pp->xvmed = pp->xvtot/NPOPUL;
                pp->yvmed = pp->yvtot/NPOPUL;
                pp->zvmed = pp->zvtot/NPOPUL;

                pp->xvstd=sqrt( (pp->xvst-((pp->xvtot*pp->xvtot)/NPOPUL))/NPOPUL);
                pp->yvstd=sqrt( (pp->yvst-((pp->yvtot*pp->yvtot)/NPOPUL))/NPOPUL);
                pp->zvstd=sqrt( (pp->zvst-((pp->zvtot*pp->zvtot)/NPOPUL))/NPOPUL);
        pp++;
        }
}
void MainWindow::calc_population_statistics()
{
      int n;
        pp=&atoms[0];
        for (n=0;n<NATOM;n++)
        {
                pp->xvstd=sqrt( (pp->xvst-((pp->xvtot*pp->xvtot)/NPOPUL))/NPOPUL);pp->xvmed=pp->xvtot/NPOPUL;
                pp->yvstd=sqrt( (pp->yvst-((pp->yvtot*pp->yvtot)/NPOPUL))/NPOPUL);pp->yvmed=pp->yvtot/NPOPUL;
                pp->zvstd=sqrt( (pp->zvst-((pp->zvtot*pp->zvtot)/NPOPUL))/NPOPUL);pp->zvmed=pp->zvtot/NPOPUL;
        pp++;
        }

}

void MainWindow::change_width_best(double w)
{
       int n;
        pp=&atoms[0];
        for (n=0;n<NATOM;n++)
        {
                pp->xvmin=pops->xv[n] - w*pp->xvstd; pp->xvmax=pops->xv[n] + w*pp->xvstd;
                pp->yvmin=pops->yv[n] - w*pp->yvstd; pp->yvmax=pops->yv[n] + w*pp->yvstd;
                pp->zvmin=pops->zv[n] - w*pp->zvstd; pp->zvmax=pops->zv[n] + w*pp->zvstd;
                pp++;
        }

}





int MainWindow::compare_r(const void *a, const void *b)
{
    struct generation *ia = (struct generation *)a;
    struct generation *ib = (struct generation *)b;
    if (ia->r > ib->r) return(1);
    else return(-1);
    //return (int)(100.f*ia->r - 100.f*ib->r);
}


void MainWindow::on_comboBox_currentIndexChanged(int index)
{

    //thread->quit();
    ui->pushButton_2->setEnabled(true);
    type=ui->comboBox->currentIndex();
    parametersSet();
    gaReset();
   ui->qwtPlot_search->setAxisScale(QwtPlot::yLeft, minx, maxx);
   ui->qwtPlot_search->setAxisScale(QwtPlot::xBottom, minx, maxx);


            d_spectrogram->setData(SchwefelData());
            d_spectrogram->attach(ui->qwtPlot_search);


    if (type==1) {

        d_spectrogram->setData(RastriginData());
        d_spectrogram->attach(ui->qwtPlot_search);
    }

    if (type==2) {

                d_spectrogram->setData(DeJongData());
        d_spectrogram->attach(ui->qwtPlot_search);
    }
    if (type==3) {

        d_spectrogram->setData(GriewankData());
        d_spectrogram->attach(ui->qwtPlot_search);
    }

ui->qwtPlot_search->replot();
    double minzs[4]={-900.0, 0,0,0};
    minz=minzs[type];
    surfacePlot();

}

void MainWindow::parametersSet(void)
{
    double alfas[4]={1.1, 1.1, 0.3, 1.1};
    int popsizes[4]={110,120,80,100};
    int minxs[4]={-500,-6,-500,-50};
    int maxxs[4]={500,6,500,50};
    int itern[4]={6000,5000,1000,20000};
    double minzs[4]={-900.0, 0.0,0.0,0.0};
    type=ui->comboBox->currentIndex();
    ui->doubleSpinBox_alfa->setValue(alfas[type]);
    ui->spinBox_npopul->setValue(popsizes[type]);
    ui->spinBox_min->setValue(minxs[type]);
    ui->spinBox_max->setValue(maxxs[type]);
    ui->spinBox_niter->setValue(itern[type]);
    minz=minzs[type];
}


void MainWindow::setTimerInterval(double ms)
{
    d_interval = qRound(ms);

    if ( d_timerId >= 0 )
    {
        killTimer(d_timerId);
        d_timerId = -1;
    }
    if (d_interval >= 0 )
        d_timerId = startTimer(d_interval);
}

void MainWindow::timerEvent(QTimerEvent *)
{

                    calc_generation(NPOPUL);
                     if (popc->r < pope->r)
                                        {
                                                statistics_add();
                                                qsort(pops,NPOPUL+1,sizeof(generation),compare_r);
                                                statistics_extract();
                                                if (pops->r < oldr)
                                                {
                                                        oldr=pops->r;
                                                        //updatePlotSearch(NPOPUL);
                                                }

                                        //updatePlotSearch(NPOPUL);


                                                change_width_best(alfa);
                                        }

                                if (iter % 100 ==0) {
                                x[iter/100]=iter;
                                y1[iter/100]=pops->r;


                                ui->lineEdit_iter->setText(QString("%1").arg(iter+100));
                                //ui->progress_iter->setValue((iter*100/NITER));
                                ui->lineEdit_r->setText(QString("%1").arg(pops->r,0,'f',4));
                                ui->lineEdit_x1->setText(QString("%1").arg( pops->xv[0],0,'f',4));
                                ui->lineEdit_x2->setText(QString("%1").arg( pops->yv[0],0,'f',4));

                                updatePlot(iter/100);
                                updatePlotSearch(NPOPUL);
                                QString text;
                                text=QString("%1\t%2").arg(iter) .arg(pops->r);
                               ui->listWidget_results->addItem(text);


                            }
                                iter++;
                                if (iter==NITER)    {
        killTimer(d_timerId);
        d_timerId = -1;
                                }
}

void MainWindow::on_spinBox_timer_valueChanged(int )
{
int timei;
timei=ui->spinBox_timer->value();
setTimerInterval((double) timei);
}



void MainWindow::surfacePlot()
{


      if (type==0) plot->setScale(1.5,1.5,0.5);
      if (type==1) plot->setScale(5.0,5.0,0.5);
      if (type==2) plot->setScale(250.0,250.0,0.5);
      if (type==3) plot->setScale(1.0,1.0,15.0);
      plot->setZoom(0.9);
      plot->setRotation(20,0,15);

      plot->coordinates()->setGridLines(true,true);
      plot->setCoordinateStyle(BOX);
      //plot->setCoordinateStyle(NOCOORD);
        plot->setPlotStyle(FILLED); //WIREFRAME,HIDDENLINE,FILLED,FILLEDMESH,POINTS,USER
        plot->setFloorStyle(FLOORDATA);//FLOORDATA, FLOORISO

        surface3d = new Surface3d(*plot,type);
        surface3d->setMesh(64,64);
        surface3d->setDomain(minx,maxx,minx,maxx);
        surface3d->setMinZ(minz);
        surface3d->create();

        plot->setRotation(30,0,15);

}
