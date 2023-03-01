#include "customerqstyleditemdelegate.h"

#include <QProgressBar>
#include <QApplication>
#include <QComboBox>

#include "QDebug"

CustomerQStyledItemDelegate::CustomerQStyledItemDelegate()
{

}

void CustomerQStyledItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
        QStyleOptionViewItem viewOption(option);
        initStyleOption(&viewOption, index);
        if (option.state.testFlag(QStyle::State_HasFocus))
            viewOption.state = viewOption.state ^ QStyle::State_HasFocus;

        QStyledItemDelegate::paint(painter, viewOption, index);

        if (index.column() == 2)
        {
            int nProgress = index.model()->data(index, Qt::UserRole).toInt();
            int nLeft = 8;
            int nTop = 8;
            int nWidth = option.rect.width() - 2 * nLeft;
            int nHeight = option.rect.height() - 2 * nTop;

            // 设置进度条的风格
            QStyleOptionProgressBar progressBarOption;
            progressBarOption.initFrom(option.widget);
            // 设置进度条显示的区域
            progressBarOption.rect = QRect(option.rect.left() + nLeft, option.rect.top() + nTop,  nWidth, nHeight);
            // 设置最小值
            progressBarOption.minimum = 0;
            // 设置最大值
            progressBarOption.maximum = 100;
            // 设置对齐方式
            progressBarOption.textAlignment = Qt::AlignCenter;
            // 设置进度
            progressBarOption.progress = nProgress;
            // 设置文本（百分比）
            progressBarOption.text = QString("%1%").arg(nProgress);
            // 设置文本可见
            progressBarOption.textVisible = true;

            QProgressBar progressBar;

            //绘制进度条
            QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBarOption, painter, &progressBar);
        }
        else if(index.column()==3)
        {
        }
}

QWidget* CustomerQStyledItemDelegate::createEditor(
        QWidget *parent, const QStyleOptionViewItem &option,
        const QModelIndex &index) const
{
    if(index.column() == 3)
    {
        QComboBox *Box = new QComboBox(parent);
        Box->setFixedHeight(option.rect.height());
        Box->addItems(index.model()->data(index,Qt::UserRole).toStringList()); //向数据源申请数据，动态改变combobox的显示内容
        return Box;
    }
    else
    {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }

}

void CustomerQStyledItemDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const
{
    QString text = index.model()->data(index, Qt::UserRole).toString();
    QComboBox *comboBox =  static_cast <QComboBox*>(editor);
    comboBox->setCurrentText(text);
}


void CustomerQStyledItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    int nColumn = index.column();
    if((nColumn==3))
    {
        QComboBox *comboBox = static_cast<QComboBox*>(editor);
        if(comboBox!=0)
            model->setData(index, comboBox->currentText(), Qt::UserRole+1);
    }
    else
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}


void CustomerQStyledItemDelegate::updateEditorGeometry(QWidget *editor,
                                           const QStyleOptionViewItem &option, const QModelIndex &) const
{
    editor->setGeometry(option.rect);
}

#include <QColor>
#include <QDebug>
mTableModel::mTableModel(QObject *parent):QAbstractTableModel (parent)
{

}

void mTableModel::updateData(QList<Info>InfoList)
{
    m_infoList = InfoList;
    beginResetModel();
    endResetModel();
}

int mTableModel::rowCount(const QModelIndex &parent) const
{
    return  m_infoList.count();
}

int mTableModel::columnCount(const QModelIndex &parent) const
{
    return 4;
}

bool mTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid())
    {
        return  false;
    }
    int colum = index.column();
    Info info = m_infoList.at(index.row());
    switch (role)
    {
    case Qt::DisplayRole:
        if(colum==1)
        {
            info.path = value.toString();
            m_infoList.replace(index.row(),info);
            emit dataChanged(index,index);
            return  true;
        }
    case Qt::CheckStateRole:
        if(colum==0)
        {
            info.state = (value.toInt()==Qt::Checked);
            m_infoList.replace(index.row(),info);
            emit dataChanged(index,index);
            return true;
        }
    case Qt::UserRole:
        if(colum==2)
        {
            emit dataChanged(index,index);
            return  true;
        }

    case Qt::UserRole+1:
        if(colum==3)
        {
            info.comboBoxCurText = value.toString();
            m_infoList.replace(index.row(),info);
            emit dataChanged(index,index);
            return  true;
        }
    default:
        return false;
    }
}

QVariant mTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
    {
        return  QVariant();
    }
    int row = index.row();
    int colum = index.column();
    Info info = m_infoList.at(row);
    switch (role)
    {
    case Qt::BackgroundRole:
        return QColor(Qt::white);
    case Qt::TextColorRole:
        return QColor(Qt::black);
    case Qt::TextAlignmentRole:
        return QVariant(Qt::AlignCenter);
    case Qt::DisplayRole:
        if(colum==1)
        {
            return info.path;
        }
        if(colum==0)
            return "";
        if(colum==3)
        {
            return m_infoList.at(index.row()).comboBoxCurText.isEmpty()?"com1":m_infoList.at(index.row()).comboBoxCurText;
        }
    case Qt::UserRole:
        if(colum==2)
            return  100;
        if(colum==3)
        {
            return info.comboBoxText;  //返回的数据可让combobox动态添加item
        }
    case Qt::CheckStateRole:
        if(colum==0)
        {
            return info.state?Qt::Checked:Qt::Unchecked;
        }

    default:
        return QVariant();
    }
}

Qt::ItemFlags mTableModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
    {
        return  QAbstractItemModel::flags(index);
    }
    Qt::ItemFlags flags = Qt::ItemIsEnabled|Qt::ItemIsSelectable;
    if(index.column()==0)
    {
        flags|=Qt::ItemIsUserCheckable;
    }
    if(index.column()==3)
    {
        flags|=Qt::ItemIsDropEnabled;
        flags|=Qt::ItemIsEditable;  //可编辑，不然combobox无法下拉
    }
    return  flags;
}

QVariant mTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role)
    {
    case Qt::TextAlignmentRole:
        return QVariant( Qt::AlignCenter);
    case Qt::BackgroundRole:
        return QColor(Qt::black);
    case Qt::TextColorRole:
        return QColor(Qt::white);
    case Qt::DisplayRole:
    {
        if (orientation == Qt::Horizontal)
        {
            if (section == 0)
                return QStringLiteral("状态");

            if (section == 1)
                return QStringLiteral("名字");

            if (section == 2)
                return QStringLiteral("进度");

            if (section == 3)
                return QStringLiteral("选择");
        }
    }
    default:
        return QVariant();
    }

    return QVariant();
}

#include <QSpinBox>
SpinDelegate::SpinDelegate(QObject *parent): QItemDelegate(parent)
{

}
QWidget* SpinDelegate::createEditor(QWidget *parent
           , const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.column() == 1) // 返回下拉框
    {
       QSpinBox* box = new QSpinBox(parent);
       box->setMinimum(1);
       box->setMaximum(99);
       return box;
    }
    return QItemDelegate::createEditor(parent, option, index);
}


