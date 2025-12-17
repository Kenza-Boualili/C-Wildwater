#!/bin/bash

# Configuration
PROGRAMME_C="./c-wildwater"
debut=$(date +%s%3N)

# Export pour s'assurer que le point est utilisé comme séparateur décimal
export LC_NUMERIC=C

erreur() {
    echo "[Erreur] $1" >&2
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
    [ ! -f "$CSV" ] && erreur "Le fichier CSV '$CSV' est introuvable."
}

verifier_compilation() {
    if [ ! -f "$PROGRAMME_C" ]; then
        echo "Compilation du programme C via Makefile..."
        make || erreur "Échec de la compilation."
    fi
}

extraire_fichier_genere() {
    # On cherche la ligne FICHIER_GENERE dans la sortie du programme
    grep "FICHIER_GENERE:" programme_sortie.log | cut -d':' -f2 | tr -d '\r '
}

generer_png() {
    fichier_dat=$1
    type=$2

    [ ! -f "$fichier_dat" ] && erreur "Le fichier de données '$fichier_dat' n'existe pas."
    base="${fichier_dat%.*}"

    # Adaptation dynamique des titres et labels selon le type
    case $type in
        max)  
            titre="Capacité maximale" 
            label_y="Capacité (M.m3/an)"
            legende="Capacité"
            ;;
        src)  
            titre="Volume capté" 
            label_y="Volume (M.m3/an)"
            legende="Volume capté"
            ;;
        real) 
            titre="Volume traité" 
            label_y="Volume (M.m3/an)"
            legende="Volume traité"
            ;;
        *) 
            titre="Analyse Usines" 
            label_y="Volume (M.m3/an)"
            legende="Volume"
            ;;
    esac

    echo "Génération de l'image combinée (Top 10 & Top 50)..."

    # Fichiers temporaires pour le tri et Gnuplot
    head_f="$fichier_dat.header"
    tri_f="$fichier_dat.sorted"
    small_f="$fichier_dat.small"
    big_f="$fichier_dat.big"
    gp_script="plot_temp.gp"

    # Préparation des données : tri numérique par volume (colonne 2)
    head -n 1 "$fichier_dat" > "$head_f"
    tail -n +2 "$fichier_dat" | sort -t";" -k2,2g > "$tri_f"

    # Extraction des 50 plus petites et 10 plus grandes usines
    head -n 50 "$tri_f" > "$small_f"
    tail -n 10 "$tri_f" > "$big_f"

    # Génération du script Gnuplot pour une image unique (Multiplot)
    cat << EOF > "$gp_script"
set terminal png size 1000,1400 font "Arial,10"
set output '${base}_combined.png'
set datafile separator ';'
set style fill solid 0.7 border -1
set ylabel '$label_y'
set grid y

set multiplot layout 2,1 title "Projet C-WildWater : ${titre}\n" font "Arial,16"

# GRAPHE 1 : Les 10 plus grandes usines (Top 10)
set title "Top 10 : Plus grandes usines" font "Arial,12"
set xtics rotate by -45
plot '$big_f' using 2:xtic(1) with boxes lc rgb "#1f77b4" title "$legende"

# GRAPHE 2 : Les 50 plus petites usines (Top 50)
set title "Top 50 : Plus petites usines" font "Arial,12"
set xtics rotate by -90 font "Arial,7"
plot '$small_f' using 2:xtic(1) with boxes lc rgb "#2ca02c" title "$legende"

unset multiplot
EOF

    # Exécution de Gnuplot
    if command -v gnuplot >/dev/null 2>&1; then
        gnuplot "$gp_script"
        echo "✅ Image générée : ${base}_combined.png"
    else
        echo "⚠ Gnuplot non trouvé. Seul le fichier .dat est conservé."
    fi

    # Nettoyage des fichiers temporaires
    rm -f "$head_f" "$tri_f" "$small_f" "$big_f" "$gp_script"
}

traitement_histo() {
    local type_val=$1
    # On redirige la sortie pour capturer le nom du fichier généré
    $PROGRAMME_C "$CSV" histo "$type_val" > programme_sortie.log 2>&1
    ret=$?
    [ $ret -ne 0 ] && erreur "Le programme C a échoué (code $ret)."

    fichier_genere=$(extraire_fichier_genere)
    if [ -f "$fichier_genere" ]; then
        generer_png "$fichier_genere" "$type_val"
    else
        erreur "Le fichier .dat n'a pas pu être localisé."
    fi
}

traitement_leaks() {
    local id_usine=$1
    $PROGRAMME_C "$CSV" leaks "$id_usine" > programme_sortie.log 2>&1
    fichier_genere=$(extraire_fichier_genere)
    if [ -f "$fichier_genere" ]; then
        echo "Résultat de la recherche de fuites :"
        cat "$fichier_genere"
    else
        erreur "Résultat de fuites introuvable."
    fi
}

# --- Point d'entrée du script ---

# Vérification minimale : on attend au moins <csv> et <commande>
[ $# -lt 2 ] && usage

CSV="$1"
shift # On retire le CSV des arguments, $1 devient la commande

verifier_fichier
verifier_compilation

case $1 in
    histo)
        # Vérifie qu'on a bien l'argument type (max, src, real)
        if [ -z "$1" ] || [ -z "$2" ]; then
             usage
        fi
        traitement_histo "$2"
        ;;
    leaks)
        # Vérifie qu'on a bien l'identifiant de l'usine
        if [ -z "$1" ] || [ -z "$2" ]; then
             usage
        fi
        traitement_leaks "$2"
        ;;
    *)
        usage
        ;;
esac

duree_totale
exit 0
