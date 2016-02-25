/**
 * \file   erdoswindow.cpp
 * \author Michał Kamiński
 * \date   20-01-2015
 * \brief  Klasa odpowiadajaca za okno programu
 */


#include "erdoswindow.h"
#include "ui_erdoswindow.h"
#include "authorsgraph.h"
#include <QMessageBox>
#include <QLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QFileDialog>
#include <ctime>
#include <cmath>


ErdosWindow::ErdosWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ErdosWindow)
{
    //Wstepne przygotowanie elementow GUI
    ui->setupUi(this);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidgetRandom->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    bar = new QProgressBar(ui->statusBar);
    ui->statusBar->addPermanentWidget(bar);
    bar->setVisible(false);
    ui->stackedWidget->setCurrentIndex(0);
    ui->algorithmButtonGroup->setId(ui->bfsButton,0);
    ui->algorithmButtonGroup->setId(ui->dijkstraButton,1);

    ui->loadingButtonGroup->setId(ui->authorsPublicationsButton,0);
    ui->loadingButtonGroup->setId(ui->publicationsButton,1);
    ui->treeWidget->setColumnCount(1);
}

ErdosWindow::~ErdosWindow()
{
    delete ui;
}



void ErdosWindow::on_analyzeButton_clicked()
{
    int startCount = ui->nSpinBox->value(); //pobierz parametry ze spinboxow
    int step = ui->stepSpinBox->value();
    int stepNo = ui->countSpinBox->value();

    if (startCount + step*stepNo >3000)
    {
        QMessageBox::warning(this,"Ostrzeżenie", "Wykonanie analizy dla podanych parametrów może zająć więcej czasu");
    }

    //Ustalanie rodzaju analizy
    if(ui->comboBox->currentIndex() == 0) //Analiza ladowania
    {
        Loading mode;

        if(ui->loadingButtonGroup->checkedId()==0) //pelne ladowanie
            mode = FULL;
        else //ladowanie po publikacjach
            mode = PUBLICATIONSONLY;

        analyzeLoading(startCount,step,stepNo,mode);
    }

    else if(ui->comboBox->currentIndex() == 1)// analiza szukania rangi
    {
        Algorithm mode;

        if(ui->algorithmButtonGroup->checkedId()==0)//analiza bfs
            mode = BFS;
        else//analiza dijkstra
            mode = DIJKSTRA;

        analyzeAlgorithm(startCount,step,stepNo, mode);

    }

    else // pelna analiza
    {
        Loading loadMode;
        Algorithm algMode;

        if(ui->loadingButtonGroup->checkedId()==0) //pelne ladowanie
            loadMode = FULL;
        else //ladowanie po publikacjach
            loadMode = PUBLICATIONSONLY;

        if(ui->algorithmButtonGroup->checkedId()==0)//analiza bfs
            algMode = BFS;
        else//analiza dijkstra
            algMode = DIJKSTRA;

        fullAnalisis(startCount,step,stepNo,loadMode,algMode);

    }

}

int ErdosWindow::generateInput(int n, std::vector<std::string> &authors, std::vector<std::vector<std::string>>& publications)
{
    int edgeCounter = 0;//licznik krawedzi
    int n1;
    set<int> nodes;

    if (n<10)
        return 0;

    authors.clear();
    publications.clear();

    authors.push_back("Erdos"); //Dodanie Erdosa

    for (int i = 1; i<n; ++i) //Tworzenie autorow
        authors.push_back(to_string(i));

    for(int i = n-1; i>0; --i) //Losowa generacja okolo n*(n-1)/4 krawedzi
    {
        if(i == 1)
        {
            publications.push_back(vector<string>()={to_string(i),"Erdos"});
            ++edgeCounter;
        }
        else if(i == 2)
        {
            n1 = rand() % i;
            if(n1 == 0)
                publications.push_back(vector<string>()={to_string(i),"Erdos"});
            else
                publications.push_back(vector<string>()={to_string(i),to_string(n1)});

            ++edgeCounter;
        }

        else
        {
            nodes.clear();

            for(int j=0; j< i/2; ++j)
            {
                do
                {
                    n1 = rand() % i;
                }while(nodes.insert(n1).second != true);

                ++edgeCounter;
            }

            for(auto node: nodes)
            {
                if(node == 0)
                    publications.push_back(vector<string>()={to_string(i),"Erdos"});
                else
                    publications.push_back(vector<string>()={to_string(i),to_string(node)});
            }
        }
    }

    return edgeCounter;
}

