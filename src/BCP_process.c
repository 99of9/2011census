#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a > b ? b : a)

#define NONE 0
#define PERSONS 1
#define SEX 2
#define FAMILIES 3
#define DWELLINGS 4
#define FEMALES15PLUS 5
#define HOUSEHOLDS 6
#define ENUMERATED 7
#define SEX1PLUS 8
#define SEX5PLUS 9
#define SEX15PLUSQUALIFIED 10
#define SEX15PLUS 11
#define SEX15PLUSEMPLOYED 12
#define SEX15PLUSDONESCHOOL 13
#define COUPLEFAMILIESWITHKIDS 14

#define MAXROWS 60000
#define MAXFIELD 8000
//#define BUFSIZE 10000
#define TITLELENGTH 200
#define MAXFILENAME 100
#define MAXSUBMAPS 300
#define MAXGPSLABELS 1000

#define DRYRUNTEXTONLY 0

double sample_lower_bound=400; /* minimum persons in an area before it counts as a possible max/min */


/* allocate to SEX if every field has either male, female or persons in it */
int normalize_by[66] = {
  SEX, //      0 B01
  NONE, //     1 B02
  ENUMERATED,//2 B03
  SEX, //      3 B04A
  SEX, //      4 B04B
  SEX, //      5 B05
  SEX, //      6 B06
  SEX, //      7 B07
  PERSONS, //  8 B08A
  PERSONS, //  9 B08B
  SEX,  //    10 B09
  PERSONS, // 11 B10A
  PERSONS, // 12 B10B
  PERSONS, // 13 B10C
  SEX, //     14 B11A  should be SEXBORNOVERSEAS ***********not implemented
  SEX, //     15 B11B  should be SEXBORNOVERSEAS ***********not implemented
  PERSONS, // 16 B12A
  PERSONS, // 17 B12B
  SEX, //     18 B13
  SEX, //     19 B14
  SEX, //     20 B15
  SEX15PLUSDONESCHOOL, //     21 B16A - changed from SEX
  SEX15PLUSDONESCHOOL, //     22 B16B - changed from SEX
  SEX15PLUS, //     23 B17A - changed from SEX
  SEX15PLUS, //     24 B17B - changed from SEX
  SEX, //     25 B18
  SEX15PLUS, //     26 B19 - changed from SEX
  SEX15PLUS, //     27 B20A - changed from SEX
  SEX15PLUS, //     28 B20B - changed from SEX
  SEX15PLUS, //     29 B21 - changed from SEX
  SEX15PLUS, //     30 B22A - changed from SEX
  SEX15PLUS, //     31 B22B - changed from SEX
  SEX, //     32 B23A
  SEX, //     33 B23B
  FEMALES15PLUS, // 34 B24
  FAMILIES, //35 B25
  FAMILIES, //36 B26
  COUPLEFAMILIESWITHKIDS, //37 B27 - changing from FAMILIES
  HOUSEHOLDS, //38 B28
  DWELLINGS, //39 B29
  HOUSEHOLDS, //40 B30
  DWELLINGS, //41 B31
  HOUSEHOLDS, //42 B32
  HOUSEHOLDS, //43 B33
  HOUSEHOLDS, //44 B34
  HOUSEHOLDS, //45 B35
  HOUSEHOLDS,  //46 B36 *originally the last set
  SEX, //47 B37
  SEX1PLUS, //48 B38
  SEX5PLUS, //49 B39
  SEX15PLUSQUALIFIED, //50 B40a
  SEX15PLUSQUALIFIED, //51 B40b
  SEX15PLUSQUALIFIED, //52 B41a
  SEX15PLUSQUALIFIED, //53 B41b
  SEX15PLUSQUALIFIED, //54 B41c
  SEX15PLUS, //55 B42a
  SEX15PLUS, //56 B42b
  SEX15PLUSEMPLOYED, //57 B43a
  SEX15PLUSEMPLOYED, //58 B43b
  SEX15PLUSEMPLOYED, //59 B43c
  SEX15PLUSEMPLOYED, //60 B43d
  SEX15PLUSEMPLOYED, //61 B44a
  SEX15PLUSEMPLOYED, //62 B44b
  SEX15PLUSEMPLOYED, //63 B45a
  SEX15PLUSEMPLOYED, //64 B45b
  SEX15PLUSEMPLOYED //65 B46
};

char *area_str;
int nrows;
double decimal_scaleout = 1000.0;


int             numGPSlabels;
char            GPSlabel[MAXGPSLABELS][1000];
double          GPSlabelpos[MAXGPSLABELS][2];

int             numsubmaps;
int             submapstate[MAXSUBMAPS];
double          submapbounds[MAXSUBMAPS][4];
char            submapnames[MAXSUBMAPS][100];
char            submapnames_spaces[MAXSUBMAPS][100];
double          submaplegendpos[MAXSUBMAPS][2];
double          submapdistancepos[MAXSUBMAPS][2];



int floatcomp(const void* elem1, const void* elem2)
{
    if(*(const float*)elem1 < *(const float*)elem2)
        return -1;
    return *(const float*)elem1 > *(const float*)elem2;
}


void remove_underscores(char title_spaces[], char title_underscores[]) {
  int i;
  strcpy(title_spaces, title_underscores);
  for (i=0; i<strlen(title_spaces); i++) {
    if (title_spaces[i]=='_') {
      title_spaces[i] = ' ';
    }
  }
}

int getGPSlabels() {
  FILE *GPSfp;
  int i;
  int j;
  char GPS_loc_filename[200];
  char GPS_line[1000];
  char label_str[100];
  double x, y;


  sprintf(GPS_loc_filename, "/home/hudson/general/wiki/ABS/census/GPS_labels.csv");
  GPSfp = fopen(GPS_loc_filename, "r");
  if (GPSfp==NULL) {
    printf("failed to find GPS labels file: %s\n", GPS_loc_filename);
    exit(1);
    return (0);
  }

  fgets(GPS_line, 1000, GPSfp);// get the header line which tells us how many GPS coords there will be
  sscanf(GPS_line, "%d", &numGPSlabels);
  printf("num GPS labels found %d\n", numGPSlabels);
  for (i=0; i<numGPSlabels; i++) {
    fgets(GPS_line, 1000, GPSfp);
    // printf("GPS full line: %s\n", GPS_line);
    //GPS_line[strlen(GPS_line)-1] ='\0';

    GPSlabelpos[i][0] = atof(strtok(GPS_line, ","));
    GPSlabelpos[i][1] = atof(strtok(NULL, ","));
    strcpy(GPSlabel[i], strtok(NULL, ","));
    printf("GPS line: %f %f %s\n", GPSlabelpos[i][0], GPSlabelpos[i][1], GPSlabel[i]);
    strtok(NULL, ",");

    // printf("GPS line:%f %f %s\n", x, y, label_str);
  } 

  fclose(GPSfp);
  return (numGPSlabels);
  
}

void print_label_names(FILE *norm_fp, double MaxBound[2], double MinBound[2]) {
  int i;
  for (i=0; i<numGPSlabels; i++) {
    if ((GPSlabelpos[i][0]>MinBound[0])&&(GPSlabelpos[i][0]<MaxBound[0])) {
      if ((GPSlabelpos[i][1]>MinBound[1])&&(GPSlabelpos[i][1]<MaxBound[1])) {
	fprintf(norm_fp, "<circle cx=\"%f\" cy=\"%f\" r=\"%f\" />", decimal_scaleout*(GPSlabelpos[i][0]-MinBound[0]), decimal_scaleout*(MaxBound[1]-GPSlabelpos[i][1]), 0.002*decimal_scaleout*(MaxBound[0]-MinBound[0]));
      }
    }
  }
  fprintf(norm_fp, "<g transform=\"translate(%f,%f)\" font-size=\"%f\" font-family=\"DejaVu Sans,sans-serif\" >", 0.004*decimal_scaleout*(MaxBound[0]-MinBound[0]), 0.004*decimal_scaleout*(MaxBound[0]-MinBound[0]), 0.015*decimal_scaleout*(MaxBound[0]-MinBound[0]));
  for (i=0; i<numGPSlabels; i++) {
    if ((GPSlabelpos[i][0]>MinBound[0])&&(GPSlabelpos[i][0]<MaxBound[0])) {
      if ((GPSlabelpos[i][1]>MinBound[1])&&(GPSlabelpos[i][1]<MaxBound[1])) {
	fprintf(norm_fp, "<text x=\"%f\" y=\"%f\">%s</text>\n", decimal_scaleout*(GPSlabelpos[i][0]-MinBound[0]), decimal_scaleout*(MaxBound[1]-GPSlabelpos[i][1]), GPSlabel[i]);
      }
    }
  }
  fprintf(norm_fp, "</g>");
}



int getsubmapbounds() {
  FILE *boundfp;
  int i;
  int j;
  char bound_loc_filename[200];

  sprintf(bound_loc_filename, "/home/hudson/general/wiki/ABS/census/2011_%s/bounds.txt", area_str);
  boundfp = fopen(bound_loc_filename, "r");
  if (boundfp==NULL) {
    printf("failed to find bounds file: %s\n", bound_loc_filename);
    exit(1);
    return (0);
  }

  fscanf(boundfp, "%d", &numsubmaps);
  for (i=0; i<numsubmaps; i++) {
    fscanf(boundfp, "%d %lf %lf %lf %lf %lf %lf %lf %lf", &submapstate[i], &submapbounds[i][0], &submapbounds[i][1], &submapbounds[i][2], &submapbounds[i][3], &submaplegendpos[i][0], &submaplegendpos[i][1], &submapdistancepos[i][0], &submapdistancepos[i][1]);
    fgetc(boundfp);
    fgets(submapnames[i], 100, boundfp);
    for (j=0; j<strlen(submapnames[i])-1; j++) {
      if (submapnames[i][j]==' ') submapnames[i][j]='_';
    } 
    submapnames[i][strlen(submapnames[i])-1] ='\0';

    remove_underscores(submapnames_spaces[i],submapnames[i]);
  } 

  fclose(boundfp);
  return (numsubmaps);
}


void get_region_in_submap(bool *region_in_submap[MAXSUBMAPS], int region_id[MAXROWS]) {
  char sublist_filename[MAXFILENAME];
  char check_id[100];
  char regionstr[100];
  FILE *sl_fp;
  int row;
  int s;

  for (s=0; s<numsubmaps; s++) {
    sprintf(sublist_filename, "/home/hudson/general/wiki/ABS/census/2011_%s/sublist-%s.txt", area_str, submapnames[s]);
    sl_fp = fopen(sublist_filename, "r");
    
    while (!feof(sl_fp)) {
      /* get the next region_id we need in the submap */
      fgets(check_id, 100, sl_fp);
      check_id[strlen(check_id)-1]='\0';
      
      /* find which row is a match */
      for (row = 0; row<nrows; row++) {
	if ( strstr(area_str, "POA") ) {
	  sprintf(regionstr, "%04d", region_id[row]);
	} else {
	  sprintf(regionstr, "%d", region_id[row]);
	}
	
	if (!strcmp(regionstr, check_id)) {
	  region_in_submap[s][row]=1;
	  //printf("compare: %s %d\n", check_id, region_id[row]);
	  break;
	}
      }
    }
  }
}  

