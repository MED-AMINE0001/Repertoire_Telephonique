#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_ENTRIES 1000
#define NAME_LEN 50
#define PHONE_LEN 25
#define ADDR_LEN 100
#define DOB_LEN 20
#define EMAIL_LEN 50

typedef struct {
    char nom[NAME_LEN];
    char telephone[PHONE_LEN];
    char adresse[ADDR_LEN];
    char naissance[DOB_LEN];
    char email[EMAIL_LEN];
    short dept;
} Entry;

/* prototypes */
int ajouter(Entry ann[], int *n, Entry e);
int contient(Entry ann[], int n, char nom[], short dept); /* retourne indice ou -1 */
int annuaireInvers(Entry ann[], int n, char telephone[], char nom[], short *dept); /* retourne indice ou -1 */
int enlever_par_nom_dept(Entry ann[], int *n, char nom[], short dept);
int enlever_par_numero(Entry ann[], int *n, char numero[]);
void trier(Entry ann[], int n);
int sauvegarder(Entry ann[], int n, const char *filename);
int charger(Entry ann[], int *n, const char *filename);
void lister(Entry ann[], int n);

/* utilitaires */
static void trim_newline(char *s) {
    size_t l = strlen(s);
    if (l && s[l-1] == '\n') s[l-1] = '\0';
}

int ajouter(Entry ann[], int *n, Entry e) {
    if (*n >= MAX_ENTRIES) return 0;
    /* éviter doublon de numéro */
    for (int i = 0; i < *n; ++i) {
        if (strcmp(ann[i].telephone, e.telephone) == 0) return 0;
    }
    ann[*n] = e;
    (*n)++;
    return 1;
}

int contient(Entry ann[], int n, char nom[], short dept) {
    for (int i = 0; i < n; ++i) {
        if (strcmp(ann[i].nom, nom) == 0 && ann[i].dept == dept) return i;
    }
    return -1;
}

int annuaireInvers(Entry ann[], int n, char telephone[], char nom[], short *dept) {
    for (int i = 0; i < n; ++i) {
        if (strcmp(ann[i].telephone, telephone) == 0) {
            strcpy(nom, ann[i].nom);
            *dept = ann[i].dept;
            return i;
        }
    }
    return -1;
}

int enlever_par_nom_dept(Entry ann[], int *n, char nom[], short dept) {
    int idx = contient(ann, *n, nom, dept);
    if (idx == -1) return 0;
    /* déplacer derniers éléments vers l'avant */
    for (int i = idx; i < (*n)-1; ++i) ann[i] = ann[i+1];
    (*n)--;
    return 1;
}

int enlever_par_numero(Entry ann[], int *n, char numero[]) {
    int idx = -1;
    for (int i = 0; i < *n; ++i) {
        if (strcmp(ann[i].telephone, numero) == 0) { idx = i; break; }
    }
    if (idx == -1) return 0;
    for (int i = idx; i < (*n)-1; ++i) ann[i] = ann[i+1];
    (*n)--;
    return 1;
}

int cmp_entry(const void *a, const void *b) {
    const Entry *ea = a;
    const Entry *eb = b;
    int r = strcmp(ea->nom, eb->nom);
    if (r != 0) return r;
    if (ea->dept != eb->dept) return (ea->dept - eb->dept);
    return strcmp(ea->telephone, eb->telephone);
}

void trier(Entry ann[], int n) {
    qsort(ann, n, sizeof(Entry), cmp_entry);
}

int sauvegarder(Entry ann[], int n, const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) return 0;
    fprintf(f, "%d\n", n);
    for (int i = 0; i < n; ++i) {
        /* format simple séparé par '|' */
        fprintf(f, "%s|%s|%s|%s|%s|%d\n",
            ann[i].nom, ann[i].telephone, ann[i].adresse,
            ann[i].naissance, ann[i].email, ann[i].dept);
    }
    fclose(f);
    return 1;
}

int charger(Entry ann[], int *n, const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return 0;
    int count = 0;
    if (fscanf(f, "%d\n", &count) != 1) { fclose(f); return 0; }
    if (count < 0 || count > MAX_ENTRIES) { fclose(f); return 0; }
    char line[512];
    int i = 0;
    while (i < count && fgets(line, sizeof(line), f)) {
        trim_newline(line);
        /* découper par '|' */
        char *p = strtok(line, "|");
        if (!p) break;
        strncpy(ann[i].nom, p, NAME_LEN); ann[i].nom[NAME_LEN-1] = '\0';

        p = strtok(NULL, "|"); if (!p) break;
        strncpy(ann[i].telephone, p, PHONE_LEN); ann[i].telephone[PHONE_LEN-1] = '\0';

        p = strtok(NULL, "|"); if (!p) break;
        strncpy(ann[i].adresse, p, ADDR_LEN); ann[i].adresse[ADDR_LEN-1] = '\0';

        p = strtok(NULL, "|"); if (!p) break;
        strncpy(ann[i].naissance, p, DOB_LEN); ann[i].naissance[DOB_LEN-1] = '\0';

        p = strtok(NULL, "|"); if (!p) break;
        strncpy(ann[i].email, p, EMAIL_LEN); ann[i].email[EMAIL_LEN-1] = '\0';

        p = strtok(NULL, "|"); if (!p) break;
        ann[i].dept = (short)atoi(p);

        i++;
    }
    *n = i;
    fclose(f);
    return 1;
}

