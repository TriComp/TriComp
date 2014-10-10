(* 
--- Réflexions pré-implémentation ---

* C'est quoi un graphe ? 
C'est : 
   - un ensemble de sommets
   - un ensemble d'arêtes (= couples de sommets)

Ici, il s'agit de graphes orientés. Donc bien couples ordonnés. 

* Définition avec des listes ou des tableaux ?

* Qu'a-t-on besoin de faire sur le graphe ?

Le parcourir pour en extraire le programme bas niveau.
Il faudrait avoir une idée de l'algorithme... À priori on regarde les voisins du sommet, et on en extrait ce qu'on veut.
Du coup, liste d'adjacence ? (en plus ça passe mieux en fonctionnel)

* On représente les sommets par des entiers : pas de contre-indication ?

*)


type 'a graphe = { Sommet :  };;
