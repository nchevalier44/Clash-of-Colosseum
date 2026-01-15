# Clash of Colosseum ⚔️

**Clash of Colosseum** est une simulation de combat et d'évolution en temps réel développée en **C++** avec la bibliothèque **SDL2**. Le projet met en scène des affrontements autonomes entre différentes classes d'unités (Guerriers, Archers, Mages, Golems), intégrant des mécaniques de reproduction et de statistiques.

## 🚀 Fonctionnalités

### Simulation de Combat
* **Affrontements autonomes** : Les entités se déplacent, détectent les ennemis et combattent sans intervention du joueur. Le multi-threading permet aux entités d'être indépendantes.
* **Classes variées** :
    * 🗡️ **Guerrier** : Combattant au corps-à-corps polyvalent.
    * 🏹 **Archer** : Unité à distance rapide et agile.
    * 🔥 **Mage** : Lanceur de boule de feu.
    * 🪨 **Golem** : Unité tank, lente mais très résistante.

### Système Génétique
* **Générations** : À la fin de chaque simulation, une nouvelle génération est créée basée sur les survivants.
* **Emjambement** : Un algorithme génétique similaire à la réalité utilisant le principe de l'emjambement est mis en place. Sur chaque attribut (PV, dégâts, ...), on prend une partie de cette attribut d'un parent et une partie de l'autre parent.
* **Mutations** :
    * **Statistiques** : Les PV, les dégâts, la vitesse, la taille et la portée peuvent muter aléatoirement.
    * **Types** : Une entité peut changer de classe (ex: un enfant de Guerrier peut devenir un Archer).
* **Paramétrage** : Taux de mutation configurables via le menu.

### Interface et Menus
* **Menu Principal** : Accès rapide à la simulation, aux paramètres et à l'historique.
* **Paramètres** :
    * Taille de la population initiale (Nombre de guerriers).
    * Taux de mutation (Type et Stats).
    * "Paix raciale" (Option `same_type_peace`) : Empêche les entités du même type de s'attaquer.
    * Vitesse des projectiles.
    * Musique et affichage des barres de vie.
* **Historique** : Visualisation des statistiques des générations passées (durée, population restante, répartition des types).

## 🛠️ Prérequis

Le projet nécessite un compilateur compatible **C++** et **CMake**.

### Dépendances SDL2
Les bibliothèques suivantes doivent être installées sur votre système :
* `SDL2`
* `SDL2_gfx`
* `SDL2_mixer`
* `SDL2_image`
* `SDL2_ttf`

#### Installation (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install libsdl2-dev libsdl2-gfx-dev libsdl2-mixer-dev libsdl2-image-dev libsdl2-ttf-dev cmake g++
```

## 📦 Compilation et Installation

1. **Cloner le dépôt**
```bash
git clone [https://github.com/votre-username/Clash-of-Colosseum.git](https://github.com/votre-username/Clash-of-Colosseum.git)
cd Clash-of-Colosseum
```


2. **Créer le dossier de build**
```bash
mkdir build
cd build
```


3. **Configurer le projet avec CMake**
```bash
cmake ..
```


4. **Compiler**
```bash
make
```



## 🎮 Utilisation

Lancez l'exécutable depuis le dossier `build` :

```bash
./Clash_of_Colosseum
```

### Navigation

* Utilisez la **souris** pour interagir avec les boutons du menu.
* Dans tous les menus appuyez sur **Entrer** pour revenir au menu précédent ou, si vous êtes dans le menu principal, lancer la simulation  
* Le menu **Paramètres** vous permet d'ajuster la simulation avant de la lancer (nombre de guerriers, musique, affichage des barres de vie, etc.).
* Utilisez les **4 flèches** dans le menu **Paramètres** pour changer les données par défaut
* Le bouton **Historique** affiche les données des simulations précédentes.
* Utilisez les flèches de droite et de gauche pendant le jeu pour augmenter ou réduire la vitesse du temps

## 📂 Structure du Projet

* `src/main.cpp` : Point d'entrée, initialisation SDL et boucle principale du jeu.
* `src/Graphics.cpp` : Boucle principal : Gestion de l'animation, de la simulation, de la reproduction.
* `src/SimulationStats.cpp` : Logique de collecte des données et gestion des générations.
* `src/Entities/` : Classes définissant les comportements des unités (`Entity`, `Guerrier`, `Archer`, `Mage`, `Golem`).
* `src/Menu/` : Gestion de l'interface utilisateur (`MainMenu`, `SettingsMenu`, `HistoryMenu`, `Button`).
* `CMakeLists.txt` : Configuration de la compilation.

## 📄 Licence

Ce projet a été réalisé dans un cadre éducatif par Nathan Chevalier et Timéo Ménard.