void lister(Entry ann[], int n) {
    if (n == 0) { printf("Annuaire vide.\n"); return; }
    for (int i = 0; i < n; ++i) {
        printf("=== #%d ===\n", i);
        printf("Nom: %s\n", ann[i].nom);
        printf("Tel: %s\n", ann[i].telephone);
        printf("Dept: %d\n", ann[i].dept);
        printf("Adresse: %s\n", ann[i].adresse);
        printf("Naissance: %s\n", ann[i].naissance);
        printf("Email: %s\n", ann[i].email);
    }
}

int main(void) {
    Entry ann[MAX_ENTRIES];
    int n = 0;
    char buffer[256];
    const char *filename = "annuaire.txt";

    while (1) {
        printf("\n--- Menu Annuaire ---\n");
        printf("1) Ajouter\n2) Chercher par nom+dept (contient)\n3) Recherche inverse (par numero)\n4) Enlever par nom+dept\n5) Enlever par numero\n6) Trier\n7) Lister tous\n8) Sauvegarder\n9) Charger\n0) Quitter\nChoix: ");
        if (!fgets(buffer, sizeof(buffer), stdin)) break;
        int choix = atoi(buffer);

        if (choix == 1) {
            Entry e;
            printf("Nom: "); fgets(e.nom, NAME_LEN, stdin); trim_newline(e.nom);
            printf("Telephone: "); fgets(e.telephone, PHONE_LEN, stdin); trim_newline(e.telephone);
            printf("Adresse: "); fgets(e.adresse, ADDR_LEN, stdin); trim_newline(e.adresse);
            printf("Date de naissance: "); fgets(e.naissance, DOB_LEN, stdin); trim_newline(e.naissance);
            printf("Email: "); fgets(e.email, EMAIL_LEN, stdin); trim_newline(e.email);
            printf("Dept (ex: 10): "); fgets(buffer, sizeof(buffer), stdin); e.dept = (short)atoi(buffer);
            if (ajouter(ann, &n, e)) printf("Ajout reussi.\n"); else printf("Echec ajout (plein ou numero existe).\n");
        } else if (choix == 2) {
            char nom[NAME_LEN];
            printf("Nom a chercher: "); fgets(nom, NAME_LEN, stdin); trim_newline(nom);
            printf("Dept: "); fgets(buffer, sizeof(buffer), stdin); short dept = (short)atoi(buffer);
            int idx = contient(ann, n, nom, dept);
            if (idx >= 0) {
                printf("Trouve a l'indice %d: Tel=%s\n", idx, ann[idx].telephone);
            } else printf("Non present.\n");
        } else if (choix == 3) {
            char tel[PHONE_LEN], nom_out[NAME_LEN]; short dept_out;
            printf("Telephone: "); fgets(tel, PHONE_LEN, stdin); trim_newline(tel);
            int idx = annuaireInvers(ann, n, tel, nom_out, &dept_out);
            if (idx >= 0) {
                printf("Trouve: Nom=%s Dept=%d\n", nom_out, dept_out);
            } else printf("Non trouve.\n");
        } else if (choix == 4) {
            char nom[NAME_LEN];
            printf("Nom: "); fgets(nom, NAME_LEN, stdin); trim_newline(nom);
            printf("Dept: "); fgets(buffer, sizeof(buffer), stdin); short dept = (short)atoi(buffer);
            if (enlever_par_nom_dept(ann, &n, nom, dept)) printf("Enleve.\n"); else printf("Non trouve.\n");
        } else if (choix == 5) {
            char num[PHONE_LEN];
            printf("Numero: "); fgets(num, PHONE_LEN, stdin); trim_newline(num);
            if (enlever_par_numero(ann, &n, num)) printf("Enleve.\n"); else printf("Non trouve.\n");
        } else if (choix == 6) {
            trier(ann, n); printf("Trie effectue.\n");
        } else if (choix == 7) {
            lister(ann, n);
        } else if (choix == 8) {
            if (sauvegarder(ann, n, filename)) printf("Sauvegarde ok (%s).\n", filename);
            else printf("Echec sauvegarde.\n");
        } else if (choix == 9) {
            if (charger(ann, &n, filename)) printf("Chargement ok (%s), %d entrees.\n", filename, n);
            else printf("Echec chargement.\n");
        } else if (choix == 0) {
            printf("Quitter.\n"); break;
        } else {
            printf("Choix invalide.\n");
        }
    }

    return 0;
}