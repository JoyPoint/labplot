/***************************************************************************
    File                 : AsciiSpreadsheetImportFilter.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2008-2009 Knut Franke
    Email (use @ for *)  : Knut.Franke*gmx.net
    Description          : Import an ASCII file as Spreadsheet.

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

#include "spreadsheet/AsciiSpreadsheetImportFilter.h"
#include "spreadsheet/Spreadsheet.h"
#include "lib/IntervalAttribute.h"
#include "core/column/Column.h"
#include "core/datatypes/String2DoubleFilter.h"

#include <QTextStream>
#include <QStringList>

QStringList AsciiSpreadsheetImportFilter::fileExtensions() const
{
	return QStringList() << "txt" << "csv" << "dat";
}

AbstractAspect * AsciiSpreadsheetImportFilter::importAspect(QIODevice * input)
{
	QTextStream stream(input);
	QStringList row, column_names;
	int i;
	// This is more efficient than it looks. The string lists are handed as-is to Column's
	// constructor, and thanks to implicit sharing the actual data is not copied.
	QList<QStringList> data;

	// skip ignored lines
	for (i=0; i<m_ignored_lines; i++)
		stream.readLine();

	// read first row
	if (m_simplify_whitespace)
		row = stream.readLine().simplified().split(m_separator);
	else if (m_trim_whitespace)
		row = stream.readLine().trimmed().split(m_separator);
	else
		row = stream.readLine().split(m_separator);

	// initialize data and determine column names
	for (int i=0; i<row.size(); i++)
		data << QStringList();
	if (m_first_row_names_columns)
		column_names = row;
	else
		for (i=0; i<row.size(); ++i) {
			column_names << QString::number(i+1);
			data[i] << row[i];
		}

	// read rest of data
	while (!stream.atEnd()) {
		if (m_simplify_whitespace)
			row = stream.readLine().simplified().split(m_separator);
		else if (m_trim_whitespace)
			row = stream.readLine().trimmed().split(m_separator);
		else
			row = stream.readLine().split(m_separator);

		for (i=0; i<row.size() && i<data.size(); ++i)
			data[i] << row[i];
		// some rows might have too few columns (re-use value of i from above loop)
		for (; i<data.size(); ++i)
			data[i] << QString();
	}

	// build a Spreadsheet from the gathered data
	// renaming will be done by the kernel
	Spreadsheet * result = new Spreadsheet(0, 0, 0, tr("Spreadsheet"));
	for (i=0; i<data.size(); ++i)
	{
		Column *new_col;
		if (m_convert_to_numeric) {
			Column * string_col = new Column(column_names[i], data[i]);
			String2DoubleFilter * filter = new String2DoubleFilter;
			filter->setNumericLocale(m_numeric_locale);
			filter->input(0, string_col);
			new_col = new Column(column_names[i], SciDAVis::Numeric);
			new_col->copy(filter->output(0));
			delete filter;
			delete string_col;
		} else
			new_col = new Column(column_names[i], data[i]);
		if (i == 0) 
			new_col->setPlotDesignation(SciDAVis::X);
		else
			new_col->setPlotDesignation(SciDAVis::Y);
		result->addChild(new_col);
	}

	return result;
}

