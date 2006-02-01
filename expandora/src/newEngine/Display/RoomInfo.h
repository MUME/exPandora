#ifndef ROOMINFO
#define ROOMINFO
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "MumeRoom.h"

class RoomInfo : public QWidget
{
  Q_OBJECT
public:

  RoomInfo(QWidget *parent);

  QLabel*     id_label;
  QLabel*     terrain_label;
  QLabel*     coord_label;

  QLabel*     name_label;
  QLineEdit*  name_edit;

  QLabel*     desc_label;
  QLineEdit*  desc_edit;

  QLabel*     note_label;
  QLineEdit*  note_edit;

  QLabel*     exits_label;
  QLabel*     doors_label;

  QPushButton* apply_button;

  void update_info(MumeRoom * rr);

private:
  QHBoxLayout   *topLayout;
  QHBoxLayout   *noteLayout;
  QHBoxLayout   *nameLayout;
  QHBoxLayout   *descLayout;
  QVBoxLayout   *mainLayout;

};
#endif
