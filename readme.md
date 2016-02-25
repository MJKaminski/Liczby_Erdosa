# Analiza Algorytmów - Projekt "Liczby Erdosa"
## Autor - Michał Kamiński

### Opis Problemu
Liczba Erdosa jest miarą bliskości współpracy z tym słynnym matematykiem.  
Naukowiec, który opublikował pracę z Erdosem, otrzymuje liczbę 1. 
Osoba, która nie opublikowała niczego z Erdosem, ale posiada pracę z osobą o liczbie, otrzymuje rangę 2, itd. 
Celem zadania jest obliczenie liczb Erdosa dla zadanej grupy naukowców i prac naukowych.

### Tryby pracy aplikacji
- Znalezienie liczb Erdosa dla danych dostarczonych w pliku XML i z wykorzystaniem wybranych algorytmów. 
Wynikiem tego trybu jest wyświetlenie zakładek listami nazwisk i obliczonych liczb Erdosa dla każdego scenariusza.
	
- Znalezienie liczb Erdosa dla danych wygenerowanych losowo i z wykorzystaniem wybranych algorytmów. 
Wynikiem tego trybu jest wyświetlenie w zakładkach list wygenerowanych autorów oraz publikacji, 
a także listę węzłów i ich obliczonych liczb Erdosa
	
- Analiza złożoności i pomiar czasu wykonania wybranych algorytmów. 
Po przeprowadzonej analizie aplikacja wyświetla tabelę zawierającą przeprowadzone pomiary oraz analiza współczynnika q.
	
### Konwencja Danych Wejściowych
Dane wejściowe powinny zostać zapisane w pliku XML o podanej strukturze:
```xml
<?xml version="1.0"?>
<Scenarios>
	<Scenario ID = "0">
		<Authors>
			<Author ID = "0" Name = "Erdos"/>
			.......
		</Authors>
		
		<Publications>
			<Publication>
				<Author ID = "0" Name = "Erdos"/>
				<Author ID = "1" Name = "Arent"/>
				<Author ID = "2" Name = "Boguslawowicz"/>
			</Publication>
			
			<Publication>
				<Author ID = "1" Name = "Arent"/>
				<Author ID = "3" Name = "Cichocka"/>
			</Publication>
			...														
		</Publications>
	</Scenario>
	<Scenario ID = "1">
		....
	</Scenario>	
	...
</Scenarios>
```

Wymaga się, aby każdy scenariusz:
- posiadał niepustą listę naukowców oraz publikacji
- posiadał naukowaca o nazwisku Erdos
- posiadał listę publikacji, która zapewnia spójność grafu

### Algorytmy tworzenia grafu 
W projekcie zaimplementowano dwa algorytmy tworzenia grafu:

#### a. Na podstawie list autorów i publikacji
Węzły grafu tworzone są w oparciu o listę autorów, krawędzie zaś tworzone są w oparciu o publikacje. Asymptotyczną złożoność oszacowano na T(n,v) = v*log(n), gdzie v – liczba wierzchołków, v-liczba krawędzi.

		Schemat algorytmu:
		i.	Dla każdej osoby na liście naukowców węzeł będący jego reprezentacją i dodaj go do zbioru węzłów
		ii.	Dla każdej publikacji z listy:
			1.	Wyszukaj w zbiorze węzłów wszystkich autorów wspomnianych w publikacji
			2.	Każdemu węzłowi związanym z daną publikacją dodaj do listy incydencji
			pozostałych autorów artykułów (jeśli dany węzeł istnieje na liście incydencji, nie należy wstawiać go po raz kolejny)
####b. Na podstawie listy publikacji
W tym wariancie lista publikacji używana jest zarówno do tworzenia węzłów, jak i krawędzi grafu. Asymptotyczną złożoność oszacowano podobnie jak w punkcie a.

		Schemat algorytmu:
		i.	Dla każdej publikacji na liście:
			1.	Utwórz węzły reprezentujące autorów publikacji i dodaj je do zbioru 
			(za wyjątkiem sytuacji, gdy dany autor istnieje już w grafie)
			2.	Każdemu węzłowi związanym z daną publikacją dodaj do listy incydencji pozostałych 
			autorów artykułów (jeśli dany węzeł istnieje na liście incydencji, nie należy wstawiać go po raz kolejny)


### Algorytmy znajdowania liczb Erdosa
W projekcie zaimplementowano dwa algorytmy grafowe znajdujące liczby Erdosa:

#### a.	BFS (Breadth-First Search)
Jest to jeden z najprostszych algorytmów przeszukiwania grafu. Kolejność przeglądania węzłów jest następująca: najpierw węzeł startowy, potem warstwa węzłów połączonych z węzłem startowego, następnie warstwa węzłów połączonych z warstwą pierwszą, itd. Z tego powodu uruchomienie tego algorytmu dla węzła „Erdos” może pozwolić na nadanie węzłom odpowiednich liczb. Asymptotyczną złożoność oszacowano na T(n,v) = v+n

	Schemat algorytmu:
		i.	Znajdź węzeł „Erdos” i nadaj mu rangę 0
		ii.	Dodaj węzeł „Erdos” do kolejny FIFO
		iii.	Dopóki kolejka nie jest pusta:
			1.	Wyjmij z kolejki pierwszy element
			2.	Dla każdego sąsiada/współpracownika danego węzła: jeśli nie przypisano mu jeszcze liczby Erdosa, 
			to przypisz mu liczbę o 1 większą od liczby wyjętego elementu i dodaj go do kolejki
			
#### b.	Algorytm Dijkstry
Domyślnie stosowany jest on do grafów o nieujemnych wagach krawędzi do znajdowania najmniejszych odległości od źródła do pozostałych wierzchołków. W naszym przypadku wszystkie krawędzie mają wagę 1 przez co algorytm ten traci przewagę nad BFS, który nie potrafi znaleźć najkrótszych ścieżek dla dowolnych dodatnich wag krawędzi. Złożoność algorytmu oszacowano na T(n,v) = v*log(n), czyli gorszą od algorymu BFS. 

	Schemat algorytmu:
		i.	Przypisz każdemu węzłowi, który nie jest źródłem, nieskończoną odległość do źródła. Węzłowi Erdos przypisz odległość 0
		ii.	Dodaj węzeł Erdos do zbioru węzłów do rozpatrzenia
		iii.	Dopóki zbiór węzłów do rozpatrzenia nie jest pusty
			1.	Wyjmij ze zbioru węzeł (w)
			2.	Jeśli któryś z jego „sąsiadów” ma przypisaną odległość większą niż odległość wyjętego węzła powiększona o 1 (dist(w) +1):
				a.	Przypisz mu odległość dist(w) + 1
				b.	Dodaj go do zbioru węzłów do rozpatrzenia
				
### Dekompozycja programu
Program składa się z dwóch modułów:
- Model danych (graf), na który składają się pliki authorsgraph.h oraz authorsgraph.cpp
- GUI, na który składają się pliki erdoswindow.h oraz erdoswindow.cpp

Dokładny opis wszystkich plików, klas, metod i pól znajduje się w pliku Dokumentacja Techniczna/html/index.html

### Uwagi dotycząca stosowania
- Nie zaleca się analizy grafów o ilości wierzchołków większej niż 5000

### Kompilacja
Aby skompilować projekt wymagane są:
- kompilator c++ 11
- biblioteka Qt wraz z narzędziami MOC i UIC (całość może zostać zainstalowana wraz IDE QtCreator)
