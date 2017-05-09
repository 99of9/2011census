#include <stdio.h>
#include <stdlib.h>

#define MAXFIELDS 6000

int main() {
  int row, col, slab;
  int field;


  char field_category_names[MAXFIELDS][500];


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

  /**********  start TABLE B04 - incomplete ***********/
  char *B04_cols[3] = {" males", " females", "s"};
  char *B04_rows[102] = {"0","1","2","3","4","0-4","5","6","7","8","9","5-9","10","11","12","13","14","10-14","15","16","17","18","19","15-19","20","21","22","23","24","20-24","25","26","27","28","29","25-29","30","31","32","33","34","30-34","35","36","37","38","39","35-39","40","41","42","43","44","40-44","45","46","47","48","49","45-49","50","51","52","53","54","50-54","55","56","57","58","59","55-59","60","61","62","63","64","60-64","65","66","67","68","69","65-69","70","71","72","73","74","70-74","75","76","77","78","79","75-79","80-84","85-89","90-94","95-99","over 100",""};
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

  /**********  start TABLE B05 - complete ***********/
  char *B05_cols[6] = {"married ", "separated ", "divorced ", "widowed ", "never married ", ""};
  char *B05_rows[10] = {"15-19", "20-24", "25-34", "35-44", "45-54", "55-64", "65-74", "75-84", "over 85", ""};
  char *B05_slabs[3] = {" males", " females", "s"};
 
  field=563;
  for (slab=0; slab<3; slab++) {
    for (row=0; row<10; row++) {
      for (col=0; col<6; col++) {
	sprintf(field_category_names[field], "%s%s year old%s", B05_cols[col], B05_rows[row], B05_slabs[slab]);
	if ((slab==2) && (row==9)) sprintf(field_category_names[field], "%speople", B05_cols[col]);
	field++;
      }
    }
  }

  /**********  start TABLE B06 - complete ***********/
  char *B06_cols[4] = {" married in a registered marriage", " married in a de facto marriage", " not currently married", ""};
  char *B06_rows[10] = {"15-19 year old", "20-24 year old", "25-34 year old", "35-44 year old", "45-54 year old", "55-64 year old", "65-74 year old", "75-84 year old", "over 85 year old", ""};
  char *B06_slabs[3] = {" males", " females", "s"};
 
  field=743;
  for (slab=0; slab<3; slab++) {
    for (row=0; row<10; row++) {
      for (col=0; col<4; col++) {
	sprintf(field_category_names[field], "%s%s%s", B06_rows[row], B06_slabs[slab], B06_cols[col]);
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


  /**********  start TABLE B09 - complete ***********/
  char *B09_rows[38] = {"Australia", "Bosnia and Herzegovina", "Cambodia", "Canada", "China excluding Hong Kong, Macau and Taiwan", "Croatia", "Egypt", "Fiji", "the Former Yugoslav Republic of Macedonia", "Germany", "Greece", "Hong Kong", "India", "Indonesia", "Iraq", "Ireland", "Italy", "Japan", "South Korea", "Lebanon", "Malaysia", "Malta", "the Netherlands", "New Zealand", "the Philippines", "Poland", "Singapore", "South Africa", "South Eastern Europe", "Sri Lanka", "Thailand", "Turkey", "the United Kingdom, Channel Islands and Isle of Mann", "the United States of America", "Vietnam", "a country not listed individually", "not stated"};
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

  /**********  start TABLE B11 - complete ***********/
  char *B11_rows[7] = {" who speak English only", " who speak another language and speak English well", " who speak another language and speak English not well or not at all", " who speak another language and did not state proficiency in English", " who speak English and another language", " who did not state proficiency in English", ""};
  char *B11_cols[11] = {" who arrived in Australia before 1996", " who arrived in Australia during 1996-2000", " who arrived in Australia during 2001-2005", " who arrived in Australia during 2006", " who arrived in Australia during 2007", " who arrived in Australia during 2008", " who arrived in Australia during 2009", " who arrived in Australia during 2010", " who arrived in Australia during 2011, prior to the census", " who did not state when they arrived in Australia", " who were born overseas"};
  char *B11_slabs[3] = {"males", "females", "people"};
  field=1937;
  for (slab=0; slab<3; slab++) {
    for (row=0; row<7; row++) {
      for (col=0; col<11; col++) {
	sprintf(field_category_names[field], "%s%s%s", B11_slabs[slab], B11_cols[col], B11_rows[row]);
	field++;
      }
    }
  }

  /**********  start TABLE B12 - incomplete **********/
  char *B12_cols[7] = {" whose fathers speak English only", " whose fathers speak another language and speak English well", " whose fathers speak another language and speak English not well or not at all", " whose fathers speak another language and did not state proficiency in English", " whose fathers speak English and another language", " whose fathers did not state proficiency in English", ""};
  char *B12_rows[7] = {" whose mothers speak English only", " whose mothers speak another language and speak English well", " whose mothers speak another language and speak English not well or not at all", " whose mothers speak another language and did not state proficiency in English", " whose mothers speak English and another language", " whose mothers did not state proficiency in English", ""};
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
  





  for (field=2168; field<=2559; field++) {
    //printf("python /home/hudson/general/wiki/pywikipedia/add_text.py -file:Template:Australian2011CensusMaps/bcpfieldcat/%d -summary:\"bot - for bulk categories on demographic maps\" -text:\"%s\"\n", field, field_category_names[field]);
    //printf("%d \t%s\n", field, field_category_names[field]);

    // for this use 
    // python /home/hudson/general/wiki/pywikipedia/pagefromfile.py -start:xxxx -end:yyyy -notitle -safe -pt:1 -file:output.txt
    printf("xxxx\n'''Template:Australian2011CensusMaps/bcpfieldcat/%d'''\n%s\nyyyy\n", field, field_category_names[field]);
  }

}
