/**
 * \file   authorsgraph.h
 * \author Michał Kamiński
 * \date   20-01-2015
 * \brief  Klasa modelowa programu. Reprezentuje problem jako graf, w ktorym wezlami sa autorzy, a krawedz swiadczy
 * o napisaniu razem artykulu przez dana pare naukowcow
 */


#ifndef AUTHORSGRAPH_H
#define AUTHORSGRAPH_H

#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <vector>
#include <set>

using namespace std;

/**
 * \brief Klasa modelowa programu.
 *
 * Reprezentuje problem jako graf, w ktorym wezlami sa autorzy, a krawedz swiadczy
 * o napisaniu razem artykulu przez dana pare naukowcow. Udostepnia interfejs pozwalajacy wybrac sposob zaladowania danych
 * oraz algorytm uzyty do znalezienia rozwiazania
 */
class AuthorsGraph
{

    /**
     * \brief Systemowa repezentacja Autora - wezla grafu.
     *
     * Przechowuje infomacje o autorze oraz zbior wskazan na
     * osoby, z ktorymi wspoltworzyl
     */
    class Author
    {
    private:
        int _number; /**< Numer Autora */
        int _rank; /**< Liczba Erdosa Autora */
        string _name; /**< Nazwisko Autora */
        bool _isRanked; /**< Czy autorowi przydzielo liczbe Erdosa */
        set<Author*> coWritersList; /**< Zbior wspolpracownikow danego Autora */

    public:
        friend class AuthorsGraph;

        /** Konstruktor klasy autor */
        Author(int n, string nam) :_number(n), _name(nam), _isRanked(false)
        {}

        /**
         * \brief Dodaje wspolpracownika
         * \param a Wspopracownik
         */
        void addEdge(Author* a)
        {
            coWritersList.insert(a);
        }

        /**
         * \brief Ustawia liczbe Erdosa
         * \param n Liczba Erdosa
         */
        void setRank(int n)
        {
            _rank = n;
            _isRanked = true;
        }

        /**
         * \brief Zwraca liczbe Erdosa
         * \return Zwraca liczbe Erdosa
         */
        int getRank() const
        {
            return _rank;
        }

        /**
         * \brief Zwraca nazwisko autora
         * \return Zwraca nazwisko autora
         */
        string getName() const
        {
            return _name;
        }

        /**
         * \brief Informuje czy autorowi przydzielono juz liczbe Erdosa.
         * Stosowane przez algorytm BFS
         * \return Informuje czy autorowi przydzielono juz liczbe Erdosa
         */
        bool isRanked() const
        {
            return _isRanked;
        }

        /**
         * \brief Prezentuje autora na wyjsciu standardowym
         */
        void presentAuthor()
        {
            cout<<"Autor: "<<_name<<endl;
            cout<<"Wspoltworzyl z: "<<endl;

            for(auto i: coWritersList)
                //cout<<"- "<< i.lock()->_name <<endl;
                cout<<"- "<< i->_name <<endl;

            cout<<endl;
        }

    };
public:
    /** Kontruktor klasy AuthorsGraph */
    AuthorsGraph() : nodeCount(0)
    {}

    /** Dekontruktor klasy AuthorsGraph */
    ~AuthorsGraph()
    {}


private:
    int nodeCount; /**< Liczba autorow */
    map<string,shared_ptr<Author>> authorsMap; /**< Mapa par (nazwisko autora)- (wskaznik na wezel) */

public:

    /**
     * \brief Laduje dane do grafu na podstawie list autorow oraz publikacji
     * \param authors Lista autorow
     * \param publications Lista publikacji
     */
    void loadGraph(std::vector<string>& authors, std::vector<std::vector<string> > &publications);

    /**
     * \brief Laduje dane do grafu na podstawie listy publikacji
     * \param publications Lista publikacji
     */
    void loadGraph(std::vector<std::vector<string>>& publications);

    /**
     * \brief Oblicza liczby Erdosa uzywajac algorytmu Dijkstry
     * \return Zwraca wskaznik na liste par nazwisko-liczba Erdosa
     */
    vector<pair<string, int> > * getRanksBFS();

    /**
     * \brief Oblicza liczby Erdosa uzywajac algorytmu BFS
     * \return Zwraca wskaznik na liste par nazwisko-liczba Erdosa
     */
    vector<pair<string, int> > * getRanksDijkstry();

    /**
     * \brief Prezentuje graf na wyjsciu stardardowym
     */
    void presentGraph();

    /**
     * \brief Usuwa dane przechowywane przez graf
     */
    void clear();


};



#endif // AUTHORSGRAPH_H
