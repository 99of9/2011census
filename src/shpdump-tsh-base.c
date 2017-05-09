/******************************************************************************
 * $Id: shpdump.c,v 1.18 2011-07-24 03:05:14 fwarmerdam Exp $
 *
 * Project:  Shapelib-tsh
 * Purpose:  Application for dumping contents of a 2-d shapefile to 
 *           the terminal in svg form.
 * Author:   Frank Warmerdam, warmerdam@pobox.com
 *           
 ******************************************************************************
 * Copyright (c) 1999, Frank Warmerdam 
 *
 * This software is available under the following "MIT Style" license,
 * or at the option of the licensee under the LGPL (see LICENSE.LGPL).  This
 * option is discussed in more detail in shapelib.html.
 *
 * --
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ******************************************************************************
 *
 *  Derivative work copyright (c) 2012 Toby Hudson released under the same license tobyyy@gmail.com
 *
 *
 * $Log: shpdump.c,v $
 * Revision 1.18  2011-07-24 03:05:14  fwarmerdam
 * use %.15g for formatting coordiantes in shpdump
 *
 */

#include <string.h>
#include <stdlib.h>
#include "math.h"
#include "shapefil.h"

SHP_CVSID("$Id: shpdump.c,v 1.18 2011-07-24 03:05:14 fwarmerdam Exp $")


#define         MAXFILENAME 1000
double 	        adfMinBound[4], adfMaxBound[4];
SHPHandle	hSHP;
DBFHandle	hDBF;
int		nEntities;

#define         MAXSUBMAPS 100
int             numsubmaps;
int             submapstate[MAXSUBMAPS];
double          submapbounds[MAXSUBMAPS][4];
char            submapnames[MAXSUBMAPS][100];
double          submaplegendpos[MAXSUBMAPS][2];

double minsqstep=0.0001*0.0001; /* 0.0001*0.0001 0.05*0.05 set to 0.0 for svg with full detail, make it bigger (0.001?) to cut down the file size (and shape boundary accuracy) */
double decimal_scaleout = 1000.0;

void plain_characters(char str[1000]) {
  int i=0;
  while(str[i]!='\0') {
    if ((str[i]<48)||((str[i]>57)&&(str[i]<65))||((str[i]>90)&&(str[i]<97))||(str[i]>123)) {
      str[i] = '_';
    } 
    i++;
  }
}


