#include "truc.h"
#include <stdlib.h>
#include <string.h>

Un_truc *creer_truc(Une_coord coord, Ttype type, Tdata data, double uv) {
    Un_truc *truc = (Un_truc *)malloc(sizeof(Un_truc));
    if (truc == NULL) {
        return NULL;
    }
    
    truc->coord = coord;
    truc->type = type;
    truc->data = data;
    truc->user_val = (float)uv;
    
    return truc;
}

void detruire_truc(Un_truc *truc) {
    if (truc == NULL) {
        return;
    }
    
    if (truc->type == STA) {
        // Liberer le nom de la station
        if (truc->data.sta.nom != NULL) {
            free(truc->data.sta.nom);
        }
        // Liberer le tableau de connexions
        if (truc->data.sta.tab_con != NULL) {
            free(truc->data.sta.tab_con);
        }
    }
    // Pour les connexions (CON), pas de liberation particuliere
    
    free(truc);
}
