#include <stdio.h>
#include <stdlib.h>


#include "hashtable.h"

#define CHUNCK_SIZE 20



typedef struct Bucket Bucket;

struct Bucket {
    unsigned int count;
    char **values;
};

struct  Hashtable{
    unsigned int count;
    Bucket *buckets;
};

//functia de calcul a hashului
unsigned int hash(const char *str, unsigned int hash_length)
{
	unsigned int hash = 5381;
	int c;

	while ( (c = *str++) != 0 )
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

	return (hash % hash_length);
}

//alocarea de memorie pentru un nou hashtable de o anumita capacitate
Hashtable* hashtable_new(unsigned int capacity)
{
    Hashtable *ht;

    ht = malloc(sizeof(Hashtable));
    if (ht == NULL) {
            return NULL;
    }
    ht->count = capacity;
    ht->buckets = malloc(ht->count * sizeof(Bucket));
    if (ht->buckets == NULL) {
            free(ht);
            return NULL;
    }
    memset(ht->buckets, 0, ht->count * sizeof(Bucket));
    return ht;
}

//stergerea intregului hashtable
void ht_delete(Hashtable *ht)
{
    unsigned int i, j, n, m;
    Bucket *bucket;
    char **value;
    if (ht == NULL) {
            return;
    }
    n = ht->count;
    bucket = ht->buckets;
    i = 0;
    while (i < n) {
            m = bucket->count;
            value = bucket->values;
            j = 0;
            while(j < m) {
                    free(value[j]);
                    j++;
            }
            free(bucket->values);
            bucket++;
            i++;
    }
    free(ht->buckets);
    free(ht);
}

//verificarea existentei unui anumit element in hashtable
int ht_get(Hashtable *ht, char *value)
{
    unsigned int index, i;
    Bucket *bucket;

    if (ht == NULL || ht->buckets == NULL) {
            return 0;
    }
    if (value == NULL) {
            return 0;
    }
    index = hash(value,ht->count);
    bucket = &(ht->buckets[index]);
    if (bucket->count == 0) return 0;
    for(i = 0; i < bucket->count; i++)
        if (strcmp(value, bucket->values[i]) == 0) return 1;
    return 0;
}

//adaugarea unui anumit element in hashtable
int ht_put(Hashtable *ht, char *value)
{
    int value_len;
    int index;
    Bucket *bucket;
    char **tmp_values;

    if (ht == NULL) {
            return 0;
    }
    if (value == NULL) {
            return 0;
    }
    value_len = strlen(value);
    index = hash(value,ht->count);
    bucket = &(ht->buckets[index]);
    //printf("primul %i\n", value_len);
   
    if (bucket->count == 0) {
        bucket->values = (char**)malloc(sizeof(char*));
        bucket->values[0] = (char*)malloc(value_len + 1);
        if (bucket->values == NULL) {
                return 0;
        }
        bucket->count = 1;
    }
    else {
        tmp_values = (char**)realloc(bucket->values, (bucket->count + 1) * sizeof(char*));
        tmp_values[bucket->count] = (char *)malloc(value_len + 1);
        if (tmp_values == NULL) {
                return 0;
        }
        bucket->values = tmp_values;
        bucket->count = bucket->count + 1;
    }
    //printf("al doilea %i\n", value_len);
    //if(strlen(value) != value_len) 
    memcpy(bucket->values[bucket->count - 1], value, value_len + 1);
    return 1;
}

//stergerea unui anumit element din hashtable
int ht_remove(Hashtable *ht, char *value){
    unsigned int index, i, j, return_value=0;
    Bucket *bucket;

    if (ht == NULL || ht->buckets == NULL) {
            return 0;
    }
    if (value == NULL) {
            return 0;
    }
    index = hash(value,ht->count);
    bucket = &(ht->buckets[index]);
    if (bucket->count == 0) return 0;
    for(i = 0; i < bucket->count; i++)
        if (strcmp(value, bucket->values[i]) == 0) {
            return_value = 1;
            for (j = i+1; j < bucket->count; j++){
                bucket->values[j-1] = (char*) realloc(bucket->values[j-1], strlen(bucket->values[j])+1);
                strcpy(bucket->values[j-1], bucket->values[j]);
            }
            break;
        }
    free (bucket->values[j-1]);
    //bucket->values[j-1] = NULL;
    bucket->values = (char**)realloc(bucket->values, (bucket->count - 1) * sizeof(char*));
    bucket->count--;
    return return_value;
}