void ErdosWindow::analyzeLoading (int startCount, int step, int stepNo, Loading loadMode)
{
    QTableWidgetItem * newItem;
    vector<double> times;
    vector<int> edgeCounter;
    double medianTime;//t(n) mediany
    double medianTheory; //T(n) mediany
    double q;

    AuthorsGraph graph;
    vector<string> authors;
    vector<vector<string>> publications;

    bar->setVisible(true); //pokaz pasek ladowania
    bar->setValue(0);
    ui->statusBar->showMessage("Trwa analiza",0);

    //Obliczanie czasow analizy
    for(int i = 0 ;i<stepNo;++i)
    {
        clock_t t;
        int a = generateInput(startCount + i*step,authors,publications);
        edgeCounter.push_back(a);

        if(loadMode == FULL)
        {
            t = clock();
            graph.loadGraph(authors,publications);
            t = clock() - t;
        }
        else
        {
            t = clock();
            graph.loadGraph(publications);
            t = clock() - t;
        }
        times.push_back(1000*((double)t)/CLOCKS_PER_SEC);
        bar->setValue(100*i/stepNo);
    }

    //Wyznaczanie mediany probek
    if(stepNo%2 ==0)
    {
        int m1 = stepNo/2;
        int m2 = m1-1;
        medianTime = (times[m1] + times[m2])/2;
        medianTheory = (edgeCounter[m1] * log2(startCount+m1*step) + edgeCounter[m2] * log2(startCount+m2*step))/2;
    }

    else
    {
        int median = stepNo/2;
        medianTime = times[median];
        medianTheory = edgeCounter[median] * log2(startCount+median*step);
    }

    //obliczanie wyświetlanie wyników
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(stepNo);
    if(loadMode == FULL)
        ui->analisisTypeLabel->setText("Pełne Ładowanie");
    else
        ui->analisisTypeLabel->setText("Ładowanie Publikacji");

    ui->complexityLabel->setText("v log(n)");

    //Obliczanie q i wyswietlanie n,v,t(n) i q
    for(int i =0; i<stepNo; ++i)
    {
        q = times[i] / (edgeCounter[i] * log2(startCount+i*step)) *medianTheory / medianTime;

        newItem = new QTableWidgetItem(QString::number(startCount+i*step)); // wyswietlanie liczby wezlow
        ui->tableWidget->setItem(i, 0, newItem);

        newItem = new QTableWidgetItem(QString::number(edgeCounter[i])); // wyswietlanie liczby krawedzi
        ui->tableWidget->setItem(i, 1, newItem);

        newItem = new QTableWidgetItem(QString::number(times[i],'f',0)); // wyswietlanie czasu
        ui->tableWidget->setItem(i, 2, newItem);

        newItem = new QTableWidgetItem(QString::number(q,'f',3)); // wyswietlanie q
        ui->tableWidget->setItem(i, 3, newItem);
    }

    ui->statusBar->clearMessage();
    bar->setVisible(false);
    ui->stackedWidget->setCurrentIndex(1);
    ui->tableWidget->horizontalHeader()->setVisible(true);
}

