# 🌊 C-WildWater — Projet Informatique préING2

## 📌 Description générale
Ce projet consiste à analyser et synthétiser des données issues d’un **système de distribution d’eau potable**, à partir d’un fichier CSV volumineux (jusqu’à plusieurs millions de lignes).

Les traitements sont réalisés via :
- un **script Shell** (point d’entrée utilisateur),
- un **programme en langage C** optimisé pour les performances et la gestion mémoire.

Le projet respecte strictement les contraintes pédagogiques imposées (C + Shell, AVL, Makefile, robustesse).

---

## 🗂️ Arborescence du projet

> ⚠️ Pour des raisons de simplicité, **tous les fichiers sont placés à la racine du projet**.

```
C-WildWater/
├── main.c
├── csv_reader.c / csv_reader.h
├── avl.c / avl.h
├── tree.c / tree.h
├── calculations.c / calculations.h
├── utils.c / utils.h
├── script.sh
├── Makefile
├── README.md
├── Projet_C-WildWater.pdf
```

---

## ⚙️ Compilation

La compilation se fait exclusivement via **Make**, conformément au sujet.

```bash
make
```

Pour nettoyer les fichiers générés :

```bash
make clean
```

Pour nettoyer **tous les fichiers générés automatiquement** (binaires, fichiers temporaires, résultats) :

```bash
make cleanfile
```

---

## 🚀 Utilisation

Le point d’entrée est le script Shell :

```bash
./script.sh <fichier_donnees.csv> <commande> [option]
```

### 📊 Histogrammes des usines

```bash
./script.sh wildwater.dat histo max
./script.sh wildwater.dat histo src
./script.sh wildwater.dat histo real
```

- `max`  : capacité maximale de traitement
- `src`  : volume total capté depuis les sources
- `real` : volume réellement traité après fuites

📁 Génère :
- un fichier de données (.dat / .csv)
- deux images PNG (50 plus petites / 10 plus grandes usines)

---

### 💧 Calcul des fuites d’une usine

```bash
./script.sh wildwater.dat leaks "Facility complex #RH400057F"
```

Résultat :
- volume total d’eau perdue (en M.m³/an)
- ajout dans un fichier historique `.dat`

⚠️ Si l’identifiant est inexistant → résultat `-1`.

---

## 🧠 Choix techniques

### 🔹 Structures de données
- **AVL** :
  - accès rapide aux informations des usines
  - évite des recherches lentes sur de gros fichiers
- **Arbre de distribution** :
  - représentation du réseau aval
  - enfants stockés via listes chaînées

Ces choix permettent :
- de garder des temps de calcul raisonnables
- de traiter des fichiers de grande taille sans ralentissements excessifs

---

## 🛡️ Robustesse & erreurs
- Vérification systématique des arguments
- Codes de retour strictement positifs en cas d’erreur
- Aucun arrêt brutal (segmentation fault)
- Données invalides détectées et gérées

---

## 📄 Documentation complémentaire

Le fichier **Projet_C-WildWater.pdf** contient :
- la répartition des tâches
- le planning du projet
- les limitations fonctionnelles

---

## ⚠️ Limitations connues
- Bonus non implémentés (histogramme cumulé, plus grosse fuite)
- Optimisations mémoire possibles sur très gros fichiers
- Messages d’erreur perfectibles dans certains cas limites

---

## 👥 Auteurs
- Étudiants préING2 — CY Tech  
- Projet réalisé dans un cadre pédagogique (2025–2026)

---

## 📜 Licence

**Projet réalisé dans le cadre du cours d'informatique. Usage académique uniquement.**

---

## ✅ Conformité au sujet
✔ Langage C obligatoire respecté  
✔ Script Shell comme point d’entrée  
✔ AVL utilisés conformément aux consignes  
✔ Makefile présent  
✔ Projet générique (non codé en dur)

---

> 📢 **Remarque** : Ce programme a été conçu pour fonctionner avec n’importe quel fichier CSV respectant la structure du sujet, y compris lors d’une évaluation avec un fichier différent.

