#ifndef NODEPROPERTYWIDGET_H
#define NODEPROPERTYWIDGET_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QDockWidget>

#include <qtpropertybrowser.h>
#include <qtpropertymanager.h>
#include <qtvariantproperty.h>
#include <qttreepropertybrowser.h>

QT_BEGIN_NAMESPACE

class QDockWidget;
class QtVariantPropertyManager;
class QtProperty;
class QtVariantProperty;
class QtVariantEditorFactory;
class QtTreePropertyBrowser;
QT_END_NAMESPACE


class NodePropertyWidget : public QDockWidget
{
    Q_OBJECT
public:
    NodePropertyWidget();
};

#endif // NODEPROPERTYWIDGET_H