void ErdosWindow::analyzeAlgorithm (int startCount, int step, int stepNo, Algorithm algorithm)
{
    QTableWidgetItem * newItem;
    vector<double> times;
    vector<int> edgeCounter;
    double medianTime; //medianowe t(n)
    double medianTheory; //medianowe T(n)
    double q;

    AuthorsGraph graph;
    vector<string> authors;
    vector<vector<string>> publications;

    bar->setVisible(true);
    bar->setValue(0);
    ui->statusBar->showMessage("Trwa analiza",0);

    //Obliczanie czasow analizy
    for(int i = 0 ;i<stepNo;++i)
    {
        clock_t t;
        int a = generateInput(startCount + i*step,authors,publications);
        edgeCounter.push_back(a);
        graph.loadGraph(publications);

        if(algorithm == BFS)
        {
            t = clock();
            auto result = graph.getRanksBFS();
            t = clock() - t;
            delete result;
        }
        else
        {
            t = clock();
            auto result = graph.getRanksDijkstry();
            t = clock() - t;
            delete result;
        }
        times.push_back(1000*((double)t)/CLOCKS_PER_SEC);
        bar->setValue(100*i/stepNo);
    }

    //Wyznaczanie mediany
    if(stepNo%2 ==0)
    {
        int m1 = stepNo/2;
        int m2 = m1-1;
        medianTime = (times[m1] + times[m2])/2;

        if(algorithm == BFS)
            medianTheory = (startCount+m1*step + edgeCounter[m1] + startCount+m2*step + edgeCounter[m2])/2;
        else
            medianTheory = (edgeCounter[m1] * log2(startCount+m1*step) + edgeCounter[m2] * log2(startCount+m2*step))/2;
    }

    else
    {
        int median = stepNo/2;
        medianTime = times[median];

        if(algorithm == BFS)
            medianTheory = startCount+median*step + edgeCounter[median];
        else
            medianTheory = edgeCounter[median] * log2(startCount+median*step);
    }

    //wyświetlanie wyników
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(stepNo);

    if(algorithm == BFS)
    {
        ui->analisisTypeLabel->setText("BFS");
        ui->complexityLabel->setText("v + n");
    }
    else
    {
        ui->analisisTypeLabel->setText("Dijkstra");
        ui->complexityLabel->setText("v log(n)");
    }


    //Obliczanie q oraz wyswietlanie n,v,t(n),q
    for(int i =0; i<stepNo; ++i)
    {
        if(algorithm == BFS)
            q = times[i]/(edgeCounter[i] + startCount + i*step) *medianTheory / medianTime;
        else
            q = times[i] / (edgeCounter[i] * log2(startCount+i*step)) *medianTheory / medianTime;

        newItem = new QTableWidgetItem(QString::number(startCount+i*step)); // wyswietlanie liczby wezlow
        ui->tableWidget->setItem(i, 0, newItem);

        newItem = new QTableWidgetItem(QString::number(edgeCounter[i])); // wyswietlanie liczby krawedzi
        ui->tableWidget->setItem(i, 1, newItem);

        newItem = new QTableWidgetItem(QString::number(times[i],'f',0)); // wyswietlanie czasu
        ui->tableWidget->setItem(i, 2, newItem);

        newItem = new QTableWidgetItem(QString::number(q,'f',3)); // wyswietlanie q
        ui->tableWidget->setItem(i, 3, newItem);
    }

    ui->statusBar->clearMessage();
    bar->setVisible(false);
    ui->stackedWidget->setCurrentIndex(1);
    ui->tableWidget->horizontalHeader()->setVisible(true);
}

