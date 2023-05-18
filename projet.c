// ------------- importation des modules -------------------------
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <unistd.h>
// ----------------- quelques parametres necessaires ----------------
#define INF 99
const int TILE_SIZE = 60;
const int PADDING = 20;
// ------------------definition du robot ----------------------------
struct Robot{
    int capacite_max;
    int contenu;
    int x;
    int y;
    int* vitesse;
    int debit;
}typedef Robot;

Robot *initialise(int capacite, int abscisse, int ordonne, int contenu,int v_montaigne,int v_ville,int v_foret, int debit){
    Robot *new_robot = malloc(sizeof(Robot));
    new_robot ->capacite_max = capacite;
    new_robot ->x = abscisse;
    new_robot->y = ordonne;
    new_robot ->contenu = contenu;
    new_robot->vitesse = (int*)malloc(sizeof(int)*3);
    *new_robot->vitesse = v_montaigne;
    *(new_robot->vitesse+1) = v_ville;
    *(new_robot->vitesse+2) = v_foret;
    new_robot->debit = debit;
    return new_robot;

}
// --------------------definition du feu -------------------
struct feu{
    int x;
    int y;
    int taille;
}typedef Feu;
Feu * initialise_feu(int x, int y,int taille){
    Feu * new_fire = malloc(sizeof(Feu));
    new_fire->taille=taille;
    new_fire->x=x;
    new_fire->y=y;
    return new_fire;
}
void delete(Feu * feu){
    free(feu);
}
void apparition_feu(int ** carte, Feu * feu){
    carte[feu->x][feu->y] = feu->taille;
}
void vidage(Robot * robot, Feu * feu){
    while(feu->taille!=0){
        printf("vidage d'eau ... sa capacité : %d\n", robot->contenu);
        robot->contenu -= 1;
        feu->taille -= 1;
        int wait_time = (int) ceil(1/robot->debit);
        SDL_Delay(wait_time* 1000);
        }
}
//------------------------fonction qui genere aleatoirement le feu dans la carte -------
Feu* generate_fire(int** map, int nlignes, int ncolones, int taille_max) {
    srand(time(NULL));
    int x, y;
    do {
        x = rand() % nlignes-1;
        y = rand() % ncolones-1;
    } while (map[y][x] == 0); //--------le feu ne doit pas emerger dans une case d'eau--
    
    Feu* feu = malloc(sizeof(Feu));
    feu->x = x;
    feu->y = y;
    feu->taille = rand() % taille_max + 1;
    printf("la taille du feu : %d\n",feu->taille);
    return feu;
}
/*--------------------chemin plus court--------------------------------*/   

int indices_to_index(int row , int col, int *dimensions){
    return row*dimensions[1] + col;
}

int *index_to_indices(int index , int *dimensions){
    int *indices = malloc(sizeof(int)*2);
    indices[1] = index % dimensions[1];
    indices[0] = (index - indices[1]) / dimensions[1] ; 
    return indices;
}

int **init_matrice(int *dimension,int value){
	int **map = malloc(sizeof(int)*dimension[0]*dimension[1]);
	for (int i = 0; i < dimension[0]; i++)
	{
		int *row = malloc(dimension[1]*sizeof(int)); 
		for (int j = 0; j < dimension[1]; j++)
		{
			row[j] = value;
		}
		map[i] = row;
	}
	return map;	
}

void update_distmap(int **dist_map,int **visited_map,int *dim){
    for (int i = 0; i < dim[0]; i++)
    {
        for (int j = 0; j < dim[1]; j++)
        {
            if (visited_map[i][j] == 1)
            {
                dist_map[i][j] = INF;
            }
        }
        
    }
    
}
int find_argmin(int **dist_map,int *dim){
    int min = INF;
    int imin = 0;
    int jmin = 0;
    for (int i = 0; i < dim[0]; i++)
    {
        for (int j = 0; j < dim[1]; j++)
        {
            if (dist_map[i][j]<min)
            {
                min = dist_map[i][j];
                imin = i;
                jmin = j;
            }
            
        }
        
    }
    return indices_to_index(imin,jmin,dim);   
}




