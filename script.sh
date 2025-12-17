#!/bin/bash

# --- Configuration et Initialisation ---
PROGRAMME_C="./c-wildwater"
debut=$(date +%s%3N)

# Export pour s'assurer que le point est utilisé comme séparateur décimal
export LC_NUMERIC=C

# --- Fonctions Utilitaires ---

# Affiche une erreur et la durée totale avant de quitter [cite: 192, 200]
erreur() {
    echo "[Erreur] $1" >&2
    duree_totale
    exit 1
}

# Calcule et affiche la durée totale en millisecondes [cite: 200]
duree_totale() {
    fin=$(date +%s%3N)
    echo "Durée totale : $((fin - debut)) ms"
}

usage() {
    echo "Usage : $0 <csv> <commande> [argument]"
    echo "Commandes :"
    echo "  histo max   : capacité maximale"
    echo "  histo src   : volume capté"
    echo "  histo real  : volume traité"
    echo "  leaks <ID>  : calcul des fuites"
    duree_totale
    exit 1
}

# Vérifie la présence du Makefile et compile si l'exécutable est absent [cite: 197, 198, 222, 223]
verifier_compilation() {
    if [ ! -f "$PROGRAMME_C" ]; then
        if [ -f "Makefile" ]; then
            echo "Compilation du programme C via Makefile..."
            make || erreur "Échec de la compilation."
        else
            erreur "Makefile introuvable pour compiler le programme C."
        fi
    fi
}

# Extrait le nom du fichier généré depuis la sortie standard du programme C
extraire_fichier_genere() {
    grep "FICHIER_GENERE:" programme_sortie.log | cut -d':' -f2 | tr -d '\r '
}

# --- Génération de l'Image ---

generer_png() {
    local fichier_dat=$1
    local type=$2
    
    [ ! -f "$fichier_dat" ] && erreur "Le fichier de données '$fichier_dat' n'existe pas."
    local base="${fichier_dat%.*}"

    # Titres dynamiques selon le type d'histogramme
    case $type in
        max)  titre="Capacité maximale"; label_y="Capacité (M.m3/an)" ;;
        src)  titre="Volume capté"; label_y="Volume (M.m3/an)" ;;
        real) titre="Volume traité"; label_y="Volume (M.m3/an)" ;;
        *)    titre="Analyse Usines"; label_y="Volume (M.m3/an)" ;;
    esac

    # Optimisation : Prétraitement des données par le Shell (plus rapide que Gnuplot) [cite: 162]
    # Tri numérique par volume (colonne 2) et extraction des extrêmes
    tail -n +2 "$fichier_dat" | sort -t";" -k2,2g > "$fichier_dat.sorted"
    head -n 50 "$fichier_dat.sorted" > "$fichier_dat.small"
    tail -n 10 "$fichier_dat.sorted" > "$fichier_dat.big"

    # Génération via Gnuplot (Multiplot Top 10 + Top 50) [cite: 162, 286]
    if command -v gnuplot >/dev/null 2>&1; then
        gnuplot << EOF
set terminal png size 1000,1200 font "Arial,10"
set output '${base}_combined.png'
set datafile separator ';'
set style fill solid 0.7 border -1
set ylabel '$label_y'
set grid y
set multiplot layout 2,1 title "Projet C-WildWater : ${titre}\n"

# GRAPHE 1 : Top 10 (Plus grandes usines)
set title "Les 10 plus grandes usines"
set xtics rotate by -45
plot '$fichier_dat.big' using 2:xtic(1) with boxes lc rgb "#1f77b4" notitle

# GRAPHE 2 : Top 50 (Plus petites usines)
set title "Les 50 plus petites usines"
set xtics rotate by -90 font "Arial,8"
plot '$fichier_dat.small' using 2:xtic(1) with boxes lc rgb "#2ca02c" notitle

unset multiplot
EOF
        echo "Image générée : ${base}_combined.png"
    else
        echo "Avertissement : Gnuplot non trouvé. Image non générée."
    fi

    # Nettoyage des fichiers temporaires
    rm -f "$fichier_dat.sorted" "$fichier_dat.small" "$fichier_dat.big"
}

# --- Traitements Principaux ---

traitement_histo() {
    local arg_val=$1
    $PROGRAMME_C "$CSV" histo "$arg_val" > programme_sortie.log 2>&1
    local ret=$?
    
    [ $ret -ne 0 ] && erreur "Le programme C a échoué (code $ret)."
    
    local fichier=$(extraire_fichier_genere)
    if [ -f "$fichier" ]; then
        generer_png "$fichier" "$arg_val"
    else
        erreur "Le fichier de données .dat n'a pas été créé par le programme C."
    fi
}

traitement_leaks() {
    local id_usine=$1
    $PROGRAMME_C "$CSV" leaks "$id_usine" > programme_sortie.log 2>&1
    local fichier=$(extraire_fichier_genere)
    
    if [ -f "$fichier" ]; then
        echo "Résultat de l'analyse des fuites pour $id_usine :"
        cat "$fichier" # Affiche le contenu du fichier .dat généré [cite: 170, 186]
    else
        erreur "Le calcul des fuites a échoué."
    fi
}

# --- Point d'Entrée ---

# Vérification du nombre d'arguments [cite: 192, 193]
[ $# -lt 2 ] && usage

CSV="$1"
COMMANDE="$2"
ARGUMENT="$3"

# Vérifications de base [cite: 10, 197]
[ ! -f "$CSV" ] && erreur "Le fichier CSV '$CSV' est introuvable."
verifier_compilation

case $COMMANDE in
    histo)
        [ -z "$ARGUMENT" ] && usage
        traitement_histo "$ARGUMENT"
        ;;
    leaks)
        [ -z "$ARGUMENT" ] && usage
        traitement_leaks "$ARGUMENT"
        ;;
    *)
        erreur "Commande inconnue : $COMMANDE"
        ;;
esac

duree_totale
exit 0
