/*
Copyright 2014 Giorgos Tsiapaliokas <giorgos.tsiapaliokas@kde.org>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef KCONFIGXTEDITORPART_H
#define KCONFIGXTEDITORPART_H

#include <KParts/ReadWritePart>

class KConfigXtEditor;

class KConfigXtEditorPart : public KParts::ReadWritePart
{
    Q_OBJECT

public:
    explicit KConfigXtEditorPart(QObject *parent = 0);
   ~KConfigXtEditorPart();

    bool openUrl(const QUrl &url) override;

protected:
    // HACK KConfigXtEditor is saving the xml file
    // without asking us. This has to be changed
    bool saveFile() override;

private:
    KConfigXtEditor *m_kconfigXtEditor;
};

#endif // KCONFIGXTEDITORPART_H
