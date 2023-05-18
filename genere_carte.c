#include <stdio.h>
#include <stdlib.h>
#include <time.h>



int main(int argc, char ** argv)
{
    int ROWS,COLS;
    printf("donne le nombre de lignes :\n");
    scanf("%d",&ROWS);
    printf("donne le nombre de colonnes :\n");
    scanf("%d",&COLS);
    int map[ROWS][COLS];
    int i, j;

    // Initialiser le générateur de nombres aléatoires
    srand(time(NULL));

    // Générer une carte aléatoire
    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLS; j++) {
            map[i][j] = rand() % 4; // Génère un nombre aléatoire entre 0 et 3 inclus
        }
    }

    // Stocker la carte dans un fichier texte
    FILE *file;
    file = fopen(argv[1], "w");

    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLS; j++) {
            fprintf(file, "%d", map[i][j]); 
            if (j < COLS - 1) { 
                fprintf(file, " ");
            }
        }
        fprintf(file, "\n");
    }
    fclose(file);

    // Afficher la carte à partir du fichier texte
    file = fopen(argv[1], "r");

    printf("Carte :\n");
    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLS; j++) {
            fscanf(file, "%d", &map[i][j]);
            printf("%d ", map[i][j]);
        }
        printf("\n");
    }

    fclose(file);

    return 0;
}
