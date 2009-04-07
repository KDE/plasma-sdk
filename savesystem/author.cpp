/*
  Copyright (c) 2009 Riccardo Iaconelli <riccardo@kde.org>
 
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "author.h"
 
Author::Author() : QObject(),
    m_name(0),
    m_email(0)
{
}

void Author::setName(const QString &name)
{
    m_name = name;
}


QString Author::name()
{
    return m_name;
}

void Author::setEmail(const QString &email)
{
    m_email = email;
}

QString Author::email()
{
    return m_email;
}



