#!/bin/bash

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
	echo "  histo all   : toutes les données"
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

trap 'rm -f plot_temp.gp *.header *.sorted *.small *.big' EXIT

generer_png() {
    fichier=$1
    type=$2
	
    [ ! -f "$fichier" ] && erreur "Le fichier de données '$fichier' n'existe pas."
    
    base="${fichier%.*}"

    case $type in
        max)  titre="Capacité maximale" ;;
        src)  titre="Volume capté" ;;
        real) titre="Volume traité" ;; 
        *) erreur "Type d'histogramme inconnu" ;;
esac

    echo "Génération des graphiques ($type)..."

    head="$fichier.header"
    tri="$fichier.sorted"
    small="$fichier.small"
    big="$fichier.big"
    gp="plot_temp.gp"

    head -n 1 "$fichier" > "$head"
   tail -n +2 "$fichier" | sort -t";" -k2,2n > "$tri"

    cat "$head" > "$small"
    head -n 50 "$tri" >> "$small"

    cat "$head" > "$big"
    tail -n 10 "$tri" >> "$big"
    echo "set terminal png size 1200,800" > "$gp"
    echo "set datafile separator ';'" >> "$gp"
    echo "set style fill solid" >> "$gp"
    echo "set xtics rotate by -45" >> "$gp"
    echo "set xlabel 'Usines'" >> "$gp"
    echo "set ylabel 'Volume (M.m3/an)'" >> "$gp"

    echo "set output '${base}_small.png'" >> "$gp"
    echo "set title '${titre} (50 plus petites usines)'" >> "$gp"
    echo "plot '$small' using 2:xtic(1) with boxes title ''" >> "$gp"
    gnuplot "$gp" || erreur "Erreur Gnuplot (50 petites usines)"

    echo "set output '${base}_big.png'" > "$gp"
    echo "set title '${titre} (10 plus grandes usines)'" >> "$gp"
    echo "plot '$big' using 2:xtic(1) with boxes title ''" >> "$gp"
    gnuplot "$gp" || erreur "Erreur Gnuplot (10 grandes usines)"

    echo "Images générées : ${base}_small.png et ${base}_big.png"
}
    

traitement_histo() {
    type=$1
	
	case "$type" in
        max|src|real|all) ;;
        *) erreur "Option histo invalide : $type" ;;
    esac
	
    echo "Traitement histogramme ($type)..."
	sortie=$($PROGRAMME_C "$CSV" histo "$type" 2>&1)
    retour=$?

    [ $retour -ne 0 ] && erreur "Erreur du programme C (code $retour)."
    
    fichier=$(echo "$sortie" | grep "FICHIER_GENERE:" | cut -d: -f2)
   [ -z "$fichier" ] && erreur "Nom de fichier non récupéré du programme C."
    [ ! -f "$fichier" ] && erreur "Fichier $fichier non généré."
	echo "Fichier généré : $fichier"

    generer_png "$fichier" "$type"
}

traitement_leaks() {
    id="$1"

    [ -z "$id" ] && erreur "Aucun ID fourni."

    echo "Calcul des fuites pour $id..."
    sortie=$($PROGRAMME_C "$CSV" leaks "$id" 2>&1)
    retour=$?

    fichier=$(echo "$sortie" | grep "FICHIER_GENERE:" | cut -d: -f2)

    [ -z "$fichier" ] && erreur "Nom de fichier non récupéré du programme C."

    case $retour in
        0) echo "Usine trouvée et fuites calculées." ;;
        1) echo "Usine non trouvée. Valeur -1 enregistrée" ;;
        *) erreur "Erreur critique du programme C (code $retour)." ;;
esac

    [ -f "$fichier" ] || erreur "Fichier $fichier introuvable"
    echo "Fichier généré : $fichier"
    cat "$fichier"
}

[ $# -lt 2 ] && usage

CSV="$1"
shift

verifier_fichier
verifier_compilation
command -v gnuplot >/dev/null || erreur "gnuplot non installé"


case $1 in
    histo)
        [ $# -ne 2 ] && erreur "Usage : histo <max|src|real>"
        traitement_histo "$2"
        ;;
    leaks)
        [ $# -ne 2 ] && erreur "Usage : leaks <ID_usine>"
        traitement_leaks "$2"
        ;;
    *)
        erreur "Commande inconnue : $1"
        ;;
esac
duree_totale