void detailed_svg(int argc, char ** argv, int s) {
  FILE *fp, *sl_fp;
  int i, iPart;
  char szFormat[32];
  char	szTitle[12];
  char label[1000];
  char path_id[100];
  char detailed_filename[150];
  char sublist_filename[150];
  int nWidth, nDecimals;
  float prevX, prevY;
  //DBFFieldType	eType;
  float MinBound[2];
  float MaxBound[2];
  float MidPoint[2];
  float long_over_lat_at_midpoint;
  float dist_km_per_degree_longitude;
  float length_km_of_width;
  int dbar_indx;
  float dbar_lengths[18]={0.01, 0.1, 1.0, 2.0, 5.0, 10.0, 20.0, 50.0, 100.0, 200.0, 500.0, 1000.0, 2000.0, 5000.0, 10000.0, 20000.0, 50000.0, 100000.0};
  float distancebar_length;
  int atstart;

  DBFGetFieldInfo( hDBF, 1, szTitle, &nWidth, &nDecimals );
  sprintf(szFormat, "%%d\t%%-%ds\n", nWidth );
  if (numsubmaps>0) {
    sprintf(sublist_filename, "sublist-%s.txt", submapnames[s]);
    sprintf(detailed_filename, "detailed_shape_base-%s.svg", submapnames[s]);
    MinBound[0] = submapbounds[s][0];
    MinBound[1] = submapbounds[s][1];
    MaxBound[0] = submapbounds[s][2];
    MaxBound[1] = submapbounds[s][3];
  } else {
    sprintf(detailed_filename, "detailed_shape_base.svg");
    MinBound[0] = adfMinBound[0];
    MinBound[1] = adfMinBound[1];
    MaxBound[0] = adfMaxBound[0];
    MaxBound[1] = adfMaxBound[1];
  }
  fp = fopen(detailed_filename, "w");
  sl_fp = fopen(sublist_filename, "w");
    
  MidPoint[0] = 0.5*(MaxBound[0]+MinBound[0]);
  MidPoint[1] = 0.5*(MaxBound[1]+MinBound[1]);

  long_over_lat_at_midpoint = cos(MidPoint[1]*M_PI/180.0); 
  
  dist_km_per_degree_longitude = M_PI*6371*cos(MidPoint[1]*M_PI/180.0)/180.0;
  length_km_of_width = dist_km_per_degree_longitude * (MaxBound[0]-MinBound[0]);

  // header is now done in the BCP_process so it can get a detailed description for each map
  //fprintf(fp, "<?xml version=\"1.0\" standalone=\"no\"?>\n<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\"\n    \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n <svg preserveAspectRatio=\"none\" width=\"2000px\" height=\"%.15gpx\" viewBox=\"%.15g %.15g %.15g %.15g\"\n xmlns=\"http://www.w3.org/2000/svg\"\n xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n xmlns:ev=\"http://www.w3.org/2001/xml-events\" version=\"1.1\">\n  \n\n", 2000.0*(MaxBound[1]-MinBound[1])/(MaxBound[0]-MinBound[0])/long_over_lat_at_midpoint, 0.0/* MinBound[0] - MinBound[0]*/, 0.0 /* -MaxBound[1] + MaxBound[1] */, decimal_scaleout*(MaxBound[0]-MinBound[0]), decimal_scaleout*(-MinBound[1]+MaxBound[1]));

   //fprintf(fp, "<g fill=\"grey\" stroke=\"black\" stroke-width=\"0.001\" stroke-linejoin=\"round\">\n");

  //fprintf(fp, "<defs>\n"); now in BCP_process
  
  for( i = 0; i < nEntities; i++ )
    {
      int		j;
      SHPObject	*psShape;
      
      if (!DBFIsAttributeNULL(hDBF,i,4)) {
	if (DBFReadDoubleAttribute(hDBF,i,4)>0.00001) {
	  printf(szFormat, i, DBFReadStringAttribute( hDBF, i, 1));
	}
      }

      psShape = SHPReadObject( hSHP, i );
      
      if( psShape == NULL )
        {
	  fprintf( stderr,
		   "Unable to read shape %d, terminating object reading.\n",
		   i );
	  break;
        }
      
      strcpy(path_id, DBFReadStringAttribute( hDBF, i, 0));
      
      if (strstr(path_id, "CED")||
	  strstr(path_id, "LGA")||
	  strstr(path_id, "POA")||
	  strstr(path_id, "SED")||
	  strstr(path_id, "SSC")
	  ) {
	memmove(path_id, path_id+3, strlen(path_id));	
      }

      if ((psShape->dfXMin < MaxBound[0])&&
	  (psShape->dfXMax > MinBound[0])&&
	  (psShape->dfYMin < MaxBound[1])&&
	  (psShape->dfYMax > MinBound[1])) {
	/* Only print shape if it is within the current bounds.
	 This rough comparison of the box will catch all that are, and leave out most that aren't. */


	fprintf(sl_fp, "%s\n", path_id);

	fprintf(fp, "  <path id=\"%s\" d=\"M ", path_id);
	
	if( psShape->nParts > 0 && psShape->panPartStart[0] != 0 )
	  {
	    fprintf( stderr, "panPartStart[0] = %d, not zero as expected.\n",
		     psShape->panPartStart[0] );
	  }
	
	prevX = 99999.9; /* a starting point that nothing will be near */
	prevY = 99999.9;
	for( j = 0, iPart = 1; j < psShape->nVertices; j++ )
	  {	  
	    if( iPart < psShape->nParts
		&& psShape->panPartStart[iPart] == j )
	      {
		iPart++;
		
		/* start a new polygon, either an island or a cutout.  It would be nice to make the cutouts carve a hole in the underlying polygon */
		//printf("\" />\n  <polygon fill=\"red\" stroke=\"blue\" stroke-width=\"0.003\" \n            points=\"");
		if (!atstart) fprintf(fp, "z M ");
		atstart = 1;
	      }
	    
	    if ((prevX-psShape->padfX[j])*(prevX-psShape->padfX[j])+(prevY-psShape->padfY[j])*(prevY-psShape->padfY[j])>minsqstep) {
	      //printf(" %.11g,%.11g", psShape->padfX[j], -psShape->padfY[j]);
	      fprintf(fp, "%5f %5f ", decimal_scaleout*(psShape->padfX[j]-MinBound[0]), decimal_scaleout*(-psShape->padfY[j]+MaxBound[1]));
	      //fprintf(fp, "%.0f %.0f ", decimal_scaleout*(psShape->padfX[j]-MinBound[0]), decimal_scaleout*(-psShape->padfY[j]+MaxBound[1]));
	      
	      atstart=0;
	      
	      prevX = psShape->padfX[j];
	      prevY = psShape->padfY[j];
	    }
	  }
	
	fprintf(fp, "z\" />\n");
	
	sprintf(label, "%s", DBFReadStringAttribute(hDBF,i,1));

	plain_characters(label);
    
	//fprintf(fp, "<text x=\"%.8g\" y=\"%.8g\" font-family=\"Verdana\" font-size=\"0.3\" fill=\"blue\" > %s </text>\n", 0.5*(psShape->dfXMin+psShape->dfXMax), -0.5*(psShape->dfYMin+psShape->dfYMax), label);
      }
      SHPDestroyObject( psShape );
    } 
  
  //fprintf(stderr, "%d %f %f\n", s, submaplegendpos[s][0], submaplegendpos[s][1]);
  
  fprintf(fp, "\t<rect id=\"bg\" x=\"%f\" y=\"%f\" width=\"100%%\" height=\"100%%\" fill=\"rgb(75,150,175)\"/>\n", 0.0, 0.0);

  //distancebar
  dbar_indx = 0;
  while (dbar_lengths[dbar_indx]<0.199*0.621371*length_km_of_width) {
    dbar_indx++;
  }
  dbar_indx--;
  distancebar_length = dbar_lengths[dbar_indx];

  fprintf(fp, "\t<g id =\"distancebar\">");
  for (i=0; i<5; i++) {
    if (i%2==0) {
      fprintf(fp, "\t<rect x=\"%f%%\" y=\"%f\" height=\"%f\" width=\"%f%%\" fill=\"rgb(0,0,0)\"/>\n", 80.0+i*20.0*distancebar_length/length_km_of_width, decimal_scaleout*(MaxBound[1]-MinBound[1]-0.037*(MaxBound[0]-MinBound[0])), 0.005*decimal_scaleout*(MaxBound[0]-MinBound[0]), 20.0*distancebar_length/length_km_of_width);
      fprintf(fp, "\t<rect x=\"%f%%\" y=\"%f\" height=\"%f\" width=\"%f%%\" fill=\"rgb(50,50,50)\"/>\n", 80.0+i*20.0/0.621371*distancebar_length/length_km_of_width, decimal_scaleout*(MaxBound[1]-MinBound[1]-0.032*(MaxBound[0]-MinBound[0])), 0.005*decimal_scaleout*(MaxBound[0]-MinBound[0]), 20.0/0.621371*distancebar_length/length_km_of_width);
    } else {
      fprintf(fp, "\t<rect x=\"%f%%\" y=\"%f\" height=\"%f\" width=\"%f%%\" fill=\"rgb(255,255,255)\"/>\n", 80.0+i*20.0*distancebar_length/length_km_of_width, decimal_scaleout*(MaxBound[1]-MinBound[1]-0.037*(MaxBound[0]-MinBound[0])), 0.005*decimal_scaleout*(MaxBound[0]-MinBound[0]), 20.0*distancebar_length/length_km_of_width);
      fprintf(fp, "\t<rect x=\"%f%%\" y=\"%f\" height=\"%f\" width=\"%f%%\" fill=\"rgb(255,255,255)\"/>\n", 80.0+i*20.0/0.621371*distancebar_length/length_km_of_width, decimal_scaleout*(MaxBound[1]-MinBound[1]-0.032*(MaxBound[0]-MinBound[0])), 0.005*decimal_scaleout*(MaxBound[0]-MinBound[0]), 20.0/0.621371*distancebar_length/length_km_of_width);
    }
  }
  fprintf(fp, "\t<rect x=\"%f%%\" y=\"%f\" height=\"%f\" width=\"%f%%\" fill=\"none\" stroke-width=\"0.1%%\" stroke=\"rgb(50,50,50)\"/>\n", 80.0, decimal_scaleout*(MaxBound[1]-MinBound[1]-0.032*(MaxBound[0]-MinBound[0])), 0.005*decimal_scaleout*(MaxBound[0]-MinBound[0]), 100.0/0.621371*distancebar_length/length_km_of_width);
  fprintf(fp, "\t<rect x=\"%f%%\" y=\"%f\" height=\"%f\" width=\"%f%%\" fill=\"none\" stroke-width=\"0.1%%\" stroke=\"rgb(0,0,0)\"/>\n", 80.0, decimal_scaleout*(MaxBound[1]-MinBound[1]-0.037*(MaxBound[0]-MinBound[0])), 0.005*decimal_scaleout*(MaxBound[0]-MinBound[0]), 100.0*distancebar_length/length_km_of_width);
  fprintf(fp, "\t</g>");

  fprintf(fp, "\t<rect id=\"undefinedbar\" x=\"%f\" y=\"%f\" width=\"%f\" height=\"%f\" stroke=\"rgb(0,0,0)\" stroke-width=\"0.1%%\"/>\n", submaplegendpos[s][0]*decimal_scaleout*(MaxBound[0]-MinBound[0]), 1.00*decimal_scaleout*(MaxBound[1]-MinBound[1])-(submaplegendpos[s][1]-0.27)*decimal_scaleout*(MaxBound[0]-MinBound[0]), 0.03*decimal_scaleout*(MaxBound[0]-MinBound[0]), 0.03*decimal_scaleout*(MaxBound[0]-MinBound[0]));
  fprintf(fp, "\t<rect id=\"scalebar\" x=\"%f\" y=\"%f\" width=\"%f\" height=\"%f\" stroke=\"rgb(0,0,0)\" stroke-width=\"0.1%%\"/>\n", submaplegendpos[s][0]*decimal_scaleout*(MaxBound[0]-MinBound[0]), 1.00*decimal_scaleout*(-MinBound[1]+MaxBound[1])-(submaplegendpos[s][1]-0.03)*decimal_scaleout*(MaxBound[0]-MinBound[0]), 0.03*decimal_scaleout*(MaxBound[0]-MinBound[0]), 0.22*decimal_scaleout*(MaxBound[0]-MinBound[0]));
  fprintf(fp, "\t<g id=\"scalemarks\" stroke=\"rgb(0,0,0)\" stroke-width=\"0.1%%\">\n");
  fprintf(fp, "\t\t<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\"/>\n", submaplegendpos[s][0]*decimal_scaleout*(MaxBound[0]-MinBound[0]), 1.00*decimal_scaleout*(-MinBound[1]+MaxBound[1])-(submaplegendpos[s][1]-0.03)*decimal_scaleout*(MaxBound[0]-MinBound[0]), submaplegendpos[s][0]*decimal_scaleout*(MaxBound[0]-MinBound[0])+ 0.035*decimal_scaleout*(MaxBound[0]-MinBound[0]), 1.00*decimal_scaleout*(-MinBound[1]+MaxBound[1])-(submaplegendpos[s][1]-0.03)*decimal_scaleout*(MaxBound[0]-MinBound[0]));
  fprintf(fp, "\t\t<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\"/>\n", submaplegendpos[s][0]*decimal_scaleout*(MaxBound[0]-MinBound[0]), 1.00*decimal_scaleout*(-MinBound[1]+MaxBound[1])-(submaplegendpos[s][1]-0.25)*decimal_scaleout*(MaxBound[0]-MinBound[0]), submaplegendpos[s][0]*decimal_scaleout*(MaxBound[0]-MinBound[0])+ 0.035*decimal_scaleout*(MaxBound[0]-MinBound[0]), 1.00*decimal_scaleout*(-MinBound[1]+MaxBound[1])-(submaplegendpos[s][1]-0.25)*decimal_scaleout*(MaxBound[0]-MinBound[0]));
  fprintf(fp, "\t\t<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\"/>\n", submaplegendpos[s][0]*decimal_scaleout*(MaxBound[0]-MinBound[0]), 1.00*decimal_scaleout*(-MinBound[1]+MaxBound[1])-(submaplegendpos[s][1]-0.14)*decimal_scaleout*(MaxBound[0]-MinBound[0]), submaplegendpos[s][0]*decimal_scaleout*(MaxBound[0]-MinBound[0])+ 0.035*decimal_scaleout*(MaxBound[0]-MinBound[0]), 1.00*decimal_scaleout*(-MinBound[1]+MaxBound[1])-(submaplegendpos[s][1]-0.14)*decimal_scaleout*(MaxBound[0]-MinBound[0]));
  fprintf(fp, "\t</g>\n");

  fclose(sl_fp);
  fclose(fp);
}

