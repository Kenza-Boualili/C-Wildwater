#!/bin/bash

# Configuration
# On pointe vers le dossier bin/ comme indiqué dans le README
PROGRAMME_C="./bin/c-wildwater" 
OUTPUT_DIR="output" 
LOG_FILE="$OUTPUT_DIR/programme_sortie.log"
debut=$(date +%s%3N)         # Capture du temps de début en millisecondes (%3N)

# Création du dossier de sortie s'il n'existe pas
mkdir -p "$OUTPUT_DIR"

# on force l'utilisation du point '.' comme séparateur décimal ( sa évite les bugs avec la virgule)
export LC_NUMERIC=C

erreur() {
    echo "[Erreur] $1" >&2
    rm -f "$CSV_FILTRE" # Nettoyage en cas d'erreur
    duree_totale
    exit 1
}

duree_totale() {
    fin=$(date +%s%3N)
    echo "Durée totale : $((fin - debut)) ms"
}

# Affiche les instructions d'utilisation si les arguments sont incorrects
usage() {
    echo "Usage : $0 <csv> <histo|leaks> <argument>"
    duree_totale
    exit 1
}

# Vérifie si le fichier CSV source existe bien avant de commencer
verifier_fichier() {
    [ ! -f "$CSV" ] && erreur "Le fichier CSV '$CSV' est introuvable."
}

# on compile le programme C avec 'make' s'il n'existe pas encore
verifier_compilation() {
    if [ ! -f "$PROGRAMME_C" ]; then
        echo "Compilation du programme C..."
        make || erreur "Échec de la compilation."
    fi
}

# Récupère le nom du fichier généré par le programme C en parsant ses logs de sortie
extraire_fichier_genere() {
    # On cherche le nom du fichier dans le log et on s'assure qu'il pointe vers output
    nom_fichier=$(grep "FICHIER_GENERE:" "$LOG_FILE" | cut -d':' -f2 | tr -d '\r ')
    echo "$OUTPUT_DIR/$nom_fichier"
}

generer_png() {
    fichier_dat="$1"
    type="$2"
    [ ! -f "$fichier_dat" ] && erreur "Fichier '$fichier_dat' introuvable."

    base_nom=$(basename "${fichier_dat%.*}")
    image_png="$OUTPUT_DIR/${base_nom}.png"
    
    case "$type" in
        max) titre="Capacité maximale"; label_y="Capacité (M.m3/an)"; legende="Capacité"; col_tri=2 ;;
        src) titre="Volume capté"; label_y="Volume (M.m3/an)"; legende="Volume capté"; col_tri=3 ;;
        real) titre="Volume traité"; label_y="Volume (M.m3/an)"; legende="Volume traité"; col_tri=3 ;;
        *) erreur "Type d'histogramme inconnu" ;;
    esac
    
    echo " 📊 Génération de l'image : $image_png"

# Noms des fichiers temporaires pour le tri (placés dans output/)
    data_tmp="$OUTPUT_DIR/data.tmp"
    sorted_tmp="$OUTPUT_DIR/sorted.tmp"
    small_f="$OUTPUT_DIR/small.tmp"
    big_f="$OUTPUT_DIR/big.tmp"
    gp_script="$OUTPUT_DIR/plot.gp"

#On enlève l'en-tête pour ne garder que les données
    tail -n +2 "$fichier_dat" > "$data_tmp"

#Tri numérique (-g) sur la colonne choisie avec le séparateur ';'
    sort -t";" -k3,3g "$data_tmp" > "$sorted_tmp"

#On isole les 50 plus petites et les 10 plus grandes usines
    head -n 50 "$sorted_tmp" > "$small_f"
    tail -n 10 "$sorted_tmp" > "$big_f"

#Création du script de commande pour Gnuplot
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
set title "10 plus grandes usines (référence: capacité)"
set xtics rotate by -45
plot "$big_f" using (\$3/1000.0):xtic(1) with boxes lc rgb "#1f77b4" title "$legende"
set title "50 plus petites usines (référence: capacité)"
set xtics rotate by -90 font "Arial,7"
plot "$small_f" using (\$3/1000.0):xtic(1) with boxes lc rgb "#1f77b4" notitle
unset multiplot
EOF

#Exécution de Gnuplot et nettoyage des fichiers temporaires
    if command -v gnuplot >/dev/null 2>&1; then
        gnuplot "$gp_script"
    fi
    rm -f "$data_tmp" "$sorted_tmp" "$small_f" "$big_f" "$gp_script"
}


traitement_histo() {
    local type="$1"
    
    # grep -E : cherche les lignes contenant "Spring", "Source" ou ";-;" (les usines)
    # - | envoie le résultat directement au programme C
    # le "-" dit au C de lire ce qui vient du pipe
    # - > programme_sortie.log 2>&1 : envoie tous les messages (erreurs et infos) dans un fichier log
    grep -E "Spring|Source|;-;" "$CSV" | $PROGRAMME_C - histo "$type" > "$LOG_FILE" 2>&1

    # PIPESTATUS[1] regarde si le programme C a réussi
    # Si c'est différent de 0 on appelle la fonction erreur.
    [ ${PIPESTATUS[1]} -ne 0 ] && erreur "Le programme C a échoué. Voir $LOG_FILE"

nom_brut=$(grep "FICHIER_GENERE:" "$LOG_FILE" | cut -d':' -f2 | tr -d '\r ')

if [ -f "$nom_brut" ]; then
        mv "$nom_brut" "$OUTPUT_DIR/"
    fi

# On appelle la fonction pour savoir quel fichier .dat a été créé
    fichier_dat="$OUTPUT_DIR/$nom_brut"

# On lance Gnuplot pour transformer les données .dat en graphique .png
    generer_png "$fichier_dat" "$type"
}

traitement_leaks() {
    #On récupère l'identifiant de l'usine passé en argument
    local id="$1"
    
    # - grep -F : Recherche très rapide, on extrait toutes les lignes 
    # - | : Le PIPE envoie ces lignes (contenant de l'id) directement au programme C
    # - $PROGRAMME_C - : Le "-" indique au programme C de lire les données 
    # depuis le flux entrant (le pipe) au lieu d'ouvrir lui-même le gros fichier
    grep -F "$id" "$CSV" | $PROGRAMME_C - leaks "$id" > "$LOG_FILE" 2>&1
    
  [ ${PIPESTATUS[1]} -ne 0 ] && erreur "Le programme C a échoué."

    fichier_dat=$(extraire_fichier_genere)
    cat "$fichier_dat"
}

# point d'entrée
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