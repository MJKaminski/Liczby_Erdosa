/**
 * \file   authorsgraph.cpp
 * \author Michał Kamiński
 * \date   20-01-2015
 * \brief  Klasa modelowa programu. Zeprezentuje problem jako graf, w ktorym wezlami sa autorzy, a krawedz swiadczy
 * o napisaniu razem artykulu przez dana pare naukowcow
 */


#include "authorsgraph.h"
#include <queue>
#include <limits>


using namespace std;



void AuthorsGraph::loadGraph(vector<string> &authors, vector<vector<string>>& publications)
{

    vector<Author*> coAuthors; //Wektor wspolautorow
    shared_ptr<Author> authorPtr;

    if(!authorsMap.empty())
        authorsMap.clear();

    for(auto i :authors) //Ladowanie wezlow na podstawie listy autorow
    {
        authorsMap.emplace(i, shared_ptr<Author> (new Author(nodeCount,i)));
        ++nodeCount;
    }

    for (auto i: publications) // ladowanie krawedzi grafow na podstawie publikacji
    {
        auto iter = i.begin();
        auto end = i.end();


        for(;iter!=end;++iter) // wyszukiwanie autorow wymienionych w publikacji ...
        {
           authorPtr = authorsMap.find(*iter)->second;
           coAuthors.push_back(authorPtr.get());
        }

        for(auto j = coAuthors.begin(); j != coAuthors.end(); ++j) // ... i dodawanie odpowiednich krawedzi
            for(auto k = j+1; k != coAuthors.end(); ++k)
            {
                (*j)->addEdge(*k);
                (*k)->addEdge(*j);
            }

        coAuthors.clear();
    }

}

void AuthorsGraph::loadGraph(vector<vector<string> > &publications)
{
    vector<Author*> coAuthors; // wektor wspolpracownikow

    if(!authorsMap.empty())
        authorsMap.clear();

    for (auto i: publications)
    {
        auto iter = i.begin();
        auto end = i.end();


        for(;iter!=end;++iter) //ladowanie wezlow bez powtorzen w oparciu o dana publikacje
        {
           string name = *iter;
           auto pair = authorsMap.emplace(name, shared_ptr<Author> (new Author(nodeCount,name)));
           if(pair.second)
           {
               ++nodeCount;
           }

           coAuthors.push_back((*(pair.first)).second.get());
        }

        for(auto j = coAuthors.begin(); j != coAuthors.end(); ++j) // dodawanie krawedzi
            for(auto k = j+1; k != coAuthors.end(); ++k)
            {
                (*j)->addEdge(*k);
                (*k)->addEdge(*j);
            }

        coAuthors.clear();
    }
}

vector<pair<string, int> > *AuthorsGraph::getRanksBFS()
{

    vector<pair<string,int> > *result = new vector<pair<string,int> > ();
    Author * a;
    shared_ptr<Author> erdos;
    queue<Author*> Queue;
    int currentRank;

    erdos = authorsMap.find("Erdos")->second; //Wyszukiwanie wezla startowego algorytmu - Erdosa
    erdos->setRank(0);

    for(auto i: erdos->coWritersList) //Dodanie wspolpracownikow Erdosa do kolejki i nadanie im rangi 1
    {
            i->setRank(1);
            Queue.push(i);
    }

    while(! Queue.empty()) //Dodawaj kolejnych wspolpracownikow i nadawaj im rangi o 1 wieksze
    {
        a = Queue.front();
        Queue.pop();
        currentRank = a->getRank();

        for(auto i: a->coWritersList)
        {
            if(!i->isRanked())
            {
                i->setRank(currentRank + 1);
                Queue.push(i);
            }
        }
    }

    for(auto it: authorsMap) // generowanie par nazwisko-ranga
    {
        string name = it.second->getName();
        int rank = it.second->getRank();
        result->push_back(make_pair(name,rank));
    }

    return result;
}

vector<pair<string,int> > *AuthorsGraph::getRanksDijkstry()
{
    vector<pair<string,int> > *result = new vector<pair<string,int> > ();
    map<string,int> minDistance; // zbior rang/odlegosci wezlow od erdosa
    set< pair<int,string> > activeVer; //zbior wezlow do rozpatrzenia

    for(auto it: authorsMap) //przydziel wezlom INT_MAX jako odlegosc do Erdosa, Erdosowi przydziel 0
    {
        if(it.first =="Erdos")
            minDistance.emplace("Erdos",0);
        else
            minDistance.emplace(it.first, numeric_limits<int>::max());
    }


    activeVer.insert({0,"Erdos"}); //Dodaj Erdosa z oglegloscia 0

    while (!activeVer.empty())
    {

        auto begin = activeVer.begin(); // wyjmij pierwszy wezel ze zbioru
        string author = begin->second;
        activeVer.erase( begin );

        shared_ptr<Author> node = authorsMap[author];

        for(auto coWorker :node->coWritersList) // dla kazdego wspolpracownika pobranego autora...
        {
            string coWorkName = coWorker->getName();
            int authorDist = minDistance[author];
            int coWorkDist = minDistance[coWorkName];

            if( coWorkDist > authorDist + 1) // ...jesli jego teoretyczna odleglosc do Erdosa jest wieksza od odleglosci pobranego autora+1..
            {
                activeVer.erase( {coWorkDist,coWorkName});
                int dis = minDistance[coWorkName] = authorDist + 1; //...to ja zaktualizuj...
                activeVer.insert( {dis, coWorkName} ); //..i dodaj do zbioru wezlow do rozpatrzenia
            }
        }
    }

    for(auto it: minDistance )// generowanie par nazwisko-ranga
    {
        result->emplace_back(it.first, it.second);
    }

    return result;
}

void AuthorsGraph::presentGraph()
{
    cout<<"Liczba wezlow "<<nodeCount<<endl;
    for (auto i: authorsMap)
        i.second->presentAuthor();
}

void AuthorsGraph::clear()
{
    authorsMap.clear();
}