int getsubmapbounds() {
  FILE *boundfp;
  int i;
  int j;

  boundfp = fopen("bounds.txt", "r");
  if (boundfp==NULL) return (0);

  fscanf(boundfp, "%d", &numsubmaps);
  printf("expecting %d submap bounds\n", numsubmaps);
  for (i=0; i<numsubmaps; i++) {
    fscanf(boundfp, "%d %lf %lf %lf %lf %lf %lf", &submapstate[i], &submapbounds[i][0], &submapbounds[i][1], &submapbounds[i][2], &submapbounds[i][3], &submaplegendpos[i][0], &submaplegendpos[i][1]);
    fgetc(boundfp);
    fgets(submapnames[i], 100, boundfp);
    for (j=0; j<strlen(submapnames[i])-1; j++) {
      if (submapnames[i][j]==' ') submapnames[i][j]='_';
    } 
    submapnames[i][strlen(submapnames[i])-1] ='\0';
    printf("submap %d: %d %f %f %f %f %f %f <%s>\n", i, submapstate[i], submapbounds[i][0], submapbounds[i][1], submapbounds[i][2], submapbounds[i][3], submaplegendpos[i][0], submaplegendpos[i][1], submapnames[i]);
    printf("done i=%d < numsubmaps=%d\n", i, numsubmaps);
  } 

  fclose(boundfp);
  return (numsubmaps);
}


