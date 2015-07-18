/***************************************************************************
    File                 : DataHandlersPrivate.h
    Project              : LabPlot
    Description          : 3D plot data handlers
    --------------------------------------------------------------------
    Copyright            : (C) 2015 by Minh Ngo (minh@fedoraproject.org)

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor,                    *
 *   Boston, MA  02110-1301  USA                                           *
 *                                                                         *
 ***************************************************************************/

#ifndef PLOT3D_DATAHANDLERSPRIVATE_H
#define PLOT3D_DATAHANDLERSPRIVATE_H

#include <KUrl>

class AbstractColumn;
class Matrix;

class IDataHandler;
class MatrixDataHandler;
class SpreadsheetDataHandler;
class FileDataHandler;

struct BaseDataHandlerPrivate{
	IDataHandler* const q;

	void update();
	QString name() const;

	BaseDataHandlerPrivate(IDataHandler* parent);
	virtual ~BaseDataHandlerPrivate();
};

struct MatrixDataHandlerPrivate : public BaseDataHandlerPrivate{
	const Matrix* matrix;

	MatrixDataHandlerPrivate(MatrixDataHandler *parent);
};

struct SpreadsheetDataHandlerPrivate : public BaseDataHandlerPrivate{
	const AbstractColumn *xColumn;
	const AbstractColumn *yColumn;
	const AbstractColumn *zColumn;

	const AbstractColumn *firstNode;
	const AbstractColumn *secondNode;
	const AbstractColumn *thirdNode;

	SpreadsheetDataHandlerPrivate(SpreadsheetDataHandler *parent);
};

struct FileDataHandlerPrivate : public BaseDataHandlerPrivate{
	KUrl path;

	FileDataHandlerPrivate(FileDataHandler *parent);
};

#endif