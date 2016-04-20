/***************************************************************************
**                                                                        **
**  QCustomPlot, an easy to use, modern plotting widget for Qt            **
**  Copyright (C) 2011-2015 Emanuel Eichhammer                            **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Emanuel Eichhammer                                   **
**  Website/Contact: http://www.qcustomplot.com/                          **
**             Date: 22.12.15                                             **
**          Version: 1.3.2                                                **
****************************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  connect(ui->cbUseCurrentSize, SIGNAL(toggled(bool)), ui->sbWidth, SLOT(setDisabled(bool)));
  connect(ui->cbUseCurrentSize, SIGNAL(toggled(bool)), ui->sbHeight, SLOT(setDisabled(bool)));
  
  ui->plot->axisRect()->setMinimumSize(300, 180);
  setupPlot();
  
  // register the plot document object (only needed once, no matter how many plots will be in the QTextDocument):
  QCPDocumentObject *plotObjectHandler = new QCPDocumentObject(this);
  ui->textEdit->document()->documentLayout()->registerHandler(QCPDocumentObject::PlotTextFormat, plotObjectHandler);
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::setupPlot()
{
  // The following plot setup is taken from the sine demo:
  // add two new graphs and set their look:
  ui->plot->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom); // this will also allow rescaling

  ui->plot->axisRect()->setupFullAxesBox(true);
  ui->plot->xAxis->setLabel("x");
  ui->plot->yAxis->setLabel("y");

  // set up the QCPColorMap:
  QCPColorMap *colorMap = new QCPColorMap(plot->xAxis, plot->yAxis);
  plot->addPlottable(colorMap);
  int nx = 200;
  int ny = 200;
  colorMap->data()->setSize(nx, ny); // we want the color map to have nx * ny data points
  colorMap->data()->setRange(QCPRange(-4, 4), QCPRange(-4, 4)); // and span the coordinate range -4..4 in both key (x) and value (y) dimensions
  // now we assign some data, by accessing the QCPColorMapData instance of the color map:
  double x, y, z;
  for (int xIndex=0; xIndex<nx; ++xIndex)
  {
    for (int yIndex=0; yIndex<ny; ++yIndex)
    {
      colorMap->data()->cellToCoord(xIndex, yIndex, &x, &y);
      double r = 3*qSqrt(x*x+y*y)+1e-2;
      z = 2*x*(qCos(r+2)/r-qSin(r+2)/r); // the B field strength of dipole radiation (modulo physical constants)
      colorMap->data()->setCell(xIndex, yIndex, z);
    }
  }
  // add a color scale:
  QCPColorScale *colorScale = new QCPColorScale(plot);


  ui->plot->plotLayout()->addElement(0, 1, colorScale); // add it to the right of the main axis rec
  ui->plot->plotLayout()->addElement(0, 1, colorScale); // add it to the right of the main axis rect
  ui->colorScale->setType(QCPAxis::atRight); // scale shall be vertical bar with tick/axis labels right (actually atRight is already the default)
  ui->colorMap->setColorScale(colorScale); // associate the color map with the color scale
  ui->colorScale->axis()->setLabel("Magnetic Field Strength");

  ui->colorMap->setGradient(QCPColorGradient::gpPolar);
  ui->colorMap->rescaleDataRange();
  QCPMarginGroup *marginGroup = new QCPMarginGroup(plot);
  // pass data points to graphs:
  ui->plot->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
  ui->colorScale->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
  // let the ranges scale themselves so graph 0 fits perfectly in the visible area:
  ui->plot->rescaleAxes();

}

void MainWindow::on_actionInsert_Plot_triggered()
{
  QTextCursor cursor = ui->textEdit->textCursor();
  
  // insert the current plot at the cursor position. QCPDocumentObject::generatePlotFormat creates a
  // vectorized snapshot of the passed plot (with the specified width and height) which gets inserted
  // into the text document.
  double width = ui->cbUseCurrentSize->isChecked() ? 0 : ui->sbWidth->value();
  double height = ui->cbUseCurrentSize->isChecked() ? 0 : ui->sbHeight->value();
  cursor.insertText(QString(QChar::ObjectReplacementCharacter), QCPDocumentObject::generatePlotFormat(ui->plot, width, height));
  
  ui->textEdit->setTextCursor(cursor);
}

void MainWindow::on_actionSave_Document_triggered()
{
  QString fileName = QFileDialog::getSaveFileName(this, "Save document...", qApp->applicationDirPath(), "*.pdf");
  if (!fileName.isEmpty())
  {
    QPrinter printer;
    printer.setFullPage(true);
    printer.setPaperSize(QPrinter::A4);
    printer.setOrientation(QPrinter::Portrait);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    ui->textEdit->document()->print(&printer);
  }
}






