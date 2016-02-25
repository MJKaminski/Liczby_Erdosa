/**
 * \file   erdoswindow.h
 * \author Michał Kamiński
 * \date   20-01-2015
 * \brief  Klasa odpowiadajaca za okno programu
 */

#ifndef ERDOSWINDOW_H
#define ERDOSWINDOW_H

#include <QMainWindow>
#include <QProgressBar>
#include <QtXml>
#include <vector>
#include <string>


namespace Ui {
class ErdosWindow;
}

/**
 * \brief Klasa odpowiadajaca za okno programu.
 *
 * Pozwala na rozwiazanie problemu liczb Erdosa na podstawie danych z pliku XML
 * lub losowych danych. Klasa umozliwa rowniez analize zlozonosci algorytmow stosowanych przez program
 */
class ErdosWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ErdosWindow(QWidget *parent = 0);/**< Konstruktor okna */
    ~ErdosWindow();


private slots:
    void on_analyzeButton_clicked(); /**< Rozpoczyna analize wybranej czesci programu */

    void on_backButton_clicked(); /**< Wraca z ekranu analizy do ekranu startowego programu */

    void on_solveXMLButton_clicked();/**< Rozpoczyna proces rozwiazywania problemu liczb Erdosa dla danych z pliku XML */

    void on_backButton2_clicked();/**< Wraca z ekranu wynikow analizy XML do ekranu startowego programu */

    void on_solveRandomButton_clicked();/**< Rozpoczyna proces rozwiazywania problemu liczb Erdosa dla losowych danych */

    void on_backButton3_clicked();/**< Wraca z ekranu wynikow losowych danych do ekranu startowego programu */

private:

    /**
     * Typ wyliczeniowy okreslajacy algorytm grafowy
     */
    enum Algorithm
    {
        BFS,
        DIJKSTRA
    };

    /**
     * Typ wyliczeniowy sposob ladowania danych
     */
    enum Loading
    {
        PUBLICATIONSONLY,
        FULL
    };

    typedef std::vector<std::string> Authors;/**< Lista Autorow */
    typedef std::vector<std::string> Publication; /**< Reprezntacja publikacji jako lista autorow */
    typedef std::vector<std::vector<std::string>> Publications;/**< Lista Publikacji */
    typedef std::vector<std::pair<Authors,Publications>> Scenarios; /**< Lista Scenariuszy */

    Ui::ErdosWindow *ui;
    QProgressBar* bar; /**< Pasek ladowania */

    Scenarios scenarios; /**< Zbior scenariuszy do rozpatrzenia */

    /**
     * \brief Generuje losowe dane dla problemu liczb Erdosa
     * \param n Liczba autorow(wierzcholkow) do utworzenia
     * \param authors Wektor, do ktorego zostana zapisani autorzy
     * \param publications wWktor, do ktorego zostana zapisane publikacje
     */
    int generateInput(int n, std::vector<std::string> &authors, std::vector<std::vector<std::string>>& publications);

    /**
     * \brief Dokonuje analizy wybranego algorytmu ladowania danych
     * \param startCount Startowa liczba autorow
     * \param step O ile autorow wiecej maja miec kolejne analizowane instacje problemu
     * \param stepNo Liczba probek do przeanalizowania
     * \param loadMode Algorytm ladowania do przeanalizowania
     * \return Zwraca liczbe utworzonych krawedzi
     */
    void analyzeLoading (int startCount, int step, int stepNo, Loading loadMode);

    /**
     * \brief Dokonuje analizy wybranego algorytmu szukania liczb Erdosa
     * \param startCount Startowa liczba autorow
     * \param step O ile autorow wiecej maja miec kolejne analizowane instacje problemu
     * \param stepNo Liczba probek do przeanalizowania
     * \param algorithm Algorytm grafowy do przeanalizowania
     */
    void analyzeAlgorithm (int startCount, int step, int stepNo, Algorithm algorithm);

    /**
     * \brief Dokonuje pelnej analizy dzialania programu przy wybranym algorytmie szukania liczb Erdosa i sposobu
     * ladowania danych
     * \param startCount Startowa liczba autorow
     * \param step O ile autorow wiecej maja miec kolejne analizowane instacje problemu
     * \param stepNo Liczba probek do przeanalizowania
     * \param loadMode Algorytm ladowania do przeanalizowania
     * \param algorithm Algorytm grafowy do przeanalizowania
     */
    void fullAnalisis (int startCount, int step, int stepNo, Loading loadMode, Algorithm algorithm);

    /**
     * \brief Oblicza i wyswietla liczby Erdosa dla danych zaladowanych z pliku XML
     */
    void getXMLSolution();

    /**
     * \brief Oblicza i wyswietla liczby Erdosa dla danych wygenerowanych losowo
     */
    void getRandomSolution();

    /**
     * \brief Laduje scenariusze z pliku XML
     * \param root Korzen pliku XML zawierajacy scenariusze
     * \return Zwraca true, jesli operacja sie powiodla, a w przeciwnym wypadku false
     */
    bool loadScenarios(QDomElement root);

    /**
     * \brief Laduje scenariusz z pliku XML
     * \param root Korzen pliku XML zawierajacy pojedynczy scenariusz
     * \return Zwraca true, jesli operacja sie powiodla, a w przeciwnym wypadku false
     */
    bool loadScenario(QDomElement root);

    /**
     * \brief Laduje autorow danego scenariusza z pliku XML
     * \param root Korzen pliku XML zawierajacy autorow danego scenariusza
     * \param authorList Wektor do zapisu autorow
     * \return Zwraca true, jesli operacja sie powiodla, a w przeciwnym wypadku false
     */
    bool loadAuthors(QDomElement root, Authors &authorList);

    /**
     * \brief Laduje publikacje danego scenariusza z pliku XML
     * \param root Korzen pliku XML zawierajacy publikacje danego scenariusza
     * \param publicationsList Wektor do zapisu publikacji
     * \return Zwraca true, jesli operacja sie powiodla, a w przeciwnym wypadku false
     */
    bool loadPublications(QDomElement root, Publications &publicationsList);

    /**
     * \brief Laduje autorow danej publikacji z pliku XML
     * \param root Korzen pliku XML zawierajacy autorow danej publikacji
     * \param pub Wektor do zapisu autorow danej publikacji
     * \return Zwraca true, jesli operacja sie powiodla, a w przeciwnym wypadku false
     */
    bool loadPublication(QDomElement root, Publication& pub);
};

#endif // ERDOSWINDOW_H
