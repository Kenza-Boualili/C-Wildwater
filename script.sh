#!/bin/bash

PROGRAMME_C="./c-wildwater"

FICHIER_HISTO_MAX="vol_max.dat"
FICHIER_HISTO_SRC="vol_captation.dat"
FICHIER_HISTO_REAL="vol_traitement.dat"
FICHIER_LEAKS="leaks.dat"

debut=$(date +%s)

erreur() {
    echo "[Erreur] $1"
    duree_totale
    exit 1
}

duree_totale() {
    fin=$(date +%s)
    echo "Durée totale : $(( (fin - debut) * 1000 )) ms"
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

generer_png() {
    fichier=$1
    type=$2

    oldIFS=$IFS
    IFS=$'\n'

    trap 'IFS=$oldIFS' RETURN
	
    if [ ! -f "$fichier" ]; then
        erreur "Le fichier de données '$fichier' n'existe pas."
    fi

    case $type in
        max)  titre="Capacité" ; base="vol_max" ;;
        src)  titre="Volume capté" ; base="vol_captation" ;;
        real) titre="Volume traité" ; base="vol_traitement" ;;
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
    echo "set output '${base}_small.png'" >> "$gp"
    echo "set title '${titre} (50 petites, M.m3/an)'" >> "$gp"
    echo "set xlabel 'Usines'" >> "$gp"
    echo "set ylabel 'Volume'" >> "$gp"
    echo "set xtics rotate by -45" >> "$gp"
    echo "set datafile separator ';'" >> "$gp"
    echo "set style fill solid" >> "$gp"
    echo "plot '$small' using (\$2/1000):xtic(1) with boxes lc rgb '#a020f0' title ''" >> "$gp"

    gnuplot "$gp"
    if [ $? -ne 0 ]; then
        echo "Erreur Gnuplot (50 petites). Nettoyage…"
        rm -f "$gp" "$head" "$tri" "$small" "$big"
        erreur "Impossible de générer l'image des 50 petites."
    fi

    echo "set output '${base}.png'" > "$gp"
    echo "set title '${titre} (10 grandes, M.m3/an)'" >> "$gp"
    echo "plot '$big' using (\$2/1000):xtic(1) with boxes lc rgb '#a020f0' title ''" >> "$gp"

    gnuplot "$gp"
    if [ $? -ne 0 ]; then
        echo "Erreur Gnuplot (10 grandes). Nettoyage…"
        rm -f "$gp" "$head" "$tri" "$small" "$big"
        erreur "Impossible de générer l'image des 10 grandes."
    fi

    rm -f "$gp" "$head" "$tri" "$small" "$big"

    echo "Images générées."
}() {
    fichier=$1
    type=$2

    oldIFS=$IFS
    IFS=$'\n'

    if [ ! -f "$fichier" ]; then
        IFS=$oldIFS
        erreur "Le fichier de données '$fichier' n'existe pas."
    fi

    case $type in
        max)  titre="Capacité" ; base="vol_max" ;;
        src)  titre="Volume capté" ; base="vol_captation" ;;
        real) titre="Volume traité" ; base="vol_traitement" ;;
        *) IFS=$oldIFS ; erreur "Type d'histogramme inconnu" ;;
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
    echo "set output '${base}_small.png'" >> "$gp"
    echo "set title '${titre} (50 petites, M.m3/an)'" >> "$gp"
    echo "set xlabel 'Usines'" >> "$gp"
    echo "set ylabel 'Volume'" >> "$gp"
    echo "set xtics rotate by -45" >> "$gp"
    echo "set datafile separator ';'" >> "$gp"
    echo "set style fill solid" >> "$gp"
    echo "plot '$small' using (\$2/1000):xtic(1) with boxes lc rgb '#a020f0' title ''" >> "$gp"

    gnuplot "$gp"
    if [ $? -ne 0 ]; then
        rm -f "$gp" "$head" "$tri" "$small" "$big"
        IFS=$oldIFS
        erreur "Erreur Gnuplot (50 petites)."
    fi

    echo "set output '${base}.png'" > "$gp"
    echo "set title '${titre} (10 grandes, M.m3/an)'" >> "$gp"
    echo "plot '$big' using (\$2/1000):xtic(1) with boxes lc rgb '#a020f0' title ''" >> "$gp"

    gnuplot "$gp"
    if [ $? -ne 0 ]; then
        rm -f "$gp" "$head" "$tri" "$small" "$big"
        IFS=$oldIFS
        erreur "Erreur Gnuplot (10 grandes)."
    fi

    rm -f "$gp" "$head" "$tri" "$small" "$big"

    IFS=$oldIFS

    echo "Images générées."
}"
}

traitement_histo() {
    type=$1

    case $type in
        max)  fichier=$FICHIER_HISTO_MAX ;;
        src)  fichier=$FICHIER_HISTO_SRC ;;
        real) fichier=$FICHIER_HISTO_REAL ;;
        *) erreur "Type histo non valide" ;;
    esac

    echo "Traitement histogramme ($type)..."

    $PROGRAMME_C histo "$type" "$CSV"
    retour=$?

    [ $retour -ne 0 ] && erreur "Erreur du programme C (code $retour)."
    [ ! -f "$fichier" ] && erreur "Fichier $fichier non généré."

    generer_png "$fichier" "$type"
}

traitement_leaks() {
    id="$1"

    [ -z "$id" ] && erreur "Aucun ID fourni."

    echo "Calcul des fuites pour $id..."

    $PROGRAMME_C leaks "$id" "$CSV"
    retour=$?

    case $retour in
        0) echo "Usine trouvée." ;;
        1) echo "Usine non trouvée. Ajout -1 dans $FICHIER_LEAKS." ;;
        *) erreur "Erreur critique (code $retour)." ;;
    esac

    if [ -f "$FICHIER_LEAKS" ]; then
        echo "Dernière entrée :"
        tail -n 1 "$FICHIER_LEAKS"
    else
        echo "Fichier $FICHIER_LEAKS introuvable."
    fi
}

[ $# -lt 2 ] && usage

CSV="$1"
shift

verifier_fichier
verifier_compilation

case $1 in
    histo)
        [ $# -ne 2 ] && erreur "Usage : histo <max|src|real>"
        traitement_histo "$2"
        ;;
    leaks)
        [ $# -ne 2 ] && erreur "Usage : leaks <ID>"
        traitement_leaks "$2"
        ;;
    *)
        erreur "Commande inconnue : $1"
        ;;
 esac

duree_totale