void ErdosWindow::fullAnalisis (int startCount, int step, int stepNo, Loading loadMode, Algorithm algorithm)
{
    QTableWidgetItem * newItem;
    vector<double> times;
    vector<int> edgeCounter;
    double medianTime;
    double medianTheory;
    double q;

    AuthorsGraph graph;
    vector<string> authors;
    vector<vector<string>> publications;

    bar->setVisible(true);
    bar->setValue(0);
    ui->statusBar->showMessage("Trwa analiza",0);

    //Obliczanie czasow analizy
    for(int i = 0 ;i<stepNo;++i)
    {
        clock_t t;
        int a = generateInput(startCount + i*step,authors,publications);
        edgeCounter.push_back(a);

        t = clock();
        if(loadMode == FULL)
            graph.loadGraph(authors,publications);
        else
            graph.loadGraph(publications);

        if(algorithm ==BFS)
        {
            auto result = graph.getRanksBFS();
            delete result;
        }
        else
        {
            auto result = graph.getRanksDijkstry();
            delete result;
        }

        t = clock() - t;
        times.push_back(1000*((double)t)/CLOCKS_PER_SEC);
        bar->setValue(100*i/stepNo);
    }

    //Wyznaczanie mediany
    if(stepNo%2 ==0)
    {
        int m1 = stepNo/2;
        int m2 = m1-1;
        medianTime = (times[m1] + times[m2])/2;
        medianTheory = (edgeCounter[m1] * log2(startCount+m1*step) + edgeCounter[m2] * log2(startCount+m2*step))/2;
    }

    else
    {
        int median = stepNo/2;
        medianTime = times[median];
        medianTheory = edgeCounter[median] * log2(startCount+median*step);
    }

    //wyświetlanie wyników
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(stepNo);


    if(loadMode == FULL)
    {
        if(algorithm == BFS)
            ui->analisisTypeLabel->setText("Pełne Ładowanie + BFS");
        else
           ui->analisisTypeLabel->setText("Pełne Ładowanie + Dijkstra");
    }

    else
    {
        if(algorithm == BFS)
            ui->analisisTypeLabel->setText("Ładowanie publikacji + BFS");
        else
           ui->analisisTypeLabel->setText("Ładowanie publikacji + Dijkstra");
    }

    ui->complexityLabel->setText("v log(n)");

    //Obliczanie q oraz wyswietlanie n,v,t(n),q
    for(int i =0; i<stepNo; ++i)
    {
        q = times[i] / (edgeCounter[i] * log2(startCount+i*step)) *medianTheory / medianTime;

        newItem = new QTableWidgetItem(QString::number(startCount+i*step)); // wyswietlanie liczby wezlow
        ui->tableWidget->setItem(i, 0, newItem);

        newItem = new QTableWidgetItem(QString::number(edgeCounter[i])); // wyswietlanie liczby krawedzi
        ui->tableWidget->setItem(i, 1, newItem);

        newItem = new QTableWidgetItem(QString::number(times[i],'f',0)); // wyswietlanie czasu
        ui->tableWidget->setItem(i, 2, newItem);

        newItem = new QTableWidgetItem(QString::number(q,'f',3)); // wyswietlanie q
        ui->tableWidget->setItem(i, 3, newItem);
    }

    ui->statusBar->clearMessage();
    bar->setVisible(false);
    ui->stackedWidget->setCurrentIndex(1);
    ui->tableWidget->horizontalHeader()->setVisible(true);
}

void ErdosWindow::getXMLSolution()
{
    AuthorsGraph graph;
    vector<pair<string,int>> * result = nullptr;
    QHBoxLayout *layout;
    QTableWidget* table;
    QWidget *page;
    QStringList labels;
    QTableWidgetItem* newItem;
    int scenarioCount = scenarios.size();
    int i =1;

    labels<<"Nazwisko"<<"Ranga";

    if(scenarioCount==0)
        return;

    ui->xmlTabWidget->clear();
    for(auto scenario: scenarios)
    {
        if(ui->loadingButtonGroup->checkedId()==0) //pelne ladowanie
            graph.loadGraph(scenario.first,scenario.second);
        else //ladowanie po publikacjach
            graph.loadGraph(scenario.second);

        if(ui->algorithmButtonGroup->checkedId()==0)//bfs
            result = graph.getRanksBFS();
        else//dijkstra
            result = graph.getRanksDijkstry();

        //wyswietlanie wynikow
        layout = new QHBoxLayout();
        table = new QTableWidget();
        table->setRowCount(result->size());
        table->setColumnCount(2);
        table->setHorizontalHeaderLabels(labels);
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        layout->addWidget(table);
        page = new QWidget();
        page->setLayout(layout);
        ui->xmlTabWidget->addTab(page,"Scenariusz "+QString::number(i));
        ++i;

        for(unsigned int j =0; j<result->size();++j)
        {
            newItem = new QTableWidgetItem(QString::fromStdString(result->at(j).first)); // wyswietlanie liczby wezlow
            table->setItem(j, 0, newItem);

            newItem = new QTableWidgetItem(QString::number(result->at(j).second)); // wyswietlanie liczby krawedzi
            table->setItem(j, 1, newItem);
        }

    }

    if(result != nullptr)
        delete result;

    ui->stackedWidget->setCurrentIndex(3);

}

