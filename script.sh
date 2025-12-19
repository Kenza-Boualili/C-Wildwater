#!/bin/bash

# Configuration
PROGRAMME_C="./bin/c-wildwater" 
OUTPUT_DIR="output" 
LOG_FILE="$OUTPUT_DIR/programme_sortie.log"
debut=$(date +%s%3N)

# Force l'utilisation du point '.' comme séparateur décimal
export LC_NUMERIC=C

# --- Fonctions utilitaires ---

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
    echo "Usage : $0 <csv> <histo|leaks> <argument>"
    duree_totale
    exit 1
}

verifier_fichier() {
    [ ! -f "$CSV" ] && erreur "Le fichier CSV '$CSV' est introuvable."
}

verifier_compilation() {
    if [ ! -f "$PROGRAMME_C" ]; then
        echo "Compilation du programme C..."
        make || erreur "Échec de la compilation."
    fi
}

extraire_fichier_genere() {
    # On récupère le nom du fichier brut noté dans le log
    grep "FICHIER_GENERE:" "$LOG_FILE" | cut -d':' -f2 | tr -d '\r '
}

# --- Traitement des Graphiques ---

generer_png() {
    fichier_dat="$1" # Reçoit le chemin complet : output/histo/nom.dat
    type="$2"
    dossier_cible=$(dirname "$fichier_dat")
    base_nom=$(basename "${fichier_dat%.*}")
    image_png="$dossier_cible/${base_nom}.png"
    
    # Sélection de la colonne de données selon le type
    case "$type" in
        max) titre="Capacité maximale"; label_y="Capacité (M.m3/an)"; legende="Capacité"; col_tri=2 ;;
        src) titre="Volume capté"; label_y="Volume (M.m3/an)"; legende="Volume capté"; col_tri=3 ;;
        real) titre="Volume traité"; label_y="Volume (M.m3/an)"; legende="Volume traité"; col_tri=3 ;;
        *) erreur "Type d'histogramme inconnu" ;;
    esac

    echo " 📊 Génération de l'image : $image_png"

    # Fichiers de travail temporaires dans le sous-dossier
    data_tmp="$dossier_cible/data.tmp"
    sorted_tmp="$dossier_cible/sorted.tmp"
    small_f="$dossier_cible/small.tmp"
    big_f="$dossier_cible/big.tmp"
    gp_script="$dossier_cible/plot.gp"

    tail -n +2 "$fichier_dat" > "$data_tmp"
    # Tri numérique sur la colonne col_tri spécifiée
    sort -t";" -k${col_tri},${col_tri}g "$data_tmp" > "$sorted_tmp"
    head -n 50 "$sorted_tmp" > "$small_f"
    tail -n 10 "$sorted_tmp" > "$big_f"

cat > "$gp_script" << EOF
set terminal png size 1200,1400 font "Arial,10"
set output "$image_png"
set datafile separator ";"
set style fill solid 0.7 border -1
set ylabel "$label_y"
set grid y
set yrange [0:*]
set boxwidth 0.7 relative
set multiplot layout 2,1 title "Projet C-WildWater : $titre" font "Arial,16"
set title "10 plus grandes usines"
set xtics rotate by -45
plot "$big_f" using (\$${col_tri}/1000.0):xtic(1) with boxes lc rgb "#1f77b4" title "$legende"
set title "50 plus petites usines"
set xtics rotate by -90 font "Arial,7"
plot "$small_f" using (\$${col_tri}/1000.0):xtic(1) with boxes lc rgb "#1f77b4" notitle
unset multiplot
EOF

    if command -v gnuplot >/dev/null 2>&1; then
        gnuplot "$gp_script"
    fi
    rm -f "$data_tmp" "$sorted_tmp" "$small_f" "$big_f" "$gp_script"
}

# --- Commandes Principales ---

traitement_histo() {
    local type="$1"
    local HISTO_DIR="$OUTPUT_DIR/histo"

    # Exécution du programme C
    grep -E "Spring|Source|;-;" "$CSV" | $PROGRAMME_C - histo "$type" > "$LOG_FILE" 2>&1
    [ ${PIPESTATUS[1]} -ne 0 ] && erreur "Le programme C a échoué."

    nom_brut=$(extraire_fichier_genere)
    
    # On déplace le .dat vers le sous-dossier histo
    if [ -f "$nom_brut" ]; then
        mv "$nom_brut" "$HISTO_DIR/"
    fi
    
    # On génère l'image dans le même sous-dossier
    generer_png "$HISTO_DIR/$nom_brut" "$type"
}

traitement_leaks() {
    local id="$1"
    local LEAKS_DIR="$OUTPUT_DIR/leaks"

    # Exécution du programme C pour les fuites
    grep -F "$id" "$CSV" | $PROGRAMME_C - leaks "$id" > "$LOG_FILE" 2>&1
    [ ${PIPESTATUS[1]} -ne 0 ] && erreur "Le programme C a échoué."

    nom_brut=$(extraire_fichier_genere)
    
    # On déplace le .dat vers le sous-dossier leaks
    if [ -f "$nom_brut" ]; then
        mv "$nom_brut" "$LEAKS_DIR/"
    fi
    
    echo "📁 Résultat sauvegardé dans : $LEAKS_DIR/$nom_brut"
    cat "$LEAKS_DIR/$nom_brut"
}

# --- Point d'entrée ---
[ $# -lt 2 ] && usage
CSV="$1"
shift 
verifier_fichier
verifier_compilation

case "$1" in
    histo) [ -z "$2" ] && usage; traitement_histo "$2" ;;
    leaks) [ -z "$2" ] && usage; traitement_leaks "$2" ;;
    *) usage ;;
esac

duree_totale
exit 0