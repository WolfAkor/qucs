/***************************************************************************
                          componentdialog.h  -  description
                             -------------------
    begin                : Tue Sep 9 2003
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

#ifndef COMPONENTDIALOG_H
#define COMPONENTDIALOG_H

#include "component.h"

#include <qlabel.h>
#include <qdialog.h>
#include <qlistview.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qregexp.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qptrlist.h>
#include <qdir.h>

class QucsDoc;
class QGridLayout;
class QValidator;
class QVBoxLayout;
class QIntValidator;
class QRegExpValidator;

/**
  *@author Michael Margraf
  */

extern QDir QucsWorkDir;


class ComponentDialog : public QDialog {
   Q_OBJECT
public:
  ComponentDialog(Component*, QucsDoc*, QWidget *parent=0);
  ~ComponentDialog();

private slots:
  void slotButtOK();
  void slotButtCancel();
  void slotSelectProperty(QListViewItem *item);
  void slotApplyInput();
  void slotApplyState(int State);
  void slotBrowseFile();
  void slotEditFile();
  void slotApplyChange(const QString& Text);
  void slotApplyProperty();
  void slotApplyPropName();

  void slotButtAdd();
  void slotButtRem();

  void slotSimTypeChange(int);
  void slotNumberChanged(const QString&);
  void slotStepChanged(const QString&);
  void slotSetChanged(int);
  void slotTextChanged(const QString&);

private:
  void correctDesc(QString &desc, QStringList *clst = 0);

  QVBoxLayout *all;   // the mother of all widgets
  QValidator  *Validator, *ValRestrict;
  QRegExp     Expr;
  QIntValidator *ValInteger;
  QListView   *prop;
  QLineEdit   *edit, *NameEdit, *CompNameEdit;
  QComboBox   *ComboEdit;
  QLabel      *Name, *Description;
  QPushButton *BrowseButt, *EditButt, *ButtAdd, *ButtRem;
  QCheckBox   *disp;
  Component   *Comp;
  QucsDoc     *Doc;
  bool        changed, transfered;
  int         tx_Dist, ty_Dist;   // remember the text position

  QLabel    *textType;
  QLabel    *textSim, *textParam, *textValues, *textStart, *textStop,
            *textStep, *textNumber;
  QLineEdit *editSim, *editParam, *editValues, *editStart, *editStop,
            *editStep, *editNumber;
  QCheckBox *checkSim, *checkParam, *checkValues, *checkStart, *checkStop,
            *checkNumber, *checkType;
  QComboBox *comboType;
};

#endif