void ErdosWindow::getRandomSolution()
{
    AuthorsGraph graph;
    Authors authors;
    Publications publications;
    vector<pair<string,int>> * result;
    QTableWidgetItem *newItem;
    QTreeWidgetItem *root;
    QTreeWidgetItem *authorsRoot;
    QTreeWidgetItem *publicationsRoot;
    QTreeWidgetItem *publicationRoot;
    int size = ui->randomSizeSpinBox->value();

    generateInput(size,authors,publications);

    if(ui->loadingButtonGroup->checkedId()==0) //pelne ladowanie
        graph.loadGraph(authors,publications);
    else //ladowanie po publikacjach
        graph.loadGraph(publications);

    if(ui->algorithmButtonGroup->checkedId()==0)//bfs
        result = graph.getRanksBFS();
    else//dijkstra
        result = graph.getRanksDijkstry();

    //Wyswietlanie struktury wylosowanych danych
    ui->treeWidget->clear();
    root = new QTreeWidgetItem(ui->treeWidget);
    root->setText(0,"Scenariusz");
    ui->treeWidget->addTopLevelItem(root);

    authorsRoot = new QTreeWidgetItem();
    authorsRoot->setText(0,"Autorzy");
    root->addChild(authorsRoot);

    publicationsRoot = new QTreeWidgetItem();
    publicationsRoot->setText(0,"Publikacje");
    root->addChild(publicationsRoot);

    //dodawanie autorów

    for(auto author: authors)
    {
        QTreeWidgetItem * authorItem = new QTreeWidgetItem();
        authorItem->setText(0,QString::fromStdString(author));
        authorsRoot->addChild(authorItem);
    }

    int j = 1;
    for(auto publication: publications) //dodawanie publikacji
    {
        publicationRoot = new QTreeWidgetItem();
        publicationRoot->setText(0,"Publikacja "+ QString::number(j));
        publicationsRoot->addChild(publicationRoot);

        for(auto author: publication)
        {
            QTreeWidgetItem * authorItem = new QTreeWidgetItem();
            authorItem->setText(0,QString::fromStdString(author));
            publicationRoot->addChild(authorItem);
        }
        ++j;
    }


    ui->tableWidgetRandom->clearContents();
    ui->tableWidgetRandom->setRowCount(result->size());

    for(unsigned int j =0; j<result->size();++j)
    {
        newItem = new QTableWidgetItem(QString::fromStdString(result->at(j).first)); // wyswietlanie liczby wezlow
        ui->tableWidgetRandom->setItem(j, 0, newItem);

        newItem = new QTableWidgetItem(QString::number(result->at(j).second)); // wyswietlanie liczby krawedzi
        ui->tableWidgetRandom->setItem(j, 1, newItem);
    }

    ui->stackedWidget->setCurrentIndex(2);
    delete result;
}