int** dijkstra(int* dim,int **map, int x, int y,int xe,int ye){
    int cpt = 0;
    int **path = NULL;
    int **dist_map = init_matrice(dim,INF); //stock les distance de chaque noeud depuis la source
    
    int **origin_map = init_matrice(dim,-1); //stock la valeur du noeud precedent
    
    int **visited_map = init_matrice(dim,0); //1 si le noeud est visite , 0 sinon
    
    int finished = 0;
    int count = 0;
    int xs = x;
    int ys = y;
    dist_map[ys][xs] = 0;
    
    while (finished == 0)
    {
        
        
        //check x+1 y
        if (xs < dim[1]-1)
        {
            if (dist_map[ys][xs+1]>map[ys][xs+1]+dist_map[ys][xs] && visited_map[ys][xs+1] != 1 )
            {
                dist_map[ys][xs+1] = map[ys][xs+1]+dist_map[ys][xs];
                origin_map[ys][xs+1] = indices_to_index(ys,xs,dim);
                
            }  
        }

        //check x-1 y
        if (xs > 0)
        {
            if (dist_map[ys][xs-1]>map[ys][xs-1]+dist_map[ys][xs] && visited_map[ys][xs-1] != 1 )
            {
                dist_map[ys][xs-1] = map[ys][xs-1]+dist_map[ys][xs];
                origin_map[ys][xs-1] = indices_to_index(ys,xs,dim);
            }  
        }

        //check x y+1
        if (ys < dim[0]-1)
        {
            
            if (dist_map[ys+1][xs]>map[ys+1][xs]+dist_map[ys][xs] && visited_map[ys+1][xs] != 1 )
            {
                dist_map[ys+1][xs] = map[ys+1][xs]+dist_map[ys][xs];
                origin_map[ys+1][xs] = indices_to_index(ys,xs,dim);
            }  
        }

        //check x y-1
        if (ys > 0)
        {
            if (dist_map[ys-1][xs]>map[ys-1][xs]+dist_map[ys][xs] && visited_map[ys-1][xs] != 1 )
            {
                dist_map[ys-1][xs] = map[ys-1][xs]+dist_map[ys][xs];
                origin_map[ys-1][xs] = indices_to_index(ys,xs,dim);
            }  
        }
        
        visited_map[ys][xs] = 1;
        update_distmap(dist_map,visited_map,dim);
        
        int *next_coords = index_to_indices(find_argmin(dist_map,dim),dim);
        xs = next_coords[1];
        ys = next_coords[0];
        free(next_coords);
        
        if (xs == xe && ys == ye)
        {
            finished = 1;
        }
        count ++;
        
    }

    
    while (xs != x || ys != y)
    {
        int *next_coords = index_to_indices(origin_map[ys][xs],dim);
        xs = next_coords[1];
        ys = next_coords[0];
        free(next_coords);
        cpt++;
        path = realloc(path, cpt * sizeof(int *));
        path[cpt-1] = malloc(2 * sizeof(int));
        path[cpt-1][0] = xs;
        path[cpt-1][1] = ys;
    }
    

    for(int i = 0; i < cpt / 2; i++)
{
    int temp[2];
    temp[0] = path[i][0];
    temp[1] = path[i][1];
    path[i][0] = path[cpt - i - 1][0];
    path[i][1] = path[cpt - i - 1][1];
    path[cpt - i - 1][0] = temp[0];
    path[cpt - i - 1][1] = temp[1];
    path[cpt] = NULL;
}

    free(dist_map);
    free(origin_map);
    free(visited_map);
    return path;
}