int main( int argc, char ** argv )

{
    int		nShapeType;
    char        file_name_shp[MAXFILENAME];
    char        file_name_dbf[MAXFILENAME];
    int         i;

    if( argc != 2 )
      {
        printf( "shpdump shp_file_stem\n" );
        exit( 1 );
      }
    
    sprintf(file_name_shp, "%s.svg", argv[1]);
    sprintf(file_name_dbf, "%s.dbf", argv[1]);
    
    hSHP = SHPOpen( file_name_shp, "rb" );
    hDBF = DBFOpen( file_name_dbf, "rb" );
    
    if( hSHP == NULL )
      {
        printf( "Unable to open:%s\n", file_name_shp );
        exit( 1 );
      }
    if( hDBF == NULL )
      {
	printf( "DBFOpen(%s,\"r\") failed.\n", file_name_dbf );
	exit( 2 );
      }
    
    SHPGetInfo( hSHP, &nEntities, &nShapeType, adfMinBound, adfMaxBound );

    printf("There are %d records with %d fields in the dbf table, and %d entities in the shp file \n", DBFGetRecordCount(hDBF), DBFGetFieldCount(hDBF), nEntities);
    
    if (DBFGetRecordCount(hDBF)!=nEntities) {
      fprintf(stderr, "DBF records and SHP entities don't match\n");
      exit(3);
    }

    if (getsubmapbounds()==0) {
      fprintf(stderr, "No bounds.txt file found.  Exporting full shapefile.\n");
    } else {
      printf("%d submap bounds found in bounds.txt.  Exporting individually.\n", numsubmaps);      
      for (i=0; i<numsubmaps; i++) {
	printf("%s\n", submapnames[i]);
      } 
    }

    for (i=0; i<numsubmaps; i++) {
      detailed_svg(argc, argv, i);
    }

    SHPClose( hSHP );
    
    exit( 0 );
}