void ErdosWindow::on_backButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void ErdosWindow::on_solveXMLButton_clicked()
{
    QDomDocument document;
    QString fileName = QFileDialog::getOpenFileName(nullptr, "Open File",
                                                    "C://",
                                                    "Plik Xml (*.xml)");

    QFile file(fileName);

       if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
       {
           QMessageBox::critical(this,"Bład","Nieudana próba otwarcia pliku");
           return;
       }
       else
       {
           if(!document.setContent(&file))
           {
               QMessageBox::critical(this,"Bład","Nieudana próba załadowania pliku XML");
               return;
           }
           file.close();
       }

       QDomElement root = document.firstChildElement();


       if(!loadScenarios(root))
         QMessageBox::critical(this,"Bład","Nieudana próba załadowania pliku XML");
       else
       {
           getXMLSolution();
       }

}

bool ErdosWindow::loadScenarios(QDomElement root)
{
    scenarios.clear();
    QDomNodeList scenarioList = root.elementsByTagName("Scenario");

     if(scenarioList.count() ==0 )
         return false;

     for(int i = 0; i < scenarioList.count(); i++)
     {
        QDomNode itemnode = scenarioList.at(i);

        //convert to element
        if(itemnode.isElement())
        {
            QDomElement element = itemnode.toElement();
            if(!loadScenario(element))
                return false;
        }
     }
     return true;
}

bool ErdosWindow::loadScenario(QDomElement root)
{
    //Ladowanie autorow
    pair<Authors,Publications> scenario;
    QDomNodeList authorList = root.elementsByTagName("Authors");

     if(authorList.count() !=1 )
         return false;

     QDomNode node = authorList.at(0);
     QDomElement authorsRoot = node.toElement();
     if(!loadAuthors(authorsRoot,scenario.first))
        return false;


     //Ladowanie publikacji
     QDomNodeList publicationList = root.elementsByTagName("Publications");

      if(publicationList.count() !=1 )
          return false;

      node = publicationList.at(0);
      QDomElement publicationRoot = node.toElement();
      if(!loadPublications(publicationRoot,scenario.second))
         return false;

     scenarios.push_back(scenario);
     return true;

}

bool ErdosWindow::loadAuthors(QDomElement root, Authors& authorList)
{
    bool erdosSpotted =false;
    QDomNodeList authorNodeList = root.elementsByTagName("Author");

     if(authorNodeList.count() ==0 )
         return false;

     for(int i = 0; i < authorNodeList.count(); i++)
     {
        QDomNode node = authorNodeList.at(i);

        if(node.isElement())
        {
            QDomElement element = node.toElement();
            auto name = element.attribute("Name").toStdString();

            if(name == "Erdos")
                erdosSpotted = true;

            authorList.push_back(name);

        }
     }
     return erdosSpotted;
}

bool ErdosWindow::loadPublications(QDomElement root, Publications& publicationsList)
{
     Publication publication;

     QDomNodeList publicationsNodeList = root.elementsByTagName("Publication");

     if(publicationsNodeList.count() ==0 )
          return false;

      for(int i = 0; i < publicationsNodeList.count(); i++)
      {
         QDomNode node = publicationsNodeList.at(i);

         if(node.isElement())
         {
             publication.clear();
             QDomElement authorsRoot = node.toElement();
             if(!loadPublication(authorsRoot,publication))
                return false;

             publicationsList.push_back(publication);

         }
      }
      return true;

}

bool ErdosWindow::loadPublication (QDomElement root, Publication &pub)
{
    QDomNodeList authorNodeList = root.elementsByTagName("Author");

     if(authorNodeList.count() ==0 )
         return false;

     for(int i = 0; i < authorNodeList.count(); i++)
     {
        QDomNode node = authorNodeList.at(i);

        if(node.isElement())
        {
            QDomElement element = node.toElement();
            auto name = element.attribute("Name").toStdString();

            pub.push_back(name);

        }
     }
     return true;
}

void ErdosWindow::on_backButton2_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void ErdosWindow::on_solveRandomButton_clicked()
{
   getRandomSolution();
}

void ErdosWindow::on_backButton3_clicked()
{
   ui->stackedWidget->setCurrentIndex(0);
}
