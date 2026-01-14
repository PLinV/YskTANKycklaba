# PROCÉDURE DE COMPILATION / MISE À JOUR

## 1. Récupérer la dernière version: 
    -> Attention ça supprime ce que vous avez en local
Vous devez être sur votre branche
```bash
git fetch origin
git reset --hard origin/main
git clean -fd
git push origin VotreBranche --force
```

## 2. RECOMPILER RAYLIB (OBLIGATOIRE si changement d'OS ou MAJ)
`libraylib.a`: n'est pas compatible entre Windows et Linux.

1.  Allez dans le dossier source de raylib :
    ```bash
    cd raylib/src
    ```
2.  Nettoyez et recompilez pour votre ordinateur :
    ```bash
    make clean
    make PLATFORM=PLATFORM_DESKTOP
    ```
3.  Revenez à la racine du projet :
    ```bash
    cd ../..
    ```

## 3. COMPILER LE JEU
```bash
make clean
make
```
*Le `makefile` détecte automatiquement si vous êtes sur Windows ou Linux et ajuste les réglages.*

## 4. LANCER LE JEU
```bash
./prog
```