//afisarea unui anumit bucket
void ht_print_bucket(Hashtable *ht, unsigned int index, FILE* stream){
    Bucket b = ht->buckets[index];
    int i;

    if (ht != NULL){
        for (i = 0; i < b.count; i++){
            fwrite(b.values[i], 1, strlen(b.values[i]), stream);
            if (i != b.count-1) fprintf(stream, " ");
        }
        if (b.count != 0) fprintf(stream, "\n");
    }
}

//afisarea continului intregului hashtable
void ht_print(Hashtable *ht, FILE* stream){
    int i, j;
    if (ht != NULL){
        for (i = 0; i < ht->count; i++)
            if (ht->buckets[i].count)
                ht_print_bucket(ht, i, stream);
    }
}

//modificarea dimensiunii hashtabelului
Hashtable* ht_resize(Hashtable *ht, unsigned int type){
    unsigned int i, j;
    Hashtable *new_ht = NULL;
    if (type == 1)
         new_ht = hashtable_new(ht->count/2);
    if (type == 2)
         new_ht = hashtable_new(ht->count*2);

    for (i = 0; i < ht->count; i++)
        if (ht->buckets[i].count)
            for (j = 0; j < ht->buckets[i].count; j++){
                ht_put(new_ht, ht->buckets[i].values[j]);
            }
    return new_ht;
}

