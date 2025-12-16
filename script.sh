#!/bin/bash

export LC_NUMERIC=C

PROGRAMME_C="./c-wildwater"
debut=$(date +%s%3N)

erreur() {
    echo "[Erreur] $1"
    duree_totale
    exit 1
}

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

verifier_fichier() {
    if [ ! -f "$CSV" ]; then
        erreur "Le fichier CSV '$CSV' est introuvable."
    fi
}

verifier_compilation() {
    if [ ! -f "$PROGRAMME_C" ]; then
        echo "Compilation du programme C..."
        make || erreur "Échec de la compilation."
    fi
}

# Nettoyage automatique des fichiers temporaires à la fin
trap 'rm -f plot_temp.gp *.header *.sorted *.small *.big' EXIT

generer_png() {
    fichier=$1
    type=$2
    
    [ ! -f "$fichier" ] && erreur "Le fichier de données '$fichier' n'existe pas."
    
    base="${fichier%.*}"
    label_y="Volume" 

    case $type in
        max)  titre="Capacité maximale"; label_y="Capacité" ;;
        src)  titre="Volume capté" ;;
        real) titre="Volume traité" ;; 
        *) titre="Analyse Usines" ;;
    esac

    echo "Génération de l'image combinée (Top 10 & Top 50)..."

    head_f="$fichier.header"
    tri_f="$fichier.sorted"
    small_f="$fichier.small"
    big_f="$fichier.big"
    gp_f="plot_temp.gp"

    # Préparation des données
    head -n 1 "$fichier" > "$head_f"
    tail -n +2 "$fichier" | sort -t";" -k2,2g > "$tri_f"

    if [ ! -s "$tri_f" ]; then
        erreur "Le fichier de données trié est vide." 
    fi

    # Extraction des extrêmes
    head -n 50 "$tri_f" > "$small_f"
    tail -n 10 "$tri_f" > "$big_f"

    # Création du script Gnuplot multiplot
    cat > "$gp_f" << EOF
set terminal png size 1000,1400 font "Arial,10"
set output '${base}_combined.png'
set datafile separator ';'
set style fill solid 0.7 border -1
set ylabel '$label_y (M.m3/an)'
set grid y

set multiplot layout 2,1 title "Projet C-WildWater : ${titre}\n" font "Arial,16"

# GRAPHE 1 : Les 10 plus grandes usines
set title "Top 10 : Plus grandes usines" font "Arial,12"
set xtics rotate by -45
plot '$big_f' using 2:xtic(1) with boxes lc rgb "#1f77b4" title ""

# GRAPHE 2 : Les 50 plus petites usines
set title "Top 50 : Plus petites usines" font "Arial,12"
set xtics rotate by -90 font "Arial,7"
plot '$small_f' using 2:xtic(1) with boxes lc rgb "#2ca02c" title ""

unset multiplot
EOF

    gnuplot "$gp_f" || erreur "Erreur lors de l'exécution de Gnuplot."
    echo "✅ Image générée : ${base}_combined.png"
}

traitement_histo() {
    type_h=$1
    echo "Traitement histogramme ($type_h)..."
    # Capture du nom du fichier via la sortie du programme C
    sortie=$($PROGRAMME_C "$CSV" histo "$type_h" 2>&1)
    [ $? -ne 0 ] && erreur "Erreur du programme C."
    
    fichier_dat=$(echo "$sortie" | grep "FICHIER_GENERE:" | cut -d: -f2 | tr -d '\r ')
    
    if [ -f "$fichier_dat" ]; then
        generer_png "$fichier_dat" "$type_h"
    else
        erreur "Fichier de données non trouvé."
    fi
}

traitement_leaks() {
    id_l="$1"
    echo "Calcul des fuites pour $id_l..."
    sortie=$($PROGRAMME_C "$CSV" leaks "$id_l" 2>&1)
    [ $? -ne 0 ] && erreur "Erreur du programme C."
    
    fichier_l=$(echo "$sortie" | grep "FICHIER_GENERE:" | cut -d: -f2 | tr -d '\r ')
    if [ -f "$fichier_l" ]; then
        cat "$fichier_l"
    else
        erreur "Fichier de fuites non trouvé."
    fi
}

# --- Main ---
[ $# -lt 2 ] && usage
CSV="$1"
shift
verifier_fichier
verifier_compilation

case $1 in
    histo) [ $# -ne 2 ] && usage; traitement_histo "$2" ;;
    leaks) [ $# -ne 2 ] && usage; traitement_leaks "$2" ;;
    *) usage ;;
esac

duree_totale
``` [cite : 341]