//----------------- stocker la map dans un tableau depuis un fichier -------------
int *taille(char *fichier){
    FILE* file=NULL;
    file=fopen(fichier,"r");
    int c=0;
    int i=0;
    int j=1;
    int *tab = (int *) malloc(2*sizeof(int));
    while (c != EOF ){
        c=fgetc(file);
        if (c == '\n'){
            break;
        }
            i++;
    }
    while (c != EOF){
        c=fgetc(file);
        if (c == '\n'){
            j++;
        }
    }
    *tab=(i+1)/2;
    *(tab+1)=j;
    return tab;
}

void perser(char *fichier,int ** tableau){
    FILE *fic = fopen(fichier,"r");
    int ncolones = *(taille(fichier));
    int c;
    int i=0; 
    int j=0;

    while (c != EOF) {
        c = fgetc(fic);
        if (c >= '0' && c <= '9') {
            tableau[i][j] = c-'0';
            j++;
            if (j == ncolones) {
                i++;
                j = 0;
            }
        }
    }
}
// -------------- fonctions pour dessiner la map ------------------
void draw_tile(SDL_Renderer* renderer, SDL_Texture** textures, int x, int y, int tile_type)
{
    SDL_Rect tile_rect = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};

    SDL_Texture* texture = textures[tile_type];

    SDL_RenderCopy(renderer, texture, NULL, &tile_rect);
}

SDL_Texture* load_texture(SDL_Renderer* renderer, const char* filename)
{
    SDL_Surface* surface = IMG_Load(filename);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}
void draw_fire(SDL_Renderer* renderer, SDL_Texture* fire_texture, int x, int y) {
    SDL_Rect fire_rect = {x, y, TILE_SIZE, TILE_SIZE};
    SDL_RenderCopy(renderer, fire_texture, NULL, &fire_rect);
}




void draw_map(SDL_Renderer* renderer, SDL_Texture** textures,int** map, int map_width, int map_height)
{   
    int map_pixel_width = map_width * TILE_SIZE + PADDING*2;
    int map_pixel_height = map_height * TILE_SIZE + PADDING*2;
    SDL_Rect background_rect = {0, 0, map_pixel_width, map_pixel_width};
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 200); // green background
    SDL_RenderFillRect(renderer, &background_rect);
    for (int y = 0; y < map_height; y++) {
        for (int x = 0; x < map_width; x++) {
            int tile_type = map[y][x];
            draw_tile(renderer, textures, x, y, tile_type); // Draw the regular tile texture
            
        }

    }
}
// ------------- fonction qui retourne une carte de meme dimension que la map, mais avec le vitesse du robot sur chaque terrain ----------------------
int ** carte_to_speed(char * fichier,int ** carte,Robot* robot){
    int nlignes = *(taille(fichier)+1);
    int ncolones = *(taille(fichier));
    int **speed = (int**)malloc(sizeof(int*)*nlignes);
    for(int k = 0; k < nlignes; k++){
        speed[k] = (int *)malloc(sizeof(int)*ncolones);
    }
    for(int i = 0; i < nlignes; i++){
        for(int j = 0; j < ncolones; j++){
            if(carte[i][j] == 0)
                speed[i][j] = 99;
            if(carte[i][j] == 1)
                speed[i][j] = *robot->vitesse;
            if(carte[i][j] == 2)
                speed[i][j] = *(robot->vitesse+1);
            if(carte[i][j] == 3)
                speed[i][j] = *(robot->vitesse+2);
        }
    }
    return speed;

}
//------------------------------une simulation pour éteindre le feu, remplacer la case du feu par la case de la carte sans feu ---------------------

