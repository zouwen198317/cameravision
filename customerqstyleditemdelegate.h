#ifndef CUSTOMERQSTYLEDITEMDELEGATE_H
#define CUSTOMERQSTYLEDITEMDELEGATE_H

#include <QMainWindow>
#include <QStyledItemDelegate>


#include <QAbstractTableModel>
struct Info
{
    QString path;
    bool state;
    QStringList comboBoxText;
    QString comboBoxCurText;
};

class mTableModel : public QAbstractTableModel
{
public:
    mTableModel(QObject *parent);
    void updateData(QList<Info> fileRecodeList);

    QList<Info>m_infoList;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    int columnCount(const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
};


class CustomerQStyledItemDelegate : public QStyledItemDelegate
{
public:
    CustomerQStyledItemDelegate();
    ~CustomerQStyledItemDelegate(){}
protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;

};




#include <QItemDelegate>
class SpinDelegate : public QItemDelegate
{
public:
    SpinDelegate(QObject *parent = Q_NULLPTR);
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};






#endif // CUSTOMERQSTYLEDITEMDELEGATE_H
