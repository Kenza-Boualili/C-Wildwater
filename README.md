# 💧 C-WildWater — Analyse d’un Réseau de Distribution d’Eau

## 📘 Description

Le projet **C-WildWater** est une application combinant **un script Shell** et **un programme en langage C** permettant d’analyser et de synthétiser les données d’un **réseau de distribution d’eau potable**.

À partir d’un fichier de données CSV volumineux fourni par l’enseignante, le programme permet :
- d’analyser les performances des usines de traitement,
- de calculer les volumes d’eau captés, réellement traités et perdus,
- de générer des fichiers de résultats,
- de produire des histogrammes exploitables.

Les données sont factices mais respectent les ordres de grandeur d’un réseau réel.

---

## 📁 Structure du projet

```
.
├── README.md
├── Makefile
├── script.sh              <- Script Shell principal
├── src/                   <- Fichiers sources C
├── include/               <- Fichiers d’en-tête
├── bin/                   <- Exécutable généré
├── data/
│   └── c-wildwater_v3.dat <- Fichier CSV (fourni par l’enseignante)
└── output/                <- Fichiers de sortie (CSV, images, .dat)
```

📌 **Important** : le fichier `c-wildwater_v3.dat` n’est pas fourni dans le dépôt.  
Il doit être **copié manuellement** par l’enseignante dans le dossier du projet.

---

## 🛠️ Installation

1. Cloner ou télécharger le projet.
2. Ouvrir un terminal et se placer dans le dossier du projet.
3. Compiler le programme C :

```bash
make
```

4. Donner les droits d’exécution au script **et au fichier de données** :

```bash
chmod +x script.sh
chmod +x c-wildwater_v3.dat
```

---

## ▶️ Utilisation

### 📊 Histogrammes des usines

```bash
./script.sh c-wildwater_v3.dat histo max
./script.sh c-wildwater_v3.dat histo src
./script.sh c-wildwater_v3.dat histo real
```

- `max`  : capacité maximale de traitement des usines  
- `src`  : volume total capté par les sources  
- `real` : volume réellement traité après pertes  

Le script génère :
- un fichier de données CSV,
- deux histogrammes :
  - les **50 plus petites usines**,
  - les **10 plus grandes usines**.

Les fichiers sont sauvegardés dans le dossier `output/`.

---

### 🚰 Calcul des pertes d’eau (leaks)

```bash
./script.sh c-wildwater_v3.dat leaks "Facility complex #RH400057F"
```

- Calcule le **volume total d’eau perdu** sur l’ensemble du réseau aval de l’usine.
- Résultat exprimé en **millions de m³ (M.m³)**.
- Si l’identifiant de l’usine n’existe pas, la valeur `-1` est retournée.
- Les résultats sont ajoutés à un fichier historique `.dat`.

---

## 🎯 Fonctionnalités

- Lecture et traitement de fichiers CSV très volumineux
- Analyse des performances des usines de traitement
- Calcul des volumes :
  - captés,
  - réellement traités,
  - perdus (fuites)
- Génération de fichiers de résultats triés
- Création d’histogrammes (PNG)
- Gestion robuste des erreurs et des arguments
- Optimisation des performances (structures adaptées)

---

## 📚 Documentation

- `rapport.pdf` :
  - description du projet,
  - choix techniques,
  - structures de données utilisées,
  - répartition des tâches,
  - limitations fonctionnelles,
  - exemples d’exécution.

---

## 👥 Auteurs

Projet réalisé dans le cadre du cursus **préING2 – 2025/2026**.

- Kenza Boualili  
- Shahd Eish  
- Tenzin Dadon  

Encadrants pédagogiques :
- Eva Ansermin  
- Romuald Grignon  

---

## 📄 Licence

Projet académique — usage pédagogique uniquement.