void eteindre_feu(SDL_Renderer* renderer, SDL_Texture** textures,int** map, int map_width, int map_height, Robot* robot,Feu* feu){
    int tile_type = map[feu->y][feu->x];
    SDL_Delay(500);
    printf("le feu est éteint\n");
    draw_tile(renderer, textures, feu->x, feu->y, tile_type);
    SDL_RenderPresent(renderer);
    SDL_Delay(10);

}
//------------------------fonction pour deplacer le robot d'une case dest_x, dest_x ----------------------------
void move_robot(SDL_Renderer* renderer, SDL_Texture** textures,int** map, int map_width, int map_height, Robot* robot, int dest_x, int dest_y, Feu* feu) {
    // Calculate the distance and travel time

    // Move the robot to the destination
    for (int i = 0; i <= 1; i++) {
        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 200);
        SDL_RenderClear(renderer);

        // Draw the map
        for (int y = 0; y < map_height; y++) {
            for (int x = 0; x < map_width; x++) {
                draw_tile(renderer,textures, x, y, map[y][x]);
            }
        }

        // Draw the fire at its current position if it exists
        SDL_Texture* fire_texture = textures[4]; // Fire texture is the 5th texture in the array
        if (feu->taille>0) {
            draw_fire(renderer, fire_texture, feu->x * TILE_SIZE, feu->y * TILE_SIZE);
        }
        
        // Draw the robot at its new position
        int robot_x = robot->x + (dest_x - robot->x) * i;
        int robot_y = robot->y + (dest_y - robot->y) * i;
        SDL_Rect robot_rect = {robot_x * TILE_SIZE, robot_y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
        SDL_Surface* robot_surface = IMG_Load("chatbot.png");
        SDL_Texture* robot_texture = SDL_CreateTextureFromSurface(renderer, robot_surface);
        SDL_RenderCopy(renderer, robot_texture, NULL, &robot_rect);

        // Render the screen
        SDL_RenderPresent(renderer);

        // Delay for a short time to control the frame rate
        SDL_Delay(10);
    }

    // Update the robot's position
    robot->x = dest_x;
    robot->y = dest_y;
}

//--------------------deplacement du robot selon le chemin le plus court-----------------------------------

void dep_robot(Robot* myrobot, int** path, int** carte, SDL_Renderer* renderer, SDL_Texture** textures, int map_width, int map_height, Feu* feu) {
    int i = 0;
    while (path[i] != NULL) {
        int x_new = path[i][0];
        int y_new = path[i][1];
        int case_courante = carte[x_new][y_new];
        if (case_courante == 0) {
            printf("Le robot ne se déplace pas dans l'eau.\n");
        } else {
            int vitesse = myrobot->vitesse[case_courante - 1];
            int wait_time = (int) ceil(1 / vitesse);
            move_robot(renderer, textures, carte, map_width, map_height, myrobot, x_new, y_new, feu);
            myrobot->x = x_new;
            myrobot->y = y_new;
            printf("Le robot se déplace en (%d, %d) à une vitesse de %d case/s.\n", x_new, y_new, vitesse);
            SDL_Delay(wait_time * 1000);
        }
        i++;
    }

}



