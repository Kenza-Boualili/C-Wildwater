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
    echo "  histo max   : capacité maximale (Axe Y: Quantité)"
    echo "  histo src   : volume capté (Axe Y: Volume)"
    echo "  histo real  : volume traité (Axe Y: Volume)"
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
    label_y="Volume" 

    case $type in
        max)  
            titre="Capacité maximale" 
            label_y="Quantité" 
            ;;
        src)  
            titre="Volume capté" 
            ;;
        real) 
            titre="Volume traité" 
            ;; 
        *) erreur "Type d'histogramme inconnu" ;;
    esac

    echo "Génération des graphiques ($type)..."

    head="$fichier.header"
    tri="$fichier.sorted"
    small="$fichier.small"
    big="$fichier.big"
    gp="plot_temp.gp"
    
    head -n 1 "$fichier" > "$head"
    tail -n +2 "$fichier" | sort -t";" -k2,2g > "$tri"

    if [ ! -s "$tri" ]; then
        erreur "Le fichier de données trié est vide." 
    fi

    cat "$head" > "$small"
    head -n 50 "$tri" >> "$small"
    cat "$head" > "$big"
    tail -n 10 "$tri" >> "$big"

    common_cfg="set datafile separator ';'; 
                set style fill solid; 
                set xtics rotate by -45; 
                set xlabel 'Identifiants'; 
                set ylabel '$label_y (M.m3/an)';"

    echo "set terminal png size 1200,800; set output '${base}_small.png';" > "$gp"
    echo "$common_cfg" >> "$gp"
    echo "set title '${titre} (50 plus petites usines)';" >> "$gp"
    echo "plot '$small' using 2:xtic(1) with boxes lc rgb 'blue' title '';" >> "$gp"
    gnuplot "$gp" || erreur "Erreur Gnuplot (50 petites)"

    echo "set terminal png size 1200,800; set output '${base}_big.png';" > "$gp"
    echo "$common_cfg" >> "$gp"
    echo "set title '${titre} (10 plus grandes usines)';" >> "$gp"
    echo "plot '$big' using 2:xtic(1) with boxes lc rgb 'blue' title '';" >> "$gp"
    gnuplot "$gp" || erreur "Erreur Gnuplot (10 grandes)"

    echo "Images générées : ${base}_small.png et ${base}_big.png"
}

traitement_histo() {
    type=$1
    case "$type" in
        max|src|real) ;;
        *) erreur "Option histo invalide : $type" ;;
    esac
    
    echo "Traitement histogramme ($type)..."
    sortie=$($PROGRAMME_C "$CSV" histo "$type" 2>&1)
    retour=$?

    [ $retour -ne 0 ] && erreur "Erreur du programme C."
    
    fichier=$(echo "$sortie" | grep "FICHIER_GENERE:" | cut -d: -f2)
    [ -z "$fichier" ] && erreur "Nom de fichier non récupéré."
    
    generer_png "$fichier" "$type"
}

traitement_leaks() {
    id="$1"
    echo "Calcul des fuites pour $id..."
    sortie=$($PROGRAMME_C "$CSV" leaks "$id" 2>&1)
    fichier=$(echo "$sortie" | grep "FICHIER_GENERE:" | cut -d: -f2)
    [ -f "$fichier" ] || erreur "Fichier $fichier introuvable"
    cat "$fichier"
}
[ $# -lt 2 ] && usage
CSV="$1"
shift

verifier_fichier
verifier_compilation

case $1 in
    histo) traitement_histo "$2" ;;
    leaks) traitement_leaks "$2" ;;
    *) usage ;;
esac

duree_totale
