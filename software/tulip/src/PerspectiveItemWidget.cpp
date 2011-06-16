#include "PerspectiveItemWidget.h"

#include <QtGui/QMouseEvent>
#include <tulip/PluginInformations.h>
#include "ui_PerspectiveItem.h"

PerspectiveItemWidget::PerspectiveItemWidget(tlp::PluginInformations *infos,QWidget *parent)
  : QWidget(parent), _perspectiveId(infos->identifier()), _ui(new Ui::PerspectiveItemData) {

  _ui->setupUi(this);
  _ui->icon->setPixmap(infos->iconPath());
  _ui->name->setText("<p><span style=\"font-size:large;\"><b>"
                     +
                     infos->name()
                     +
                     "</b></span></p>");
  _ui->description->setText("<p><span style=\"color:#626262;\">"
                            +
                            infos->shortDescription()
                            +
                            "</span></p>");
  connect(_ui->runButton,SIGNAL(clicked()),this,SIGNAL(selected()));
}

void PerspectiveItemWidget::focusInEvent(QFocusEvent *) {
  _ui->frame->setStyleSheet("QFrame#frame { background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(225, 225, 225, 255), stop:1 rgba(205,205,205, 255));  border-left: 1px solid \"#C9C9C9\";  border-right: 1px solid \"#C9C9C9\";  border-top: 1px solid \"#C9C9C9\";  border-bottom: 1px solid \"#C9C9C9\";}");
}

void PerspectiveItemWidget::focusOutEvent(QFocusEvent *) {
  _ui->frame->setStyleSheet("QFrame#frame { background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(245, 245, 245, 255), stop:1 rgba(225,225,225, 255));  border-left: 1px solid \"#C9C9C9\";  border-right: 1px solid \"#C9C9C9\";  border-top: 1px solid \"#C9C9C9\";  border-bottom: 1px solid \"#C9C9C9\";}");
}

void PerspectiveItemWidget::mouseDoubleClickEvent(QMouseEvent *e) {
  e->accept();
  emit selected();
}