void remplissage(Robot *robot){
    while(robot->contenu<robot->capacite_max){
        printf("remplissage d'eau ... la capacité du robot : %d\n",robot->contenu);
        robot->contenu +=1;
        int wait_time = (int)ceil(1/robot->debit);
        SDL_Delay(wait_time*1000);
        }

}
double distance(int x1, int y1, int x2, int y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}
// ---------------- fonction retourne la case d'eau la plus proche d'une source(x,y) ------------------------
int *find_nearest_zero(int **carte, int x, int y, int nlignes, int ncolones) {
    int i, j;
    int* nearestZero = NULL;
    double nearestDistance = -1;

    // Iterate over each element in the map
    for (i = 0; i < nlignes; i++) {
        for (j = 0; j < ncolones; j++) {
            // If the current element is a 0, calculate its distance from the starting point
            if (carte[i][j] == 0) {
                double d = distance(x, y, j, i);
                // If this is the first 0 found, or if it is closer than the previous closest 0, update nearestZero
                if (nearestZero == NULL || d < nearestDistance) {
                    nearestZero = malloc(2 * sizeof(int));
                    nearestZero[0] = j;
                    nearestZero[1] = i;
                    nearestDistance = d;
                }
            }
        }
    }
    return nearestZero;
}
//______________________boulot_du_robot_______________
void remplir(SDL_Renderer* renderer, SDL_Texture** textures,Feu *feu, Robot *myrobot, int ** map, char * fichier){
    int nlignes = *(taille(fichier)+1);
    int ncolones = *(taille(fichier));
    if((myrobot->contenu != myrobot->capacite_max) && ((map[myrobot->x+1][myrobot->y] == 0) || (map[myrobot->x-1][myrobot->y] == 0) || (map[myrobot->x][myrobot->y-1] == 0) || (map[myrobot->x][myrobot->y+1] == 0))){
        printf("le robot est dans une case adjacente de l'eau\n");
        remplissage(myrobot);
        
    }
    else{
        int* start = find_nearest_zero(map,myrobot->y,myrobot->x,nlignes,ncolones);
        int x = start[0];
        int y = start[1];
        int **speed = carte_to_speed(fichier,map,myrobot);
        int **path = dijkstra(&nlignes,speed,myrobot->x,myrobot->y,x-1,y+1);
        printf("Le robot n'est pas dans une case adjacente de l'eau\n");
        printf("deplacement du robot\n");
        printf("la case d'eau la plus proche (%d,%d)\n",x,y);
        dep_robot(myrobot,path,map,renderer,textures,ncolones,nlignes,feu);
        remplissage(myrobot);
        free(path);
        

    }

}
void verser(SDL_Renderer* renderer, SDL_Texture** textures,Feu *feu, Robot *robot, char * fichier, int ** map){
    int nlignes = *(taille(fichier)+1);
    int ncolones = *(taille(fichier));
    int **speed = carte_to_speed(fichier,map,robot);
    if ((robot->contenu>=feu->taille) && ((robot->x == feu->x) && (robot->y == feu->y)))
    {
        printf("Le robot est sur une case avec un feu.\n");
        printf("Le robot possède bien l’eau nécessaire\n");
        vidage(robot,feu);
        eteindre_feu(renderer,textures,map,ncolones,nlignes,robot,feu);
    }
    else if ((robot->contenu>=feu->taille) && ((robot->x != feu->x) || (robot->y != feu->y))){
        printf("Le robot n'est pas sur une case avec un feu.\nDéplacement du robot ...\n");
        int **path = dijkstra(&nlignes,speed,robot->x,robot->y,feu->x,feu->y);
        dep_robot(robot,path,map,renderer,textures,ncolones,nlignes,feu);
        move_robot(renderer, textures, map, ncolones, nlignes, robot,feu->x,feu->y,feu);
        printf("Le robot possède bien l’eau nécessaire :)\n");
        vidage(robot,feu);
        eteindre_feu(renderer,textures,map,ncolones,nlignes,robot,feu);

    }
    else if (robot->contenu<feu->taille)
    {
        printf("Le robot ne possède pas l’eau nécessaire :(\nLe robot va remplir de l'eau ...\n");

        remplir(renderer,textures,feu,robot,map,fichier);
        printf("le robot va aller eteindre le feu\n");
        int **path = dijkstra(&nlignes,speed,robot->x,robot->y,feu->x,feu->y);
        dep_robot(robot,path,map,renderer,textures,ncolones,nlignes,feu);
        move_robot(renderer, textures, map, ncolones, nlignes, robot,feu->x,feu->y,feu);
        vidage(robot,feu);
        eteindre_feu(renderer,textures,map,ncolones,nlignes,robot,feu);

    }
}
//_________________________________________


