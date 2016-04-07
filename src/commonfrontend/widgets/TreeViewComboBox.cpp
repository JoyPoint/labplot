/***************************************************************************
    File                 : TreeViewComboBox.cpp
    Project              : LabPlot
    Description          : Provides a QTreeView in a QComboBox
    --------------------------------------------------------------------
    Copyright            : (C) 2008-2014 by Alexander Semke (alexander.semke@web.de)
    Copyright            : (C) 2008 Tilman Benkert (thzs@gmx.net)

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

#include "commonfrontend/widgets/TreeViewComboBox.h"
#include "backend/core/AbstractAspect.h"
#include "backend/core/AspectTreeModel.h"
#include <QHeaderView>
#include <qlayout.h>
#include <QDebug>
#include <KLocalizedString>
#include <KLineEdit>

/*!
    \class TreeViewComboBox
    \brief Provides a QTreeView in a QComboBox.

    \ingroup backend/widgets
*/


TreeViewComboBox::TreeViewComboBox(QWidget* parent):QComboBox(parent){
	
	m_layout = new QVBoxLayout;
	m_treeView = new QTreeView;
	m_lineEdit = new KLineEdit;
	m_groupBox = new QGroupBox;
	
	m_layout->addWidget(m_lineEdit);
	m_layout->addWidget(m_treeView);
	
	m_groupBox->setLayout(m_layout);
	m_groupBox->setParent(parent, Qt::Popup);
	m_groupBox->hide();
	m_groupBox->installEventFilter(this);
	
	m_treeView->header()->hide();
	m_treeView->setSelectionMode(QAbstractItemView::SingleSelection);
	m_treeView->setUniformRowHeights(true);
	
	m_lineEdit->setPlaceholderText(i18n("Search/Filter Options"));
	qobject_cast<KLineEdit*>(m_lineEdit)->setClearButtonShown(true);
	m_lineEdit->setFocus();

	addItem("");
	setCurrentIndex(0);

	connect(m_treeView, SIGNAL(activated(QModelIndex)), this, SLOT(treeViewIndexActivated(QModelIndex)) );
	connect(m_lineEdit, SIGNAL(textChanged(QString)), this, SLOT(lineEditTextEdited(QString)));
}

void TreeViewComboBox::setTopLevelClasses(QList<const char*> list) {
	m_topLevelClasses=list;
}


void TreeViewComboBox::setSelectableClasses(QList<const char*> list) {
	m_selectableClasses=list;
}

/*!
	Sets the \a model for the view to present.
*/
void TreeViewComboBox::setModel(QAbstractItemModel *model){
	m_treeView->setModel(model);

	//show only the first column in the combo box
	for (int i=1; i<model->columnCount(); i++){
	  m_treeView->hideColumn(i);
	}

	//Expand the complete tree in order to see everything in the first popup.
	m_treeView->expandAll();
}

/*!
	Sets the current item to be the item at \a index and selects it.
	\sa currentIndex()
*/
void TreeViewComboBox::setCurrentModelIndex(const QModelIndex& index){
// 	view()->setCurrentIndex(index);
	m_treeView->setCurrentIndex(index);
	QComboBox::setItemText(0, index.data().toString());
}

/*!
	Returns the model index of the current item.

	\sa setCurrentModelIndex()
*/
QModelIndex TreeViewComboBox::currentModelIndex() const{
	return m_treeView->currentIndex();
}

/*!
	Displays the tree view of items in the combobox.
	Triggers showTopLevelOnly() to show toplevel items only.
*/
void TreeViewComboBox::showPopup(){
	if (!m_treeView->model() || !m_treeView->model()->hasChildren())
		return;

	QModelIndex root = m_treeView->model()->index(0,0);
	showTopLevelOnly(root);
	
	m_groupBox->show();
	m_groupBox->resize(this->width(), 250);
	m_groupBox->move(mapToGlobal( this->rect().topLeft() ));
}

void TreeViewComboBox::hidePopup() {
	m_groupBox->hide();
}


/*!
	Hides the non-toplevel items of the model used in the tree view.
*/
void TreeViewComboBox::showTopLevelOnly(const QModelIndex & index){
	int rows = index.model()->rowCount(index);
	QModelIndex currentChild;
	bool isTopLevel;
	for (int i=0; i<rows; i++) {
		currentChild = index.child(i, 0);
		showTopLevelOnly(currentChild);
		AbstractAspect* aspect =  static_cast<AbstractAspect*>(currentChild.internalPointer());
		isTopLevel = false;
		foreach(const char * classString, m_topLevelClasses)
			if (aspect->inherits(classString)) {
				if ( strcmp(classString, "Spreadsheet")==0 ) {
					if (aspect->inherits("FileDataSource"))
						isTopLevel = false;
					else
						isTopLevel = true;
				} else {
					isTopLevel = true;
				}
			}

		m_treeView->setRowHidden(i, index, !isTopLevel);
	}
}

/*!
	catches the MouseButtonPress-event and hides the tree view on mouse clicking.
*/
bool TreeViewComboBox::eventFilter(QObject *object, QEvent *event){
	if ( (object==m_groupBox) && event->type()==QEvent::MouseButtonPress){
		m_groupBox->hide();
		this->setFocus();
		return true;
	}
	return false;
}

//SLOTs

void TreeViewComboBox::treeViewIndexActivated( const QModelIndex & index){
	if (index.internalPointer()) {
		AbstractAspect* aspect =  static_cast<AbstractAspect*>(index.internalPointer());
		const char* currentClassName = aspect->metaObject()->className();
		foreach(const char* className, m_selectableClasses) {
			if ( strcmp(currentClassName, className)==0 ) {
				QComboBox::setCurrentIndex(0);
				QComboBox::setItemText(0, index.data().toString());
				emit currentModelIndexChanged(index);
// 				m_treeView->hide();
				m_groupBox->hide();
				return;
			}
		}
	}

	m_treeView->setCurrentIndex(QModelIndex());
	setCurrentIndex(0);
	QComboBox::setItemText(0, "");
	emit currentModelIndexChanged(QModelIndex());
// 	m_treeView->hide();
	m_groupBox->hide();
}

void TreeViewComboBox::lineEditTextEdited(const QString &text) {
	AspectTreeModel * model = qobject_cast<AspectTreeModel *>(m_treeView->model());
	if(!model)
		return;

	model->setFilterString(text);
	m_treeView->update();
}
