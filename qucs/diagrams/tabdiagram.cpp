/***************************************************************************
                          tabdiagram.cpp  -  description
                             -------------------
    begin                : Fri Oct 24 2003
    copyright            : (C) 2003 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "tabdiagram.h"
#include "main.h"

#include <qwidget.h>

#include <math.h>


TabDiagram::TabDiagram(int _cx, int _cy) : Diagram(_cx, _cy)
{
  x1 = 0;    // no extension to select area
  y1 = 0;
  x2 = 300;  // initial size of diagram
  y2 = 200;
  Name = "Tab";
  xAxis.limit_min = 0.0;  // scroll bar position (needs to be saved in file)

  calcDiagram();    // calculate circles for smith chart with |r|=1
}

TabDiagram::~TabDiagram()
{
}

// ------------------------------------------------------------
// calculates the text in the tabular
int TabDiagram::calcDiagram()
{
  Lines.clear();
  Texts.clear();
  Arcs.clear();

  x1 = 0;  // no scroll bar
  QFontMetrics  metrics(QucsSettings.font);
  int tHeight = metrics.lineSpacing();
  QString Str;
  int colWidth=0, x=8, y = y2-tHeight-6;

  // outer frame
  Lines.append(new Line(0, y2, x2, y2, QPen(QPen::black,0)));
  Lines.append(new Line(0, y2, 0, 0, QPen(QPen::black,0)));
  Lines.append(new Line(x2, y2, x2, 0, QPen(QPen::black,0)));
  Lines.append(new Line(0, 0, x2, 0, QPen(QPen::black,0)));
  Lines.append(new Line(0, y+2, x2, y+2, QPen(QPen::black,2)));

  Graph *g = Graphs.first();
  if(g == 0) {  // no variables specified in diagram ?
    Str = QObject::tr("no variables");
    colWidth = checkColumnWidth(Str, metrics, colWidth, x, y2);
    if(colWidth >= 0)
      Texts.append(new Text(x-4, y2-2, Str)); // independent variable
    return 0;
  }


  int NumAll=0;   // how many numbers per column
  int NumLeft=0;  // how many numbers could not be written

  double *py, *px;
  int counting, invisibleCount=0;
  int startWriting, lastCount = 1;

  while(g->cPointsX.isEmpty()) {  // any graph with data ?
    g = Graphs.next();
    if(g == 0) break;
  }
if(g) if(!g->cPointsX.isEmpty()) {
  // ................................................
  counting = g->cPointsX.getFirst()->count * g->countY;  // number of values

  invisibleCount = counting - y/tHeight;
  if(invisibleCount <= 0)  xAxis.limit_min = 0.0;// height bigger than needed
  else
    if(invisibleCount < int(xAxis.limit_min))
      xAxis.limit_min = double(invisibleCount); // adjust limit of scroll bar

  for(DataX *pD = g->cPointsX.last(); pD!=0; pD = g->cPointsX.prev()) {
    colWidth = 0;
    Str = pD->Var;
    colWidth = checkColumnWidth(Str, metrics, colWidth, x, y2);
    if(colWidth < 0)  goto funcEnd;
    startWriting = int(xAxis.limit_min);  // when to reach visible area

    Texts.append(new Text(x-4, y2-2, Str)); // independent variable
    if(pD->count != 0) {
      y = y2-tHeight-5;
      counting /= pD->count;   // how many rows to be skipped
      for(int z1=0; z1<lastCount; z1++) {
        px = pD->Points;
        for(int z=pD->count; z>0; z--) {
	  if(startWriting <= 0) { // reached visible area ?
	    y += tHeight*startWriting;
	    startWriting = 0;
	    if(y < tHeight) break;  // no room for more rows ?
	    Str = StringNum(*px);
	    colWidth = checkColumnWidth(Str, metrics, colWidth, x, y);
	    if(colWidth < 0)  goto funcEnd;

            Texts.append(new Text( x, y, Str));
            y -= tHeight*counting;
	  }
	  else startWriting -= counting;
	  px++;
        }
	if(pD == g->cPointsX.getFirst())   // only paint one time
	  if(y > 0) if(y < y2-tHeight-5)
            Lines.append(new Line(0, y+1, x2, y+1, QPen(QPen::black,0)));
      }
      lastCount *= pD->count;
    }
    x += colWidth+15;
    Lines.append(new Line(x-8, y2, x-8, 0, QPen(QPen::black,0)));
  }
  Lines.current()->style = QPen(QPen::black,2);

}  // of if no data in graphs


  // ................................................
  for(g = Graphs.first(); g!=0; g = Graphs.next()) {// all dependent variables
    y = y2-tHeight-5;
    colWidth = 0;

    Str = g->Var;
    colWidth = checkColumnWidth(Str, metrics, colWidth, x, y2);
    if(colWidth < 0)  goto funcEnd;
    Texts.append(new Text(x, y2-2, Str));  // dependent variable


    startWriting = int(xAxis.limit_min);  // when to reach visible area
    py = g->cPointsY - 2;
    if(g->cPointsX.getFirst()) {
      int z=g->cPointsX.getFirst()->count * g->countY;
      if(z > NumAll)  NumAll = z;
      for(; z>0; z--) {
        py += 2;
        if(startWriting-- > 0) continue; // reached visible area ?
        if(y < tHeight) break;           // no room for more rows ?
        switch(g->numMode) {
          case 0: Str = complexRect(*py, *(py+1), g->Precision); break;
          case 1: Str = complexDeg (*py, *(py+1), g->Precision); break;
          case 2: Str = complexRad (*py, *(py+1), g->Precision); break;
        }

        colWidth = checkColumnWidth(Str, metrics, colWidth, x, y);
        if(colWidth < 0)  goto funcEnd;

        Texts.append(new Text(x, y, Str));
        y -= tHeight;
      }
      if(z > NumLeft)  NumLeft = z;
    }
    else {   // no data in graph
      Str = QObject::tr("no data");
      colWidth = checkColumnWidth(Str, metrics, colWidth, x, y);
      if(colWidth < 0)  goto funcEnd;
      Texts.append(new Text(x, y, Str));
    }
    x += colWidth+15;
    Lines.append(new Line(x-8, y2, x-8, 0, QPen(QPen::black,0)));
  }

funcEnd:
  if(invisibleCount > 0) {  // could all numbers be written ?
    x1 = 18;   // extend the select area to the left

    // calculate data for painting scroll bar
    xAxis.numGraphs = int(xAxis.limit_min);
    NumLeft = NumAll - NumLeft - xAxis.numGraphs;

    // height of scroll bar
    zAxis.numGraphs = (y2 - 40) * NumLeft / NumAll;
    if(zAxis.numGraphs < 2)  zAxis.numGraphs = 2;

    // position of scroll bar in pixel
    yAxis.numGraphs = (y2 - 40) * xAxis.numGraphs / NumAll;

    xAxis.numGraphs = NumLeft;  // number of lines in the diagram
  }

  return 1;
}

// ------------------------------------------------------------
int TabDiagram::checkColumnWidth(const QString& Str,
		const QFontMetrics& metrics, int colWidth, int x, int y)
{
  QSize r = metrics.size(0, Str);  // width of text
  if(r.width() > colWidth) {
    colWidth = r.width();
    if((x+colWidth) >= x2) {    // enough space for text ?
      // mark lack of space with a small arrow
      Lines.append(new Line(x2-6, y-4, x2+7, y-4, QPen(QPen::red,2)));
      Lines.append(new Line(x2,   y-7, x2+6, y-4, QPen(QPen::red,2)));
      Lines.append(new Line(x2,   y-1, x2+6, y-4, QPen(QPen::red,2)));
      return -1;
    }
  }
  return colWidth;
}

// ------------------------------------------------------------
bool TabDiagram::scroll(int clickPos)
{
  if(x1 <= 0) return false;   // no scroll bar ?
  double tmp = xAxis.limit_min;

  int y = cy;
  if(clickPos > (cy-20)) {  // scroll one line down ?
    xAxis.limit_min++;
  }
  else {
    y -= y2 - 20;
    if(clickPos < y) {  // scroll bar one line up ?
      if(xAxis.limit_min <= 0)  return false;
      xAxis.limit_min--;
    }
    else {
      y += yAxis.numGraphs;
      if(clickPos < y) {  // scroll bar one page up ?
        xAxis.limit_min -= double(xAxis.numGraphs);
        if(xAxis.limit_min < 0.0)  xAxis.limit_min = 0.0;
      }
      else {
        y += zAxis.numGraphs;
        if(clickPos > y)   // a page down?
          xAxis.limit_min += double(xAxis.numGraphs);
      }
    }
  }

  calcDiagram();
  if(tmp == xAxis.limit_min)  return false;   // did anything change ?
  return true;
}

// ------------------------------------------------------------
TabDiagram* TabDiagram::newOne()
{
  return new TabDiagram();
}