int main(int argc, char** argv) {
    int i, result, nr_files;
    Hashtable *ht;
    char input[CHUNCK_SIZE];
    char byte;
    char buff[6];
    FILE *stream, *out, **files;
    char *word, *filename;
    int cont, filecont;

    ht = hashtable_new(atoi(argv[1]));
    if (ht == NULL){
        printf("Alocare esuata!");
    }
    
    //afisarea implicita la stdout
    out = stdout;
    if (argc < 3) {
        nr_files = 1;
	//alocare vector fisiere de intrare
        files = (FILE**) malloc(sizeof(FILE*));
        DIE (files == NULL, "Alocare esuata.");
        files[0] = stdin;
    }
    else {
        if (strcmp(argv[2],"<")==0){
            files = (FILE**) malloc((argc-3)*sizeof(FILE*));
            DIE (files == NULL, "Alocare esuata.");
            nr_files = argc - 3;
            for (i = 0; i < nr_files; i++){
                files[i] = fopen(argv[i+3],"r");
                DIE (files[i] == NULL, "Eroare deschidere fisier.");
            }
        }
        else{
            nr_files = argc - 2;
            files = (FILE**) malloc((argc-2)*sizeof(FILE*));
            DIE (files == NULL, "Alocare esuata.");
            for (i = 0; i < nr_files; i++){
                files[i] = fopen(argv[i+2],"r");
                DIE (files[i] == NULL, "Eroare deschidere fisier.");
            }
        }

    }
    for (i = 0; i < nr_files; i++){
        stream = files[i];

        while(fscanf(stream, "%s", &input)!=EOF){
            if (strcmp(input, "add") == 0) {
                fgetc(stream);
                word = (char*)malloc(1*sizeof(char));
                cont = 0;
                while ((fread(&byte,1,1,stream))) { //citire cuvant
                    if (byte=='\n') break;
                    if (byte >= 'A' && byte <= 'Z' || byte >= 'a' && byte <= 'z'){		
			word = (char*)realloc(word, (cont+1)*sizeof(char));
                        word[cont] = byte;
                        cont++;
                    }
                }
		word = (char*)realloc(word, (cont+1)*sizeof(char));
                word[cont] = '\0';
                if (ht_get(ht, word)==0) ht_put(ht, word);
		free(word);
            }
            else if(strcmp(input, "remove") == 0) {
                fgetc(stream);
                word = (char*)malloc(sizeof(char));
                cont = 0;
                while ((fread(&byte,1,1,stream))) {
                    if (byte=='\n') break;
                    if (byte >= 'A' && byte <= 'Z' || byte >= 'a' && byte <= 'z'){
                        word[cont]=byte;
                        word = (char*)realloc(word, (cont+1)*sizeof(char));
                        cont++;
                    }
                }
                word[cont] = '\0';
                ht_remove(ht, word);
            }
            else if(strcmp(input, "find") == 0) {
                fgetc(stream);
                word = (char*)malloc(sizeof(char));
                filename = (char*)malloc(sizeof(char));
                cont = 0;
				filecont = 0;
                while ((fread(&byte,1,1,stream))) {
                    if (byte == '\n') break;
                    if (byte == ' '){
                        while ((fread(&byte,1,1,stream))){ //citire nume fisier
                            if (byte == '\n') break;
                            filename[filecont] = byte;
                            filename = (char*)realloc(filename, (filecont+1)*sizeof(char));
                            filecont++;
                        }
                        break;
                    }
                    if (byte >= 'A' && byte <= 'Z' || byte >= 'a' && byte <= 'z'){
                        word[cont]=byte;
                        word = (char*)realloc(word, (cont+1)*sizeof(char));
                        cont++;
                    }
                }
                filename[filecont]='\0';
                if (filecont != 0) out = fopen(filename,"a");
                word[cont] = '\0';
                if (ht_get(ht, word))
                    sprintf(buff, "True\n");
                else sprintf(buff, "False\n");
                fwrite(buff, 1, sizeof(buff), out);
                if (filecont != 0) fclose(out);
            }
            else if(strcmp(input, "clear") == 0) {
                ht_delete(ht);
                ht = hashtable_new(atoi(argv[1]));
            }
            else if(strcmp(input, "print_bucket") == 0) {
                fgetc(stream);
                word = (char*)malloc(sizeof(char));
                filename = (char*)malloc(sizeof(char));
                cont = 0;
				filecont = 0;
                while ((fread(&byte,1,1,stream))) {
                    if (byte=='\n') break;
                    if (byte == ' '){
                        while ((fread(&byte,1,1,stream))){
                            if (byte == '\n') break;
                            filename[filecont] = byte;
                            filename = (char*)realloc(filename, (filecont+1)*sizeof(char));
                            filecont++;
                        }
                        break;
                    }
                    word[cont]=byte;
                    word = (char*)realloc(word, (cont+1)*sizeof(char));
                    cont++;
                    
                }
                filename[filecont]='\0';
                if (filecont != 0) out = fopen(filename,"a");
                word[cont] = '\0';
                ht_print_bucket(ht, atoi(word), out);
                if (filecont != 0) fclose(out);
            }
            else if(strcmp(input, "print") == 0) {
                filename = (char*)malloc(sizeof(char));
                filecont = 0;
                while ((fread(&byte,1,1,stream))) {
                    if (byte=='\n') break;
                    if (byte == ' '){
                        while ((fread(&byte,1,1,stream))){
                            if (byte == '\n') break;
                            filename[filecont] = byte;
                            filename = (char*)realloc(filename, (filecont+1)*sizeof(char));
                            filecont++;
                        }
                        break;
                    }
                }
                filename[filecont]='\0';
                if (filecont != 0) out = fopen(filename,"a");
                ht_print(ht, out);
                if (filecont != 0) fclose(out);
            }
            else if(strcmp(input, "resize") == 0) {
                fscanf(stream, "%s", &input);
                if (strcmp(input, "double") == 0)
                    ht = ht_resize(ht, 2);
                if (strcmp(input, "halve") == 0)
                    ht = ht_resize(ht, 1);

            }
        }
    }
    
    return (EXIT_SUCCESS);
}