int main (int argc, char *argv[]) {
  int remove_regionstring;
  int file_i;
  int f, totfields;
  int field, start_field, nfields;
  int row;
  FILE *fp, *norm_fp, *sl_fp, *head_fp;
  FILE *upload_fp;
  FILE *refresh_descriptions_fp;
  FILE *filelist_fp;
  FILE *refresh_filenames_fp;
  FILE *tallies;
  int i, s, r, col;
  char ch;
  char field_title[MAXFIELD][TITLELENGTH];
  char field_title_spaces[MAXFIELD][TITLELENGTH];
  char field_normalizestr[MAXFIELD][TITLELENGTH];
  char norm_fn[MAXFILENAME];
  char sublist_filename[MAXFILENAME];
  char command[400];
  char check_id[100];
  char regionstr[100];
  char fullareastr[100]; /* eg POA gets converted into Postcode */
  //char buff[BUFSIZ];
  int gotdata, ret;
  double normalize;
  int undef_regions;
  double MinBound[2];
  double MaxBound[2];
  float MidPoint[2];
  float long_over_lat_at_midpoint;
  float dist_km_per_degree_longitude;
  float length_km_of_width;
  int dbar_indx;
  float dbar_lengths[18]={0.01, 0.1, 1.0, 2.0, 5.0, 10.0, 20.0, 50.0, 100.0, 200.0, 500.0, 1000.0, 2000.0, 5000.0, 10000.0, 20000.0, 50000.0, 100000.0};
  float distancebar_length;


  //int table[MAXROWS][MAXFIELD];

  float** table = malloc(MAXROWS * sizeof(float*));    // allocate the rows
  for (r = 0; r < MAXROWS; ++r)
    {
      table[r] = malloc(MAXFIELD * sizeof(float));    // allocate the columns
    }


  float *tmp_tablesort =  malloc(MAXROWS * sizeof(float));;

  int* region_id = malloc(MAXROWS*sizeof(int));
  int* file_num = malloc(MAXFIELD*sizeof(int));

  bool** region_in_submap = malloc(MAXSUBMAPS*sizeof(bool*));
  for (s = 0; s < MAXSUBMAPS; ++s)
    {
      region_in_submap[s] = malloc(MAXROWS * sizeof(bool));    // allocate the columns
    }
  

  float** field_min = malloc(MAXFIELD*sizeof(float*));
  float** field_mid = malloc(MAXFIELD*sizeof(float*));
  float** field_max = malloc(MAXFIELD*sizeof(float*));
  for (field=0; field<MAXFIELD; field++) {
    field_min[field] = malloc(MAXSUBMAPS*sizeof(float));
    field_mid[field] = malloc(MAXSUBMAPS*sizeof(float));
    field_max[field] = malloc(MAXSUBMAPS*sizeof(float));
    for (s=0; s<MAXSUBMAPS; s++) {
      field_min[field][s] = 999999.9;
      field_mid[field][s] = 0.0;
      field_max[field][s] = 0.0;
    }
  }
  
  int this_colour[3];
  float** field_colours_min = malloc(MAXFIELD*sizeof(float*));
  float** field_colours_mid = malloc(MAXFIELD*sizeof(float*));
  float** field_colours_max = malloc(MAXFIELD*sizeof(float*));
  for (f=0; f<MAXFIELD; f++) {
    field_colours_min[f] = malloc(3*sizeof(float));
    field_colours_mid[f] = malloc(3*sizeof(float));
    field_colours_max[f] = malloc(3*sizeof(float));
  }

  int* total_persons = malloc(MAXROWS * sizeof(int));
  int* total_males = malloc(MAXROWS * sizeof(int));
  int* total_females = malloc(MAXROWS * sizeof(int));
  int* total_persons_enumerated = malloc(MAXROWS * sizeof(int));
  int* total_males_enumerated = malloc(MAXROWS * sizeof(int));
  int* total_females_enumerated = malloc(MAXROWS * sizeof(int));
  int* total_families = malloc(MAXROWS * sizeof(int));
  int* total_couplefamilieswithkids = malloc(MAXROWS * sizeof(int));
  int* total_dwellings = malloc(MAXROWS * sizeof(int));  
  int* total_households = malloc(MAXROWS * sizeof(int));  
  int* total_persons1plus = malloc(MAXROWS * sizeof(int));
  int* total_males1plus = malloc(MAXROWS * sizeof(int));
  int* total_females1plus = malloc(MAXROWS * sizeof(int));
  int* total_persons5plus = malloc(MAXROWS * sizeof(int));
  int* total_males5plus = malloc(MAXROWS * sizeof(int));
  int* total_females5plus = malloc(MAXROWS * sizeof(int));
  int* total_persons15plus = malloc(MAXROWS * sizeof(int));
  int* total_males15plus = malloc(MAXROWS * sizeof(int));
  int* total_females15plus = malloc(MAXROWS * sizeof(int));
  int* total_persons15plusemployed = malloc(MAXROWS * sizeof(int));
  int* total_males15plusemployed = malloc(MAXROWS * sizeof(int));
  int* total_females15plusemployed = malloc(MAXROWS * sizeof(int));
  int* total_persons15plusqualified = malloc(MAXROWS * sizeof(int));
  int* total_males15plusqualified = malloc(MAXROWS * sizeof(int));
  int* total_females15plusqualified = malloc(MAXROWS * sizeof(int));
  int* total_persons15plusdoneschool = malloc(MAXROWS * sizeof(int));
  int* total_males15plusdoneschool = malloc(MAXROWS * sizeof(int));
  int* total_females15plusdoneschool = malloc(MAXROWS * sizeof(int));

  int sort_indx;
  int offset;
  int state_borders;
  
  if (argc!=67) {
    fprintf(stderr, "ERROR: expected 66 files in Basic Community Profile sets, got %d\n", argc-1);
    exit(1);
  }

  if (strstr(argv[2], "CED")||
      strstr(argv[2], "LGA")||
      strstr(argv[2], "POA")||
      strstr(argv[2], "SED")||
      strstr(argv[2], "SSC")
      ) {
    remove_regionstring = 1;
  } else {
    remove_regionstring = 0;
  }

  if ( strstr(argv[2], "CED") ) {
    sprintf(fullareastr, "Commonwealth Electoral Division");
  } else if ( strstr(argv[2], "LGA") ) {
    sprintf(fullareastr, "Local Government Area");
  } else if ( strstr(argv[2], "POA") ) {
    sprintf(fullareastr, "Postal Area");
  } else if ( strstr(argv[2], "SED") ) {
    sprintf(fullareastr, "State Electoral Division");
  } else if ( strstr(argv[2], "SSC") ) {
    sprintf(fullareastr, "State Suburb");
  } else if ( strstr(argv[2], "GCCSA") ) {
    sprintf(fullareastr, "Greater Capital City Statistical Areas");
  } else if ( strstr(argv[2], "IARE") ) {
    sprintf(fullareastr, "Indigenous Area");
  } else if ( strstr(argv[2], "ILOC") ) {
    sprintf(fullareastr, "Indigenous Location");
  } else if ( strstr(argv[2], "IREG") ) {
    sprintf(fullareastr, "Indigenous Region");
  } else if ( strstr(argv[2], "SA1") ) {
    sprintf(fullareastr, "Statistical Area Level 1");
  } else if ( strstr(argv[2], "SA2") ) {
    sprintf(fullareastr, "Statistical Area Level 2");
  } else if ( strstr(argv[2], "SA3") ) {
    sprintf(fullareastr, "Statistical Area Level 3");
  } else if ( strstr(argv[2], "SA4") ) {
    sprintf(fullareastr, "Statistical Area Level 4");
  } else if ( strstr(argv[2], "SLA") ) {
    sprintf(fullareastr, "Statistical Local Area");
  } else if ( strstr(argv[2], "STE") ) {
    sprintf(fullareastr, "State/Territory");
  } else {
    sprintf(fullareastr, "Unknown");
    fprintf(stderr, "unknown area string\n");
    exit(1);
  }


  getGPSlabels();
  
  f = 1;

  for (file_i = 0; file_i < argc-1; file_i++) {
    fprintf(stderr, "opening file %d\n", file_i);

    fp = fopen(argv[file_i+1], "r");
    
    /* find the bit of the filename that refers to which type of statistical region (hence which map) we are using */
    strtok(argv[file_i+1], "_");
    strtok(NULL, "_");
    strtok(NULL, "_");
    area_str = strtok(NULL, "_");
    //printf("area string %s\n", area_str);

    getsubmapbounds();

    start_field = f;
    i = 0;
    ch = ' ';
    
    /* read headers */
    while (ch!='\n') { 
      ch = fgetc(fp);
      //printf("%c\n", ch);
      if (ch=='\r') ch=fgetc(fp);
      if ((ch==',')||(ch=='\n')) {
	field_title[f][i] = '\0';
	
	/* fix an error in the data */
	if ((file_i==10)&&(f==1300)) {
	  fprintf(stderr, "correcting file 10:%s, field 1300: from %s to Japan_Persons\n", argv[11], field_title[f]);
	  sprintf(field_title[f], "Japan_Persons");
	}
	
	
	if (strcmp(field_title[f],"region_id")) {
	  /* for all fields other than the region_id (the id is in every file, no need to keep records of it) */
	  
	  if (0||(normalize_by[file_i]==SEX)&&(strstr(field_title[f],"Male")==NULL)&&(strstr(field_title[f],"Female")==NULL)&&(strstr(field_title[f],"Person")==NULL)&&(f!=start_field)) {
	    printf("File\t%d\tField\t%d\tNormType %d\tTitle\t %s\n", file_i, f, normalize_by[file_i], field_title[f]);
	  }
	  
	  file_num[f]=file_i;
	  f++;
	}
	i = 0;
	
      } else {
	field_title[f][i] = ch;
	i++;
      }
    }
    nfields = f-start_field;
    
    /* now read main body of table */
    row=0;
    //while ( fgets( buff, sizeof buff, fp ) != NULL ) {
    while (!feof(fp)) {
      //printf("reading file %d row %d\n", file_i, row);
      /* first get the region_id */

      if (remove_regionstring) {
	/* the first three characters of the region_id are letters, strip them */
	ch=fgetc(fp);
	ch=fgetc(fp);
	ch=fgetc(fp);
      }

      gotdata = fscanf(fp, "%d", &region_id[row]);
      ch=fgetc(fp);

      //printf("region id %d\n", region_id[row]);

      /* now read the rest of the fields */
      for (field = start_field; field<start_field+nfields; field++) {
	gotdata = fscanf(fp, "%f", &table[row][field]);
	ch=fgetc(fp);
	
	//printf("data [%d] %lf\n", field, table[row][field]);

	/*
	  if (!strcmp(field_title[field],"Total_Persons_Persons")) {
	  total_persons[row] = (int)(table[row][field]+0.1);
	  } else if (!strcmp(field_title[field],"Total_Persons_Males")) {
	  total_males[row] = (int)(table[row][field]+0.1);
	  //printf("Total males found\n");
	  } else if (!strcmp(field_title[field],"Total_Persons_Females")) {
	  total_females[row] = (int)(table[row][field]+0.1);
	  } else 
	*/
	
	if (gotdata==1) {
	  if (((field==7942)||(field==7324)||(field==7534)||(field==7834)||(field==7942))&&(table[row][field]>total_persons15plusemployed[row])) {
	    total_persons15plusemployed[row] = (int)(table[row][field]+0.1);
	  } else if (((field==6904)||(field==7634)||(field==7940))&&(table[row][field]>total_males15plusemployed[row])) {
	    total_males15plusemployed[row] = (int)(table[row][field]+0.1);
	  } else if (((field==7114)||(field==7734)||(field==7941))&&(table[row][field]>total_females15plusemployed[row])) {
	    total_females15plusemployed[row] = (int)(table[row][field]+0.1);
	  } else if (((field==5929)||(field==6334))&&(table[row][field]>total_persons15plusqualified[row])) {
	    total_persons15plusqualified[row] = (int)(table[row][field]+0.1);
	  } else if (((field==5731)||(field==6064))&&(table[row][field]>total_males15plusqualified[row])) {
	    total_males15plusqualified[row] = (int)(table[row][field]+0.1);
	  } else if (((field==5830)||(field==6199))&&(table[row][field]>total_females15plusqualified[row])) {
	    total_females15plusqualified[row] = (int)(table[row][field]+0.1);
	  } else if ((field==3123)&&(table[row][field]>total_persons15plusdoneschool[row])) {
	    total_persons15plusdoneschool[row] = (int)(table[row][field]+0.1);
	  } else if ((field==2963)&&(table[row][field]>total_males15plusdoneschool[row])) {
	    total_males15plusdoneschool[row] = (int)(table[row][field]+0.1);
	  } else if ((field==3043)&&(table[row][field]>total_females15plusdoneschool[row])) {
	    total_females15plusdoneschool[row] = (int)(table[row][field]+0.1);
	  } else if (((field==6694)||(field==3513)||(field==3777)||(field==3987)||(field==4107)||(field==4317))&&(table[row][field]>total_persons15plus[row])) {
	    total_persons15plus[row] = (int)(table[row][field]+0.1);
	  } else if (((field==6454)||(field==3253)||(field==3697)||(field==3847)||(field==4027)||(field==4177))&&(table[row][field]>total_males15plus[row])) {
	    total_males15plus[row] = (int)(table[row][field]+0.1);
	  } else if (((field==4821)||(field==3383)||(field==3737)||(field==3917)||(field==4067)||(field==4247)||(field==6574))&&(table[row][field]>total_females15plus[row])) {
	    total_females15plus[row] = (int)(table[row][field]+0.1);
	  } else if (field==5581) {
	    total_persons1plus[row] = (int)(table[row][field]+0.1);
	  } else if (field==5579) {
	    total_males1plus[row] = (int)(table[row][field]+0.1);
	  } else if (field==5580) {
	    total_females1plus[row] = (int)(table[row][field]+0.1);
	  } else if (field==5632) {
	    total_persons5plus[row] = (int)(table[row][field]+0.1);
	  } else if (field==5630) {
	    total_males5plus[row] = (int)(table[row][field]+0.1);
	  } else if (field==5631) {
	    total_females5plus[row] = (int)(table[row][field]+0.1);
	  } else if (field==4965) {
	    total_couplefamilieswithkids[row] = (int)(table[row][field]+0.1);
	  } else if ((!strcmp(field_title[field],"Total_Families"))&&(table[row][field]>total_families[row])) {
	    total_families[row] = (int)(table[row][field]+0.1);
	  } else if ((!strcmp(field_title[field],"Total_Total"))&&(file_i==36)&&(table[row][field]>total_families[row])) {
	    total_families[row] = (int)(table[row][field]+0.1);
	  } else if ((!strcmp(field_title[field],"Total_Dwellings"))&&(table[row][field]>total_dwellings[row])) {
	    total_dwellings[row] = (int)(table[row][field]+0.1);
	  } else if ((!strcmp(field_title[field],"Total_private_dwellings_Dwellings"))&&(table[row][field]>total_dwellings[row])) {
	    total_dwellings[row] = (int)(table[row][field]+0.1);
	  } else if ((!strcmp(field_title[field],"Total_Total"))&&(file_i==2)) {
	    if (table[row][field]>table[row][105]+table[row][108]) {
	      //printf("persons in %d from TotalTotal file 2: %d>%d\n", region_id[row], (int)table[row][field],(int)table[row][105]+(int)table[row][108]);
	      total_persons_enumerated[row] = (int)(table[row][field]+0.1);
	    } else {
	      //printf("persons in %d from sum home+alway f3: %d<%d\n", region_id[row], (int)table[row][field],(int)table[row][105]+(int)table[row][108]);
	      total_persons_enumerated[row] = (int)(table[row][105]+table[row][108]+0.1);
	    } 
	  } else if (field==106) {
	    total_males_enumerated[row] = (int)(table[row][103]+table[row][106]+0.1);
	  } else if (field==107) {
	    total_females_enumerated[row] = (int)(table[row][104]+table[row][107]+0.1);
	  } else if ((strstr(field_title[field],"Male"))&&((field<103)||(field>256))&&(table[row][field]>total_males[row])) {
	    total_males[row] = (int)(table[row][field]+0.1);
	  } else if ((strstr(field_title[field],"Female"))&&((field<103)||(field>256))&&(table[row][field]>total_females[row])) {
	    total_females[row] = (int)(table[row][field]+0.1);
	  } else if ((strstr(field_title[field],"Person"))&&((field<103)||(field>256))&&(table[row][field]>total_persons[row])) {
	    //printf("region %d field %d updating persons %d>%d\n", region_id[row], field, (int)table[row][field], total_persons[row]);
	    total_persons[row] = (int)(table[row][field]+0.1);
	  } else if ((!strcmp(field_title[field],"Total_Total"))&&(file_i>=38)&&(file_i<=46)&&(file_i!=41)&&(table[row][field]>total_households[row])) {
	    total_households[row] = (int)(table[row][field]+0.1);
	  }
	}
      }

      // there's an extra trailing character (CR?)
      ch=fgetc(fp);

      row++;
    }
    
    nrows=row;
    if (nrows>MAXROWS) {
      fprintf(stderr, "too many rows\n");
      exit(1);
    } 
    
    fclose(fp);
  }
  
  totfields=f-1;

  get_region_in_submap(region_in_submap, region_id);


  for (row = 0; row<nrows; row++) { 
    for (field = 1; field<=totfields; field++) {
      
      switch (normalize_by[file_num[field]]) {
      case NONE:
	normalize = 1.0;
	sprintf(field_normalizestr[field], "raw figure");
	field_colours_min[field][0] = 0.0;
	field_colours_min[field][1] = 0.0;
	field_colours_min[field][2] = 0.0;
	field_colours_max[field][0] = 0.0;
	field_colours_max[field][1] = 255.9;
	field_colours_max[field][2] = 150.0;
	break;
      case ENUMERATED:
	if (strstr(field_title[field],"Male")) {
	  normalize = total_males_enumerated[row];
	  sprintf(field_normalizestr[field], "total males here on census night");
	  field_colours_min[field][0] = 230.0;
	  field_colours_min[field][1] = 230.0;
	  field_colours_min[field][2] = 150.0;
	  field_colours_max[field][0] = 0.0;
	  field_colours_max[field][1] = 0.0;
	  field_colours_max[field][2] = 55.9;
	  break;
	} else if (strstr(field_title[field],"Female")) {
	  normalize = total_females_enumerated[row];
	  sprintf(field_normalizestr[field], "total females here on census night");
	  field_colours_min[field][0] = 230.0;
	  field_colours_min[field][1] = 230.0;
	  field_colours_min[field][2] = 150.0;
	  field_colours_max[field][0] = 125.9;
	  field_colours_max[field][1] = 0.0;
	  field_colours_max[field][2] = 0.0;
	  break;
	} else {
	  normalize = total_persons_enumerated[row];
	  sprintf(field_normalizestr[field], "total persons here on census night");
	  field_colours_min[field][0] = 230.0;
	  field_colours_min[field][1] = 230.0;
	  field_colours_min[field][2] = 150.0;
	  field_colours_max[field][0] = 0.0;
	  field_colours_max[field][1] = 55.9;
	  field_colours_max[field][2] = 0.0;
	  break;
	}
      case SEX:
	if (strstr(field_title[field],"Male")) {
	  normalize = total_males[row];
	  sprintf(field_normalizestr[field], "total males");
	  field_colours_min[field][0] = 230.0;
	  field_colours_min[field][1] = 230.0;
	  field_colours_min[field][2] = 150.0;
	  field_colours_max[field][0] = 0.0;
	  field_colours_max[field][1] = 0.0;
	  field_colours_max[field][2] = 55.9;
	  break;
	} else if (strstr(field_title[field],"Female")) {
	  normalize = total_females[row];
	  sprintf(field_normalizestr[field], "total females");
	  field_colours_min[field][0] = 230.0;
	  field_colours_min[field][1] = 230.0;
	  field_colours_min[field][2] = 150.0;
	  field_colours_max[field][0] = 125.9;
	  field_colours_max[field][1] = 0.0;
	  field_colours_max[field][2] = 0.0;
	  break;
	} else if (strstr(field_title[field],"Person")) {
	  normalize = total_persons[row];
	  sprintf(field_normalizestr[field], "total persons");
	  field_colours_min[field][0] = 230.0;
	  field_colours_min[field][1] = 230.0;
	  field_colours_min[field][2] = 150.0;
	  field_colours_max[field][0] = 0.0;
	  field_colours_max[field][1] = 55.9;
	  field_colours_max[field][2] = 0.0;
	  break;
	} else {
	  fprintf(stderr,"Male/Female/Person not found in SEX: %s\n", field_title[field]);
	  exit(1);
	}
      case SEX1PLUS:
	if (strstr(field_title[field],"Male")) {
	  normalize = total_males1plus[row];
	  sprintf(field_normalizestr[field], "total males aged 1 and over");
	  field_colours_min[field][0] = 230.0;
	  field_colours_min[field][1] = 230.0;
	  field_colours_min[field][2] = 150.0;
	  field_colours_max[field][0] = 0.0;
	  field_colours_max[field][1] = 0.0;
	  field_colours_max[field][2] = 55.9;
	  break;
	} else if (strstr(field_title[field],"Female")) {
	  normalize = total_females1plus[row];
	  sprintf(field_normalizestr[field], "total females aged 1 and over");
	  field_colours_min[field][0] = 230.0;
	  field_colours_min[field][1] = 230.0;
	  field_colours_min[field][2] = 150.0;
	  field_colours_max[field][0] = 125.9;
	  field_colours_max[field][1] = 0.0;
	  field_colours_max[field][2] = 0.0;
	  break;
	} else if (strstr(field_title[field],"Person")) {
	  normalize = total_persons1plus[row];
	  sprintf(field_normalizestr[field], "total persons aged 1 and over");
	  field_colours_min[field][0] = 230.0;
	  field_colours_min[field][1] = 230.0;
	  field_colours_min[field][2] = 150.0;
	  field_colours_max[field][0] = 0.0;
	  field_colours_max[field][1] = 55.9;
	  field_colours_max[field][2] = 0.0;
	  break;
	} else {
	  fprintf(stderr,"Male/Female/Person not found in SEX1PLUS: %s\n", field_title[field]);
	  exit(1);
	}
      case SEX5PLUS:
	if (strstr(field_title[field],"Male")) {
	  normalize = total_males5plus[row];
	  sprintf(field_normalizestr[field], "total males aged 5 and over");
	  field_colours_min[field][0] = 230.0;
	  field_colours_min[field][1] = 230.0;
	  field_colours_min[field][2] = 150.0;
	  field_colours_max[field][0] = 0.0;
	  field_colours_max[field][1] = 0.0;
	  field_colours_max[field][2] = 55.9;
	  break;
	} else if (strstr(field_title[field],"Female")) {
	  normalize = total_females5plus[row];
	  sprintf(field_normalizestr[field], "total females aged 5 and over");
	  field_colours_min[field][0] = 230.0;
	  field_colours_min[field][1] = 230.0;
	  field_colours_min[field][2] = 150.0;
	  field_colours_max[field][0] = 125.9;
	  field_colours_max[field][1] = 0.0;
	  field_colours_max[field][2] = 0.0;
	  break;
	} else if (strstr(field_title[field],"Person")) {
	  normalize = total_persons5plus[row];
	  sprintf(field_normalizestr[field], "total persons aged 5 and over");
	  field_colours_min[field][0] = 230.0;
	  field_colours_min[field][1] = 230.0;
	  field_colours_min[field][2] = 150.0;
	  field_colours_max[field][0] = 0.0;
	  field_colours_max[field][1] = 55.9;
	  field_colours_max[field][2] = 0.0;
	  break;
	} else {
	  fprintf(stderr,"Male/Female/Person not found in SEX5PLUS: %s\n", field_title[field]);
	  exit(1);
	}
      case SEX15PLUS:
	if (strstr(field_title[field],"Male")) {
	  normalize = total_males15plus[row];
	  sprintf(field_normalizestr[field], "total males aged 15 and over");
	  field_colours_min[field][0] = 230.0;
	  field_colours_min[field][1] = 230.0;
	  field_colours_min[field][2] = 150.0;
	  field_colours_max[field][0] = 0.0;
	  field_colours_max[field][1] = 0.0;
	  field_colours_max[field][2] = 55.9;
	  break;
	} else if (strstr(field_title[field],"Female")) {
	  normalize = total_females15plus[row];
	  sprintf(field_normalizestr[field], "total females aged 15 and over");
	  field_colours_min[field][0] = 230.0;
	  field_colours_min[field][1] = 230.0;
	  field_colours_min[field][2] = 150.0;
	  field_colours_max[field][0] = 125.9;
	  field_colours_max[field][1] = 0.0;
	  field_colours_max[field][2] = 0.0;
	  break;
	} else if (strstr(field_title[field],"Person")) {
	  normalize = total_persons15plus[row];
	  sprintf(field_normalizestr[field], "total persons aged 15 and over");
	  field_colours_min[field][0] = 230.0;
	  field_colours_min[field][1] = 230.0;
	  field_colours_min[field][2] = 150.0;
	  field_colours_max[field][0] = 0.0;
	  field_colours_max[field][1] = 55.9;
	  field_colours_max[field][2] = 0.0;
	  break;
	} else {
	  fprintf(stderr,"Male/Female/Person not found in SEX15PLUS: %s\n", field_title[field]);
	  exit(1);
	}
      case SEX15PLUSQUALIFIED:
	if (strstr(field_title[field],"Male")) {
	  normalize = total_males15plusqualified[row];
	  sprintf(field_normalizestr[field], "total qualified males aged 15 and over");
	  field_colours_min[field][0] = 230.0;
	  field_colours_min[field][1] = 230.0;
	  field_colours_min[field][2] = 150.0;
	  field_colours_max[field][0] = 0.0;
	  field_colours_max[field][1] = 0.0;
	  field_colours_max[field][2] = 55.9;
	  break;
	} else if (strstr(field_title[field],"Female")) {
	  normalize = total_females15plusqualified[row];
	  sprintf(field_normalizestr[field], "total qualified females aged 15 and over");
	  field_colours_min[field][0] = 230.0;
	  field_colours_min[field][1] = 230.0;
	  field_colours_min[field][2] = 150.0;
	  field_colours_max[field][0] = 125.9;
	  field_colours_max[field][1] = 0.0;
	  field_colours_max[field][2] = 0.0;
	  break;
	} else if (strstr(field_title[field],"Person")) {
	  normalize = total_persons15plusqualified[row];
	  sprintf(field_normalizestr[field], "total qualified persons aged 15 and over");
	  field_colours_min[field][0] = 230.0;
	  field_colours_min[field][1] = 230.0;
	  field_colours_min[field][2] = 150.0;
	  field_colours_max[field][0] = 0.0;
	  field_colours_max[field][1] = 55.9;
	  field_colours_max[field][2] = 0.0;
	  break;
	} else {
	  fprintf(stderr,"Male/Female/Person not found in SEX15PLUSQUALFIED: %s\n", field_title[field]);
	  exit(1);
	}
      case SEX15PLUSDONESCHOOL:
	if (strstr(field_title[field],"Male")) {
	  normalize = total_males15plusdoneschool[row];
	  sprintf(field_normalizestr[field], "total males aged 15 and over who are no longer attending primary or secondary school");
	  field_colours_min[field][0] = 230.0;
	  field_colours_min[field][1] = 230.0;
	  field_colours_min[field][2] = 150.0;
	  field_colours_max[field][0] = 0.0;
	  field_colours_max[field][1] = 0.0;
	  field_colours_max[field][2] = 55.9;
	  break;
	} else if (strstr(field_title[field],"Female")) {
	  normalize = total_females15plusdoneschool[row];
	  sprintf(field_normalizestr[field], "total females aged 15 and over who are no longer attending primary or secondary school");
	  field_colours_min[field][0] = 230.0;
	  field_colours_min[field][1] = 230.0;
	  field_colours_min[field][2] = 150.0;
	  field_colours_max[field][0] = 125.9;
	  field_colours_max[field][1] = 0.0;
	  field_colours_max[field][2] = 0.0;
	  break;
	} else if (strstr(field_title[field],"Person")) {
	  normalize = total_persons15plusdoneschool[row];
	  sprintf(field_normalizestr[field], "total persons aged 15 and over who are no longer attending primary or secondary school");
	  field_colours_min[field][0] = 230.0;
	  field_colours_min[field][1] = 230.0;
	  field_colours_min[field][2] = 150.0;
	  field_colours_max[field][0] = 0.0;
	  field_colours_max[field][1] = 55.9;
	  field_colours_max[field][2] = 0.0;
	  break;
	} else {
	  fprintf(stderr,"Male/Female/Person not found in SEX15PLUSDONESCHOOL: %s\n", field_title[field]);
	  exit(1);
	}
      case SEX15PLUSEMPLOYED:
	if (strstr(field_title[field],"Male")) {
	  normalize = total_males15plusemployed[row];
	  sprintf(field_normalizestr[field], "total employed males aged 15 and over");
	  field_colours_min[field][0] = 230.0;
	  field_colours_min[field][1] = 230.0;
	  field_colours_min[field][2] = 150.0;
	  field_colours_max[field][0] = 0.0;
	  field_colours_max[field][1] = 0.0;
	  field_colours_max[field][2] = 55.9;
	  break;
	} else if (strstr(field_title[field],"Female")) {
	  normalize = total_females15plusemployed[row];
	  sprintf(field_normalizestr[field], "total employed females aged 15 and over");
	  field_colours_min[field][0] = 230.0;
	  field_colours_min[field][1] = 230.0;
	  field_colours_min[field][2] = 150.0;
	  field_colours_max[field][0] = 125.9;
	  field_colours_max[field][1] = 0.0;
	  field_colours_max[field][2] = 0.0;
	  break;
	} else /* not actually all by sex, thus many don't state "Persons" */ {
	  normalize = total_persons15plusemployed[row];
	  sprintf(field_normalizestr[field], "total employed persons aged 15 and over");
	  field_colours_min[field][0] = 230.0;
	  field_colours_min[field][1] = 230.0;
	  field_colours_min[field][2] = 150.0;
	  field_colours_max[field][0] = 0.0;
	  field_colours_max[field][1] = 55.9;
	  field_colours_max[field][2] = 0.0;
	  break;
	}
      case PERSONS:
	normalize = total_persons[row];
	sprintf(field_normalizestr[field], "total persons");
	field_colours_min[field][0] = 230.0;
	field_colours_min[field][1] = 230.0;
	field_colours_min[field][2] = 150.0;
	field_colours_max[field][0] = 0.0;
	field_colours_max[field][1] = 55.9;
	field_colours_max[field][2] = 0.0;
	break;
      case FAMILIES:
	if (strstr(field_title[field],"Persons")) {
	  normalize = total_persons[row];
	  sprintf(field_normalizestr[field], "total persons");
	  field_colours_min[field][0] = 230.0;
	  field_colours_min[field][1] = 230.0;
	  field_colours_min[field][2] = 150.0;
	  field_colours_max[field][0] = 0.0;
	  field_colours_max[field][1] = 55.9;
	  field_colours_max[field][2] = 0.0;
	} else {
	  normalize = total_families[row];
	  sprintf(field_normalizestr[field], "total families");
	  field_colours_min[field][0] = 230.0;
	  field_colours_min[field][1] = 230.0;
	  field_colours_min[field][2] = 150.0;
	  field_colours_max[field][0] = 50.0;
	  field_colours_max[field][1] = 25.0;
	  field_colours_max[field][2] = 64.0;
	}
	break;
      case COUPLEFAMILIESWITHKIDS:
	normalize = total_couplefamilieswithkids[row];
	sprintf(field_normalizestr[field], "total couple families with kids");
	field_colours_min[field][0] = 230.0;
	field_colours_min[field][1] = 230.0;
	field_colours_min[field][2] = 150.0;
	field_colours_max[field][0] = 50.0;
	field_colours_max[field][1] = 25.0;
	field_colours_max[field][2] = 64.0;
	break;
      case DWELLINGS:
	if (strstr(field_title[field],"Persons")) {
	  normalize = total_persons[row];
	  sprintf(field_normalizestr[field], "total persons");
	  field_colours_min[field][0] = 230.0;
	  field_colours_min[field][1] = 230.0;
	  field_colours_min[field][2] = 150.0;
	  field_colours_max[field][0] = 0.0;
	  field_colours_max[field][1] = 55.9;
	  field_colours_max[field][2] = 0.0;
	} else {
	  normalize = total_dwellings[row];
	  sprintf(field_normalizestr[field], "total dwellings");
	  field_colours_min[field][0] = 230.0;
	  field_colours_min[field][1] = 230.0;
	  field_colours_min[field][2] = 150.0;
	  field_colours_max[field][0] = 70.0;
	  field_colours_max[field][1] = 35.0;
	  field_colours_max[field][2] = 10.0;
	}
	break;
      case FEMALES15PLUS:
	normalize = total_females15plus[row];
	sprintf(field_normalizestr[field], "total females aged 15 and over");
	field_colours_min[field][0] = 230.0;
	field_colours_min[field][1] = 230.0;
	field_colours_min[field][2] = 150.0;
	field_colours_max[field][0] = 128;
	field_colours_max[field][1] = 0.0;
	field_colours_max[field][2] = 64.0;
	break;
      case HOUSEHOLDS:
	normalize = total_households[row];
	sprintf(field_normalizestr[field], "total households");
	field_colours_min[field][0] = 230.0;
	field_colours_min[field][1] = 230.0;
	field_colours_min[field][2] = 150.0;
	field_colours_max[field][0] = 140.0;
	field_colours_max[field][1] = 50.0;
	field_colours_max[field][2] = 0.0;
	break;
      default:
	fprintf(stderr,"Normalize not found\n");
	break;
      }
      
      /* some fields are by place of enumeration in the middle of a by-usual-residence file. Adjust their normalization manually  */
      if ((field>=103)&&(field<=108)) {
      	if (strstr(field_title[field],"Male")) {
	  normalize = total_males_enumerated[row];
	  sprintf(field_normalizestr[field], "raw figure");
	} else if (strstr(field_title[field],"Female")) {
	  normalize = total_females_enumerated[row];
	  sprintf(field_normalizestr[field], "raw figure");
	} else {
	  normalize = total_persons_enumerated[row];
	  sprintf(field_normalizestr[field], "raw figure");
	}
      }


      /* some fields are not normalized in the middle of a by SEX file. Adjust their normalization manually  */
      if ((field>=5495)&&(field<=5503)) {
	normalize = 100.0; /* to convert out of percentage to decimal */
      	if (strstr(field_title[field],"Male")) {
	  sprintf(field_normalizestr[field], "males over 15");
	} else if (strstr(field_title[field],"Female")) {
	  sprintf(field_normalizestr[field], "females over 15");
	} else {
	  sprintf(field_normalizestr[field], "persons over 15");
	}	
      }
      

      // except in the following cases leave all colours on a single gradient scale: mid = (min+max)/2
      field_colours_mid[field][0] = 0.5*(field_colours_min[field][0]+field_colours_max[field][0]);
      field_colours_mid[field][1] = 0.5*(field_colours_min[field][1]+field_colours_max[field][1]);
      field_colours_mid[field][2] = 0.5*(field_colours_min[field][2]+field_colours_max[field][2]);

      switch (normalize_by[file_num[field]]) {
      case NONE:
	normalize = 1;
	sprintf(field_normalizestr[field], "raw figure");
	field_colours_min[field][0] = 87.0;
	field_colours_min[field][1] = 0.0;
	field_colours_min[field][2] = 87.0;
	field_colours_mid[field][0] = 230.0;
	field_colours_mid[field][1] = 230.0;
	field_colours_mid[field][2] = 150.0;
	field_colours_max[field][0] = 200.0;
	field_colours_max[field][1] = 0.0;
	field_colours_max[field][2] = 0.0;
	break;
      }      
      if (field==1) {
	normalize = total_persons[row];
	sprintf(field_normalizestr[field], "total persons");
	field_colours_min[field][0] = 255.9;
	field_colours_min[field][1] = 50.0;
	field_colours_min[field][2] = 50.0;
	field_colours_mid[field][0] = 255.9;
	field_colours_mid[field][1] = 255.9;
	field_colours_mid[field][2] = 255.9;
	field_colours_max[field][0] = 50.0;
	field_colours_max[field][1] = 50.0;
	field_colours_max[field][2] = 255.9;
      } else if (field==2) {
	normalize = total_persons[row];
	sprintf(field_normalizestr[field], "total persons");
	field_colours_min[field][0] = 50;
	field_colours_min[field][1] = 50.0;
	field_colours_min[field][2] = 255.9;
	field_colours_mid[field][0] = 255.9;
	field_colours_mid[field][1] = 255.9;
	field_colours_mid[field][2] = 255.9;
	field_colours_max[field][0] = 255.9;
	field_colours_max[field][1] = 50.0;
	field_colours_max[field][2] = 50.0;
      }

      
      if (normalize>0.00001) {	
	
	table[row][field] /= normalize;
	
	
	if ((total_persons[row]>sample_lower_bound)||(normalize_by[file_num[field]]==NONE)) {
	  /* this is statistically large enough to consider as a max or min bound */
	  for (s=0; s<numsubmaps; s++) {
	    if (region_in_submap[s][row]) {
	      if (table[row][field]<field_min[field][s]) {
		field_min[field][s] = table[row][field];
		field_mid[field][s] = 0.5*(field_min[field][s]+field_max[field][s]);
	      } 
	      if (table[row][field]>field_max[field][s]) {
		field_max[field][s] = table[row][field];
		field_mid[field][s] = 0.5*(field_min[field][s]+field_max[field][s]);
	      } 
	    }
	    field_mid[1][s] = 0.5; /* special behaviour for fields 1 and 2 */
	    field_mid[2][s] = 0.5;
	  }
	}
	/* this range might get undone later if a quartile sort is done */

      } else if (table[row][field]>0.9) {
	fprintf(stderr, "divide by zero: [%d][%d]: %f/%f\n", row, field, table[row][field], normalize);
	exit(1);
      } else {
	table[row][field] = -1.0; /* if normalizing 0/0 */
      }

      /* check that we're not exceeding 1.0 for normalized fields */
      if ((field!=0)&&(normalize_by[file_num[field]]!=NONE)&&(!(strstr(field_title[field],"esponses")))&&(table[row][field]>1.0)) {
	printf("%d %s n[%d] p=%d r=%d id=%d f=%d %5.3f %s\n", file_num[field], argv[file_num[field]+1], normalize_by[file_num[field]], total_persons[row], row, region_id[row], field, table[row][field], field_title[field]);
      }
      //printf("%f\t", table[row][field]);
    }
    //printf("\n");	  
  }    



  for (field = 1; field<=totfields; field++) {
      
    /* consider all regions for quartile bounds */
    for (s=0; s<numsubmaps; s++) {
      sort_indx = 0;
      for (row = 0; row<nrows; row++) { 
	if (region_in_submap[s][row] && (table[row][field]>-0.1)) {
	  tmp_tablesort[sort_indx] = table[row][field];
	  sort_indx++;
	}
      }
      
      //printf("Decile refinement [%d][%d] n=%d\t (%f %f %f) ", field, s, sort_indx, field_min[field][s], field_mid[field][s], field_max[field][s]);

      qsort(tmp_tablesort, sort_indx, sizeof(float), floatcomp);
	
      //printf("->  (%f %f %f)\n", tmp_tablesort[sort_indx/10], tmp_tablesort[sort_indx/2], tmp_tablesort[(9*sort_indx)/10]);

      field_min[field][s] = tmp_tablesort[sort_indx/20];
      field_mid[field][s] = tmp_tablesort[sort_indx/2];
      offset=1;
      while ((field_mid[field][s]<0.0000000000001)&&(sort_indx/2+offset<sort_indx)) {
	field_mid[field][s] = tmp_tablesort[sort_indx/2+offset];
	offset++;
      }
      field_max[field][s] = tmp_tablesort[(19*sort_indx)/20];
      if (offset>(4*sort_indx)/10) {
	field_max[field][s] = field_mid[field][s];
      }

      field_mid[1][s] = 0.5; /* special behaviour for fields 1 and 2 */
      field_mid[2][s] = 0.5;
    }
  } 


  tallies = fopen("tallies.txt", "w");
  for (row = 0; row<nrows; row++) { 
    fprintf(tallies, "row\t%d\tindex\t%d\ttotal persons\t%d\tmales\t%d\tfemales\t%d\tfamilies\t%d\tdwellings\t%d\tfemales15+\t%d\thouseholds\t%d\n", row, region_id[row], total_persons[row], total_males[row], total_females[row], total_families[row], total_dwellings[row], total_females15plus[row], total_households[row]);

    
    /* check that men+women ~= people */
    if (fabs((total_males[row]+total_females[row])/(double)total_persons[row] - 1.0)>0.05) {
      fprintf(stderr, "** row\t%d\tindex\t%d\ttotal persons\t%d\tmales\t%d\tfemales\t%d\tfamilies\t%d\tdwellings\t%d\tfemales15+\t%d\thouseholds\t%d\n", row, region_id[row], total_persons[row], total_males[row], total_females[row], total_families[row], total_dwellings[row], total_females15plus[row], total_households[row]);
      //exit(1);
    }
  }  
  fclose(tallies);
  

  sprintf(command, "mkdir normalized");
  printf("execute command: %s\n", command);
  ret=system(command);

  upload_fp = fopen("normalized/upload.sh", "w");
  refresh_descriptions_fp = fopen("normalized/refresh_descriptions.txt", "w");
  refresh_filenames_fp = fopen("normalized/refresh_filenames.txt", "w");
  filelist_fp = fopen("normalized/filelist.sh", "w");

  for (field = 1; field<=totfields; field++) {    
    undef_regions = 0;
    //if (!((field==2557)||(field==2577)||(field==2697)||(field==2664)||(field==2574)||(field==1261)) && (field<3)) {
    //if ((field==17)||(field==277)||(field==508)) {
      //if ((field>=1)&&(field<=7942)) {
    //if ((field>=1)&&(field<=totfields)) {
    if ((field>=1601)&&(field<=2200)) {
    //if ((field>=5474)&&(field<=5530)) {
    //if (field>=2560) {
    //if (((field>=3124)&&(field<=3513))||((field>=3658)&&(field<=4317))) {
    //if (field==114) {
    //if (field%10==1) {
      /* only make a few maps at this stage (disk space issue) */








      if (field==4318|| field==4328 || field==4338 ||field==4349 || field==4350 || field==4351 || field==4352 || field==4353 || field==4354 || field==4355 || field==4356 || field==4358 || field==4360 || field==4361 || field==4362 || field==4363 || field==4364 || field==4365 || field==4366 || field==4368 || field==4378 || field==4388 || field==4398 || field==4408 || field==4438 || field==4448 || field==4458 || field==4469 || field==4470 || field==4471 || field==4472 || field==4473 || field==4474 || field==4475 || field==4476 || field==4478 || field==4480 || field==4481 || field==4482 || field==4483 || field==4484 || field==4485 || field==4486 || field==4488 || field==4498 || field==4508 || field==4518 || field==4528 || field==4558 || field==4568 || field==4578 || field==4589 || field==4590 || field==4591 || field==4592 || field==4593 || field==4594 || field==4595 || field==4596 || field==4598 || field==4600 || field==4601 || field==4602 || field==4603 || field==4604 || field==4605 || field==4606 || field==4608 || field==4618 || field==4628 || field==4638 || field==4648 || field==5028 ) {
	/* these fields are theoretically impossible, so maps would all be blank - don't bother creating or uploading */
	/* IF CHANGING THIS LIST, ALSO CHANGE EQUIVS IN CATEGORY_TITLES.C */
	continue;
      } 








      remove_underscores(field_title_spaces[field],field_title[field]);

      sprintf(norm_fn,"normalized/map-markup.%d.svg",field);
      for (s=0; s<numsubmaps; s++) {

	if ((strcmp(submapnames[s],"Australia")==0)||(strcmp(submapnames[s],"Western_Australia")==0)||(strcmp(submapnames[s],"South_Australia")==0)||(strcmp(submapnames[s],"Queensland")==0)||(strcmp(submapnames[s],"New_South_Wales")==0)||(strcmp(submapnames[s],"Victoria")==0)||(strcmp(submapnames[s],"Northern_Territory")==0)) {
	  state_borders = 1;
	} else {
	  state_borders = 0;
	}
	
	
	MinBound[0] = submapbounds[s][0];
	MinBound[1] = submapbounds[s][1];
	MaxBound[0] = submapbounds[s][2];
	MaxBound[1] = submapbounds[s][3];

	MidPoint[0] = 0.5*(MaxBound[0]+MinBound[0]);
	MidPoint[1] = 0.5*(MaxBound[1]+MinBound[1]);
       
	long_over_lat_at_midpoint = cos(MidPoint[1]*M_PI/180.0); 
	dist_km_per_degree_longitude = M_PI*6371*cos(MidPoint[1]*M_PI/180.0)/180.0;
	length_km_of_width = dist_km_per_degree_longitude * (MaxBound[0]-MinBound[0]);


	head_fp = fopen("normalized/header.svg", "w");		

	fprintf(head_fp, "<?xml version=\"1.0\" standalone=\"no\"?>\n<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\"\n    \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n <svg preserveAspectRatio=\"none\" width=\"2000px\" height=\"%.15gpx\" viewBox=\"%.15g %.15g %.15g %.15g\"\n xmlns=\"http://www.w3.org/2000/svg\"\n xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n xmlns:ev=\"http://www.w3.org/2001/xml-events\" version=\"1.1\">\n  \n\n", 2000.0*(MaxBound[1]-MinBound[1])/(MaxBound[0]-MinBound[0])/long_over_lat_at_midpoint, 0.0/* MinBound[0] - MinBound[0]*/, 0.0 /* -MaxBound[1] + MaxBound[1] */, decimal_scaleout*(MaxBound[0]-MinBound[0]), decimal_scaleout*(-MinBound[1]+MaxBound[1]));
	fprintf(head_fp, "\t<title>Map of: %s in %s by Toby Hudson</title>\n", field_title_spaces[field], submapnames_spaces[s]);
	fprintf(head_fp, "\t<desc>%s (field %d) as a fraction of %s, in %s, Australia, according to the 2011 census. The map is divided into geographical subdivisions by %s. This graphic uses data from the Australian Bureau of Statistics, under the CC-BY-2.5-AU license. This graphic is copyright: Toby Hudson, and is released under the CC-BY-SA-3.0-AU license.</desc>\n", field_title_spaces[field], field, field_normalizestr[field], submapnames_spaces[s], fullareastr);
	   
	fprintf(head_fp, "<defs>\n"); /*the shapedump-tsh-base program puts together lots of defs, we also need to def the inset map */
	
	fclose(head_fp);

	norm_fp = fopen(norm_fn, "w");		
	
	fprintf(upload_fp, "python /home/hudson/general/wiki/pywikipedia/reupload_tsh.py -noverify -keep -filename:Australian_Census_2011_demographic_map_-_%s_by_%s_-_BCP_field_%04d_%.*s.svg map.%s.%d.svg \"=={{int:filedesc}}==\n{{Information\n|Description={{en|{{ucfirst:{{Australian2011CensusMaps/bcpfieldcat/%d}}}} as a fraction of %s, in %s, Australia, according to the 2011 census results. The map is divided into geographical subdivisions by %s. Note the split scale above and below the median value.}}\n|Source={{own}}\n{{Australian2011CensusMaps|1=Basic Community Profile|2=%d|3=%s|4=%s|5=%s}}\n|Date=2012-12-06\n|Author=[[User:99of9|Toby Hudson]] based on data from the [http://www.abs.gov.au Australian Bureau of Statistics]\n|Permission=\n|other_versions=\n}}\n== {{int:license-header}} ==\n{{self|cc-by-sa-3.0-au|attribution=[[User:99of9|Toby Hudson]] based on data from the [http://www.abs.gov.au Australian Bureau of Statistics]}}\n[[Category:Demographic maps of {{Australian2011CensusMaps/bcpfieldcat/%d}} in %s]]\n[[Category:Demographic maps of {{Australian2011CensusMaps/bcpfieldcat/%d}} by %s]]\n[[Category:Demographic maps of %s by %s based on the 2011 Census of Population and Housing]]\n{{User:99of9/Census2011}}\n\"\n", submapnames[s], area_str, field, 172-(int)strlen(submapnames[s])-(int)strlen(area_str), field_title[field], submapnames[s], field, field, field_normalizestr[field], submapnames_spaces[s], fullareastr, field, area_str, submapnames_spaces[s], field_title_spaces[field], field, submapnames_spaces[s], field, fullareastr, submapnames_spaces[s], fullareastr);

	// run in the normalized directory with: python /home/hudson/general/wiki/pywikipedia/pagefromfile.py -start:xxxx -end:yyyy -notitle -force -pt:10 -file:refresh_descriptions.txt
	fprintf(refresh_descriptions_fp, "xxxx\n'''File:Australian_Census_2011_demographic_map_-_%s_by_%s_-_BCP_field_%04d_%.*s.svg'''\n=={{int:filedesc}}==\n{{Information\n|Description={{en|{{ucfirst:{{Australian2011CensusMaps/bcpfieldcat/%d}}}} as a fraction of %s, in %s, Australia, according to the 2011 census results. The map is divided into geographical subdivisions by %s. Note the split scale above and below the median value.}}\n|Source={{own}}\n{{Australian2011CensusMaps|1=Basic Community Profile|2=%d|3=%s|4=%s|5=%s}}\n|Date=2012-12-06\n|Author=[[User:99of9|Toby Hudson]] based on data from the [http://www.abs.gov.au Australian Bureau of Statistics]\n|Permission=\n|other_versions=\n}}\n== {{int:license-header}} ==\n{{self|cc-by-sa-3.0-au|attribution=[[User:99of9|Toby Hudson]] based on data from the [http://www.abs.gov.au Australian Bureau of Statistics]}}\n[[Category:Demographic maps of {{Australian2011CensusMaps/bcpfieldcat/%d}} in %s]]\n[[Category:Demographic maps of {{Australian2011CensusMaps/bcpfieldcat/%d}} by %s]]\n[[Category:Demographic maps of %s by %s based on the 2011 Census of Population and Housing]]\n{{User:99of9/Census2011}}\nyyyy\n", submapnames[s], area_str, field,  172-(int)strlen(submapnames[s])-(int)strlen(area_str), field_title[field],  field, field_normalizestr[field], submapnames_spaces[s], fullareastr, field, area_str, submapnames_spaces[s], field_title_spaces[field], field, submapnames_spaces[s], field, fullareastr, submapnames_spaces[s], fullareastr);
	if (field<1000) {
	  fprintf(refresh_filenames_fp, "[[File:Australian_Census_2011_demographic_map_-_%s_by_%s_-_BCP_field_%d_%.*s.svg]] [[File:Australian_Census_2011_demographic_map_-_%s_by_%s_-_BCP_field_%04d_%.*s.svg]]\n", submapnames[s], area_str, field,  172-(int)strlen(submapnames[s])-(int)strlen(area_str), field_title[field], submapnames[s], area_str, field,  172-(int)strlen(submapnames[s])-(int)strlen(area_str), field_title[field]);
	}
	fprintf(filelist_fp, "Australian_Census_2011_demographic_map_-_%s_by_%s_-_BCP_field_%04d_%.*s.svg\n", submapnames[s], area_str, field,  172-(int)strlen(submapnames[s])-(int)strlen(area_str), field_title[field]);

	fprintf(norm_fp, "</defs>\n"); /*the shapedump-tsh-base program puts together lots of defs */

	fprintf(norm_fp,"<use xlink:href=\"#bg\" />\n");

	for (row = 0; row<nrows; row++) {
	  if (region_in_submap[s][row]==1) {
	    if ( strstr(area_str, "POA") ) {
	      sprintf(regionstr, "%04d", region_id[row]);
	    } else {
	      sprintf(regionstr, "%d", region_id[row]);
	    }
	   
	    fprintf(norm_fp, "<use xlink:href=\"#%s\" ", regionstr);
	    
	    if (table[row][field]>-0.001) {
	      for (col=0; col<3; col++) {
		if (table[row][field]<field_min[field][s]+0.00000000000000001) {
		  this_colour[col] = (int)field_colours_min[field][col];
		} else if (table[row][field]<field_mid[field][s]) {
		  this_colour[col] = (int)(field_colours_min[field][col]+(field_colours_mid[field][col]-field_colours_min[field][col])*MAX(0.0,MIN(1.0,(table[row][field]-field_min[field][s])/(field_mid[field][s]-field_min[field][s]))));
		} else {
		  this_colour[col] = (int)(field_colours_mid[field][col]+(field_colours_max[field][col]-field_colours_mid[field][col])*MAX(0.0,MIN(1.0,(table[row][field]-field_mid[field][s])/(field_max[field][s]-field_mid[field][s]))));
		}
		//if (this_colour[col]>field_colours_max[field][col]) this_colour[col] = (int)field_colours_max[field][col];
		//if (this_colour[col]<0) this_colour[col] = 0;
	      }
	      
	      fprintf(norm_fp, "fill=\"rgb(%d,%d,%d)\" />\n", this_colour[0], this_colour[1], this_colour[2] );
	    } else {
	      /* there is no data for this field 0/0 */
	      undef_regions = 1;
	      fprintf(norm_fp, "fill=\"rgb(220,200,200)\" />\n");
	    }
	  }
	}

	/* thick state outlines for large state maps */
	if (state_borders) {
	  fprintf(norm_fp, "<g transform=\"translate(%f,%f) scale(%f,%f)\">\n", decimal_scaleout*(-MinBound[0]+96.816941408), decimal_scaleout*(+9.142175977+MaxBound[1]), 1.0, 1.0); 
	  fprintf(norm_fp,"\t<use xlink:href=\"#geolocator_inset_states\" fill=\"none\"  stroke = \"rgb(0,0,0)\" stroke-width = \"%f\" opacity=\"0.5\"/>\n", 0.05*decimal_scaleout);
	  fprintf(norm_fp, "</g>\n");
	}

	/* Now set up a scale box */
	fprintf(norm_fp, "<linearGradient id = \"g1\" x1 = \"0%%\" y1 = \"0%%\" x2 = \"0%%\" y2 = \"100%%\">\n  <stop stop-color = \"rgb(%d,%d,%d)\" offset = \"0%%\"/>\n  <stop stop-color = \"rgb(%d,%d,%d)\" offset = \"%d%%\"/>\n  <stop stop-color = \"rgb(%d,%d,%d)\" offset = \"100%%\"/>\n</linearGradient>\n", (int)field_colours_max[field][0], (int)field_colours_max[field][1], (int)field_colours_max[field][2], (int)field_colours_mid[field][0], (int)field_colours_mid[field][1], (int)field_colours_mid[field][2], /*(int)(100.0*(field_max[field][s]-field_mid[field][s])/(field_max[field][s]-field_min[field][s]))*/ 50, (int)field_colours_min[field][0], (int)field_colours_min[field][1], (int)field_colours_min[field][2]);

	

	fprintf(norm_fp, "<filter id=\"f1\" filterUnits=\"userSpaceOnUse\" x=\"-500%%\" y=\"-500%%\" width=\"1000%%\" height=\"1000%%\">\n\t<feOffset result=\"offOut\" in=\"SourceAlpha\" dx=\"%f\" dy=\"%f\" />\n\t<feGaussianBlur result=\"blurOut\" in=\"offOut\" stdDeviation=\"%f\" />\n\t<feBlend in=\"SourceGraphic\" in2=\"blurOut\" mode=\"normal\" />\n\t</filter>\n", 0.003*decimal_scaleout*(MaxBound[0]-MinBound[0]), 0.003*decimal_scaleout*(MaxBound[0]-MinBound[0]),  0.006*decimal_scaleout*(MaxBound[0]-MinBound[0]));
	
	//fprintf(norm_fp,"<filter id=\"Matrix\" x=\"-30%%\" y=\"-5%%\" width=\"140%%\" height=\"110%%\">\n\t<feColorMatrix type=\"matrix\" in=\"SourceAlpha\" out=\"white\"\n\t\tvalues=\".0 .0 .0 1.0 1.0\n\t\t\t.0 .0 .0 1.0 1.0\n\t\t\t.0 .0 .0 1.0 1.0\n\t\t\t.0 .0 .0 1.0 0.3\"/>\n\t<feOffset result=\"offOut\" in=\"white\" dx=\"5\" dy=\"5\" />\n\t<feGaussianBlur result=\"blurOut\" in=\"offOut\" stdDeviation=\"5\" />\n\t<feBlend in=\"SourceGraphic\" in2=\"blurOut\" mode=\"normal\" />\n</filter>\n");
	fprintf(norm_fp,"<filter id=\"Matrix\" filterUnits=\"userSpaceOnUse\" x=\"%f\" y=\"%f\" width=\"%f\" height=\"%f\" >\n\t<feOffset result=\"offOut\" in=\"SourceAlpha\" dx=\"5\" dy=\"5\" />\n\t<feGaussianBlur result=\"blurOut\" in=\"offOut\" stdDeviation=\"5\" />\n\t<feColorMatrix type=\"matrix\" in=\"SourceGraphic\" result=\"white\"\n\t\tvalues=\".0 .0 .0 1.0 1.0\n\t\t\t.0 .0 .0 1.0 1.0\n\t\t\t.0 .0 .0 1.0 1.0\n\t\t\t.0 .0 .0 1.0 0.6\"/>\n\t<feBlend in=\"SourceGraphic\" in2=\"white\" mode=\"normal\" />\n</filter>\n", -0.055*decimal_scaleout*(MaxBound[0]-MinBound[0]), 0.00*decimal_scaleout*(MaxBound[0]-MinBound[0]), 0.185*decimal_scaleout*(MaxBound[0]-MinBound[0]), 0.315*decimal_scaleout*(MaxBound[0]-MinBound[0]));
	//fprintf(norm_fp,"<filter id=\"Matrix\" filterUnits=\"userSpaceOnUse\" x=\"-5.2%%\" y=\"1.5%%\" width=\"21.5%%\" height=\"50.2%%\" >\n\t<feOffset result=\"offOut\" in=\"SourceAlpha\" dx=\"5\" dy=\"5\" />\n\t<feGaussianBlur result=\"blurOut\" in=\"offOut\" stdDeviation=\"5\" />\n\t<feColorMatrix type=\"matrix\" in=\"blurOut\" out=\"white\"\n\t\tvalues=\".0 .0 .0 1.0 1.0\n\t\t\t.0 .0 .0 1.0 1.0\n\t\t\t.0 .0 .0 1.0 1.0\n\t\t\t.0 .0 .0 1.0 0.5\"/>\n\t<feBlend in=\"SourceGraphic\" in2=\"white\" mode=\"normal\" />\n</filter>\n");
	//fprintf(norm_fp,"<filter id=\"Matrix\" x=\"-42%%\" y=\"-2%%\" width=\"152%%\" height=\"107%%\">\n\t<feOffset result=\"offOut\" in=\"SourceAlpha\" dx=\"5\" dy=\"5\" />\n\t<feGaussianBlur result=\"blurOut\" in=\"offOut\" stdDeviation=\"5\" />\n\t<feColorMatrix type=\"matrix\" in=\"blurOut\" out=\"white\"\n\t\tvalues=\".0 .0 .0 1.0 1.0\n\t\t\t.0 .0 .0 1.0 1.0\n\t\t\t.0 .0 .0 1.0 1.0\n\t\t\t.0 .0 .0 1.0 0.3\"/>\n\t<feBlend in=\"SourceGraphic\" in2=\"white\" mode=\"normal\" />\n</filter>\n");

	print_label_names(norm_fp, MaxBound, MinBound);

	fprintf(norm_fp,"\t<rect x=\"%f\" y=\"%f\" width=\"%f\" height=\"%f\" fill=\"rgb(150,200,200)\" opacity=\"0.7\" stroke=\"black\" stroke-width=\"0.1%%\"/>\n", 0.005*decimal_scaleout*(MaxBound[0]-MinBound[0]), 0.005*decimal_scaleout*(MaxBound[0]-MinBound[0]), 0.185*decimal_scaleout*(MaxBound[0]-MinBound[0]),  0.17*decimal_scaleout*(MaxBound[0]-MinBound[0])*long_over_lat_at_midpoint);
	fprintf(norm_fp, "<g transform=\"translate(-%f,%f) scale(%f,%f)\">\n", 55.0*(MaxBound[0]-MinBound[0]), 0.006*decimal_scaleout*(MaxBound[0]-MinBound[0]), 0.0041758*(MaxBound[0]-MinBound[0]), 0.0041758*(MaxBound[0]-MinBound[0])*long_over_lat_at_midpoint/cos(26.44*M_PI/180.0)); /* last bit is to ensure Australia scales to Australia's midpoint */
	fprintf(norm_fp,"\t<use xlink:href=\"#geolocator_inset_states\" fill=\"white\"  stroke = \"black\" stroke-width = \"%f\"/>\n", 0.04*decimal_scaleout);
	fprintf(norm_fp,"\t<rect x=\"%f\" y=\"%f\" width=\"%f\" height=\"%f\" fill=\"none\" stroke=\"red\" stroke-width = \"%f\" opacity=\"0.8\"/>\n", decimal_scaleout*(MinBound[0]-96.816941408), decimal_scaleout*(-9.142175977-MaxBound[1]), decimal_scaleout*(MaxBound[0]-MinBound[0]), decimal_scaleout*(MaxBound[1]-MinBound[1]), 0.4*decimal_scaleout);
	fprintf(norm_fp, "</g>\n");



	
	fprintf(norm_fp, "<g transform=\"translate(%f,%f)\" filter=\"url(#Matrix)\" font-size=\"%f\" fill=\"black\"  stroke = \"none\" stroke-width = \"%f\" font-family=\"DejaVu Sans,sans-serif\"  style=\"letter-spacing:%f;\">\n", (submaplegendpos[s][0]+0.04)*decimal_scaleout*(MaxBound[0]-MinBound[0]), decimal_scaleout*(MaxBound[1]-MinBound[1])-submaplegendpos[s][1]*decimal_scaleout*(MaxBound[0]-MinBound[0]), 0.025*decimal_scaleout*(MaxBound[0]-MinBound[0]), 0.0004*decimal_scaleout*(MaxBound[0]-MinBound[0]), 0.0*decimal_scaleout);	

	fprintf(norm_fp,"\t<rect x=\"%f\" y=\"%f\" width=\"%f\" height=\"%f\" fill=\"none\" stroke=\"black\" stroke-width=\"0.2%%\"/>\n", -0.055*decimal_scaleout*(MaxBound[0]-MinBound[0]), 0.00*decimal_scaleout*(MaxBound[0]-MinBound[0]), 0.185*decimal_scaleout*(MaxBound[0]-MinBound[0]), 0.315*decimal_scaleout*(MaxBound[0]-MinBound[0]));

	fprintf(norm_fp, "\t<text y=\"%f\" x=\"1%%\">?</text>\n", 0.295*decimal_scaleout*(MaxBound[0]-MinBound[0]));

	//if ((normalize_by[file_num[field]]!=NONE)&&(!((field>=5495)&&(field<=5503)))) {
	if (normalize_by[file_num[field]]!=NONE) {
	  if (field_min[field][s]>0.000000000000001) {
	    if (100.0*field_min[field][s]>1.0) {
	      fprintf(norm_fp, "\t<text y=\"%f\">&lt;%#4.2F%%</text>\n", 0.26*decimal_scaleout*(MaxBound[0]-MinBound[0]), 100.0*field_min[field][s]);
	    } else {
	      fprintf(norm_fp, "\t<text y=\"%f\">&lt;%#4.2G%%</text>\n", 0.26*decimal_scaleout*(MaxBound[0]-MinBound[0]), 100.0*field_min[field][s]);
	    }
	  } else {
	    fprintf(norm_fp, "\t<text y=\"%f\" x=\"1%%\">0</text>\n", 0.26*decimal_scaleout*(MaxBound[0]-MinBound[0]));
	  }
	  if (100.0*field_mid[field][s]>1.0) {
	    fprintf(norm_fp, "\t<text y=\"%f\" x=\"1%%\">%#4.2F%%</text>\n", 0.149*decimal_scaleout*(MaxBound[0]-MinBound[0]), 100.0*field_mid[field][s]);
	  } else {
	    fprintf(norm_fp, "\t<text y=\"%f\" x=\"1%%\">%#4.2G%%</text>\n", 0.149*decimal_scaleout*(MaxBound[0]-MinBound[0]), 100.0*field_mid[field][s]);
	  }
	  if (100.0*field_max[field][s]>1.0) {
	    fprintf(norm_fp, "\t<text y=\"%f\">&gt;%#4.2F%%</text>\n", 0.038*decimal_scaleout*(MaxBound[0]-MinBound[0]), 100.0*field_max[field][s]);
	  } else {
	    fprintf(norm_fp, "\t<text y=\"%f\">&gt;%#4.2G%%</text>\n", 0.038*decimal_scaleout*(MaxBound[0]-MinBound[0]), 100.0*field_max[field][s]);
	  }
	} else {
	  /* don't put non-normalized things in percentages */
	  if (field_min[field][s]>0.000000000000001) {
	    fprintf(norm_fp, "\t<text y=\"%f\">&lt;%#.4G</text>\n", 0.26*decimal_scaleout*(MaxBound[0]-MinBound[0]), field_min[field][s]);
	  } else {
	    fprintf(norm_fp, "\t<text y=\"%f\" x=\"1%%\">0</text>\n", 0.26*decimal_scaleout*(MaxBound[0]-MinBound[0]));
	  }
	  fprintf(norm_fp, "\t<text y=\"%f\" x=\"1%%\">%#.4G</text>\n", 0.149*decimal_scaleout*(MaxBound[0]-MinBound[0]), field_mid[field][s]);
	  fprintf(norm_fp, "\t<text y=\"%f\">&gt;%#.4G</text>\n", 0.038*decimal_scaleout*(MaxBound[0]-MinBound[0]), field_max[field][s]);
	}


	fprintf(norm_fp, "</g>\n");

	fprintf(norm_fp, "<use xlink:href=\"#scalebar\" fill=\"url(#g1)\" filter=\"url(#f1)\"/>\n");
	fprintf(norm_fp, "<use xlink:href=\"#scalemarks\" filter=\"url(#f1)\"/>\n"); /* bar across scale to show the split scale */
	//if (undef_regions) {
	fprintf(norm_fp, "<use xlink:href=\"#undefinedbar\"  fill=\"rgb(220,200,200)\" filter=\"url(#f1)\"/>\n");
	fprintf(norm_fp, "<use xlink:href=\"#distancebar\" opacity=\"1.0\" transform=\"translate(%f,%f)\"/>\n", (submapdistancepos[s][0]-1.0)*decimal_scaleout*(MaxBound[0]-MinBound[0]), (submapdistancepos[s][1]-1.0)*decimal_scaleout*(MaxBound[0]-MinBound[0]));

	//distancebar
	dbar_indx = 0;
	while (dbar_lengths[dbar_indx]<0.18*length_km_of_width) {
	  dbar_indx++;
	}
	dbar_indx--;
	distancebar_length = dbar_lengths[dbar_indx];
	fprintf(norm_fp, "<text font-size=\"%f\" fill=\"black\"  stroke = \"none\" stroke-width = \"%f\" font-family=\"DejaVu Sans,sans-serif\"  style=\"letter-spacing:%f;\" y=\"%f\" x=\"%f%%\">%6.0f km</text>\n",  0.024*decimal_scaleout*(MaxBound[0]-MinBound[0]), 0.0004*decimal_scaleout*(MaxBound[0]-MinBound[0]), 0.0*decimal_scaleout, decimal_scaleout*(MaxBound[1]-MinBound[1]+(submapdistancepos[s][1]-1.0-0.04)*(MaxBound[0]-MinBound[0])), 100*(submapdistancepos[s][0]-0.20), distancebar_length);
	fprintf(norm_fp, "<text font-size=\"%f\" fill=\"rgb(50,50,50)\"  stroke = \"none\" stroke-width = \"%f\" font-family=\"DejaVu Sans,sans-serif\"  style=\"letter-spacing:%f;\" y=\"%f\" x=\"%f%%\">%6.0f mi</text>\n",  0.024*decimal_scaleout*(MaxBound[0]-MinBound[0]), 0.0004*decimal_scaleout*(MaxBound[0]-MinBound[0]), 0.0*decimal_scaleout, decimal_scaleout*(MaxBound[1]-MinBound[1]+(submapdistancepos[s][1]-1.0-0.005)*(MaxBound[0]-MinBound[0])), 100*(submapdistancepos[s][0]-0.20), distancebar_length);


	fprintf(norm_fp, "<text font-size=\"%f\" fill=\"rgb(50,50,50)\"  stroke = \"none\" stroke-width = \"%f\" font-family=\"DejaVu Sans,sans-serif\"  style=\"letter-spacing:%f;text-anchor:middle;\" y=\"%f\" x=\"98.3%%\">N</text>\n",  0.03*decimal_scaleout*(MaxBound[0]-MinBound[0]), 0.0004*decimal_scaleout*(MaxBound[0]-MinBound[0]), 0.0, 0.03*decimal_scaleout*(MaxBound[0]-MinBound[0]));
	fprintf(norm_fp, "<text font-size=\"%f\" fill=\"rgb(50,50,50)\"  stroke = \"none\" stroke-width = \"%f\" font-family=\"DejaVu Sans,sans-serif\"  style=\"letter-spacing:%f;text-anchor:middle;\" y=\"%f\" x=\"98.3%%\">↑</text>\n",  0.05*decimal_scaleout*(MaxBound[0]-MinBound[0]), 0.0004*decimal_scaleout*(MaxBound[0]-MinBound[0]), 0.0, 0.07*decimal_scaleout*(MaxBound[0]-MinBound[0]));

	fprintf(norm_fp,"<rect x=\"0%%\" y=\"0%%\" width=\"100%%\" height=\"100%%\" fill=\"none\" stroke=\"black\" stroke-width=\"0.4%%\"/>\n");


	//}


	/* try a border around the map
	fprintf(norm_fp, "<g fill=\"yellow\" stroke=\"none\">\n");
	//top border
	fprintf(norm_fp, "\t<rect x=\"%f\" y=\"%f\" width=\"%f\" height=\"%f\" />\n", 0.0, 0.0, decimal_scaleout*(MaxBound[0]-MinBound[0]), 10.0);
	//bottom border
	fprintf(norm_fp, "\t<rect x=\"%f\" y=\"%f\" width=\"%f\" height=\"%f\" />\n", 0.0, decimal_scaleout*(MaxBound[1]-MinBound[1])-10.0, decimal_scaleout*(MaxBound[0]-MinBound[0]), 10.0);
	//left border
	fprintf(norm_fp, "\t<rect x=\"%f\" y=\"%f\" width=\"%f\" height=\"%f\" />\n", 0.0, 0.0, 10.0, decimal_scaleout*(MaxBound[1]-MinBound[1]));
	//right border
	fprintf(norm_fp, "\t<rect x=\"%f\" y=\"%f\" width=\"%f\" height=\"%f\" />\n", decimal_scaleout*(MaxBound[0]-MinBound[0])-10.0, 0.0, 10.0, decimal_scaleout*(MaxBound[1]-MinBound[1]));
	fprintf(norm_fp, "</g>\n");
	*/

	fprintf(norm_fp, "</svg>\n");
	fclose(norm_fp);

	if (state_borders) {
	  sprintf(command, "cat ./normalized/header.svg ~/general/wiki/ABS/census/2011_%s/detailed_shape_base-%s.svg ~/general/wiki/ABS/census/geolocator_inset_detailed.svg ./normalized/map-markup.%d.svg > ./normalized/map.%s.%d.svg", area_str, submapnames[s], field, submapnames[s], field);
	} else {
	  sprintf(command, "cat ./normalized/header.svg ~/general/wiki/ABS/census/2011_%s/detailed_shape_base-%s.svg ~/general/wiki/ABS/census/geolocator_inset.svg ./normalized/map-markup.%d.svg > ./normalized/map.%s.%d.svg", area_str, submapnames[s], field, submapnames[s], field);
	}

	printf("execute command: %s\n", command);
	ret = system(command);
	printf("return %d\n", ret);


	if (DRYRUNTEXTONLY) {
	  sprintf(command, "rm ./normalized/map.%s.%d.svg", submapnames[s], field);
	  ret = system(command);
	}

      }
      sprintf(command, "rm ./normalized/map-markup.%d.svg", field);
      ret=system(command);
      if (ret!=0) {
	printf("return %d\n", ret);
	exit(1);
      }
    }
  }

  sprintf(command, "rm ./normalized/header.svg");
  ret=system(command);
  
  fclose(upload_fp);
  fclose(filelist_fp);
  
  /* free the memory allocated to the enormous table */
  for (r = 0; r < MAXROWS; ++r) {
    free(table[r]);
  }
  free(table);
  free(tmp_tablesort);
  free(region_id);
  free(file_num);
  free(field_min);
  free(field_mid);
  free(field_max);
  for (f=0; f<MAXFIELD; f++) {
    free(field_colours_min[f]);
    free(field_colours_mid[f]);
    free(field_colours_max[f]);
  }
  free(field_colours_min);
  free(field_colours_max);
  free(total_persons);
  free(total_males);
  free(total_females);
  free(total_females15plus);
  free(total_families);
  free(total_dwellings);
  free(total_households);
  
}
