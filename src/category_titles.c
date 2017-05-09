#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXFIELDS 8000
#define EXTRACATS 3
//117
#define MAXCATS 6


#define NSUBDIV 4
#define NAREAS 6

#define DOBCPFIELDCATS 0
#define DOSUBDIV 1
#define DOAREAS 1
#define DOEXTRACATS 1

int startfield=2662;
int finishfield=2664;
//int finishfield=7942;

//int do_subdiv[NSUBDIV] = {3};
int do_subdiv[NSUBDIV] = {0, 3, 10, 14};
int do_areas[NAREAS] = {0, 1, 2, 3, 5, 9};
//int do_areas[NAREAS] = {9};

char *geosubdivision[16] = {"", "CED", "LGA", "POA", "SED", "SSC", "GCCSA", "IARE", "ILOC", "IREG", "SA1", "SA2", "SA3", "SA4", "SLA", "STE"};
char *geosubdivision_long[16] = {"geographic subdivision", "Commonwealth Electoral Division", "Local Government Area", "Postal Area", "State Electoral Division", "State Suburb", "Greater Capital City Statistical Area", "Indigenous Area", "Indigenous Location", "Indigenous Region", "Statistical Area Level 1", "Statistical Area Level 2", "Statistical Area Level 3", "Statistical Area Level 4", "Statistical Local Area", "State or Territory"};

char *area_names[20] = {"Australia by state or territory", "Australia by city", "Australia", "Inner Sydney", "Greater Sydney", "New South Wales", "Greater Melbourne", "Victoria", "Greater Brisbane", "Queensland", "Greater Adelaide", "South Australia", "Greater Perth", "Western Australia", "Greater Hobart", "Tasmania", "Greater Darwin", "Northern Territory", "Australian Capital Territory", "Canberra"};
char *area_wikinames[20] = {"*state or territory", "*city", "Australia", "Sydney", "Sydney", "New South Wales", "Melbourne", "Victoria", "Brisbane", "Queensland", "Adelaide", "South Australia", "Perth", "Western Australia", "Hobart", "Tasmania", "Darwin", "Northern Territory", "Australian Capital Territory", "Canberra"};
char *superarea_names[20] = {"Australia", "Australia", "", "New South Wales", "New South Wales", "Australia", "Victoria", "Australia", "Queensland", "Australia", "South Australia", "Australia", "Western Australia", "Australia", "Tasmania", "Australia", "Northern Territory", "Australia", "Australia", "Australian Capital Territory"};
char *area_names_by[20] = {"", "", "", "Australia by city", "Australia by city", "Australia by state or territory", "Australia by city", "Australia by state or territory", "Australia by city", "Australia by state or territory", "Australia by city", "Australia by state or territory", "Australia by city", "Australia by state or territory", "Australia by city", "Australia by state or territory", "Australia by city", "Australia by state or territory", "Australia by state or territory", "Australia by city"};


char *replace_str(char *str, char *orig, char *rep)
{
  static char buffer[4096];
  char *p;

  if(!(p = strstr(str, orig)))  // Is 'orig' even in 'str'?
    return str;

  strncpy(buffer, str, p-str); // Copy characters from 'str' start to 'orig' st$
  buffer[p-str] = '\0';

  sprintf(buffer+(p-str), "%s%s", rep, p+strlen(orig));

  return buffer;
}

