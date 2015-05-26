#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::addDirectory(QTreeWidgetItem *parent,QString Name, QString Size, QString Hash,QVector<QTreeWidgetItem*> &vec)
{
    QTreeWidgetItem *itm = new QTreeWidgetItem;
    itm->setText(0,Name);
    itm->setText(1,"");
    itm->setText(2,"");
    parent->addChild(itm);
    vec.push_back(itm);
}

void MainWindow::addFile(QTreeWidgetItem *parent, QString Name, QString Size, QString Hash)
{
    QTreeWidgetItem *itm = new QTreeWidgetItem;
    itm->setText(0,Name);
    itm->setText(1,Size);
    itm->setText(2,Hash);
    parent->addChild(itm);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    ui->lineEdit->setText(QFileDialog::getOpenFileName(0,QObject::tr("Открыть файл"),QDir::homePath(), QObject::tr("(*.tsv)")));
}

void MainWindow::on_pushButton_3_clicked()
{
        ui->treeWidget->clear();
        QFile f(ui->lineEdit->text());
        if(!f.open(QFile::ReadOnly | QFile::Text))
        {
            QMessageBox::information(this,"Error","Path isn't correct");
            return;
        }

        QTextStream stream(&f);
        QVector <QString> Path;
        QVector <QString> Names;
        QVector <QString> Sizes;
        QVector <QString> Hash;
        bool flag = false;
        QString temp;
        while(!stream.atEnd())
        {
           temp = stream.readLine();
           if(temp == "Path\tFile name\tFile size\tHash")
           {
               continue;
           }
           QString dir = "";
           QString Name = "";
           QString Size = "";
           QString adlerHash = "";
           int counter = 0;
           for(int i = 0; i<temp.size(); i++)
           {

               if(temp[i]=='\t')
               {
                   counter++;
                   continue;
               }
               switch(counter)
               {

               case 0:
                   dir = dir + temp[i];
                   break;

               case 1:
                   Name = Name + temp[i];
                   break;

               case 2:
                   Size = Size + temp[i];

               case 3:
                    adlerHash = adlerHash + temp[i];
                    break;

               default:

                   break;

               }

           }
           if(dir==""||adlerHash==""||Name==""||Size=="")
                   {
                       break;
                   }
           Path.push_back(dir);
           Names.push_back(Name);
           Sizes.push_back(Size);
           Hash.push_back(adlerHash);
        }
        int Min;
        int Max;
        QVector <int> deep;
        for(int i = 0;i<Path.size();i++)
        {
            int counter = 0;
            QString str = Path[i];
            for(int k = 0; k<str.length();k++)
            {
                if (QChar(str[k])==92){
                    counter++;
                }


            }
            deep.push_back(counter);
            if(!flag)
            {
                Min = counter;
                Max = counter;
                flag = true;
            }
            if(counter<Min)
                Min = counter;
            if(counter>Max)
                Max = counter;
        }
        flag = false;
        int counter = 0;
        QString Name = "";
        QString str = Path[0];
        for(int i = 0;i<str.length();i++)
        {
            if(QChar(str[i])==92)
                counter++;
            if(counter==(Min-1))
            {
                Name+=str[i];
            }
            if(counter>=Min)
            {
                break;
            }
        }

        QTreeWidgetItem *itm1 = new QTreeWidgetItem;
        itm1->setText(0,Name);
        itm1->setText(1,"");
        itm1->setText(2,"");
        ui->treeWidget->addTopLevelItem(itm1);
        QVector <QString> tmpN;
        QVector <QTreeWidgetItem*> tree;
        tmpN.push_back(Name);
        tree.push_back(itm1);
        for(int i = 0;i<Path.size();i++)
        {
            for(int k = Min; k<deep[i];k++)
            {
                QString Name1 = "";
                Name = "";
                counter = 0;
                if (deep[i]-k >= 1)
                {
                    for (int j = 0;j<Path[i].length();j++)
                    {
                        if(QChar(Path[i][j])==92)
                            counter++;
                        if(counter==k-1)
                        {
                            Name1+=Path[i][j];
                        }
                        if(counter==k)
                        {
                            Name+=Path[i][j];
                        }
                        if(counter>k)
                        {
                            break;
                        }

                    }
                    bool flag1 = false;
                    bool flag2 = false;
                    for(int j = 0; j<tmpN.size();j++)
                    {
                        if (tmpN[j]==Name)
                        {
                            flag1 = true;
                        }
                        if(tmpN[j]==Name1)
                        {
                            itm1 = tree[j];
                            flag2 = true;
                        }
                        if(flag1&&flag2)
                            break;
                    }

                    if(flag1&&flag2)
                    {
                        continue;
                    }

                    if(flag1&&!flag2)
                    {
                        QMessageBox::information(this,"Error","What the fuck?");
                    }

                    if(!flag1&&!flag2)
                    {
                        QMessageBox::information(this,"Error","What the fuck?ver2");
                    }

                    if(!flag1&&flag2)
                    {
                        tmpN.push_back(Name);

                        addDirectory(itm1,Name,"","",tree);
                    }
                }
            }
        }
        for(int i=0;i<Path.size();i++)
        {
            QString Name1 = "";
            counter = 0;
            for(int k = 0;k<Path[i].length();k++)
            {
                if(QChar(Path[i][k])==92)
                    counter++;
                if(counter==deep[i]-1)
                {
                    Name1+=Path[i][k];
                }

                if(counter>deep[i])
                {
                    break;
                }
            }
            for(int k = 0;k<tmpN.length();k++)
            {
                if(tmpN[k]==Name1)
                {
                    addFile(tree[k],Names[i],Sizes[i],Hash[i]);
                }
            }

        }

}
