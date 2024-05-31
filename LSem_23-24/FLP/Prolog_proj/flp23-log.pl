/**
* Kostra grafu
* xduric05
* Peter Ďurica
*/

% Deklaracia dynamickeho predikatu
% path(A, B) -> existuje cesta z A do B
:- dynamic(path/2).

% Zobrane z input2.pl - Cita vstup a rozdeluje ho do samostatnych listov
% (read_line, isEOFEOL, read_lines, split_line, split_lines)
% -------------------------------------------------------------------------------------------------
read_line(L,C) :-
	get_char(C),
	(isEOFEOL(C), L = [], !;
		read_line(LL,_),
		[C|LL] = L).

isEOFEOL(C) :-
	C == end_of_file;
	(char_code(C,Code), Code==10).

read_lines(Ls) :-
	read_line(L,C),
	( C == end_of_file, Ls = [] ;
	  read_lines(LLs), Ls = [L|LLs]
	).

split_line([],[[]]) :- !.
split_line([' '|T], [[]|S1]) :- !, split_line(T,S1).
split_line([32|T], [[]|S1]) :- !, split_line(T,S1). 
split_line([H|T], [[H|G]|S1]) :- split_line(T,[G|S1]).


split_lines([],[]).
split_lines([L|Ls],[H|T]) :- split_lines(Ls,T), split_line(L,H).
% -------------------------------------------------------------------------------------------------
/**
* create_paths(+Input : list)
* Dostava vystup zo split lines, ktory nasledne rozbali a prida nove pravidla do dynamickeho predikatu
* Vytvori pravidla pre cestu z A do B aj pre cestu z B do A (Graf je obojsmerny)
*/
create_paths([]).
create_paths([[[A], [B]]|T]) :- assert(path(A, B)), assert(path(B, A)), create_paths(T).

% -------------------------------------------------------------------------------------------------
/**
* add_values(+Line : list, +V1 : list, -VOut : list)
* Dostane riadok zo vstupneho suboru, skontroluje ci sa vrcholy uz nachadzaju vo V1 a ak nie, tak
* ich vlozi do vystupneho listu.
* Pomocny predikat pre vytvorenie zoznamu unikatnych vrcholov
*/
add_values([[A],[B]], [], [A, B]). % Pre prazdne V1 vloz obe hodnoty do VOut
add_values([[A], [B]], V1, V) :-
                ( member(A, V1), member(B, V1) -> V = V1
                ; member(A, V1) -> V = [B | V1]
                ; member(B, V1) -> V = [A | V1]
                ; V = [A,B | V1]
                ).

/**
* create_vertices_list(+Input : list, ?Ver : list)
* Zanori sa na posledny riadok vstupu a nasledne postupne vlozi unikatne vrcholy do Ver pomocou add_values
*/
create_vertices_list([],_).
create_vertices_list([H|T], V) :- create_vertices_list(T, V1), add_values(H, V1, V).
% -------------------------------------------------------------------------------------------------

% -------------------------------------------------------------------------------------------------
/**
* connected (+A : atom, +B : atom, ?L : list)
* Hlada prepojenie medzi dvoma lubovolnymi vrcholmi
* L sluzi ako zoznam uz navstivenych vrcholov
*/
connected(A, B, _) :- path(A, B). % Ak su vrcholy rovno medzi sebou prepojene
connected(A, B, L) :- path(A, C), C \= A, \+ member(C, L), connected(C, B, [A | L]). % Ak je medzi vrcholmi viacer ciest

/**
* check_full_connectivity(+A : atom, +Ver : list)
* Zkontroluje ci existuje prepojenie z jedneho vrcholu do kazdeho ineho vrcholu
*/
check_full_connectivity(_, []).
check_full_connectivity(A, [B|T]) :- connected(A,B, []), check_full_connectivity(A, T).

/**
* vertices_connected(+Ver : list)
* Len rozdeli zoznam vrcholov na prvy vrchol a na zvysok
*/
vertices_connected([]) :- false. % Ak je Ver prazdny tak false
vertices_connected([H|T]) :- check_full_connectivity(H, T).

/**
* full_connectivity(+Ver : list)
* Ak niesu vrcholy spojene tak zastavi program inak pokracuje
*/
full_connectivity(Ver) :-
                ( 
                  \+ vertices_connected(Ver) -> halt
                  ; true
                ).
% -------------------------------------------------------------------------------------------------

% -------------------------------------------------------------------------------------------------
/**
* get_paths(?Control : list, -Paths : list)
* Ak existuje medzi vrcholmi cesta a tato cesta nieje este v kontrolnom zozname, vlozi ju do zoznamu
* Po najdeni vsetkych ciest sa prepise Control do Paths
*/
get_paths(Control, Paths) :-
                (path(A, B), \+ member([A, B], Control), \+ member([B, A], Control) ->
                  get_paths([[A, B] | Control], Paths)
                ; Paths = Control
                ).