int main() {
  int i, r, row, col, slab;
  int field;
  int extracat;
  int sc, countsc, osc;

  char field_category_names[MAXFIELDS][350];
  char field_generalcategory_names[MAXFIELDS][100];
  //char field_supercategory_names[MAXFIELDS][MAXCATS][150];
  
  char*** field_supercategory_names = malloc(MAXFIELDS * sizeof(char**));    // allocate the rows
  for (r = 0; r < MAXFIELDS; ++r)
    {
      field_supercategory_names[r] = malloc(MAXCATS * sizeof(char*));    // allocate the columns
      for (i = 0; i<MAXCATS; i++) field_supercategory_names[r][i] = malloc(550*sizeof(char));
    }

  //char *extra_category_names[EXTRACATS] = {"males by industry of employment", "females by industry of employment", "people by industry of employment", "0-4 year old dependent children by mother's proficiency in spoken English", "5-9 year old dependent children by mother's proficiency in spoken English", "10-12 year old dependent children by mother's proficiency in spoken English", "13-14 year old dependent children by mother's proficiency in spoken English", "15-17 year old dependent children by mother's proficiency in spoken English", "18-20 year old dependent children by mother's proficiency in spoken English", "21-24 year old dependent children by mother's proficiency in spoken English", "dependent children by mother's proficiency in spoken English", "0-4 year old dependent children by father's proficiency in spoken English", "5-9 year old dependent children by father's proficiency in spoken English", "10-12 year old dependent children by father's proficiency in spoken English", "13-14 year old dependent children by father's proficiency in spoken English", "15-17 year old dependent children by father's proficiency in spoken English", "18-20 year old dependent children by father's proficiency in spoken English", "21-24 year old dependent children by father's proficiency in spoken English", "dependent children by father's proficiency in spoken English", "males born overseas by year of arrival", "females born overseas by year of arrival", "people by proficiency in spoken English", "males by proficiency in spoken English", "females by proficiency in spoken English", "males by languages spoken at home", "females by languages spoken at home", "people by religious affiliation", "males by religious affiliation", "females by religious affiliation", "people by languages spoken at home", "people born overseas by year of arrival", "people by ancestry", "people by parental birthplace", "males by birthplace", "females by birthplace", "people by birthplace", "families", "dwellings", "households", "people by employment status", "males by employment status", "females by employment status", "dwellings by type", "dwellings by bedroom count", "males by age", "females by age", "people by age", "males by marital status", "females by marital status", "people by marital status", "people by gender", "people by location on census night", "males by total personal income", "females by total personal income", "people by total personal income", "males by core activity need for assistance", "females by core activity need for assistance", "people by core activity need for assistance", "males by voluntary work participation", "females by voluntary work participation", "people by voluntary work participation", "males by highest year of school completed", "females by highest year of school completed", "people by highest year of school completed", "over 15 year old males employed by method of travel to work", "over 15 year old females employed by method of travel to work", "over 15 year olds employed by method of travel to work", "males by field of study", "females by field of study", "people by field of study", "males by occupation", "females by occupation", "people by occupation", "males by unpaid domestic workload", "females by unpaid domestic workload", "people by unpaid domestic workload", "males by unpaid provision of childcare", "females by unpaid provision of childcare", "people by unpaid provision of childcare", "families by composition", "people in families by composition", "dwellings by type", "households by income", "families by income", "people by residence one year ago", "males by residence one year ago", "females by residence one year ago", "people by residence five years ago", "males by residence five years ago", "females by residence five years ago", "dwellings by weekly rental cost", "dwellings by monthly mortgage repayment", "dwellings by tenure type", "people by qualification", "males by qualification", "females by qualification", "people with a qualification by age", "males with a qualification by age", "females with a qualification by age", "people with a qualification", "males with a qualification", "females with a qualification", "females by number of children ever born", "dwellings by number of motor vehicles", "households by size", "dwellings by type of landlord", "dwellings by internet connection", "people in dwellings by type", "males by relationship within the household", "females by relationship within the household", "people by relationship within the household", "males who travel to work by train", "females who travel to work by train", "people who travel to work by train", "males who travel to work by bus", "females who travel to work by bus", "people who travel to work by bus"};

  // char *extra_category_wikinames[EXTRACATS] = {"*industry of employment", "*industry of employment", "*industry of employment", "0-4 year old dependent children", "5-9 year old dependent children", "10-12 year old depedent children", "13-14 year old dependent children", "15-17 year old dependent children", "18-20 year old dependent children", "21-24 year old dependent children", "*mother's proficiency in spoken English", "0-4 year old dependent children", "5-9 year old dependent children", "10-12 year old depedent children", "13-14 year old dependent children", "15-17 year old dependent children", "18-20 year old dependent children", "21-24 year old dependent children", "*father's proficiency in spoken English", "*year of arrival", "*year of arrival", "*proficiency in spoken English", "*proficiency in spoken English", "*proficiency in spoken English", "*languages spoken at home", "*languages spoken at home", "*religious affiliation", "*religious affiliation", "*religious affiliation", "*languages spoken at home", "*year of arrival", "*ancestry", "*parental birthplace", "*birthplace males", "*birthplace females", "*birthplace people", "families", "dwellings", "households", "*employment status", "*employment status", "*employment status", "*type", "*bedroom count", "*age", "*age", "*age", "*marital status", "*marital status", "*marital status", "*gender", "*location on census night", "*income", "*income", "*income", "*assistance", "*assistance", "*assistance", "*voluntary work", "*voluntary work", "*voluntary work", "*school completed", "*school completed", "*school completed", "*travel", "*travel", "*travel", "*field of study", "*field of study", "*field of study", "*occupation", "*occupation", "*occupation", "*domestic workload", "*domestic workload", "*domestic workload", "*childcare provision", "*childcare provision", "*childcare provision", "*composition", "people in families by composition", "*type", "*income", "*income", "*residence 1 year ago", "*residence 1 year ago", "*residence 1 year ago", "*residence 5 years ago", "*residence 5 years ago", "*residence 5 years ago", "*rental cost", "*mortgage repayment", "*tenure", "*qualification", "*qualification", "*qualification", "*age", "*age", "*age", "", "", "", "*children", "*motor vehicles", "*size", "*landlord", "*internet connection", "*dwelling type", "*relationship within the household", "*relationship within the household", "*relationship within the household", "train", "train", "train", "bus", "bus", "bus"};

  //char *extra_supercategory_names[EXTRACATS] = {"", "", "", "dependent children by mother's proficiency in spoken English", "dependent children by mother's proficiency in spoken English", "dependent children by mother's proficiency in spoken English", "dependent children by mother's proficiency in spoken English","dependent children by mother's proficiency in spoken English","dependent children by mother's proficiency in spoken English","dependent children by mother's proficiency in spoken English", "people by parental proficiency in spoken English", "dependent children by father's proficiency in spoken English", "dependent children by father's proficiency in spoken English", "dependent children by father's proficiency in spoken English", "dependent children by father's proficiency in spoken English","dependent children by father's proficiency in spoken English","dependent children by father's proficiency in spoken English","dependent children by father's proficiency in spoken English", "people by parental proficiency in spoken English", "males born overseas","females born overseas", "", "", "", "", "", "", "", "", "", "people born overseas", "", "", "", "", "", "", "", "", "", "", "", "dwellings", "dwellings", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",  "", "", "", "", "", "", "families", "families by composition", "dwellings", "households", "families", "", "", "", "", "", "", "dwellings", "dwellings", "dwellings",  "people with a qualification", "males with a qualification", "females with a qualification", "people with a qualification", "males with a qualification", "females with a qualification", "", "", "", "females", "dwellings", "households", "dwellings", "dwellings", "dwellings by type", "", "", "", "over 15 year old males employed by method of travel to work", "over 15 year old females employed by method of travel to work", "over 15 year olds employed by method of travel to work", "over 15 year old males employed by method of travel to work", "over 15 year old females employed by method of travel to work", "over 15 year olds employed by method of travel to work"};

  char *extra_category_names[EXTRACATS] = {"males who travel to work by car", "females who travel to work by car", "people who travel to work by car"};
  char *extra_category_wikinames[EXTRACATS] = {"car", "car", "car"};
  char *extra_supercategory_names[EXTRACATS] = {"over 15 year old males employed by method of travel to work", "over 15 year old females employed by method of travel to work", "over 15 year olds employed by method of travel to work"};

  for (i=0; i<EXTRACATS; i++) {
    // fprintf(stderr, "%d\t%s\t%s\t%s\n", i, extra_supercategory_names[i], extra_category_wikinames[i], extra_category_names[i]);
  }


  /* initialize */
  for (field=0; field<MAXFIELDS; field++) {
    field_generalcategory_names[field][0] = '\0';
    for (sc = 0; sc<MAXCATS; sc++) {
      field_supercategory_names[field][sc][0] = '\0';
    }
  } 
  

  /**********  start TABLE B01 - complete ***********/
  char *B01_rows[36]= {"", "0-4 year old", "5-14 year old", "15-19 year old", "20-24 year old", "25-34 year old", "35-44 year old", "45-54 year old", "55-64 year old", "65-74 year old", "75-84 year old", "over 85 year old", "counted at home on census night", "counted elsewhere in Australia on census night", "Aboriginal", "Torres Strait Islander", "Aboriginal and Torres Strait Islander", "Indigenous", "born in Australia", "not born in Australia", "who only speak English at home", "who speak a language other than English at home", "who are Australian citizens", "0-4 year old", "5-14 year old", "15-19 year old", "20-24 year old", "over 25 year old", "whose highest year of school completed is year 12", "whose highest year of school completed is year 11", "whose highest year of school completed is year 10", "whose highest year of school completed is year 9", "whose highest year of school completed is year 8 or below", "who did not go to school", "in occupied private dwellings", "not in occupied private dwellings"};
  char *B01_cols[3] = {"males", "females", "people"};
  char *B01_cols_replacement[3] = {" males", " females", "s"};
  field=1;
  for (row=0; row<36; row++) {
    for (col=0; col<3; col++) {
      sprintf(field_category_names[field], "%s %s", B01_rows[row], B01_cols[col]);
      sprintf(field_category_names[1], "males");
      sprintf(field_category_names[2], "females");
      sprintf(field_category_names[3], "people");
      if (row>0 && row<=11) sprintf(field_category_names[field], "%s%s", B01_rows[row], B01_cols_replacement[col]);
      if (row==12 || row==13 || row==18 || row==19 || row==20 || row==21 || row==22 || (row>=28) ) sprintf(field_category_names[field], "%s %s", B01_cols[col], B01_rows[row]);
      if (row>=23 && row<=27) sprintf(field_category_names[field], "%s%s attending an educational institution", B01_rows[row], B01_cols_replacement[col]);
      field++;
    }
  }
  field=1;
  for (row=0; row<36; row++) {
    for (col=0; col<3; col++) {
      if (row != 0) { 
	sprintf(field_supercategory_names[field][0], "%s", B01_cols[col]);
	if (col!=2) 
	  sprintf(field_supercategory_names[field][1], "%s", field_category_names[field+2-col]);
      } else {
	if (col!=2)
	  sprintf(field_supercategory_names[field][0], "%s", field_category_names[field+2-col]);
      }
      field++;
    } 
  }


  /**********  start TABLE B02 - complete ***********/
  sprintf(field_category_names[109], "median age");
  sprintf(field_category_names[110], "median monthly mortgage repayment");
  sprintf(field_category_names[111], "median weekly total personal income");
  sprintf(field_category_names[112], "median weekly rent");
  sprintf(field_category_names[113], "median weekly total family income");
  sprintf(field_category_names[114], "average people per bedroom");
  sprintf(field_category_names[115], "median weekly total household income");
  sprintf(field_category_names[116], "average household size");

  sprintf(field_supercategory_names[109][0], "age");
  sprintf(field_supercategory_names[109][1], "people");
  sprintf(field_supercategory_names[110][0], "finances");
  sprintf(field_supercategory_names[110][1], "households");
  sprintf(field_supercategory_names[111][0], "income");
  sprintf(field_supercategory_names[111][1], "people");
  sprintf(field_supercategory_names[112][0], "dwellings");
  sprintf(field_supercategory_names[112][1], "finances");
  sprintf(field_supercategory_names[113][0], "income");
  sprintf(field_supercategory_names[113][1], "families");
  sprintf(field_supercategory_names[114][0], "people");
  sprintf(field_supercategory_names[114][1], "dwellings");
  sprintf(field_supercategory_names[115][0], "households");
  sprintf(field_supercategory_names[115][1], "income");
  sprintf(field_supercategory_names[116][0], "households");
  sprintf(field_supercategory_names[116][1], "people");



  /**********  start TABLE B03 - complete ***********/
  char *B03_cols[10] = {"0-14 year olds", "15-24 year olds", "25-34 year olds", "35-44 year olds", "45-54 year olds", "55-64 year olds", "65-74 year olds", "75-84 year olds", "over 85 year olds", "people"};
  char *B03_rows[14] = {" counted at home on census night", " visiting from somewhere in the same statistical area (level 2)", " visiting from New South Wales", " visiting from Victoria", " visiting from Queensland", " visiting from South Australia", " visiting from Western Australia", " visiting from Tasmania", " visiting from the Northern Territory", " visiting from the Australian Capital Territory", " visiting from other Territories", " visiting from another statistical area (level 2)", " visiting this residence on census night", ""};
  field=117;
  for (row=0; row<14; row++) {
    for (col=0; col<10; col++) {
      sprintf(field_category_names[field], "%s%s", B03_cols[col], B03_rows[row]);
      field++;
    }
  }
  field=117;
  for (row=0; row<14; row++) {
    for (col=0; col<10; col++) {
      if (row != 13) { 
	sprintf(field_supercategory_names[field][0], "%s", B03_cols[col]);
	if (col!=9) 
	  sprintf(field_supercategory_names[field][1], "%s", field_category_names[field+9-col]);
	else 
	  sprintf(field_supercategory_names[field][0], "people by location on census night"); // replace "people" supercat
      } else {
	if (col!=9)
	  sprintf(field_supercategory_names[field][0], "people by age");
	//sprintf(field_supercategory_names[field][0], "%s", field_category_names[field+9-col]);
      }
      field++;
    } 
  }



  /**********  start TABLE B04 - incomplete ***********/
  char *B04_cols[3] = {" males", " females", "s"};
  char *B04_rows[102] = {"0","1","2","3","4","0-4","5","6","7","8","9","5-9","10","11","12","13","14","10-14","15","16","17","18","19","15-19","20","21","22","23","24","20-24","25","26","27","28","29","25-29","30","31","32","33","34","30-34","35","36","37","38","39","35-39","40","41","42","43","44","40-44","45","46","47","48","49","45-49","50","51","52","53","54","50-54","55","56","57","58","59","55-59","60","61","62","63","64","60-64","65","66","67","68","69","65-69","70","71","72","73","74","70-74","75","76","77","78","79","75-79","80-84","85-89","90-94","95-99","over 100",""};
  char *B04_cols_replacement[3] = {"males", "females", "people"};
  field=257;
  for (row=0; row<101; row++) {
    for (col=0; col<3; col++) {
      sprintf(field_category_names[field], "%s year old%s", B04_rows[row], B04_cols[col]);
      field++;
    }
  }
  sprintf(field_category_names[560], "males");
  sprintf(field_category_names[561], "females");
  sprintf(field_category_names[562], "people");

  field=257;
  for (row=0; row<102; row++) {
    for (col=0; col<3; col++) {
      if (row != 101) { 
	sprintf(field_supercategory_names[field][0], "%s", B04_cols_replacement[col]);
	if (col!=2) 
	  sprintf(field_supercategory_names[field][1], "%s", field_category_names[field+2-col]);
	else 
	  sprintf(field_supercategory_names[field][0], "people by age"); // replace "people" supercat
      } else {
	if (col!=2)
	  sprintf(field_supercategory_names[field][0], "people by gender");
	//sprintf(field_supercategory_names[field][0], "%s", field_category_names[field+2-col]);
      }
      field++;
    } 
  }






  /**********  start TABLE B05 - complete ***********/
  char *B05_cols[6] = {"married ", "separated ", "divorced ", "widowed ", "never married ", ""};
  char *B05_rows[10] = {"15-19", "20-24", "25-34", "35-44", "45-54", "55-64", "65-74", "75-84", "over 85", "over 15"};
  char *B05_slabs[3] = {" males", " females", "s"};
  char *B05_slabs_replacement[3] = {"males", "females", "people"};
 
  field=563;
  for (slab=0; slab<3; slab++) {
    for (row=0; row<10; row++) {
      for (col=0; col<6; col++) {
	sprintf(field_category_names[field], "%s%s year old%s", B05_cols[col], B05_rows[row], B05_slabs[slab]);
	//if (row==9) sprintf(field_category_names[field], "%s%s%s", B05_cols[col], B05_rows[row], B05_slabs_replacement[slab]);
	//if ((slab==2) && (row==9)) sprintf(field_category_names[field], "%speople", B05_cols[col]);
	field++;
      }
    }
  }

  field=563;
  for (slab=0; slab<3; slab++) {
    for (row=0; row<10; row++) {
      for (col=0; col<6; col++) {
	sc=0;
	if (col!=5) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+5-col]);
	  sc++;
	} else {
	  // could take this out of the else if you want to break down by marital status by age, here just age-genders by age
	  sprintf(field_supercategory_names[field][sc], "%s by age", B05_slabs_replacement[slab]);
	  sc++;
	}
	if (slab!=2) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(2-slab)*10*6]);
	  sc++;
	} 
	// last because "over 15 year olds" is a rubbish sortkey
	if (row!=9) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(9-row)*6]);
	  sc++;
	} else {
	  if (col!=5) {
	    sprintf(field_supercategory_names[field][sc], "%s by marital status", B05_slabs_replacement[slab]);	 
	    sc++;
	  }
	}

	field++;
      } 
    }
  }




  /**********  start TABLE B06 - complete ***********/
  char *B06_cols[4] = {" married in a registered marriage", " married in a de facto marriage", " not currently married", ""};
  char *B06_rows[10] = {"15-19 year old", "20-24 year old", "25-34 year old", "35-44 year old", "45-54 year old", "55-64 year old", "65-74 year old", "75-84 year old", "over 85 year old", "over 15 year old"};
  char *B06_slabs[3] = {" males", " females", "s"};
  char *B06_slabs_replacement[3] = {"males", "females", "people"};
 
  field=743;
  for (slab=0; slab<3; slab++) {
    for (row=0; row<10; row++) {
      for (col=0; col<4; col++) {
	sprintf(field_category_names[field], "%s%s%s", B06_rows[row], B06_slabs[slab], B06_cols[col]);
	/*
	if (row==9) {
	  switch (slab) {
	  case 0:
	    sprintf(field_category_names[field], "%s%s", "males", B06_cols[col]);
	    break;
	  case 1:
	    sprintf(field_category_names[field], "%s%s", "females", B06_cols[col]);
	    break;
	  case 2:
	    sprintf(field_category_names[field], "%s%s", "people", B06_cols[col]);
	    break;
	  }
	}
	*/
	field++;
      }
    }
  }

  field=743;
  for (slab=0; slab<3; slab++) {
    for (row=0; row<10; row++) {
      for (col=0; col<4; col++) {
	sc=0;
	if (col!=3) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+3-col]);
	  sc++;
	} else {
	  sprintf(field_supercategory_names[field][sc], "%s by age", B06_slabs_replacement[slab]);
	  sc++;
	}
	if (slab!=2) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(2-slab)*10*4]);
	  sc++;
	} 
	if (row!=9) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(9-row)*4]);
	  sc++;
	} else {
	  if (col!=3) {
	    sprintf(field_supercategory_names[field][sc], "%s by marital status", B06_slabs_replacement[slab]);	 
	    sc++;
	  }
	}

	field++;
      } 
    }
  }





  /**********  start TABLE B07 - complete ***********/
  char *B07_cols[3] = {" males", " females", "s"};
  char *B07_cols_replacement[3] = {"males", "females", "people"};
  char *B07_rows[15] = {"0-4 year old", "5-9 year old", "10-14 year old", "15-19 year old", "20-24 year old", "25-29 year old", "30-34 year old", "35-39 year old", "40-44 year old", "45-49 year old", "50-54 year old", "55-59 year old", "60-64 year old", "over 65 year old", ""};
  char *B07_slabs[4] = {"indigenous ", "non-indigenous ", " who did not state their indigenous status", ""};
 
  field=863;
  for (row=0; row<15; row++) {
    for (slab=0; slab<4; slab++) {
      for (col=0; col<3; col++) {
	sprintf(field_category_names[field], "%s%s%s",  B07_slabs[slab], B07_rows[row], B07_cols[col]);
	if (slab==2) sprintf(field_category_names[field], "%s%s%s", B07_rows[row], B07_cols[col], B07_slabs[slab]);
	if (row==14) {
	  sprintf(field_category_names[field], "%s%s%s",  B07_slabs[slab], B07_rows[row], B07_cols_replacement[col]);
	  if (slab==2) sprintf(field_category_names[field], "%s%s%s", B07_rows[row], B07_cols_replacement[col], B07_slabs[slab]);
	}
	field++;
      }   
    }
  }



  field=863;
  for (row=0; row<15; row++) {
    for (slab=0; slab<4; slab++) {
      for (col=0; col<3; col++) {
	sc=0;
	if (col!=2) {
	  if ((row!=14)||(slab!=3)) {
	    sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+2-col]);
	    sc++;
	  } else {
	    sprintf(field_supercategory_names[field][sc], "%s by gender", field_category_names[field+2-col]);
	    sc++;
	  }
	}
	if (slab!=3) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(3-slab)*3]);
	  sc++;
	} 
	if (row!=14) {
	  if (slab!=3) { 
	    sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(14-row)*3*4]);
	    sc++;
	  } else {
	    sprintf(field_supercategory_names[field][sc], "%s by age", field_category_names[field+(14-row)*3*4]);
	    sc++;
	  }
	} else {
	  if (slab!=3) {
	    sprintf(field_supercategory_names[field][sc], "%s by indigenous status", B07_cols_replacement[col]);	 
	    sc++;
	  }
	}

	field++;
      } 
    }
  }




  /**********  start TABLE B08 - complete ***********/
  char *B08_rows[34] = {"Australian", "Australian Aboriginal", "Chinese", "Croatian", "Dutch", "English", "Filipino", "French", "German", "Greek", "Hungarian", "Indian", "Irish", "Italian", "Korean", "Lebanese", "Macedonian", "Maltese", "Maori", "New Zealander", "Polish", "Russian", "Scottish", "Serbian", "Sinhalese", "South African", "Spanish", "Turkish", "Vietnamese", "Welsh", "other", "unstated", "total ancestry responses from people", "people"};
  char *B08_cols[6] = {" whose parents were both born overseas", " whose father was born overseas and mother was born in Australia", " whose mother was born overseas and father was born in Australia", " whose parents were both born in Australia", " whose parental birthplace was not stated", ""};
  field=1043;
  for (row=0; row<32; row++) {
    for (col=0; col<6; col++) {
      sprintf(field_category_names[field], "people of %s ancestry%s", B08_rows[row], B08_cols[col]);
      field++;
    }
  }
  for (row=32; row<=33; row++) {
    for (col=0; col<6; col++) {
      sprintf(field_category_names[field], "%s%s", B08_rows[row], B08_cols[col]);
      field++;
    }
  }



  field=1043;
  for (row=0; row<34; row++) {
    for (col=0; col<6; col++) {
      if (row != 33) { 
	sprintf(field_supercategory_names[field][0], "%s", field_category_names[field+(33-row)*6]);
	if (col!=5) 
	  sprintf(field_supercategory_names[field][1], "%s", field_category_names[field+5-col]);
	else 
	  sprintf(field_supercategory_names[field][0], "people by ancestry");
      } else {
	if (col!=5)
	  sprintf(field_supercategory_names[field][0], "people by parental birthplace");
      }
      field++;
    } 
  }




  /**********  start TABLE B09 - complete ***********/
  char *B09_rows[38] = {"Australia", "Bosnia and Herzegovina", "Cambodia", "Canada", "China excluding Hong Kong, Macau and Taiwan", "Croatia", "Egypt", "Fiji", "the Former Yugoslav Republic of Macedonia", "Germany", "Greece", "Hong Kong", "India", "Indonesia", "Iraq", "Ireland", "Italy", "Japan", "South Korea", "Lebanon", "Malaysia", "Malta", "the Netherlands", "New Zealand", "the Philippines", "Poland", "Singapore", "South Africa", "South Eastern Europe", "Sri Lanka", "Thailand", "Turkey", "the United Kingdom, Channel Islands and Isle of Mann", "the United States of America", "Vietnam", "a country not listed individually", "not stated", ""};
  char *B09_cols[3] = {"males", "females", "people"};
  field=1247;
  for (row=0; row<38; row++) {
    for (col=0; col<3; col++) {
      sprintf(field_category_names[field], "%s born in %s", B09_cols[col], B09_rows[row]);
      if (row==36) sprintf(field_category_names[field], "%s who did not state their country of birth", B09_cols[col]);
      if (row==37) sprintf(field_category_names[field], "%s", B09_cols[col]);
      field++;
    }
  }



  field=1247;
  for (row=0; row<38; row++) {
    for (col=0; col<3; col++) {
      if (row != 37) { 
	sprintf(field_supercategory_names[field][0], "%s by birthplace", B09_cols[col]);
	if (col!=2) 
	  sprintf(field_supercategory_names[field][1], "%s", field_category_names[field+2-col]);
	else 
	  sprintf(field_supercategory_names[field][0], "people by birthplace");
      } else {
	if (col!=2)
	  sprintf(field_supercategory_names[field][0], "people by gender");
      }
      field++;
    }
  }



  /**********  start TABLE B10 - complete ***********/
  char *B10_rows[36] = {"Bosnia and Herzegovina", "Cambodia", "Canada", "China excluding Hong Kong, Macau and Taiwan", "Croatia", "Egypt", "Fiji", "the Former Yugoslav Republic of Macedonia", "Germany", "Greece", "Hong Kong", "India", "Indonesia", "Iraq", "Ireland", "Italy", "Japan", "South Korea", "Lebanon", "Malaysia", "Malta", "the Netherlands", "New Zealand", "the Philippines", "Poland", "Singapore", "South Africa", "South Eastern Europe", "Sri Lanka", "Thailand", "Turkey", "the United Kingdom, Channel Islands and Isle of Mann", "the United States of America", "Vietnam", "a country not listed individually", "total"};
  char *B10_cols[16] = {"before 1941", "during 1941-1950", "during 1951-1960", "during 1961-1970", "during 1971-1980", "during 1981-1990", "during 1991-2000", "during 2001-2005", "during 2006", "during 2007", "during 2008", "during 2009", "during 2010", "during 2011, prior to the census", "not stated", "total"};
  field=1361;
  for (row=0; row<36; row++) {
    for (col=0; col<16; col++) {
      sprintf(field_category_names[field], "people born in %s who arrived in Australia %s", B10_rows[row], B10_cols[col]);
      if (row==35) sprintf(field_category_names[field], "people who arrived in Australia %s", B10_cols[col]);
      if (col==15) sprintf(field_category_names[field], "people born in %s", B10_rows[row]);
      if ((row==35) && (col==15)) sprintf(field_category_names[field], "people born overseas", B10_rows[row]);
      field++;
    }
  }



  field=1361;
  for (row=0; row<36; row++) {
    for (col=0; col<16; col++) {
      sc=0;
      if (row != 35) { 
	sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(35-row)*16]);
	sc++;
	if (col!=15) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+15-col]);
	  sc++;
	} else { 
	  sprintf(field_supercategory_names[field][sc], "people by birthplace");
	  sc++;
	}
      } else {
	if (col!=15) {
	  sprintf(field_supercategory_names[field][0], "people born overseas by year of arrival");
	  sc++;
	}
      }
      field++;
    }
  }



  /**********  start TABLE B11 - complete ***********/
  char *B11_rows[7] = {" who speak English only", " who speak another language and speak English well", " who speak another language and speak English not well or not at all", " who speak another language and did not state proficiency in English", " who speak English and another language", " who did not state proficiency in English", ""};
  char *B11_rows_super[7] = {"", " who speak English and another language", " who speak English and another language", " who speak English and another language", "", "", ""};
  char *B11_cols[11] = {" who arrived in Australia before 1996", " who arrived in Australia during 1996-2000", " who arrived in Australia during 2001-2005", " who arrived in Australia during 2006", " who arrived in Australia during 2007", " who arrived in Australia during 2008", " who arrived in Australia during 2009", " who arrived in Australia during 2010", " who arrived in Australia during 2011, prior to the census", " who did not state when they arrived in Australia", " born overseas"};
  char *B11_slabs[3] = {"males", "females", "people"};
  char *B11_slabs_replacement[3] = {"males", "females", "people"};
  field=1937;
  for (slab=0; slab<3; slab++) {
    for (row=0; row<7; row++) {
      for (col=0; col<11; col++) {
	sprintf(field_category_names[field], "%s%s%s", B11_slabs[slab], B11_cols[col], B11_rows[row]);
	field++;
      }
    }
  }



  field=1937;
  for (slab=0; slab<3; slab++) {
    for (row=0; row<7; row++) {
      for (col=0; col<11; col++) {
	sc=0;
	if (col!=10) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+10-col]);
	  sc++;
	} else {
	  if (row!=6) { 
	    sprintf(field_supercategory_names[field][sc], "%s by proficiency in spoken English", B11_slabs[slab]);
	    sc++;
	  }
	}
	if (slab!=2) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(2-slab)*11*7]);
	  sc++;
	} 
	if (row!=6) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(6-row)*11]);
	  sc++;
	  
	  if (strlen(B11_rows_super[row])!=0) {
	    sprintf(field_supercategory_names[field][sc], "%s%s%s", B11_slabs[slab], B11_cols[col], B11_rows_super[row]);
	    sc++;
	  }

	} else {
	  if (col!=10) {
	    sprintf(field_supercategory_names[field][sc], "%s born overseas by year of arrival", B11_slabs_replacement[slab]);	 
	    sc++;
	  }
	}

	field++;
      } 
    }
  }




  /**********  start TABLE B12 - incomplete **********/
  char *B12_cols[7] = {" whose fathers speak English only", " whose fathers speak another language and speak English well", " whose fathers speak another language and speak English not well or not at all", " whose fathers speak another language and did not state proficiency in English", " whose fathers speak English and another language", " whose fathers did not state proficiency in English", ""};
  char *B12_cols_super[7] = {"", " whose fathers speak English and another language", " whose fathers speak English and another language", " whose fathers speak English and another language", "", "", ""};
  char *B12_rows[7] = {" whose mothers speak English only", " whose mothers speak another language and speak English well", " whose mothers speak another language and speak English not well or not at all", " whose mothers speak another language and did not state proficiency in English", " whose mothers speak English and another language", " whose mothers did not state proficiency in English", ""};
  char *B12_rows_super[7] = {"", " whose mothers speak English and another language", " whose mothers speak English and another language", " whose mothers speak English and another language", "", "", ""};
  char *B12_slabs[8] = {"0-4 year old dependent children","5-9 year old dependent children","10-12 year old dependent children","13-14 year old dependent children", "15-17 year old dependent children", "18-20 year old dependent children", "21-24 year old dependent children", "dependent children"};
  field=2168;
  for (slab=0; slab<8; slab++) {
    for (row=0; row<7; row++) {
      for (col=0; col<7; col++) {
	if ((row==6)||(col==6)) {
	  sprintf(field_category_names[field], "%s%s%s", B12_slabs[slab], B12_cols[col], B12_rows[row]);
	} else {
	  sprintf(field_category_names[field], "%s%s and%s", B12_slabs[slab], B12_cols[col], B12_rows[row]);
	}
	field++;
      }
    }
  }



  field=2168;
  for (slab=0; slab<8; slab++) {
    for (row=0; row<7; row++) {
      for (col=0; col<7; col++) {
	sc=0;
	if (col!=6) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+6-col]);
	  sc++;

	  if (strlen(B12_cols_super[col])!=0) {
	    if (row==6) {
	      sprintf(field_supercategory_names[field][sc], "%s%s%s", B12_slabs[slab], B12_cols_super[col], B12_rows[row]);
	      sc++;
	    } else {
	      sprintf(field_supercategory_names[field][sc], "%s%s and%s", B12_slabs[slab], B12_cols_super[col], B12_rows[row]);
	      sc++;
	    }
	  }

	} else {
	  if (row!=6) { 
	    sprintf(field_supercategory_names[field][sc], "%s by mother's proficiency in spoken English", B12_slabs[slab]);
	    sc++;
	  }
	}
	if (slab!=7) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(7-slab)*7*7]);
	  sc++;
	} 
	if (row!=6) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(6-row)*7]);
	  sc++;
	  
	  if (strlen(B12_rows_super[row])!=0) {
	    if (col==6) {
	      sprintf(field_supercategory_names[field][sc], "%s%s%s", B12_slabs[slab], B12_cols[col], B12_rows_super[row]);
	      sc++;
	    } else { 
	      sprintf(field_supercategory_names[field][sc], "%s%s and%s", B12_slabs[slab], B12_cols[col], B12_rows_super[row]);
	      sc++;
	    }
	  }

	} else {
	  if (col!=6) {
	    sprintf(field_supercategory_names[field][sc], "%s by father's proficiency in spoken English", B12_slabs[slab]);	 
	    sc++;
	  }
	}

	field++;
      } 
    }
  }



  
  /**********  start TABLE B13 - complete ***********/
  char *B13_rows[50] = {"English", "Arabic", "Assyrian", "Australian Indigenous Languages", "Cantonese", "Mandarin", "another Chinese language", "a Chinese language", "Croatian", "Dutch", "French", "German", "Greek", "Hungarian", "Bengali", "Hindi", "Punjabi", "Sinhalese", "Urdu", "another Indo-Aryan language", "an Indo-Aryan language", "Dari", "Persian (excluding Dari)", "another Iranic language", "an Iranic language", "Italian", "Japanese", "Khmer", "Korean", "Macedonian", "Maltese", "Polish", "Portugese", "Russian", "Samoan", "Serbian", "Filipino", "Indonesian", "Tagalog", "another Southeast Asian or Austronesian language", "a Southeast Asian or Austronesian language", "Spanish", "Tamil", "Thai", "Turkish", "Vietnamese", "a language not listed individually", "a language other than English", "", ""};
  char *B13_rows_super[50] = {"", "a language other than English", "a language other than English", "a language other than English", "a Chinese language", "a Chinese language", "a Chinese language", "a language other than English", "a language other than English", "a language other than English", "a language other than English", "a language other than English", "a language other than English", "a language other than English", "an Indo-Aryan language", "an Indo-Aryan language", "an Indo-Aryan language", "an Indo-Aryan language", "an Indo-Aryan language", "an Indo-Aryan language", "a language other than English", "an Iranic language", "an Iranic language", "an Iranic language", "a language other than English", "a language other than English", "a language other than English", "a language other than English", "a language other than English", "a language other than English", "a language other than English", "a language other than English", "a language other than English", "a language other than English", "a language other than English", "a language other than English", "a Southeast Asian or Austronesian language", "a Southeast Asian or Austronesian language", "a Southeast Asian or Austronesian language", "a Southeast Asian or Austronesian language", "a language other than English", "a language other than English", "a language other than English", "a language other than English", "a language other than English", "a language other than English", "a language other than English", "", "", ""};
  char *B13_cols[3] = {"males", "females", "people"};
  field=2560;
  for (row=0; row<50; row++) {
    for (col=0; col<3; col++) {
      sprintf(field_category_names[field], "%s who speak %s at home", B13_cols[col], B13_rows[row]);
      if (row==48) {
	sprintf(field_category_names[field], "%s whose language spoken at home was not stated", B13_cols[col]);
      } else if (row==49) {
	sprintf(field_category_names[field], "%s", B13_cols[col]);
      }
      field++;
    }
  }



  field=2560;
  for (row=0; row<50; row++) {
    for (col=0; col<3; col++) {
      sc=0;
      if (row != 49) { 

	sprintf(field_supercategory_names[field][sc], "%s by languages spoken at home", field_category_names[field+(49-row)*3]);
	sc++;
	if (col!=2) { 
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+2-col]);
	  sc++;
	} 

	if (strlen(B13_rows_super[row])!=0) {
	  sprintf(field_supercategory_names[field][sc], "%s who speak %s at home", B13_cols[col], B13_rows_super[row]);
	  sc++;
	}

      } else {
	if (col!=2) {
	  sprintf(field_supercategory_names[field][sc], "people by gender");
	  sc++;
	}
      }
      field++;
    } 
  }



  
  /**********  start TABLE B14 - complete ***********/
  char *B14_rows[31]= {"Buddhism", "Anglican", "Assyrian Apostolic", "Baptist", "Bretheren", "Catholic", "Churches of Christ", "Eastern Orthodox", "Jehovah's Witnesses", "Latter-day Saints", "Lutheran", "Oriental Orthodox", "other Protestant", "Pentecostal", "Presbyterian and Reformed", "Salvation Army", "Seventh-day Adventist", "Uniting Church", "Apostolic Church, Church of God, Australian Christian Churches, or New Church Alliance", "other Christian", "Christian", "Hinduism", "Islam", "Judaism", "Australian Aboriginal Traditional Religions", "other religious groups", "other religions", "no religion, agnosticism, atheism, humanism, or rationalism", "undefined religious belief, New Age, or Theism", "did not state a religious affiliation", ""};
  char *B14_rows_super[31]= {"", "Christian", "Christian", "Christian", "Christian", "Christian", "Christian", "Christian", "Christian", "Christian", "Christian", "Christian", "Christian", "Christian", "Christian", "Christian", "Christian", "Christian", "Christian", "Christian", "", "", "", "", "other religions", "other religions", "", "", "", "", ""};
  char *B14_cols[3] = {"males", "females", "people"};
  field=2710;
  for (row=0; row<31; row++) {
    for (col=0; col<3; col++) {
      sprintf(field_category_names[field], "%s who identify as %s", B14_cols[col], B14_rows[row]);
      if ((row==0)||(row==21)||(row==22)||(row==23)||(row==24)||(row==25)||(row==26)||(row==27)||(row==28)) {
	sprintf(field_category_names[field], "%s who are affiliated with %s", B14_cols[col], B14_rows[row]);
      } 
     if (row==29) {
	sprintf(field_category_names[field], "%s who %s", B14_cols[col], B14_rows[row]);
      }
      if (row==30) {
	sprintf(field_category_names[field], "%s", B14_cols[col]);
      }
      field++;
    }
  }


  field=2710;
  for (row=0; row<31; row++) {
    for (col=0; col<3; col++) {
      sc=0;
      if (row != 30) { 

	sprintf(field_supercategory_names[field][sc], "%s by religious affiliation", field_category_names[field+(30-row)*3]);
	sc++;
	if (col!=2) { 
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+2-col]);
	  sc++;
	} 

	if (strlen(B14_rows_super[row])!=0) {
	  sprintf(field_supercategory_names[field][sc], "%s who identify as %s", B14_cols[col], B14_rows_super[row]);
	  if ((row==0)||(row==21)||(row==22)||(row==23)||(row==24)||(row==25)||(row==26)||(row==27)||(row==28)) {
	    sprintf(field_supercategory_names[field][sc], "%s who are affiliated with %s", B14_cols[col], B14_rows_super[row]);
	  }
	  sc++;
	}

      } else {
	if (col!=2) {
	  sprintf(field_supercategory_names[field][sc], "people by gender");
	  sc++;
	}
      }
      field++;
    } 
  }




  
  /**********  start TABLE B15 - complete ***********/
  char *B15_rows[27]= {"pre-school", "a government infants or primary school", "a Catholic infants or primary school", "another non-government infants or primary school", "an infants or primary school", "a government secondary school", "a Catholic secondary school", "another non-governmnet secondary school", "secondary school", "a technical or further educational institution (full time, 15-24 years old)", "a technical or further educational institution (full time, over 25 years old)", "a technical or further educational institution (part time, 15-24 years old)", "a technical or further educational institution (part time, over 25 years old)", "a technical or further educational institution (full or part time status not stated)", "a technical or further educational institution", "university or other tertiary institutions (full time, 15-24 years old)", "university or other tertiary institutions (full time, over 25 years old)", "university or other tertiary institutions (part time, 15-24 years old)", "university or other tertiary institutions (part time, over 25 years)", "university or other tertiary institutions (full or part time status not stated)", "university or other tertiary institutions", "another type of educational institution (full time)", "another type of educational institution (part time)", "another type of educational institution (full or part time status not stated)", "another type of educational institution", "an educational institution where the type was not stated", "an educational institution"};
  char *B15_rows_super[27]= {"", "an infants or primary school", "an infants or primary school", "an infants or primary school", "", "secondary school", "secondary school", "secondary school", "", "a technical or further educational institution", "a technical or further educational institution", "a technical or further educational institution", "a technical or further educational institution", "a technical or further educational institution", "", "university or other tertiary institutions", "university or other tertiary institutions", "university or other tertiary institutions", "university or other tertiary institutions", "university or other tertiary institutions", "", "another type of educational institution", "another type of educational institution", "another type of educational institution", "", "", ""};
  char *B15_cols[3] = {"males", "females", "people"};
  field=2803;
  for (row=0; row<27; row++) {
    for (col=0; col<3; col++) {
      sprintf(field_category_names[field], "%s attending %s", B15_cols[col], B15_rows[row]);
      field++;
    }
  }

  field=2803;
  for (row=0; row<27; row++) {
    for (col=0; col<3; col++) {
      sc=0;
      if (row!=26) {
	if (strlen(B15_rows_super[row])!=0) {
	  sprintf(field_supercategory_names[field][sc], "%s attending %s", B15_cols[col], B15_rows_super[row]);
	  sc++;
	} else {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(26-row)*3]);
	  sc++;
	}
      }
      
      if (col!=2) {
	sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+2-col]);
	sc++;
      }
      
      field++;
    }
  }
  


  /**********  start TABLE B16 - complete ***********/
  char *B16_rows[8] = {"year 12", "year 11", "year 10", "year 9", "year 8 or below", "did not go to school", "not stated", "are no longer attending primary or secondary school"};
  char *B16_cols[10] = {"out of school 15-19 year old", "20-24 year old", "25-34 year old", "35-44 year old", "45-54 year old", "55-64 year old", "65-74 year old", "75-84 year old", "over 85 year old", ""};
  char *B16_cols_replacements[10] = {"15-19 year old", "20-24 year old", "25-34 year old", "35-44 year old", "45-54 year old", "55-64 year old", "65-74 year old", "75-84 year old", "over 85 year old", ""};
  char *B16_slabs[3] = {" males", " females", "s"};
  char *B16_slabs_replacement[3] = {"males", "females", "people"};
 
  field=2884;
  for (slab=0; slab<3; slab++) {
    for (row=0; row<8; row++) {
      for (col=0; col<10; col++) {
	sprintf(field_category_names[field], "%s%s whose highest year of school completed is %s", B16_cols[col], B16_slabs[slab], B16_rows[row]);
	if ((row==5)||(row==7)) {
	  sprintf(field_category_names[field], "%s%s who %s", B16_cols[col], B16_slabs[slab], B16_rows[row]);
	  if ((row==7)&&(col==0)) sprintf(field_category_names[field], "%s%s", B16_cols[col], B16_slabs[slab]);

	}
	if ((row==7)&&(!((col==0)||(col==9)))) {
	    sprintf(field_category_names[field], "%s%s", B16_cols[col], B16_slabs[slab]);
	}

	if (col==9) {
	  sprintf(field_category_names[field], "%s%s whose highest year of school completed is %s", B16_cols[col], B16_slabs_replacement[slab], B16_rows[row]);
	  if ((row==5)||(row==7)) sprintf(field_category_names[field], "%s%s who %s", B16_cols[col], B16_slabs_replacement[slab], B16_rows[row]);
	  if ((row==7)&&(!((col==0)||(col==9)))) sprintf(field_category_names[field], "%s", B16_slabs_replacement[slab]);
	}

	field++;
      }
    }
  }


  field=2884;
  for (slab=0; slab<3; slab++) {
    for (row=0; row<8; row++) {
      for (col=0; col<10; col++) {
	sc=0;
	
	if (col!=9) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+9-col]);
	  sc++;
	} else {
	  if (row!=7) { 
	    sprintf(field_supercategory_names[field][sc], "%s by highest year of school completed", B16_slabs_replacement[slab]);
	    sc++;
	  } else {
	    sprintf(field_supercategory_names[field][sc], "%s", B16_slabs_replacement[slab]);	 
	    sc++;
	  }
	}
	if (slab!=2) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(2-slab)*10*8]);
	  sc++;
	} 
	if (row!=7) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(7-row)*10]);
	  sc++;
	} else {
	  if (col!=9) {
	    sprintf(field_supercategory_names[field][sc], "%s%s", B16_cols_replacements[col], B16_slabs[slab]);	 
	    sc++;
	  }
	}

	field++;
      }
    }
  }



  /**********  start TABLE B17 - complete ***********/
  char *B17_rows[13] = {"negative or nil", "in the range $1-$199", "in the range $200-$299", "in the range $300-$399", "in the range $400-$599", "in the range $600-$799", "in the range $800-$999", "in the range $1000-$1249", "in the range $1250-$1499", "in the range $1500-$1999", "$2000 or more", "not stated", ""};
  char *B17_cols[10] = {"15-19 year old", "20-24 year old", "25-34 year old", "35-44 year old", "45-54 year old", "55-64 year old", "65-74 year old", "75-84 year old", "over 85 year old", "over 15 year old"};
  char *B17_slabs[3] = {" males", " females", "s"};
  char *B17_slabs_replacement[3] = {"males", "females", "people"};
 
  field=3124;
  for (slab=0; slab<3; slab++) {
    for (row=0; row<13; row++) {
      for (col=0; col<10; col++) {
	sprintf(field_category_names[field], "%s%s with a personal weekly income %s", B17_cols[col], B17_slabs[slab], B17_rows[row]);

	if (row==12) {
	  sprintf(field_category_names[field], "%s%s", B17_cols[col], B17_slabs[slab]);
	}
	
	/*
	if (col==9) {
	  sprintf(field_category_names[field], "%s%s with a personal weekly income %s", B17_cols[col], B17_slabs_replacement[slab], B17_rows[row]);
	  if (row==12) sprintf(field_category_names[field], "%s", B17_slabs_replacement[slab]);
	}
	*/

	field++;
      }
    }
  }

  field=3124;
  for (slab=0; slab<3; slab++) {
    for (row=0; row<13; row++) {
      for (col=0; col<10; col++) {
	sc=0;
	
	if (col!=9) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+9-col]);
	  sc++;
	} else {
	  if (row!=12) { 
	    sprintf(field_supercategory_names[field][sc], "%s by total personal income", B17_slabs_replacement[slab]);
	    sc++;
	  } else {
	    sprintf(field_supercategory_names[field][sc], "%s", B17_slabs_replacement[slab]);	 
	    sc++;
	  }
	}
	if (slab!=2) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(2-slab)*10*13]);
	  sc++;
	} 
	if (row!=12) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(12-row)*10]);
	  sc++;
	} else {
	  sprintf(field_supercategory_names[field][sc], "%s by age", B17_slabs_replacement[slab]);	 
	  sc++;
	}

	field++;
      }
    }
  }





  /**********  start TABLE B18 - complete ***********/
  char *B18_cols[4] = {"have need for assistance", "do not have need for assistance", "need for assistance is not stated", ""};
  char *B18_rows[12] = {"0-4 years old", "5-14 years old", "15-19 year old", "20-24 year old", "25-34 year old", "35-44 year old", "45-54 year old", "55-64 year old", "65-74 year old", "75-84 year old", "over 85 year old", ""};
  char *B18_slabs[3] = {" males", " females", "s"};
  char *B18_slabs_replacement[3] = {"males", "females", "people"};
 
  field=3514;
  for (slab=0; slab<3; slab++) {
    for (row=0; row<12; row++) {
      for (col=0; col<4; col++) {
	sprintf(field_category_names[field], "%s%s who %s", B18_rows[row], B18_slabs[slab], B18_cols[col]);

	if (col==3) {
	  sprintf(field_category_names[field], "%s%s", B18_rows[row], B18_slabs[slab]);
	}

	if (row==11) {
	  sprintf(field_category_names[field], "%s who %s", B18_slabs_replacement[slab], B18_cols[col]);
	  if (col==3) sprintf(field_category_names[field], "%s", B18_slabs_replacement[slab]);
	}

	field++;
      }
    }
  }



  field=3514;
  for (slab=0; slab<3; slab++) {
    for (row=0; row<12; row++) {
      for (col=0; col<4; col++) {
	sc=0;
	
	if (col!=3) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+3-col]);
	  sc++;
	} else {
	  if (row!=11) { 
	    sprintf(field_supercategory_names[field][sc], "%s by age", B18_slabs_replacement[slab]);
	    sc++;
	  }
	}
	if (slab!=2) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(2-slab)*12*4]);
	  sc++;
	} 
	if (row!=11) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(11-row)*4]);
	  sc++;
	} else {
	  if (col!=3) {
	    sprintf(field_supercategory_names[field][sc], "%s by core activity need for assistance", B18_slabs_replacement[slab]);	 
	    sc++;
	  }
	}

	field++;
      }
    }
  }



  /**********  start TABLE B19 - complete ***********/
  char *B19_cols[4] = {"do voluntary work for an organization", "do not do voluntary work for an organization", "did not state whether they do voluntary work for an organization", ""};
  char *B19_rows[10] = {"15-19 year old", "20-24 year old", "25-34 year old", "35-44 year old", "45-54 year old", "55-64 year old", "65-74 year old", "75-84 year old", "over 85 year old", "over 15 year old"};
  char *B19_slabs[3] = {" males", " females", "s"};
  char *B19_slabs_replacement[3] = {"males", "females", "people"};
 
  field=3658;
  for (slab=0; slab<3; slab++) {
    for (row=0; row<10; row++) {
      for (col=0; col<4; col++) {
	sprintf(field_category_names[field], "%s%s who %s", B19_rows[row], B19_slabs[slab], B19_cols[col]);

	if (col==3) {
	  sprintf(field_category_names[field], "%s%s", B19_rows[row], B19_slabs[slab]);
	}

	field++;
      }
    }
  }


  field=3658;
  for (slab=0; slab<3; slab++) {
    for (row=0; row<10; row++) {
      for (col=0; col<4; col++) {
	sc=0;
	
	if (col!=3) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+3-col]);
	  sc++;
	} else {
	  sprintf(field_supercategory_names[field][sc], "%s by age", B19_slabs_replacement[slab]);	 
	  sc++;
	}
	if (slab!=2) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(2-slab)*10*4]);
	  sc++;
	} 
	if (row!=9) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(9-row)*4]);
	  sc++;
	} else {
	  if (col!=3) { 
	    sprintf(field_supercategory_names[field][sc], "%s by voluntary work participation", B19_slabs_replacement[slab]);	 
	    sc++;
	  }
	}

	field++;
      }
    }
  }






  /**********  start TABLE B20 - incomplete ***********/
  char *B20_cols[7] = {"do less than 5 hours of unpaid domestic work in a week", "do 5-14 hours of unpaid domestic work in a week", "do 15-29 hours of unpaid domestic work in a week", "do 30 hours or more of unpaid domestic work in a week", "do no unpaid domestic work", "did not state the amount of domestic work they do", ""};
  char *B20_rows[10] = {"15-19 year old", "20-24 year old", "25-34 year old", "35-44 year old", "45-54 year old", "55-64 year old", "65-74 year old", "75-84 year old", "over 85 year old", "over 15 year old"};
  char *B20_slabs[3] = {" males", " females", "s"};
  char *B20_slabs_replacement[3] = {"males", "females", "people"};
 
  field=3778;
  for (slab=0; slab<3; slab++) {
    for (row=0; row<10; row++) {
      for (col=0; col<7; col++) {
	sprintf(field_category_names[field], "%s%s who %s", B20_rows[row], B20_slabs[slab], B20_cols[col]);

	if (col==6) {
	  sprintf(field_category_names[field], "%s%s", B20_rows[row], B20_slabs[slab]);
	}

	field++;
      }
    }
  }



  field=3778;
  for (slab=0; slab<3; slab++) {
    for (row=0; row<10; row++) {
      for (col=0; col<7; col++) {
	sc=0;
	
	if (col!=6) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+6-col]);
	  sc++;
	} else {
	  sprintf(field_supercategory_names[field][sc], "%s by age", B20_slabs_replacement[slab]);	 
	  sc++;
	}
	if (slab!=2) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(2-slab)*10*7]);
	  sc++;
	} 
	if (row!=9) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(9-row)*7]);
	  sc++;
	} else {
	  if (col!=6) { 
	    sprintf(field_supercategory_names[field][sc], "%s by unpaid domestic workload", B20_slabs_replacement[slab]);	 
	    sc++;
	  }
	}

	field++;
      }
    }
  }



  /**********  start TABLE B21 - complete ***********/
  char *B21_cols[4] = {"provide unpaid assistance to a person with a disability", "do not provide unpaid assistance to a person with a disability", "did not state whether they provide unpaid assistance to a person with a disability", ""};
  char *B21_rows[10] = {"15-19 year old", "20-24 year old", "25-34 year old", "35-44 year old", "45-54 year old", "55-64 year old", "65-74 year old", "75-84 year old", "over 85 year old", "over 15 year old"};
  char *B21_slabs[3] = {" males", " females", "s"};
  char *B21_slabs_replacement[3] = {"males", "females", "people"};
 
  field=3988;
  for (slab=0; slab<3; slab++) {
    for (row=0; row<10; row++) {
      for (col=0; col<4; col++) {
	sprintf(field_category_names[field], "%s%s who %s", B21_rows[row], B21_slabs[slab], B21_cols[col]);

	if (col==3) {
	  sprintf(field_category_names[field], "%s%s", B21_rows[row], B21_slabs[slab]);
	}

	field++;
      }
    }
  }


  field=3988;
  for (slab=0; slab<3; slab++) {
    for (row=0; row<10; row++) {
      for (col=0; col<4; col++) {
	sc=0;
	
	if (col!=3) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+3-col]);
	  sc++;
	} else {
	  sprintf(field_supercategory_names[field][sc], "%s by age", B21_slabs_replacement[slab]);	 
	  sc++;
	}
	if (slab!=2) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(2-slab)*10*4]);
	  sc++;
	} 
	if (row!=9) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(9-row)*4]);
	  sc++;
	} else {
	  if (col!=3) { 
	    sprintf(field_supercategory_names[field][sc], "%s by unpaid assistance to a person with a disability", B21_slabs_replacement[slab]);	 
	    sc++;
	  }
	}

	field++;
      }
    }
  }



  /**********  start TABLE B22 - complete ***********/
  char *B22_cols[7] = {"provide unpaid child care for their own children only", "provide unpaid child care for others' children only", "provide unpaid child care for both their own children and others' children", "provide unpaid child care", "do not provide unpaid child care", "did not state whether they provide unpaid child care", ""};
  char *B22_rows[10] = {"15-19 year old", "20-24 year old", "25-34 year old", "35-44 year old", "45-54 year old", "55-64 year old", "65-74 year old", "75-84 year old", "over 85 year old", "over 15 year old"};
  char *B22_slabs[3] = {" males", " females", "s"};
  char *B22_slabs_replacement[3] = {"males", "females", "people"};
 
  field=4108;
  for (slab=0; slab<3; slab++) {
    for (row=0; row<10; row++) {
      for (col=0; col<7; col++) {
	sprintf(field_category_names[field], "%s%s who %s", B22_rows[row], B22_slabs[slab], B22_cols[col]);

	if (col==6) {
	  sprintf(field_category_names[field], "%s%s", B22_rows[row], B22_slabs[slab]);
	}

	field++;
      }
    }
  }

  field=4108;
  for (slab=0; slab<3; slab++) {
    for (row=0; row<10; row++) {
      for (col=0; col<7; col++) {
	sc=0;
	
	if (col!=6) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+6-col]);
	  sc++;
	} else {
	  sprintf(field_supercategory_names[field][sc], "%s by age", B22_slabs_replacement[slab]);	 
	  sc++;
	}
	if (slab!=2) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(2-slab)*10*7]);
	  sc++;
	} 
	if (row!=9) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(9-row)*7]);
	  sc++;
	} else {
	  if (col!=6) { 
	    sprintf(field_supercategory_names[field][sc], "%s by unpaid provision of childcare", B22_slabs_replacement[slab]);	 
	    sc++;
	  }
	}

	field++;
      }
    }
  }






  /**********  start TABLE B23 - complete ***********/
  //char *B23_rows[12] = {"husband or wife in a registered marriage", "partner in de facto marriage", "lone parent", "child under 15", "dependent student", "non-dependent child", "other related individual", "unrelated individual living in family household", "group household member", "living alone", "visitor from within Australia", ""};
  char *B23_rows_replacement[12] = {"husbands or wives in a registered marriage", "partners in de facto marriage", "lone parents", "children under 15", "dependent students", "non-dependent children", "other related individuals", "unrelated individuals living in family household", "group household members", "individuals living alone", "visitors from within Australia", ""};
  char *B23_cols[10] = {"0-14 year old", "15-24 year old", "25-34 year old", "35-44 year old", "45-54 year old", "55-64 year old", "65-74 year old", "75-84 year old", "over 85 year old", ""};
  char *B23_slabs[3] = {" male", " female", ""};
  char *B23_slabs_replacement[3] = {"male", "female", ""};
  char *B23_slabs_replacement2[3] = {"males", "females", "people"};
  char *B23_slabs_row_replacement[3] = {"husbands in a registered marriage", "wives in a registered marriage", "husbands or wives in a registered marriage"};
 
  field=4318;
  for (slab=0; slab<3; slab++) {
    for (row=0; row<12; row++) {
      for (col=0; col<10; col++) {

	if ((col==0)&&(!((row==3)||(row==10)||(row==11)))) {
	  sprintf(field_category_names[field], "impossible - should be deleted");
	  field++;
	  continue; 
	}
	if ((row==3)&&(col!=0)&&(col!=9)) {
	  sprintf(field_category_names[field], "impossible - should be deleted");
	  field++;
	  continue;
	}
	if ((row==4)&&(col!=1)&&(col!=9)) {
	  sprintf(field_category_names[field], "impossible - should be deleted");
	  field++;
	  continue; 
	}

	sprintf(field_category_names[field], "%s%s %s", B23_cols[col], B23_slabs[slab], B23_rows_replacement[row]);

	if (row==0) {
	  sprintf(field_category_names[field], "%s %s", B23_cols[col], B23_slabs_row_replacement[slab]);
	}

	if (row==11) {
	  sprintf(field_category_names[field], "%s%ss", B23_cols[col], B23_slabs[slab]);
	}

	if (col==9) {
	  if (row==0) {
	    sprintf(field_category_names[field], "%s", B23_slabs_row_replacement[slab]);
	  } else if (row!=11) {
	    sprintf(field_category_names[field], "%s %s", B23_slabs_replacement[slab], B23_rows_replacement[row]);
	    if (slab==2) sprintf(field_category_names[field], "%s", B23_rows_replacement[row]);
	  } else {
	    sprintf(field_category_names[field], "%ss", B23_slabs_replacement[slab]);	    
	    if (slab==2) sprintf(field_category_names[field], "people");	    
	  }
	}

	field++;
      }
    }
  }



  field=4318;
  for (slab=0; slab<3; slab++) {
    for (row=0; row<12; row++) {
      for (col=0; col<10; col++) {
	sc=0;
	
	if (col!=9) {
	  if (row!=11) { 
	    sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+9-col]);
	    sc++;
	  }
	} else {
	  if (row!=11) { 
	    sprintf(field_supercategory_names[field][sc], "%s by relationship within the household", B23_slabs_replacement2[slab]);	 
	    sc++;
	  }
	}
	if (slab!=2) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(2-slab)*12*10]);
	  sc++;
	} 
	if (row!=11) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(11-row)*10]);
	  sc++;
	} else {
	  if (col!=9) { 
	    sprintf(field_supercategory_names[field][sc], "%s by age", B23_slabs_replacement2[slab]);	 
	    sc++;
	  } else if (slab!=2) {
	    sprintf(field_supercategory_names[field][sc], "people by gender");	 
	    sc++;
	  }
	}
	
	field++;
      }
    }
  }









  /**********  start TABLE B24 - complete ***********/
  char *B24_rows[16] = {"15-19 year old", "20-24 year old", "25-29 year old", "30-34 year old", "35-39 year old", "40-44 year old", "45-49 year old", "50-54 year old", "55-59 year old", "60-64 year old", "65-69 year old", "70-74 year old", "75-79 year old", "80-84 year old", "over 85 year old", "over 15 year old"};
  char *B24_cols[9] = {"no children", "one child", "two children", "three children", "four children", "five children", "six or more children", " who did not state the number of children they have had", ""};
  field=4678;
  for (row=0; row<16; row++) {
    for (col=0; col<9; col++) {
      sprintf(field_category_names[field], "%s females who have had %s", B24_rows[row], B24_cols[col]);
      if ((col==7)||(col==8)) {
	sprintf(field_category_names[field], "%s females%s", B24_rows[row], B24_cols[col]);
      }
      field++;
    }
  }



  field=4678;
  for (row=0; row<16; row++) {
    for (col=0; col<9; col++) {
      sc=0;
      if (row != 15) { 
	sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(15-row)*9]);
	sc++;
      } else {	
	if (col!=8) { 
	  sprintf(field_supercategory_names[field][sc], "females by number of children ever born");
	  sc++;
	}
      }
      if (col!=8) {
	sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+8-col]);
	sc++;
      } else { 
	sprintf(field_supercategory_names[field][sc], "females by age");
	sc++;
      }
      field++;
    } 
  }


  


  /**********  start TABLE B25 - complete ***********/
  char *B25_rows[23] = {"a couple with no children", "a couple with children under 15 and dependent students and non-dependent children", "a couple with children under 15 and dependent students and no non-dependent children", "a couple with children under 15 and no dependent students and non-dependent children", "a couple with children under 15 and no dependent students and no non-dependent children", "a couple with children under 15", "a couple with no children under 15 and dependent students and non-dependent children", "a couple with no children under 15 and dependent students and no non-dependent children", "a couple with no children under 15 and no dependent students and non-dependent children", "a couple with no children under 15", "a couple with children", "one parent with children under 15 and dependent students and non-dependent children", "one parent with children under 15 and dependent students and no non-dependent children", "one parent with children under 15 and nodependent students and non-dependent children", "one parent with children under 15 and no dependent students and no non-dependent children", "one parent with children under 15", "one parent with no children under 15 and dependent students and non-dependent children", "one parent with no children under 15 and dependent students and no non-dependent children", "one parent with no children under 15 and no dependent students and non-dependent children", "one parent with no children under 15", "one parent with children", "another family composition", ""};
  char *B25_rows_super[23] = {"", "a couple with children under 15", "a couple with children under 15", "a couple with children under 15", "a couple with children under 15", "a couple with children", "a couple with no children under 15", "a couple with no children under 15", "a couple with no children under 15", "a couple with children", "", "one parent with children under 15", "one parent with children under 15", "one parent with children under 15", "one parent with children under 15", "one parent with children", "one parent with no children under 15", "one parent with no children under 15", "one parent with no children under 15", "one parent with children", "", "", ""};
  char *B25_cols[2] = {"families", "persons in families"};
  field=4822;
  for (row=0; row<23; row++) {
    for (col=0; col<2; col++) {
      sprintf(field_category_names[field], "%s comprising %s", B25_cols[col], B25_rows[row]);
      if (row==22) {
	sprintf(field_category_names[field], "%s", B25_cols[col]);
      }
      field++;
    }
  }
  

  field=4822;
  for (row=0; row<23; row++) {
    for (col=0; col<2; col++) {
      sc=0;
      if (row != 22) { 
	sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(22-row)*2]);
	sc++;
      }

      if (strlen(B25_rows_super[row])!=0) {
	sprintf(field_supercategory_names[field][sc], "%s comprising %s", B25_cols[col], B25_rows_super[row]);
	sc++;
      }
      
      if (col==0) { 
	sprintf(field_supercategory_names[field][sc], "families by composition");
	sc++;
      } else {
	sprintf(field_supercategory_names[field][sc], "people in families by composition");
	sc++;
	sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field-1]);
	sc++;
      }
      
      field++;
    } 
  }






  /**********  start TABLE B26 - complete ***********/
  char *B26_rows[18] = {"negative or nil", "in the range $1-$199", "in the range $200-$299", "in the range $300-$399", "in the range $400-$599", "in the range $600-$799", "in the range $800-$999", "in the range $1000-$1249", "in the range $1250-$1499", "in the range $1500-$1999", "in the range $2000-$2499", "in the range $2500-$2999", "in the range $3000-$3499", "in the range $3500-$3999", "$4000 or more", "partially not stated", "not stated", ""};
  char *B26_cols[5] = {"couple families with no children", "couple families with children", "one parent families", "other families", "families"};
 
  field=4868;
  for (row=0; row<18; row++) {
    for (col=0; col<5; col++) {
      sprintf(field_category_names[field], "%s with a total family income %s", B26_cols[col],  B26_rows[row]);
      if (row==17) sprintf(field_category_names[field], "%s", B26_cols[col]);

      field++;
    }
  }



  field=4868;
  for (row=0; row<18; row++) {
    for (col=0; col<5; col++) {
      sc=0;
      if (row != 17) { 
	sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(17-row)*5]);
	sc++;
      } else {	
	if (col!=4) { 
	  sprintf(field_supercategory_names[field][sc], "families by composition");
	  sc++;
	}
      }
      if (col!=4) {
	sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+4-col]);
	sc++;
      } else { 
	if (row != 17) { 
	  sprintf(field_supercategory_names[field][sc], "families by income");
	  sc++;
	}
      }
      field++;
    } 
  }





  /**********  start TABLE B27 - complete ***********/
  sprintf(field_category_names[4958], "intact families with no other children present");
  sprintf(field_category_names[4959], "step families with no other children present");
  sprintf(field_category_names[4960], "blended families with no other children present");
  sprintf(field_category_names[4961], "intact families with other children present");
  sprintf(field_category_names[4962], "step families with other children present");
  sprintf(field_category_names[4963], "blended families with other children present");
  sprintf(field_category_names[4964], "other couple families with other children only");
  sprintf(field_category_names[4965], "couple families with children");


  field=4958;
  for (col=0; col<8; col++) {
    sc=0;
    if (col!=7) { 
      sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+7-col]);
      sc++;
    }
    sprintf(field_supercategory_names[field][sc], "families by composition");
    sc++;
    field++; 
  }

      

  /**********  start TABLE B28 - complete ***********/
  char *B28_rows[18] = {"negative or nil", "in the range $1-$199", "in the range $200-$299", "in the range $300-$399", "in the range $400-$599", "in the range $600-$799", "in the range $800-$999", "in the range $1000-$1249", "in the range $1250-$1499", "in the range $1500-$1999", "in the range $2000-$2499", "in the range $2500-$2999", "in the range $3000-$3499", "in the range $3500-$3999", "$4000 or more", "partially not stated", "not stated", ""};
  char *B28_cols[3] = {"family households", "non-family households", "households"};
 
  field=4966;
  for (row=0; row<18; row++) {
    for (col=0; col<3; col++) {
      sprintf(field_category_names[field], "%s with a total household income %s", B28_cols[col],  B28_rows[row]);
      if (row==17) sprintf(field_category_names[field], "%s", B28_cols[col]);

      field++;
    }
  }

  field=4966;
  for (row=0; row<18; row++) {
    for (col=0; col<3; col++) {
      sc=0;
      if (row != 17) { 
	sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(17-row)*3]);
	sc++;
      } else {
	if (col==0) {
	  sprintf(field_supercategory_names[field][sc], "families");
	  sc++;
	} 
      }

      if (col!=2) {
	sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+2-col]);
	sc++;
      } else { 
	if (row != 17) { 
	  sprintf(field_supercategory_names[field][sc], "households by income");
	  sc++;
	}
      }
      field++;
    } 
  }



  /**********  start TABLE B29 - complete ***********/
  sprintf(field_category_names[5020], "dwellings with no motor vehicles");
  sprintf(field_category_names[5021], "dwellings with one motor vehicle");
  sprintf(field_category_names[5022], "dwellings with two motor vehicles");
  sprintf(field_category_names[5023], "dwellings with three motor vehicles");
  sprintf(field_category_names[5024], "dwellings with four or more motor vehicles");
  sprintf(field_category_names[5025], "dwellings with motor vehicles");
  sprintf(field_category_names[5026], "dwellings where the number of motor vehicles was not stated");
  sprintf(field_category_names[5027], "dwellings");

  field=5020;
  for (col=0; col<8; col++) {
    sc=0;
    if (col!=7) {
      sprintf(field_supercategory_names[field][sc], "dwellings by number of motor vehicles");
      sc++;
    }
    field++; 
  }




  /**********  start TABLE B30 - complete ***********/
  char *B30_rows[7] = {" with one person usually resident", " with two people usually resident", " with three people usually resident", " with four people usually resident", " with five people usually resident", " with six or more people usually resident", ""};
  char *B30_cols[3] = {"family households", "non-family households", "households"};
 
  field=5028;
  for (row=0; row<7; row++) {
    for (col=0; col<3; col++) {
      sprintf(field_category_names[field], "%s%s", B30_cols[col],  B30_rows[row]);

      if ((row==0) && (col==0)) sprintf(field_category_names[field], "impossible - should be deleted");

      field++;
    }
  }

  field=5028;
  for (row=0; row<7; row++) {
    for (col=0; col<3; col++) {
      sc=0;
      if (row != 6) { 
	sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(6-row)*3]);
	sc++;
      } else {
	if (col==0) {
	  sprintf(field_supercategory_names[field][sc], "families");
	  sc++;
	} 
      }

      if (col!=2) {
	sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+2-col]);
	sc++;
      } else { 
	if (row != 6) { 
	  sprintf(field_supercategory_names[field][sc], "households by size");
	  sc++;
	}
      }
      field++;
    } 
  }



  /**********  start TABLE B31 - complete ***********/
  char *B31_rows[17] = {"separate house", "one storey semi-detached, row, or terrace house, or townhouse", "two or more storey semi-detached, row, or terrace house, or townhouse", "semi-detached, row, or terrace house, or townhouse", "flat, unit or apartment in a one or two storey block", "flat, unit or apartment in a three storey block", "flat, unit or apartment in a four or more storey block", "flat, unit or apartment attached to a house", "flat, unit or apartment", "caravan, cabin or houseboat", "sleepers out, tent, or improvised home", "house or flat attached to a shop, office or similar", "total other", "structure not stated", "occupied private", "unoccupied private", "private"};
  char *B31_rows_super[17] = {"", "semi-detached, row, or terrace house, or townhouse", "semi-detached, row, or terrace house, or townhouse", "", "flat, unit or apartment", "flat, unit or apartment", "flat, unit or apartment", "flat, unit or apartment", "", "total other", "total other", "total other", "", "", "", "", ""};
  char *B31_rows_super2[17] = {"occupied private", "occupied private", "occupied private", "occupied private", "occupied private", "occupied private", "occupied private", "occupied private", "occupied private", "occupied private", "occupied private", "occupied private", "occupied private", "occupied private", "", "", ""};
  char *B31_cols[2] = {"", "persons in "};
  field=5049;
  for (row=0; row<17; row++) {
    for (col=0; col<2; col++) {
      sprintf(field_category_names[field], "%s%s dwellings", B31_cols[col], B31_rows[row]);
      field++;
    }
  }

  field=5049;
  for (row=0; row<17; row++) {
    for (col=0; col<2; col++) {
      sc=0;
      if (row != 16) { 
	sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(16-row)*2]);
	sc++;
      }

      if (strlen(B31_rows_super[row])!=0) {
	sprintf(field_supercategory_names[field][sc], "%s%s dwellings", B31_cols[col], B31_rows_super[row]);
	sc++;
      }
      if (strlen(B31_rows_super2[row])!=0) {
	sprintf(field_supercategory_names[field][sc], "%s%s dwellings", B31_cols[col], B31_rows_super2[row]);
	sc++;
      }
      
      if (col==0) { 
	sprintf(field_supercategory_names[field][sc], "dwellings by type");
	sc++;
      } else {
	sprintf(field_supercategory_names[field][sc], "people in dwellings by type");
	sc++;
	sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field-1]);
	sc++;
      }
      
      field++;
    } 
  }



  

  /**********  start TABLE B32 - complete ***********/
  char *B32_cols[6] = {"separate houses", "semi-detached, row, or terrace houses, or townhouses",  "flats, units or apartments", "other dwellings", "structures not stated", "dwellings"};
  char *B32_rows[12] = {"owned outright", "owned with a mortgage", "rented through a real estate agent", "rented from a state or territory housing authority", "rented from a person not in the same household", "rented from a housing co-operative, community or church group", "rented from an employer or through a residential park", "rented from a landlord type not stated", "rented", "with another tenure type", "where the tenure type was not stated", ""};
  field=5083;
  for (row=0; row<12; row++) {
    for (col=0; col<6; col++) {
      sprintf(field_category_names[field], "%s %s", B32_cols[col], B32_rows[row]); 
      if (row==11) sprintf(field_category_names[field], "%s%s", B32_cols[col], B32_rows[row]); 
      field++;
    }
  }

  field=5083;
  for (row=0; row<12; row++) {
    for (col=0; col<6; col++) {
      sc=0;
      if (row != 11) { 
	sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(11-row)*6]);
	sc++;
      } else {	
	if (col!=5) { 
	  sprintf(field_supercategory_names[field][sc], "dwellings by type");
	  sc++;
	}
      }
      if (col!=5) {
	sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+5-col]);
	sc++;
      } else { 
	if (row != 11) { 
	  sprintf(field_supercategory_names[field][sc], "dwellings by tenure type");
	  sc++;
	}
      }
      field++;
    } 
  }

  


  /**********  start TABLE B33 - complete ***********/
  char *B33_rows[13] = {" in the range $0-$299", " in the range $300-$449", " in the range $450-$599", " in the range $600-$799", " in the range $800-$999", " in the range $1000-$1399", " in the range $1400-$1799", " in the range $1800-$2399", " in the range $2400-$2999", " in the range $3000-$3999", " of $4000 or more", " not stated", ""};
  char *B33_cols[6] = {"separate houses", "semi-detached, row, or terrace houses, or townhouses",  "flats, units or apartments", "other dwellings", "structures not stated", "private dwellings"};
 
  field=5155;
  for (row=0; row<13; row++) {
    for (col=0; col<6; col++) {
      sprintf(field_category_names[field], "%s being purchased with a monthly mortgage repayment%s", B33_cols[col],  B33_rows[row]);
      field++;
    }
  }

  field=5155;
  for (row=0; row<13; row++) {
    for (col=0; col<6; col++) {
      sc=0;
      if (row != 12) { 
	sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(12-row)*6]);
	sc++;
      } else {	
	sprintf(field_supercategory_names[field][sc], "%s", B33_cols[col]);
	sc++;
      }
      if (col!=5) {
	sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+5-col]);
	sc++;
      } else { 
	if (row != 12) { 
	  sprintf(field_supercategory_names[field][sc], "dwellings by monthly mortgage repayment");
	  sc++;
	}
      }
      field++;
    } 
  }



  /**********  start TABLE B34 - complete ***********/
  char *B34_rows[13] = {" in the range $0-$74", " in the range $75-$99", " in the range $100-$149", " in the range $150-$199", " in the range $200-$224", " in the range $225-$274", " in the range $275-$349", " in the range $350-$449", " in the range $450-$549", " in the range $550-$649", " of over $650", " not stated", ""};
  char *B34_cols[7] = {"rented through a real estate agent", "rented from a state or territory housing authority", "rented from a person not in the same household", "rented from a housing co-operative, community or church group", "rented from an employer or through a residential park", "rented from a landlord type not stated", "rented"};
 
  field=5233;
  for (row=0; row<13; row++) {
    for (col=0; col<7; col++) {
      sprintf(field_category_names[field], "occupied private dwellings being %s at a weekly cost%s", B34_cols[col],  B34_rows[row]);
      if (row==12) sprintf(field_category_names[field], "occupied private dwellings being %s", B34_cols[col]);
      field++;
    }
  }

  field=5233;
  for (row=0; row<13; row++) {
    for (col=0; col<7; col++) {
      sc=0;
      if (row != 12) { 
	sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(12-row)*7]);
	sc++;
      } else {	
	if (col!=6) { 
	  sprintf(field_supercategory_names[field][sc], "dwellings by type of landlord");
	  sc++;
	}
      }
      if (col!=6) {
	sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+6-col]);
	sc++;
      } else { 
	if (row != 12) { 
	  sprintf(field_supercategory_names[field][sc], "dwellings by weekly rental cost");
	  sc++;
	} else {
	  sprintf(field_supercategory_names[field][sc], "private dwellings");
	  sc++;
	}
      }
      field++;
    } 
  }


  /**********  start TABLE B35 - complete ***********/
  char *B35_rows[7] = {" with no internet connection", " with broadband internet", " with dial-up internet", " with another type of internet connection", " with an internet connection", " where the internet connection was not stated", ""};
  char *B35_cols[6] = {"separate houses", "semi-detached, row, or terrace houses, or townhouses",  "flats, units or apartments", "other dwellings", "structures not stated", "private dwellings"};
 
  field=5324;
  for (row=0; row<7; row++) {
    for (col=0; col<6; col++) {
      sprintf(field_category_names[field], "%s%s", B35_cols[col],  B35_rows[row]);
      field++;
    }
  }


  field=5324;
  for (row=0; row<7; row++) {
    for (col=0; col<6; col++) {
      sc=0;
      if (row != 6) { 
	sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(6-row)*6]);
	sc++;
      } else {	
	sprintf(field_supercategory_names[field][sc], "dwellings by type");
	sc++;
      }
      if (col!=5) {
	sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+5-col]);
	sc++;
      } else { 
	if (row != 6) { 
	  sprintf(field_supercategory_names[field][sc], "dwellings by internet connection");
	  sc++;
	}
      }
      field++;
    } 
  }


  /**********  start TABLE B36 - complete ***********/
  char *B36_rows[12] = {"separate houses", "one storey semi-detached, row, or terrace houses, or townhouses", "two or more storey semi-detached, row, or terrace houses, or townhouses", "semi-detached, row, or terrace houses, or townhouses", "flats, units or apartments in a one or two storey block", "flats, units or apartments in a three storey block", "flats, units or apartments in a four or more storey block", "flats, units or apartments attached to a house,", "flats, units or apartments", "other dwellings", "dwelling structures not stated", "occupied private dwellings"};
  char *B36_cols[9] = {" with no bedrooms", " with one bedroom", " with two bedrooms", " with three bedrooms", " with four bedrooms", " with five bedrooms", " with six or more bedrooms", " with an unstated number of bedrooms", ""};
  field=5366;
  for (row=0; row<12; row++) {
    for (col=0; col<9; col++) {
      sprintf(field_category_names[field], "%s%s", B36_rows[row], B36_cols[col]);
      field++;
    }
  }

  field=5366;
  for (row=0; row<12; row++) {
    for (col=0; col<9; col++) {
      sc=0;
      if (row != 11) { 
	sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(11-row)*9]);
	sc++;
	if (col!=8) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+8-col]);
	  sc++;
	} else { 
	  sprintf(field_supercategory_names[field][sc], "dwellings by type");
	  sc++;
	}
      } else {
	if (col!=8) {
	  sprintf(field_supercategory_names[field][sc], "dwellings by bedroom count");
	  sc++;
	}
      }
      field++;
    } 
  }


  
  /**********  start TABLE B37 - complete ***********/
  char *B37_rows[19]= {"over 15 years old", " over 15 years old and employed full time", " over 15 years old and employed part time", " over 15 years old and employed but away from work", " over 15 years old and unemployed and looking for work", " over 15 years old and in the labour force", " over 15 years old and not in the labour force", " unemployment as a percentage of the labour force", " labour force participation as a percentage of the population over 15 years old", " employment as a percentage of the population over 15 years old", " with qualifications up to postgraduate degree level", " with qualifications up to graduate diploma and graduate certificate level", " with qualifications up to bachelor degree level", " with qualifications up to advanced diploma or diploma level", " with qualifications up to certificate level", " who lived at the same address one year ago", " who lived at a different address one year ago", " who lived at the same address one year ago", " who lived at a different address five years ago"};
  char *B37_cols[3] = {"males", "females", "people"};
  char *B37_cols_replacements[3] = {" males", " females", "s"};
  field=5474;
  for (row=0; row<19; row++) {
    for (col=0; col<3; col++) {
      sprintf(field_category_names[field], "%s%s", B37_cols[col], B37_rows[row]);
      if (row==0) sprintf(field_category_names[field], "%s%s", B37_rows[row], B37_cols_replacements[col]);
      field++;
    }
  }
  field=5474;
  for (row=0; row<19; row++) {
    for (col=0; col<3; col++) {
      sprintf(field_supercategory_names[field][0], "%s", B01_cols[col]);
      if (col!=2) 
	sprintf(field_supercategory_names[field][1], "%s", field_category_names[field+2-col]);
      field++;
    } 
  }


  /**********  start TABLE B38 - complete ***********/
  char *B38_rows[17]= {" who lived at the same address one year ago", " who lived at a different address one year ago in the same statistical area (level 2)", " who lived in a different statistical area (level 2) in New South Wales one year ago", " who lived in a different statistical area (level 2) in Victoria one year ago", " who lived in a different statistical area (level 2) in Queensland one year ago", " who lived in a different statistical area (level 2) in South Australia one year ago", " who lived in a different statistical area (level 2) in Western Australia one year ago", " who lived in a different statistical area (level 2) in Tasmania one year ago", " who lived in a different statistical area (level 2) in the Northern Territory one year ago", " who lived in a different statistical area (level 2) in the Australian Capital Territory one year ago", " who lived in a different statistical area (level 2) in other Australian territories one year ago", " who lived in a different statistical area (level 2) one year ago", " who lived overseas one year ago", " who lived at a different address one year ago, but did not state where", " who lived at a different address one year ago", " who did not state whether they lived at the same address one year ago", "over 1 year old" };
  char *B38_rows_super[17]= {"", "", " who lived in a different statistical area (level 2) one year ago", " who lived in a different statistical area (level 2) one year ago", " who lived in a different statistical area (level 2) one year ago", " who lived in a different statistical area (level 2) one year ago", " who lived in a different statistical area (level 2) one year ago", " who lived in a different statistical area (level 2) one year ago", " who lived in a different statistical area (level 2) one year ago", " who lived in a different statistical area (level 2) one year ago", " who lived in a different statistical area (level 2) one year ago", " who lived at a different address one year ago", " who lived at a different address one year ago", " who lived at a different address one year ago", "", "", "" };
  char *B38_cols_replacement[3] = {" males", " females", "s"};
  char *B38_cols[3] = {"males", "females", "people"};
  field=5531;
  for (row=0; row<17; row++) {
    for (col=0; col<3; col++) {
      sprintf(field_category_names[field], "%s%s", B38_cols[col], B38_rows[row]);
      if (row==16) sprintf(field_category_names[field], "%s%s", B38_rows[row], B38_cols_replacement[col]);
      field++;
    }
  }
  
  field=5531;
  for (row=0; row<17; row++) {
    for (col=0; col<3; col++) {
      sc=0;
      if (row!=16) {
	if (strlen(B38_rows_super[row])!=0) {
	  sprintf(field_supercategory_names[field][sc], "%s%s", B38_cols[col], B38_rows_super[row]);
	  sc++;
	}
	sprintf(field_supercategory_names[field][sc], "%s by residence one year ago", B38_cols[col]);
	sc++;
	sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(16-row)*3]);
	sc++;
      } else {
	sprintf(field_supercategory_names[field][sc], "%s by age", B38_cols[col]);
	sc++;
      }
      
      if (col!=2) {
	sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+2-col]);
	sc++;
      }
      
      field++;
    }
  }


  /**********  start TABLE B39 - complete ***********/
  char *B39_rows[17]= {" who lived at the same address five years ago", " who lived at a different address five years ago in the same statistical area (level 2)", " who lived in a different statistical area (level 2) in New South Wales five years ago", " who lived in a different statistical area (level 2) in Victoria five years ago", " who lived in a different statistical area (level 2) in Queensland five years ago", " who lived in a different statistical area (level 2) in South Australia five years ago", " who lived in a different statistical area (level 2) in Western Australia five years ago", " who lived in a different statistical area (level 2) in Tasmania five years ago", " who lived in a different statistical area (level 2) in the Northern Territory five years ago", " who lived in a different statistical area (level 2) in the Australian Capital Territory five years ago", " who lived in a different statistical area (level 2) in other Australian territories five years ago", " who lived in a different statistical area (level 2) five years ago", " who lived overseas five years ago", " who lived at a different address five years ago, but did not state where", " who lived at a different address five years ago", " who did not state whether they lived at the same address five years ago", "over 5 year old" };
  char *B39_rows_super[17]= {"", "", " who lived in a different statistical area (level 2) five years ago", " who lived in a different statistical area (level 2) five years ago", " who lived in a different statistical area (level 2) five years ago", " who lived in a different statistical area (level 2) five years ago", " who lived in a different statistical area (level 2) five years ago", " who lived in a different statistical area (level 2) five years ago", " who lived in a different statistical area (level 2) five years ago", " who lived in a different statistical area (level 2) five years ago", " who lived in a different statistical area (level 2) five years ago", " who lived at a different address five years ago", " who lived at a different address five years ago", " who lived at a different address five years ago", "", "", "" };
  char *B39_cols[3] = {"males", "females", "people"};
  char *B39_cols_replacement[3] = {" males", " females", "s"};
  field=5582;
  for (row=0; row<17; row++) {
    for (col=0; col<3; col++) {
      sprintf(field_category_names[field], "%s%s", B39_cols[col], B39_rows[row]);
      if (row==16) sprintf(field_category_names[field], "%s%s", B39_rows[row], B39_cols_replacement[col]);
      field++;
    }
  }
  
  field=5582;
  for (row=0; row<17; row++) {
    for (col=0; col<3; col++) {
      sc=0;
      if (row!=16) {
	if (strlen(B39_rows_super[row])!=0) {
	  sprintf(field_supercategory_names[field][sc], "%s%s", B39_cols[col], B39_rows_super[row]);
	  sc++;
	}
	sprintf(field_supercategory_names[field][sc], "%s by residence five years ago", B39_cols[col]);
	sc++;
	sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(16-row)*3]);
	sc++;
      } else {
	sprintf(field_supercategory_names[field][sc], "%s by age", B39_cols[col]);
	sc++;
      }
      
      if (col!=2) {
	sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+2-col]);
	sc++;
      }
      
      field++;
    }
  }







  /**********  start TABLE B40 - complete ***********/
  char *B40_rows[11] = {"postgraduate degree level", "graduate diploma or graduate certificate level", "bachelor degree level", "advanced diploma or diploma level", "certificate level (not fully described)", "certificate III or IV level", "certificate I or II level", "certificate level", "inadequately described", "not stated", ""};
  char *B40_rows_super[11] = {"", "", "", "", "certificate level", "certificate level", "certificate level", "", "", "", ""};
  char *B40_cols[9] = {"15-24 year old", "25-34 year old", "35-44 year old", "45-54 year old", "55-64 year old", "65-74 year old", "75-84 year old", "over 85 year old", "over 15 year old"};
  char *B40_slabs[3] = {" males", " females", "s"};
  char *B40_slabs_replacement[3] = {"males", "females", "people"};
 
  field=5633;
  for (slab=0; slab<3; slab++) {
    for (row=0; row<11; row++) {
      for (col=0; col<9; col++) {
	sprintf(field_category_names[field], "%s%s with a %s qualification", B40_cols[col], B40_slabs[slab], B40_rows[row]);
	if (row==8) sprintf(field_category_names[field], "%s%s with an %s qualification", B40_cols[col], B40_slabs[slab], B40_rows[row]);
	if (row==10) sprintf(field_category_names[field], "%s%s with a qualification", B40_cols[col], B40_slabs[slab]);

	field++;
      }
    }
  }


  field=5633;
  for (slab=0; slab<3; slab++) {
    for (row=0; row<11; row++) {
      for (col=0; col<9; col++) {
	sc=0;
	
	if (col!=8) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+8-col]);
	  sc++;
	} else {
	  sprintf(field_supercategory_names[field][sc], "%s by qualification", B40_slabs_replacement[slab]);	 
	  sc++;
	}
	if (slab!=2) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(2-slab)*11*9]);
	  sc++;
	} 
	if (row!=10) {
	  if (strlen(B40_rows_super[row])!=0) {
	    sprintf(field_supercategory_names[field][sc], "%s%s with a %s qualification", B40_cols[col], B40_slabs[slab], B40_rows_super[row]);
	    sc++;
	  }
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(10-row)*9]);
	  sc++;
	} else {
	  sprintf(field_supercategory_names[field][sc], "%s%s", B40_cols[col], B40_slabs[slab]);
	  sc++;
	  sprintf(field_supercategory_names[field][sc], "%s with a qualification by age", B40_slabs_replacement[slab]);	 
	  sc++;
	}
	
	field++;
      }
    }
  }




  /**********  start TABLE B41 - complete ***********/
  char *B41_rows[15] = {"natural or physical sciences", "information technology", "engineering or related technologies", "architecture or building", "agriculture, environmental or related studies", "health", "education", "management or commerce", "society or culture", "creative arts", "food, hospitality or personal services", "mixed field programmes", "an inadequately described field", "an unstated field", "total"};
  char *B41_cols[9] = {"15-24 year old", "25-34 year old", "35-44 year old", "45-54 year old", "55-64 year old", "65-74 year old", "75-84 year old", "over 85 year old", "over 15 year old"};
  char *B41_slabs[3] = {" males", " females", "s"};
  char *B41_slabs_replacement[3] = {"males", "females", "people"};
 
  field=5930;
  for (slab=0; slab<3; slab++) {
    for (row=0; row<15; row++) {
      for (col=0; col<9; col++) {
	sprintf(field_category_names[field], "%s%s with a qualification in %s", B41_cols[col], B41_slabs[slab], B41_rows[row]);
	if (row==14) sprintf(field_category_names[field], "%s%s with a qualification", B41_cols[col], B41_slabs[slab]);
	field++;
      }
    }
  }

  field=5930;
  for (slab=0; slab<3; slab++) {
    for (row=0; row<15; row++) {
      for (col=0; col<9; col++) {
	sc=0;
	
	if (col!=8) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+8-col]);
	  sc++;
	} else {
	  sprintf(field_supercategory_names[field][sc], "%s by field of study", B41_slabs_replacement[slab]);	 
	  sc++;
	}
	if (slab!=2) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(2-slab)*15*9]);
	  sc++;
	} 
	if (row!=14) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(14-row)*9]);
	  sc++;
	} else {
	  sprintf(field_supercategory_names[field][sc], "%s%s", B41_cols[col], B41_slabs[slab]);	 
	  sc++;
	}

	field++;
      }
    }
  }




  /**********  start TABLE B42 - complete ***********/
  char *B42_rows[12]= {" employed full time", " employed part time", " employed but away from work", " hours worked not stated", " employed", " unemployed looking for full time work", " unemployed looking for part time work", " unemployed", " in the labour force", " not in the labour force", " that did not state their labour force status", ""};
  char *B42_cols[10] = {"15-19 year old", "20-24 year old", "25-34 year old", "35-44 year old", "45-54 year old", "55-64 year old", "65-74 year old", "75-84 year old", "over 85 year old", "over 15 year old"};
  char *B42_slabs[3] = {" males", " females", "s"};
  char *B42_slabs_replacement[3] = {"males", "females", "people"};
 
  field=6335;
  for (slab=0; slab<3; slab++) {
    for (row=0; row<12; row++) {
      for (col=0; col<10; col++) {
	sprintf(field_category_names[field], "%s%s%s", B42_cols[col], B42_slabs[slab], B42_rows[row]);
	field++;
      }
    }
  }



  field=6335;
  for (slab=0; slab<3; slab++) {
    for (row=0; row<12; row++) {
      for (col=0; col<10; col++) {
	sc=0;
	if (col!=9) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+9-col]);
	  sc++;
	} else {
	  sprintf(field_supercategory_names[field][sc], "%s by employment status", B42_slabs_replacement[slab]);
	  sc++;
	}
	if (slab!=2) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(2-slab)*10*12]);
	  sc++;
	} 
	if (row!=11) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(11-row)*10]);
	  sc++;
	} else {
	  sprintf(field_supercategory_names[field][sc], "%s by age", B42_slabs_replacement[slab]);	 
	  sc++;
	}

	field++;
      } 
    }
  }




  /**********  start TABLE B43 - complete ***********/
  char *B43_rows[21]= {"agriculture, forestry and fishing", "mining", "manufacturing", "electricity, gas, water and waste services", "construction", "wholesale trade", "retail trade", "accommodation and food services", "transport, postal and warehousing", "information media and telecommunications", "financial and insurance services", "rental, hiring and real estate services", "professional, scientific and technical services", "administrative and support services", "public administration and safety", "education and training", "health care and social assistance", "arts and recreation services", "other services", "inadequately described or unstated", ""};
  char *B43_cols[10] = {"15-19 year old", "20-24 year old", "25-34 year old", "35-44 year old", "45-54 year old", "55-64 year old", "65-74 year old", "75-84 year old", "over 85 year old", "over 15 year old"};
  char *B43_slabs[3] = {" males", " females", "s"};
  char *B43_slabs_replacement[3] = {"males", "females", "people"};
 
  field=6695;
  for (slab=0; slab<3; slab++) {
    for (row=0; row<21; row++) {
      for (col=0; col<10; col++) {
	sprintf(field_category_names[field], "%s%s employed in the %s industry", B43_cols[col], B43_slabs[slab], B43_rows[row]);
	if (row==18) sprintf(field_category_names[field], "%s%s employed in an%s industry", B43_cols[col], B43_slabs[slab], B43_rows[row]);
	if (row==19) sprintf(field_category_names[field], "%s%s employed in an %s industry", B43_cols[col], B43_slabs[slab], B43_rows[row]);
	if (row==20) sprintf(field_category_names[field], "%s%s employed", B43_cols[col], B43_slabs[slab]);
	field++;
      }
    }
  }

  field=6695;
  for (slab=0; slab<3; slab++) {
    for (row=0; row<21; row++) {
      for (col=0; col<10; col++) {
	sc=0;
	
	if (col!=9) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+9-col]);
	  sc++;
	} else {
	  if (row!=20) { 
	    sprintf(field_supercategory_names[field][sc], "%s by industry of employment", B43_slabs_replacement[slab]);
	    sc++;
	  } else {
	    sprintf(field_supercategory_names[field][sc], "%s by employment status", B43_slabs_replacement[slab]);	 
	    sc++;
	  }
	}
	if (slab!=2) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(2-slab)*10*21]);
	  sc++;
	} 
	if (row!=20) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(20-row)*10]);
	  sc++;
	} else {
	  sprintf(field_supercategory_names[field][sc], "%s%s", B43_cols[col], B43_slabs[slab]);	 
	  sc++;
	}
	
	
	field++;
      }
    }
  }




  /**********  start TABLE B43 - complete ***********/
  char *B44_rows[21]= {"agriculture, forestry and fishing", "mining", "manufacturing", "electricity, gas, water and waste services", "construction", "wholesale trade", "retail trade", "accommodation and food services", "transport, postal and warehousing", "information media and telecommunications", "financial and insurance services", "rental, hiring and real estate services", "professional, scientific and technical services", "administrative and support services", "public administration and safety", "education and training", "health care and social assistance", "arts and recreation services", "other services", "inadequately described or unstated", ""};
  char *B44_cols[10] = {"managers", "professionals", "technicians and trades workers", "community and personal service workers", "clerical and administrative workers", "sales workers", "machinery operators and drivers", "labourers", "workers whose occupation was inadequately described or not stated", "over 15 year olds"};
 
  field=7325;
  for (row=0; row<21; row++) {
    for (col=0; col<10; col++) {
      sprintf(field_category_names[field], "%s employed in the %s industry", B44_cols[col], B44_rows[row]);
      if (row==18) sprintf(field_category_names[field], "%s employed in an%s industry", B44_cols[col], B44_rows[row]);
      if (row==19) sprintf(field_category_names[field], "%s employed in an %s industry", B44_cols[col], B44_rows[row]);
      if (row==20) sprintf(field_category_names[field], "%s employed", B44_cols[col]);
      field++;
    }
  }
  
  
  field=7325;
  for (row=0; row<21; row++) {
    for (col=0; col<10; col++) {
      sc=0;
      if (row != 20) { 
	sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(20-row)*10]);
	sc++;
	if (col!=9) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+9-col]);
	  sc++;
	} else { 
	  sprintf(field_supercategory_names[field][sc], "people by industry of employment");
	  sc++;
	}
      } else {
	if (col!=9) {
	  sprintf(field_supercategory_names[field][sc], "people by occupation");
	  sc++;
	} else {
	  sprintf(field_supercategory_names[field][sc], "over 15 year olds");	 
	  sc++;
	  sprintf(field_supercategory_names[field][sc], "people by employment status");	 
	  sc++;
	}
      }
      field++;
    } 
  }




  /**********  start TABLE B45 - complete ***********/
  char *B45_cols[10] = {"managers", "professionals", "technicians and trades workers", "community and personal service workers", "clerical and administrative workers", "sales workers", "machinery operators and drivers", "labourers", "workers whose occupation was inadequately described or not stated", ""};
  char *B45_rows[10] = {"15-19 year old", "20-24 year old", "25-34 year old", "35-44 year old", "45-54 year old", "55-64 year old", "65-74 year old", "75-84 year old", "over 85 year old", "over 15 year old"};
  char *B45_slabs[3] = {" males", " females", "s"};
  char *B45_slabs_replacement[3] = {"males", "females", "people"};
 
  field=7535;
  for (slab=0; slab<3; slab++) {
    for (row=0; row<10; row++) {
      for (col=0; col<10; col++) {
	sprintf(field_category_names[field], "%s%s employed as %s", B45_rows[row], B45_slabs[slab], B45_cols[col]);
	if (col==9) sprintf(field_category_names[field], "%s%s employed", B45_rows[row], B45_slabs[slab]);
	field++;
      }
    }
  }

  
  field=7535;
  for (slab=0; slab<3; slab++) {
    for (row=0; row<10; row++) {
      for (col=0; col<10; col++) {
	sc=0;
	
	if (col!=9) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+9-col]);
	  sc++;
	} else {
	  sprintf(field_supercategory_names[field][sc], "%s%s", B45_rows[row], B45_slabs[slab]);	 
	  sc++;
	}
	if (slab!=2) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(2-slab)*10*10]);
	  sc++;
	} 
	if (row!=9) {
	  sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+(9-row)*10]);
	  sc++;
	} else {
	  if (col!=9) { 
	    sprintf(field_supercategory_names[field][sc], "%s by occupation", B45_slabs_replacement[slab]);
	    sc++;
	  } else {
	    sprintf(field_supercategory_names[field][sc], "%s by employment status", B45_slabs_replacement[slab]);	 
	    sc++;
	  }
	}
	
	field++;
      }
    }
  }




  /**********  start TABLE B46 - complete ***********/
  char *B46_rows[36]= {"train", "bus", "ferry", "tram or light rail", "taxi", "car, as driver", "car, as passenger", "truck", "motorbike or scooter", "bicycle", "other single method", "foot only", "one method", "train and bus", "train and ferry", "train and tram or light rail", "train and car, as driver", "train and car, as passenger", "train and other", "train and any other method", "bus and ferry", "bus and tram or light rail", "bus and car, as driver", "bus and car, as passenger", "bus and other", "bus and any other method", "two methods not including a train or bus", "two methods", "train and two other methods", "bus and two methods other than train", "three methods not including a train or bus", "three methods", " who woked at home", " who did not go to work", " who did not state their method of travel to work", ""};
  char *B46_rows_super[36]= {"one method", "one method", "one method", "one method", "one method", "one method", "one method", "one method", "one method", "one method", "one method", "one method", "number of methods", "two methods", "two methods", "two methods", "two methods", "two methods", "two methods", "two methods", "two methods", "two methods", "two methods", "two methods", "two methods", "two methods", "two methods", "number of methods", "three methods", "three methods", "three methods", "number of methods", "", "", "", ""}; //FIRST (NOT WITH BCPFIELD) RUN ONCE UP TO 7855 WITH THIS COMMENTED OUT, BUT THE NEXT LINES IN (AND SEE 10 LINES BELOW)
  //char *B46_rows_super[36]= {"", "", "", "", "", "", "", "", "", "", "", "", "number of methods", "two methods", "two methods", "two methods", "two methods", "two methods", "two methods", "two methods", "two methods", "two methods", "two methods", "two methods", "two methods", "two methods", "two methods", "number of methods", "three methods", "three methods", "three methods", "number of methods", "", "", "", ""};
  char *B46_rows_super2[36]= {"train", "bus", "ferry", "tram or light rail", "taxi", "car, as driver", "car, as passenger", "", "", "", "", "", "", "train and any other method", "train and any other method", "train and any other method", "train and any other method", "train and any other method", "train and any other method", "train", "bus and any other method", "bus and any other method", "bus and any other method", "bus and any other method", "bus and any other method", "bus", "", "", "train", "bus", "", "", "", "", "", ""};
  //char *B46_rows_super2[36]= {"", "", "", "", "", "car", "car", "", "", "", "", "", "", "train and any other method", "train and any other method", "train and any other method", "train and any other method", "train and any other method", "train and any other method", "train", "bus and any other method", "bus and any other method", "bus and any other method", "bus and any other method", "bus and any other method", "bus", "", "", "train", "bus", "", "", "", "", "", ""};
  char *B46_rows_super3[36]= {"", "", "", "", "", "car", "car", "", "", "", "", "", "", "train", "train", "train", "train", "train", "train", "", "bus", "bus", "bus", "bus", "bus", "", "", "", "", "", "", "", "", "", "", ""};
  //char *B46_rows_super3[36]= {"", "", "", "", "", "", "", "", "", "", "", "", "", "train", "train", "train", "train", "train", "train", "", "bus", "bus", "bus", "bus", "bus", "", "", "", "", "", "", "", "", "", "", ""};
  char *B46_rows_super4[36]= {"", "", "", "", "", "", "", "", "", "", "", "", "", "bus", "ferry", "tram or light rail", "car, as driver", "car, as passenger", "", "", "ferry", "tram or light rail", "car, as driver", "car, as passenger", "", "", "", "", "", "", "", "", "", "", "", ""};
  char *B46_cols[3] = {"males", "females", "people"};
  char *B46_cols_replacement[3] = {" males", " females", "s"};
  field=7835;
  for (row=0; row<36; row++) {
    for (col=0; col<3; col++) {
      sprintf(field_category_names[field], "%s who travel to work by %s", B46_cols[col], B46_rows[row]);
      if (row<=9) sprintf(field_category_names[field], "%s who travel to work by %s only", B46_cols[col], B46_rows[row]); // AND THIS COMMENTED OUT
      if (row>=32) sprintf(field_category_names[field], "%s%s", B46_cols[col], B46_rows[row]);
      if (row==35) sprintf(field_category_names[field], "over 15 year old%s employed", B46_cols_replacement[col]);
      field++;
    }
  }


  field=7835;
  for (row=0; row<36; row++) {
    for (col=0; col<3; col++) {
      sc=0;
      if (row!=35) {
	if (strlen(B46_rows_super[row])!=0) {
	  sprintf(field_supercategory_names[field][sc], "%s who travel to work by %s", B46_cols[col], B46_rows_super[row]);
	  sc++;
	  if (strlen(B46_rows_super2[row])!=0) {
	    sprintf(field_supercategory_names[field][sc], "%s who travel to work by %s", B46_cols[col], B46_rows_super2[row]);
	    sc++;
	    if (strlen(B46_rows_super3[row])!=0) {
	      sprintf(field_supercategory_names[field][sc], "%s who travel to work by %s", B46_cols[col], B46_rows_super3[row]);
	      sc++;
	      if (strlen(B46_rows_super4[row])!=0) {
		sprintf(field_supercategory_names[field][sc], "%s who travel to work by %s", B46_cols[col], B46_rows_super4[row]);
		sc++;
	      }
	    }
	  }
	}
	sprintf(field_supercategory_names[field][sc], "%s by method of travel to work", field_category_names[field+(35-row)*3]);
	sc++;
      }
      
      if (col!=2) {
	sprintf(field_supercategory_names[field][sc], "%s", field_category_names[field+2-col]);
	sc++;
      }
      
      field++;
    }
  }






  int sd, ar;


  for (field=startfield; field<=finishfield; field++) {
    //printf("python /home/hudson/general/wiki/pywikipedia/add_text.py -file:Template:Australian2011CensusMaps/bcpfieldcat/%d -summary:\"bot - for bulk categories on demographic maps\" -text:\"%s\"\n", field, field_category_names[field]);
    //printf("%d \t%s\n", field, field_category_names[field]);

    // for this use 
    // python /home/hudson/general/wiki/pywikipedia/pagefromfile.py -start:xxxx -end:yyyy -notitle -safe -pt:3 -file:output.txt

    if (field==4318||field==4328 || field==4338 ||field==4349 || field==4350 || field==4351 || field==4352 || field==4353 || field==4354 || field==4355 || field==4356 || field==4358 || field==4360 || field==4361 || field==4362 || field==4363 || field==4364 || field==4365 || field==4366 || field==4368 || field==4378 || field==4388 || field==4398 || field==4408 || field==4438 || field==4448 || field==4458 || field==4469 || field==4470 || field==4471 || field==4472 || field==4473 || field==4474 || field==4475 || field==4476 || field==4478 || field==4480 || field==4481 || field==4482 || field==4483 || field==4484 || field==4485 || field==4486 || field==4488 || field==4498 || field==4508 || field==4518 || field==4528 || field==4558 || field==4568 || field==4578 || field==4589 || field==4590 || field==4591 || field==4592 || field==4593 || field==4594 || field==4595 || field==4596 || field==4598 || field==4600 || field==4601 || field==4602 || field==4603 || field==4604 || field==4605 || field==4606 || field==4608 || field==4618 || field==4628 || field==4638 || field==4648 || field==5028 ) {
      /* these fields are theoretically impossible, so maps would all be blank - don't bother creating or uploading */
      /* IF CHANGING THIS LIST, ALSO CHANGE EQUIVS IN BCP_PROCESS.C */
      continue;
    } 
	  
    if (DOBCPFIELDCATS) printf("xxxx\n'''Template:Australian2011CensusMaps/bcpfieldcat/%d'''\n%s\nyyyy\n", field, field_category_names[field]);
  }
  
  
  if (DOSUBDIV) {
    for (sd = 0; sd<NSUBDIV; sd++ ) {
      if (DOEXTRACATS) {
	for (extracat=0; extracat<EXTRACATS; extracat++) {
	  printf("xxxx\n'''Category:Demographic maps of %s by %s'''\n", extra_category_names[extracat], geosubdivision_long[do_subdiv[sd]]);

	  if (strstr(extra_category_names[extracat], "females")) {
	    if (!strstr(extra_category_names[extracat], "year old females")) {
	      printf("[[Category:Demographic maps of %s by %s|*females]]\n", replace_str(extra_category_names[extracat], "females", "people"), geosubdivision_long[do_subdiv[sd]]);
	    } else {
	      printf("[[Category:Demographic maps of %s by %s|*females]]\n", replace_str(extra_category_names[extracat], "year old females", "year olds"), geosubdivision_long[do_subdiv[sd]]);
	    }
	    printf("[[Category:Demographic maps of females by %s|%s]]\n", geosubdivision_long[do_subdiv[sd]], extra_category_wikinames[extracat] );
	  } else if (strstr(extra_category_names[extracat],"males")) {
	    if (!strstr(extra_category_names[extracat], "year old males")) {
	      printf("[[Category:Demographic maps of %s by %s|*males]]\n", replace_str(extra_category_names[extracat], "males", "people"), geosubdivision_long[do_subdiv[sd]]);
	    } else {
	      printf("[[Category:Demographic maps of %s by %s|*males]]\n", replace_str(extra_category_names[extracat], "year old males", "year olds"), geosubdivision_long[do_subdiv[sd]]);
	    }
	    printf("[[Category:Demographic maps of males by %s|%s]]\n", geosubdivision_long[do_subdiv[sd]], extra_category_wikinames[extracat] );
	  }

	  if (geosubdivision[do_subdiv[sd]][0]!='\0') {
	    printf("[[Category:Demographic maps of %s by geographic subdivision|*%s]]\n", extra_category_names[extracat], geosubdivision_long[do_subdiv[sd]]);
	    if (extra_supercategory_names[extracat][0]!='\0') {
	      printf("[[Category:Demographic maps of %s by %s|%s]]\n", extra_supercategory_names[extracat], geosubdivision_long[do_subdiv[sd]], extra_category_wikinames[extracat]);
	    }
	  } else {
	    printf("[[Category:Demographic maps of %s|*%s]]\n", extra_category_names[extracat], geosubdivision_long[do_subdiv[sd]]);
	    if (extra_supercategory_names[extracat][0]!='\0') {
	      printf("[[Category:Demographic maps of %s by geographic subdivision|%s]]\n", extra_supercategory_names[extracat], extra_category_wikinames[extracat]);
	    }
	  }
	  if (extra_supercategory_names[extracat][0]=='\0') {
	    if (!strstr(extra_category_names[extracat],"males")) { /* this will screen out both gender categories */
	      printf("[[Category:Demographic maps by %s|%s]]\n", geosubdivision_long[do_subdiv[sd]], extra_category_wikinames[extracat]);
	    }
	  }

	  printf("{{User:File Upload Bot (99of9)/CensusCategory}}\n");
	  printf("yyyy\n");
	}
      } else { // don't do fields and extracats in the same batch, otherwise there will be duplication when you do many ranges of fields
	for (field=startfield; field<=finishfield; field++) {
	  

	  if (field==4318||field==4328 || field==4338 ||field==4349 || field==4350 || field==4351 || field==4352 || field==4353 || field==4354 || field==4355 || field==4356 || field==4358 || field==4360 || field==4361 || field==4362 || field==4363 || field==4364 || field==4365 || field==4366 || field==4368 || field==4378 || field==4388 || field==4398 || field==4408 || field==4438 || field==4448 || field==4458 || field==4469 || field==4470 || field==4471 || field==4472 || field==4473 || field==4474 || field==4475 || field==4476 || field==4478 || field==4480 || field==4481 || field==4482 || field==4483 || field==4484 || field==4485 || field==4486 || field==4488 || field==4498 || field==4508 || field==4518 || field==4528 || field==4558 || field==4568 || field==4578 || field==4589 || field==4590 || field==4591 || field==4592 || field==4593 || field==4594 || field==4595 || field==4596 || field==4598 || field==4600 || field==4601 || field==4602 || field==4603 || field==4604 || field==4605 || field==4606 || field==4608 || field==4618 || field==4628 || field==4638 || field==4648 || field==5028 ) {
	    /* these fields are theoretically impossible, so maps would all be blank - don't bother creating or uploading */
	    /* IF CHANGING THIS LIST, ALSO CHANGE EQUIVS IN BCP_PROCESS.C */
	    continue;
	  } 




	  printf("xxxx\n'''Category:Demographic maps of %s by %s'''\n", field_category_names[field], geosubdivision_long[do_subdiv[sd]]);
	  if (geosubdivision[do_subdiv[sd]][0]!='\0') {
	    printf("[[Category:Demographic maps of %s by geographic subdivision|*%s]]\n", field_category_names[field], geosubdivision_long[do_subdiv[sd]]);
	  } else {
	    printf("[[Category:Demographic maps of %s|*%s]]\n", field_category_names[field], geosubdivision_long[do_subdiv[sd]]);
	  }
	  if (field_generalcategory_names[field][0]!='\0') 
	    printf("[[Category:Demographic maps of %s by %s]]\n", field_generalcategory_names[field], geosubdivision_long[do_subdiv[sd]]);
	  
	  countsc=0;
	  while ((field_supercategory_names[field][countsc][0]!='\0')&&(countsc<MAXCATS)) {
	    //printf("field %d countsc %d\n", field, countsc);
	    countsc++;
	    if (countsc==MAXCATS) break; 
	  } 
	  for (sc=0; sc<countsc; sc++) {
	    if (countsc==0) 
	      printf("[[Category:Demographic maps of %s by %s", field_supercategory_names[field][sc], geosubdivision_long[do_subdiv[sd]]);
	    else 
	      printf("[[Category:Demographic maps of %s by %s|", field_supercategory_names[field][sc], geosubdivision_long[do_subdiv[sd]]);
	    
	    for (osc=0; osc<countsc; osc++) { 
	      // the other supercategory names give a clue to what the sortkey should be in this supercategory 
	      //printf("(%d<%d)", osc, countsc);
	      if (osc!=sc) {
		if (osc>1 || (osc==1 && sc!=0)) printf(" "); 
		printf("%s",field_supercategory_names[field][osc]);
	      }
	    }
	    if (countsc==1) printf("%s",field_category_names[field]);
	    printf("]]\n");
	  }
	  if (countsc==0)  printf("[[Category:Demographic maps by %s|%s]]\n", geosubdivision_long[do_subdiv[sd]], field_category_names[field]);
	  printf("{{User:File Upload Bot (99of9)/CensusCategory}}\n");
	  printf("yyyy\n");
	}
      }
    }
  }

  if (DOAREAS) {
    for (ar = 0; ar<NAREAS; ar++) {

      if (DOEXTRACATS) {
	for (extracat=0; extracat<EXTRACATS; extracat++) {
	  printf("xxxx\n'''Category:Demographic maps of %s in %s'''\n", extra_category_names[extracat], area_names[do_areas[ar]]);

	  if (strstr(extra_category_names[extracat], "females")) {
	    if (!strstr(extra_category_names[extracat], "year old females")) {
	      printf("[[Category:Demographic maps of %s in %s|*females]]\n", replace_str(extra_category_names[extracat], "females", "people"), area_names[do_areas[ar]]);
	    } else {
	      printf("[[Category:Demographic maps of %s in %s|*females]]\n", replace_str(extra_category_names[extracat], "year old females", "year olds"), area_names[do_areas[ar]]);
	    }
	    printf("[[Category:Demographic maps of females in %s|%s]]\n", area_names[do_areas[ar]], extra_category_wikinames[extracat] );
	  } else if (strstr(extra_category_names[extracat],"males")) {
	    if (!strstr(extra_category_names[extracat], "year old males")) {
	      
	      printf("[[Category:Demographic maps of %s in %s|*males]]\n", replace_str(extra_category_names[extracat], "males", "people"), area_names[do_areas[ar]]);
	    } else {
	      printf("[[Category:Demographic maps of %s in %s|*males]]\n", replace_str(extra_category_names[extracat], "year old males", "year olds"), area_names[do_areas[ar]]);
	    }
	    printf("[[Category:Demographic maps of males in %s|%s]]\n", area_names[do_areas[ar]], extra_category_wikinames[extracat] );
	  }
	  
	  if (area_names_by[do_areas[ar]][0]!='\0') {
	    printf("[[Category:Demographic maps of %s in %s|%s]]\n", extra_category_names[extracat], area_names_by[do_areas[ar]], area_wikinames[do_areas[ar]]);
	  }
	  if (superarea_names[do_areas[ar]][0]!='\0') {
	    printf("[[Category:Demographic maps of %s in %s|%s]]\n", extra_category_names[extracat], superarea_names[do_areas[ar]], area_wikinames[do_areas[ar]]);
	  }
	  if (extra_supercategory_names[extracat][0]!='\0') {
	    printf("[[Category:Demographic maps of %s in %s|%s]]\n", extra_supercategory_names[extracat], area_names[do_areas[ar]], extra_category_wikinames[extracat]);
	  } else {
	    if (!strstr(extra_category_names[extracat],"males")) { /* this will catch either male or female gender categories */
	      printf("[[Category:Demographic maps of %s|%s]]\n", area_names[do_areas[ar]], extra_category_wikinames[extracat]);
	    }
	  }
	  printf("{{User:File Upload Bot (99of9)/CensusCategory}}\n");
	  printf("yyyy\n");
	}
      } else { // don't do fields and extracats in the same batch, otherwise there will be duplication when you do many ranges of fields
	for (field=startfield; field<=finishfield; field++) {
	  
	  if (field==4318||field==4328 || field==4338 ||field==4349 || field==4350 || field==4351 || field==4352 || field==4353 || field==4354 || field==4355 || field==4356 || field==4358 || field==4360 || field==4361 || field==4362 || field==4363 || field==4364 || field==4365 || field==4366 || field==4368 || field==4378 || field==4388 || field==4398 || field==4408 || field==4438 || field==4448 || field==4458 || field==4469 || field==4470 || field==4471 || field==4472 || field==4473 || field==4474 || field==4475 || field==4476 || field==4478 || field==4480 || field==4481 || field==4482 || field==4483 || field==4484 || field==4485 || field==4486 || field==4488 || field==4498 || field==4508 || field==4518 || field==4528 || field==4558 || field==4568 || field==4578 || field==4589 || field==4590 || field==4591 || field==4592 || field==4593 || field==4594 || field==4595 || field==4596 || field==4598 || field==4600 || field==4601 || field==4602 || field==4603 || field==4604 || field==4605 || field==4606 || field==4608 || field==4618 || field==4628 || field==4638 || field==4648 || field==5028 ) {
	    /* these fields are theoretically impossible, so maps would all be blank - don't bother creating or uploading */
	    /* IF CHANGING THIS LIST, ALSO CHANGE EQUIVS IN BCP_PROCESS.C */
	    continue;
	  } 


	  printf("xxxx\n'''Category:Demographic maps of %s in %s'''\n", field_category_names[field], area_names[do_areas[ar]]);
	  if (area_names_by[do_areas[ar]][0]!='\0') 
	    printf("[[Category:Demographic maps of %s in %s|%s]]\n", field_category_names[field], area_names_by[do_areas[ar]], area_wikinames[do_areas[ar]]);
	  if (superarea_names[do_areas[ar]][0]!='\0') 
	    printf("[[Category:Demographic maps of %s in %s|%s]]\n", field_category_names[field], superarea_names[do_areas[ar]], area_wikinames[do_areas[ar]]);
	  if (field_generalcategory_names[field][0]!='\0') 
	    printf("[[Category:Demographic maps of %s in %s]]\n", field_generalcategory_names[field], area_names[do_areas[ar]]);
	  
	  countsc=0;
	  while ((field_supercategory_names[field][countsc][0]!='\0')&&(countsc<MAXCATS)) {
	    countsc++;
	    if (countsc==MAXCATS) break; 
	  } 
	  for (sc=0; sc<countsc; sc++) {
	    if (countsc==0) 
	      printf("[[Category:Demographic maps of %s in %s", field_supercategory_names[field][sc], area_names[do_areas[ar]]);
	    else 
	      printf("[[Category:Demographic maps of %s in %s|", field_supercategory_names[field][sc], area_names[do_areas[ar]]);
	    
	    for (osc=0; osc<countsc; osc++) { 
	      // the other supercategory names give a clue to what the sortkey should be in this supercategory 
	      if (osc!=sc) {
		if (osc>1 || (osc==1 && sc!=0)) printf(" "); 
		printf("%s",field_supercategory_names[field][osc]);
	      }
	    }
	    if (countsc==1) printf("%s",field_category_names[field]);
	    printf("]]\n");
	  }
	  if (countsc==0) printf("[[Category:Demographic maps of %s]]\n", area_names[do_areas[ar]]);
	  printf("{{User:File Upload Bot (99of9)/CensusCategory}}\n");
	  printf("yyyy\n");
	}
      }
    }
  }
}
  
