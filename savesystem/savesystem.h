/*
  Copyright (c) 2009 Riccardo Iaconelli <riccardo@kde.org>
 
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/
 
#ifndef SAVESYSTEM_H
#define SAVESYSTEM_H

#include <QObject>

class QString;

class SaveSystem : public QObject
{
    Q_OBJECT
    public:
        SaveSystem(QObject *parent = 0);

        bool canRollback(); // if we have a clean index, other things...

    public slots:
        void snapshot();

};
 
#endif // SAVESYSTEM_H
