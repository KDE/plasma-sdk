/*
  Copyright (c) 2009 Riccardo Iaconelli <riccardo@kde.org>
 
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/
 
#ifndef SAVESYSTEM_H
#define SAVESYSTEM_H

class QString;
class Author;
class CommitData;

class SaveSystem
{
    public:
        SaveSystem();
        
    public slots:
        void snapshot();

};
 
#endif // SAVESYSTEM_H