% -------------------------------------------------------------------------------------------------

% -------------------------------------------------------------------------------------------------
/**
* gen_per(+L : list, ?Control: list, -Permutation : list, +Size : atom)
* Vytvori permutaciu bez opakovania zo zoznamu L o dlzke Size
* Vklada hodnoty z listu L do listu Control, po vlozeni vymaze tuto hodnotu z listu L
* Po dosiahnuti dostatocnej velkosti prepise Control list do Permutation 
*/
gen_per(L, Control, Permutation, Size) :- 
                (\+ length(Control, Size) -> member(A, L), delete(L, A, L1), gen_per(L1, [A | Control], Permutation, Size)
                ; Permutation = Control
                ).

/**
* tree_not_used(+STrees : list, +NewTree : list)
* skontroluje ci nova kostra neobsahuje rovnake cesty ako predchadajuce cesty
*/
tree_not_used([], _).
tree_not_used([H|T], NewTree) :- \+ subset(NewTree, H), tree_not_used(T, NewTree).

/**
* all_ver_used(+Stree : list, +V : list)
* skontroluje ci v kostre su pouzite vsetky vrcholy
*/
all_ver_used(_, []). % ak sa odstrania vsetky vrcholy predtym ako sa prejde cez cesty v kostre
all_ver_used([], _) :- false. % ak zostanu nejake vrcholy a preslo sa cez vsetky cesty v kostre
all_ver_used([[A, B] | T], V) :- % ak zostavaju vrcholy aj cesty na kontrolu
                % Skontroluje ci sa vrcholy v ceste nachadzaju v zozname vrcholov a pripadne ich odstrani
                % pre dalsie volania
                ( member(A, V), member(B, V) -> delete(V, A, V1), delete(V1, B, V2), all_ver_used(T, V2)
                ; member(A, V) -> delete(V, A, V1), all_ver_used(T, V1)
                ; member(B, V) -> delete(V, B, V1), all_ver_used(T, V1)
                ; all_ver_used(T, V)
                ).
/**
* find_spanning_trees(+Paths : list, ?Control : list, -STrees : list, +Size: atom, +Ver : list)
* Stromy sa hladaju pomocou permutacii ciest o dlzke len(Ver) - 1,
* Po vygenerovani kostry sa skontroluje ci uz nieje pouzita podobna predtym, potom ci spaja vsetky vrcholy
* Ak splna vsetky poziadavky vlozi sa do Control zoznamu
* Po vygenerovani vsetkych kostier sa nasledne ulozia do STrees
*/
find_spanning_trees(Paths, Control, STrees, Size, Ver) :-
                (   gen_per(Paths, [], Permutation, Size), tree_not_used(Control, Permutation), all_ver_used(Permutation, Ver) -> find_spanning_trees(Paths, [Permutation | Control], STrees, Size, Ver)
                ;   STrees = Control
                ).
% -------------------------------------------------------------------------------------------------

% -------------------------------------------------------------------------------------------------

/**
* print_path(+Path : list)
* Vytlaci vrcholy cesty na stdout
*/
print_path([A,B]) :- write(A), write('-'), write(B).

/**
* print_tree(+STree : list)
* Vytlaci cesty kostry na riadok stdout
*/
print_tree([Path]) :- print_path(Path). % pri poslednej ceste na riadku sa nevytlaci medzera
print_tree([Path | Rest]) :- print_path(Path), write(' '), print_tree(Rest).

/**
* print_output(+STrees : list)
* Vytlaci vsetky kostry na samostatne riadky stdout
*/
print_output([]).
print_output([Tree | Rest]) :- print_tree(Tree), nl, print_output(Rest).
% -------------------------------------------------------------------------------------------------

/**
* start - vstupny predikat pre program
*/
start :-
        prompt(_, ''), % nastavi prompt na prazdny string
        read_lines(LL), % precita vstupny subor
        split_lines(LL, S), % rozdeli vstupny subor
        create_paths(S), % nastavi dynamicky predikat path/2
        create_vertices_list(S, Ver), % vytvori list vrcholov v grafe
        full_connectivity(Ver), % skontroluje ci vrcholy su medzi sebou spojene
        get_paths([], Paths), % ziska list vsetkych ciest
        length(Ver, VerLen), % vypocita dlzku zoznamu vrcholov
        Size is VerLen - 1, % vypocita dlzku permutacii
        find_spanning_trees(Paths, [], STrees, Size, Ver), % najde vsetky kostry grafu
        print_output(STrees), % vytlaci kostry grafu na stdout
        halt.
