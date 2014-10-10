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
   Il faudrait avoir une idée de l'algorithme... À priori on regarde les voisins du 
   sommet, et on en extrait ce qu'on veut.
   Du coup, liste d'adjacence ? (en plus ça passe mieux en fonctionnel)

   * On représente les sommets par des entiers : pas de contre-indication ?

*)

(* suggestion dans un coin : faire un warning si quelqu'un essaie de faire des 
   trucs difficiles physiquement *)

type type_maille = Endroit | Envers | Torse;;

type maille = { id : int*int ; (* numéro de rang * numéro de maille *)
		m_type : type_maille ; 
		extremites : maille*maille ; 
		(* mailles auxquelles la boucle de fil est reliée : 
		   il peut y en avoir plus de 2 ? *)
		pred : maille ; succ : maille ; (* quand on fait les croisements *)
		EOR : bool};; (* la maille est-elle à la fin du rang ? *)

type tricot = maille list;;

let monter_mailles n = 
  

let jersey m n = (* Crée un carré de jersey de taille m×n*)
  
