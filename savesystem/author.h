/*
  Copyright (c) 2009 Riccardo Iaconelli <riccardo@kde.org>
 
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/
 
#ifndef AUTHOR_H
#define AUTHOR_H
 
class QString;

class Author
{
    public:
        Author();

        void setName(const QString &name);
        QString name();
        
        void setEmail(const QString &email);
        QString email();
};
 
#endif // AUTHOR_H
