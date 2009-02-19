/*
  Copyright (c) 2009 Riccardo Iaconelli <riccardo@kde.org>
 
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/
 
#ifndef SAVESYSTEM_H
#define SAVESYSTEM_H
 
// #include <QWidget>

class QString;

class SaveSystem
{
    public:
        SaveSystem();
        
        void setAuthor(const QString &author);
        QString author();
        
        void setEmail(const QString &email);
        QString email();
        
        // inits the git repo
        void init();
        
        void add(const QString &path);
        
        void commit(const QString &log);
};
 
#endif // SAVESYSTEM_H