int main(int argc, char* argv[])
{   
    // quelque paramètres essentiels ------------------------
    int abscisse, ordonee, capacite, contenu, debit, v_foret, v_montagne, v_ville, taille_max;
    int nlignes = *(taille(argv[1])+1);
    int ncolones = *(taille(argv[1]));
    int map_pixel_width = ncolones * TILE_SIZE + PADDING;
    int map_pixel_height = nlignes * TILE_SIZE + PADDING;
    // ---------------------mallocage pour la carte --------------------
    
    int **map = (int **)malloc(sizeof(int*)*nlignes);
    for(int k = 0; k < nlignes; k++){
        map[k] = (int *)malloc(sizeof(int)*ncolones);
    }


// -----------parser-----------------------
    perser(argv[1],map);
// --------------initiation robot et taille max feu -----------------
    SDL_Init(SDL_INIT_VIDEO);
	printf("Enter the x-coordinate (less than %d) where you want to place your robot, considering the map dimensions:\n", ncolones - 1);
	scanf("%d", &abscisse);
	printf("Enter the y-coordinate (less than %d) where you want to place your robot:\n", nlignes - 1);
	scanf("%d", &ordonee);
	while (map[abscisse][ordonee] == 0) {
	    printf("This is a water tile. Please choose different coordinates.\n");
	    printf("Enter the x-coordinate (less than %d) where you want to place your robot, considering the map dimensions:\n", ncolones - 1);
	    scanf("%d", &abscisse);
	    printf("Enter the y-coordinate (less than %d) where you want to place your robot:\n", nlignes - 1);
	    scanf("%d", &ordonee);
	}
	printf("Enter the maximum capacity of the robot:\n");
	scanf("%d", &capacite);
	printf("Enter the current water content of the robot:\n");
	scanf("%d", &contenu);
	printf("Enter the water filling/draining rate of the robot:\n");
	scanf("%d", &debit);
	printf("Enter the speed of the robot (cells/second) in the forest:\n");
	scanf("%d", &v_foret);
	printf("Enter the speed of the robot (cells/second) in the mountains:\n");
	scanf("%d", &v_montagne);
	printf("Enter the speed of the robot (cells/second) in the city:\n");
	scanf("%d", &v_ville);
	printf("Enter the maximum size the fire can have, considering the robot's maximum capacity:\n");
	scanf("%d", &taille_max);

    //---------------creer window et renderer-------------------
    SDL_Window* window = SDL_CreateWindow("Map", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, map_pixel_width, map_pixel_height, 0);
    if (window == NULL) {
        fprintf(stderr, "Error creating window: %s\n", SDL_GetError());
        exit(1);
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        fprintf(stderr, "Error creating renderer: %s\n", SDL_GetError());
        exit(1);
    }


    Feu* feu = generate_fire(map,nlignes,ncolones,taille_max);
    Robot * robot = initialise(capacite,abscisse,ordonee,contenu,v_montagne,v_ville,v_foret,debit);
//------------------------- importation des textures ----------------
    SDL_Texture* textures[5];
    textures[0] = load_texture(renderer, "mer.png");
    textures[1] = load_texture(renderer, "montagne.png");
    textures[2] = load_texture(renderer, "foret.png");
    textures[3] = load_texture(renderer, "city.png");
    textures[4] = load_texture(renderer, "fire.gif");
// ----------------------affichage de la carte ---------------
    SDL_RenderClear(renderer);
    draw_map(renderer,textures, map, ncolones, nlignes);

// ------------- actualisation de l'ecran-------------------
    SDL_RenderPresent(renderer);
//---------------boulot -----------------
    verser(renderer,textures,feu,robot,argv[1],map);
    
    
//-------------------Main-loop----------------
    SDL_Event event;
    int quit = 0;
    while (!quit) {
        // Gérer les événements
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = 1;
            }
        }
        
        SDL_RenderPresent(renderer);
        SDL_Delay(10);
        
        }
// ------------------Clean--------------------up
    SDL_RenderPresent(renderer);
    for (int y = 0; y < nlignes; y++) {
        free(map[y]);
    }
    free(map);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;

}
