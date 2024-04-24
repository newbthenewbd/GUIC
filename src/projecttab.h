#ifndef PROJECTTAB_H
#define PROJECTTAB_H

#define W_NO_PROPERTY_MACRO
#include <wobjectdefs.h>
#include <QWidget>
#include "imagelistitem.h"

namespace Ui {
    class ProjectTab;
}

class ProjectTab : public QWidget
{
    //Q_OBJECT
    W_OBJECT(ProjectTab)
    
    public:
    explicit ProjectTab(QWidget *parent = nullptr);
    ~ProjectTab();
    
    public slots:
    void addImages();
    void addImagesFromPaths(QStringList paths);
    void imageListItemSelected(QListWidgetItem* imageListItem);
    
    private:
    Ui::ProjectTab *ui;
};

#endif // PROJECTTAB_H
