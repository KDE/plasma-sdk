/*
 * This file is part of the vng project
 * Copyright (C) 2008 Thomas Zander <tzander@trolltech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Hunk.h"

#include <QDebug>
#include <QRegExp>

class SubHunk
{
public:
    SubHunk() : start(-1), length(0), accepted(Vng::Undecided), added(0), removed(0) { }
    int start, length;
    Vng::Acceptance accepted;
    int added, removed;
    int lineNumber;
};

class Hunk::Private
{
public:
    Private() : lineNumber(-1), ref(1), linesBefore(0), linesAfter(0)
    {
        SubHunk *sh = new SubHunk();
        subhunks.append(sh);
    }

    ~Private() {
        qDeleteAll(subhunks);
    }
    QByteArray header;
    int lineNumber;
    QByteArray diff;
#if QT_VERSION >= 0x040400
    QAtomicInt ref;
#else
    int ref;
#endif
    int linesBefore, linesAfter;

    QList<SubHunk *> subhunks;

    QByteArray patchForAccepted(bool inverse);
};

Hunk::Hunk()
    : d(new Private())
{
}

Hunk::Hunk(const Hunk &other)
    : d(other.d)
{
#if QT_VERSION >= 0x040400
    d->ref.ref();
#else
    d->ref++;
#endif
}

Hunk::~Hunk()
{
#if QT_VERSION >= 0x040400
    if (!d->ref.deref())
#else
    if (--d->ref == 0)
#endif
        delete d;
}

void Hunk::addLine(const QByteArray &line)
{
    if (line.size() == 0)
        return;
    if(d->header.isEmpty()) {
        d->header = line;
        QString header(line);
        QRegExp regexp("^@@ -[\\d,]+ \\+(\\d+)\\b");
        if (regexp.indexIn(header) != -1)
            d->lineNumber = regexp.cap(1).toInt();
        return;
    }
    if (line[0] == '\\') { // "\ No newline at end of file"
        if (d->diff.size()) {
            // check if we removed something from the last hunk and adjust its size
            SubHunk *last = d->subhunks[d->subhunks.count()-1];
            if (last->start + last->length == d->diff.size())
                --last->length;
            d->diff.chop(1); // TODO should that be 2 on Windows?
        }
        return;
    }
    else if (line[0] != ' ') {
        SubHunk *last = d->subhunks[d->subhunks.count()-1];
        if (last->start < 0) {
            last->start = d->diff.size();
            last->lineNumber = d->lineNumber + qMax(d->linesBefore, d->linesAfter);
        }
        else if (last->start + last->length < d->diff.size()) {
            last = new SubHunk();
            d->subhunks.append(last);
            last->start = d->diff.size();
            last->lineNumber = d->lineNumber + qMax(d->linesBefore, d->linesAfter);
        }
        last->length+=line.length();
        if (line[0] == '-') {
            last->removed++;
            d->linesBefore++;
        }
        else {
            last->added++;
            d->linesAfter++;
        }
    }
    else {
        d->linesBefore++;
        d->linesAfter++;
    }
    d->diff.append(line);
}

Hunk & Hunk::operator=(const Hunk &other)
{
#if QT_VERSION >= 0x040400
    other.d->ref.ref();
    if (! d->ref.deref())
#else
    other.d->ref++;
    if (--d->ref == 0)
#endif
        delete d;
    d = other.d;
    return *this;
}

int Hunk::lineNumber() const
{
    return d->lineNumber;
}

int Hunk::lineNumber(int subHunk) const
{
    Q_ASSERT(subHunk < d->subhunks.count());
    SubHunk *sh = d->subhunks[subHunk];
    return sh->lineNumber;
}

QByteArray Hunk::patch() const
{
    return d->diff;
}

bool Hunk::isEmpty() const
{
    return d->diff.isEmpty();
}

Vng::Acceptance Hunk::acceptance() const
{
    Vng::Acceptance a = Vng::Undecided;
    for (int i = 0; i < d->subhunks.count(); i++) {
        SubHunk *sh = d->subhunks[i];
        if (a != sh->accepted) {
            if (a == Vng::Undecided)
                a = sh->accepted;
            else
                return Vng::MixedAcceptance;
        }
    }
    return a;
}

void Hunk::setAcceptance(Vng::Acceptance acceptance)
{
    foreach(SubHunk *sh, d->subhunks)
        sh->accepted = acceptance;
}

Vng::Acceptance Hunk::acceptance(int subHunk) const
{
    Q_ASSERT(subHunk < d->subhunks.count());
    SubHunk *sh = d->subhunks[subHunk];
    return sh->accepted;
}

void Hunk::setAcceptance(int subhunk, Vng::Acceptance accepted)
{
    Q_ASSERT(subhunk < d->subhunks.count());
    SubHunk *sh = d->subhunks[subhunk];
    sh->accepted = accepted;
}

QByteArray Hunk::header() const
{
    return d->header;
}

QByteArray Hunk::subHunk(int index) const
{
    if (d->subhunks.count() == 0)
        return QByteArray();
    Q_ASSERT(index >= 0);
    Q_ASSERT(index < d->subhunks.count());
    SubHunk *sh = d->subhunks[index];
    return d->diff.mid(sh->start, sh->length);
}

int Hunk::subHunkCount() const
{
    return d->subhunks.count();
}

QByteArray Hunk::rejectedPatch() const
{
    return d->patchForAccepted(true);
}

QByteArray Hunk::acceptedPatch() const
{
    return d->patchForAccepted(false);
}

QByteArray Hunk::Private::patchForAccepted(bool inverse)
{
    QByteArray patch;
    QDataStream out (&patch, QIODevice::WriteOnly);
    const char *myDiff = diff.data();
    const char *space = " ";
    int offsetInDiff = 0; // position in diff
    int before = linesBefore; // for the header
    foreach(SubHunk *sh, subhunks) {
        out.writeRawData(myDiff + offsetInDiff, sh->start - offsetInDiff); //    write until start
        if ((!inverse && sh->accepted == Vng::Accepted) || (inverse && sh->accepted != Vng::Accepted)) {
            out.writeRawData(myDiff + sh->start, sh->length);
        }
        else {
            const int end = sh->start + sh->length;
            bool startOfLine = true;
            bool skipLine;
            int lastLineStart = 0;
            for (int x = sh->start; x < end; x++) {
                if (startOfLine) {
                    startOfLine = false;
                    skipLine = myDiff[x] == '-';
                    if (! skipLine) {
                        out.writeRawData(space, 1);
                        lastLineStart = x + 1;
                    }
                    continue;
                }
                if (myDiff[x] == '\n') {
                    startOfLine = true;
                    if (! skipLine)
                        out.writeRawData(myDiff + lastLineStart, x - lastLineStart + 1);
                }
            }
            before += sh->added - sh->removed;
        }
        offsetInDiff = sh->start + sh->length;
    }
    Q_ASSERT(offsetInDiff <= diff.size());
    out.writeRawData(myDiff + offsetInDiff, diff.size() - offsetInDiff);
    if (patch[patch.size() -1] != '\n') {
        const char *noNewLine = "\n\\ No newline at end of file\n";;
        out.writeRawData(noNewLine, strlen(noNewLine));
    }

    // create header
    QString header = "@@ -"+ QString::number(lineNumber) +","+ QString::number(before)
        +" +"+ QString::number(lineNumber) +","+  QString::number(linesAfter) + " @@\n";

    QByteArray answer;
    answer.append(header);
    answer.append(patch);
    return answer;
}

int Hunk::linesAdded() const
{
    int total = 0;
    foreach(SubHunk *sh, d->subhunks)
        total += sh->added;
    return total;
}

int Hunk::linesRemoved() const
{
    int total = 0;
    foreach(SubHunk *sh, d->subhunks)
        total += sh->removed;
    return total;
}
