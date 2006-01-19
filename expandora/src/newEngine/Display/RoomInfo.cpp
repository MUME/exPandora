#include "RoomInfo.h"
#include "Terrain.h"

RoomInfo::RoomInfo(QWidget* parent)
    : QWidget(parent)
{
  /* ------- */
  id_label = new QLabel("ID: Out of sync");
  terrain_label = new QLabel("T: NONE");
  coord_label = new QLabel("COORD:");

  topLayout = new QHBoxLayout;
  topLayout->addWidget(id_label);
  topLayout->addStretch(2);
  topLayout->addWidget(terrain_label);
  topLayout->addStretch(2);
  topLayout->addWidget(coord_label);
  /* ------- */

  /* ------- */
  name_label= new QLabel("RN ");
  name_edit = new QLineEdit(this);

  name_edit->setAlignment(Qt::AlignLeft);

  nameLayout = new QHBoxLayout;
  nameLayout->addWidget(name_label);
  //  nameLayout->addStretch(1);
  nameLayout->addWidget(name_edit);
  /* ------- */
  desc_label= new QLabel("D ");

  desc_edit = new QLineEdit(this);

  descLayout = new QHBoxLayout;
  descLayout->addWidget(desc_label);
  //  descLayout->addStretch(1);
  descLayout->addWidget(desc_edit);

  /* --- */
  note_label= new QLabel("Note ");

  note_edit = new QLineEdit(this);

  noteLayout = new QHBoxLayout;
  noteLayout->addWidget(note_label);
  //  noteLayout->addStretch(1);
  noteLayout->addWidget(note_edit);
  /* --- */

  exits_label = new QLabel("Exits:");
  doors_label = new QLabel("Doors:");


  mainLayout = new QVBoxLayout(this);
  mainLayout->addLayout(topLayout);
  mainLayout->addLayout(nameLayout);
  mainLayout->addLayout(descLayout);
  mainLayout->addLayout(noteLayout);
  mainLayout->addWidget(doors_label);
  mainLayout->addWidget(exits_label);


  mainLayout->setMargin(11);
  mainLayout->setSpacing(6);

}

void RoomInfo::update_info(Room * rr)
{
  char str[200];

  Coordinate r;

  if (rr) r = rr->getCoordinate();

  sprintf(str, "Coord: X: %i, Y %i, Z: %i", r.x, r.y, r.z);
  coord_label->setText( str );


  if(rr)
  {
    sprintf(str, " %s ", (const char *)(Terrain::terrains[rr->getTerrain()]->desc).toLatin1());


    terrain_label->setText( str );
  }
  else
  {
    terrain_label->setText(" T: NONE ");
  }

}
